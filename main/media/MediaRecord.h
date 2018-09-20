#ifndef MEDIA_RECORD_H
#define MEDIA_RECORD_H

#include <QString>
#include <QtGUI/qwindowdefs_win.h>

class CMediaRecord
{
public:
	CMediaRecord();
	virtual ~CMediaRecord();

public:
	bool start(QString path,HWND hWnd);
	void stop();

	inline QString getPath()
	{
		return m_path;
	};

private:
	bool		m_record;
	QString		m_path;
};

#endif 