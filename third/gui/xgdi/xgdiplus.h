#pragma once

#include "xbase.h"
#include <gdiplus.h>

NAMESPACE_BEGIN(xgdi)

using namespace Gdiplus;

class CXGdiplus
{
public:
    CXGdiplus()
    {
        Status ret = GdiplusStartup(&m_pGdiToken, &m_gdiplusStartupInput, NULL);
        ATLASSERT(ret == Ok);
    }

    ~CXGdiplus()
    {
        GdiplusShutdown(m_pGdiToken);
    }

private:
    GdiplusStartupInput                 m_gdiplusStartupInput;
    ULONG_PTR                           m_pGdiToken;
};

NAMESPACE_END(xgdi)