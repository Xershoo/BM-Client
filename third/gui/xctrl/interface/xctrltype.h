/*
 * wtlctrl_if.h
 *
 */
#pragma once

#include "xbase.h"
#include "xgdi/xgdi.h"


NAMESPACE_BEGIN(xctrl)


struct WndBorder
{
	WndBorder(){pix=0;clr=0;}
	WndBorder(int pix, COLORREF clr)
	{
			this->pix = pix;
			this->clr = clr;
	}
	WndBorder operator=(WndBorder* p)
	{
			if(p){
				pix = p->pix;
				clr = p->clr;
			}else{
				pix = 0;
				clr = 0;
			}
			return *this;
	}
	unsigned int pix;
	COLORREF	 clr;
};

NAMESPACE_END(xctrl)