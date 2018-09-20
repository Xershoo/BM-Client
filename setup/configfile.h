#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <QString>
#include <QMap>
#include <QSettings>

class ConfigFile
{

public:
    explicit ConfigFile();
    virtual ~ConfigFile();

    virtual bool init() = 0;

    virtual bool setValue(const QString &valueName,const QVariant &defaultValue) = 0;
    virtual bool getValue(const QString &valueName,QVariant &outValue)const = 0;
    virtual bool getAllValue(QMap<QString,QVariant> &outValues)const = 0;

    virtual bool createItem(const QString &ItemPath) = 0;

protected:

    QSettings *m_settings;
    QString m_filePath;
    bool m_exists;

};

#endif // CONFIGFILE_H
