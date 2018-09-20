#pragma once
#ifndef XMLFILE_H
#define XMLFILE_H

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
};

#endif // XMLFILE_H

