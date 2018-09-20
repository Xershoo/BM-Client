#ifndef INIFILE_H
#define INIFILE_H

#include <configfile.h>

class IniFile : public ConfigFile
{
public:
    explicit IniFile(const QString &filePath = "");
    ~IniFile();

    bool init();

    bool setValue(const QString &valueName,const QVariant &defaultValue);
    bool getValue(const QString &valueName,QVariant &outValue)const;
    bool getAllValue(QMap<QString,QVariant> &outValues)const;

    bool createItem(const QString &ItemPath);

private:
    IniFile(const IniFile&){}
    IniFile& operator=(const IniFile&){return *this;}
};

#endif // INIFILE_H
