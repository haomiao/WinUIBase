/*++ BUILD Version: 0001    

Module Name:

        kmenuwnd.h

Abstract:

        简易菜单封装窗口

Author:

        zhouhao3   2018-5-10

Revision History:

--*/

#ifndef _KMENU_WND_H_
#define _KMENU_WND_H_

#include <GdiPlus.h>
#include <string>
#include <vector>
#include "titlebar/knsedrawbutton.h"
#include "ctrl/kpopuptipwnd.h"

class KMenuWnd : public KPopupTipWnd
{
public:

	struct MenuItemPair 
	{
		UINT nMenuID;			// 菜单项ID(Note：不同菜单用不同ID，当菜单ID为-1，则标识为分隔符)
		UINT nOffset;			// 菜单项距左侧偏移量
		std::wstring strMenu;	// 菜单项字符串
		UINT nIconID;			// 菜单项字符串前的ICON图标ID，若为-1，则不显示该图标
	};

    KMenuWnd();
    virtual ~KMenuWnd();

public:

	 /*++
	  * ShowMenuWnd
	  * 移动窗口位置并调整显示
	  * 返回值为对应菜单ID，否则为(UINT)-1
	--*/
	UINT ShowMenuWnd(__in const POINT &ptPos);

	 /*++
	  * AddMenu
	  * 添加菜单项
	--*/
	void AddMenu(
		__in UINT nMenuID,
		__in UINT nOffset,
		__in const std::wstring& strMenu,
		__in UINT nIconID = (UINT)-1
		);
	void AddMenu(__in const MenuItemPair& menuPair);

private:

	virtual void InitResources();
	virtual void InitControls();
	virtual void ReleaseResources();

	virtual void DrawBackground(HDC hDC);
	virtual void DrawOthers(HDC hDC);
	virtual void DrawControls(HDC hDC);

	virtual LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:

	void InitFonts();
	void InitImages();

    void HookWndProc();
    void UnHookWndProc();

	 /*++
	  * 消息泵
	--*/
    void MsgPump();

	UINT GetMenuPairIndex(__in const POINT& pt);
	void ReSetItemRect();
	UINT GetMenuLineItemCount();
	UINT GetMostWidthItemIndex();
	void AutoAdjustWindow();

private:

	static LRESULT CALLBACK _HookCallWndProc(
		__in int nCode,
		__in WPARAM wParam,
		__in LPARAM lParam
		);

private:

	HFONT m_hTextFont;
	Gdiplus::Image *m_pBackgroundImg;
	static HHOOK m_hCallProcHook;
	static KMenuWnd* m_pSuspensionMenuWnd;
	UINT m_nRetID;
	std::vector<MenuItemPair> m_vecMenuPair;
	std::vector<RECT> m_vecMenuRect;
	bool m_bTrackMouse;
};

inline bool operator== (
	const KMenuWnd::MenuItemPair& ts,
	const KMenuWnd::MenuItemPair& tt
	)
{
	return ts.nMenuID == tt.nMenuID
		&& ts.nOffset == tt.nOffset
		&& ts.strMenu == tt.strMenu
		&& ts.nIconID == tt.nIconID;
}

#endif  //_KMENU_WND_H_
