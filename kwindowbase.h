/*++ BUILD Version: 0001    

Module Name:

        kwindowbase.h

Abstract:

        窗体基类,封装基本的窗口注册、创建、消息机制

Author:

        zhouhao3   2018-4-17

Revision History:

--*/

#ifndef _KWINDOW_BASE_H_
#define _KWINDOW_BASE_H_

#include <GdiPlus.h>
#include <string>
#include <WindowsX.h>
#include "resource.h"
#include "titlebar/titlebardefine.h"
#include "titlebar/knsedrawbutton.h"

class KWindowBase
{
public:

    /*++
	 * 窗口显示所在点位置
	 --*/
    enum POS_TOWARD
    {
        TOWARD_LEFT_UP = 0,
        TOWARD_RIGHT_UP,
        TOWARD_LEFT_DOWN,
        TOWARD_RIGHT_DOWN,
    };

public:

    KWindowBase();
    virtual ~KWindowBase();

public:

     /*++
	  * CreateWnd
	  * 创建窗口
	--*/
	BOOL CreateWnd(
        __in HWND hParentWnd,
		__in const wchar_t* pStrClassName,
		__in const wchar_t* pStrWndName = L"",
		__in DWORD dwStyle =
			WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		__in DWORD dwExStyle = 
			WS_EX_OVERLAPPEDWINDOW,
		__in int x = CW_USEDEFAULT,
		__in int y = CW_USEDEFAULT,
		__in int cx = CW_USEDEFAULT,
		__in int cy = CW_USEDEFAULT,
        __in double lfScale = 1.0
		);

	 /*++
	  * CloseWnd
	  * 关闭窗口
	--*/
	void CloseWnd(__in int nRetCode);

	 /*++
	  * GetWndClassName
	  * 获取窗口类名
	--*/
	std::wstring GetWndClassName() const;

	/*++
	 * GetWndName
	 * 获取窗口名
	 --*/
	std::wstring GetWndName() const;

     /*++
	  * GetWindowHandle
	  * 获取窗口句柄
	--*/
	HWND GetWindowHandle() const;

     /*++
	  * GetParentWindowHandle
	  * 获取父窗口句柄
	--*/
    HWND GetParentWindowHandle() const;

     /*++
	  * MoveWindow
	  * 移动窗口位置
	--*/
	void MoveWindow(__in const RECT &rect);

     /*++
	  * ShowWindow
	  * 移动窗口位置并调整显示,非模态窗体
	--*/
	void ShowWindow(
        __in const POINT &ptPos,
        __in POS_TOWARD ePosToward = TOWARD_LEFT_DOWN,
		__in BOOL bShow = TRUE
		);

     /*++
	  * ShowWindow
	  * 显示/隐藏窗口,非模态窗体
	--*/
	void ShowWindow(__in BOOL bShow);

	 /*++
	  * DoModal
	  * 显示窗口,模态窗体
	--*/
	void DoModal();

     /*++
	  * RePaint
	  * 重绘窗口
	--*/
	void RePaint();

	 /*++
	  * SendMessage/PostMessage
	  * 发送窗体消息
	--*/
	BOOL SendMessage(
		__in UINT nMsg, 
		__in WPARAM wParam, 
		__in LPARAM lParam
		);

	BOOL PostMessage(
		__in UINT nMsg, 
		__in WPARAM wParam, 
		__in LPARAM lParam
		);

private:

     /*++
	  * 消息泵
	--*/
    void MsgPump();

     /*++
	  * 销毁窗体
	--*/
    void DestroyWnd();

     /*++
	  * 注册/反注册窗口
	--*/
    BOOL RegisterWnd(__in const wchar_t* pStrClasName);
    void UnRegisterWnd();

     /*++
	  * 更新窗口位置
	--*/
    void UpdateWndPos(
		__in const POINT &point,
		__in POS_TOWARD ePosToward
		) const;

protected:

	 /*++ InitializeWnd
	  * 初始化窗体
    --*/
	virtual void InitializeWnd();

	 /*++ OnTranslateMsg
	  * 返回true，则过滤该消息
    --*/
	virtual bool OnTranslateMsg(MSG& msg);

	 /*++ HandleMessage
	  * 消息处理函数
	--*/
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	 /*++ OnFinalMessage
	  * 窗口销毁时最后一次消息处理函数,一般可做一些收尾相关工作
	--*/
    virtual void OnFinalMessage(HWND hWnd);

private:

	 /*++ _WndProc
	  * 窗口消息处理函数
	--*/
    static LRESULT CALLBACK _WndProc(
		HWND hWnd,
		UINT message,
		WPARAM wParam, 
		LPARAM lParam
		);

private:

    KWindowBase(const KWindowBase&);
    KWindowBase& operator= (const KWindowBase&);

protected:

	HWND	m_hWnd;
	HWND	m_hParent;
    WNDPROC m_oldWndProc;
	std::wstring m_strClassName;
	std::wstring m_strWndName;

    double	m_lfScale;
    BOOL	m_bHide;
    BOOL    m_bRegisterWnd;
};

#endif  //_KWINDOW_BASE_H_
