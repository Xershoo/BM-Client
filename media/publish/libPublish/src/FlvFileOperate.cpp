#include "stdafx.h"
#include "FlvFileOperate.h"
#include "TimeUtils.h"
#include <string.h>

#define NALU_TYPE_SLICE 1
#define NALU_TYPE_DPA 2
#define NALU_TYPE_DPB 3
#define NALU_TYPE_DPC 4
#define NALU_TYPE_IDR 5
#define NALU_TYPE_SEI 6          
#define NALU_TYPE_SPS 7
#define NALU_TYPE_PPS 8
#define NALU_TYPE_AUD 9
#define NALU_TYPE_EOSEQ 10
#define NALU_TYPE_EOSTREAM 11
#define NALU_TYPE_FILL 12

static int  audio_specific_config (unsigned char objectType,int samplerate, int channels, unsigned char *p)
{
	int rates [] = { 96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350, 0 };
	unsigned char count = 0;
	for (count = 0; rates[count] != samplerate; count++)
	{
		if (rates [count] == 0)
			return 0;
	}
	p[0] = ((objectType << 3) | (count >> 1));
	p[1] = (((count) & 0x01) << 7) | (channels << 3);
	return 2;
}

#define putIntToThreeChar(szThree,data) flv_put_num_to_buf(szThree,(const char*)&data,3)
#define putDoubleToEightChar(szEight,data) flv_put_num_to_buf(szEight,(const char*)&data,8)

#define Swap16(s) ((((s) & 0xff) << 8) | (((s) >> 8) & 0xff))

#define Swap32(l) (((l) >> 24) | \
(((l) & 0x00ff0000) >> 8)  | \
	(((l) & 0x0000ff00) << 8)  | \
	((l) << 24))

#define Swap64(ll) (((ll) >> 56) |\
	(((ll) & 0x00ff000000000000) >> 40) |\
	(((ll) & 0x0000ff0000000000) >> 24) |\
	(((ll) & 0x000000ff00000000) >> 8)    |\
	(((ll) & 0x00000000ff000000) << 8)    |\
	(((ll) & 0x0000000000ff0000) << 24) |\
	(((ll) & 0x000000000000ff00) << 40) |\
	(((ll) << 56)))

#define BigEndian_16(s) flv_big_endian_test() ? s : Swap16(s)
#define LittleEndian_16(s) flv_big_endian_test() ? Swap16(s) : s
#define BigEndian_32(l) flv_big_endian_test() ? l : Swap32(l)
#define LittleEndian_32(l) flv_big_endian_test() ? Swap32(l) : l
#define BigEndian_64(ll) flv_big_endian_test() ? ll : Swap64(ll)
#define LittleEndian_64(ll) flv_big_endian_test() ? Swap64(ll) : ll


static int CopyDataToNewMemWithSize(char ** dst,const unsigned char * src,int srcLength)
{
	*dst = new char[srcLength + sizeof(short_2)];
	short_2 sSize = srcLength;
	sSize = BigEndian_16(sSize);
	memcpy(* dst,&sSize,sizeof(short_2));
	memcpy(* dst + sizeof(short_2),src,srcLength);
	return  srcLength + sizeof(short_2);
}

//tag时间戳第四位是扩展位
//ffmpeg中是7f TagStamp[3] = v & 0x7f
#define FLVFILE_COPYSTAMP(stamp,TagStamp)\
{\
	long v = stamp;\
	TagStamp [2] = (unsigned char)(v & 0xFF);\
	v >>= 8;\
	TagStamp [1] = (unsigned char)(v & 0xFF);\
	v >>= 8;\
	TagStamp [0] = (unsigned char)(v & 0xFF);\
	v >>= 8;\
	TagStamp [3] = (unsigned char)(v & 0xFF);\
}

static int flv_mem_cp(void *dst,const void * src,int size)
{
	memcpy(dst,src,size);
	return size;
}

static int CreatMetaDataNodeCommon(const char * szName,char nameLength[2],string & name )
{
	int nSize = 0;
	short_2 nNameLenght = strlen(szName);
	nSize += nNameLenght;
	name = szName;

	nNameLenght = BigEndian_16(nNameLenght);
	memcpy(nameLength,&nNameLenght,sizeof(short_2));
	nSize += sizeof(short_2);
	return nSize;
}

static int CreateMetaDataNode(const char * szName,double data,SFlvAMFArrayNode &node)
{
	int nSize = CreatMetaDataNodeCommon(szName,node.nameLength,node.Name);

	node.type = 0x0;//double类型
	nSize += sizeof(node.type);

	node.data = data;
	nSize += sizeof(node.data);
	return nSize;
}

static int CreateMetaDataNode(const char * szName,bool data,SFlvAMFBOOLArraryNode &node)
{
	int nSize = CreatMetaDataNodeCommon(szName,node.nameLength,node.Name);

	node.type = 0x1;//bool 类型
	nSize += sizeof(node.type);

	node.data = data ? 1 : 0;
	nSize += sizeof(node.data);
	return nSize;
}

static int WrtiteMetaDataNode(unsigned char * buf,int nLength,const SFlvAMFArrayNode &node)
{
	if(flv_right_bigger(nLength,sizeof(node.nameLength)))
		return 0;
	int n_ret_sie = 0;
	n_ret_sie = flv_mem_cp(buf,node.nameLength,sizeof(node.nameLength));
		
	if(flv_right_bigger(nLength,n_ret_sie + strlen(node.Name.c_str())))
		return 0;
	n_ret_sie += flv_mem_cp(buf+n_ret_sie,node.Name.c_str(),strlen(node.Name.c_str()));

	if(flv_right_bigger(nLength,n_ret_sie + sizeof(node.type)))
		return 0;
	n_ret_sie += flv_mem_cp(buf+n_ret_sie,&node.type,sizeof(node.type));

	if(flv_right_bigger(nLength,n_ret_sie + sizeof(double)))
		return 0;
	putDoubleToEightChar(buf+n_ret_sie,node.data);

	return n_ret_sie + sizeof(double);
}

static int WrtiteMetaDataNode(unsigned char * buf,int nLength,const SFlvAMFBOOLArraryNode &node)
{
	if(flv_right_bigger(nLength,sizeof(node.nameLength)))
		return 0;
	int n_ret_sie = 0;
	n_ret_sie = flv_mem_cp(buf,node.nameLength,sizeof(node.nameLength));

	if(flv_right_bigger(nLength,n_ret_sie + strlen(node.Name.c_str())))
		return 0;
	n_ret_sie += flv_mem_cp(buf+n_ret_sie,node.Name.c_str(),strlen(node.Name.c_str()));

	if(flv_right_bigger(nLength,n_ret_sie + sizeof(node.type)))
		return 0;
	n_ret_sie += flv_mem_cp(buf+n_ret_sie,&node.type,sizeof(node.type));

	if(flv_right_bigger(nLength,n_ret_sie + sizeof(node.data)))
		return 0;
    n_ret_sie += flv_mem_cp(buf+n_ret_sie,&node.data,sizeof(node.data));
	return n_ret_sie;
}

static int WriteSFlvAMFHeader(unsigned char * dst,int ndstlen,const SFlvAMFHeader & amfHeader)
{
	int nMetaDataSize = 0;
	if(flv_right_bigger(ndstlen,sizeof(amfHeader.amf1type)))
		return 0;

	nMetaDataSize += flv_mem_cp(dst,&amfHeader.amf1type,sizeof(amfHeader.amf1type));
	
	if(flv_right_bigger(ndstlen,sizeof(amfHeader.stringLength)+nMetaDataSize))
		return 0;
	short_2 stringLength = BigEndian_16(amfHeader.stringLength);
	nMetaDataSize += flv_mem_cp(dst+nMetaDataSize,&stringLength,sizeof(amfHeader.stringLength));

	if(flv_right_bigger(ndstlen,amfHeader.stringLength+nMetaDataSize))
		return 0;
	nMetaDataSize += flv_mem_cp(dst+nMetaDataSize,amfHeader.pData,amfHeader.stringLength);

	if(flv_right_bigger(ndstlen,sizeof(amfHeader.amf2type)+nMetaDataSize))
		return 0;
	nMetaDataSize += flv_mem_cp(dst+nMetaDataSize,&amfHeader.amf2type,sizeof(amfHeader.amf2type));

	if(flv_right_bigger(ndstlen,sizeof(amfHeader.arraySize)+nMetaDataSize))
		return 0;
	int_4 arraySize = BigEndian_32(amfHeader.arraySize);
	nMetaDataSize += flv_mem_cp(dst+nMetaDataSize,&arraySize,sizeof(amfHeader.arraySize));

	return nMetaDataSize;
}

static int write_flv_header_mem(unsigned char * szHeader,int nLength,bool bHasAudio)
{
	if(flv_right_bigger(nLength, sizeof(SFlvHeader)))
		return 0;

	SFlvHeader *header = (SFlvHeader*)szHeader;
	memcpy(header->szFLV,"FLV",3);
	header->szVersion = 0x01;
	//写流信息
	if(bHasAudio)
		header->streamInfo = 0x05;
	else
		header->streamInfo = 0x01;
	//Header长度
	header->nHeaderSize = sizeof(SFlvHeader);
	header->nHeaderSize = BigEndian_32(header->nHeaderSize);
	//memcpy(szHeader,header,sizeof(SFlvHeader));
	return sizeof (SFlvHeader);
}

static int  write_flv_metadata_mem(unsigned char * szMetadata,int nLength,const FlvNeedParam * pflv,AMFHeaderOutPos * pOut)
{
	SFlvTagWithPreSize tagWithHeager;
	int mem_flag = 0;
	int nDataLenghtPos = sizeof(tagWithHeager.nPreTagSize)+ sizeof(tagWithHeager.tagHeader.tagHeaderType);

	tagWithHeager.nPreTagSize = 0x0;
	tagWithHeager.tagHeader.tagHeaderType = 0x12;//脚本 记录视频信息

	memset(tagWithHeager.tagHeader.tagDataLength,0,sizeof(tagWithHeager.tagHeader.tagDataLength));
	memset(tagWithHeager.tagHeader.Timestamp,0,sizeof(tagWithHeager.tagHeader.Timestamp));
	memset(&tagWithHeager.tagHeader.StreamID,0,sizeof(tagWithHeager.tagHeader.StreamID));
	
	if(flv_right_bigger(nLength,sizeof(tagWithHeager)))
		return 0;
	mem_flag += flv_mem_cp(szMetadata,&tagWithHeager,sizeof(tagWithHeager));

	//tag data
	int last_size = mem_flag;
	int nMetaDataSize = 0;
	SFlvAMFHeader amfHeader;
	amfHeader.amf1type = 0x02;
	amfHeader.stringLength = 0x0a;
	amfHeader.pData = "onMetaData";
	amfHeader.amf2type = 0x08;	
	if(pflv->bNeedDurationAndFileSize)
		amfHeader.arraySize = 0xd;//这个根据下面几个SFlvAMFArrayNode决定
	else
		amfHeader.arraySize = 0xb;//这个根据下面几个SFlvAMFArrayNode决定

	mem_flag += WriteSFlvAMFHeader(szMetadata+mem_flag,nLength-mem_flag,amfHeader);
	
	SFlvAMFArrayNode node;

	SFlvAMFBOOLArraryNode nodeBool;
	CreateMetaDataNode("hasVideo",true,nodeBool);
	mem_flag += WrtiteMetaDataNode(szMetadata+mem_flag,nLength-mem_flag,nodeBool);

	CreateMetaDataNode("hasAudio",pflv->audiosamplerate != 0,nodeBool);
	mem_flag += WrtiteMetaDataNode(szMetadata+mem_flag,nLength-mem_flag,nodeBool);

	CreateMetaDataNode("hasMetadata",1,nodeBool);
	mem_flag += WrtiteMetaDataNode(szMetadata+mem_flag,nLength-mem_flag,nodeBool);

	CreateMetaDataNode("width",pflv->n_width,node);
	mem_flag += WrtiteMetaDataNode(szMetadata+mem_flag,nLength-mem_flag,node);
	
	CreateMetaDataNode("height",pflv->n_height,node);
	mem_flag += WrtiteMetaDataNode(szMetadata+mem_flag,nLength-mem_flag,node);
	
	CreateMetaDataNode("videodatarate",0,node);
	mem_flag += WrtiteMetaDataNode(szMetadata+mem_flag,nLength-mem_flag,node);
	
	CreateMetaDataNode("framerate",pflv->n_fps,node);
	mem_flag += WrtiteMetaDataNode(szMetadata+mem_flag,nLength-mem_flag,node);
	
	//7是h264编码 现在只支持h264的封装
	CreateMetaDataNode("videocodecid",0x7,node);
	mem_flag += WrtiteMetaDataNode(szMetadata+mem_flag,nLength-mem_flag,node);

	CreateMetaDataNode("audiocodecid",0xA,node);
	mem_flag += WrtiteMetaDataNode(szMetadata+mem_flag,nLength-mem_flag,node);

	CreateMetaDataNode("audiosamplerate ",pflv->audiosamplerate,node);
	mem_flag += WrtiteMetaDataNode(szMetadata+mem_flag,nLength-mem_flag,node);

	CreateMetaDataNode("audiosamplesize ",16,node);
	mem_flag += WrtiteMetaDataNode(szMetadata+mem_flag,nLength-mem_flag,node);

	if(pflv->bNeedDurationAndFileSize)
	{
		CreateMetaDataNode("duration",0.0,node);
		mem_flag += WrtiteMetaDataNode(szMetadata+mem_flag,nLength-mem_flag,node);
		if(pOut)	
		{
			pOut->n_duration_pos = mem_flag - sizeof(double);
		}
	}

	if(pflv->bNeedDurationAndFileSize)
	{
		CreateMetaDataNode("filesize",0,node);
		mem_flag += WrtiteMetaDataNode(szMetadata+mem_flag,nLength-mem_flag,node);
		if(pOut)	
		{
			pOut->n_filesize_pos = mem_flag - sizeof(double);
		}
	}

	nMetaDataSize = mem_flag - last_size;
	putIntToThreeChar(szMetadata+nDataLenghtPos,nMetaDataSize);

	tagWithHeager.nPreTagSize = nMetaDataSize + sizeof(SFlvTagHeader);
	tagWithHeager.nPreTagSize =BigEndian_32(tagWithHeager.nPreTagSize);
	if(flv_right_bigger(nLength-mem_flag,sizeof(tagWithHeager.nPreTagSize)))
		return 0;
	mem_flag += flv_mem_cp(szMetadata+mem_flag,&tagWithHeager.nPreTagSize,sizeof(tagWithHeager.nPreTagSize));
	return mem_flag;
}

static int WriteAAChdr(const SAudioFrame *frame,unsigned char *p,bool bIsFirstFrame)
{
	if(!bIsFirstFrame)
	{
		p[0] = 0xAF; // AAC audio, need these codes first
		p[1] = 0x01;
		return  2;
	}
	else
	{
		int c = audio_specific_config (frame->objectType,frame->samplerate,frame->nChannels,p+2);

		p[0] = 0xAF; // AAC audio, need these codes first
		p[1] = 0x0;
		return  2+c;
	}
}

static int  WriteVideoConfigToMem(unsigned char * pBuf,int nBufLength,const SFlvAVCConfigHeader & node)
{
	if(flv_right_bigger(nBufLength,sizeof(node.configurationVersion)))
		return 0;
	int nLenght = 0;
	nLenght += flv_mem_cp(pBuf,&node.configurationVersion,sizeof(node.configurationVersion));

	if(flv_right_bigger(nBufLength,nLenght + sizeof(node.AVCProfileIndication)))
		return 0;
	nLenght += flv_mem_cp(pBuf+nLenght,&node.AVCProfileIndication,sizeof(node.AVCProfileIndication));

	if(flv_right_bigger(nBufLength,nLenght + sizeof(node.profile_compatibility)))
		return 0;
	nLenght += flv_mem_cp(pBuf+nLenght,&node.profile_compatibility,sizeof(node.profile_compatibility));

	if(flv_right_bigger(nBufLength,nLenght + sizeof(node.AVCLevelIndication)))
		return 0;
	nLenght += flv_mem_cp(pBuf+nLenght,&node.AVCLevelIndication,sizeof(node.AVCLevelIndication));

	if(flv_right_bigger(nBufLength,nLenght + sizeof(node.lenghtSizeMinusOne)))
		return 0;
	nLenght += flv_mem_cp(pBuf+nLenght,&node.lenghtSizeMinusOne,sizeof(node.lenghtSizeMinusOne));

	if(flv_right_bigger(nBufLength,nLenght + sizeof(node.numOfSequenceParameterSets)))
		return 0;
	nLenght += flv_mem_cp(pBuf+nLenght,&node.numOfSequenceParameterSets,sizeof(node.numOfSequenceParameterSets));

	if(flv_right_bigger(nBufLength,nLenght + node.nspsLenght))
		return 0;
	nLenght += flv_mem_cp(pBuf+nLenght,node.sps,node.nspsLenght);

	if(flv_right_bigger(nBufLength,nLenght + sizeof(node.numOfPictureParameterSets)))
		return 0;
	nLenght += flv_mem_cp(pBuf+nLenght,&node.numOfPictureParameterSets,sizeof(node.numOfPictureParameterSets));

	if(node.nppsLenghth >= 0)
	{
		if(flv_right_bigger(nBufLength,nLenght + node.nppsLenghth))
			return 0;
		nLenght += flv_mem_cp(pBuf+nLenght,node.pps,node.nppsLenghth);
	}
	return nLenght;
}

static int  WriteVideoConfig(unsigned char * pBuf,int nBufLen,const unsigned char * sps,int nspsLength,const unsigned char * pps,int nppsLength)
{
	//写配置信息
	SFlvVideoTagHeader  AVCConfigHeader;
	AVCConfigHeader.Type = 0x17;
	AVCConfigHeader.AVCPacketType = 0x0;
	AVCConfigHeader.compositiontime[0] = 0x0;
	AVCConfigHeader.compositiontime[1] = 0x0;
	AVCConfigHeader.compositiontime[2] = 0x0;
	if(flv_right_bigger(nBufLen,sizeof(SFlvVideoTagHeader)))
		return 0;
	int nSize = flv_mem_cp(pBuf,&AVCConfigHeader,sizeof(SFlvVideoTagHeader));

	SFlvAVCConfigHeader AVCTagDataHeader;
	AVCTagDataHeader.configurationVersion = 0x1;
	AVCTagDataHeader.AVCProfileIndication = sps[1];
	AVCTagDataHeader.profile_compatibility = sps[2];
	AVCTagDataHeader.AVCLevelIndication = sps[3];
	AVCTagDataHeader.lenghtSizeMinusOne = 0xff;

	AVCTagDataHeader.numOfSequenceParameterSets = 0xE1;
	AVCTagDataHeader.nspsLenght = CopyDataToNewMemWithSize(&AVCTagDataHeader.sps,sps,nspsLength);

	if(nppsLength <= 0)
	{
		AVCTagDataHeader.numOfPictureParameterSets = 0;
		AVCTagDataHeader.nppsLenghth = 0;
		AVCTagDataHeader.pps = NULL;
	}
	else
	{
		AVCTagDataHeader.numOfPictureParameterSets = 0x1;
		AVCTagDataHeader.nppsLenghth = CopyDataToNewMemWithSize(&AVCTagDataHeader.pps,pps,nppsLength);
	}

	nSize += WriteVideoConfigToMem(pBuf+nSize,nBufLen-nSize,AVCTagDataHeader);
	delete []AVCTagDataHeader.sps;
	if (AVCTagDataHeader.pps)
	{
		delete []AVCTagDataHeader.pps;
	}
	return nSize;
}

static int  WriteFrameTagsHeader(unsigned char * buf,int nlen,FlvTagHeadertParam &out)
{
	if(flv_right_bigger(nlen,sizeof(SFlvTagHeader)))
		return 0;

    if(out.tag_type == -1)
        *buf = out.bVideo ? 0x09 : 0x08;
    else
        *buf = (unsigned char)out.tag_type;

	out.n_tag_size_pos = sizeof(char);
	buf += 4;
	FLVFILE_COPYSTAMP(out.stamp,buf)
	*(buf+4) = 0;
	*(buf+5) = 0;
	*(buf+6) = 0;
	return sizeof(SFlvTagHeader);
}

static int WriteLastTagSize(unsigned char * pbuf,int nlen,int_4 lastTagSize)
{
	int_4 nPreTagSize = lastTagSize;
	nPreTagSize = BigEndian_32(nPreTagSize);
	if(flv_right_bigger(nlen,sizeof(nPreTagSize)))
		return 0;
	return flv_mem_cp(pbuf,&nPreTagSize,sizeof(nPreTagSize));
}

static int   GetNalNum(const unsigned char * pDataBuf,unsigned int nDataLen)
{
	if(nDataLen < 3)
		return -1;
	bool bIs3Nal = pDataBuf[0] == 0x00 &&pDataBuf[1] == 0x00 &&  pDataBuf[2] == 0x01;
	if(nDataLen == 3)
		return bIs3Nal ? 3 : -1; 
	else if(bIs3Nal)
		return 3;
	else if(pDataBuf[0] == 0x00 &&pDataBuf[1] == 0x00 &&  pDataBuf[2] == 0x00 && pDataBuf[3] == 0x01)
		return 4;

	return -1;
}

static int WriteAudioFrameTagData(unsigned char * pdst,int ndst,const SAudioFrame * pFrame,bool bFirstAudio)
{
	if(flv_right_bigger(ndst,4+pFrame->nBufLen))
		return 0;
	int nTagDataSize = WriteAAChdr(pFrame,pdst,bFirstAudio);
	//write data
	if(!bFirstAudio)
		nTagDataSize += flv_mem_cp(pdst + nTagDataSize,pFrame->pDataBuf,pFrame->nBufLen);	
	return nTagDataSize;
}
static int flv_build_audio_frame(unsigned char *pdst,int ndstlen,const SAudioFrame * pFrame,bool bFirst)
{
	FlvTagHeadertParam out(false,pFrame->stamp);
	int nSize = WriteFrameTagsHeader(pdst,ndstlen,out);

	//写tag数据
	int nTagDataLenghth = WriteAudioFrameTagData(pdst+nSize,ndstlen-nSize,pFrame,bFirst);
	nSize += nTagDataLenghth;	
	nSize += WriteLastTagSize(pdst+nSize,ndstlen-nSize,sizeof(SFlvTagHeader) + nTagDataLenghth);
	putIntToThreeChar(pdst+out.n_tag_size_pos,nTagDataLenghth);
	return nSize;
}

static int  WriteNalu(unsigned char * pDstDataBuf,int ndstlen,const SVideoFrame* pFrame)
{
	if(flv_right_bigger(ndstlen,sizeof(SFlvVideoTagHeader)+ sizeof(int_4) + pFrame->nDataLen))
		return 0;

	if(pFrame->bIsKeyFrame)
		*pDstDataBuf = 0x17;
	else
		*pDstDataBuf = 0x27;
	
	*(pDstDataBuf+1) = 0x1;//nalu

	*(pDstDataBuf+2) = 0x0;
	*(pDstDataBuf+3) = 0x0;
	*(pDstDataBuf+4) = 0x0;//暂时没有发现有什么用

	int nWritenSize = 5;
	int_4 nNaluSize = BigEndian_32(pFrame->nDataLen);

	nWritenSize += flv_mem_cp(pDstDataBuf+nWritenSize,&nNaluSize,sizeof(nNaluSize));

	nWritenSize += flv_mem_cp(pDstDataBuf+nWritenSize,pFrame->pDataBuf,pFrame->nDataLen);

	return nWritenSize;
}



int   cc_flv_build_header(unsigned char *pdst,int ndstlen,
		const SBuildFileHeader* pData,
		OUT AMFHeaderOutPos * out)
{
	int nsize = write_flv_header_mem(pdst,ndstlen,pData->audio != NULL);
    int nlastsize = nsize;	
	FlvNeedParam flv;
	flv.audiosamplerate = pData->audio->samplerate;
	flv.n_fps = pData->fps;
	flv.n_width = pData->width;
	flv.n_height = pData->height;
	flv.bNeedDurationAndFileSize = out != NULL ? true : false;
	nsize += write_flv_metadata_mem(pdst + nsize,ndstlen-nsize,&flv,out);
	if(out != NULL)
	{
		out->n_duration_pos += nlastsize;
		out->n_filesize_pos += nlastsize;
	}
	if(pData->sps && pData->pps)
	{
		FlvTagHeadertParam param(true,0);
	    int n_last_size = nsize;	
		nsize += WriteFrameTagsHeader(pdst + nsize ,ndstlen-nsize,param);
		int nTagDataLenghth = WriteVideoConfig(pdst+nsize,ndstlen-nsize,
				pData->sps->pDataBuf,pData->sps->nDataLen,
				pData->pps->pDataBuf,pData->pps->nDataLen);
		nsize += nTagDataLenghth;
		nsize += WriteLastTagSize(pdst + nsize,ndstlen-nsize,sizeof(SFlvTagHeader) + nTagDataLenghth);
		putIntToThreeChar(pdst +n_last_size +  param.n_tag_size_pos,nTagDataLenghth);
	}
	if(pData->audio)
	{
		nsize += flv_build_audio_frame(pdst+nsize,ndstlen-nsize,pData->audio,true);
	}
	return nsize;
}

static int  write_nalu_header(unsigned char * pDstDataBuf,int ndstlen,int b_is_key_frame,int frame_size)
{
	if(flv_right_bigger(ndstlen,sizeof(SFlvVideoTagHeader)+ sizeof(int_4)))
		return 0;

	if(b_is_key_frame)
		*pDstDataBuf = 0x17;
	else
		*pDstDataBuf = 0x27;
	
	*(pDstDataBuf+1) = 0x1;//nalu

	*(pDstDataBuf+2) = 0x0;
	*(pDstDataBuf+3) = 0x0;
	*(pDstDataBuf+4) = 0x0;//暂时没有发现有什么用

	int nWritenSize = 5;
	int_4 nNaluSize = BigEndian_32(frame_size);

	nWritenSize += flv_mem_cp(pDstDataBuf+nWritenSize,&nNaluSize,sizeof(nNaluSize));
	
	return nWritenSize;
}
//return tag size
//pre_size = tag_size + frame_size 
int cc_flv_build_video_frame_header(unsigned char * pdst,int ndstlen,int b_is_key_frame,int stamp,int frame_size)
{
	FlvTagHeadertParam out(true,stamp);
	int nSize = WriteFrameTagsHeader(pdst,ndstlen,out);

	//写tag数据
	int nTagDataLenghth = write_nalu_header(pdst+nSize,
			ndstlen-nSize,
			b_is_key_frame,
			frame_size);
	nSize += nTagDataLenghth;
	nTagDataLenghth += frame_size;
	putIntToThreeChar(pdst+out.n_tag_size_pos,nTagDataLenghth);
	return nSize;
}

int cc_flv_build_audio_frame_header(unsigned char *pdst,int ndstlen,int stamp,int frame_size)
{
	FlvTagHeadertParam out(false,stamp);
	int nSize = WriteFrameTagsHeader(pdst,ndstlen,out);

	int nTagDataSize = WriteAAChdr(NULL,pdst+nSize,false);
	nSize += nTagDataSize;	
	nTagDataSize += frame_size;
	putIntToThreeChar(pdst+out.n_tag_size_pos,nTagDataSize);
	return nSize;
}
int cc_flv_build_video_frame(unsigned char *pdst,int ndstlen,const SVideoFrame* pFrame)
{
	FlvTagHeadertParam out(true,pFrame->stamp);
	int nSize = WriteFrameTagsHeader(pdst,ndstlen,out);

	//写tag数据
	int nTagDataLenghth = WriteNalu(pdst+nSize,
			ndstlen-nSize,
			pFrame);
	nSize += nTagDataLenghth;
	nSize += WriteLastTagSize(pdst+nSize,ndstlen-nSize,sizeof(SFlvTagHeader) + nTagDataLenghth);
	putIntToThreeChar(pdst+out.n_tag_size_pos,nTagDataLenghth);
	return nSize;
}

int cc_flv_build_audio_frame(unsigned char *pdst,int ndstlen,const SAudioFrame * pFrame)
{
	return flv_build_audio_frame(pdst,ndstlen,pFrame,false);
}

int cc_flv_build_my_frame_header(unsigned char * pdst,int ndstlen,int frame_size,unsigned char frame_type,int stamp)
{
    FlvTagHeadertParam out(false,stamp);
    out.tag_type = frame_type;
    int nSize = WriteFrameTagsHeader(pdst,ndstlen,out);

    int nTagDataSize = frame_size;
    putIntToThreeChar(pdst+out.n_tag_size_pos,nTagDataSize);
    return nSize;

}
