#pragma once
#include <vector>
#include <map>
#include "DeComp.h"
#include "Markup.h"
#include "LayoutEncrypt.h"

typedef struct _css_property
{
	wstring name;
	wstring attr;
}CSS_PROPERTY;

typedef std::vector<CSS_PROPERTY> GTCSS;

class Cakcss
{
public:

	Cakcss(void)
	{
		LoadFromBuffer();
	}

	~Cakcss(void)
	{
		Clear();
	}
	
	static Cakcss* Inst()
	{
		static Cakcss akcss;
		return &akcss;
	}

public:
	const GTCSS * GetCSS(wstring name)
	{
		std::map<wstring, GTCSS*>::const_iterator itr = m_css.find( name );
		if( itr != m_css.end() )
		{
			return itr->second;
		}
		return NULL;
	}
	void LoadFromXML(){}
	bool LoadFromBuffer()
	{
		Clear();
		
		wstring strContent;
		void* buf = NULL;
		int len = 0;
		CDeComp::Inst()->ExtractToMem(CDeComp::Image, _T("akcss.gt"), &buf, &len);
		CLayoutEncrypt::DecodeMem((const unsigned char*)buf, len, strContent);
		CDeComp::Inst()->FreeBuffer(buf);

		CMarkup xml;
		if( !xml.SetDoc( strContent.c_str() ) ) 
		{
			return false;
		}
		if( !xml.FindElem() ) { // 顶层元素_T("ak_css")
			return false;
		}
		//双层结构，直接循环
		while(true)
		{
			if( !xml.FindChildElem() ) 
				break;
			xml.IntoElem();
			wstring winName = xml.GetTagName();
			while(true)
			{
				if( !xml.FindChildElem() ) 
					break;
				xml.IntoElem();
				wstring cssName = xml.GetTagName();
				cssName = winName + _T("_") + cssName;

				int idx = 0;
				CSS_PROPERTY tmp;
				GTCSS* css = new GTCSS;
				while(true)
				{
					tmp.name = xml.GetAttribName( idx );
					if( tmp.name.empty() ) {
						break;
					}
					tmp.attr = xml.GetAttrib( tmp.name );
					css->push_back(tmp);
					++idx;
				}
				if(!css->empty() && m_css.find(cssName) == m_css.end())
				{
					m_css.insert(std::map<wstring, GTCSS*>::value_type(cssName, css));
				}
				else
				{
					delete css;
					css = NULL;
				}
				xml.OutOfElem();
			}
			xml.OutOfElem();
		}
		return true;
	}
private:
	std::map<wstring, GTCSS*> m_css;
	void Clear()
	{
		std::map<wstring, GTCSS*>::iterator itr = m_css.begin();
		for( ; itr != m_css.end(); ++itr)
		{
			if(itr->second)
			{
				itr->second->clear();
				delete itr->second;
			}
		}
		m_css.clear();
	}

};
