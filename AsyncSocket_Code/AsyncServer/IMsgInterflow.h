/*
@brief 消息交流接口
*/
#pragma once
#include "AsyncServDefine.h"

class IMsgInterflow
{
public:
    //响应套接字关闭
    virtual void OnFDClose(SOCKET sClient) = 0;

protected:
    virtual ~IMsgInterflow() {}
};