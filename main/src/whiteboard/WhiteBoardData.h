//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：WhiteBoardData.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.12.4
//	说明：与白板相关的数据结构和常量的定义头文件
//  接口：
//
//**********************************************************************
#ifndef WHITEBOARD_DATA_H
#define WHITEBOARD_DATA_H

#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtGui/QColor>
#include <string>
#include <vector>
#include "varname.h"

using namespace std;

#define FONT_FACESIZE (32)

enum WBSize
{
    WB_SIZE_BIG = 0,
    WB_SIZE_MID,
    WB_SIZE_SMALL,

    WB_SIZE_UNKNOW,
};

enum WBColor
{
    WB_COLOR_RED = 0,
    WB_COLOR_YELLOW,
    WB_COLOR_BLUE,
    WB_COLOR_WHITE,
    WB_COLOR_GREEN,
};

enum WBMode
{
    WB_MODE_NONE = 0,
    WB_MODE_CURVE,
    WB_MODE_TEXT,
    WB_MODE_ERASER,
    WB_MODE_LARSER,
};

enum WBCtrl
{
    WB_CTRL_NONE = 0,
    WB_CTRL_SELF,
    WB_CTRL_EVERY,
};

struct WBPointF
{
public:
    WBPointF()
    {
        X = Y = 0.0f;
    };

    WBPointF(float x,float y)
    {
        X = x;
        Y = y;
    }

public:
    float X;
    float Y;
};

struct WBRectF
{
public:
    WBRectF()
    {
        X = Y = 0.0f;
        Width = Height = 0.0f;
    };

    WBRectF(float x,float y,float width,float height)
    {
        X = x;
        Y = y;
        Width = width;
        Height = height;
    };

public:
    float X;
    float Y;
    float Width;
    float Height;
};

typedef vector<WBPointF>	PointVector;

enum
{
    LINE_DIRECTION_UNKNOW = 0,
    LINE_DIRECTION_LEFT ,
    LINE_DIRECTION_LEFTTOP ,
    LINE_DIRECTION_TOP,
    LINE_DIRECTION_RIGHTTOP,
    LINE_DIRECTION_RIGHT,
    LINE_DIRECTION_RIGHTBOTTOM,
    LINE_DIRECTION_BOTTOM,
    LINE_DIRECTION_LEFTBOTTOM,
    LINE_DIRECTION_NONE,
};

struct _paint_element
{
    virtual void ToString(string& str) = 0;
    virtual void FromString(string& str) = 0;
};

typedef struct WBFreeCurve : public _paint_element
{
    DWORD			dwColor;
    float			nWidth;
    PointVector		vecPointReal;
    PointVector		vecPointCalc;

    void PutCalcPoint(float ratio,bool end = false);

    void ToString(string& str);
    void FromString(string& str);

    static int GetLineDirection(WBPointF p1,WBPointF p2);
    static bool IsDirectionCorner(int l1,int l2);
}WBFREECURVE,*LPWBFREECURVE;

typedef struct WBText : public _paint_element
{
    wstring			wstrText;
    WBRectF			rcText;

    WCHAR			wszFaceName[FONT_FACESIZE];
    DWORD			dwTextMask;
    DWORD			dwTextEffects;
    float			yHeight;
    DWORD			dwColor;

    void ToString(string& str);
    void FromString(string& str);

}WBTEXT,*LPWBTEXT;

typedef struct WBErase : public _paint_element
{
    __int64 nUserId;
    int	 nPaintId;

    void ToString(string& str);
    void FromString(string& str);
}WBERASE,*LPWBERASE;

typedef struct WBLarser : public _paint_element
{
    WBRectF		rcLarser;
    bool        bShow;
    void ToString(string& str);
    void FromString(string& str);
}WBLASER,*LPWBLARSER;

class WhiteBoardItem
{
public:
    virtual void startDraw(QGraphicsSceneMouseEvent * event) = 0; 
    virtual void drawing(QGraphicsSceneMouseEvent * event) = 0;
    virtual void endDraw(QGraphicsSceneMouseEvent * event) = 0;

    virtual void setHoverColor(QColor& clr) = 0;
    virtual void getItemString(std::string& strItem) = 0;

    inline int getId()
    {
        return m_itemId;
    };

    inline void setId(int id)
    {
        m_itemId = id;
    };

    inline __int64 getUserId()
    {
        return m_userId;
    };

    inline void setUserId(__int64 id)
    {
        m_userId = id;
    };

protected:
    int         m_itemId;
    __int64     m_userId;
};

#endif