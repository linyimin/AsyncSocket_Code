// AsyncTalkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AsyncServer.h"
#include "AsyncTalkDlg.h"
#include "AsyncTalkMng.h"
#include "AsyncFunc.h"

#define WM_NETWORK_TALK WM_USER+101
#define DEFAULT_BUFFER 4096

// CAsyncTalkDlg dialog

IMPLEMENT_DYNAMIC(CAsyncTalkDlg, CDialog)

CAsyncTalkDlg::CAsyncTalkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAsyncTalkDlg::IDD, pParent)
    , m_pMsgFlow(NULL)
{

}

CAsyncTalkDlg::~CAsyncTalkDlg()
{
}

void CAsyncTalkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAsyncTalkDlg, CDialog)
    ON_BN_CLICKED(IDC_BTN_OVER, &CAsyncTalkDlg::OnBnClickedBtnOver)
    ON_MESSAGE(WM_NETWORK_TALK,&CAsyncTalkDlg::OnNetwork)
    ON_BN_CLICKED(IDC_BTN_SEND, &CAsyncTalkDlg::OnBnClickedBtnSend)
END_MESSAGE_MAP()


// CAsyncTalkDlg message handlers

BOOL CAsyncTalkDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    InitDlgTitle();
    InitSocketMsg();

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CAsyncTalkDlg::InitDlgTitle()
{
    CString strTitle = _T("");
    strTitle += _T("Talk with:");
    strTitle += clientSock.strIPAddr;
    SetWindowText(strTitle);
}

void CAsyncTalkDlg::InitSocketMsg()
{
    int nAsyncRet = WSAAsyncSelect(clientSock.sClient, m_hWnd, WM_NETWORK_TALK, FD_READ|FD_CLOSE);
    if (SOCKET_ERROR == nAsyncRet)
    {
        CAsyncFunc::SetAppendText(this, IDC_EDIT_DESC, _T("注册网络事件失败！"));

        return;
    }
}

void CAsyncTalkDlg::OnBnClickedBtnOver()
{
    CloseTalkDlg();
}

void CAsyncTalkDlg::CloseTalkDlg()
{
    PostMessage(WM_CLOSE);
}

BOOL CAsyncTalkDlg::PreTranslateMessage(MSG* pMsg)
{
    //对话框屏蔽Enter和ESC键
    if (WM_KEYDOWN == pMsg->message)
    {
        if (VK_RETURN == pMsg->wParam)
        {
            if (GetDlgItem(IDC_BTN_SEND)->IsWindowEnabled())
            {
                OnBnClickedBtnSend();
            }
        }

        if (    VK_RETURN == pMsg->wParam
            || VK_ESCAPE == pMsg->wParam)
        {
            return TRUE;
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void CAsyncTalkDlg::OnCancel()
{
    DestroyWindow();
}

void CAsyncTalkDlg::PostNcDestroy()
{
    CDialog::PostNcDestroy();

    //窗口销毁时，删除该对象
    CAsyncTalkMng::GetInstance()->DeleteTalkDlg(clientSock.sClient);
}

void CAsyncTalkDlg::SetClientSock(StClientSock* pClientSock)
{
    clientSock.sClient = pClientSock->sClient;
    clientSock.strIPAddr = pClientSock->strIPAddr;
}

LRESULT CAsyncTalkDlg::OnNetwork(WPARAM wParam, LPARAM lParam)  
{
    WORD netEvent = WSAGETSELECTEVENT(lParam);
    WORD error = WSAGETSELECTERROR(lParam);
    if (error!=0)
    {
        CString strErrorInfo = _T("");
        strErrorInfo.Format(_T("Error code: %d"), error);
        CAsyncFunc::SetAppendText(this, IDC_EDIT_DESC, strErrorInfo);

        return -1;
    }

    switch(netEvent)
    {
    case FD_READ: OnFDRead(); break;
    case FD_CLOSE: OnFDClose((SOCKET)wParam); break;
    }

    return 0;
}

void CAsyncTalkDlg::OnFDRead()
{
    char szBuff[DEFAULT_BUFFER] = {0};

    int ret = recv(clientSock.sClient, szBuff, DEFAULT_BUFFER, 0);
    if (0 == ret)
    {
        return;
    }
    else if (SOCKET_ERROR == ret)
    {
        CAsyncFunc::SetAppendText(this, IDC_EDIT_RECV, _T("recv() failed"));
        return;
    }
    szBuff[ret] = '\0';

    wstring wsRecvInfo = CAsyncFunc::AnsiToUnicode(string(szBuff));
    CString strRecvInfo = _T("");
    strRecvInfo += clientSock.strIPAddr;
    strRecvInfo += _T(":");
    strRecvInfo += (CString)(wsRecvInfo.c_str());

    CAsyncFunc::SetAppendText(this, IDC_EDIT_RECV, strRecvInfo);
}

void CAsyncTalkDlg::OnFDClose(SOCKET sClient)
{
    if (NULL == m_pMsgFlow)
    {
        return;
    }

    m_pMsgFlow->OnFDClose(sClient);
}

void CAsyncTalkDlg::OnBnClickedBtnSend()
{
    CString strSendMsg = _T("");
    GetDlgItemText(IDC_EDIT_SEND, strSendMsg);

    if (strSendMsg.IsEmpty())
    {
        MessageBox(_T("发送信息不能为空"), _T("提示"), MB_OK|MB_ICONWARNING);
        return;
    }

    string saSendMsg = CAsyncFunc::UnicodeToAnsi(wstring(strSendMsg));
    const char* szSendMsg = saSendMsg.c_str();
    int ret = send(clientSock.sClient, szSendMsg, (int)strlen(szSendMsg), 0);
    if (ret == 0 || ret == SOCKET_ERROR)
    {
        CAsyncFunc::SetAppendText(this, IDC_EDIT_RECV, _T("send() failed!"));
        return;
    }

    CString strSendInfo = _T("");
    strSendInfo += _T("me:");
    strSendInfo += strSendMsg;

    CAsyncFunc::SetAppendText(this, IDC_EDIT_RECV, strSendInfo);
    SetDlgItemText(IDC_EDIT_SEND, _T(""));
}

void CAsyncTalkDlg::StopTalk()
{
    SetForegroundWindow();
    CAsyncFunc::SetAppendText(this, IDC_EDIT_RECV, _T("连接已关闭"));

    SetDlgItemText(IDC_EDIT_SEND, _T(""));
    GetDlgItem(IDC_EDIT_SEND)->EnableWindow(FALSE);
    GetDlgItem(IDC_BTN_SEND)->EnableWindow(FALSE);
}

void CAsyncTalkDlg::SetMsgInterflow(IMsgInterflow* pMsgFlow)
{
    m_pMsgFlow = pMsgFlow;
}
