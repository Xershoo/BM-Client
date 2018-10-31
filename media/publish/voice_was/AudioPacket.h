#ifndef _AUDIO_PACKET_H
#define _AUDIO_PACKET_H

#include "common_macro.h"

typedef struct audio_packet
{
public:
	audio_packet()
	{
		memset(dataAudio,NULL,sizeof(dataAudio));
		sizeAudio = 0;
		timeStamp = 0;
	};

	int pushData(unsigned char* buf,int size)
	{
		if(NULL==buf|| 0>= size)
		{
			return 0;
		}

		int copy = 0;
		if((size + sizeAudio) > MAX_AUIDO_DATA_SIZE)
		{
			copy = MAX_AUIDO_DATA_SIZE - sizeAudio;
		}
		else
		{
			copy = size;
		}

		if(copy <= 0)
		{
			return 0;
		}

		memcpy(dataAudio+sizeAudio,buf,copy);
		sizeAudio += copy;

		return copy;
	};

	bool pushDataAll(unsigned char* buf,int size)
	{
		if(NULL==buf|| 0>= size)
		{
			return false;
		}

		if((size + sizeAudio) > MAX_AUIDO_DATA_SIZE)
		{
			return false;
		}


		memcpy(dataAudio+sizeAudio,buf,size);
		sizeAudio += size;

		return true;
	};

	int popData(unsigned char* buf, int size)
	{
		if(NULL==buf|| 0>= size)
		{
			return 0;
		}

		int copy = size > (int)sizeAudio ? (int)sizeAudio :size;
		memcpy(buf,dataAudio,copy);

		if(copy == sizeAudio)
		{
			memset(dataAudio,NULL,sizeof(dataAudio));
			sizeAudio = 0;
		}
		else
		{
			sizeAudio -= size;
			memmove(dataAudio,dataAudio+size,sizeAudio);
		}

		return copy;
	};

public:
	unsigned char dataAudio[MAX_AUIDO_DATA_SIZE];
	unsigned int  sizeAudio;
	ULONGLONG timeStamp;
}AUDIOPACKET,*LPAUDIOPACKET;

#endif