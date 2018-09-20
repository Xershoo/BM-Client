#include "xmlmanager.h"

CXMLFile::CXMLFile(const QString &filePath = QString()):m_doc(NULL),m_element(QDomElement())
{
    this->m_filePath = filePath;
}

CXMLFile::~CXMLFile()
{
    if(m_doc)
        delete m_doc;
}

QDomElement CXMLFile::firstChildElement(const QDomElement &element,const QString &tagName){
    return element.firstChildElement(tagName);
}

QDomElement CXMLFile::nextSiblingElement(const QDomElement &element,const QString &tagName){
    return element.nextSiblingElement(tagName);
}

bool CXMLFile::nextElement(pFun pfun,QDomElement &outElement,const QString &tagName){
    QDomElement element = pfun(outElement,tagName);
    if(element.isNull())
        return false;
    outElement = element;
    return true;
}

bool CXMLFile::init()
{
    m_doc = new QDomDocument;
    if(!m_doc)
        return false;
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }
    if (!m_doc->setContent(&file))
    {
        file.close();
        return false;
    }
    file.close();
    m_element = m_doc->documentElement();
    return true;
}

bool CXMLFile::firstChildElement()
{
    if(m_element.isNull())
    {
        return false;
    }
    if(nextElement(firstChildElement,m_element))
    {
        return true;
    }
    return false;
}

bool CXMLFile::firstChildElement(const QString &tagName)
{
    if(m_element.isNull())
    {
        return false;
    }
    if(nextElement(firstChildElement,m_element,tagName))
    {
        return true;
    }
    return false;
}

bool CXMLFile::nextSiblingElement()
{
    if(m_element.isNull())
    {
        return false;
    }
    if(nextElement(nextSiblingElement,m_element))
    {
        return true;
    }
    return false;
}

bool CXMLFile::nextSiblingElement(const QString &tagName)
{
    if(m_element.isNull())
    {
        return false;
    }
    if(nextElement(nextSiblingElement,m_element,tagName))
    {
        return true;
    }
    return false;
}

bool CXMLFile::getCurrentTagName(QString &outTagName)const
{
    if(m_element.isNull())
        return false;
    outTagName = m_element.tagName();
    return true;
}

bool CXMLFile::getCurrentValue(QVariant &outValue)const
{
    if(m_element.isNull())
        return false;
    outValue = m_element.text();
    return true;
}

bool CXMLFile::getCurrentValue(const QString &tagName,QVariant &outValue)const
{
    if(m_element.isNull() || (m_element.tagName() != tagName) )
        return false;
    outValue = m_element.text();
    return true;
}

QDomElement CXMLFile::getFirstChildElement()const
{
    if(m_doc)
        return m_doc->firstChildElement();
    return QDomElement();
}

QDomElement CXMLFile::getCurrentElement()const
{
    return this->m_element;
}

void CXMLFile::setCurrentElement(const QDomElement &element)
{
    this->m_element = element;
}

bool CXMLFile::getValue(const QString &tagPath,QVariant &outValue)const
{
    QStringList path = tagPath.split(SPACE_CHARACTER);
    QDomElement root = m_doc->documentElement();
    for(int i = 0;i < path.size();++i)
    {
        QDomElement child;
        if(!findElement(root,path[i],child))
        {
            return false;
        }
        root = child;
    }
    outValue = root.text();
    return true;
}

bool CXMLFile::findElement(const QDomElement &root,const QString &tagName,QDomElement &outElement)const{
    QDomElement element = root.firstChildElement();
    while(!element.isNull())
    {
        if(tagName == element.tagName())
        {
            outElement = element;
            return true;
        }
        element = element.nextSiblingElement();
    }
    return false;
}


CXMLFile::CXMLFile(const CXMLFile &)
{

}

CXMLFile& CXMLFile::operator=(const CXMLFile &)
{
    return *this;
}
