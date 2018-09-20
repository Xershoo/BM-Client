// AVE_PROCESSING.h : AVE_PROCESSING DLL 的主头文件
//

#pragma once#include "resource.h"		// 主符号


// CAVE_PROCESSINGApp
// 有关此类实现的信息，请参阅 AVE_PROCESSING.cpp
//

class CAVE_PROCESSINGApp : public CWinApp
{
public:
	CAVE_PROCESSINGApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
