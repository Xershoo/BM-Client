#ifndef AUDIO_DEVICE_DETECT_H
#define AUDIO_DEVICE_DETECT_H

#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QAudioInput>
#include <QtCore/QFile>
#include <QtCore/QIODevice>
#include <QtCore/QMutex>
#include <QtMultimedia/QAudioDeviceInfo>

#define Q_AUDIO_BUF_MAX_SIZE	(1024*1024*4)
class QAudioBuffer: public QIODevice
{
	Q_OBJECT
public:
	QAudioBuffer();

public:
	virtual void close();

public:
	qint64 readData(char *data, qint64 maxlen);
	qint64 writeData(const char *data, qint64 len);
	int	   getLevel();

protected:
	int		m_sizeBuf;
	char	m_bufAudio[Q_AUDIO_BUF_MAX_SIZE];
	
	QMutex	m_mutexBuf;
	int		m_curLevel;
};

class QPCMFile: public QFile
{
	Q_OBJECT
public:
	QPCMFile();

public:
	qint64 readData(char *data, qint64 maxlen);
	int	   getLevel();
protected:
	int	   m_curLevel;
};

class AudioDeviceDetect : public QObject
{
	Q_OBJECT
public:
	AudioDeviceDetect(QObject* parent);
	~AudioDeviceDetect();

	void startDetectAudioOutDevice(QString& devName);
	void stopDetectAudioOutDevice();
	int  getAudioOutDeviceVolume();

	void startDetectAudioInDevice(QString& micName,QString& spkName);
	void stopDetectAudioInDevice();
	int  getAudioInDeviceVolume();

protected:
	void initOutAudioFormat();
	void initInAudioFormat();

	void openOutAudioDevice(QString& spkName,QAudioFormat& fmtAudio,QIODevice* deviceIo);
	void closeOutAudioDevice();

	virtual void timerEvent(QTimerEvent*);
protected:
	QPCMFile	*	m_fileAudio;
	QAudioOutput*	m_outAudio;
	QAudioFormat	m_fmtOutAudio;
	QList<QAudioDeviceInfo>* m_lstOutAudioDevice;	

	QAudioBuffer*	m_bufInAudio;
	QAudioInput*	m_inAudio;
	QList<QAudioDeviceInfo>* m_lstInAudioDevice;
	QAudioFormat	m_fmtInAudio;
	QString			m_outSelDevName;
	int				m_tidPlayback;
};

#endif