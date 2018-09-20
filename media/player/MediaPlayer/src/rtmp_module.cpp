#include "rtmp_module.h"
#include "rtmpprocess.h"
#pragma comment(lib, "zlib.lib")

RTMP_MODULE_API HMODULE_T module_new(void* hlibrary)
{
	if(!CRTMPProcess::createInstance())
	{
		return NULL;
	}

	module_t * pModule = new module_t;
	set_moduleattribute_string(pModule->name,"rtmp");
	set_moduleattribute_string(pModule->type,MD_IPPUT);
	set_moduleattribute_string(pModule->capability,"rtmp-url-net");
	set_moduleattribute_string(pModule->desc,"rtmp-network-protocol");
	set_moduleattribute_pointer(pModule->hmodule,hlibrary);
	set_moduleattribute_pointer(pModule->mdfunc,NULL);
	
	return pModule;
}

RTMP_MODULE_API long module_commond(moudle_commond_t& cmd,void* Param)
{
	if(CRTMPProcess::GetInstance())
		return (long)CRTMPProcess::GetInstance()->SetRtmpCommond(cmd,Param);
	return 0;
}

RTMP_MODULE_API long module_activate(char* szUrl,put_callback putcallback,get_callback getcallback)
{
	if(CRTMPProcess::GetInstance())
	{
		CRTMPProcess::GetInstance()->Activate(szUrl,putcallback,getcallback);
		return 1;
	}
	return 0;
}

RTMP_MODULE_API long module_destory(HMODULE_T pModule)
{
	if(pModule)
	{
		delete pModule;
		pModule = NULL;
	}
	return CRTMPProcess::destoryInstance();
}