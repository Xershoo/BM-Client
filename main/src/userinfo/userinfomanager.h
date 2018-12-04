//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�userinfomanager.h
//	�汾�ţ�1.0
//	���ߣ�������
//	ʱ�䣺2015.11.28
//	˵�����û�������Ϣ��
//	�޸ļ�¼��
//  ��ע��
//      1.������¼�û�����������Ϣ  �磺��ݼ�������ͷѡ��Ƥ���ȵ�
//      
//**********************************************************************

#pragma once
#include <QFile>
#include <QDomDocument>

class UserInfoManager
{
public:
    UserInfoManager(int userID);
    virtual ~UserInfoManager(void);

    QString getHotkeyValue(const QString &name);
    bool updateHotkey(const QString &name, const QString &value);

	bool isDeviceDetect();
	int  getSelSpeaker();
	int	 getSelCamera();
	int	 getSelMic();

	void getSpeakerList(QVector<QString>&);
	void getCameraList(QVector<QString>&);
	void getMicList(QVector<QString>&);

	void setDeviceDetect(bool isDetect);
	void updateSpeakerList(int selSpk,QVector<QString>& vecSpkName);
	void updateCameraList(int selCamera,QVector<QString>& vecCameraName);
	void updateMicList(int selMic,QVector<QString>& vecMicName);
private:
    QFile m_file;
    QDomDocument m_domDoc;
};

