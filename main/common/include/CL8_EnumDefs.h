//CL8_EnumDefs.h

#pragma once

#define  nWhiteBoardViewWBIdBegin  50

typedef enum _EnumLoginStyle
{
	UserLogStyle_None = 0,	//Ĭ�ϣ���ס�û���
	UserLogStyle_Bar,		//����ģʽ
	UserLogStyle_PW,		//��ס����
	UserLogStyle_Auto,		//�Զ���¼
}EnmuserLoginStyle;


typedef enum _LOGIN_STATUS
{
	LOGIN_SUCCESS,								// ��¼�ɹ�
	LOGIN_TIMEOUT,								// ��¼��ʱ
	LOGIN_KICKOFF,								// ���߳�
	LOGIN_LOGOUT,								// �����˳�
	LOGIN_RECONNECT,							// ��¼����
	LOGIN_ERROR,								// ��¼����
	LOGIN_ING,									// ���ڵ�¼
	LOGIN_NEEDREGISTER,							//��Ҫ�����˺�
	LOGIN_REGISTER_FAILED,						//�˺ż���ʧ��

	LOGIN_FRI_RETRY,							// ���µ�¼����
	LOGIN_FRI_OFFLINE,							// ���߷�ʽ��¼����
}LOGIN_STATUS;