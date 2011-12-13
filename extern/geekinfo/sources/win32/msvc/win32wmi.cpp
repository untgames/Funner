/*
  win32wmi.cpp

  Copyright (c) 2006-2009 Primate Labs

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include "win32wmi.h"
#define _WIN32_DCOM
#include <windows.h>
#include <comdef.h>
#include <wbemidl.h>

#include <sstream>
#include <iostream>

static IWbemServices *  pSvc = NULL;
static IWbemServices *  pSvcSmbios = NULL;
static IWbemLocator *   pLoc = NULL;
static bool             wmiInitialized = false;

static bool initializeWmi()
{
  HRESULT            result;

  result = CoInitialize(NULL);
  if (FAILED(result)) {
    return false;
  }

  result = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL); 
  if (FAILED(result)) {
    CoUninitialize();
    return false;
  }

  result = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&pLoc);
  if (FAILED(result)) {
    CoUninitialize();
    return false;
  }

  result = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pSvc);
  if (FAILED(result)) {
    CoUninitialize();
    return false;
  }

  result = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
  if (FAILED(result)) {
    CoUninitialize();
    return false;
  }
  
  result = pLoc->ConnectServer(_bstr_t(L"ROOT\\WMI"), NULL, NULL, 0, NULL, 0, 0, &pSvcSmbios);
  if (FAILED(result)) {
    CoUninitialize();
    return false;
  }
  
  result = CoSetProxyBlanket(pSvcSmbios, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
  if (FAILED(result)) {
    CoUninitialize();
    return false;
  }

  
  return true;
}

static std::string asciiEncode(const std::wstring & w)
{
  std::ostringstream  s;
  char *              c;
  int         r;

  c = new char[ MB_CUR_MAX + 1 ];
  for (size_t i = 0; i < w.length(); i++) {
    r = wctomb(c, w[i]);
    c[r] = '\0';
    if (r <= 1 && c[0] > 0) {
      s << c;
    }
  }
  return s.str();
}

static std::wstring asciiDecode(const std::string & s)
{
  std::wostringstream    w;
  wchar_t                c;

  for (size_t i = 0; i < s.length(); i++) {
    mbtowc(&c, &s[i], 1);
    w << c;
  }
  return w.str();
}

static std::string trim(const std::string & s)
{
  std::string::size_type  p;
  std::string::size_type  q;

  p = s.find_first_not_of(' ');
  if (p == std::string::npos) {
    p = 0;
  }
  q = s.find_last_not_of(' ');
  if (q == std::string::npos) {
    q = s.length() - 1;
  }
  return s.substr(p, q - p + 1);

}

bool runWmiQuery(std::string query, std::map< std::string, uint64 > & wmiMap)
{
  IEnumWbemClassObject *  pEnumerator = NULL;
  BSTR                    b;
  IWbemClassObject *      pclsObj = NULL;
  HRESULT                 result;
  VARIANT                 vtProp;
  ULONG                   uReturn = 0;

  if (!wmiInitialized) {
    wmiInitialized = initializeWmi();
    if (!wmiInitialized) {
      return false;
    }
  }

  b = _bstr_t(asciiDecode(query).c_str());

  result = pSvc->ExecQuery(L"WQL", b, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
  if (FAILED(result)) {
    return false;
  }

  while (pEnumerator) {
    result = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
    if (!uReturn) {
      break;
    }
    VariantInit(&vtProp);
    pclsObj->BeginEnumeration(WBEM_FLAG_NONSYSTEM_ONLY);
    {
      CIMTYPE   pvtType;
      VARIANT     pVal;
      BSTR        pstrName; 

      while (pclsObj->Next(0, &pstrName, &pVal, &pvtType, NULL) == WBEM_S_NO_ERROR) {
                //std::wcout << pstrName << " " << pVal.vt << std::endl;
        if (pVal.vt == VT_I4) {
          wmiMap[ asciiEncode(pstrName) ] = pVal.lVal;
                    //std::wcout << pstrName << ":" << pVal.bstrVal << std::endl;
        }
        SysFreeString(pstrName);
        VariantClear(&pVal);
      }
    }
  }
  pEnumerator->Release();
  if (pclsObj) {
    pclsObj->Release();
  }

  return true;
}
bool getWmiClass(const char * wmiClass, std::map< std::string, std::string > & wmiMap)
{
  IEnumWbemClassObject *  pEnumerator = NULL;
  std::wostringstream     query;
  std::wstring            q;
  BSTR                    b;
  IWbemClassObject *      pclsObj = NULL;
  HRESULT                 result;
  VARIANT                 vtProp;
  ULONG                   uReturn = 0;

  if (!wmiInitialized) {
    wmiInitialized = initializeWmi();
    if (!wmiInitialized) {
      return false;
    }
  }

  query << "SELECT * FROM " << asciiDecode(wmiClass);
  b = _bstr_t(query.str().c_str());

  result = pSvc->ExecQuery(L"WQL", b, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
  if (FAILED(result)) {
    return false;
  }

  while (pEnumerator) {
    result = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
    if (!uReturn) {
      break;
    }
    VariantInit(&vtProp);
    pclsObj->BeginEnumeration(WBEM_FLAG_NONSYSTEM_ONLY);
    {
      CIMTYPE        pvtType;
      VARIANT        pVal;
      BSTR        pstrName; 
      while (pclsObj->Next(0, &pstrName, &pVal, &pvtType, NULL) == WBEM_S_NO_ERROR) {
                //std::wcout << pstrName << " " << pVal.vt << std::endl;
        if (pVal.vt == VT_BSTR) {
          wmiMap[ asciiEncode(pstrName) ] = trim(asciiEncode(pVal.bstrVal));
                    //std::wcout << pstrName << ":" << pVal.bstrVal << std::endl;
        }
        SysFreeString(pstrName);
        VariantClear(&pVal);
      }
    }
  }
  pEnumerator->Release();
  if (pclsObj) {
    pclsObj->Release();
  }
  return true;
}

bool getWmiClass(const char * wmiClass, std::map< std::string, uint64 > & wmiMap)
{
  IEnumWbemClassObject *  pEnumerator = NULL;
  std::wostringstream     query;
  std::wstring            q;
  BSTR                    b;
  IWbemClassObject *      pclsObj = NULL;
  HRESULT                 result;
  VARIANT                 vtProp;
  ULONG                   uReturn = 0;

  if (!wmiInitialized) {
    wmiInitialized = initializeWmi();
    if (!wmiInitialized) {
      return false;
    }
  }

  query << "SELECT * FROM " << asciiDecode(wmiClass);
  b = _bstr_t(query.str().c_str());

  result = pSvc->ExecQuery(L"WQL", b, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
  if (FAILED(result)) {
    return false;
  }

  while (pEnumerator) {
    result = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
    if (!uReturn) {
      break;
    }
    VariantInit(&vtProp);
    pclsObj->BeginEnumeration(WBEM_FLAG_NONSYSTEM_ONLY);
    {
      CIMTYPE   pvtType;
      VARIANT     pVal;
      BSTR        pstrName; 

      while (pclsObj->Next(0, &pstrName, &pVal, &pvtType, NULL) == WBEM_S_NO_ERROR) {
                //std::wcout << pstrName << " " << pVal.vt << std::endl;
        if (pVal.vt == VT_I4) {
          wmiMap[ asciiEncode(pstrName) ] = pVal.lVal;
                    //std::wcout << pstrName << ":" << pVal.bstrVal << std::endl;
        }
        SysFreeString(pstrName);
        VariantClear(&pVal);
      }
    }
  }
  pEnumerator->Release();
  if (pclsObj) {
    pclsObj->Release();
  }

  return true;
}

bool getWmiSmbios(uint8 ** data, uint32 * length)
{
  IEnumWbemClassObject *  pEnumerator = NULL;
  std::wostringstream     query;
  std::wstring            q;
  BSTR                    b;
  IWbemClassObject *      pInstance = NULL;
  HRESULT                 result;
  VARIANT                 vtProp;
  ULONG                   uReturn = 0;
  CIMTYPE                 pvtType;

  if (!wmiInitialized) {
    wmiInitialized = initializeWmi();
    if (!wmiInitialized) {
      return false;
    }
  }

  result = pSvcSmbios->CreateInstanceEnum(L"MSSMBios_RawSMBiosTables", 0, NULL, &pEnumerator);
  if (FAILED(result)) {
    return false;
  }
   
   
  while (pEnumerator) {
    result = pEnumerator->Next(WBEM_INFINITE, 1, &pInstance, &uReturn);
    if (!uReturn) {
      break;
    }
    VariantInit(&vtProp);
    
    result = pInstance->Get(bstr_t("SMBiosData"), 0, &vtProp, &pvtType, NULL);
    if (SUCCEEDED(result)) {
      SAFEARRAY * array = V_ARRAY(&vtProp);
  
      *length = array->rgsabound[0].cElements;
      *data = new uint8[*length];
      memcpy(*data, (BYTE*)array->pvData, *length);
      VariantClear(&vtProp);
    }
  }
  pEnumerator->Release();
  if (pInstance) {
    pInstance->Release();
  }
  return true; 
}

