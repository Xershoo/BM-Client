#pragma once

/*
*	读写视频文件接口。caoc
*/
typedef struct _SVideoFrame
{
	unsigned char *pDataBuf;
	unsigned int nDataLen;
	bool bIsKeyFrame;
	long stamp;
}SVideoFrame;



typedef struct _SAudioFram
{
	int nChannels;//1为单通道 2为双通道
	int samplerate;
	unsigned char * pDataBuf;
	unsigned int nBufLen;
	long stamp;//如果为-1 则内部获取时间戳
	unsigned char objectType;//1 or 2
}SAudioFrame;


typedef struct _SBuildFileHeader
{
	SVideoFrame * sps;
	SVideoFrame * pps;
	SAudioFrame * audio;
	int 	      width;
	int 		  height; 
	int 		  fps;
}SBuildFileHeader;


typedef struct _AMFHeaderOutPos
{
	int n_duration_pos;
	int n_filesize_pos;
}AMFHeaderOutPos;

