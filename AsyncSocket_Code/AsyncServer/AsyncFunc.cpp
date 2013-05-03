#include "StdAfx.h"
#include "AsyncFunc.h"

CAsyncFunc::CAsyncFunc(void)
{
}

CAsyncFunc::~CAsyncFunc(void)
{
}

//Unicode to ANSI
wstring CAsyncFunc::AnsiToUnicode(const string& str)
{
    int  unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
    if (unicodeLen <= 0)
    {
        return L"";
    }

    wchar_t* pUnicode = new wchar_t[unicodeLen+1];
    memset(pUnicode, 0, (unicodeLen+1)*sizeof(wchar_t));

    ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, (LPWSTR)pUnicode, unicodeLen);

    wstring rt = pUnicode;

    delete[] pUnicode;
    pUnicode = NULL;

    return rt;
}

string CAsyncFunc::UnicodeToAnsi(const wstring& str)
{
    int iTextLen = WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
    if (iTextLen <= 0)
    {
        return "";
    }

    char* pElementText = new char[iTextLen + 1];
    memset(pElementText, 0, sizeof(char)*(iTextLen + 1));

    ::WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, pElementText, iTextLen, NULL, NULL);

    string strText = pElementText;

    delete[] pElementText;
    pElementText = NULL;

    return strText;
}

void CAsyncFunc::SetAppendText(CWnd* pWnd, int nID, const CString& strInfo)
{
    if (NULL == pWnd)
    {
        return;
    }

    CEdit* pEdit = (CEdit*)pWnd->GetDlgItem(nID);

    int nTextLen = pEdit->GetWindowTextLength();
    pEdit->SetSel(nTextLen, nTextLen);

    CString strFullInfo = _T("");
    strFullInfo += strInfo;
    strFullInfo += _T("\r\n");

    pEdit->ReplaceSel(strFullInfo); 
}
