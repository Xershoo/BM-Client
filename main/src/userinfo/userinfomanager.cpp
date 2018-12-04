//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：userinfomanager.cpp
//	版本号：1.0
//	作者：郭鹏程
//	时间：2015.11.28
//	说明：用户个人信息类
//	修改记录：
//**********************************************************************

#include "userinfomanager.h"
#include "Env.h"
#include <QTextStream>
#include <QDir>
#include <QVector>

UserInfoManager::UserInfoManager(int userID)
{
    QString filePath = Env::getExePath();
    filePath = QString("%1\\..\\..\\Users\\%2").arg(filePath).arg(userID);
    QDir dir(filePath);
    if (!dir.exists())
    {
        dir.mkpath(filePath);
    }
    filePath += "/usersetting.xml";
    m_file.setFileName(filePath);
    m_file.open(QFileDevice::ReadWrite);
    m_domDoc.setContent(&m_file);
	
	if(!m_domDoc.hasChildNodes()){
		QDomElement rootNode = m_domDoc.createElement("UserSetting");
		m_domDoc.appendChild(rootNode);
	}
}

UserInfoManager::~UserInfoManager(void)
{
    if (m_file.isOpen())
    {
        m_file.resize(0);
        QTextStream out(&m_file);
        m_domDoc.save(out, 4);
        m_file.close();
    }
}

QString UserInfoManager::getHotkeyValue(const QString &name)
{
    if (!m_file.isOpen())
    {
        return "";
    }

    QDomNodeList elementList = m_domDoc.elementsByTagName(name);
    if (elementList.count() > 0)
    {
        return elementList.at(0).toElement().text();
    }
    
    return "";
}

bool UserInfoManager::updateHotkey(const QString &name, const QString &value)
{
    if (!m_file.isOpen())
    {
        return false;
    }

	QDomNode rootNode = m_domDoc.firstChild();
	if(rootNode.isNull()){
		return false;
	}

    QDomNodeList list = rootNode.childNodes();
    bool findNode = false;
    QDomNode node;

    for (int i = 0; i != list.count(); i++)
    {
        node = list.at(i);
        if (node.nodeName() == "HotKeySetting")
        {
            findNode = true;
            break;
        }
    }

    if (!findNode)
    {
        node = m_domDoc.createElement("HotKeySetting");
        rootNode.appendChild(node);
    }

    findNode = false;

    QDomNode nodeKey;
    QDomNodeList elementList = m_domDoc.elementsByTagName(name);
    if (elementList.count() > 0)
    {
        findNode = true;
        nodeKey = elementList.at(0);
    }

    if (findNode)
    {
        QDomElement newValue = m_domDoc.createElement(name);
        newValue.appendChild(m_domDoc.createTextNode(value));
        node.replaceChild(newValue, nodeKey);
    }
    else
    {
        QDomElement element = m_domDoc.createElement(name);
        element.appendChild(m_domDoc.createTextNode(value));
        node.appendChild(element);
    }
    
    return true;
}

bool UserInfoManager::isDeviceDetect()
{
	if (!m_file.isOpen())
	{
		return false;
	}

	QDomNodeList elementList = m_domDoc.elementsByTagName(QString("DeviceDetect"));
	if (elementList.count() > 0)
	{
		int ret = elementList.at(0).toElement().text().toInt();
		return (ret == 1) ? true : false;
	}

	return false;
}

int UserInfoManager::getSelSpeaker()
{
	if (!m_file.isOpen())
	{
		return -1;
	}

	QDomNodeList elementList = m_domDoc.elementsByTagName(QString("Speaker"));
	if (elementList.count() <= 0)
	{
		return -1;
	}
	
	QDomNodeList spkNodeList = elementList.at(0).childNodes();

	for(int i=0;i<spkNodeList.count();i++){
		QDomNode nodeItem = spkNodeList.at(i);
		if(nodeItem.nodeName().compare(QString("SelectDevice")) == 0){
			return nodeItem.toElement().text().toInt();
		}
	}
	
	return -1;
}

int UserInfoManager::getSelCamera()
{
	if (!m_file.isOpen())
	{
		return -1;
	}

	QDomNodeList elementList = m_domDoc.elementsByTagName(QString("Camera"));
	if (elementList.count() <= 0)
	{
		return -1;
	}

	QDomNodeList spkNodeList = elementList.at(0).childNodes();

	for(int i=0;i<spkNodeList.count();i++){
		QDomNode nodeItem = spkNodeList.at(i);
		if(nodeItem.nodeName().compare(QString("SelectDevice")) == 0){
			return nodeItem.toElement().text().toInt();
		}
	}

	return -1;
}

int UserInfoManager::getSelMic()
{
	if (!m_file.isOpen())
	{
		return -1;
	}

	QDomNodeList elementList = m_domDoc.elementsByTagName(QString("Microphone"));
	if (elementList.count() <= 0)
	{
		return -1;
	}

	QDomNodeList spkNodeList = elementList.at(0).childNodes();

	for(int i=0;i<spkNodeList.count();i++){
		QDomNode nodeItem = spkNodeList.at(i);
		if(nodeItem.nodeName().compare(QString("SelectDevice")) == 0){
			return nodeItem.toElement().text().toInt();
		}
	}

	return -1;
}

void UserInfoManager::getSpeakerList(QVector<QString>& devNameList)
{	
	if (!m_file.isOpen())
	{
		return;
	}

	QDomNodeList elementList = m_domDoc.elementsByTagName(QString("Speaker"));
	if (elementList.count() <= 0)
	{
		return;
	}

	QDomNodeList spkNodeList = elementList.at(0).childNodes();

	for(int i=0;i<spkNodeList.count();i++){
		QDomNode nodeItem = spkNodeList.at(i);
		if(nodeItem.nodeName().compare(QString("Device")) == 0){
			devNameList.push_back(nodeItem.toElement().text());
		}
	}

	return;
}

void UserInfoManager::getCameraList(QVector<QString>& devNameList)
{
	if (!m_file.isOpen())
	{
		return ;
	}

	QDomNodeList elementList = m_domDoc.elementsByTagName(QString("Camera"));
	if (elementList.count() <= 0)
	{
		return ;
	}

	QDomNodeList spkNodeList = elementList.at(0).childNodes();

	for(int i=0;i<spkNodeList.count();i++){
		QDomNode nodeItem = spkNodeList.at(i);
		if(nodeItem.nodeName().compare(QString("Device")) == 0){
			devNameList.push_back(nodeItem.toElement().text());
		}
	}
	return ;
}

void UserInfoManager::getMicList(QVector<QString>& devNameList)
{
	if (!m_file.isOpen())
	{
		return;
	}

	QDomNodeList elementList = m_domDoc.elementsByTagName(QString("Microphone"));
	if (elementList.count() <= 0)
	{
		return;
	}

	QDomNodeList spkNodeList = elementList.at(0).childNodes();

	for(int i=0;i<spkNodeList.count();i++){
		QDomNode nodeItem = spkNodeList.at(i);
		if(nodeItem.nodeName().compare(QString("Device")) == 0){
			devNameList.push_back(nodeItem.toElement().text());
		}
	}
	return;
}

void UserInfoManager::updateSpeakerList(int selSpk,QVector<QString>& vecSpkName)
{
	if (!m_file.isOpen())
	{
		return;
	}

	QDomNode rootNode = m_domDoc.firstChild();
	if(rootNode.isNull()){
		return;
	}

	QDomNodeList nodeList=rootNode.childNodes();
	QDomNode devSettingNode;
	QDomNode newSpkNode;
	QDomNode oldSpkNode;
	QDomElement childSpkNode;
	int i=0;

	for(;i<nodeList.size();i++){
		devSettingNode = nodeList.at(i);
		if(devSettingNode.nodeName().compare(QString("DeviceSetting")) == 0){
			break;
		}
	}

	if(i>=nodeList.size()){
		devSettingNode = m_domDoc.createElement(QString("DeviceSetting"));
		devSettingNode = rootNode.appendChild(devSettingNode);
	}else{
		nodeList=devSettingNode.childNodes();

		for(i=0;i<nodeList.size();i++){
			QDomNode tempNode = nodeList.at(i);
			if(tempNode.nodeName().compare(QString("Speaker")) == 0){
				oldSpkNode = tempNode;
				break;
			}
		}
	}

	newSpkNode = m_domDoc.createElement(QString("Speaker"));
	
	childSpkNode = m_domDoc.createElement(QString("SelectDevice"));
	QString	qstrSelDevice = QString("%1").arg(selSpk);
	childSpkNode.appendChild(m_domDoc.createTextNode(qstrSelDevice));
	newSpkNode.appendChild(childSpkNode);

	for(i=0;i<vecSpkName.size();i++){
		childSpkNode = m_domDoc.createElement(QString("Device"));
		childSpkNode.appendChild(m_domDoc.createTextNode(vecSpkName.at(i)));
		newSpkNode.appendChild(childSpkNode);
	}

	if(oldSpkNode.isNull()){
		devSettingNode.appendChild(newSpkNode);
	}else{
		devSettingNode.replaceChild(newSpkNode,oldSpkNode);
	}
}

void UserInfoManager::updateCameraList(int selCamera,QVector<QString>& vecCameraName)
{
	if (!m_file.isOpen())
	{
		return;
	}

	QDomNode rootNode = m_domDoc.firstChild();
	if(rootNode.isNull()){
		return;
	}

	QDomNodeList nodeList=rootNode.childNodes();
	QDomNode devSettingNode;
	QDomNode newCameraNode;
	QDomNode oldCameraNode;
	QDomElement childCameraNode;
	int i=0;

	for(;i<nodeList.size();i++){
		devSettingNode = nodeList.at(i);
		if(devSettingNode.nodeName().compare(QString("DeviceSetting")) == 0){
			break;
		}
	}

	if(i>=nodeList.size()){
		devSettingNode = m_domDoc.createElement(QString("DeviceSetting"));
		devSettingNode = rootNode.appendChild(devSettingNode);
	}else{
		nodeList=devSettingNode.childNodes();

		for(i=0;i<nodeList.size();i++){
			QDomNode tempNode = nodeList.at(i);
			if(tempNode.nodeName().compare(QString("Camera")) == 0){
				oldCameraNode = tempNode;
				break;
			}
		}
	}

	newCameraNode = m_domDoc.createElement(QString("Camera"));

	childCameraNode = m_domDoc.createElement(QString("SelectDevice"));
	QString	qstrSelDevice = QString("%1").arg(selCamera);
	childCameraNode.appendChild(m_domDoc.createTextNode(qstrSelDevice));
	newCameraNode.appendChild(childCameraNode);

	for(i=0;i<vecCameraName.size();i++){
		childCameraNode = m_domDoc.createElement(QString("Device"));
		childCameraNode.appendChild(m_domDoc.createTextNode(vecCameraName.at(i)));
		newCameraNode.appendChild(childCameraNode);
	}

	if(oldCameraNode.isNull()){
		devSettingNode.appendChild(newCameraNode);
	}else{
		devSettingNode.replaceChild(newCameraNode,oldCameraNode);
	}
}

void UserInfoManager::updateMicList(int selMic,QVector<QString>& vecMicName)
{
	if (!m_file.isOpen())
	{
		return;
	}

	QDomNode rootNode = m_domDoc.firstChild();
	if(rootNode.isNull()){
		return;
	}

	QDomNodeList nodeList=rootNode.childNodes();
	QDomNode devSettingNode;
	QDomNode newMicNode;
	QDomNode oldMicNode;
	QDomElement childMicNode;
	int i=0;

	for(;i<nodeList.size();i++){
		devSettingNode = nodeList.at(i);
		if(devSettingNode.nodeName().compare(QString("DeviceSetting")) == 0){
			break;
		}
	}

	if(i>=nodeList.size()){
		devSettingNode = m_domDoc.createElement(QString("DeviceSetting"));
		devSettingNode = rootNode.appendChild(devSettingNode);
	}else{
		nodeList=devSettingNode.childNodes();

		for(i=0;i<nodeList.size();i++){
			QDomNode tempNode = nodeList.at(i);
			if(tempNode.nodeName().compare(QString("Microphone")) == 0){
				oldMicNode = tempNode;
				break;
			}
		}
	}

	newMicNode = m_domDoc.createElement(QString("Microphone"));

	childMicNode = m_domDoc.createElement(QString("SelectDevice"));
	QString	qstrSelDevice = QString("%1").arg(selMic);
	childMicNode.appendChild(m_domDoc.createTextNode(qstrSelDevice));
	newMicNode.appendChild(childMicNode);

	for(i=0;i<vecMicName.size();i++){
		childMicNode = m_domDoc.createElement(QString("Device"));
		childMicNode.appendChild(m_domDoc.createTextNode(vecMicName.at(i)));
		newMicNode.appendChild(childMicNode);
	}

	if(oldMicNode.isNull()){
		devSettingNode.appendChild(newMicNode);
	}else{
		devSettingNode.replaceChild(newMicNode,oldMicNode);
	}
}

void UserInfoManager::setDeviceDetect(bool isDetect)
{
	if (!m_file.isOpen())
	{
		return;
	}

	QDomNode rootNode = m_domDoc.firstChild();
	if(rootNode.isNull()){
		return;
	}

	QDomNodeList nodeList=rootNode.childNodes();
	QDomNode devSettingNode;
	QDomNode newNode;
	QDomNode oldNode;
	int i=0;

	for(;i<nodeList.size();i++){
		devSettingNode = nodeList.at(i);
		if(devSettingNode.nodeName().compare(QString("DeviceSetting")) == 0){
			break;
		}
	}

	if(i>=nodeList.size()){
		devSettingNode = m_domDoc.createElement(QString("DeviceSetting"));
		devSettingNode = rootNode.appendChild(devSettingNode);
	}else{
		nodeList=devSettingNode.childNodes();

		for(i=0;i<nodeList.size();i++){
			QDomNode tempNode = nodeList.at(i);
			if(tempNode.nodeName().compare(QString("DeviceDetect")) == 0){
				oldNode = tempNode;
				break;
			}
		}
	}

	newNode = m_domDoc.createElement(QString("DeviceDetect"));
	newNode.appendChild(m_domDoc.createTextNode(QString("%1").arg(isDetect?1:0)));

	if(oldNode.isNull()){
		devSettingNode.appendChild(newNode);
	}else{
		devSettingNode.replaceChild(newNode,oldNode);
	}
}
