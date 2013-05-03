#pragma once
#include "IMsgInterflow.h"

// CAsyncTalkDlg dialog

class CAsyncTalkDlg : public CDialog
{
	DECLARE_DYNAMIC(CAsyncTalkDlg)

public:
	CAsyncTalkDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAsyncTalkDlg();

    void SetMsgInterflow(IMsgInterflow* pMsgFlow);
    void SetClientSock(StClientSock* pClientSock);
    void StopTalk();
    void CloseTalkDlg();

// Dialog Data
	enum { IDD = IDD_TALK_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    virtual void OnCancel();
    virtual void PostNcDestroy();
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    LRESULT OnNetwork(WPARAM wParam, LPARAM lParam);

    afx_msg void OnBnClickedBtnOver();
    afx_msg void OnBnClickedBtnSend();

	DECLARE_MESSAGE_MAP()

private:
    void InitDlgTitle();
    void InitSocketMsg();

    //响应消息接受事件
    void OnFDRead();
    void OnFDClose(SOCKET sClient);

private:
    StClientSock clientSock;
    IMsgInterflow* m_pMsgFlow;
};
