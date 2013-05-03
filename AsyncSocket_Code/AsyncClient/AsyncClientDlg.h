// AsyncClientDlg.h : header file
//

#pragma once
#include <WinSock2.h>

// CAsyncClientDlg dialog
class CAsyncClientDlg : public CDialog
{
// Construction
public:
	CAsyncClientDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ASYNCCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnBnClickedBtnConn();
    afx_msg void OnBnClickedBtnClose();
    afx_msg void OnBnClickedBtnSend();
    afx_msg void OnClose();
    LRESULT OnNetwork(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

private:
    void OnFDRead();
    void OnFDClose();
	void OnFDWrite();

private:
    SOCKET m_sClient;
    bool m_bSocketClose;
};
