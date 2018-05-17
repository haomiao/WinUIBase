#include "stdafx.h"
#include "kmenuwnd.h"
#include <algorithm>

namespace
{
#define TOP_SPACE 4
#define BOTTOM_SPACE 4
#define DLG_BORDER_COLOR RGB(0xDD, 0xDD, 0xDD)
#define DLG_BACKGROUND_COLOR RGB(0xFF, 0xFF, 0xFF)

#define IMAGE_TOP_HOLLOW 1
#define IMAGE_LEFT_HOLLOW 12
#define IMAGE_RIGHT_HOLLOW 8
#define IMAGE_BOTTOM_HOLLOW 2

#define ICON_WIDTH 20
#define ICON_STR_SPACE 5
#define MENU_ITEM_TEXT_FONT_HEIGHT 13 // 12
#define MENU_ITEM_TEXT_COLOR RGB(0x42, 0x42, 0x42)
#define MENU_ITEM_BACKGROUND_COLOR RGB(0x00, 0x00, 0x00)
#define MENU_ITEM_HOVER_BACKGROUND_COLOR RGB(0xDF, 0xEB, 0xF9)
#define MENU_ITEM_DISABLE_BACKGROUND_COLOR RGB(190, 190, 190)
#define MENU_ITEM_LINE_COLOR RGB(0xE8, 0xE8, 0xE8)
#define MENU_ITEM_LINE_HEIGHT 9
}

HHOOK KMenuWnd::m_hCallProcHook = nullptr;
KMenuWnd* KMenuWnd::m_pSuspensionMenuWnd = nullptr;

KMenuWnd::KMenuWnd()
	: m_hTextFont(NULL)
	, m_pBackgroundImg(nullptr)
	, m_nRetID((UINT)-1)
	, m_bTrackMouse(false)
{

}

KMenuWnd::~KMenuWnd()
{
	ReleaseResources();
	m_vecMenuPair.clear();
	m_vecMenuRect.clear();
}

UINT KMenuWnd::ShowMenuWnd(__in const POINT &ptPos)
{
	if (m_hWnd && ::IsWindow(m_hWnd))
	{
		::SetWindowPos(
			m_hWnd, nullptr,
			ptPos.x, ptPos.y,
			0, 0,
			SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOSIZE
			);
		ReSetItemRect();
		HookWndProc();
		MsgPump();
		UnHookWndProc();
	}
	return m_nRetID;
}

void KMenuWnd::AddMenu(
	__in UINT nMenuID,
	__in UINT nOffset,
	__in const std::wstring& strMenu,
	__in UINT nIconID
	)
{
	MenuItemPair menuPair = {nMenuID, nOffset, strMenu, nIconID};
	AddMenu(menuPair);
}

void KMenuWnd::AddMenu(__in const MenuItemPair& menuPair)
{
	if(std::find(m_vecMenuPair.begin(), m_vecMenuPair.end(), menuPair)
		!= m_vecMenuPair.end())
	{
		return;
	}

	m_vecMenuPair.emplace_back(menuPair);

	ReSetItemRect();

	if (m_hWnd && ::IsWindow(m_hWnd) && ::IsWindowVisible(m_hWnd))
	{
		RePaint();
	}
}

void KMenuWnd::InitResources()
{
	InitFonts();
	InitImages();
}

void KMenuWnd::InitControls()
{
	m_pSuspensionMenuWnd = this;
}

void KMenuWnd::ReleaseResources()
{
	SAFE_DELETE_OBJECT(m_hTextFont);
	SAFE_FREE(m_pBackgroundImg);
}

LRESULT KMenuWnd::OnMouseMove(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_bTrackMouse)
	{
		TRACKMOUSEEVENT MouseEvent = {sizeof(TRACKMOUSEEVENT)};
		MouseEvent.dwFlags = TME_LEAVE;
		MouseEvent.hwndTrack = m_hWnd;
		MouseEvent.dwHoverTime = 1000;
		if(::_TrackMouseEvent(&MouseEvent))
		{
			m_bTrackMouse = true;
		}
	}

	POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
	UINT nMenuItemIndex = GetMenuPairIndex(pt);
	m_nRetID = (UINT)-1;
	if (nMenuItemIndex != (UINT)-1)
	{
		m_nRetID = m_vecMenuPair[nMenuItemIndex].nMenuID;
	}
	RePaint();

	bHandled = FALSE;
	return 0;
}

LRESULT KMenuWnd::OnMouseLeave(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_bTrackMouse = false;
	m_nRetID = (UINT)-1;
	RePaint();

	bHandled = FALSE;
	return 0;
}

LRESULT KMenuWnd::OnLButtonDown(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::PostMessage(m_hWnd, WM_QUIT, WPARAM(m_nRetID), 0);
	bHandled = TRUE;
	return 0;
}

void KMenuWnd::InitFonts()
{
	HDC hDC = ::GetDC(NULL);
	int nCaps = ::GetDeviceCaps(hDC,LOGPIXELSY);
	::ReleaseDC(m_hWnd, hDC);

	if (!m_hTextFont)
	{
		int nFontHeight = MulDiv(MENU_ITEM_TEXT_FONT_HEIGHT, nCaps, 72);
		m_hTextFont = ::CreateFontW(
			nFontHeight, 0, 0, 0, FW_NORMAL, FALSE, 
			FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
			DEFAULT_PITCH | FF_SWISS, L"微软雅黑"
			);
	}
}

void KMenuWnd::InitImages()
{
	if (m_pBackgroundImg)
	{
		return;
	}

	//GetResourceImage(IDP_BACKGROUND_IMG, L"PNG", &m_pBackgroundImg);
}

void KMenuWnd::HookWndProc()
{
	m_hCallProcHook = ::SetWindowsHookEx(
		WH_CALLWNDPROC,
		(HOOKPROC)_HookCallWndProc,
		NULL,
		GetCurrentThreadId()
		);
}

void KMenuWnd::UnHookWndProc()
{
	if(m_hCallProcHook)
	{
		::UnhookWindowsHookEx(m_hCallProcHook);
		m_hCallProcHook = nullptr;
	}
}

void KMenuWnd::MsgPump()
{
	if(!m_hWnd)
	{
		return;
	}

	HWND hWndParent = ::GetWindowOwner(m_hWnd);
	::ShowWindow(m_hWnd, SW_SHOWNORMAL);
	::EnableWindow(hWndParent, FALSE);

	m_bHide = FALSE;
	MSG msg = {};
	while(::GetMessage(&msg, NULL, 0, 0))
	{
		if((msg.message == WM_LBUTTONDOWN && msg.hwnd != m_hWnd)
			|| msg.message == WM_NCLBUTTONDOWN)
		{
			POINT point = {};
			::GetCursorPos(&point);
			RECT rcTipWnd = {};
			::GetWindowRect(m_hWnd, &rcTipWnd);
			if(!::PtInRect(&rcTipWnd, point))
			{
				ShowWindow(FALSE);
			}
		}

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
		if(m_bHide)
		{
			break;
		}
	}

	::EnableWindow(hWndParent, TRUE);
	::SetFocus(hWndParent);
}

UINT KMenuWnd::GetMenuPairIndex(const POINT& pt)
{
	UINT nMenuPairSize = (UINT)m_vecMenuPair.size();
	UINT nMenuIndx = (UINT)-1;
	for (UINT index = 0; index < nMenuPairSize; ++index)
	{
		if (::PtInRect(&m_vecMenuRect[index], pt))
		{
			nMenuIndx = index;
			break;
		}
	}
	return nMenuIndx;
}

void KMenuWnd::ReSetItemRect()
{
	if (!(m_hWnd && ::IsWindow(m_hWnd)))
	{
		return;
	}

	m_vecMenuRect.clear();

	CRect rcWnd;
	::GetClientRect(m_hWnd, &rcWnd);
	UINT nLineCount = GetMenuLineItemCount();
	UINT nMenuPairSize = (UINT)m_vecMenuPair.size();
	UINT nMenuValidSize = nMenuPairSize - nLineCount;
	LONG nPearHeight = (LONG)(
		(rcWnd.Height() - (TOP_SPACE + BOTTOM_SPACE 
		+ nLineCount * MENU_ITEM_LINE_HEIGHT) * m_lfScale
		) / (nMenuValidSize * 1.0));
	LONG nLeftTopX = 0;
	LONG nLeftTopY = (LONG)(TOP_SPACE * m_lfScale);
	LONG nRightBottomX = rcWnd.Width();
	LONG nRightBottomY = nLeftTopY;
	for (UINT index = 0; index < nMenuPairSize; ++index)
	{
		nLeftTopX = m_vecMenuPair[index].nOffset * m_lfScale;
		nLeftTopY = nRightBottomY;
		if (m_vecMenuPair[index].nMenuID == (UINT)-1)
		{	
			nRightBottomY = nLeftTopY + MENU_ITEM_LINE_HEIGHT * m_lfScale;
		}
		else
		{
			nRightBottomY = nLeftTopY + nPearHeight;
		}

		RECT rc = {nLeftTopX, nLeftTopY, nRightBottomX, nRightBottomY};
		m_vecMenuRect.emplace_back(rc);
	}
}

UINT KMenuWnd::GetMenuLineItemCount()
{
	UINT nMenuPairSize = (UINT)m_vecMenuPair.size();
	UINT nLineCount = 0;
	for (UINT index = 0; index < nMenuPairSize; ++index)
	{
		if (m_vecMenuPair[index].nMenuID == (UINT)-1)
		{
			++nLineCount;
		}
	}
	return nLineCount;
}

LRESULT CALLBACK KMenuWnd::_HookCallWndProc(
	__in int nCode,
	__in WPARAM wParam,
	__in LPARAM lParam
	)
{
	if (!m_hCallProcHook)
	{
		return 0;
	}

	if(nCode == HC_ACTION && m_pSuspensionMenuWnd)
	{
		CWPSTRUCT *pcwp = (CWPSTRUCT *)lParam;
		if (pcwp)
		{
			if(pcwp->message == WM_ACTIVATE 
				&& pcwp->hwnd != m_pSuspensionMenuWnd->m_hWnd)
			{
				if(LOWORD(pcwp->wParam) == WA_INACTIVE)
				{
					m_pSuspensionMenuWnd->ShowWindow(FALSE);
					return 0;
				}
			}
			else if(pcwp->message == WM_PARENTNOTIFY 
				&& pcwp->hwnd != m_pSuspensionMenuWnd->m_hWnd)
			{
				if(pcwp->wParam == WM_LBUTTONDOWN 
					|| pcwp->wParam == WM_RBUTTONDOWN)
				{
					m_pSuspensionMenuWnd->ShowWindow(FALSE);
					return 0;
				}
			}
		}
	}
	return ::CallNextHookEx(m_hCallProcHook, nCode, wParam, lParam);
}

void KMenuWnd::DrawBackground(HDC hDC)
{
	if (!m_hWnd)
	{
		return;
	}

	RECT rcWnd = {};
	::GetClientRect(m_hWnd, &rcWnd);
	int nWidth = rcWnd.right - rcWnd.left;
	int nHeight = rcWnd.bottom - rcWnd.top;

	HBRUSH hFillBrush = ::CreateSolidBrush(DLG_BACKGROUND_COLOR);
	HPEN hBorderPen = ::CreatePen(PS_SOLID, 1, DLG_BORDER_COLOR);
	HBRUSH hOldFillBrush = (HBRUSH)::SelectObject(hDC, hFillBrush);
	HBRUSH hOldBorderPen = (HBRUSH)::SelectObject(hDC, hBorderPen);
	::Rectangle(hDC, 0, 0, nWidth, nHeight);
	::SelectObject(hDC, hOldFillBrush);
	::SelectObject(hDC, hOldBorderPen);
	SAFE_DELETE_OBJECT(hFillBrush);
	SAFE_DELETE_OBJECT(hBorderPen);
}

void KMenuWnd::DrawOthers(HDC hDC)
{
	if (!m_hWnd)
	{
		return;
	}

	RECT rcWnd = {};
	::GetClientRect(m_hWnd, &rcWnd);
	int nWidth = rcWnd.right - rcWnd.left;
	int nHeight = rcWnd.bottom - rcWnd.top;

	// 绘制背景图片
	if (m_pBackgroundImg)
	{
		Gdiplus::Graphics graphics(hDC);
		graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
		graphics.SetPixelOffsetMode(PixelOffsetModeHalf);
		graphics.SetSmoothingMode(SmoothingModeHighSpeed);

		Gdiplus::RectF rcImg; 
		rcImg.X = IMAGE_LEFT_HOLLOW * m_lfScale;
		rcImg.Y = IMAGE_TOP_HOLLOW * m_lfScale; 
		rcImg.Width = m_pBackgroundImg->GetWidth() * m_lfScale; 
		rcImg.Height = m_pBackgroundImg->GetHeight() * m_lfScale;

		graphics.DrawImage(
			m_pBackgroundImg,
			rcImg,
			0,
			0,
			(Gdiplus::REAL)m_pBackgroundImg->GetWidth(), 
			(Gdiplus::REAL)m_pBackgroundImg->GetHeight(),
			Gdiplus::UnitPixel
			);
	}

	// 绘制各菜单项
	HFONT oldFont = (HFONT)::SelectObject(hDC, m_hTextFont);
	::SetTextColor(hDC, MENU_ITEM_TEXT_COLOR);

	UINT nMenuPairSize = (UINT)m_vecMenuPair.size();
	for (UINT index = 0; index < nMenuPairSize; ++index)
	{
		RECT rc = m_vecMenuRect[index];

		// 绘制分隔符
		if (m_vecMenuPair[index].nMenuID == (UINT)-1)
		{
			int nRcWidth = rc.right - rc.left;
			int nRcHeight = rc.bottom - rc.top;
			POINT ptLineStart = 
				{rc.left, rc.top + nRcHeight / 2};
			POINT ptLineEnd = 
				{rc.right - m_vecMenuPair[index].nOffset * m_lfScale, ptLineStart.y};
			HPEN hDivPen = CreatePen(PS_SOLID, 1, MENU_ITEM_LINE_COLOR);
			HPEN hOldPen = (HPEN)::SelectObject(hDC, hDivPen);
			::MoveToEx(hDC, ptLineStart.x, ptLineStart.y, NULL);
			::LineTo(hDC, ptLineEnd.x, ptLineEnd.y);
			::DeleteObject(hDivPen);
			::SelectObject(hDC, hOldPen);
			continue;
		}

		// 高亮绘制选中的菜单项背景
		if (m_nRetID == m_vecMenuPair[index].nMenuID)
		{
			HBRUSH hFillBrush = 
				::CreateSolidBrush(MENU_ITEM_HOVER_BACKGROUND_COLOR);
			::SelectObject(hDC, hFillBrush);
			RECT rcFill = rc;
			rcFill.left = 0;
			::FillRect(hDC, &rcFill, hFillBrush);
			::DeleteObject(hFillBrush);
		}

		// 绘制图标和文本信息
		if (m_vecMenuPair[index].nIconID != (UINT)-1)
		{
			// TODO: 暂时未添加图标支持
		}
		
		::DrawTextW(
			hDC,
			m_vecMenuPair[index].strMenu.c_str(),
			(int)wcslen(m_vecMenuPair[index].strMenu.c_str()),
			&rc,
			DT_LEFT | DT_VCENTER | DT_SINGLELINE
			);
	}
	::SelectObject(hDC, oldFont);
}

void KMenuWnd::DrawControls(HDC hDC)
{
	
}
