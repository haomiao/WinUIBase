#include "stdafx.h"
#include "ktooltipwnd.h"

namespace
{
#define DLG_BORDER_COLOR 0XFFFFFF
#define DLG_BACKGROUND_COLOR 0XFFFFFF

#define IMAGE_TOP_SPACE 0

#define TOOLTIP_TOP_SPACE 4
#define TOOLTIP_LEFT_SPACE 0

#define TOOLTIP_TEXT_SIZE 13
#define TOOLTIP_TEXT_COLOR RGB(0x4D, 0x4D, 0x4D)

#define TOOLTIP_TEXT_DEFAULT _T("提示内容为空")
#define TOOLTIP_TEXT_SPACE 0

#define HEIGHT_TRIANG 5
}

KToolTipWnd::KToolTipWnd()
	: m_pTipBkImg(nullptr)
	, m_hTextFont(0)
	, m_nFontSize(TOOLTIP_TEXT_SIZE)
	, m_nBackgroundImgID((uint32_t)-1)
	, m_strToolTip(TOOLTIP_TEXT_DEFAULT)
	, m_rgbBackground(DLG_BACKGROUND_COLOR)
	, m_rgbToolTipText(TOOLTIP_TEXT_COLOR)
	, m_ePosToward(TOWARD_BOTTOM)
	, m_bIsSingleLine(true)
	, m_hToolTipRgn(NULL)
{
	::memset(&m_ptToolTip, 0, sizeof(m_ptToolTip));
}

KToolTipWnd::~KToolTipWnd()
{
	ReleaseResources();
}

void KToolTipWnd::SetToolTip(
	__in const std::wstring& strToolTip,
	__in COLORREF rgbBackground,
	__in COLORREF rgbToolTipText,
	__in uint32_t nFontSize,
	__in uint32_t nBackgroundImgID
	)
{
	m_strToolTip = strToolTip;
	m_vecStrToolTip.clear();

	// 拆分提示信息
	SplitStringW(m_strToolTip, L"\n", m_vecStrToolTip);

	m_rgbBackground = rgbBackground;
	m_rgbToolTipText = rgbToolTipText;
	if (nFontSize != (uint32_t)-1)
	{
		m_nFontSize = nFontSize;
		InitFonts();
	}

	if (nBackgroundImgID != (uint32_t)-1)
	{
		m_nBackgroundImgID = nBackgroundImgID;
		InitImages();
	}
}

void KToolTipWnd::ShowToolTip(
	__in const POINT& ptToolTip,
	__in TOOLTIP_POS_TOWARD ePosToward,
	__in bool bIsSingleLine)
{
	m_ptToolTip = ptToolTip;

	if (m_ePosToward != ePosToward)
	{
		m_ePosToward = ePosToward;
		UpdateToolTipRgn();
	}

	MoveWindowPos();
	
	m_bIsSingleLine = bIsSingleLine;

	if (m_hWnd && !::IsWindowVisible(m_hWnd))
	{
		ShowWindow(TRUE);
	}
	RePaint();
}

void KToolTipWnd::InitResources()
{
	InitFonts();
	InitImages();
}

void KToolTipWnd::InitControls()
{
	if (!(m_hWnd && ::IsWindow(m_hWnd)))
	{
		return;
	}

	UpdateToolTipRgn();
}

void KToolTipWnd::ReleaseResources()
{
	SAFE_FREE(m_pTipBkImg);
	SAFE_DELETE_OBJECT(m_hToolTipRgn);
}

void KToolTipWnd::InitFonts()
{
	SAFE_DELETE_OBJECT(m_hTextFont);

	HDC hDC = ::GetDC(m_hWnd);
	int nCaps = ::GetDeviceCaps(hDC, LOGPIXELSY);
	::ReleaseDC(m_hWnd, hDC);
	int nFontHeight = MulDiv(m_nFontSize, nCaps, 72);
	m_hTextFont = ::CreateFontW(
		nFontHeight,
		0,
		0,
		0,
		FW_NORMAL,
		FALSE, 
		FALSE,
		0,
		ANSI_CHARSET, 
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_SWISS,
		L"微软雅黑"
		);
}

void KToolTipWnd::InitImages()
{
	SAFE_FREE(m_pTipBkImg);
	if (m_nBackgroundImgID != (uint32_t)-1)
	{
		GetResourceImage(m_nBackgroundImgID, L"PNG", &m_pTipBkImg);
	}
}

void KToolTipWnd::UpdateToolTipRgn()
{
	SAFE_DELETE_OBJECT(m_hToolTipRgn);

	if (!m_hToolTipRgn)
	{
		CRect rcWnd;
		::GetClientRect(m_hWnd, &rcWnd);

		HRGN hTooltipRgn = NULL;

		POINT ptTriang[3];
		switch(m_ePosToward)
		{
		case TOWARD_LEFT:
			{
				m_hToolTipRgn = ::CreateRectRgn(0, 0, rcWnd.Width() - HEIGHT_TRIANG, rcWnd.Height());
				hTooltipRgn = ::CreateRectRgn(0, 0, rcWnd.Width() - HEIGHT_TRIANG, rcWnd.Height());
				ptTriang[0].x = rcWnd.Width()- HEIGHT_TRIANG;
				ptTriang[0].y = rcWnd.Height() / 2 - 3 ;
				ptTriang[1].x = rcWnd.Width();
				ptTriang[1].y = rcWnd.Height() / 2;
				ptTriang[2].x = rcWnd.Width() - HEIGHT_TRIANG - 1;
				ptTriang[2].y = rcWnd.Height() / 2 + 4;
			}
			break;
		case TOWARD_TOP:
			{
				m_hToolTipRgn = ::CreateRectRgn(0, 0, rcWnd.Width(), rcWnd.Height() - HEIGHT_TRIANG);
				hTooltipRgn = ::CreateRectRgn(0, 0, rcWnd.Width(), rcWnd.Height() - HEIGHT_TRIANG);
				ptTriang[0].x = rcWnd.Width() / 2 - 3;
				ptTriang[0].y = rcWnd.Height() - HEIGHT_TRIANG;
				ptTriang[1].x = rcWnd.Width() / 2;
				ptTriang[1].y = rcWnd.Height();
				ptTriang[2].x = rcWnd.Width() / 2 + 4;
				ptTriang[2].y = rcWnd.Height() - HEIGHT_TRIANG - 1;
			}
			break;
		case TOWARD_RIGHT:
			{
				m_hToolTipRgn = ::CreateRectRgn(HEIGHT_TRIANG, 0, rcWnd.Width(), rcWnd.Height());
				hTooltipRgn = ::CreateRectRgn(HEIGHT_TRIANG, 0, rcWnd.Width(), rcWnd.Height());
				ptTriang[0].x = HEIGHT_TRIANG;
				ptTriang[0].y = rcWnd.Height() / 2 - 3 ;
				ptTriang[1].x = 0;
				ptTriang[1].y = rcWnd.Height() / 2;
				ptTriang[2].x = HEIGHT_TRIANG - 1;
				ptTriang[2].y = rcWnd.Height() / 2 + 4;
			}
			break;
		case TOWARD_BOTTOM:
			{
				m_hToolTipRgn = ::CreateRectRgn(0, HEIGHT_TRIANG, rcWnd.Width(), rcWnd.Height());
				hTooltipRgn = ::CreateRectRgn(0, HEIGHT_TRIANG, rcWnd.Width(), rcWnd.Height());
				ptTriang[0].x = rcWnd.Width() / 2 - 3;
				ptTriang[0].y = HEIGHT_TRIANG;
				ptTriang[1].x = rcWnd.Width() / 2;
				ptTriang[1].y = 0;
				ptTriang[2].x = rcWnd.Width() / 2 + 4;
				ptTriang[2].y = HEIGHT_TRIANG - 1;
			}
			break;
		}

		HRGN hTriangRgn = ::CreatePolygonRgn(ptTriang, sizeof(ptTriang) / sizeof(POINT), WINDING);
		::CombineRgn(hTooltipRgn, hTooltipRgn, hTriangRgn, RGN_OR);
		::SetWindowRgn(m_hWnd, hTooltipRgn, FALSE);
		::DeleteObject(hTooltipRgn);

		::CombineRgn(m_hToolTipRgn, m_hToolTipRgn, hTriangRgn, RGN_OR);
		::DeleteObject(hTriangRgn);
	}
}

void KToolTipWnd::MoveWindowPos()
{
	if (m_hWnd)
	{
		CRect rcWnd,rcParent;
		::GetClientRect(m_hWnd, &rcWnd);
		::GetClientRect(GetParentWindowHandle(), &rcParent);
		int x = 0, y = 0;
		switch(m_ePosToward)
		{
		case TOWARD_LEFT:
			{
				x = m_ptToolTip.x - rcWnd.Width();
				y = m_ptToolTip.y + (rcParent.Height() - rcWnd.Height()) / 2;
			}
			break;
		case TOWARD_TOP:
			{
				x = m_ptToolTip.x;
				y = m_ptToolTip.y - rcWnd.Height();
			}
			break;
		case TOWARD_RIGHT:
			{
				x = m_ptToolTip.x + rcParent.Width();
				y = m_ptToolTip.y + (rcParent.Height() - rcWnd.Height()) / 2;
			}
			break;
		case TOWARD_BOTTOM:
			{
				x = m_ptToolTip.x;
				y = m_ptToolTip.y + rcParent.Height();
			}
			break;
		}
		::SetWindowPos(m_hWnd, NULL, x, y, 0, 0, SWP_NOSIZE);
	}
}

void KToolTipWnd::DrawBackground(HDC hDC)
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
	//::Rectangle(hDC, 0, 0, nWidth, nHeight);
	::FillRgn(hDC, m_hToolTipRgn, hFillBrush);
	::FrameRgn(hDC, m_hToolTipRgn, hFillBrush, 1, 1);
	::SelectObject(hDC, hOldFillBrush);
	::SelectObject(hDC, hOldBorderPen);
	SAFE_DELETE_OBJECT(hFillBrush);
	SAFE_DELETE_OBJECT(hBorderPen);
}

void KToolTipWnd::DrawOthers(HDC hDC)
{
	if (!m_hWnd)
	{
		return;
	}

	RECT rcWnd = {};
	::GetClientRect(m_hWnd, &rcWnd);
	int nWidth = rcWnd.right - rcWnd.left;
	int nHeight = rcWnd.bottom - rcWnd.top;

	// 绘制背景
	if (m_pTipBkImg)
	{
		Gdiplus::Graphics graphics(hDC);
		graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
		graphics.SetPixelOffsetMode(PixelOffsetModeHalf);
		graphics.SetSmoothingMode(SmoothingModeHighSpeed);

		Gdiplus::RectF rcImg; 
		rcImg.X = (Gdiplus::REAL)(nWidth - \
			(Gdiplus::REAL)m_pTipBkImg->GetWidth() * m_lfScale) / 2;
		rcImg.Y = IMAGE_TOP_SPACE * m_lfScale; 
		rcImg.Width = (Gdiplus::REAL)m_pTipBkImg->GetWidth() * m_lfScale; 
		rcImg.Height = (Gdiplus::REAL)m_pTipBkImg->GetHeight() * m_lfScale;

		graphics.DrawImage(
			m_pTipBkImg,
			rcImg,
			0,
			0,
			(Gdiplus::REAL)m_pTipBkImg->GetWidth(), 
			(Gdiplus::REAL)m_pTipBkImg->GetHeight(),
			Gdiplus::UnitPixel
			);
	}

	// 绘制提示文本
	if (!m_hTextFont)
	{
		return;
	}

	HFONT oldFont = (HFONT)::SelectObject(hDC, m_hTextFont);
	RECT rcTipText = {};

	switch (m_ePosToward)
	{
	case TOWARD_LEFT:
	case TOWARD_TOP:
	case TOWARD_RIGHT:
		{
			rcTipText.left = TOOLTIP_LEFT_SPACE * m_lfScale;
			rcTipText.top = TOOLTIP_TOP_SPACE * m_lfScale;
			rcTipText.right = rcTipText.left + nWidth;
			rcTipText.bottom = rcTipText.top + nHeight;
		}
		break;
	case TOWARD_BOTTOM:
		{
			rcTipText.left = TOOLTIP_LEFT_SPACE * m_lfScale;
			rcTipText.top = (TOOLTIP_TOP_SPACE + HEIGHT_TRIANG) * m_lfScale;
			rcTipText.right = rcTipText.left + nWidth;
			rcTipText.bottom = rcTipText.top + nHeight;
		}
		break;
	}

	::SetTextColor(hDC, m_rgbToolTipText);
	if (m_bIsSingleLine)
	{
		::DrawTextW(
			hDC,
			m_strToolTip.c_str(),
			(int)wcslen(m_strToolTip.c_str()),
			&rcTipText,
			DT_CENTER | DT_VCENTER | DT_SINGLELINE
			);
	}
	else
	{
		// 处理多行显示
		SIZE szExtent = {};
		::GetTextExtentExPoint(hDC, TOOLTIP_TEXT_DEFAULT,
			(int)wcslen(TOOLTIP_TEXT_DEFAULT),
			0, NULL, NULL, 
			&szExtent);
		
		rcTipText.bottom = rcTipText.top + szExtent.cy + TOOLTIP_TEXT_SPACE * m_lfScale;
		uint32_t uToolTipSize = (uint32_t)m_vecStrToolTip.size();
		for (uint32_t index = 0; index < uToolTipSize; ++index)
		{
			::DrawTextW(
				hDC,
				m_vecStrToolTip[index].c_str(),
				(int)wcslen(m_vecStrToolTip[index].c_str()),
				&rcTipText,
				DT_CENTER | DT_VCENTER | DT_SINGLELINE
				);
			rcTipText.top = rcTipText.bottom;
			rcTipText.bottom = rcTipText.top + szExtent.cy + TOOLTIP_TEXT_SPACE * m_lfScale;
		}
	}

	::SelectObject(hDC, oldFont);
}

void KToolTipWnd::DrawControls(HDC hDC)
{
	
}
