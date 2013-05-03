// AsyncClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AsyncClient.h"
#include "AsyncClientDlg.h"
#include "AsyncFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAsyncClientDlg dialog

#define WM_NETWORK WM_USER+100
#define DEFAULT_BUFFER 4096

const UINT DEFAULT_PORT = 5150;


CAsyncClientDlg::CAsyncClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAsyncClientDlg::IDD, pParent)
    , m_bSocketClose(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAsyncClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAsyncClientDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BTN_CONN, &CAsyncClientDlg::OnBnClickedBtnConn)
    ON_BN_CLICKED(IDC_BTN_CLOSE, &CAsyncClientDlg::OnBnClickedBtnClose)
    ON_BN_CLICKED(IDC_BTN_SEND, &CAsyncClientDlg::OnBnClickedBtnSend)
    ON_MESSAGE(WM_NETWORK,&CAsyncClientDlg::OnNetwork)
    ON_WM_CLOSE()
END_MESSAGE_MAP()


// CAsyncClientDlg message handlers

BOOL CAsyncClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
    GetDlgItem(IDC_EDIT_SEND)->EnableWindow(FALSE);
    GetDlgItem(IDC_BTN_SEND)->EnableWindow(FALSE);

    CIPAddressCtrl* pIP=(CIPAddressCtrl*)GetDlgItem(IDC_IP_CONN);
    pIP->SetAddress(127,0,0,1);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAsyncClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAsyncClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CAsyncClientDlg::OnBnClickedBtnConn()
{
    CIPAddressCtrl * pIP=(CIPAddressCtrl*)GetDlgItem(IDC_IP_CONN);
    if (pIP->IsBlank())
    {
        MessageBox(_T("服务端地址不能为空"), _T("提示"), MB_OK|MB_ICONWARNING);
        return;
    }

    //获取IP地址
    BYTE nf1,nf2,nf3,nf4;
    pIP->GetAddress(nf1,nf2,nf3,nf4);

    char szServer[128] = {0};
    sprintf_s(szServer, 128, "%d.%d.%d.%d", nf1,nf2,nf3,nf4);

    WSADATA wsd;
    if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
    {
        CAsyncFunc::SetAppendText(this, IDC_EDIT_RECV, _T("load Winsock library failed!"));
        return;
    }
    CAsyncFunc::SetAppendText(this, IDC_EDIT_RECV, _T("load Winsock library success!"));

    m_sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (m_sClient == INVALID_SOCKET)
    {
        CAsyncFunc::SetAppendText(this, IDC_EDIT_RECV, _T("socket() failed!"));
        return;
    }
    CAsyncFunc::SetAppendText(this, IDC_EDIT_RECV, _T("socket() success!"));

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);
    server.sin_addr.s_addr = inet_addr(szServer);

    if (connect(m_sClient, (struct sockaddr*)&server, sizeof(server))
        == SOCKET_ERROR)
    {
        CAsyncFunc::SetAppendText(this, IDC_EDIT_RECV, _T("connect() failed!"));
        return;
    }
    CAsyncFunc::SetAppendText(this, IDC_EDIT_RECV, _T("connect() success!"));

    //注册感兴趣的网络事件
    int nAsyncRet = WSAAsyncSelect(m_sClient, m_hWnd, WM_NETWORK, FD_READ|FD_CLOSE|FD_WRITE);
    if (SOCKET_ERROR == nAsyncRet)
    {
        CAsyncFunc::SetAppendText(this, IDC_EDIT_RECV, _T("注册网络事件失败！"));

        return;
    }

    GetDlgItem(IDC_EDIT_SEND)->EnableWindow(TRUE);
    GetDlgItem(IDC_BTN_SEND)->EnableWindow(TRUE);

    GetDlgItem(IDC_IP_CONN)->EnableWindow(FALSE);
    GetDlgItem(IDC_BTN_CONN)->EnableWindow(FALSE);
}

void CAsyncClientDlg::OnBnClickedBtnClose()
{
    PostMessage(WM_CLOSE);
}

void CAsyncClientDlg::OnBnClickedBtnSend()
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




    int ret = send(m_sClient, szSendMsg, (int)strlen(szSendMsg), 0);
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

LRESULT CAsyncClientDlg::OnNetwork(WPARAM wParam, LPARAM lParam)
{
    //SOCKET s = (SOCKET)wParam;
    WORD netEvent = WSAGETSELECTEVENT(lParam);
    WORD error = WSAGETSELECTERROR(lParam);
    if (error!=0)
    {
        CString strErrorInfo = _T("");
        strErrorInfo.Format(_T("Error code: %d"), error);
        CAsyncFunc::SetAppendText(this, IDC_EDIT_RECV, strErrorInfo);

        return -1;
    }

    switch(netEvent)
    {
    case FD_READ: OnFDRead(); break;
	case FD_WRITE: OnFDWrite(); break;
    case FD_CLOSE: OnFDClose(); break;
    }

    return 0;
}

void CAsyncClientDlg::OnFDRead()  
{
    char szBuff[DEFAULT_BUFFER] = {0};

    int ret = recv(m_sClient, szBuff, DEFAULT_BUFFER, 0);
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
    strRecvInfo += _T("服务端:");
    strRecvInfo += (CString)(wsRecvInfo.c_str());

    CAsyncFunc::SetAppendText(this, IDC_EDIT_RECV, strRecvInfo);
}

void CAsyncClientDlg::OnFDWrite()  
{
	//当连接成功后，接受到写写消息，说明现在可以调用send发送消息了
	//(1)另外一种可能是，当发送send消息，发送缓冲区被沾满，则发送失败，当发送缓冲区内容发送出去后，
	//	可以再次发送内容时，会接受到改消息
	CAsyncFunc::SetAppendText(this, IDC_EDIT_RECV, _T("写数据成功:"));
}
void CAsyncClientDlg::OnFDClose()
{
    closesocket(m_sClient);
    m_bSocketClose = true;

    CAsyncFunc::SetAppendText(this, IDC_EDIT_RECV, _T("服务端已关闭"));

    GetDlgItem(IDC_EDIT_SEND)->EnableWindow(FALSE);
    GetDlgItem(IDC_BTN_SEND)->EnableWindow(FALSE);
}

BOOL CAsyncClientDlg::PreTranslateMessage(MSG* pMsg)
{
    //对话框屏蔽Enter和ESC键
    if (WM_KEYDOWN == pMsg->message)
    {
        //回车键响应消息发送
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


void CAsyncClientDlg::OnClose()
{
    // TODO: Add your message handler code here and/or call default
    if (!m_bSocketClose)
    {
        closesocket(m_sClient);
    }

    WSACleanup();
    
    CDialog::OnClose();
}
