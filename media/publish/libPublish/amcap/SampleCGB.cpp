//------------------------------------------------------------------------------
// File: SampleCGB.cpp
//
// Desc: DirectShow sample code - Sample capture graph builder class
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "SampleCGB.h"
#include "ksproxy.h"

ISampleCaptureGraphBuilder::ISampleCaptureGraphBuilder()
{
	HRESULT hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, 
		IID_ICaptureGraphBuilder2, (void**)&graphBuilder2_ ); 
	ASSERT( S_OK == hr );
}

ISampleCaptureGraphBuilder::~ISampleCaptureGraphBuilder()
{
	if(graphBuilder2_)
	{
		graphBuilder2_->Release();
		graphBuilder2_ = NULL;
	}
}

HRESULT 
ISampleCaptureGraphBuilder::AllocCapFile( LPCOLESTR lpwstr, DWORDLONG dwlSize )
{
    return graphBuilder2_->AllocCapFile( lpwstr, dwlSize );
}


HRESULT 
ISampleCaptureGraphBuilder::ControlStream( const GUID *pCategory,
                                          const GUID *pType,
                                          IBaseFilter *pFilter,
                                          REFERENCE_TIME *pstart,
                                          REFERENCE_TIME *pstop,
                                          WORD wStartCookie,
                                          WORD wStopCookie )
{
    return graphBuilder2_->ControlStream( pCategory, pType, pFilter, 
                                          pstart, pstop, wStartCookie, wStopCookie );
}


HRESULT ISampleCaptureGraphBuilder::FindInterface(const GUID *pCategory,
                                                  const GUID *pType,
                                                  IBaseFilter *pf,
                                                  REFIID riid,
                                                  void **ppint
                                                  )
{
    return graphBuilder2_->FindInterface( pCategory, pType, pf, riid, ppint );
}

HRESULT 
ISampleCaptureGraphBuilder::FindPin( IUnknown *pSource,
                                      PIN_DIRECTION pindir,
                                      const GUID *pCategory,
                                      const GUID *pType,
                                      BOOL fUnconnected,
                                      int num,
                                      IPin **ppPin)
{
    return graphBuilder2_->FindPin( pSource, pindir, pCategory, pType, 
                                    fUnconnected, num, ppPin ); 
}


HRESULT 
ISampleCaptureGraphBuilder::RenderStream( const GUID *pCategory,
                                          const GUID *pType,
                                          IUnknown *pSource,
                                          IBaseFilter *pIntermediate,
                                          IBaseFilter *pSink)
{
	return graphBuilder2_->RenderStream( pCategory, pType, pSource, pIntermediate, pSink );
}


HRESULT 
ISampleCaptureGraphBuilder::SetFiltergraph( IGraphBuilder *pfg )
{
    return graphBuilder2_->SetFiltergraph( pfg );
}


HRESULT 
ISampleCaptureGraphBuilder::SetOutputFileName(  const GUID *pType,
                                                LPCOLESTR lpwstrFile,
                                                IBaseFilter **ppf,
                                                IFileSinkFilter **pSink )
{
    if( ! pType || ! lpwstrFile || !ppf || !pSink )
    {
        return E_INVALIDARG;
    }

    return graphBuilder2_->SetOutputFileName(pType, lpwstrFile, ppf, pSink );
}


BOOL ISampleCaptureGraphBuilder::IsVideoPin( IPin *pPin )
{
    return HasMediaType( pPin, MEDIATYPE_Video );
}

//
//  search the encoder that has this special medium
//  video == TRUE -- look for a video pin
//  video == FALSE -- look for a audio pin  
//
HRESULT ISampleCaptureGraphBuilder::FindPin( 
            IBaseFilter *pFilter, 
            const REGPINMEDIUM& regPinMedium, 
            PIN_DIRECTION direction, 
            BOOL video,             
            IPin **ppPin)
{
    if( !pFilter )
    {
        return E_POINTER;
    }

    SmartPtr<IEnumPins> pEnumPins;
    HRESULT hr = pFilter->EnumPins( &pEnumPins );
    if( FAILED( hr ) )
    {
        return hr;
    }
    
    SmartPtr<IPin>   pTempPin;
    ULONG           fetched;
    REGPINMEDIUM    regPinMediumTemp;
    PIN_DIRECTION   dir;

    hr = pEnumPins->Reset( );
    while( pTempPin.Release(), S_OK == pEnumPins->Next( 1, &pTempPin, &fetched ) )
    {
        ASSERT( pTempPin );

        hr = pTempPin->QueryDirection( &dir );
        if( FAILED( hr ) || dir != direction )
        {
            continue;
        }

        hr = GetMedium( pTempPin, regPinMediumTemp );
        if( FAILED( hr ) )
        {
            continue;
        }

        if( !IsVideoPin( pTempPin ) )
        {
            continue;
        }

        if( ::IsEqualGUID( regPinMediumTemp.clsMedium, regPinMedium.clsMedium  ) &&
            regPinMediumTemp.dw1 == regPinMedium.dw1 )
        {
            (*ppPin) = pTempPin.Detach();
            return S_OK;
        }
    }

    return E_FAIL;
}

//
//  Get a special medium from this pin.
//  If there is not one, return GUID_NULL.
//  Returns the first one it finds special
//

HRESULT 
ISampleCaptureGraphBuilder::GetMedium( 
                        IPin *pPin, 
                        REGPINMEDIUM& regPinMedium )
{
    if( !pPin )
    {
        return E_POINTER;
    }

    SmartPtr<IKsPin> pKsPin;
    HRESULT hr = pPin->QueryInterface(IID_IKsPin, (void**)&pKsPin );
    if( FAILED( hr ) )
    {
        return hr;
    }

    PKSMULTIPLE_ITEM pmi;
    hr = pKsPin->KsQueryMediums( &pmi );
    if( FAILED( hr ) )
    {
        return hr;
    }
    
    REGPINMEDIUM *pMedium = (REGPINMEDIUM *)(pmi + 1);
    for( ULONG i  = 0; i < pmi->Count; i++ )
    {
        if( !::IsEqualGUID( pMedium->clsMedium, GUID_NULL ) &&
            !::IsEqualGUID( pMedium->clsMedium, KSMEDIUMSETID_Standard )
        )
        {
            regPinMedium.clsMedium = pMedium->clsMedium;
            regPinMedium.dw1 = pMedium->dw1;
            regPinMedium.dw2 = pMedium->dw2;
            CoTaskMemFree( pmi );
            return S_OK;
        }
    }

    regPinMedium.clsMedium = GUID_NULL;
    regPinMedium.dw1 = 0;
    regPinMedium.dw2 = 0;
    CoTaskMemFree( pmi );
    return S_OK;
}



BOOL ISampleCaptureGraphBuilder::IsAudioPin( IPin *pPin )
{
    if( !pPin )
    {
        return FALSE;   // NULL pointer
    }

    return HasMediaType( pPin, MEDIATYPE_Audio) ;
}


BOOL ISampleCaptureGraphBuilder::HasMediaType(IPin *pPin,  REFGUID majorType )
{
    if( !pPin )
    {
        return FALSE;
    }

    SmartPtr<IEnumMediaTypes> pMediaTypes;
    HRESULT hr = pPin->EnumMediaTypes( &pMediaTypes );
    if( FAILED( hr ) )
    {
        return FALSE;
    }

    hr = pMediaTypes->Reset();
    if( FAILED( hr ) )
    {
        return FALSE;
    }

    ULONG           fetched;
    AM_MEDIA_TYPE   *mediaType;

    while( S_OK == pMediaTypes->Next( 1, &mediaType, &fetched ) )
    {
        if( ::IsEqualGUID( mediaType->majortype, majorType ) )
        {
            DeleteMediaType( mediaType );
            return TRUE;
        }
        DeleteMediaType( mediaType );
    }

    return FALSE;

}


