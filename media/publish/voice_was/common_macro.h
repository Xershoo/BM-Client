#ifndef _COMMON_MACRO_H_
#define _COMMON_MACRO_H_

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) if(x){x->Release();x=NULL;};
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) if(x){delete x;x=NULL;};
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(x) if(x){free(x);x=NULL;};
#endif

#ifndef SAFE_CLOSE_HANDLE
#define SAFE_CLOSE_HANDLE(x) if(x){::CloseHandle(x);x=NULL;};
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)  if(NULL != p) {delete []p; p=NULL; };
#endif

#define MAX_AUIDO_DATA_SIZE			(128*1024)

#define REFTIMES_PER_SEC			(10000000)
#define REFTIMES_PER_MILLISEC		(10000)

#define DEF_AUDIO_CHANNEL			(1)
#define DEF_AUDIO_SAMPLE_BIT		(16)
#define DEF_AUDIO_SAMPLE_RATE		(32000)

#define DMO_AEC_CHANNEL				(1)
#define DMO_AEC_SAMPERATE			(16000)
#define DMO_AEC_SAMPEBIT			(16)
#define DMO_AEC_ALIGN				((DMO_AEC_SAMPEBIT*DMO_AEC_CHANNEL)/8)
#define DMO_AEC_SAMPECAP			(DMO_AEC_SAMPERATE*DMO_AEC_ALIGN)
#define DMO_THREAD_LOOP_TIME		(20)

#define MAX_SAVE_AUDIO_PACKET		(3)

#endif