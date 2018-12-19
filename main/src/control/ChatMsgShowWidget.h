#ifndef CHAT_MSG_SHOW_WIDGET_H
#define CHAT_MSG_SHOW_WIDGET_H

#include <QtWidgets\QListWidget>
#include <QtWidgets\QLabel>
#include <QtWidgets\QTextEdit>
#include <QtCore\QMap>
#include <QtCore\QStringList>
#include <QtGui\QMovie>

typedef QMap<int,QString>	QMapIntString;

class QChatMsgShowItem : public QWidget
{
	enum 
	{
		MSG_TIME = 0,
		MSG_SYS,
		MSG_SELF,
		MSG_USER,

		MSG_UNKNOWN
	};
	Q_OBJECT
public:
	QChatMsgShowItem(QWidget* parent,int itemWidth);
	~QChatMsgShowItem();

	void	showSysMsg(const QString& strMsg,bool time = false);
	void  	showChatMsg(bool isSelf,const QString& qstrUserName,const QString& qstrHeadImage,const QString& qstrMsg);
	QSize	getItemSize();
	void	resetItemWidth(int itemWidth);
	bool	judgeChatMsg(bool isSelf,const QString& qstrUserName,const QString& qstrHeadImage,const QString& qstrMsg);

public:
	inline void setItemIndex(int index){
		m_itemIndex = index;
	}

	inline int getItemIndex(){
		return m_itemIndex;
	}

protected:
	virtual void paintEvent(QPaintEvent *);
	virtual void timerEvent(QTimerEvent *);
	virtual bool eventFilter(QObject *, QEvent *);
protected:
	void	addUserName(const QString& userName);
	void	addUserHeader(const QString& imageHead);
	void	addUserMsg(const QString& qstrMsg);
	void	showProcessMovie();
	QSize	getTextSize(const QString& qstrMsg);

	void	parseHtml(const QString& qstrHtml,bool judgeImageLoad);
	void	freeMovieMap();
	void	parseImageMovie(const QString& imageFile);
	void	parseLoadImage(const QString& imageFile);
	void	judgeImageLoadFinish();
	void	showImageMovie(QMovie* movie);

	QPixmap	loadHeadImage(const QString& qstrHeadImage);
protected slots:
	void	imageAnimate(int);

signals:
	void	refreshMsgShow(int);

protected:
	int		m_msgType;
	int		m_refreshTimer;
	int		m_itemIndex;

	QLabel		* m_labelUserName;
	QLabel		* m_labelUserHeader;
	QTextEdit	* m_textUserMsg;
	QLabel		* m_labelMovie;
	QMovie		* m_movieProcess;

	QString		m_userMsg;
	QString		m_userName;
	QString		m_userHeader;
	
	QStringList		m_loadImageList;
	QMapIntString	m_mapMovieToName;
};

class QChatMsgShowWidget : public QListWidget
{
	Q_OBJECT
public:
	QChatMsgShowWidget(QWidget* parent);
	~QChatMsgShowWidget();

public:
	virtual void addSystemMsg(const QString& qstrSysMsg);
	virtual void addChatMsg(bool isSelf,const QString& qstrUserName,const QString& qstrHeadImage,const QString& qstrMsg);
	virtual void clearAllMsg();
	virtual void delFrontMsg(int delMsgNum);

	virtual void refresh();
public slots:
	virtual void refreshItemShow(int);
signals:
	void loadFinished();

protected:
	virtual void resizeEvent(QResizeEvent *);
	virtual void timerEvent(QTimerEvent *event);

protected:
	void popFrontListItem(int number);
	void addListItem(QChatMsgShowItem* msgItem);

	int getItemWidth();
	void resetListItem();

	bool hasChatMsg(bool isSelf,const QString& qstrUserName,const QString& qstrHeadImage,const QString& qstrMsg);

protected:
	__int64 m_lastChatTime;
};

#endif