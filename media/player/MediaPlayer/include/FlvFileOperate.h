#pragma once
//#include "VideoFileOperateBase.h"
//#include "VideoFileOperate.h"
//#include "sc_TimeUtils.h"
//#include "CalculationInterval.h"
#include <string>
using namespace std;

typedef struct _SVideoFrame
{
	unsigned char *pDataBuf;
	unsigned int nDataLen;
	BOOL bIsKeyFrame;
	long stamp;
}SVideoFrame;

enum _CC_Audio_Codec_Type
{
    CC_UNCOMPRESSED     = 16,
    CC_ADPCM            = 1,
    CC_MP3              = 2,
    CC_LINEAR_LE        = 3,
    CC_NELLY16          = 4,
    CC_NELLY8           = 5,
    CC_NELLY            = 6,
    CC_G711A            = 7,
    CC_G711U            = 8,
    CC_AAC              = 10,
    CC_SPEEX            = 11,
    CC_MP3_8            = 14,
    CC_DEVSPEC          = 15,
};

typedef enum _CC_Audio_Codec_Type CC_Audio_Codec_Type;

typedef struct _SAudioFram
{
	int nChannels;//1为单通道 2为双通道
	int samplerate;
	unsigned char * pDataBuf;
	unsigned int nBufLen;
	long stamp;//如果为-1 则内部获取时间戳
	unsigned char objectType;//1 or 2
    unsigned char SoundSize;/* 1=8bit, 2=16bit */
    CC_Audio_Codec_Type dataType;
    int           audio_config;/*0 config ,1 data*/
}SAudioFrame;


typedef int		int_4;
typedef short	short_2;

#pragma pack(1)
struct SFlvHeader
{
	char szFLV[3];
	char szVersion;
	char streamInfo;
	int_4 nHeaderSize;
};

struct SFlvTagHeader
{
	char  tagHeaderType;//音频为0x8 视频为0x9  脚本为0x12
	char  tagDataLength[3];//数据区长度
	unsigned char  Timestamp[4];//时间戳 最后一个字节为扩展时间戳
	char  StreamID[3];//Always 0
};

struct SFlvTagWithPreSize
{
	int_4 nPreTagSize; //上一个tag的大小
	SFlvTagHeader tagHeader;
};

struct SFlvAMFHeader
{
	char amf1type;//第一个amf的类型 一般是0x02.表示字符串
	short_2 stringLength;//一般是0x0a
	char * pData;//后面为数据 一般为"onMetaData"
	char amf2type;//第二份amf2的类型 一般是0x08 表示数组
	int_4 arraySize;//数组元素的个数

};
//double 类型
struct SFlvAMFArrayNode
{
	char nameLength[2];//数组元素名的长度
	string Name;//元素名
	char type;//类型 00
	double data;//数据
};

struct SFlvAMFBOOLArraryNode
{
	char nameLength[2];//数组元素名的长度
	string Name;//元素名
	char type;//类型 01
	char data;//数据
};

typedef struct _AMFHeaderOutPos
{
	int n_duration_pos;
	int n_filesize_pos;
}AMFHeaderOutPos;

struct SFlvVideoTagHeader
{
	/*
		//前四位为Frame Type 
		1 = key frame (for AVC, a seekable frame)
		2 = inter frame (for AVC, a non-seekable frame)
		3 = disposable inter frame (H.263 only)
		4 = generated key frame (reserved for server use only)
		5 = video info/command frame 
		后四位为CodecID
		2 = Sorenson H.263
		3 = Screen video
		4 = On2 VP6
		5 = On2 VP6 with alpha channel
		6 = Screen video version 2
		7 = AVC 
	*/
	char Type;//我们这里为0x17为关键字 27为一般帧
	/*
		0 = AVC sequence header sps
		1 = AVC NALU
		2 = AVC end of sequence (lower level NALU sequence ender is 
		not required or supported)
	*/
	char AVCPacketType;
	/*
		IF AVCPacketType == 1
			Composition time offset 
		ELSE
			0
	*/
	char compositiontime[3];
};

struct SH264_SPS_PPS
{
	unsigned char * sps;
	int n_sps_len;
	unsigned char * pps;
	int n_pps_len;
};

struct SFlvAVCConfigHeader
{
	//版本号 0x1
	char configurationVersion;

	//sps的第一个数据
	char AVCProfileIndication;

	//sps的第二个数据
	char profile_compatibility;

	//sps的第三个数据
	char AVCLevelIndication;

	//NALUnitLeght的长度 该值一般为ff
	//前6为保留 为111111
	char lenghtSizeMinusOne;

	//sps的个数
	//前3位保留 为111
	//后五位为sps的个数
	char numOfSequenceParameterSets;
	//sps_size + sps数据
	//sps_size为2个字节
	char * sps;
	int nspsLenght;

	//pps的个数
	char numOfPictureParameterSets;
	//pps_size + sps数据
	//pps_size为2个字节
	char * pps;
	int  nppsLenghth;
};

typedef struct _SBuildFileHeader
{
	SVideoFrame * sps;
	SVideoFrame * pps;
	SAudioFrame * audio;
    char        * meta_data;
    int           meta_data_size;
	int 	      width;
	int 		  height; 
	int 		  fps;
    unsigned int  video_data_rate;
    unsigned int  audio_data_rate;
    char        * aac_header;
    int           aac_header_size;
}SBuildFileHeader;
#pragma pack()

//FLV读

struct FlvNeedParam
{
	int n_fps;
	int n_width;
	int n_height;
	int audiosamplerate;
    unsigned int video_data_rate;
    unsigned int audio_data_rate;
	bool bNeedDurationAndFileSize;
    bool bHasAudio ;
    bool bHasVideo ;
    FlvNeedParam()
    {
        n_fps = n_width = n_height = audiosamplerate = 0;
        bNeedDurationAndFileSize = false;
        bHasAudio = bHasVideo = false;
        video_data_rate = 0;
        audio_data_rate = 0;
    }
};

struct FlvTagHeadertParam
{
    int  n_tag_size_pos;
    int  tag_type;
    long stamp;
    bool bVideo;
    FlvTagHeadertParam(bool bIsVideo,long s):
        n_tag_size_pos(0),
        tag_type(-1),
        stamp(s),
        bVideo(bIsVideo){
        }
};

inline bool flv_right_bigger(int left,int right)
{
	if(left < right)
	{
		perror("flv:dst size is smaller\n");
		return true;
	}
	return false;
}

#define FLV_BIGENDIAN 1  
#define FLV_LITTLEENDIAN 0  

inline bool flv_big_endian_test()  
{  
	const short n = 1;  
	if(*(char *)&n)  
	{  
		return FLV_LITTLEENDIAN;  
	}  
	return FLV_BIGENDIAN;  
} 

inline void flv_put_num_to_buf(OUT unsigned char szNum[],const char * psrc,int dstLenght)
{
	if(flv_big_endian_test())
	{
		for (int n = 0; n < dstLenght; ++n)
		{
			szNum[n] = psrc[n];
		}
	}
	else
	{
		for (int n = 0; n < dstLenght; ++n)
		{
			szNum[n] = psrc[dstLenght -1 - n];
		}
	}
}



int   cc_flv_build_header(unsigned char *pdst,int ndstlen,const SBuildFileHeader* pData,OUT AMFHeaderOutPos* out);

int   cc_flv_build_audio_frame(unsigned char *pdst,int ndstlen,const SAudioFrame * pFrame);

int   cc_flv_build_video_frame(unsigned char *pdst,int ndstlen,const SVideoFrame* pFrame);

int   cc_flv_build_video_frame_header(unsigned char * pdst,
		int ndstlen,
		int b_is_key_frame,
		int stamp,
		int frame_size);

 int   cc_flv_build_audio_frame_header(unsigned char *pdst,
		int ndstlen,
		int stamp,
		int frame_size);

int cc_flv_build_my_frame_header(unsigned char * pdst,
        int ndstlen,
        int frame_size,
        unsigned char frame_type,
        int stamp);

