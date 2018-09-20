#include "chatwindowtextblockuserdata.h"

ChatWindowTextBlockUserData::ChatWindowTextBlockUserData()
{
    m_data = 0;
}

ChatWindowTextBlockUserData::~ChatWindowTextBlockUserData()
{

}

void ChatWindowTextBlockUserData::setData(int data)
{
    m_data = data;
}

int ChatWindowTextBlockUserData::getData()
{
    return m_data;
}