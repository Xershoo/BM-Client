#include "registryfile.h"
#include <QFile>
RegistryFile::RegistryFile(const QString &filePath)
    :ConfigFile()
{
    this->m_filePath = filePath;
}

RegistryFile::~RegistryFile(){

}

bool RegistryFile::init()
{
    m_settings = new QSettings(m_filePath, QSettings::NativeFormat);
    return true;
}

bool RegistryFile::setValue(const QString &valueName,const QVariant &defaultValue = QVariant())
{
    m_settings->setValue(valueName,defaultValue);
    return true;
}

bool RegistryFile::getAllValue(QMap<QString,QVariant> &outValues)const
{
    foreach(QString str,m_settings->childKeys())
    {
        outValues.insert(str,m_settings->value(str));
    }
    return true;
}

bool RegistryFile::getValue(const QString &valueName,QVariant &outValue)const
{
    foreach(QString str,m_settings->childKeys())
    {
        if(str == valueName)
        {
            outValue = m_settings->value(str);
            return true;
        }
    }
    return false;
}

bool RegistryFile::createItem(const QString &ItemPath){
    QSettings *tempSetting = new QSettings(ItemPath,QSettings::NativeFormat);
    delete tempSetting;
    return true;
}
