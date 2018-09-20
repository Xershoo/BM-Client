#ifndef PLAYAUDIO_H
#define PALYAUDIO_H
#include <Mmsystem.h>
#include "dsound.h"

class CPlayAudio
{
public:
	CPlayAudio();
	~CPlayAudio();

	bool StopSound();
	bool PlaySoundByDS(unsigned char* pBuf,unsigned int nSize,int nChannel, int nSample);
private:
	WAVEFORMATEX GetWaveFormat(WORD wFormatTag, int nChannel, int nSample, int nBits);
	bool PlaySoundByDSInit();
private:
	LPDIRECTSOUND		    m_pDirectSound;
	LPDIRECTSOUNDBUFFER		m_pDsBuffer;
	int						m_bufferlen;
	int						m_samplerate;
	int						m_nchannel;
	DWORD					m_writeOffset;
};

#endif