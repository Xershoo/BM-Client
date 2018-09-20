#ifndef _AVE_GLOBAL_H
#define _AVE_GLOBAL_H

#include "stdint.h"


// common defination
#ifndef MIN
#define MIN(a, b) ((a)<(b)?(a):(b))
#endif
#ifndef MAX
#define MAX(a, b) ((a)>(b)?(a):(b))
#endif
#define TICK_START()
#define TICK_END(n, name)																		

// testing
#define AVE_SAVING 0
#define AVE_LOG_ZL 0

// processing parameters
#define MAX_NUM              32
///////////////////////////Handsup detection
#define HP_DETECTION_NUM     32 // frames
#define HP_DETECTION_NUM_TH  76 // OF 128 ��˼32*(76/128)��֡��⵽��HP������֣���ֹ����
#define HP_SKINPIX_NUM_TH    56 // of 128 ��˼width*height*(56/128)��pixel�Ƿ�ɫ�������о��֣�

///////////////////////////Video Beautys
#define AVE_BEAUTY_SMOOTH_METHOD0     2
#define AVE_BEAUTY_LIGHTING_METHOD0   1
#define AVE_BEAUTY_SMOOTHING_LEVEL0   1 // 0-10
#define AVE_BEAUTY_BRIGHNESS_LEVEL0   0 // 0-10

#define AVE_BEAUTY_SMOOTH_METHOD1     2
#define AVE_BEAUTY_LIGHTING_METHOD1   1
#define AVE_BEAUTY_SMOOTHING_LEVEL1   3 // 0-10
#define AVE_BEAUTY_BRIGHNESS_LEVEL1   0 // 0-10

#define AVE_BEAUTY_SMOOTH_METHOD2     2
#define AVE_BEAUTY_LIGHTING_METHOD2   1
#define AVE_BEAUTY_SMOOTHING_LEVEL2   3 // 0-10 
#define AVE_BEAUTY_BRIGHNESS_LEVEL2   0 // 0-10

#define AVE_CONTRAST_BRIGHTNESS_VALUE0       3 // 
#define AVE_CONTRAST_CONTRAST_VALUE0         -20 // 0-
#define AVE_CONTRAST_BRIGHTNESS_VALUE1       5 // 
#define AVE_CONTRAST_CONTRAST_VALUE1         -15 // 0-
#define AVE_CONTRAST_BRIGHTNESS_VALUE2       10 // 
#define AVE_CONTRAST_CONTRAST_VALUE2         -10 //
#define AVE_CONTRAST_BRIGHTNESS_VALUE3       10 // 
#define AVE_CONTRAST_CONTRAST_VALUE3         0

typedef struct _sqstack
{
   int stack[MAX_NUM];
   int top;
}sqstack;

// states
typedef struct _AVE_STATE
{
	bool bHandsup;
	bool bMoving;
	bool bWindowOff;

	bool bVideoNetworksOn;
	bool bAudioNetworksOn;
	bool bScreenNetworksOn;

	bool bNetworksOn;
}AVE_STATE;

// Video Anlyzing
typedef struct _AVE_VIDEO_ANALYSIS{
	bool bDetected;
	sqstack *sStates;
}AVE_VIDEO_ANALYSIS;

//����Ϣ
typedef struct _AVE_POINT
{
	int nX;
	int nY;
}AVE_POINT;

typedef struct ave_video_frame
{
	//int struct_size;			// ���ṹ����ֽ�����= sizeof(xx_image_t)
	uint8_t * data[4];			// ��ͼ��ƽ��
	int stride[4];				// ��ͼ��ƽ����м��(�������е��ֽ���)
	int nWidth;					// ͼ���
	int nHeight;					// ͼ���
	int nChannel;					// ɫ�ʿռ�
	int nRotate;
} AVE_Video_Frame;

//  ��Ч������ 
typedef enum _AVE_FILTER_TYPE{ 
	R_FILTER_NONE         = 0,
	R_FILTER_MOSAIC       = 12,
	R_FILTER_STAGGERED    = 13,
	R_FILTER_FILTERGLASS  = 16,
	R_FILTER_FILTERBEAUTY = 17,
	R_FILTER_SKETCH		  = 19,
	R_FILTER_EDGING       = 22,
	R_FILTER_BGSWITCH     = 23,
	// paten recognition
	R_FILTER_HANDSUP      = 100,
	R_FILTER_MOVING       = 101,
	R_FILTER_OTHERS       = 102
}AVE_FILTER_TYPE;	

#endif //_AVE_GLOBAL_H