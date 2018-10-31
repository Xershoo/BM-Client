#ifndef _AUDIO_INCLUDE_H
#define _AUDIO_INCLUDE_H

#include <wmcodecdsp.h>
#include <mediaobj.h>
#include <mmreg.h>
#include <mmdeviceapi.h>
#include <EndPointVolume.h>
#include <Audioclient.h>
#include <audiopolicy.h>
#include <process.h>
#include <avrt.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <dmort.h>
#include <MMSystem.h>
#include <uuids.h>

#include "PolicyConfig.h"
#include "voice_was_interface.h"
#include "AutoLock.h"

extern "C"  
{
#include "libswresample/swresample.h"
#include "libavutil/channel_layout.h"
#include "libavutil/opt.h"
};

#pragma comment(lib,"Msdmo.lib")
#pragma comment(lib,"dmoguids.lib")
#pragma comment(lib,"strmiids.lib")
#pragma comment(lib,"wmcodecdspuuid.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"swresample.lib")
#pragma comment(lib,"Avrt.lib")
#pragma comment(lib,"Winmm.lib")

#endif