#ifndef REGISTRYFILE_H
#define REGISTRYFILE_H

#include <configfile.h>

class RegistryFile : public ConfigFile
{
public:
    explicit RegistryFile(const QString &filePath);
    ~RegistryFile();

    bool init();

    bool setValue(const QString &valueName,const QVariant &defaultValue);
    bool getValue(const QString &valueName,QVariant &outValue)const;
    bool getAllValue(QMap<QString,QVariant> &outValues)const;

    bool createItem(const QString &ItemPath);

private:
    RegistryFile(const RegistryFile&){}
    RegistryFile& operator=(const RegistryFile&){return *this;}
};

#endif // REGISTRYFILE_H
