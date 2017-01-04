﻿// TraceTool.cpp : Defines the class behaviors for the application.
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Classes Reference and related electronic
// documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft C++ Libraries products.

#include "stdafx.h"
#include "TraceTool.h"
#include "TraceToolDlg.h"
//////////////////////////////////////////////////////////////////////
//Resource (satellite) dll search and load routines. see LoadLocResDll
//an example.
//Note: Change made here must be sync with all other tools.

__inline
errno_t __cdecl DuplicateEnvString(TCHAR **ppszBuffer, size_t *pnBufferSizeInTChars, const TCHAR *pszVarName)
{
    /* validation section */
    if (ppszBuffer == NULL)
    {
        return  EINVAL;
    }
    *ppszBuffer = NULL;
    if (pnBufferSizeInTChars != NULL)
    {
        *pnBufferSizeInTChars = 0;
    }
    /* varname is already validated in getenv */
    TCHAR szDummyBuff[1] = {0};
    size_t nSizeNeeded = 0;
    errno_t ret=_tgetenv_s(&nSizeNeeded,szDummyBuff,1,pszVarName);
    if (nSizeNeeded > 0)
    {
        *ppszBuffer = new TCHAR[nSizeNeeded];
        if (*ppszBuffer != NULL)
        {
            size_t nSizeNeeded2 = 0;
            ret=_tgetenv_s(&nSizeNeeded2,*ppszBuffer,nSizeNeeded,pszVarName);
            if (nSizeNeeded2!=nSizeNeeded)
            {
                ret=ERANGE;
            }
            else if (pnBufferSizeInTChars != NULL)
            {
                *pnBufferSizeInTChars = nSizeNeeded;
            }
        }
        else
        {
            ret=ENOMEM;
        }
    }
    return ret;
}

#define _TCSNLEN(sz,c) (min(_tcslen(sz), c))
#define PATHLEFT(sz) (_MAX_PATH - _TCSNLEN(sz, (_MAX_PATH-1)) - 1)

typedef LANGID (WINAPI* PFNGETUSERDEFAULTUILANGUAGE)();

static BOOL CALLBACK _EnumResLangProc(HMODULE /*hModule*/, LPCTSTR /*pszType*/,
                                      LPCTSTR /*pszName*/, WORD langid, LONG_PTR lParam)
{
    if(lParam == NULL)
    {
        return FALSE;
    }

    LANGID* plangid = reinterpret_cast< LANGID* >( lParam );
    *plangid = langid;

    return TRUE;
}
//////////////////////////////////////////////////////////////////////////
//Purpose: GetUserDefaultUILanguage for downlevel platforms (Win9x, NT4).
//Input: szDllName - the string resource dll name to search. Ex: ToolUI.dll
//Output: TCHAR *szPathOut - filled with absolute path to dll, if found.
//		  size_t sizeInCharacters - buffer size in characters
//Returns: Success - HMODULE of found dll, Failure - NULL
//////////////////////////////////////////////////////////////////////////
HRESULT GetUserDefaultUILanguageLegacyCompat(LANGID* pLangid)
{
    HRESULT hr=E_FAIL;
    if (pLangid == NULL)
    {
        return E_POINTER;
    }
    PFNGETUSERDEFAULTUILANGUAGE pfnGetUserDefaultUILanguage;
    HINSTANCE hKernel32 = ::GetModuleHandle(_T("kernel32.dll"));
    pfnGetUserDefaultUILanguage = (PFNGETUSERDEFAULTUILANGUAGE)::GetProcAddress(hKernel32, "GetUserDefaultUILanguage");
    if(pfnGetUserDefaultUILanguage != NULL)
    {
        *pLangid = pfnGetUserDefaultUILanguage();
        hr = S_OK;
    }
    else
    {
        // We're not on an MUI-capable system.
        OSVERSIONINFO version;
        memset(&version, 0, sizeof(version));
        version.dwOSVersionInfoSize = sizeof(version);
        ::GetVersionEx(&version);
        if( version.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
        {
            // We're on Windows 9x, so look in the registry for the UI language
            HKEY hKey = NULL;
            LONG nResult = ::RegOpenKeyEx(HKEY_CURRENT_USER,
                                          _T( "Control Panel\\Desktop\\ResourceLocale" ), 0, KEY_READ, &hKey);
            if (nResult == ERROR_SUCCESS)
            {
                DWORD dwType;
                TCHAR szValue[16];
                ULONG nBytes = sizeof( szValue );
                nResult = ::RegQueryValueEx(hKey, NULL, NULL, &dwType, LPBYTE( szValue ),
                                            &nBytes );
                if ((nResult == ERROR_SUCCESS) && (dwType == REG_SZ))
                {
                    DWORD dwLangID;
                    int nFields = _stscanf_s( szValue, _T( "%x" ), &dwLangID );
                    if( nFields == 1 )
                    {
                        *pLangid = LANGID( dwLangID );
                        hr = S_OK;
                    }
                }

                ::RegCloseKey(hKey);
            }
        }
        else
        {
            // We're on NT 4.  The UI language is the same as the language of the version
            // resource in ntdll.dll
            HMODULE hNTDLL = ::GetModuleHandle( _T( "ntdll.dll" ) );
            if (hNTDLL != NULL)
            {
                *pLangid = 0;
                ::EnumResourceLanguages( hNTDLL, RT_VERSION, MAKEINTRESOURCE( 1 ),
                                         _EnumResLangProc, reinterpret_cast< LONG_PTR >( pLangid ) );
                if (*pLangid != 0)
                {
                    hr = S_OK;
                }
            }
        }
    }
    return hr;
}

//////////////////////////////////////////////////////////////////////////
//Purpose: Searches for a resource dll in sub directories using a search order
//		   based on szPath - a directory to search res dll below.
//		   see example at .
//Input: szDllName - the string resource dll name to search. Ex: ToolUI.dll
//Output: TCHAR *szPathOut - filled with absolute path to dll, if found.
//		  size_t sizeInCharacters - buffer size in characters
//Returns: Success (found dll) - S_OK , Failure - E_FAIL or E_UNEXPECTED
//////////////////////////////////////////////////////////////////////////
HRESULT LoadUILibrary(LPCTSTR szPath, LPCTSTR szDllName, DWORD dwExFlags,
                      HINSTANCE *phinstOut, LPTSTR szFullPathOut,size_t sizeInCharacters,
                      LCID *plcidOut)
{
    TCHAR szPathTemp[_MAX_PATH + 1] = _T("");
    HRESULT hr = E_FAIL;
    LCID lcidFound = (LCID)-1;
    size_t nPathEnd = 0;

    // Gotta have this stuff!
    if (szPath==NULL || *szPath == '\0')
    {
        return E_POINTER;
    }

    if (szDllName==NULL || *szDllName == '\0')
    {
        return E_POINTER;
    }

    if (!szPath || !*szPath || !szDllName || !*szDllName)
    {
        return E_INVALIDARG;
    }

    if (phinstOut != NULL)
    {
        *phinstOut = NULL;
    }

    szPathTemp[_MAX_PATH-1] = L'\0';

    // Add \ to the end if necessary
    _tcsncpy_s(szPathTemp,_countof(szPathTemp), szPath, _TRUNCATE);
    if (szPathTemp[_TCSNLEN(szPathTemp, _MAX_PATH-1) - 1] != L'\\')
    {
        _tcsncat_s(szPathTemp,_countof(szPathTemp), _T("\\"), PATHLEFT(szPathTemp));
    }

    // Check if given path even exists
    if (GetFileAttributes(szPathTemp) == 0xFFFFFFFF)
    {
        return E_FAIL;
    }

    nPathEnd = _TCSNLEN(szPathTemp, _MAX_PATH-1);

    {
        LANGID langid=0;
        if (FAILED(GetUserDefaultUILanguageLegacyCompat(&langid)))
        {
            return E_UNEXPECTED;
        }
        const LCID lcidUser = MAKELCID(langid, SORT_DEFAULT);

        LCID rglcid[3];
        rglcid[0] = lcidUser;
        rglcid[1] = MAKELCID(MAKELANGID(PRIMARYLANGID(lcidUser), SUBLANG_DEFAULT), SORTIDFROMLCID(lcidUser));
        rglcid[2] = 0x409;
        for (int i = 0; i < _countof(rglcid); i++)
        {
            TCHAR szNumBuf[10];

            // Check if it's the same as any LCID already checked,
            // which is very possible
            int n = 0;
            for (n = 0; n < i; n++)
            {
                if (rglcid[n] == rglcid[i])
                    break;
            }

            if (n < i)
            {
                continue;
            }

            szPathTemp[nPathEnd] = L'\0';
            _itot_s(rglcid[i], szNumBuf,_countof(szNumBuf), 10);
            _tcsncat_s(szPathTemp, _countof(szPathTemp),szNumBuf , PATHLEFT(szPathTemp));
            _tcsncat_s(szPathTemp,_countof(szPathTemp), _T("\\"), PATHLEFT(szPathTemp));
            _tcsncat_s(szPathTemp,_countof(szPathTemp), szDllName, PATHLEFT(szPathTemp));

            if (GetFileAttributes(szPathTemp) != 0xFFFFFFFF)
            {
                lcidFound = rglcid[i];

                hr = S_OK;
                goto Done;
            }
        }
    }

    // None of the default choices exists, so now look for the dll in a folder below
    //the given path (szPath)
    {
        szPathTemp[nPathEnd] = L'\0';
        _tcsncat_s(szPathTemp,_countof(szPathTemp), _T("*.*"), PATHLEFT(szPathTemp));

        WIN32_FIND_DATA wfdw;
        HANDLE hDirs = FindFirstFile(szPathTemp, &wfdw);
        nPathEnd = _TCSNLEN(szPathTemp, _MAX_PATH-1)-3;
        if (hDirs != INVALID_HANDLE_VALUE)
        {
            while (FindNextFile(hDirs, &wfdw))
            {
                // We are only interested in directories, since at this level, that should
                // be the only thing in this directory, i.e, LCID sub dirs
                if (wfdw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    // Skip current and previous dirs, "." and ".."
                    if (!_tcscmp(wfdw.cFileName, _T(".")) || !_tcscmp(wfdw.cFileName, _T("..")))
                        continue;

                    // Does this dir have a copy of the dll?
                    szPathTemp[nPathEnd] = L'\0';
                    _tcsncat_s(szPathTemp,_countof(szPathTemp), wfdw.cFileName, PATHLEFT(szPathTemp));
                    _tcsncat_s(szPathTemp,_countof(szPathTemp), _T("\\"), PATHLEFT(szPathTemp));
                    _tcsncat_s(szPathTemp,_countof(szPathTemp), szDllName, PATHLEFT(szPathTemp));

                    if (GetFileAttributes(szPathTemp) != 0xFFFFFFFF)
                    {
                        // Got it!
                        lcidFound = (LCID)_tstol(wfdw.cFileName);

                        hr = S_OK;
                        break;
                    }
                }
            }

            FindClose(hDirs);
        }
    }

Done:
    if (SUCCEEDED(hr))
    {
        // Set the default LCID
        if (plcidOut)
        {
            if (lcidFound == (LCID)-1)
            {
                return E_UNEXPECTED;
            }
            *plcidOut = lcidFound;
        }

        // Finally, attempt to load the library
        // Beware!  A dll loaded with LOAD_LIBRARY_AS_DATAFILE won't
        // let you use LoadIcon and things like that (only general calls like
        // FindResource and LoadResource).
        if (phinstOut != NULL)
        {
            *phinstOut = LoadLibraryEx(szPathTemp, NULL, dwExFlags);
            hr = (*phinstOut) ? S_OK : E_FAIL;
        }
        if ( szFullPathOut )
        {
            _tcsncpy_s(szFullPathOut,sizeInCharacters, szPathTemp, _MAX_PATH-1);
        }
    }

    return hr;
}
//////////////////////////////////////////////////////////////////////////
//Purpose: Iterates env("PATH") directories to try to find (using LoadUILibrary)
//		   resource dll a directory below PATH dirs. Ex: if PATH="c:\bin;d:\win"
//		   and szDllName="ToolUI.dll", then the first of c:\bin\1033\ToolUI.dll
//		   and d:\win\SomeFolder\ToolUI.dll will be loaded.
//		   See LoadLocResDll doc (below) for example.
//Input: szDllName - the string resource dll name to search. Ex: ToolUI.dll
//Output: TCHAR *szPathOut - filled with absolute path to dll, if found.
//		  size_t sizeInCharacters - buffer size in characters
//Returns: Success - HMODULE of found dll, Failure - NULL
//////////////////////////////////////////////////////////////////////////
HMODULE LoadSearchPath(LPCTSTR szDllName,TCHAR *szPathOut, size_t sizeInCharacters)
{
    TCHAR * szEnvPATH = NULL;
    TCHAR * szEnvPATHBuff = NULL;
    int nPathLen = 0;
    int nPathIndex = 0;
    HMODULE hmod = NULL;
    if (DuplicateEnvString(&szEnvPATHBuff,NULL,_T("PATH"))==0 && (szEnvPATH=szEnvPATHBuff) != NULL)
    {
        while (*szEnvPATH)
        {
            /* skip leading white space and nop semicolons */
            for (; *szEnvPATH == L' ' || *szEnvPATH == L';'; ++szEnvPATH)
            {} /* NOTHING */

            if (*szEnvPATH == L'\0')
            {
                break;
            }

            ++nPathIndex;

            /* copy this chunk of the path into our trypath */
            nPathLen = 0;
            TCHAR szPath[_MAX_PATH+1];
            TCHAR * pszTry = NULL;
            for (pszTry = szPath; *szEnvPATH != L'\0' && *szEnvPATH != L';'; ++szEnvPATH)
            {
                ++nPathLen;
                if (nPathLen < _MAX_PATH)
                {
                    *pszTry++ = *szEnvPATH;
                }
                else
                {
                    break;
                }
            }
            *pszTry = L'\0';

            if (nPathLen == 0 || nPathLen >= _MAX_PATH)
            {
                continue;
            }

            LoadUILibrary(szPath, szDllName, LOAD_LIBRARY_AS_DATAFILE,
                          &hmod, szPathOut,sizeInCharacters, NULL);
            if ( hmod )
            {
                break;
            }
        }
    }
    if (szEnvPATHBuff!=NULL)
    {
        delete [] szEnvPATHBuff;
    }
    return hmod;
}
//Example: Say PATH="c:\bin;d:\win", resource dll name (szDllName) is "ToolUI.dll",
//		   user locale is 936, and the .exe calling LoadLocResDll is c:\MyTools\Tool.exe
//			Search order:
//			a) c:\MyTools\936\ToolUI.dll (exe path + user default UI lang)
//			b) c:\MyTools\1033 (same with eng)
//			c) c:\MyTools\*\ToolUI.dll (where * is sub folder).
//			d) c:\bin\936\ToolUI.dll (first in path)
//			e) c:\bin\1033\ToolUI.dll (first in path + eng)
//			f) c:\bin\*\ToolUI.dll
//			g) d:\win\936\ToolUI.dll  (second in path)
//			h) d:\win\1033\ToolUI.dll (second in path + eng)
//			i) d:\win\*\ToolUI.dll (second in path + eng)
//			j) if bExeDefaultModule and not found, return exe HINSTANCE.
//			Note: The primary lang (without the sublang) is tested after the user ui lang.
// Main Input: szDllName - the name of the resource dll <ToolName>ui.dll. Ex: vcdeployUI.dll
// Main Output: HMODULE of resource dll or NULL - if not found (see bExeDefaultModule).
HMODULE LoadLocResDll(LPCTSTR szDllName,BOOL bExeDefaultModule=TRUE,DWORD dwExFlags=LOAD_LIBRARY_AS_DATAFILE,LPTSTR pszPathOut = NULL,size_t sizeInCharacters = 0  )
{
    HMODULE hmod = NULL;
    TCHAR driverpath[_MAX_PATH + 1], exepath[_MAX_PATH + 1];
    LPTSTR p = NULL;

    GetModuleFileName(GetModuleHandle(NULL), driverpath, _MAX_PATH);

    // find path of tool
    ATLENSURE(AtlIsValidString(driverpath, _MAX_PATH-1));
    p = driverpath + _TCSNLEN(driverpath, _MAX_PATH-1)-1;

    while ( *p != L'\\' && p != driverpath)
    {
        p--;
    }
    *p = '\0';

    LoadUILibrary(driverpath, szDllName, dwExFlags,
                  &hmod, exepath,_countof(exepath), NULL);

    if ( hmod == NULL )
    {
        // search PATH\<lcid> for <ToolName>ui.dll
        hmod = LoadSearchPath(szDllName,exepath,_countof(exepath));
    }

    if ( hmod && pszPathOut )
    {
        _tcsncpy_s(pszPathOut,sizeInCharacters, exepath, _MAX_PATH-1);
    }
    //Not found dll, return the exe HINSTANCE as a fallback.
    if (hmod == NULL && bExeDefaultModule)
    {
        hmod=GetModuleHandle(NULL);
    }
    return hmod;
}
//End loc routines
////////////////////////////////////////////////////////////////////
const TCHAR* szTraceToolUIDll = _T("AtlTraceToolUI.dll");



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CTraceToolApp

BEGIN_MESSAGE_MAP(CTraceToolApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTraceToolApp construction

CTraceToolApp::CTraceToolApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
    EnableHtmlHelp();
}


// The one and only CTraceToolApp object

CTraceToolApp theApp;


// CTraceToolApp initialization

static const LPCTSTR g_pszEventName = _T( "ATLTraceTool_Instance" );

BOOL CTraceToolApp::InitInstance()
{
    // Turn on the "termination on heap corruption" flag.
    (void)HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    // Check to see if another instance of the tool is running
    BOOL bCreated = m_hSingleInstanceEvent.Create( NULL, FALSE, FALSE, g_pszEventName );
    if( !bCreated )
    {
        return( FALSE );
    }
    HINSTANCE hInstRes=LoadLocResDll(szTraceToolUIDll,TRUE,0); //Do not load resource dll as data file
    AfxSetResourceHandle(hInstRes);
    if( ::GetLastError() == ERROR_ALREADY_EXISTS )
    {
        m_hSingleInstanceEvent.Set();  // Notify the original instance
        return( FALSE );
    }

    CWinApp::InitInstance();


    CTraceToolDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK)
    {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with OK
    }
    else if (nResponse == IDCANCEL)
    {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with Cancel
    }

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}
