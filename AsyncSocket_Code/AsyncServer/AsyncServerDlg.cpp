// AsyncServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AsyncServer.h"
#include "AsyncServerDlg.h"
#include "AsyncFunc.h"
#include "AsyncTalkMng.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_NETWORK WM_USER+100

const UINT DEFAULT_PORT = 5150;
const UINT LISTEN_LEN = 8;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAsyncServerDlg dialog




CAsyncServerDlg::CAsyncServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAsyncServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    CAsyncTalkMng::GetInstance();
}

void CAsyncServerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_CLIENT, m_ClientList);
}

BEGIN_MESSAGE_MAP(CAsyncServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_MESSAGE(WM_NETWORK,&CAsyncServerDlg::OnNetwork)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BTN_LISTEN, &CAsyncServerDlg::OnBnClickedBtnListen)
    ON_BN_CLICKED(IDC_BTN_TALK, &CAsyncServerDlg::OnBnClickedBtnTalk)
    ON_BN_CLICKED(IDC_BTN_QUIT, &CAsyncServerDlg::OnBnClickedBtnQuit)
    ON_WM_CLOSE()
END_MESSAGE_MAP()


// CAsyncServerDlg message handlers

BOOL CAsyncServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAsyncServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAsyncServerDlg::OnPaint()
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
HCURSOR CAsyncServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAsyncServerDlg::OnBnClickedBtnListen()
{
    WSADATA wsd;
    if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
    {
        CAsyncFunc::SetAppendText(this, IDC_EDIT_RECV, _T("load Winsock fail!"));
        return;
    }

    CAsyncFunc::SetAppendText(this, IDC_EDIT_DESC, _T("load Winsock success!"));

    //Create our listening socket
    m_sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (m_sListen == SOCKET_ERROR)
    {
        CString strError = _T("");
        strError.Format(_T("socket() failed:%d!"), WSAGetLastError());
        CAsyncFunc::SetAppendText(this, IDC_EDIT_DESC, strError);
        return;
    }
    CAsyncFunc::SetAppendText(this, IDC_EDIT_DESC, _T("socket() success!\n"));

    struct sockaddr_in local;
    //INADDR_ANY是所有网卡地址
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_family = AF_INET;
    local.sin_port = htons(DEFAULT_PORT);

    //绑定监听网卡地址和端口
    if (bind(m_sListen, (struct sockaddr*)&local, sizeof(local)) == SOCKET_ERROR)
    {
        CString strError = _T("");
        strError.Format(_T("bind() failed:%d!"), WSAGetLastError());
        CAsyncFunc::SetAppendText(this, IDC_EDIT_DESC, strError);

        return;
    }
    CAsyncFunc::SetAppendText(this, IDC_EDIT_DESC, _T("bind() success!\n"));

    //开始监听客户端
    listen(m_sListen, LISTEN_LEN);

    CString strTipInfo = _T("");
    strTipInfo.Format(_T("等待客户端连接（端口%d）..."), DEFAULT_PORT);

    CAsyncFunc::SetAppendText(this, IDC_EDIT_DESC, strTipInfo);

    //注册感兴趣的网络事件
    int nAsyncRet = WSAAsyncSelect(m_sListen, m_hWnd, WM_NETWORK, FD_ACCEPT|FD_READ|FD_CLOSE);
    if (SOCKET_ERROR == nAsyncRet)
    {
        CAsyncFunc::SetAppendText(this, IDC_EDIT_DESC, _T("注册网络事件失败！"));

        return;
    }

    //监听按钮置为灰
    GetDlgItem(IDC_BTN_LISTEN)->EnableWindow(FALSE);
}

LRESULT CAsyncServerDlg::OnNetwork(WPARAM wParam, LPARAM lParam)  
{  
    SOCKET sClient = (SOCKET)wParam;
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
    case FD_ACCEPT: OnAccept(); break;
    case FD_READ: OnFDRead(sClient); break;
    case FD_CLOSE: OnFDClose(sClient); break;
    }

    return 0;
}

void CAsyncServerDlg::OnFDRead(SOCKET sClient)
{
    //该客户端首次收到消息，启动对话框
    CAsyncTalkMng::GetInstance()->StartTalkDlg(sClient, this);
}

void CAsyncServerDlg::OnFDClose(SOCKET sClient)
{
    //删除列表条目
    int nItemCount = m_ClientList.GetCount();
    for (int i = 0; i < nItemCount; i++)
    {
        SOCKET sTemp = (SOCKET)m_ClientList.GetItemData(i);
        if (sTemp == sClient)
        {
            m_ClientList.DeleteString(i);
            break;
        }
    }

    //关闭连接
    CAsyncTalkMng::GetInstance()->RemoveClient(sClient);

    //通知对话框，连接已关闭
    CAsyncTalkMng::GetInstance()->StopTalkDlg(sClient);
}

void CAsyncServerDlg::OnAccept()
{
    struct sockaddr_in client;
    int iAddrSize = sizeof(client);
    SOCKET sClient = accept(m_sListen, (struct sockaddr*)&client, &iAddrSize);
    if (sClient == INVALID_SOCKET)
    {
        CString strErrorInfo = _T("");
        strErrorInfo.Format(_T("accept() failed:%d"), WSAGetLastError());

        CAsyncFunc::SetAppendText(this, IDC_EDIT_DESC, strErrorInfo);

        return;
    }

    char* sClientIP = inet_ntoa(client.sin_addr);
    wstring wsClientIP = CAsyncFunc::AnsiToUnicode(string(sClientIP));
    CString strClientIP = wsClientIP.c_str();

    CString strIPAddr = _T("");
    strIPAddr.Format(_T("%s:%d"), strClientIP, ntohs(client.sin_port));

    CString strTipInfo = _T("");
    strTipInfo += _T("accepted client:");
    strTipInfo += strIPAddr;
    CAsyncFunc::SetAppendText(this, IDC_EDIT_DESC, strTipInfo);

    StClientSock* pClientSock = new StClientSock;
    pClientSock->sClient = sClient;
    pClientSock->strIPAddr = strIPAddr;

    CAsyncTalkMng::GetInstance()->AddClient(pClientSock);

    int nItemIndex = m_ClientList.AddString(strIPAddr);

    m_ClientList.SetItemData(nItemIndex, sClient);
}

void CAsyncServerDlg::OnBnClickedBtnTalk()
{
    int nCurSel = m_ClientList.GetCurSel();
    if (-1 == nCurSel)
    {
        MessageBox(_T("请选择一个客户端！"), _T("提示"), MB_OK|MB_ICONWARNING);
        return;
    }

    SOCKET sClient = (SOCKET)m_ClientList.GetItemData(nCurSel);
    CAsyncTalkMng::GetInstance()->StartTalkDlg(sClient, this);
}

void CAsyncServerDlg::OnBnClickedBtnQuit()
{
    PostMessage(WM_CLOSE);
}

BOOL CAsyncServerDlg::PreTranslateMessage(MSG* pMsg)
{
    //对话框屏蔽Enter和ESC键
    if (WM_KEYDOWN == pMsg->message)
    {
        if (    VK_RETURN == pMsg->wParam
            || VK_ESCAPE == pMsg->wParam)
        {
            return TRUE;
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void CAsyncServerDlg::OnClose()
{
    //关闭所有对话框和socket连接
    CAsyncTalkMng::GetInstance()->CloseAllClient();

    closesocket(m_sListen);
    WSACleanup();

    CAsyncTalkMng::ReleaseInstance();

    CDialog::OnClose();
}
