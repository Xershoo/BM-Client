//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：WhiteBoardData.cpp
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.12.4
//	说明：与白板相关的数据结构和常量的定义源文件
//  接口：
//
//**********************************************************************
#include "WhiteBoardData.h"
#include "macros.h"
#include "jason/include/json.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////////
void WBFreeCurve::ToString(string& str)
{
	Json::Value jValueRoot;
			
	jValueRoot["clr"] = Json::Value((int)dwColor);
	jValueRoot["width"] = Json::Value((double)nWidth);
	jValueRoot["point-size"] = Json::Value((UINT)vecPointCalc.size());

	for(int i= 0; i<vecPointCalc.size();i++)
	{
		Json::Value jValuePoint;
		WBPointF pt = vecPointCalc.at(i);
		jValuePoint["X"] = Json::Value((double)pt.X);
		jValuePoint["Y"] = Json::Value((double)pt.Y);

		char szKey[32] = { 0 };
		sprintf_s(szKey,"point_%d",i+1);
		jValueRoot[szKey] = jValuePoint;
	}

	Json::FastWriter jFastWrite;
	str = jFastWrite.write(jValueRoot);
}

void WBFreeCurve::FromString(string& str)
{
	string strKey;
	string strValue;

	Json::Reader jReader; 
	Json::Value jValue; 

	jReader.parse(str,jValue);
	Json::Value::Members jKeys = jValue.getMemberNames();

	UINT nPtNum = 0;

	for(Json::Value::Members::iterator iter = jKeys.begin(); iter != jKeys.end(); ++iter)
	{
		strKey = *iter;
		if(strKey.compare("clr") == 0)
		{			
			dwColor = (DWORD)jValue[strKey.c_str()].asUInt();
		}
		else if(strKey.compare("width") == 0)
		{			
			nWidth = jValue[strKey.c_str()].asDouble();
		}
		else if(strKey.compare("point-size") == 0)
		{			
			nPtNum = jValue[strKey.c_str()].asInt();
		}
	}

	for(int i= 0 ; i< nPtNum;i++)
	{
		char szKey[32] = { 0 };
		sprintf_s(szKey,"point_%d",i+1);

		if(jValue[szKey].isNull())
		{
			continue;
		}

		Json::Value jValuePoint = jValue[szKey]; 
		
		WBPointF pt;
		
		pt.X = (float)jValuePoint["X"].asDouble();
		pt.Y = (float)jValuePoint["Y"].asDouble();

		vecPointCalc.push_back(pt);
		vecPointReal.push_back(pt);
	}
}

int WBFreeCurve::GetLineDirection(WBPointF p1,WBPointF p2)
{
	int x1 = (int)p1.X;
	int x2 = (int)p2.X;
	int y1 = (int)p1.Y;
	int y2 = (int)p2.Y;

	if( x1 == x2)
	{
		if(y1 == y2)
		{
			return LINE_DIRECTION_NONE;
		}
		else if( y1 > y2)
		{
			return LINE_DIRECTION_BOTTOM;
		}
		else 
		{
			return LINE_DIRECTION_TOP;
		}
	}
	else if( x1 > x2)
	{
		if(y1 == y2)
		{
			return LINE_DIRECTION_LEFT;
		}
		else if( y1 > y2)
		{
			return LINE_DIRECTION_LEFTBOTTOM;
		}
		else 
		{
			return LINE_DIRECTION_LEFTTOP;
		}
	}
	else
	{
		if(y1 == y2)
		{
			return LINE_DIRECTION_RIGHT;
		}
		else if( y1 > y2)
		{
			return LINE_DIRECTION_RIGHTBOTTOM;
		}
		else 
		{
			return LINE_DIRECTION_RIGHTTOP;
		}
	}
}

bool WBFreeCurve::IsDirectionCorner(int l1,int l2)
{
	if(l1 == l2)
	{
		return false;
	}

	switch(l1)
	{
	case LINE_DIRECTION_LEFT:
		{
			if(l2 == LINE_DIRECTION_LEFTTOP || l2 == LINE_DIRECTION_LEFTBOTTOM)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		break;
	case LINE_DIRECTION_LEFTTOP:
		{
			if(l2 == LINE_DIRECTION_LEFT || l2 == LINE_DIRECTION_TOP)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		break;
	case LINE_DIRECTION_TOP:
		{
			if(l2 == LINE_DIRECTION_LEFTTOP || l2 == LINE_DIRECTION_RIGHTTOP)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		break;
	case LINE_DIRECTION_RIGHTTOP:
		{
			if(l2 == LINE_DIRECTION_RIGHT || l2 == LINE_DIRECTION_TOP)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		break;
		
	case LINE_DIRECTION_RIGHT:
		{
			if(l2 == LINE_DIRECTION_RIGHTTOP || l2 == LINE_DIRECTION_RIGHTBOTTOM)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		break;
	case LINE_DIRECTION_RIGHTBOTTOM:
		{
			if(l2 == LINE_DIRECTION_RIGHT || l2 == LINE_DIRECTION_BOTTOM)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		break;
	case LINE_DIRECTION_BOTTOM:
		{
			if(l2 == LINE_DIRECTION_LEFTBOTTOM || l2 == LINE_DIRECTION_RIGHTBOTTOM)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		break;
	case LINE_DIRECTION_LEFTBOTTOM:
		{
			if(l2 == LINE_DIRECTION_LEFT || l2 == LINE_DIRECTION_BOTTOM)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		break;
	};

	return true;
}

void WBFreeCurve::PutCalcPoint(float ratio,bool end /* = false */)
{
	if(vecPointReal.empty())
	{
		return;
	}

	int nSize = vecPointReal.size();	
	WBPointF lastPoint = vecPointReal.at(nSize - 1);
	if( nSize == 1 || nSize == 2 || end)
	{
		vecPointCalc.push_back(lastPoint);
		return;
	}

	int nSizeC = vecPointCalc.size();
	WBPointF lastPointC = vecPointCalc.at(nSizeC-1);
	if(	abs(lastPoint.X * ratio - lastPointC.X * ratio) >= 10 || 
		abs(lastPoint.Y * ratio- lastPointC.Y * ratio) >= 10)
	{
		vecPointCalc.push_back(lastPoint);
		return;
	}

	WBPointF pr1 = vecPointReal.at(nSize - 1);
	WBPointF pr2 = vecPointReal.at(nSize - 2);

	WBPointF pc1 = vecPointCalc.at(nSizeC - 1);
	WBPointF pc2 = vecPointCalc.at(nSizeC - 2);
		
	pr1.X *= ratio;
	pr1.Y *= ratio;
	pr2.X *= ratio;
	pr2.Y *= ratio;
	
	pc1.X *= ratio;
	pc1.Y *= ratio;
	pc2.X *= ratio;
	pc2.Y *= ratio;

	int ld1 = GetLineDirection(pr2,pr1);
	int ld2 = GetLineDirection(pc2,pc1);

	if(!IsDirectionCorner(ld1,ld2))
	{
		return;
	}

	vecPointCalc.push_back(lastPoint);
}

//////////////////////////////////////////////////////////////////////////
void WBErase::ToString(string& str)
{
	Json::Value jValueRoot;
	jValueRoot["pid"] = Json::Value(nPaintId);
	jValueRoot["uid"] = Json::Value((int)nUserId);

	Json::FastWriter jFastWrite;
	str = jFastWrite.write(jValueRoot);
}

void WBErase::FromString(string& str)
{
	string strKey;
	string strValue;

	Json::Reader jReader; 
	Json::Value jValue; 

	jReader.parse(str,jValue);
	Json::Value::Members jKeys = jValue.getMemberNames();

	for(Json::Value::Members::iterator iter = jKeys.begin(); iter != jKeys.end(); ++iter)
	{
		strKey = *iter;
		if(strKey.compare("pid") == 0)
		{			
			nPaintId = jValue[strKey.c_str()].asInt();
		}
		else if(strKey.compare("uid") == 0)
		{
			nUserId = (__int64)jValue[strKey.c_str()].asInt();
		}
	}
	
}

//////////////////////////////////////////////////////////////////////////
void WBLarser::ToString(string& str)
{
	Json::Value jValueRoot;
	jValueRoot["left"] = Json::Value((double)rcLarser.X);
	jValueRoot["top"] = Json::Value((double)rcLarser.Y);
	jValueRoot["width"] = Json::Value((double)rcLarser.Width);
	jValueRoot["height"] = Json::Value((double)rcLarser.Height);
    jValueRoot["show"] = Json::Value((bool)bShow);
	Json::FastWriter jFastWrite;
	str = jFastWrite.write(jValueRoot);
}

void WBLarser::FromString(string& str)
{
	string strKey;
	string strValue;

	Json::Reader jReader; 
	Json::Value jValue; 

	jReader.parse(str,jValue);
	Json::Value::Members jKeys = jValue.getMemberNames();

	for(Json::Value::Members::iterator iter = jKeys.begin(); iter != jKeys.end(); ++iter)
	{
		strKey = *iter;
		if(strKey.compare("left") == 0)
		{			
			rcLarser.X = (float)jValue[strKey.c_str()].asDouble();
		}
		else if(strKey.compare("top") == 0)
		{
			rcLarser.Y = (float)jValue[strKey.c_str()].asDouble();
		}
		else if(strKey.compare("width") == 0)
		{
			rcLarser.Width = (float)jValue[strKey.c_str()].asDouble();
		}
		else if(strKey.compare("height") == 0)
		{
			rcLarser.Height = (float)jValue[strKey.c_str()].asDouble();
		}
        else if(strKey.compare("show") == 0)
        {
            bShow = (bool)jValue[strKey.c_str()].asBool();
        }
	}
}

//////////////////////////////////////////////////////////////////////////
void WBText::ToString(string& str)
{
	Json::Value jValueRoot;
	Json::Value jValueRect;

    int   lenText = wstrText.length() * 2 + 1;
    char* pszText = new char[lenText];
    char  szFace[FONT_FACESIZE*2] = { 0 };

    memset(pszText,NULL,sizeof(char)*lenText);

    Util::UnicodeToAnsi(wszFaceName,wcslen(wszFaceName),szFace,FONT_FACESIZE*2);
    Util::UnicodeToAnsi(wstrText.c_str(),wstrText.length(),pszText,lenText);

	jValueRoot["txt"] = Json::Value(pszText);
	jValueRoot["clr"] = Json::Value((int)dwColor);
	jValueRoot["height"] = Json::Value((double)yHeight);
	jValueRoot["face"] = Json::Value(szFace);
	jValueRect["top"] = Json::Value((double)rcText.Y);
	jValueRect["left"] = Json::Value((double)rcText.X);
	jValueRect["width"] = Json::Value((double)rcText.Width);
	jValueRect["height"] = Json::Value((double)rcText.Height);
	jValueRoot["rect"] = jValueRect;

	Json::FastWriter jFastWrite;
	str = jFastWrite.write(jValueRoot);

    delete[] pszText;
}

void WBText::FromString(string& str)
{
	string strKey;
	string strValue;

	Json::Reader jReader; 
	Json::Value jValue; 

	jReader.parse(str,jValue);
	Json::Value::Members jKeys = jValue.getMemberNames();

	for(Json::Value::Members::iterator iter = jKeys.begin(); iter != jKeys.end(); ++iter)
	{
		strKey = *iter;
		if(strKey.compare("txt") == 0)
		{
			strValue = jValue[strKey.c_str()].asString();
        
            int     nLength = strValue.length() + 1;
            WCHAR*  wszValue = new WCHAR[nLength];
            memset(wszValue,0,sizeof(WCHAR) * nLength);

            Util::AnsiToUnicode(strValue.c_str(),strValue.length(),wszValue,nLength);
			wstrText = wstring(wszValue);

            delete[] wszValue;
		}
		else if(strKey.compare("clr") == 0)
		{
			dwColor = (DWORD)jValue[strKey.c_str()].asUInt();
		}
		else if(strKey.compare("height") == 0)
		{
			yHeight = jValue[strKey.c_str()].asDouble();
		}
		else if(strKey.compare("face") == 0)
		{
			strValue = jValue[strKey.c_str()].asString();

            int     nLength = strValue.length() + 1;
            WCHAR*  wszValue = new WCHAR[nLength];
            memset(wszValue,0,sizeof(WCHAR) * nLength);

            Util::AnsiToUnicode(strValue.c_str(),strValue.length(),wszValue,nLength);

			wcscpy_s(wszFaceName,wszValue);

            delete[] wszValue;
		}
		else if(strKey.compare("rect") == 0)
		{
			Json::Value jValueRect = jValue[strKey.c_str()]; 
			
			Json::Value::Members jKeysRect = jValueRect.getMemberNames();
			
			for(Json::Value::Members::iterator itRect = jKeysRect.begin(); itRect != jKeysRect.end(); ++itRect)
			{
				string strKeyRect = *itRect;
				if(strKeyRect.compare("left") == 0)
				{
					rcText.X = (float)jValueRect[strKeyRect.c_str()].asDouble();
				}
				else if(strKeyRect.compare("top") == 0)
				{
					rcText.Y = (float)jValueRect[strKeyRect.c_str()].asDouble();
				}
				else if(strKeyRect.compare("width") == 0)
				{
					rcText.Width = (float)jValueRect[strKeyRect.c_str()].asDouble();
				}
				else if(strKeyRect.compare("height") == 0)
				{
					rcText.Height = (float)jValueRect[strKeyRect.c_str()].asDouble();
				}
			}
		}			
	}
}
