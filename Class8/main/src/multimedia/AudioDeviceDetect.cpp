#include "AudioDeviceDetect.h"
#include "./common/macros.h"
#include "./common/Util.h"
#include <QtCore/QMutexLocker>
#include <QtCore/QTimerEvent>

#define VOLUME_MAX				(32767)
#define MAX_CALC_BUF_LEN		(8000)

static int ComputeLevel(short* pcmData, int sample)
{
	signed short ret = 0;
	if (sample > 0){
		int sum = 0;
		signed short* pos = (signed short *)pcmData;
		for (int i = 0; i < sample; i++){
			sum += abs(*pos);
			pos++;
		}
		ret = sum * 500.0 / (sample * VOLUME_MAX);
		if (ret >= 100){
			ret = 100;
		}
	}
	return ret;
}
//////////////////////////////////////////////////////////////////////////
QAudioBuffer::QAudioBuffer():QIODevice()
{
	memset(m_bufAudio,0,sizeof(m_bufAudio));
	m_sizeBuf = 0;
}

void QAudioBuffer::close()
{
	memset(m_bufAudio,0,sizeof(m_bufAudio));
	m_sizeBuf = 0;

	QIODevice::close();
}

qint64 QAudioBuffer::readData(char *data, qint64 maxlen)
{
	if(!m_mutexBuf.tryLock(500)){
		return 0;
	}

	qint64 ret = 0;
		
	if(m_sizeBuf<=0){
		ret = maxlen > MAX_CALC_BUF_LEN ? MAX_CALC_BUF_LEN:maxlen;
		memset(data,NULL,sizeof(char)*ret);
	}else{
		ret = maxlen > m_sizeBuf ? m_sizeBuf:maxlen;
		memcpy(data,m_bufAudio,ret);

		if((m_sizeBuf-ret)>0){
			memmove(m_bufAudio,m_bufAudio+ret,m_sizeBuf-ret);
			m_sizeBuf-=ret;
		} else {
			memset(m_bufAudio,0,sizeof(m_bufAudio));
			m_sizeBuf = 0;
		}
	}

	m_mutexBuf.unlock();
	
	return ret;
}

qint64 QAudioBuffer::writeData(const char *data, qint64 len)
{
	if(!m_mutexBuf.tryLock(500)){
		return 0;
	}

	qint64 ret = len;

	if(m_sizeBuf+len > sizeof(m_bufAudio)){
		int move = m_sizeBuf+len - sizeof(m_bufAudio);
		memmove(m_bufAudio,m_bufAudio+move,m_sizeBuf-move);

		m_sizeBuf-=move;
	}

	memcpy(m_bufAudio+m_sizeBuf,data,len);
	m_sizeBuf +=len;

	m_mutexBuf.unlock();

	m_curLevel = ComputeLevel((short*)data,ret/sizeof(short));

	return ret;
}

int QAudioBuffer::getLevel()
{
	return m_curLevel;
}
//////////////////////////////////////////////////////////////////////////
QPCMFile::QPCMFile():QFile(),m_curLevel(0)
{

}

qint64 QPCMFile::readData(char *data, qint64 maxlen)
{
	maxlen = maxlen > MAX_CALC_BUF_LEN ? MAX_CALC_BUF_LEN:maxlen;
	qint64 ret = QFile::readData(data,maxlen);
	m_curLevel = ComputeLevel((short*)data,ret/sizeof(short));
	return ret;
}

int QPCMFile::getLevel()
{
	return m_curLevel;
}

//////////////////////////////////////////////////////////////////////////
AudioDeviceDetect::AudioDeviceDetect(QObject* parent):m_outAudio(NULL)
	,m_lstOutAudioDevice(NULL)
	,m_fileAudio(NULL)
	,m_inAudio(NULL)
	,m_lstInAudioDevice(NULL)
	,m_bufInAudio(NULL)
	,m_tidPlayback(0)
{
	m_lstOutAudioDevice=new QList<QAudioDeviceInfo>(QAudioDeviceInfo::availableDevices(QAudio::AudioOutput));
	m_lstInAudioDevice=new QList<QAudioDeviceInfo>(QAudioDeviceInfo::availableDevices(QAudio::AudioInput));

	initOutAudioFormat();
	initInAudioFormat();

	m_fileAudio = new QPCMFile();
	m_bufInAudio = new QAudioBuffer();
}

AudioDeviceDetect::~AudioDeviceDetect()
{
	/* if delete this list , cause abnormal
	if(m_lstOutAudioDevice){
		delete m_lstOutAudioDevice;
		m_lstOutAudioDevice = NULL;
	}

	if(m_lstInAudioDevice){
		delete m_lstInAudioDevice;
		m_lstInAudioDevice = NULL;
	}

	*/
	stopDetectAudioOutDevice();
	stopDetectAudioInDevice();
	SAFE_DELETE(m_fileAudio);
	SAFE_DELETE(m_bufInAudio);
}

void AudioDeviceDetect::startDetectAudioOutDevice(QString& devName)
{
	stopDetectAudioOutDevice();

	m_fileAudio->setFileName(QString(":/media/res/media/audio.pcm"));
	m_fileAudio->open(QIODevice::ReadOnly);
	if(!m_fileAudio->isOpen()){
		return;
	}

	openOutAudioDevice(devName,m_fmtOutAudio,m_fileAudio);
	return;
}
 
void AudioDeviceDetect::stopDetectAudioOutDevice()
{
	if (NULL!=m_fileAudio&&m_fileAudio->isOpen()){
		m_fileAudio->close();
	}

	closeOutAudioDevice();
}

int AudioDeviceDetect::getAudioOutDeviceVolume()
{
	if(NULL==m_fileAudio){
		return 0;
	}

	int volume = ((QPCMFile*)m_fileAudio)->getLevel();
	return (int)volume;
}

void AudioDeviceDetect::initOutAudioFormat()
{
	m_fmtOutAudio.setSampleRate(8000);
	m_fmtOutAudio.setChannelCount(2);
	m_fmtOutAudio.setSampleSize(16);
	m_fmtOutAudio.setCodec(QString("audio/pcm"));
	m_fmtOutAudio.setByteOrder(QAudioFormat::LittleEndian);
	m_fmtOutAudio.setSampleType(QAudioFormat::UnSignedInt);
}

void AudioDeviceDetect::initInAudioFormat()
{
	m_fmtInAudio.setSampleRate(8000);
	m_fmtInAudio.setChannelCount(2);
	m_fmtInAudio.setSampleSize(16);
	m_fmtOutAudio.setCodec(QString("audio/pcm"));
	m_fmtInAudio.setByteOrder(QAudioFormat::LittleEndian);
	m_fmtInAudio.setSampleType(QAudioFormat::UnSignedInt);
}

void AudioDeviceDetect::startDetectAudioInDevice(QString& micName,QString& spkName)
{
	stopDetectAudioInDevice();

	if(micName.isEmpty()){
		return;
	}

	if(m_lstInAudioDevice->isEmpty()){
		return;
	}

	m_bufInAudio->open(QIODevice::ReadWrite);
	if(!m_bufInAudio->isOpen()){
		return;
	}

	QAudioDeviceInfo devInAudio;
	for(int i=0;i<m_lstInAudioDevice->size();i++)
	{
		QAudioDeviceInfo tempDevice = m_lstInAudioDevice->at(i);
		QString tempName = tempDevice.deviceName();
		if(tempName.compare(micName)==0){
			devInAudio = tempDevice;
			break;
		}
	}

	if(devInAudio.isNull()){
		devInAudio = QAudioDeviceInfo::defaultInputDevice();
	}

	if(!devInAudio.isFormatSupported(m_fmtInAudio)){
		m_fmtInAudio = devInAudio.nearestFormat(m_fmtInAudio);
	}

	m_inAudio = new QAudioInput(devInAudio,m_fmtInAudio,this);
	m_inAudio->start(m_bufInAudio);

	m_outSelDevName = spkName;

	if(m_tidPlayback == 0){
		m_tidPlayback = startTimer(3000);
	}

	return;
}

void AudioDeviceDetect::stopDetectAudioInDevice()
{
	if (NULL!=m_bufInAudio&&m_bufInAudio->isOpen()){
		m_bufInAudio->close();
	}

	if (m_tidPlayback>0){
		killTimer(m_tidPlayback);
		m_tidPlayback = 0;
	}

	closeOutAudioDevice();

	if(NULL==m_inAudio){
		return;
	}

	m_inAudio->stop();
	SAFE_DELETE(m_inAudio);
}

int AudioDeviceDetect::getAudioInDeviceVolume()
{
	if(NULL==m_bufInAudio){
		return 0;
	}

	int volume = m_bufInAudio->getLevel();
	return (int)volume;
}

void AudioDeviceDetect::openOutAudioDevice(QString& spkName,QAudioFormat& fmtAudio,QIODevice* deviceIo)
{
	closeOutAudioDevice();

	if(spkName.isEmpty()){
		return;
	}

	if(m_lstOutAudioDevice->isEmpty()){
		return;
	}

	QAudioDeviceInfo devOutAudio;
	for(int i=0;i<m_lstOutAudioDevice->size();i++)
	{
		QAudioDeviceInfo tempDevice = m_lstOutAudioDevice->at(i);
		QString tempName = tempDevice.deviceName();
		if(tempName.compare(spkName)==0){
			devOutAudio = tempDevice;
			break;
		}
	}

	if(devOutAudio.isNull()){
		devOutAudio = QAudioDeviceInfo::defaultOutputDevice();
	}

	if(!devOutAudio.isFormatSupported(fmtAudio)){
		return;
	}

	m_outAudio = new QAudioOutput(devOutAudio,fmtAudio,this);
	m_outAudio->start(deviceIo);
	
	return;
}

void AudioDeviceDetect::closeOutAudioDevice()
{
	if(NULL==m_outAudio){
		return;
	}

	m_outAudio->stop();
	SAFE_DELETE(m_outAudio);
}

void AudioDeviceDetect::timerEvent(QTimerEvent * event)
{
	if(NULL==event){
		return;
	}

	int tid = event->timerId();
	if(tid == m_tidPlayback){
		killTimer(m_tidPlayback);
		m_tidPlayback=0;

		openOutAudioDevice(m_outSelDevName,m_fmtOutAudio,m_bufInAudio);

		if(NULL!=m_inAudio){
			m_inAudio->stop();
			SAFE_DELETE(m_inAudio);
			return;
		}
	}
}