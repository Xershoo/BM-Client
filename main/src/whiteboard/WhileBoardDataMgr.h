//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�QWhiteBoardDataMgr.cpp
//	�汾�ţ�1.0
//	���ߣ�����
//	ʱ�䣺2015.12.11
//	˵����
//  �޸�˵����
//**********************************************************************

#pragma once

#include <QObject>
#include <vector>
#include <QMutex>
#include "../varname.h"
#include "QWhiteBoard.h"


#define NAME_MAX_LENGTH		(255)

using namespace std;

//�װ�����
typedef struct tagWhiteBoardData
{
    tagWhiteBoardData()
    {
        m_nWhiteboardID = 0;
        memset(m_wszName, 0, NAME_MAX_LENGTH);
        m_pWhiteBoard = NULL;
    }

    int			        m_nWhiteboardID;
    wchar_t		        m_wszName[NAME_MAX_LENGTH];
    QWhiteBoardView*    m_pWhiteBoard;

}WHITEBOARDDATA, *LPWHITEBOARDDATA;

typedef vector<LPWHITEBOARDDATA> WhiteboardVector;

class WhiteBoardDataMgr : public QObject
{
    Q_OBJECT
public:
    static WhiteBoardDataMgr* getInstance();
    static void freeInstance();

public:
    WhiteBoardDataMgr();
    virtual ~WhiteBoardDataMgr();

    int getNewId();

    QWhiteBoardView* AddWhiteboard(int nId,QString strName,bool sendMsg = true);

    bool RenameWhiteboard(int nWhiteboardID, LPCWSTR pwszName);    
    bool DelWhiteboardByID(int nWhiteboardID); 
    bool DelWhiteboardByName(LPCWSTR pwszName);    
    bool OpenWhiteboardByID(int nWhiteboardID,bool sendMsg = true);    
    bool OpenWhiteboardByName(LPCWSTR pwszName);
    bool CloseWhiteboardByID(int nWhiteboardID);
    bool CloseWhiteboardByName(LPCWSTR pwszName);
    
    LPWHITEBOARDDATA GetWhiteboardByID(int nWhiteboardID);    
    LPWHITEBOARDDATA GetWhiteboardByName(LPCWSTR pwszName);
    
    void ClearAll();    
    int GetWhiteboardCount() 
    { 
        return m_vecWhiteborad.size(); 
    };

public:    
    void SetMode(LPCWSTR pwszName, WBMode mode);
    void SetTextSize(LPCWSTR pwszName, WBSize tSize);
    void SetColor(LPCWSTR pwszName, WBColor clr);
    void SetEnable(LPCWSTR pwszName, WBCtrl enable);
    void UnDo(LPCWSTR pwszName);
    void Clear(LPCWSTR pwszName);
	void SetAllEnable(WBCtrl enable);

signals:
    void add_whiteboard(QString);
    void modifyname_wb(QString, QString);
    void mainshowchanged(int, int, int);

protected:
    WhiteboardVector    m_vecWhiteborad;
    QMutex              m_mutex;
    
    int                 m_nBaseID;

private:
    static WhiteBoardDataMgr*  m_instance;
};

