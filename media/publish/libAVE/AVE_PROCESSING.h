// AVE_PROCESSING.h : AVE_PROCESSING DLL ����ͷ�ļ�
//

#pragma once#include "resource.h"		// ������


// CAVE_PROCESSINGApp
// �йش���ʵ�ֵ���Ϣ������� AVE_PROCESSING.cpp
//

class CAVE_PROCESSINGApp : public CWinApp
{
public:
	CAVE_PROCESSINGApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
