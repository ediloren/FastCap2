

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Tue Aug 18 11:57:04 2015
 */
/* Compiler settings for FastCap2.odl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __FastCap2_h_h__
#define __FastCap2_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IFastCap_FWD_DEFINED__
#define __IFastCap_FWD_DEFINED__
typedef interface IFastCap IFastCap;

#endif 	/* __IFastCap_FWD_DEFINED__ */


#ifndef __Document_FWD_DEFINED__
#define __Document_FWD_DEFINED__

#ifdef __cplusplus
typedef class Document Document;
#else
typedef struct Document Document;
#endif /* __cplusplus */

#endif 	/* __Document_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __FastCap_LIBRARY_DEFINED__
#define __FastCap_LIBRARY_DEFINED__

/* library FastCap */
/* [version][uuid] */ 


DEFINE_GUID(LIBID_FastCap,0xA51B7596,0x8D71,0x41A3,0x96,0xBE,0xFD,0x0A,0x00,0xB5,0x65,0xFA);

#ifndef __IFastCap_DISPINTERFACE_DEFINED__
#define __IFastCap_DISPINTERFACE_DEFINED__

/* dispinterface IFastCap */
/* [uuid] */ 


DEFINE_GUID(DIID_IFastCap,0x4B8E95F2,0x2399,0x4D1D,0xBA,0xB8,0xC1,0x8A,0x09,0x22,0x3D,0x5E);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("4B8E95F2-2399-4D1D-BAB8-C18A09223D5E")
    IFastCap : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IFastCapVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFastCap * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFastCap * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFastCap * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IFastCap * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IFastCap * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IFastCap * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IFastCap * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } IFastCapVtbl;

    interface IFastCap
    {
        CONST_VTBL struct IFastCapVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFastCap_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFastCap_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFastCap_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFastCap_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IFastCap_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IFastCap_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IFastCap_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IFastCap_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_Document,0x1ECCFB33,0x710A,0x4B5C,0x92,0x95,0xA5,0x03,0xCA,0x2D,0x10,0xE2);

#ifdef __cplusplus

class DECLSPEC_UUID("1ECCFB33-710A-4B5C-9295-A503CA2D10E2")
Document;
#endif
#endif /* __FastCap_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


