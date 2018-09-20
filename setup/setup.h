#ifndef SETUP_H
#define SETUP_H

#include <QString>
#include <QObject>

class QProcess;

class SetUp : public QObject{

    Q_OBJECT

signals:

	void sendProcess(int);

public:
    explicit SetUp();
    ~SetUp();

    void setConfigPath(const QString &configPath);
    void run();
    void createShortCut();
    void startExe();

	void createShortCutToDeskTop();
	void createShortCutToStartMenue();
	void selfStarting();

private slots:

    void uncompressFinished();
    void uncompressSuccess();

private:

    QString m_cfgPath;
    QProcess *m_pro;
    bool m_isOver;

private:
    void createRegeditData()const;
    void createNetLogin()const;
    SetUp& operator =(const SetUp &){return *this;}
    SetUp(const SetUp&){}
};

#endif // SETUP_H
