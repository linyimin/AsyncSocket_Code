#include "StdAfx.h"
#include "AsyncServer.h"
#include "AsyncTalkMng.h"
#include "AsyncTalkDlg.h"

CAsyncTalkMng* CAsyncTalkMng::m_pInstance = NULL;
CAsyncTalkMng::CAsyncTalkMng(void)
{
}

CAsyncTalkMng::~CAsyncTalkMng(void)
{
}

CAsyncTalkMng* CAsyncTalkMng::GetInstance()
{
    if (NULL == m_pInstance)
    {
        m_pInstance = new CAsyncTalkMng();
    }

    return m_pInstance;
}

void CAsyncTalkMng::ReleaseInstance()
{
    if (m_pInstance != NULL)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}

void CAsyncTalkMng::StartTalkDlg(SOCKET sClient, IMsgInterflow* pMsgFlow)
{
    //判断对话框是否存在
    map<SOCKET, CAsyncTalkDlg*>::iterator mDlgPos = m_TalkDlgMap.find(sClient);
    if (mDlgPos != m_TalkDlgMap.end())
    {
        CAsyncTalkDlg* pTalkDlg = mDlgPos->second;
        pTalkDlg->SetForegroundWindow();

        return;
    }

    //获取Socket信息
    map<SOCKET, StClientSock*>::iterator mClientPos = m_SocketMap.find(sClient);
    if (mClientPos == m_SocketMap.end())
    {
        return;
    }

    StClientSock* pClientSock = mClientPos->second;

    CAsyncTalkDlg* pTalkDlg = new CAsyncTalkDlg();
    pTalkDlg->SetMsgInterflow(pMsgFlow);
    pTalkDlg->SetClientSock(pClientSock);

    pTalkDlg->Create(CAsyncTalkDlg::IDD, AfxGetMainWnd());
    pTalkDlg->ShowWindow(SW_SHOW);

    m_TalkDlgMap.insert(make_pair(pClientSock->sClient, pTalkDlg));
}

void CAsyncTalkMng::StopTalkDlg(SOCKET sClient)
{
    //判断对话框是否存在
    map<SOCKET, CAsyncTalkDlg*>::iterator mDlgPos = m_TalkDlgMap.find(sClient);
    if (mDlgPos == m_TalkDlgMap.end())
    {
        return;
    }

    CAsyncTalkDlg* pTalkDlg = mDlgPos->second;
    pTalkDlg->StopTalk();
}

void CAsyncTalkMng::DeleteTalkDlg(SOCKET sClient)
{
    map<SOCKET, CAsyncTalkDlg*>::iterator mPos = m_TalkDlgMap.find(sClient);
    if (mPos == m_TalkDlgMap.end())
    {
        return;
    }

    delete mPos->second;
    mPos->second = NULL;

    m_TalkDlgMap.erase(mPos);
}

void CAsyncTalkMng::AddClient(StClientSock* pClientSock)
{
    if (NULL == pClientSock)
    {
        return;
    }

    m_SocketMap.insert(make_pair(pClientSock->sClient, pClientSock));
}

void CAsyncTalkMng::RemoveClient(SOCKET sClient)
{
    map<SOCKET, StClientSock*>::iterator mPos = m_SocketMap.find(sClient);
    if (mPos == m_SocketMap.end())
    {
        return;
    }

    //关闭连接
    closesocket(mPos->second->sClient);

    delete mPos->second;
    mPos->second = NULL;

    m_SocketMap.erase(mPos);
}

void CAsyncTalkMng::CloseAllClient()
{
    //关闭所有对话框
    vector<CAsyncTalkDlg*> vTalkDlg;
    map<SOCKET, CAsyncTalkDlg*>::iterator mDlgPos = m_TalkDlgMap.begin();
    for (; mDlgPos != m_TalkDlgMap.end(); ++mDlgPos)
    {
        vTalkDlg.push_back(mDlgPos->second);
    }

    vector<CAsyncTalkDlg*>::iterator vTalkIter = vTalkDlg.begin();
    for (; vTalkIter != vTalkDlg.end(); ++vTalkIter)
    {
        (*vTalkIter)->CloseTalkDlg();
    }

    //关闭所有连接
    vector<SOCKET> vClientSocket;
    map<SOCKET, StClientSock*>::iterator mClientPos = m_SocketMap.begin();
    for (; mClientPos != m_SocketMap.end(); ++mClientPos)
    {
        vClientSocket.push_back(mClientPos->first);
    }

    vector<SOCKET>::iterator vClientIter = vClientSocket.begin();
    for (; vClientIter != vClientSocket.end(); ++vClientIter)
    {
        RemoveClient(*vClientIter);
    }
}
