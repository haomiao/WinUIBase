#include "stdafx.h"
#include "kpopuptipwnd.h"


KPopupTipWnd::KPopupTipWnd()
{

}

KPopupTipWnd::~KPopupTipWnd()
{
	ReleaseResources();
}

LRESULT KPopupTipWnd::OnPaint(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!(m_hWnd && ::IsWindow(m_hWnd)))
	{
		return 0;
	}

	RECT rcWnd = {};
	::GetClientRect(m_hWnd, &rcWnd);
	if (::IsRectEmpty(&rcWnd))
	{
		return 0;
	}

	int nWidth = rcWnd.right - rcWnd.left;
	int nHeight = rcWnd.bottom - rcWnd.top;

	PAINTSTRUCT ps;
	HDC hDC = ::BeginPaint(m_hWnd, &ps);
	HDC hMemDC = ::CreateCompatibleDC(hDC);
	HBITMAP hBmpMem = ::CreateCompatibleBitmap(hDC, nWidth, nHeight);
	HBITMAP hOldBmpMem = (HBITMAP)::SelectObject(hMemDC, hBmpMem);
	::SetBkMode(hMemDC, TRANSPARENT);

	DrawBackground(hMemDC);
	DrawOthers(hMemDC);
	DrawControls(hMemDC);

	::BitBlt(hDC, 0, 0, nWidth, nHeight, hMemDC, 0, 0, SRCCOPY);
	::SelectObject(hMemDC, hOldBmpMem);

	SAFE_DELETE_OBJECT(hBmpMem);
	SAFE_DELETE_DC(hMemDC);

	::EndPaint(m_hWnd, &ps);

	bHandled = TRUE;
	return 0;
}

BOOL KPopupTipWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

LRESULT KPopupTipWnd::OnCommand(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	return (LRESULT)OnCommand(wParam, lParam);
}

LRESULT KPopupTipWnd::OnSize(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	return (LRESULT)OnSize(wParam, lParam);
}

BOOL KPopupTipWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

LRESULT KPopupTipWnd::OnKeyUp(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	return (LRESULT)OnKeyUp(wParam, lParam);
}

BOOL KPopupTipWnd::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

void KPopupTipWnd::InitializeWnd()
{
	if (!(m_hWnd && ::IsWindow(m_hWnd)))
	{
		return;
	}

	InitResources();
	InitControls();
}

void KPopupTipWnd::DrawBackground(HDC hDC)
{
	
}

void KPopupTipWnd::DrawOthers(HDC hDC)
{

}

void KPopupTipWnd::DrawControls(HDC hDC)
{

}

void KPopupTipWnd::InitControls()
{

}

void KPopupTipWnd::InitResources()
{

}

void KPopupTipWnd::ReleaseResources()
{
	
}

