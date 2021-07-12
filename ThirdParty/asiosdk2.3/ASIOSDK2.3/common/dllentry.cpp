//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1996  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;

//
// classes used to support dll entrypoints for COM objects.
//
// #include "switches.h"

#include <windows.h>
#include "wxdebug.h"
#include "combase.h"
#ifdef DEBUG
#include <tchar.h>
#endif

#include <stdio.h>

extern CFactoryTemplate g_Templates[];
extern int g_cTemplates;

HINSTANCE hinstance = 0;
DWORD	  g_amPlatform;		// VER_PLATFORM_WIN32_WINDOWS etc... (from GetVersionEx)
OSVERSIONINFO g_osInfo;

//
// an instance of this is created by the DLLGetClassObject entrypoint
// it uses the CFactoryTemplate object it is given to support the
// IClassFactory interface

class CClassFactory : public IClassFactory
{

private:
    const CFactoryTemplate * m_pTemplate;

    ULONG m_cRef;

    static int m_cLocked;
public:
    CClassFactory(const CFactoryTemplate *);

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void ** ppv);
    STDMETHODIMP_(ULONG)AddRef();
    STDMETHODIMP_(ULONG)Release();

    // IClassFactory
    STDMETHODIMP CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, void **pv);
    STDMETHODIMP LockServer(BOOL fLock);

    // allow DLLGetClassObject to know about global server lock status
    static BOOL IsLocked() {
        return (m_cLocked > 0);
    };
};

// process-wide dll locked state
int CClassFactory::m_cLocked = 0;

CClassFactory::CClassFactory(const CFactoryTemplate *pTemplate)
{
    m_cRef = 0;
    m_pTemplate = pTemplate;
}


STDMETHODIMP CClassFactory::QueryInterface(REFIID riid,void **ppv)
{
    CheckPointer(ppv,E_POINTER)
    ValidateReadWritePtr(ppv,sizeof(PVOID));
    *ppv = NULL;

    // any interface on this object is the object pointer.
    if ((riid == IID_IUnknown) || (riid == IID_IClassFactory)) {
        *ppv = (LPVOID) this;
	// AddRef returned interface pointer
        ((LPUNKNOWN) *ppv)->AddRef();
        return NOERROR;
    }

    return ResultFromScode(E_NOINTERFACE);
}


STDMETHODIMP_(ULONG) CClassFactory::AddRef()
{
    return ++m_cRef;
}

STDMETHODIMP_(ULONG) CClassFactory::Release()
{
	LONG	rc;

    if (--m_cRef == 0) {
		delete this;
		rc = 0;
    } else rc = m_cRef;

	return rc;
}

STDMETHODIMP CClassFactory::CreateInstance(LPUNKNOWN pUnkOuter,REFIID riid,void **pv)
{
	CheckPointer(pv,E_POINTER)
    ValidateReadWritePtr(pv,sizeof(void *));

    /* Enforce the normal OLE rules regarding interfaces and delegation */

    if (pUnkOuter != NULL) {
        if (IsEqualIID(riid,IID_IUnknown) == FALSE) {
            return ResultFromScode(E_NOINTERFACE);
        }
    }

    /* Create the new object through the derived class's create function */

    HRESULT hr = NOERROR;
    CUnknown *pObj = m_pTemplate->CreateInstance(pUnkOuter, &hr);

    if (pObj == NULL) {
        return E_OUTOFMEMORY;
    }

    /* Delete the object if we got a construction error */

	if (FAILED(hr)) {
		delete pObj;
      return hr;
   }

    /* Get a reference counted interface on the object */

    /* We wrap the non-delegating QI with NDAddRef & NDRelease. */
    /* This protects any outer object from being prematurely    */
    /* released by an inner object that may have to be created  */
    /* in order to supply the requested interface.              */
    pObj->NonDelegatingAddRef();
    hr = pObj->NonDelegatingQueryInterface(riid, pv);
    pObj->NonDelegatingRelease();
    /* Note that if NonDelegatingQueryInterface fails, it will  */
    /* not increment the ref count, so the NonDelegatingRelease */
    /* will drop the ref back to zero and the object will "self-*/
    /* destruct".  Hence we don't need additional tidy-up code  */
    /* to cope with NonDelegatingQueryInterface failing.        */

    if (SUCCEEDED(hr)) {
        ASSERT(*pv);
    }

    return hr;
}

STDMETHODIMP CClassFactory::LockServer(BOOL fLock)
{
    if (fLock) {
        m_cLocked++;
    } else {
        m_cLocked--;
    }
    return NOERROR;
}


// --- COM entrypoints -----------------------------------------
// DllRegisterServer

//called by COM to get the class factory object for a given class
STDAPI DllGetClassObject(REFCLSID rClsID,REFIID riid,void **pv)
{
	// DebugBreak();

    if (!(riid == IID_IUnknown) && !(riid == IID_IClassFactory)) {
            return E_NOINTERFACE;
    }

    // traverse the array of templates looking for one with this
    // class id
    for (int i = 0; i < g_cTemplates; i++) {
        const CFactoryTemplate * pT = &g_Templates[i];
        if (pT->IsClassID(rClsID)) {

            // found a template - make a class factory based on this
            // template

            *pv = (LPVOID) (LPUNKNOWN) new CClassFactory(pT);
            if (*pv == NULL) {
                return E_OUTOFMEMORY;
            }
            ((LPUNKNOWN)*pv)->AddRef();
            return NOERROR;
        }
    }
    return CLASS_E_CLASSNOTAVAILABLE;
}

//
//  Call any initialization routines
//
void DllInitClasses(BOOL bLoading)
{
   int i;

	// DebugBreak();

	// traverse the array of templates calling the init routine
   // if they have one
   for (i = 0; i < g_cTemplates; i++) {
		const CFactoryTemplate * pT = &g_Templates[i];
      if (pT->m_lpfnInit != NULL) {
			(*pT->m_lpfnInit)(bLoading, pT->m_ClsID);
      }
   }

}

// called by COM to determine if this dll can be unloaded
// return ok unless there are outstanding objects or a lock requested
// by IClassFactory::LockServer
//
// CClassFactory has a static function that can tell us about the locks,
// and CCOMObject has a static function that can tell us about the active
// object count
STDAPI DllCanUnloadNow()
{
	// DebugBreak();

	DbgLog((LOG_MEMORY,2,TEXT("DLLCanUnloadNow called - IsLocked = %d, Active objects = %d"),
        CClassFactory::IsLocked(),
        CBaseObject::ObjectsActive()));

   if (CClassFactory::IsLocked() || CBaseObject::ObjectsActive()) {
	
		return S_FALSE;
   } 
	else {
		return S_OK;
   }
}


// --- standard WIN32 entrypoints --------------------------------------


//extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);
//BOOL WINAPI DllEntryPoint(HINSTANCE hInstance,ULONG ulReason,LPVOID pv)
//BOOL WINAPI DllMain (HINSTANCE hInstance,ULONG ulReason,LPVOID pv)
BOOL WINAPI DllEntryPoint (HINSTANCE hInstance,ULONG ulReason,LPVOID pv)
{

	// DebugBreak();
   
	switch (ulReason) {
	
		case DLL_PROCESS_ATTACH:
			DisableThreadLibraryCalls(hInstance);
			DbgInitialise(hInstance);
			{
				// The platform identifier is used to work out whether
				// full unicode support is available or not.  Hence the
				// default will be the lowest common denominator - i.e. N/A
            g_amPlatform = VER_PLATFORM_WIN32_WINDOWS; // win95 assumed in case GetVersionEx fails

            g_osInfo.dwOSVersionInfoSize = sizeof(g_osInfo);
            if (GetVersionEx(&g_osInfo)) {
        			g_amPlatform = g_osInfo.dwPlatformId;
				} 
				else {
					DbgLog((LOG_ERROR, 1, "Failed to get the OS platform, assuming Win95"));
				}
			}
			hinstance = hInstance;
			DllInitClasses(TRUE);
			
			break;

		case DLL_PROCESS_DETACH:
			DllInitClasses(FALSE);

#ifdef DEBUG
			if (CBaseObject::ObjectsActive()) {
				DbgSetModuleLevel(LOG_MEMORY, 2);
            TCHAR szInfo[512];
            extern TCHAR m_ModuleName[];     // Cut down module name

            TCHAR FullName[_MAX_PATH];      // Load the full path and module name
            TCHAR *pName;                   // Searches from the end for a backslash

            GetModuleFileName(NULL,FullName,_MAX_PATH);
            pName = _tcsrchr(FullName,'\\');
            if (pName == NULL) {
                pName = FullName;
            } 
				else {
					pName++;
            }

				DWORD cch = wsprintf(szInfo, TEXT("Executable: %s  Pid %x  Tid %x. "),
					pName, GetCurrentProcessId(), GetCurrentThreadId());

            wsprintf(szInfo+cch, TEXT("Module %s, %d objects left active!"),
                     m_ModuleName, CBaseObject::ObjectsActive());
            DbgAssert(szInfo, TEXT(__FILE__),__LINE__);

				// If running remotely wait for the Assert to be acknowledged
				// before dumping out the object register
            DbgDumpObjectRegister();
			}
			DbgTerminate();
#endif
			break;
    }
    return TRUE;
}


