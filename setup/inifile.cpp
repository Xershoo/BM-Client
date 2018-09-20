#include "inifile.h"
#include <QFile>

IniFile::IniFile(const QString &filePath)
    :ConfigFile()
{
    this->m_filePath = filePath;
}

IniFile::~IniFile()
{

}

bool IniFile::init()
{
    if(QFile::exists(m_filePath))
        m_exists = true;
    else
        return false;
    m_settings = new QSettings(m_filePath, QSettings::IniFormat);
    return true;
}

bool IniFile::setValue(const QString &valueName,const QVariant &defaultValue = QVariant())
{
    if(!m_exists)
        return false;
    m_settings->setValue(valueName,defaultValue);
    return true;
}

bool IniFile::getAllValue(QMap<QString,QVariant> &outValues)const
{
    if(!m_exists)
        return false;
    foreach(QString str,m_settings->childKeys())
    {
        outValues.insert(str,m_settings->value(str));
    }
    return true;
}

bool IniFile::getValue(const QString &valueName,QVariant &outValue)const
{
    if(!m_exists)
        return false;
    foreach(QString str,m_settings->childKeys())
    {
        if(valueName == str)
        {
            outValue = m_settings->value(str);
            return true;
        }
    }
    return false;
}

bool IniFile::createItem(const QString &ItemPath){
    QFile file(ItemPath);
    if(file.exists())
        return false;
    file.open(QIODevice::WriteOnly);
    file.close();
    this->m_filePath = ItemPath;
    return true;
}
