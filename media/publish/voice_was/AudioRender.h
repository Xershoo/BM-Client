#ifndef _AUDIO_RENDER_H
#define _AUDIO_RENDER_H

#include "AudioImplBase.h"

class CAudioRender : public CAudioImplBase
{
public:
	CAudioRender(IMMDevice* pDevice);
	virtual ~CAudioRender();

protected:
	void getResampleParam(int& in_sample_bit,int& in_channels ,int& in_sample_rate ,int& out_sample_bit,int& out_channels,int& out_sample_rate);

protected:
	virtual UINT doAuidoImpl();
};

#endif