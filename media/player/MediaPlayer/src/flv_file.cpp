#include "stdafx.h"
#include "flvFileOperate.h"
#include "flv_file.h"
#include <string.h>

#define put_int_to_three_char(szThree,data) flv_put_num_to_buf(szThree,(const char*)&data,3)
#define put_double_to_eight_char(szEight,data) flv_put_num_to_buf(szEight,(const char*)&data,8)

#define s_wap16(s) ((((s) & 0xff) << 8) | (((s) >> 8) & 0xff))

#define s_wap32(l) (((l) >> 24) | \
(((l) & 0x00ff0000) >> 8)  | \
	(((l) & 0x0000ff00) << 8)  | \
	((l) << 24))

#define s_wap64(ll) (((ll) >> 56) |\
	(((ll) & 0x00ff000000000000) >> 40) |\
	(((ll) & 0x0000ff0000000000) >> 24) |\
	(((ll) & 0x000000ff00000000) >> 8)    |\
	(((ll) & 0x00000000ff000000) << 8)    |\
	(((ll) & 0x0000000000ff0000) << 24) |\
	(((ll) & 0x000000000000ff00) << 40) |\
	(((ll) << 56)))

#define big_endian_16(s) flv_big_endian_test() ? s : s_wap16(s)
#define little_endian_16(s) flv_big_endian_test() ? s_wap16(s) : s
#define big_endian_32(l) flv_big_endian_test() ? l : s_wap32(l)
#define little_endian_32(l) flv_big_endian_test() ? s_wap32(l) : l
#define big_endian_64(ll) flv_big_endian_test() ? ll : s_wap64(ll)
#define little_endian_64(ll) flv_big_endian_test() ? s_wap64(ll) : ll

//tag时间戳第四位是扩展位
//ffmpeg中是7f TagStamp[3] = v & 0x7f
#define FLVFILECOPYSTMP(stamp,TagStamp)\
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

static int writeFLVAMFHeader(unsigned char * dst,int ndstlen,const SFlvAMFHeader & amfHeader)
{
	int nm_databufSize = 0;
	if(flv_right_bigger(ndstlen,sizeof(amfHeader.amf1type)))
		return 0;

	nm_databufSize += flv_mem_cp(dst,&amfHeader.amf1type,sizeof(amfHeader.amf1type));
	
	if(flv_right_bigger(ndstlen,sizeof(amfHeader.stringLength)+nm_databufSize))
		return 0;
	short_2 stringLength = big_endian_16(amfHeader.stringLength);
	nm_databufSize += flv_mem_cp(dst+nm_databufSize,&stringLength,sizeof(amfHeader.stringLength));

	if(flv_right_bigger(ndstlen,amfHeader.stringLength+nm_databufSize))
		return 0;
	nm_databufSize += flv_mem_cp(dst+nm_databufSize,amfHeader.pData,amfHeader.stringLength);

	if(flv_right_bigger(ndstlen,sizeof(amfHeader.amf2type)+nm_databufSize))
		return 0;
	nm_databufSize += flv_mem_cp(dst+nm_databufSize,&amfHeader.amf2type,sizeof(amfHeader.amf2type));

	if(flv_right_bigger(ndstlen,sizeof(amfHeader.arraySize)+nm_databufSize))
		return 0;
	int_4 arraySize = big_endian_32(amfHeader.arraySize);
	nm_databufSize += flv_mem_cp(dst+nm_databufSize,&arraySize,sizeof(amfHeader.arraySize));

	return nm_databufSize;
}

static int creatm_databufNodeCommon(const char * szName,char nameLength[2],string & name )
{
	int nSize = 0;
	short_2 nNameLenght = strlen(szName);
	nSize += nNameLenght;
	name = szName;

	nNameLenght = big_endian_16(nNameLenght);
	memcpy(nameLength,&nNameLenght,sizeof(short_2));
	nSize += sizeof(short_2);
	return nSize;
}

static int createm_databufNode(const char * szName,double data,SFlvAMFArrayNode &node)
{
	int nSize = creatm_databufNodeCommon(szName,node.nameLength,node.Name);

	node.type = 0x0;//double类型
	nSize += sizeof(node.type);

	node.data = data;
	nSize += sizeof(node.data);
	return nSize;
}

static int createm_databufNode(const char * szName,bool data,SFlvAMFBOOLArraryNode &node)
{
	int nSize = creatm_databufNodeCommon(szName,node.nameLength,node.Name);

	node.type = 0x1;//bool 类型
	nSize += sizeof(node.type);

	node.data = data ? 1 : 0;
	nSize += sizeof(node.data);
	return nSize;
}

static int wrtitem_databufNode(unsigned char * buf,int nLength,const SFlvAMFArrayNode &node)
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
	put_double_to_eight_char(buf+n_ret_sie,node.data);

	return n_ret_sie + sizeof(double);
}

static int wrtitem_databufNode(unsigned char * buf,int nLength,const SFlvAMFBOOLArraryNode &node)
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

CFlvFile::CFlvFile()
{
	m_bHasVideo = false ;
	m_bHasAudio  = false;
	m_flv = NULL;
	m_fileSizePos = 0;
	m_durationPos = 0;
	m_databuf = NULL;
	m_databufsize = 1024 * 1024;
	m_flvDuration = 0.0;
	m_flvSize = 0.0;
}

CFlvFile::~CFlvFile()
{
	closeFlvFile();
}

bool CFlvFile::openFlvFile(const char* szFlvFileName,bool bHasVideo,bool bHasAudio)
{
	m_bHasVideo = bHasVideo ;
	m_bHasAudio  = bHasAudio;;
	if(szFlvFileName)
	{
		m_flv =  fopen(szFlvFileName,"wb");
		if(m_flv)
		{
			m_databuf = new unsigned char[m_databufsize];
			if(m_databuf)
				return writeFlvHeader();
		}
	}
	return false;
}

bool CFlvFile::closeFlvFile()
{
	if(m_flv)
	{
		unsigned char szDouble[8];
		fseek(m_flv,m_durationPos,SEEK_SET);
		float fDur = m_flvDuration / 1000;
		put_double_to_eight_char(szDouble,fDur);
		fwrite(szDouble,1,sizeof(szDouble),m_flv);

		fseek(m_flv,m_fileSizePos,SEEK_SET);
		put_double_to_eight_char(szDouble,m_flvSize);
		fwrite(szDouble,1,sizeof(szDouble),m_flv);

		fclose(m_flv);
	}
	if(m_databuf)
	{
		delete m_databuf;
		m_databuf = NULL;
	}
	return true;
}

bool CFlvFile::writeFlvHeader()
{
	if(m_flv)
	{
		unsigned char flv_header[13];
		memset(flv_header,0,sizeof(flv_header));
		flv_header[0] = 0x46; //'F'
		flv_header[1] = 0x4c; //'L'
		flv_header[2] = 0x56; //'V'
		flv_header[3] = 0x1; //version 1
		if(m_bHasVideo && m_bHasAudio) //type
			flv_header[4] = 0x05;
		else 
			flv_header[4] = 0x01;
		flv_header[5] = 0x00;
		flv_header[6] = 0x00;
		flv_header[7] = 0x00;
		flv_header[8] = 0x09; /* header size */
		flv_header[9] = 0x00;
		flv_header[10] = 0x00;
		flv_header[11] = 0x00;
		flv_header[12] = 0x00; //tag0 size

		m_flvSize += sizeof(flv_header);
		if(fwrite(flv_header,1,sizeof(flv_header),m_flv) == sizeof(flv_header))
			return true;
	}
	return false;
}

bool CFlvFile::writeMediaHeaderTag(FlvMediaData fmd)
{
	if(m_flv  == NULL || m_databuf == NULL)
		return false;

	unsigned int medadatasize = 0;
	memset(m_databuf,0,1024);

	SFlvTagHeader tagHeader;

	int nDataLenghtPos = sizeof(tagHeader.tagHeaderType);

	tagHeader.tagHeaderType = 0x12;//脚本 记录视频信息
	memset(tagHeader.tagDataLength,0,sizeof(tagHeader.tagDataLength));
	memset(tagHeader.Timestamp,0,sizeof(tagHeader.Timestamp));
	memset(&tagHeader.StreamID,0,sizeof(tagHeader.StreamID));

	medadatasize += flv_mem_cp(m_databuf,&tagHeader,sizeof(tagHeader));

	int last_size = medadatasize;
	SFlvAMFHeader amfHeader;
	amfHeader.amf1type = 0x12;
	amfHeader.stringLength = 0x0a;
	amfHeader.pData = "onm_databuf";
	amfHeader.amf2type = 0x08;	
	if(fmd.bNeedDurationAndFileSize)
		amfHeader.arraySize = 0xd;//这个根据下面几个SFlvAMFArrayNode决定
	else
		amfHeader.arraySize = 0xb;//这个根据下面几个SFlvAMFArrayNode决定

	medadatasize += writeFLVAMFHeader(m_databuf+medadatasize,m_databufsize-medadatasize,amfHeader);

	SFlvAMFArrayNode node;

	if(fmd.bNeedDurationAndFileSize)
	{
		createm_databufNode("duration",0.0,node);
		medadatasize += wrtitem_databufNode(m_databuf+medadatasize,m_databufsize-medadatasize,node);
		m_durationPos = (medadatasize - sizeof(double))+13;
	}

	SFlvAMFBOOLArraryNode nodeBool;
	createm_databufNode("hasVideo",true,nodeBool);
	medadatasize += wrtitem_databufNode(m_databuf+medadatasize,m_databufsize-medadatasize,nodeBool);

	createm_databufNode("hasAudio",fmd.audiosamplerate != 0,nodeBool);
	medadatasize += wrtitem_databufNode(m_databuf+medadatasize,m_databufsize-medadatasize,nodeBool);

	createm_databufNode("hasMetadata",1,nodeBool);
	medadatasize += wrtitem_databufNode(m_databuf+medadatasize,m_databufsize-medadatasize,nodeBool);

	createm_databufNode("width",fmd.n_width,node);
	medadatasize += wrtitem_databufNode(m_databuf+medadatasize,m_databufsize-medadatasize,node);
	
	createm_databufNode("height",fmd.n_height,node);
	medadatasize += wrtitem_databufNode(m_databuf+medadatasize,m_databufsize-medadatasize,node);
	
	createm_databufNode("framerate",fmd.n_fps,node);
	medadatasize += wrtitem_databufNode(m_databuf+medadatasize,m_databufsize-medadatasize,node);

	createm_databufNode("videodatarate",fmd.video_data_rate,node);
	medadatasize += wrtitem_databufNode(m_databuf+medadatasize,m_databufsize-medadatasize,node);
	if(fmd.bNeedDurationAndFileSize)
	{
		createm_databufNode("filesize",0,node);
		medadatasize += wrtitem_databufNode(m_databuf+medadatasize,m_databufsize-medadatasize,node);
		m_fileSizePos = (medadatasize - sizeof(double))+13;
	}
	//7是h264编码 现在只支持h264的封装
	createm_databufNode("videocodecid",0x7,node);
	medadatasize += wrtitem_databufNode(m_databuf+medadatasize,m_databufsize-medadatasize,node);

	createm_databufNode("audiocodecid",0xA,node);
	medadatasize += wrtitem_databufNode(m_databuf+medadatasize,m_databufsize-medadatasize,node);

	createm_databufNode("audiosamplerate ",fmd.audiosamplerate,node);
	medadatasize += wrtitem_databufNode(m_databuf+medadatasize,m_databufsize-medadatasize,node);

	createm_databufNode("audiosamplesize ",16,node);
	medadatasize += wrtitem_databufNode(m_databuf+medadatasize,m_databufsize-medadatasize,node);

	int nm_databufSize = medadatasize - last_size;
	put_int_to_three_char(m_databuf+nDataLenghtPos,nm_databufSize);

	unsigned int nPreTagSize = nm_databufSize + sizeof(SFlvTagHeader);
	nPreTagSize =big_endian_32(nPreTagSize);
	if(flv_right_bigger(m_databufsize-medadatasize,sizeof(nPreTagSize)))
		return false;
	medadatasize += flv_mem_cp(m_databuf+medadatasize,&nPreTagSize,sizeof(nPreTagSize));

	m_flvSize += medadatasize;
	if(fwrite(m_databuf,1,medadatasize,m_flv) == medadatasize)
	{
		return true;
	}
	return false;
}

bool CFlvFile::writeMediaDataTagToMem(unsigned char* pDest,unsigned int& nDestLen,unsigned char* pbuf,unsigned int nbufsize
		,long stamp, bool bIsVideo)
{
	if(nbufsize > nDestLen || pDest == NULL || pbuf == NULL)
		return false;
	int nIndex = 0;
	if(!bIsVideo)
		pDest[nIndex++] = 0x08;
	else
		pDest[nIndex++] = 0x09;
	int n_tag_size_pos = nIndex;
	nIndex += 3;

	unsigned char * buf = pDest+nIndex;
	FLVFILECOPYSTMP(stamp,buf);
	nIndex += sizeof(int);
	pDest[nIndex++] = 0;
	pDest[nIndex++] = 0;
	pDest[nIndex++] = 0;
	
	int nBufLen = nDestLen - nIndex;
	if(flv_right_bigger(nBufLen,nbufsize))
		return false;
	buf = pDest + nIndex;
	int nSize = flv_mem_cp(buf,pbuf,nbufsize);
	nIndex += nSize;

	int_4 nPreTagSize = nIndex;
	nPreTagSize = big_endian_32(nPreTagSize);
	nBufLen = nDestLen - nIndex;

	if(flv_right_bigger(nBufLen,sizeof(nPreTagSize)))
		return false;

	buf = pDest + nIndex;
	nIndex += flv_mem_cp(buf,&nPreTagSize,sizeof(nPreTagSize));
	put_int_to_three_char(pDest+n_tag_size_pos,nbufsize);

	nDestLen = nIndex;
	return true;
}

bool CFlvFile::writeMediaDataTag(unsigned char* pbuf,unsigned int nbufsize,long stamp, bool bIsVideo)
{
	if(nbufsize > m_databufsize)
		return false;

	if(m_flv  == NULL || m_databuf == NULL)
		return false;

	memset(m_databuf,0,m_databufsize);
	int nIndex = 0;
	if(!bIsVideo)
		m_databuf[nIndex++] = 0x08;
	else
		m_databuf[nIndex++] = 0x09;
	int n_tag_size_pos = nIndex;
	nIndex += 3;

	unsigned char * buf = m_databuf+nIndex;
	FLVFILECOPYSTMP(stamp,buf);
	nIndex += sizeof(int);
	m_databuf[nIndex++] = 0;
	m_databuf[nIndex++] = 0;
	m_databuf[nIndex++] = 0;
	
	int nBufLen = m_databufsize - nIndex;
	if(flv_right_bigger(nBufLen,nbufsize))
		return 0;
	buf = m_databuf + nIndex;
	int nSize = flv_mem_cp(buf,pbuf,nbufsize);
	nIndex += nSize;

	int_4 nPreTagSize = nIndex;
	nPreTagSize = big_endian_32(nPreTagSize);
	nBufLen = m_databufsize - nIndex;

	if(flv_right_bigger(nBufLen,sizeof(nPreTagSize)))
		return 0;

	buf = m_databuf + nIndex;
	nIndex += flv_mem_cp(buf,&nPreTagSize,sizeof(nPreTagSize));
	put_int_to_three_char(m_databuf+n_tag_size_pos,nbufsize);

	m_flvSize += nIndex;
	m_flvDuration = stamp;
	if(fwrite(m_databuf,1,nIndex,m_flv) == nIndex)
		return true;
	return false;
}
