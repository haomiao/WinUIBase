/*++ BUILD Version: 0001    

Module Name:

        ksuspensionwnd.h 

Abstract:

        云盘Shell悬浮窗体

Author:

        zhouhao3   2018-5-8

Revision History:

--*/

#ifndef _KSUSPENSION_WND_H_
#define _KSUSPENSION_WND_H_

#include <GdiPlus.h>
#include <string>
#include "titlebar/knsedrawbutton.h"
#include "ctrl/ktooltipwnd.h"
#include "ctrl/kpopuptipwnd.h"
#include "kdroptargetex.h"

class KSuspensionWnd : public KPopupTipWnd
{
public:

    KSuspensionWnd();
    virtual ~KSuspensionWnd();

public:

	/*++
	  * CreateSuspensionWnd
	  * 创建浮窗窗口
	--*/
	BOOL CreateSuspensionWnd(
		__in HWND hParentWnd,
		__in int x,
		__in int y,
		__in int cx,
		__in int cy,
		__in double lfScale = 1.0
		);

	 /*++
	  * SetOpacity
	  * 设置不透明度
	  * nOpacity: [0~255]
	--*/
	BOOL SetOpacity(BYTE nOpacity);

private:

	virtual void InitResources();
	virtual void InitControls();
	virtual void ReleaseResources();

	virtual void DrawBackground(HDC hDC);
	virtual void DrawOthers(HDC hDC);
	virtual void DrawControls(HDC hDC);

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnLButtonDBClk(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMove(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnTimer(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	LRESULT OnDropFiles(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDropEnter(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDropLeave(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnContextMenu(WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:

	void InitFonts();
	void InitImages();
	void ShowToolTip();
	void ShowCustomMenus(__in const POINT& pt);
	void StopTimer();

	void OpenWPSCloud();
	void ShowTransDetail();
	void OpenDragDir();

	void GetToolTipType(
		__out KToolTipWnd::TOOLTIP_POS_TOWARD& ePosToward,
		__out CPoint& pt
		);

	BOOL ChangeTransparency();

private:

	HFONT m_hTextFont;
	Gdiplus::Image *m_pIconImg;
	HRGN m_hRgn;
	bool m_bIsLBtnDown;
	bool m_bTrackMouse;
	COLORREF m_clBackground;
	COLORREF m_clBorder;
	COLORREF m_clIconBackground;
	std::wstring m_strInfoText;
	KDropTargetEx* m_pDropTarget;
	KNseDrawButton m_lookupBtn;
	KToolTipWnd m_tooltipWnd;
	bool m_isShowLookupBtn;
	bool m_isFirstDragFile;
	UINT_PTR m_nIDEvent;
	byte m_nOpacity;
	byte m_nLastOpacity;

	typedef BOOL (__stdcall *PFUNCSETLAYEREDWINDOWATTR)(HWND, COLORREF, BYTE, DWORD);
	PFUNCSETLAYEREDWINDOWATTR m_fSetLayeredWindowAttributes;
};

#endif  //_KSUSPENSION_WND_H_
