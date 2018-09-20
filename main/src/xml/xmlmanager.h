#ifndef XMLMANAGER_H
#define XMLMANAGER_H

#include <QString>
#include <QDomDocument>
#include <qfile.h>
#include <qvariant.h>

const QString SPACE_CHARACTER = "\\";

class CXMLFile
{
public:
    explicit CXMLFile(const QString &filePath);
    ~CXMLFile();

    bool init();

    bool getValue(const QString &tagPath,QVariant &outValue)const;
    bool getCurrentValue(QVariant &outValue)const;
    bool getCurrentValue(const QString &tagName,QVariant &outValue)const;
    bool getCurrentTagName(QString &outTagName)const;

    bool firstChildElement();
    bool firstChildElement(const QString &tagName);
    bool nextSiblingElement();
    bool nextSiblingElement(const QString &tagName);

    void setCurrentElement(const QDomElement &element);
    QDomElement getFirstChildElement()const;
    QDomElement getCurrentElement()const;

private:
    QDomDocument* m_doc;
    QDomElement m_element;
    QString m_filePath;

private:
    CXMLFile(const CXMLFile &);
    CXMLFile& operator=(const CXMLFile &);
    bool findElement(const QDomElement &root,const QString &tagName,QDomElement &outElement)const;

    static inline QDomElement firstChildElement(const QDomElement &element,const QString &tagName = QString());
    static inline QDomElement nextSiblingElement(const QDomElement &element,const QString &tagName = QString());
    typedef QDomElement(*pFun)(const QDomElement &,const QString &);
    bool nextElement(pFun pfun,QDomElement &outElement,const QString &tagName = QString());

};

#endif // XMLMANAGER_H
