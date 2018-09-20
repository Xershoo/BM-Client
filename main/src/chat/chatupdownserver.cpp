#include "chatupdownserver.h"
#include <QDir>
#include <QFileInfo>
#include <QFile>

#include "common/Env.h"
#include "token/UploadTokenMgr.h"
#include "common/Util.h"
#include "biz/BizInterface.h"
#include "session/classsession.h"
#include "common/HttpSessionMgr.h"
#include "jason/include/json.h"
#include <string>
#include "common/Config.h"

using namespace std;

void ChatUpDownServer::HttpEventCallBack(int httpEventType, unsigned int lpUser, bool bIsFirst, unsigned int Param)
{
    ChatUpDownServer *pThis = (ChatUpDownServer*)lpUser;
    LPHTTPSESSION pHttpS = (LPHTTPSESSION)Param;
    if (NULL == pThis || NULL == pHttpS || pThis!= this)
    {
        return;
    }

    switch (httpEventType)
    {
    case HTTP_EVENT_BEGIN:
        {
           
        }
        break;

    case HTTP_EVENT_PROGRESS:
        {
            
        }
        break;

    case HTTP_EVENT_END:
        {
            pThis->httpEndTask((LPVOID)Param);
        }
        break;
    }
}

ChatUpDownServer::ChatUpDownServer(QObject *parent) : QObject(parent)
{
    connect(CHttpSessionMgr::GetInstance(),SIGNAL(httpEvent(int, unsigned int, bool, unsigned int)),this,SLOT(HttpEventCallBack(int, unsigned int, bool, unsigned int)),Qt::QueuedConnection);
}

ChatUpDownServer::~ChatUpDownServer()
{

}

bool ChatUpDownServer::downLoadFile(const ChatUserHttpData &userData)
{
    QString picCachePath = Env::GetUserPicCachePath(ClassSeeion::GetInst()->_nUserId) + userData.m_fileName;
    picCachePath.replace("\\","/");
    if(!QFileInfo(picCachePath).exists())
    {
        QString downLoadImagePath = getHttpDownUrl() + userData.m_fileName;
        int nID = CHttpSessionMgr::GetInstance()->HttpDownloadFile(
            downLoadImagePath.toLatin1().constData(),
            picCachePath.toLatin1().constData(),
            CHAT_HTTP_OUTTIME,0,(void*)NULL, (void*)this);

        if (-1 != nID)
        {
            QVector<int> idVec;
            idVec.append(nID);
            m_downLoadDataMap.insert(userData,idVec);
        }
    }
    return true;
}

bool ChatUpDownServer::upLoadFile(const ChatUserHttpData &userData)
{
    QString md5FilePath = getMd5Path(userData.m_fileName);
    if(!QFile(md5FilePath).exists())
        return false;

    char pszToken[MAX_PATH] = {0};
    if(!getUploadToken(pszToken,ClassSeeion::GetInst()->_nUserId))
        return false;

    QString sourceFileName = QFileInfo(userData.m_fileName).fileName();
    QString uploadUrl = getHttpUploadUrl();
    QString form = QString("{\"uid\":\"%1\",\"t\":\"%2\",\"old\":\"%3\",\"POS\":\"2\"}")
        .arg(ClassSeeion::GetInst()->_nUserId)
        .arg(pszToken)
        .arg(sourceFileName);

    int nID = CHttpSessionMgr::GetInstance()->HttpUploadFile(
        uploadUrl.toLatin1().constData(),
        md5FilePath.toLatin1().constData(),
        form.toLatin1().constData(),
        CHAT_HTTP_OUTTIME,0,(void*)NULL, (void*)this);

    QVector<int> idVec;
    idVec.append(nID);
    m_upLoadDataMap.insert(userData,idVec);
    return true;
}

bool ChatUpDownServer::copyFile(const QString &sourcePath,const QString &targetPath)
{
    if(sourcePath == targetPath)
        return true;

    QDir dir(Env::GetUserPicCachePath(ClassSeeion::GetInst()->_nUserId));
    if(!dir.exists())
        dir.mkdir(targetPath);
    if(!QFile::exists(sourcePath)|| !dir.exists())
        return false;
    return QFile::copy(sourcePath,targetPath);
}

bool ChatUpDownServer::copyFileToMD5Path(const QString &sourcePath)
{
    return copyFile(sourcePath,getMd5Path(sourcePath));
}

QString ChatUpDownServer::getMd5Path(const QString &sourcePath)
{
    QString md5Path = "";
    QString picCachePath = Env::GetUserPicCachePath(ClassSeeion::GetInst()->_nUserId);
    Util::CaleFileMd5(sourcePath,md5Path);
    md5Path = picCachePath + md5Path + '.' + QFileInfo(sourcePath).suffix();
    return md5Path;
}

bool ChatUpDownServer::getUploadToken(char *pszToken, __int64 nUserId)
{
    if (0 != CUploadTokenMgr::GetInstance()->GetToken(pszToken, nUserId))
    {
        return false;
    }
    return true;
}


QString ChatUpDownServer::getHttpUploadUrl()
{
    //return "http://61.147.188.58:14080/eduup2c/up/chat_img";
    //外网
    //return "http://221.228.195.57:8063/eduup2c/up/chat_img";

    QString uploadUrl(Config::getConfig()->m_urlChatImageUpload.c_str()); 
    return uploadUrl;    
}

QString ChatUpDownServer::getHttpDownUrl()
{
    //return "http://61.147.188.58:12080/edudown/img/chat/";
    //外网
    //return "http://221.228.195.57:12080/edudown/img/chat/";

    QString downUrl(Config::getConfig()->m_urlChatImageDown.c_str());    
    return downUrl;
}

void ChatUpDownServer::httpEndTask(void* param)
{
    LPHTTPSESSION lpHttp = (LPHTTPSESSION)param;

    if(HTTP_SESSION_DOWNLOAD == lpHttp->type)
    {
        QMap<ChatUserHttpData,QVector<int>>::iterator it;
        for(it = m_downLoadDataMap.begin(); it != m_downLoadDataMap.end();++it)
        {
            //本地文件校验，若失败则重复下载。
            if((it).value().contains(lpHttp->id)){
                QString md5 = "";
                QString picCachePath = Env::GetUserPicCachePath(ClassSeeion::GetInst()->_nUserId)
                    + QFileInfo((it).key().m_fileName).fileName();
                Util::CaleFileMd5(picCachePath,md5);
                if(md5 != QFileInfo((it).key().m_fileName).baseName()  && CHAT_DOWNLOAD_UPLOAD_MAX_NUM > (it).value().size())
                {
                    innerDownLoadFile((it).key());
                    return;
                }
                else if(md5 != QFileInfo((it).key().m_fileName).baseName()  && CHAT_DOWNLOAD_UPLOAD_MAX_NUM <= (it).value().size())
                {
                    emit sendHttpResult(DOWN_LOAD_FAILED,(it).key());
                    m_downLoadDataMap.erase(it);
                    return;
                }
                else
                {
                    emit sendHttpResult(DOWN_LOAD_SUCCESS,(it).key());
                    m_downLoadDataMap.erase(it);
                    return;
                }
            }
        }
    }
    else if(HTTP_SESSION_UPLOAD == lpHttp->type)
    {
        QMap<ChatUserHttpData,QVector<int>>::iterator it;
        for(it = m_upLoadDataMap.begin(); it != m_upLoadDataMap.end();++it)
        {
            //使用LPHTTPSESSION验证是否上传成功
            if((it).value().contains(lpHttp->id))
            {
                string strKey;
                string strValue;
                int nCode;
                Json::Reader jReader;
                Json::Value jValue;
                jReader.parse(lpHttp->response, jValue);
                Json::Value::Members jKeys = jValue.getMemberNames();
                for (Json::Value::Members::iterator iter = jKeys.begin(); iter != jKeys.end(); ++iter)
                {
                    strKey = *iter;
                    if (0 == strKey.compare("code"))
                    {
                        nCode = jValue[strKey.c_str()].asInt();
                    }
                    if (0 == strKey.compare("url"))
                    {
                        strValue = jValue[strKey.c_str()].asString();
                        Util::URLDecode(strValue.c_str(), strValue);
                    }
                }

                if (40101 == nCode || lpHttp->rcode != 200 || strValue == "" && CHAT_DOWNLOAD_UPLOAD_MAX_NUM > (it).value().size())
                {
                    upLoadFile((it).key());
                }
                else if(40101 == nCode || lpHttp->rcode != 200 || strValue == "" && CHAT_DOWNLOAD_UPLOAD_MAX_NUM <= (it).value().size())
                {
                    emit sendHttpResult(UP_LOAD_FAILED,(it).key());
                    m_upLoadDataMap.erase(it);
                }
                else
                {
                    emit sendHttpResult(UP_LOAD_SUCCESS,(it).key());
                    m_upLoadDataMap.erase(it);
                }
                return;
            }
        }

    }
    else
        return;
}
void ChatUpDownServer::innerDownLoadFile(const ChatUserHttpData &userData)
{
    QString picCachePath = Env::GetUserPicCachePath(ClassSeeion::GetInst()->_nUserId)
        + QFileInfo(userData.m_fileName).fileName();

    if(QFile(picCachePath).exists())
        QFile::remove(picCachePath);

    picCachePath.replace("\\","/");
    int nID = CHttpSessionMgr::GetInstance()->HttpDownloadFile(
        userData.m_fileName.toLatin1().constData(),
        picCachePath.toLatin1().constData(),
        CHAT_HTTP_OUTTIME,
        0,(void*)NULL, (void*)this
        );
    m_downLoadDataMap[userData].append(nID);
}

void ChatUpDownServer::innerUpLoadFile(const ChatUserHttpData &userData)
{
    QString md5FilePath = getMd5Path(userData.m_fileName);

    if(!QFile(md5FilePath).exists())
        return;

    char pszToken[MAX_PATH] = {0};
    if(!getUploadToken(pszToken,ClassSeeion::GetInst()->_nUserId))
        return;

    QString sourceFileName = QFileInfo(userData.m_fileName).fileName();
    QString uploadUrl = getHttpUploadUrl();
    QString form = QString("{\"uid\":\"%1\",\"t\":\"%2\",\"old\":\"%3\",\"POS\":\"2\"}")
        .arg(ClassSeeion::GetInst()->_nUserId)
        .arg(pszToken)
        .arg(sourceFileName);

    int nID = CHttpSessionMgr::GetInstance()->HttpUploadFile(
        uploadUrl.toLatin1().constData(),
        md5FilePath.toLatin1().constData(),
        form.toLatin1().constData(),
        CHAT_HTTP_OUTTIME, 0, (void*)NULL, (void*)this);

    m_upLoadDataMap[userData].append(nID);
}

