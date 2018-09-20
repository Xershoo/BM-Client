#ifndef CHATWINDOWTEXTBLOCKUSERDATA_H
#define CHATWINDOWTEXTBLOCKUSERDATA_H

#include <QTextBlockUserData>

class ChatWindowTextBlockUserData : public QTextBlockUserData
{
public:
    ChatWindowTextBlockUserData();
    ~ChatWindowTextBlockUserData();

    int getData();
    void setData(int data);
private:
    int m_data;
};

#endif // CHATWINDOWTEXTBLOCKUSERDATA_H
