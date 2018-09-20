#ifndef CHATUPDOWNSERVER_H
#define CHATUPDOWNSERVER_H

#include <QObject>
#include <QVector>
#include <QMap>

const int CHAT_DOWNLOAD_UPLOAD_MAX_NUM = 30;
const int CHAT_HTTP_OUTTIME = 600;

enum HttpResult{
    DOWN_LOAD_SUCCESS = 0,
    DOWN_LOAD_FAILED,
    UP_LOAD_SUCCESS,
    UP_LOAD_FAILED
};

struct ChatUserHttpData{
    int m_chatId;
    QString m_fileName;
    ChatUserHttpData(int chatId = -1,const QString &fileName = "")
        :m_chatId(chatId),m_fileName(fileName){
    }
    ChatUserHttpData(const ChatUserHttpData& msg){
        this->m_fileName = msg.m_fileName;
        this->m_chatId = msg.m_chatId;
    }
	bool operator <(const ChatUserHttpData &rhs){
		return int(m_chatId) < int(rhs.m_chatId);
	} 
	friend bool operator<(const ChatUserHttpData &lhs,const ChatUserHttpData &rhs){
		return int(lhs.m_chatId) < int(rhs.m_chatId);
	}
private:
    ChatUserHttpData& operator = (const ChatUserHttpData& msg){
        return *this;
    }
};


class ChatUpDownServer : public QObject
{
    Q_OBJECT
signals:

	void sendHttpResult(HttpResult httpResult,const ChatUserHttpData &userData);

public:
    explicit ChatUpDownServer(QObject *parent = 0);
	~ChatUpDownServer();

    bool downLoadFile(const ChatUserHttpData &userData);
    bool upLoadFile(const ChatUserHttpData &userData);

private:

    QMap<ChatUserHttpData,QVector<int>> m_upLoadDataMap;
    QMap<ChatUserHttpData,QVector<int>> m_downLoadDataMap;

    void innerDownLoadFile(const ChatUserHttpData &userData);
    void innerUpLoadFile(const ChatUserHttpData &userData);

    void httpEndTask(void* param);


public:
    static bool copyFile(const QString &sourcePath,const QString &targetPath);
    static bool copyFileToMD5Path(const QString &sourcePath);
    static bool getUploadToken(char *pszToken, __int64 nUserId);

    static QString getMd5Path(const QString &sourcePath);
    static QString getHttpUploadUrl();
    static QString getHttpDownUrl();

protected slots:    
    void HttpEventCallBack(int, unsigned int, bool, unsigned int);
};

#endif // CHATUPDOWNSERVER_H
