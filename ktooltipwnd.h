/*++ BUILD Version: 0001    

Module Name:

        ktooltipwnd.h 

Abstract:

        ToolTip提示窗体

Author:

        zhouhao3   2018-5-9

Revision History:

--*/

#ifndef _KTOOL_TIP_WND_H_
#define _KTOOL_TIP_WND_H_

#include <GdiPlus.h>
#include <string>
#include "kpopuptipwnd.h"

class KToolTipWnd : public KPopupTipWnd
{
public:

	 /*++
	  * 窗口显示所在点位置
	--*/
    enum TOOLTIP_POS_TOWARD
    {
        TOWARD_LEFT = 0,
        TOWARD_RIGHT,
        TOWARD_TOP,
        TOWARD_BOTTOM,
    };

    KToolTipWnd();
    virtual ~KToolTipWnd();

public:

	/*++
	 * 设置ToolTip,对于支持多行显示的文本,目前则需要以"\n"作为分行标识
   --*/
	void SetToolTip(
		__in const std::wstring& strToolTip,
		__in COLORREF rgbBackground = RGB(0xFF, 0xFF, 0xFF),
		__in COLORREF rgbToolTipText = RGB(0x4D, 0x4D, 0x4D),
		__in uint32_t nFontSize = (uint32_t)-1,
		__in uint32_t nBackgroundImgID = (uint32_t)-1
		);

	/*++
	 * 显示ToolTip
	 * ptToolTip: 一般为对应父窗口左上角或其他指定相对位置
   --*/
	void ShowToolTip(
		__in const POINT& ptToolTip,
		__in TOOLTIP_POS_TOWARD ePosToward = TOWARD_BOTTOM,
		__in bool bIsSingleLine = true
		);

private:

	virtual void InitResources();
	virtual void InitControls();
	virtual void ReleaseResources();

	virtual void DrawBackground(HDC hDC);
	virtual void DrawOthers(HDC hDC);
	virtual void DrawControls(HDC hDC);

private:

	void InitFonts();
	void InitImages();
	void UpdateToolTipRgn();
	void MoveWindowPos();
	void AutoAdjustWindow();

private:

	Gdiplus::Image *m_pTipBkImg;
	HFONT m_hTextFont;
	uint32_t m_nFontSize;
	uint32_t m_nBackgroundImgID;
	std::wstring m_strToolTip;
	std::vector<std::wstring> m_vecStrToolTip;
	COLORREF m_rgbBackground;
	COLORREF m_rgbToolTipText;
	POINT m_ptToolTip;
	TOOLTIP_POS_TOWARD m_ePosToward;
	bool m_bIsSingleLine;
	HRGN m_hToolTipRgn;
};

#endif  //_KTOOL_TIP_WND_H_
