#ifndef PLAYHEADER_H
#define PLAYHEADER_H

#include <Windows.h>
#include <stdint.h>

#define VIDEOTYPE 0x10000000
#define AUDIOTYPE 0x20000000


#define DRAWIAGER_SDL 1// sdl œ‘ æÕºœÒ
#define DRAWIAGER_DX  2// dxœ‘ æ

typedef struct HShowWindow
{
	int  nDrawToolType;
	int  left;
	int  top;
	int  right;
	int  bottom;
	HWND hwnd;
	HShowWindow()
	{
		nDrawToolType = DRAWIAGER_SDL;
		left = 0;
		top = 0;
		right = 0;
		bottom= 0;
		hwnd = NULL;
	}
}HShowWindow;

struct H264VideoFrame
{
	unsigned char * pFrame;
	unsigned int    nFrameSize;
	unsigned int    mW;
	unsigned int    nH;
	int64_t        pts;
	int            nSpan;
	H264VideoFrame()
	{
		pFrame = NULL;
		nFrameSize = 0;
		mW = 0;
		nH = 0;
		pts = 0;
		nSpan = 0;
	}
};

struct MEMNode
{
	unsigned char* buf;
	unsigned int   size;
	MEMNode()
	{
		buf = NULL;
		size = 0;
	}
};

struct AACAudioFrame
{
	unsigned char * pFrame;
	unsigned int    nFrameSize;
	int				nSamplerate;
	int             nChannel;
	int64_t        pts;
	int            nSpan;
	AACAudioFrame()
	{
		pFrame = NULL;
		nFrameSize = 0;
		nSamplerate = 0;
	    nChannel = 0;
		pts = 0;
		nSpan = 0;

	}
};



#endif