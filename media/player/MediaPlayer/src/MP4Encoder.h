#ifndef MP4_ENCODER_H
#define MP4_ENCODER_H

#include "mp4v2/mp4v2.h"

struct MP4FileData
{
    uint16_t    type;       //0 video 1 audio
    char *      buf;
    uint32_t    size;    
    uint64_t    duration;
    bool        frame;
};

class CMP4Encoder
{
public:
    CMP4Encoder();
    virtual ~CMP4Encoder();

    enum
    {
          set_video = 0x1,
          set_audio = 0x2,
          set_spspps = 0x4,
    };

public:
    bool create(const char* pszFileName);
    void close();    
    bool writeData(const MP4FileData& fileData);
    
    bool WriteH264File(const char* pFile264,const char* pFileMp4);
public:
    inline char* getFileName()
    {
        return m_fileName;
    }

    inline bool isOpen()
    {
        return m_mp4File != NULL ? true : false;
    }

    void setAudio(uint32_t audioSample,uint32_t audioChannel)
    {
        m_setStatu |= set_audio;

        m_audioSample = audioSample;
        m_audioChannel = audioChannel;

        setupWriteData();
    };

    void setVideo(uint16_t videoWidth,uint16_t videoHeight,uint16_t videoFps)
    {
        m_setStatu |= set_video;

        m_videoWidth = videoWidth;
        m_videoHeight = videoHeight;
        m_videoFps = videoFps;

        setupWriteData();
    }

    void setSpsAndPps(uint8_t*  sps,uint32_t spsSize,uint8_t* pps,uint32_t ppsSize)
    {
        if(NULL==sps||NULL==pps||0>=spsSize||0>=ppsSize)
        {
            return;
        }

        m_setStatu |= set_spspps;

        m_spsSize = spsSize;
        m_ppsSize = ppsSize;
        memcpy(m_sps,sps,spsSize);
        memcpy(m_pps,pps,ppsSize);

        setupWriteData();
    }
protected:
    bool    setupWriteData();

private:
    MP4FileHandle   m_mp4File;
    char            m_fileName[512];

    MP4TrackId      m_idVideoTrack;
    uint64_t        m_lastVideoDuration;

    MP4TrackId      m_idAudioTrack;
    uint64_t        m_lastAudioDuration;

    uint16_t        m_videoWidth;
    uint16_t        m_videoHeight;
    uint16_t        m_videoFps;
    uint32_t        m_audioSample;
    uint32_t        m_audioChannel;
    uint8_t         m_sps[128];
    uint32_t        m_spsSize;
    uint8_t         m_pps[32];
    uint32_t        m_ppsSize;
    uint8_t         m_setStatu;
};

#endif