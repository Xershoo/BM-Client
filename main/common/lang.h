#ifndef LANG_SET_H
#define LANG_SET_H

namespace LangSet
{
    enum
    {        
        LANG_CHA=0,
        LANG_ENG,

		LANG_UNKNOW,
    };

    int     getSystemLang();
    bool    saveLangSet(const char * keyReg,int langId);
    int     loadLangSet(const char* keyReg);
};

#endif