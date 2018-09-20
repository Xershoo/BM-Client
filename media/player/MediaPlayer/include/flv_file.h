#ifndef FLV_FILE_H
#define FLV_FILE_H

#include <stdio.h>

struct FlvMediaData
{
	int n_fps;
	int n_width;
	int n_height;
	int audiosamplerate;
    unsigned int video_data_rate;
    unsigned int audio_data_rate;
	bool bNeedDurationAndFileSize;
    FlvMediaData()
    {
        n_fps = n_width = n_height = audiosamplerate = 0;
        bNeedDurationAndFileSize = false;
        video_data_rate = 0;
        audio_data_rate = 0;
    }
};


class CFlvFile
{
public:
	CFlvFile();
	~CFlvFile();
public:
	bool openFlvFile(const char* szFlvFileName,bool bHasVideo = true,bool bHasAudio = true);
	bool closeFlvFile();

	bool writeMediaHeaderTag(FlvMediaData fmd);
	bool writeMediaDataTag(unsigned char* pbuf,unsigned int nbufsize,long stamp, bool bIsVideo);
	bool writeMediaDataTagToMem(unsigned char* pDest,unsigned int& nDestLen,unsigned char* pbuf,unsigned int nbufsize
		,long stamp, bool bIsVideo);
private:
	bool writeFlvHeader();
private:
	FILE * m_flv;
	bool m_bHasVideo ;
	bool m_bHasAudio ;
	unsigned int m_fileSizePos;
	unsigned int m_durationPos;
	unsigned char * m_databuf;
	unsigned int    m_databufsize;
	double       m_flvDuration;
	double       m_flvSize;
};

#endif