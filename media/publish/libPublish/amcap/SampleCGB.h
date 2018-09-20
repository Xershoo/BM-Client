//------------------------------------------------------------------------------
// File: SampleCGB.h
//
// Desc: DirectShow sample code - definition of sample capture graph builder
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------
#include <Windows.h>
#include <WTypes.h>
#include <ks.h>


#ifndef KSCATEGORY_ENCODER
#define STATIC_KSCATEGORY_ENCODER\
    0x19689bf6, 0xc384, 0x48fd, 0xad, 0x51, 0x90, 0xe5, 0x8c, 0x79, 0xf7, 0xb
DEFINE_GUIDSTRUCT("19689BF6-C384-48fd-AD51-90E58C79F70B", KSCATEGORY_ENCODER);
#define KSCATEGORY_ENCODER DEFINE_GUIDNAMED(KSCATEGORY_ENCODER)
#endif

#ifndef KSCATEGORY_MULTIPLEXER
#define STATIC_KSCATEGORY_MULTIPLEXER\
    0x236C9559, 0xADCE, 0x4736, 0xBF, 0x72, 0xBA, 0xB3, 0x4E, 0x39, 0x21, 0x96
DEFINE_GUIDSTRUCT("236C9559-ADCE-4736-BF72-BAB34E392196", KSCATEGORY_MULTIPLEXER);
#define KSCATEGORY_MULTIPLEXER DEFINE_GUIDNAMED(KSCATEGORY_MULTIPLEXER)
#endif

#ifndef MEDIASUBTYPE_Mpeg2
#define STATIC_MEDIASUBTYPE_Mpeg2\
    0x7DC2C665, 0x4033, 0x4CAF, 0x92, 0x73, 0xF7, 0xD7, 0x97, 0xFB, 0x24, 0x5C
DEFINE_GUIDSTRUCT("7DC2C665-4033-4CAF-9273-F7D797FB245C", MEDIASUBTYPE_Mpeg2);
#define MEDIASUBTYPE_Mpeg2 DEFINE_GUIDNAMED(MEDIASUBTYPE_Mpeg2)
#endif

#ifndef CLSID_Dump
#define STATIC_CLSID_Dump\
    0x36a5f770, 0xfe4c, 0x11ce, 0xa8, 0xed, 0x00, 0xaa, 0x00, 0x2f, 0xea, 0xb5
DEFINE_GUIDSTRUCT("36a5f770-fe4c-11ce-a8ed-00aa002feab5", CLSID_Dump);
#define CLSID_Dump DEFINE_GUIDNAMED(CLSID_Dump)
#endif





class ISampleCaptureGraphBuilder
{

public:
    ISampleCaptureGraphBuilder();
	virtual ~ISampleCaptureGraphBuilder();

public:

    STDMETHOD(AllocCapFile)( LPCOLESTR lpwstr, DWORDLONG dwlSize );

    STDMETHOD(ControlStream)( const GUID *pCategory,
                          const GUID *pType,
                          IBaseFilter *pFilter,
                          REFERENCE_TIME *pstart,
                          REFERENCE_TIME *pstop,
                          WORD wStartCookie,
                          WORD wStopCookie
                          );

    STDMETHOD(FindInterface)(const GUID *pCategory,
                          const GUID *pType,
                          IBaseFilter *pf,
                          REFIID riid,
                          void **ppint
                          );

    STDMETHOD(FindPin)( IUnknown *pSource,
                      PIN_DIRECTION pindir,
                      const GUID *pCategory,
                      const GUID *pType,
                      BOOL fUnconnected,
                      int num,
                      IPin **ppPin
                      );


    STDMETHOD(RenderStream)( const GUID *pCategory,
                          const GUID *pType,
                          IUnknown *pSource,
                          IBaseFilter *pIntermediate,
                          IBaseFilter *pSink
                          );


    STDMETHOD(SetFiltergraph)( IGraphBuilder *pfg );


    STDMETHOD(SetOutputFileName)(
                                const GUID *pType,
                                LPCOLESTR lpwstrFile,
                                IBaseFilter **ppf,
                                IFileSinkFilter **pSink
                                );


protected:

    HRESULT FindPin( 
                IBaseFilter *pFilter, 
                const REGPINMEDIUM& regPinMedium, 
                PIN_DIRECTION direction, 
                BOOL video,             
                IPin **ppPin);

    HRESULT GetMedium( IPin *pPin, REGPINMEDIUM& regPinMedium );


    BOOL IsVideoPin( IPin *pPin );
    BOOL IsAudioPin( IPin *pPin );
    
    BOOL HasMediaType( IPin *pPin, REFGUID majorType );

protected:

	ICaptureGraphBuilder2 *graphBuilder2_;

};
