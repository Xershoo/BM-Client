#include <QTime>
#include "MediaRecord.h"
#include "util.h"
#include "../media/publish/libPublish/include/PublishInterface.h"

CMediaRecord::CMediaRecord():m_record(false)
{

}

CMediaRecord::~CMediaRecord()
{
	stop();
}

bool CMediaRecord::start(QString path,HWND hWnd)
{
	if(m_record)
	{
		stop();
	}

	if(!path.isEmpty())
	{
		m_path = path;
	}
	
	QString qstrTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss");
	QString qstrFile = QString("%1\\%2.flv").arg(m_path).arg(qstrTime);
	std::string  strFile;
	Util::QStringToString(qstrFile,strFile);

	m_record = ::savePushDataToLocalFlvFileBegin(strFile.c_str(),NULL);

	return m_record;
}

void CMediaRecord::stop()
{
	if(!m_record)
	{
		return;
	}

	::savePushDataToLocalFlvFileEnd();
	m_record = false;
}