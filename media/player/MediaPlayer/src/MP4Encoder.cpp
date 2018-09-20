#include "MP4Encoder.h"
#include <vector>

#define MP4_TIME_SCALE      (90000)

using namespace std;
typedef unsigned char byte;

bool is_nalu(byte * data, size_t size, int& type)
{
    if(size < 4)
    {
        return false;
    }

    if(data[0] == 0 && data[1] == 0)
    {
        if(data[2] == 1)
        {
            type = data[3] & 0x1F;
            return true;
        }
        
        if(data[2] == 0 && data[3] == 1)
        {
            if(size < 5)return false;
            type = data[4] & 0x1F;
            return true;
        }
    }
    return false;
}

void deal_sps(byte * data, size_t size, vector<byte>& sps)
{
    for(size_t i = 0; i < size; ++i)
    {
        int nalu_type = -1;
        if(is_nalu(&data[i], size - i, nalu_type))
        {
            if(nalu_type == 1 || nalu_type == 5 || nalu_type == 6) // slice, sei
            { 
                return;
            }
        }

        sps.push_back(data[i]);
    }
}

CMP4Encoder::CMP4Encoder():m_mp4File(NULL)
    ,m_idVideoTrack(0)
    ,m_lastVideoDuration(0)
    ,m_idAudioTrack(0)
    ,m_lastAudioDuration(0)
    ,m_spsSize(0)
    ,m_ppsSize(0)
    ,m_videoFps(0)
    ,m_setStatu(0)
    ,m_videoWidth(0)
    ,m_videoHeight(0)
    ,m_audioSample(0)
    ,m_audioChannel(0)
{
    memset(m_fileName,NULL,sizeof(m_fileName));
    memset(m_sps,NULL,sizeof(m_sps));
    memset(m_pps,NULL,sizeof(m_pps));
}
 
CMP4Encoder::~CMP4Encoder()
{
    close();
}

bool CMP4Encoder::create(const char* pszFileName)
{
    close();

    if (NULL == pszFileName || NULL == pszFileName[0])
    {
        return false;
    }

    m_mp4File = MP4CreateEx(pszFileName,0,1,1,0,0,0);
    if (MP4_INVALID_FILE_HANDLE == m_mp4File)  
    {   
        m_mp4File = NULL;
        return false;  
    }

    MP4SetTimeScale(m_mp4File,MP4_TIME_SCALE);
    MP4SetVideoProfileLevel(m_mp4File, 0x01);  
    MP4SetAudioProfileLevel(m_mp4File, 0x02); 

    strcpy_s(m_fileName,pszFileName);
    return true;
}

bool CMP4Encoder::setupWriteData()
{
    if (NULL == m_mp4File)
    {
        return false;
    }
    uint8_t avcProfileIndication = 0;
    uint8_t profile_compat = 0;
    uint8_t avcLevelIndication = 1;

    bool br = false;

    if ((m_idVideoTrack == 0) && 
        (m_setStatu&set_video) &&
        (m_setStatu&set_spspps))
    {
        if(NULL != m_sps && m_spsSize >= 4)
        {
            avcProfileIndication = m_sps[1];
            profile_compat = m_sps[2];
            avcLevelIndication = m_sps[3];
        }    

        m_idVideoTrack = MP4AddH264VideoTrack(m_mp4File,
            MP4_TIME_SCALE,
            MP4_TIME_SCALE/m_videoFps,
            m_videoWidth,
            m_videoHeight,
            avcProfileIndication,
            profile_compat,
            avcLevelIndication,3);

        if(MP4_INVALID_TRACK_ID == m_idVideoTrack)
        {
            m_idVideoTrack = 0;
            return false;
        }

        //设置sps和pps
        if(NULL!=m_sps )
        {
            MP4AddH264SequenceParameterSet(m_mp4File, m_idVideoTrack, m_sps, m_spsSize);
        }

        if(NULL!=m_pps)
        {
            MP4AddH264PictureParameterSet(m_mp4File, m_idVideoTrack, m_pps, m_ppsSize);
        }

        m_setStatu &= ~set_video;
        m_setStatu &= ~set_spspps;
    }

    if((m_setStatu&set_audio) && (m_idAudioTrack==0))
    {
        if (m_idAudioTrack == 0 )        
        {
            m_idAudioTrack = MP4AddAudioTrack(m_mp4File, m_audioSample, 1024, MP4_MPEG2_AAC_LC_AUDIO_TYPE);
            if(MP4_INVALID_TRACK_ID == m_idAudioTrack)
            {
                m_idAudioTrack = 0;
                return false;
            }

            unsigned char trackConfig[] = { 0x12,0x88};
            br = MP4SetTrackESConfiguration(m_mp4File,m_idAudioTrack,trackConfig,2);
            if(!br)
            {
                return false;
            }
        }

        m_setStatu &= ~set_audio;        
    }

    return true;
}

void CMP4Encoder::close()
{
    if(NULL==m_mp4File)
    {
        return;
    }

    MP4Close(m_mp4File);
    m_mp4File = NULL;
    
    m_idVideoTrack = 0;
    m_lastVideoDuration = 0;

    m_idAudioTrack = 0;    
    m_lastAudioDuration = 0;

    m_spsSize = 0;
    m_ppsSize = 0;

    m_videoWidth = 0;
    m_videoHeight = 0;
    m_videoFps = 0;
    m_audioSample = 0;
    m_audioChannel = 0;
    m_setStatu = 0;

    memset(m_sps,NULL,sizeof(m_sps));
    memset(m_pps,NULL,sizeof(m_pps));
    memset(m_fileName,NULL,sizeof(m_fileName));
}

bool CMP4Encoder::writeData(const MP4FileData& fileData)
{
    if (NULL == m_mp4File || NULL == fileData.buf || 0 >= fileData.size)
    {
        return false;
    }

    bool br = false;
    switch(fileData.type)
    {
    case 0:
        {
            if(m_idVideoTrack <= 0)
            {
                break;
            }

            // MP4 Nalu前四个字节表示Nalu长度
            fileData.buf[0] = (fileData.size-4)>>24;  
            fileData.buf[1] = (fileData.size-4)>>16;  
            fileData.buf[2] = (fileData.size-4)>>8;  
            fileData.buf[3] = (fileData.size-4)&0xff;

            uint64_t durVideo = -1;
            if(m_lastVideoDuration > 0)
            {
                durVideo = (fileData.duration - m_lastVideoDuration) * MP4_TIME_SCALE / 1000;        
            }

            m_lastVideoDuration = fileData.duration;

            br = MP4WriteSample(m_mp4File,m_idVideoTrack,(const uint8_t*)fileData.buf,fileData.size,durVideo,0,fileData.frame);
        }
        break;
    case 1:
        {
            if(m_idAudioTrack <= 0)
            {
                break;
            }

            uint64_t durVideo = -1;
            if(m_lastAudioDuration > 0)
            {
                durVideo = (fileData.duration - m_lastAudioDuration) * m_audioSample / 1000;        
            }

            m_lastAudioDuration = fileData.duration;

            br = MP4WriteSample(m_mp4File,m_idAudioTrack,(const uint8_t*)fileData.buf,fileData.size,durVideo,0,1);
        }
        break;
    default:
        break;
    }

    return br;
}

//////////////////////////////////////////////////////////////////////////
#define BUFFER_SIZE  (1024*1024)

// NALU单元  
typedef struct _MP4ENC_NaluUnit  
{  
    int type;  
    int size;  
    unsigned char *data;  
}MP4ENC_NaluUnit;

int ReadOneNaluFromBuf(const unsigned char *buffer,unsigned int nBufferSize,unsigned int offSet,MP4ENC_NaluUnit &nalu)  
{  
    int i = offSet;  
    while(i<nBufferSize)  
    {  
        if(buffer[i++] == 0x00 && buffer[i++] == 0x00 &&  buffer[i++] == 0x00 &&  buffer[i++] == 0x01 )  
        {  
            int pos = i;  
            
            while (pos<nBufferSize)  
            {  
                if(buffer[pos++] == 0x00 &&  buffer[pos++] == 0x00 &&  buffer[pos++] == 0x00 &&  buffer[pos++] == 0x01 )  
                {  
                    break;  
                }  
            } 

            if(pos == nBufferSize)  
            {  
                nalu.size = pos-i;    
            }  
            else  
            {  
                nalu.size = (pos-4)-i;  
            }  

            nalu.type = buffer[i]&0x1f;  
            nalu.data =(unsigned char*)&buffer[i];  
            
            return (nalu.size+i-offSet);  
        }  
    } 

    return 0;  
}

static MP4TrackId g_videoId = 0; 
int WriteH264Data(MP4FileHandle hMp4File,const unsigned char* pData,int size)  
{  
    if(hMp4File == NULL)  
    {  
        return -1;  
    }  
    
    if(pData == NULL)  
    {  
        return -1;  
    }

    MP4ENC_NaluUnit nalu;  
    int pos = 0, len = 0;

    while (len = ReadOneNaluFromBuf(pData,size,pos,nalu))  
    {  
        if(nalu.type == 0x07) // sps  
        {  
            // 添加h264 track      
            g_videoId = MP4AddH264VideoTrack  
                (hMp4File,   
                90000,   
                90000 / 25,   
                384,     // width  
                288,    // height  
                nalu.data[1], // sps[1] AVCProfileIndication  
                nalu.data[2], // sps[2] profile_compat  
                nalu.data[3], // sps[3] AVCLevelIndication  
                3);           // 4 bytes length before each NAL unit 

            if (g_videoId == MP4_INVALID_TRACK_ID)  
            {  
                printf("add video track failed.\n");  
                return 0;  
            }  
            
            MP4SetVideoProfileLevel(hMp4File, 1); //  Simple Profile @ Level 3  

            MP4AddH264SequenceParameterSet(hMp4File,g_videoId,nalu.data,nalu.size);  
        }  
        else if(nalu.type == 0x08) // pps  
        {  
            MP4AddH264PictureParameterSet(hMp4File,g_videoId,nalu.data,nalu.size);  
        }  
        else  
        {  
            int datalen = nalu.size+4;  
            unsigned char *data = new unsigned char[datalen];  
            // MP4 Nalu前四个字节表示Nalu长度  
            data[0] = nalu.size>>24;  
            data[1] = nalu.size>>16;  
            data[2] = nalu.size>>8;  
            data[3] = nalu.size&0xff;  
            memcpy(data+4,nalu.data,nalu.size);  
            if(!MP4WriteSample(hMp4File, g_videoId, data, datalen,MP4_INVALID_DURATION, 0, 1))  
            {  
                return 0;  
            }  
            delete[] data;  
        }  

        pos += len;  
    }

    return pos;  
}  



bool CMP4Encoder::WriteH264File(const char* pFile264,const char* pFileMp4)  
{  
    if(pFile264 == NULL || pFileMp4 == NULL)  
    {  
        return false;  
    }  

    bool br = create(pFileMp4);  

    if(m_mp4File == NULL)  
    {
        return false;  
    }  

    FILE *fp = NULL;
    fopen_s(&fp,pFile264, "rb");
    if(!fp)    
    {
        close();
        return false;  
    }

    fseek(fp, 0, SEEK_SET);  

    unsigned char *buffer  = new unsigned char[BUFFER_SIZE];  
    int pos = 0;  
    
    while(true)  
    {  
        int readlen = fread(buffer+pos, sizeof(unsigned char), BUFFER_SIZE-pos, fp);  
        if(readlen<=0)  
        {  
            break;  
        }  

        readlen += pos;  

        int writelen = 0;  
        for(int i = readlen-1; i>=0; i--)  
        {  
            if(buffer[i--] == 0x01 &&  
                buffer[i--] == 0x00 &&  
                buffer[i--] == 0x00 &&  
                buffer[i--] == 0x00  
                )  
            {  
                writelen = i+5;  
                break;  
            }  
        }  

        writelen = WriteH264Data(m_mp4File,buffer,writelen);  
        if(writelen<=0)  
        {  
            break;  
        }

        memcpy(buffer,buffer+writelen,readlen-writelen+1);  
        pos = readlen-writelen+1;  
    }  
    fclose(fp);  

    delete[] buffer;  
    close();  

    return true;  
} 
