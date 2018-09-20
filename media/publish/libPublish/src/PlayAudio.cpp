#include "stdafx.h"
#include "playaudio.h"

#define DX_BUFFER_FLAG  DSBCAPS_GLOBALFOCUS|DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_CTRLVOLUME


CPlayAudio::CPlayAudio()
{
	m_pDirectSound = NULL;
	m_pDsBuffer = NULL;
	m_bufferlen = 0;
	m_samplerate = 0;
	m_nchannel = 0;
	m_writeOffset = 0;
	PlaySoundByDSInit();
}

CPlayAudio::~CPlayAudio()
{
	if(m_pDsBuffer)
	{
		DWORD dwStatus = 0;
		m_pDsBuffer->GetStatus(&dwStatus);
		if((dwStatus & DSBSTATUS_PLAYING) == DSBSTATUS_PLAYING)
		{
			m_pDsBuffer->Stop();
			if (m_pDsBuffer)
			{
				m_pDsBuffer->Release();
				m_pDsBuffer = NULL;
			}
		}
	}
	if (m_pDirectSound) 
	{
		m_pDirectSound->Release();
		m_pDirectSound = NULL;
	}
}

WAVEFORMATEX CPlayAudio::GetWaveFormat(WORD wFormatTag, int nChannel, int nSample, int nBits)
{
	WAVEFORMATEX wfx;
	wfx.wFormatTag		=   wFormatTag;   
	wfx.nChannels		=   nChannel;   
	wfx.nSamplesPerSec  =   nSample;   
	wfx.wBitsPerSample  =   nBits;   
	wfx.nAvgBytesPerSec =   nSample * nChannel * nBits/8;   
	wfx.nBlockAlign		=   nChannel * nBits/8;

	wfx.cbSize			=   0;   
	return wfx;
}


bool CPlayAudio::PlaySoundByDSInit()
{
	// Create IDirectSound using the primary sound device
	if (FAILED(DirectSoundCreate(NULL, &m_pDirectSound, NULL))||NULL == m_pDirectSound)
	{
			return false;
	}

	// Set DirectSound coop level
	if(FAILED(m_pDirectSound->SetCooperativeLevel(GetDesktopWindow(), DSSCL_PRIORITY)))
	{
		return false;
	}

	if (m_pDsBuffer)
	{
		m_pDsBuffer->Release();
		m_pDsBuffer = NULL;
	}
	
	return true;

}

bool CPlayAudio::PlaySoundByDS(unsigned char* pBuf,unsigned int nSize,int nChannel, int nSample)
{
	if(NULL == m_pDirectSound)
	{
		return false;
	}

	if(m_pDsBuffer == NULL || (nChannel != m_nchannel || nSample != m_samplerate))
	{
		if(m_pDsBuffer)
		{
			m_pDsBuffer->Release();
			m_pDsBuffer = NULL;
		}
		m_samplerate = nSample;
		m_nchannel = nChannel;
		WAVEFORMATEX wfx =	GetWaveFormat(WAVE_FORMAT_PCM, m_nchannel, m_samplerate, 16);

		m_bufferlen = wfx.nAvgBytesPerSec;

		DSBUFFERDESC dsbd;

		ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
		dsbd.dwSize        = sizeof(DSBUFFERDESC);
		dsbd.dwFlags       = DX_BUFFER_FLAG|DSBCAPS_PRIMARYBUFFER;
		dsbd.dwBufferBytes = m_bufferlen;
		dsbd.lpwfxFormat   = &wfx;

		HRESULT hr;
		if (FAILED( hr = m_pDirectSound->CreateSoundBuffer(&dsbd, &m_pDsBuffer, NULL)))
		{
			dsbd.dwFlags   = DX_BUFFER_FLAG|DSBCAPS_LOCSOFTWARE;
			if (FAILED( hr = m_pDirectSound->CreateSoundBuffer(&dsbd, &m_pDsBuffer, NULL) ))
				return false;			
		}
	}

	if (!m_pDsBuffer)
			return false;

	DWORD dwPlayPosition, dwWritePosition;
	m_pDsBuffer->GetCurrentPosition(&dwPlayPosition, &dwWritePosition);

	if(dwPlayPosition >= dwWritePosition)
	{
		int nnn = dwWritePosition;
	}

	int xx		= (dwWritePosition - dwPlayPosition + m_bufferlen) % m_bufferlen;
	int off		= (m_writeOffset - dwPlayPosition + m_bufferlen) % m_bufferlen;
	int woff	= (m_writeOffset - dwWritePosition + m_bufferlen) % m_bufferlen;
	if (woff > (m_bufferlen>>1))// 当播放的速度超过了输入数据的速度，
	{							 // m_writeOffset需要重新同步为当前播放位置.
		m_writeOffset = dwWritePosition;
	}

	HRESULT hr;
	VOID*   pDSLockedBuffer = NULL;
	VOID*   pDSLockedBuffer2 = NULL;
	DWORD   dwDSLockedBufferSize;
	DWORD   dwDSLockedBufferSize2;

	if(pBuf && nSize > 0)
	{
		// Lock the DirectSound buffer
		if (FAILED( hr = m_pDsBuffer->Lock(m_writeOffset, nSize,&pDSLockedBuffer, &dwDSLockedBufferSize,&pDSLockedBuffer2, &dwDSLockedBufferSize2, 0L) ))
		{
			return false;
		}
		else
		{
			memcpy(pDSLockedBuffer, pBuf, dwDSLockedBufferSize);
			if (pDSLockedBuffer2)
			{
				memcpy(pDSLockedBuffer2, pBuf + dwDSLockedBufferSize, dwDSLockedBufferSize2);
			}
			if(dwDSLockedBufferSize+dwDSLockedBufferSize2 != nSize)
			{
				int nn = nSize - (dwDSLockedBufferSize+dwDSLockedBufferSize2);
			}
			m_writeOffset += nSize;

			if (FAILED(hr = m_pDsBuffer->Unlock(pDSLockedBuffer, dwDSLockedBufferSize,pDSLockedBuffer2, dwDSLockedBufferSize2))) 
			{
				return false;
			}		
		
			m_writeOffset %= m_bufferlen;
		}

		DWORD dwStatus = 0;
		m_pDsBuffer->GetStatus(&dwStatus);
		if(!(dwStatus & DSBSTATUS_PLAYING) != 0)
		{
			if(FAILED(hr = m_pDsBuffer->SetCurrentPosition(m_writeOffset)))
			{
				return false;
			}
			if (FAILED(hr = m_pDsBuffer->Play(0, 0, DSBPLAY_LOOPING)))
			{
				return false;
			}
		}
	}
	return true;
}

bool CPlayAudio::StopSound()
{
	if(m_pDsBuffer)
	{
		DWORD dwStatus = 0;
		m_pDsBuffer->GetStatus(&dwStatus);
		if((dwStatus & DSBSTATUS_PLAYING) == DSBSTATUS_PLAYING)
		{
			m_pDsBuffer->Stop();
			if (m_pDsBuffer)
			{
				m_pDsBuffer->Release();
				m_pDsBuffer = NULL;
			}
		}
	}
	return true;
}