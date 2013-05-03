#pragma once
#include <string>
using std::wstring;
using std::string;

class CAsyncFunc
{
public:
    CAsyncFunc(void);
    ~CAsyncFunc(void);

    static wstring AnsiToUnicode(const string& str);
    static string UnicodeToAnsi(const wstring& str);
    /*
    @brief 以追加模式设置控件信息
    @param[in] pWnd 父窗口
    @param[in] nID 控件ID
    @param[in] strInfo 控件信息
    */
    static void SetAppendText(CWnd* pWnd, int nID, const CString& strInfo);
};
