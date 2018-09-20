#pragma once

/*
*	��д��Ƶ�ļ��ӿڡ�caoc
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
	int nChannels;//1Ϊ��ͨ�� 2Ϊ˫ͨ��
	int samplerate;
	unsigned char * pDataBuf;
	unsigned int nBufLen;
	long stamp;//���Ϊ-1 ���ڲ���ȡʱ���
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

