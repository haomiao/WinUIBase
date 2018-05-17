#include "stdafx.h"
#include "ksuspensionwnd.h"
#include "ctrl/kmenuwnd.h"
#include "kvirtualfolder.h"

namespace
{
#define IDT_TIMER 1101
#define ELAPSE_MILSEC_TIME 800

#define TOP_SPACE 0
#define ROUND_RECT_SIDE 2

#define IMAGE_BACKGROUND_NORMAL_COLOR RGB(0x5B, 0x9C, 0xFF)
#define IMAGE_BACKGROUND_HOVER_COLOR RGB(0x3D, 0x7B, 0xDE)

#define DLG_BORDER_COLOR RGB(0x3D, 0x7B, 0xDE)
#define DLG_BACKGROUND_COLOR RGB(0xE7, 0xF1, 0xFF)
#define DLG_BORDER_HOVER_COLOR IMAGE_BACKGROUND_HOVER_COLOR
#define DLG_BACKGROUND_HOVER_COLOR DLG_BACKGROUND_COLOR

#define TEXT_FONT_HEIGHT 13 //12

#define IMAGE_TOP_HOLLOW 6
#define IMAGE_LEFT_HOLLOW 4
#define IMAGE_RIGHT_HOLLOW 4
#define IMAGE_BOTTOM_HOLLOW 6
#define IMAGE_WIDTH 30

#define TIP_INFO_TEXT_LEFT_SPACE 10
#define TIP_INFO_TEXT_COLOR RGB(0x4E, 0x4E, 0x4E)
#define TIP_INFO_TEXT _T("拖拽添加到文档")
#define TIP_DRAG_TEXT _T("松开左键开始添加")
#define TIP_ADDING_TEXT _T("正在添加...")
#define TIP_FINISHED_TEXT _T("或继续添加")
#define LOOKUP_BTN_TEXT _T("查看")
#define LOOKUP_BTN_TEXT_COLOR RGB(0x44, 0x86, 0xF0)
#define LOOKUP_BTN_TEXT_SIZE TEXT_FONT_HEIGHT //11
#define LOOKUP_TIP_TEXT _T("点击查看拖拽所在目录")

#define TOOLTIP_TEXT _T("快速添加到:\n拖拽文件 / 文件夹 到此")

#define MENUITEM_OPEN_WPSCLOUD _T("打开文档")
#define MENUITEM_LOOKUP_TRANS_DETAIL _T("查看传输详情")
#define MENUITEM_CLOSE _T("关闭")

#define MENUITEM_OFFSET 12
#define HOVER_OPACITY_VALUE 255
}

KSuspensionWnd::KSuspensionWnd()
	: m_hTextFont(NULL)
	, m_pIconImg(nullptr)
	, m_hRgn(NULL)
	, m_bIsLBtnDown(false)
	, m_clBackground(DLG_BACKGROUND_COLOR)
	, m_clBorder(DLG_BORDER_COLOR)
	, m_clIconBackground(IMAGE_BACKGROUND_NORMAL_COLOR)
	, m_bTrackMouse(false)
	, m_strInfoText(TIP_INFO_TEXT)
	, m_pDropTarget(nullptr)
	, m_isShowLookupBtn(false)
	, m_isFirstDragFile(true)
	, m_nIDEvent(0)
	, m_nOpacity(255)
	, m_nLastOpacity(255)
	, m_fSetLayeredWindowAttributes(nullptr)
{

}

KSuspensionWnd::~KSuspensionWnd()
{
	StopTimer();
	ReleaseResources();

	if (m_pDropTarget)
	{
		m_pDropTarget->UnRegister();
		delete m_pDropTarget;
	}
}

BOOL KSuspensionWnd::CreateSuspensionWnd(
	__in HWND hParentWnd,
	__in int x,
	__in int y,
	__in int cx,
	__in int cy,
	__in double lfScale
	)
{
	return __super::CreateWnd(
		hParentWnd,
		L"KSuspensionWnd",
		L"", 
		WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
		x,
		y,
		cx * m_lfScale,
		cy * m_lfScale,
		m_lfScale
		);
}

BOOL KSuspensionWnd::SetOpacity(BYTE nOpacity)
{
	m_nOpacity = nOpacity;
	m_nLastOpacity = m_nOpacity;
	BOOL bResult = ChangeTransparency();
	RePaint();
	return bResult;
}

void KSuspensionWnd::InitResources()
{
	InitFonts();
	InitImages();
}

void KSuspensionWnd::InitControls()
{
	if (!(m_hWnd && ::IsWindow(m_hWnd)))
	{
		return;
	}
	
	CRect rcWnd;
	::GetClientRect(m_hWnd, &rcWnd);

	if (!m_hRgn)
	{
		m_hRgn = ::CreateRoundRectRgn(
			0, 0,
			rcWnd.Width() * m_lfScale, rcWnd.Height() * m_lfScale,
			ROUND_RECT_SIDE * m_lfScale, ROUND_RECT_SIDE * m_lfScale
			);
		::SetWindowRgn(m_hWnd, m_hRgn, FALSE);
	}

	if (!m_pDropTarget)
	{
		m_pDropTarget = new KDropTargetEx(DROP_REGISTER_SUSPENSION);
		m_pDropTarget->Register(m_hWnd);
		::DragAcceptFiles(m_hWnd, TRUE);
	}

	if (!m_lookupBtn.GetSafeHWND())
	{
		RECT rcBtn = {};
		rcBtn.left = 0;
		rcBtn.top = TOP_SPACE * m_lfScale;
		rcBtn.right = 0;
		rcBtn.bottom = 0;
		m_lookupBtn.SetNeedScale(TRUE, m_lfScale);
		m_lookupBtn.CreateLinkBtn(
			LOOKUP_BTN_TEXT,
			rcBtn,
			m_hWnd,
			IDM_LOOKUP_BTN,
			LOOKUP_BTN_TEXT_COLOR,
			DLG_BACKGROUND_COLOR,
			TRUE,
			nullptr,
			LOOKUP_BTN_TEXT_SIZE
			);
		m_lookupBtn.EnableButton(TRUE);
		m_lookupBtn.SetToolTipText(LOOKUP_TIP_TEXT);
		m_lookupBtn.ShowBtn(FALSE);
	}

	if (!m_tooltipWnd.GetWindowHandle())
	{
		m_tooltipWnd.CreateWnd(
			m_hWnd,
			L"ToolTipWnd",
			L"",
			WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
			0,
			0,
			rcWnd.Width(),
			46,
			m_lfScale
			);
		m_tooltipWnd.SetToolTip(TOOLTIP_TEXT);
	}
}

void KSuspensionWnd::ReleaseResources()
{
	SAFE_DELETE_OBJECT(m_hTextFont);
	SAFE_FREE(m_pIconImg);
	SAFE_DELETE_OBJECT(m_hRgn);
}

BOOL KSuspensionWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch(HIWORD(wParam))
	{
	case BN_CLICKED:
		{
			switch(LOWORD(wParam))
			{
			case IDR_SUSPENSION_OPEN:
				{
					OpenWPSCloud();
				}
				break;
			case IDR_SUSPENSION_TRANS_DETAIL:
				{
					ShowTransDetail();
				}
				break;
			case IDR_SUSPENSION_CLOSE:
				{
					ShowWindow(FALSE);
				}
				break;
			case IDM_LOOKUP_BTN:
				{
					OpenDragDir();
				}
				break;
			default:
				break;
			}
		}
		break;
	}
	return TRUE;
}

LRESULT KSuspensionWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bHandled = FALSE;
	LRESULT lResult = 0;
	switch(uMsg)
	{
	case WM_CONTEXTMENU:
		{
			lResult = OnContextMenu(wParam, lParam, bHandled);
		}
		break;
	case WM_DROPFILES:
		{
			lResult = OnDropFiles(wParam, lParam, bHandled);
		}
		break;
	case IDM_DROP_ENTER:
		{
			lResult = OnDropEnter(wParam, lParam, bHandled);
		}
		break;
	case IDM_DROP_LEAVE:
		{
			lResult = OnDropLeave(wParam, lParam, bHandled);
		}
		break;
	}

	if(bHandled)
	{
		return lResult;
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT KSuspensionWnd::OnMouseMove(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	if (m_bIsLBtnDown)
	{
		RECT rcWnd = {};
		::GetClientRect(m_hWnd, &rcWnd);
		POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		if (::PtInRect(&rcWnd, pt))
		{
			::PostMessage(
				m_hWnd,
				WM_NCLBUTTONDOWN,
				HTCAPTION,
				MAKELPARAM(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))
				);
		}
	}

	if (!m_bTrackMouse)
	{
		TRACKMOUSEEVENT MouseEvent = {sizeof(TRACKMOUSEEVENT)};
		MouseEvent.dwFlags = TME_LEAVE | TME_HOVER;
		MouseEvent.hwndTrack = m_hWnd;
		MouseEvent.dwHoverTime = 10;
		if(::_TrackMouseEvent(&MouseEvent))
		{
			m_bTrackMouse = true;
		}
	}
	return 0;
}

LRESULT KSuspensionWnd::OnMouseHover(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	m_clBackground = DLG_BACKGROUND_HOVER_COLOR;
	m_clBorder = DLG_BORDER_HOVER_COLOR;
	m_clIconBackground = IMAGE_BACKGROUND_HOVER_COLOR;
	m_nOpacity = HOVER_OPACITY_VALUE;
	ChangeTransparency();

	RePaint();

	ShowToolTip();
	return 0;
}

LRESULT KSuspensionWnd::OnMouseLeave(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_bTrackMouse = false;
	m_clBackground = DLG_BACKGROUND_COLOR;
	m_clBorder = DLG_BORDER_COLOR;
	m_clIconBackground = IMAGE_BACKGROUND_NORMAL_COLOR;
	m_nOpacity = m_nLastOpacity;
	ChangeTransparency();

	RePaint();
	
	if (m_tooltipWnd.GetWindowHandle()
		&& ::IsWindowVisible(m_tooltipWnd.GetWindowHandle()))
	{
		m_tooltipWnd.ShowWindow(FALSE);
	}

	bHandled = FALSE;
	return 0;
}

LRESULT KSuspensionWnd::OnLButtonDown(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_bIsLBtnDown = true;
	if (m_tooltipWnd.GetWindowHandle()
		&& ::IsWindowVisible(m_tooltipWnd.GetWindowHandle()))
	{
		m_tooltipWnd.ShowWindow(FALSE);
	}
	bHandled = FALSE;
	return 0;
}

LRESULT KSuspensionWnd::OnLButtonUp(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_bIsLBtnDown = false;
	bHandled = FALSE;
	return 0;
}

LRESULT KSuspensionWnd::OnLButtonDBClk(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// 双击打开WPS云文档
	OpenWPSCloud();
	bHandled = TRUE;
	return 0;
}

LRESULT KSuspensionWnd::OnMove(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int nX = (int)(short)LOWORD(lParam);
	int nY = (int)(short)HIWORD(lParam);
	int nCx = ::GetSystemMetrics(SM_CXFULLSCREEN);
	int nCy = ::GetSystemMetrics(SM_CYFULLSCREEN);
	CRect rcWnd;
	::GetClientRect(m_hWnd, &rcWnd);
	bool bShouldReMove = false;
	if (nX < 0)
	{
		nX = 0;
		bShouldReMove = true;
	}

	if (nX + rcWnd.Width() > nCx)
	{
		nX = nCx - rcWnd.Width();
		bShouldReMove = true;
	}

	if (nY < 0)
	{
		nY = 0;
		bShouldReMove = true;
	}

	if (nY > nCy)
	{
		nY = nCy;
		bShouldReMove = true;
	}

	if (bShouldReMove)
	{
		::SetWindowPos(m_hWnd, nullptr, nX, nY, 0, 0, SWP_NOSIZE);
	}

	bHandled = TRUE;
	return 0;
}

LRESULT KSuspensionWnd::OnTimer(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == IDT_TIMER)
	{
		StopTimer();

		m_clBackground = DLG_BACKGROUND_COLOR;
		m_clBorder = DLG_BORDER_COLOR;
		m_clIconBackground = IMAGE_BACKGROUND_NORMAL_COLOR;
		m_strInfoText = TIP_FINISHED_TEXT;
		m_isShowLookupBtn = true;
		m_isFirstDragFile = false;
		RePaint();
	}

	bHandled = TRUE;
	return 0;
}

LRESULT KSuspensionWnd::OnDropFiles(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HDROP hDrop = (HDROP)wParam;
	if (!hDrop)
	{
		return 0;
	}

	m_clBackground = DLG_BACKGROUND_COLOR;
	m_clBorder = DLG_BORDER_COLOR;
	m_clIconBackground = IMAGE_BACKGROUND_NORMAL_COLOR;
	m_strInfoText = TIP_ADDING_TEXT;
	RePaint();

	StopTimer();
	m_nIDEvent =
		::SetTimer(m_hWnd, IDT_TIMER, ELAPSE_MILSEC_TIME, (TIMERPROC)nullptr);

	// 获取拖拽文件列表
	std::list<std::wstring> listFileName;
	UINT nFileNumber = ::DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);
	WCHAR szPath[MAX_PATH];
	for (UINT index = 0; index < nFileNumber; ++index)
	{
		::DragQueryFileW(hDrop, index, szPath, MAX_PATH);
		listFileName.emplace_back(szPath);
	}
	::DragFinish(hDrop);

	bHandled = TRUE;
	return 0;
}

LRESULT KSuspensionWnd::OnDropEnter(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_clBackground = DLG_BACKGROUND_HOVER_COLOR;
	m_clBorder = DLG_BORDER_HOVER_COLOR;
	m_clIconBackground = IMAGE_BACKGROUND_HOVER_COLOR;
	m_strInfoText = TIP_DRAG_TEXT;
	m_isShowLookupBtn = false;
	m_nOpacity = HOVER_OPACITY_VALUE;
	ChangeTransparency();

	if (m_lookupBtn.GetSafeHWND()
		&& ::IsWindowVisible(m_lookupBtn.GetSafeHWND()))
	{
		m_lookupBtn.ShowBtn(FALSE);
	}
	RePaint();

	bHandled = TRUE;
	return 0;
}

LRESULT KSuspensionWnd::OnDropLeave(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_clBackground = DLG_BACKGROUND_COLOR;
	m_clBorder = DLG_BORDER_COLOR;
	m_clIconBackground = IMAGE_BACKGROUND_NORMAL_COLOR;
	m_nOpacity = m_nLastOpacity;
	ChangeTransparency();

	if (!m_isFirstDragFile)
	{
		m_isShowLookupBtn = true;
	}

	if (!m_isShowLookupBtn)
	{
		m_strInfoText = TIP_INFO_TEXT;
	}
	else
	{
		m_strInfoText = TIP_FINISHED_TEXT;
	}
	
	RePaint();

	bHandled = TRUE;
	return 0;
}

LRESULT KSuspensionWnd::OnContextMenu(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt;
	::GetCursorPos(&pt);
	ShowCustomMenus(pt);

	bHandled = TRUE;
	return 0;
}

void KSuspensionWnd::InitFonts()
{
	HDC hDC = ::GetDC(NULL);
	int nCaps = ::GetDeviceCaps(hDC,LOGPIXELSY);
	::ReleaseDC(m_hWnd, hDC);

	if (!m_hTextFont)
	{
		int nFontHeight = MulDiv(TEXT_FONT_HEIGHT, nCaps, 72);
		m_hTextFont = ::CreateFontW(
			nFontHeight, 0, 0, 0, FW_NORMAL, FALSE, 
			FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
			DEFAULT_PITCH | FF_SWISS, L"微软雅黑"
			);
	}
}

void KSuspensionWnd::InitImages()
{
	if (m_pIconImg)
	{
		return;
	}

	GetResourceImage(IDP_MAIN_IMG, L"PNG", &m_pIconImg);
}

void KSuspensionWnd::ShowToolTip()
{
	if (m_tooltipWnd.GetWindowHandle())
	{
		KToolTipWnd::TOOLTIP_POS_TOWARD ePosToward = KToolTipWnd::TOWARD_BOTTOM;
		CPoint pt;
		GetToolTipType(ePosToward, pt);
		m_tooltipWnd.ShowToolTip(pt, ePosToward, false);	
	}
}

void KSuspensionWnd::ShowCustomMenus(__in const POINT& pt)
{
	KMenuWnd suspensionMenu;
	suspensionMenu.CreateWnd(
		m_hWnd,
		L"SuspensionMenuWnd",
		L"",
		WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CS_DBLCLKS,
		WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
		0,
		0,
		110 * m_lfScale,
		90 * m_lfScale,
		m_lfScale
		);
	KMenuWnd::MenuItemPair menuItemPair;
	menuItemPair.nMenuID = IDR_SUSPENSION_OPEN;
	menuItemPair.nIconID = -1;
	menuItemPair.nOffset = MENUITEM_OFFSET;
	menuItemPair.strMenu = MENUITEM_OPEN_WPSCLOUD;
	suspensionMenu.AddMenu(menuItemPair);

	menuItemPair.nMenuID = IDR_SUSPENSION_TRANS_DETAIL;
	menuItemPair.nIconID = -1;
	menuItemPair.nOffset = MENUITEM_OFFSET;
	menuItemPair.strMenu = MENUITEM_LOOKUP_TRANS_DETAIL;
	suspensionMenu.AddMenu(menuItemPair);

	menuItemPair.nMenuID = (UINT)-1;
	menuItemPair.nIconID = -1;
	menuItemPair.nOffset = MENUITEM_OFFSET;
	menuItemPair.strMenu = L"";
	suspensionMenu.AddMenu(menuItemPair);

	menuItemPair.nMenuID = IDR_SUSPENSION_CLOSE;
	menuItemPair.nIconID = -1;
	menuItemPair.nOffset = MENUITEM_OFFSET;
	menuItemPair.strMenu = MENUITEM_CLOSE;
	suspensionMenu.AddMenu(menuItemPair);

	UINT uID = suspensionMenu.ShowMenuWnd(pt);
	if (uID != (UINT)-1)
	{
		::PostMessage(m_hWnd, WM_COMMAND, MAKEWPARAM(uID, BN_CLICKED), 0);
	}
}

void KSuspensionWnd::StopTimer()
{
	if (m_hWnd && ::IsWindow(m_hWnd) && m_nIDEvent)
	{
		::KillTimer(m_hWnd, IDT_TIMER);
		m_nIDEvent = 0;
	}
}

void KSuspensionWnd::OpenWPSCloud()
{
	IShellFolder *pIShellFolder = NULL;
	LPITEMIDLIST pNsePidl = NULL;
	PIDLIST_ABSOLUTE pidlBind = NULL;

	IShellFolder* pDesktopFolder = NULL;
	LPITEMIDLIST pMyComputerPidl = NULL;
	if (FAILED(::SHGetDesktopFolder(&pDesktopFolder)))
	{
		goto REALSE_INTERFACE;
	}

	::SHGetSpecialFolderLocation(NULL, CSIDL_DRIVES, &pMyComputerPidl);
	if (FAILED(pDesktopFolder->BindToObject(pMyComputerPidl,
		NULL,
		IID_IShellFolder,
		(void**)&pIShellFolder)))
	{
		goto REALSE_INTERFACE;
	}

	pIShellFolder->ParseDisplayName(
		NULL,
		NULL,
		L"::{5FCD4425-CA3A-48F4-A57C-B8A75C32ACB1}",
		NULL,
		&pNsePidl,
		NULL
		);
	if (!pNsePidl)
	{
		goto REALSE_INTERFACE;
	}

	pidlBind = ::ILCombine(pMyComputerPidl, pNsePidl);
	if (pidlBind == NULL)
	{
		goto REALSE_INTERFACE;
	}

	SHELLEXECUTEINFO sei;
	ZeroMemory(&sei, sizeof(sei));
	sei.cbSize = sizeof(sei);
	sei.fMask = SEE_MASK_IDLIST;
	sei.lpIDList = pidlBind;
	sei.lpVerb = L"open";
	sei.hwnd = NULL;
	sei.nShow = SW_SHOWNORMAL;
	::ShellExecuteEx(&sei);

REALSE_INTERFACE:

	if (pidlBind)
	{
		::CoTaskMemFree(pidlBind);
	}
	if (pNsePidl)
	{
		::CoTaskMemFree(pNsePidl);
	}
	if (pMyComputerPidl)
	{
		::CoTaskMemFree(pMyComputerPidl);
	}
	if (pIShellFolder)
	{
		pIShellFolder->Release();
	}
	if (pDesktopFolder)
	{
		pDesktopFolder->Release();
	}
}

void KSuspensionWnd::ShowTransDetail()
{
	PEXPLORER_THREAD_PARAMTER pThreadParam = 
		g_kNseModule.GetThreadParamter();
	if (pThreadParam)
	{
		PEXPLORER_HWND_PARAMTER pEHP = 
			KGetExplorerParamter(pThreadParam->hActiveWnd);
		if (pEHP && pEHP->pIpcNotify)
		{
			// 传输详情窗口大小: 600 * 450, 居中显示第三页
			int nPosX = (::GetSystemMetrics(SM_CXFULLSCREEN) - 600) / 2;
			int nPosY = (::GetSystemMetrics(SM_CYFULLSCREEN) - 450) / 2;
			pEHP->pIpcNotify->IpcProxy().showTransDeatil(2, nPosX, nPosY);
		}
	}
}

void KSuspensionWnd::OpenDragDir()
{
	
}	

void KSuspensionWnd::GetToolTipType(
	__out KToolTipWnd::TOOLTIP_POS_TOWARD& ePosToward,
	__out CPoint& pt
	)
{
	CRect rcBtn;
	::GetWindowRect(m_hWnd, &rcBtn);
	pt = rcBtn.TopLeft();
	int nCx = ::GetSystemMetrics(SM_CXFULLSCREEN);
	int nCy = ::GetSystemMetrics(SM_CYFULLSCREEN);
	ePosToward = KToolTipWnd::TOWARD_BOTTOM;

	// 上半部分
	if (pt.x < nCx / 4 && pt.y < nCy / 2)
	{
		ePosToward = KToolTipWnd::TOWARD_BOTTOM;
	}
	else if (pt.x < nCx / 2 && pt.y < nCy / 2)
	{
		if (pt.y < nCy / 4)
		{
			ePosToward = KToolTipWnd::TOWARD_BOTTOM;
		}
		else
		{
			ePosToward = KToolTipWnd::TOWARD_RIGHT;
		}
	}
	else if (pt.x < nCx * 3 / 4 && pt.y < nCy / 2)
	{
		if (pt.y < nCy / 4)
		{
			ePosToward = KToolTipWnd::TOWARD_BOTTOM;
		}
		else
		{
			ePosToward = KToolTipWnd::TOWARD_LEFT;
		}
	}
	else if (pt.x > nCx / 2 && pt.y < nCy / 2)
	{
		ePosToward = KToolTipWnd::TOWARD_BOTTOM;
	}
	// 下半部分
	else if (pt.x < nCx / 4 && pt.y > nCy / 2)
	{
		if (pt.y < nCy * 3 / 4)
		{
			ePosToward = KToolTipWnd::TOWARD_RIGHT;
		}
		else
		{
			ePosToward = KToolTipWnd::TOWARD_TOP;
		}
		
	}
	else if (pt.x < nCx / 2 && pt.y > nCy / 2)
	{
		ePosToward = KToolTipWnd::TOWARD_TOP;
	}
	else if (pt.x < nCx * 3 / 4 && pt.y > nCy / 2)
	{
		ePosToward = KToolTipWnd::TOWARD_TOP;
	}
	else if (pt.x > nCx / 2 && pt.y > nCy / 2)
	{
		if (pt.x < nCx * 3 / 4)
		{
			ePosToward = KToolTipWnd::TOWARD_LEFT;
		}
		else
		{
			ePosToward = KToolTipWnd::TOWARD_TOP;
		}
	}
}

BOOL KSuspensionWnd::ChangeTransparency()
{
	if (!(m_hWnd && ::IsWindow(m_hWnd)))
	{
		return FALSE;
	}

	if (!m_fSetLayeredWindowAttributes)
	{
		HMODULE hUser32 = ::GetModuleHandle(_T("User32.dll"));
		if (hUser32)
		{
			m_fSetLayeredWindowAttributes = 
				(PFUNCSETLAYEREDWINDOWATTR)::GetProcAddress(hUser32, "SetLayeredWindowAttributes");
			if(!m_fSetLayeredWindowAttributes)
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	DWORD dwNewStyle = dwStyle;
	if(m_nOpacity >= 0 && m_nOpacity < 256)
	{
		dwNewStyle |= WS_EX_LAYERED;
	}
	else
	{
		dwNewStyle &= ~WS_EX_LAYERED;
	}

	if(dwStyle != dwNewStyle)
	{
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwNewStyle);
	}
	m_fSetLayeredWindowAttributes(m_hWnd, 0, m_nOpacity, LWA_ALPHA);
	return TRUE;
}

void KSuspensionWnd::DrawBackground(HDC hDC)
{
	if (!m_hWnd)
	{
		return;
	}

	RECT rcWnd = {};
	::GetClientRect(m_hWnd, &rcWnd);
	int nWidth = rcWnd.right - rcWnd.left;
	int nHeight = rcWnd.bottom - rcWnd.top;

	HPEN hBorderPen = ::CreatePen(PS_SOLID, 1, m_clBorder);
	HBRUSH hOldBorderPen = (HBRUSH)::SelectObject(hDC, hBorderPen);

	HBRUSH hIconBrush = ::CreateSolidBrush(m_clIconBackground);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, hIconBrush);
	::Rectangle(hDC, 0, 0, IMAGE_WIDTH * m_lfScale, nHeight - 2);
	::SelectObject(hDC, hOldBrush);
	SAFE_DELETE_OBJECT(hIconBrush);

	HBRUSH hTextBrush = ::CreateSolidBrush(m_clBackground);
	hOldBrush = (HBRUSH)::SelectObject(hDC, hTextBrush);
	::Rectangle(
		hDC,
		IMAGE_WIDTH * m_lfScale - 1,
		0,
		nWidth - 1,
		nHeight - 2
		);
	::SelectObject(hDC, hOldBrush);
	SAFE_DELETE_OBJECT(hTextBrush);

	::SelectObject(hDC, hOldBorderPen);
	SAFE_DELETE_OBJECT(hBorderPen);
}

void KSuspensionWnd::DrawOthers(HDC hDC)
{
	if (!m_hWnd)
	{
		return;
	}

	RECT rcWnd = {};
	::GetClientRect(m_hWnd, &rcWnd);
	int nWidth = rcWnd.right - rcWnd.left;
	int nHeight = rcWnd.bottom - rcWnd.top;

	// 绘制ICON图片
	Gdiplus::RectF rcImg;
	if (m_pIconImg)
	{
		Gdiplus::Graphics graphics(hDC);
		graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
		graphics.SetPixelOffsetMode(PixelOffsetModeHalf);
		graphics.SetSmoothingMode(SmoothingModeHighSpeed);

		rcImg.X = IMAGE_LEFT_HOLLOW * m_lfScale;
		rcImg.Y = IMAGE_TOP_HOLLOW * m_lfScale; 
		rcImg.Width = (Gdiplus::REAL)(m_pIconImg->GetWidth() * m_lfScale); 
		rcImg.Height = (Gdiplus::REAL)(m_pIconImg->GetHeight() * m_lfScale);

		graphics.DrawImage(
			m_pIconImg,
			rcImg,
			0,
			0,
			(Gdiplus::REAL)m_pIconImg->GetWidth(), 
			(Gdiplus::REAL)m_pIconImg->GetHeight(),
			Gdiplus::UnitPixel
			);
	}

	// 绘制信息内容
	SIZE szExtent = {};
	RECT rcStr = {};
	HFONT oldFont = (HFONT)::SelectObject(hDC, m_hTextFont);
	::GetTextExtentExPoint(
		hDC,
		m_strInfoText.c_str(),
		(int)wcslen(m_strInfoText.c_str()),
		0,
		NULL,
		NULL, 
		&szExtent
		);

	rcStr.bottom = nHeight;
	rcStr.top = TOP_SPACE * m_lfScale;
	rcStr.left = (IMAGE_WIDTH + TIP_INFO_TEXT_LEFT_SPACE) * m_lfScale;
	if (m_isShowLookupBtn && m_lookupBtn.m_hWnd)
	{
		CRect rcBtn;
		::GetClientRect(m_lookupBtn.m_hWnd, &rcBtn);
		rcStr.left += rcBtn.Width();
		rcStr.left += 2 * m_lfScale; // add space
	}
	rcStr.right = rcStr.left + szExtent.cx;

	::SetBkMode(hDC, TRANSPARENT);
	::SetTextColor(hDC, TIP_INFO_TEXT_COLOR);
	::DrawTextW(
		hDC,
		m_strInfoText.c_str(),
		(int)wcslen(m_strInfoText.c_str()),
		&rcStr,
		DT_LEFT | DT_VCENTER | DT_SINGLELINE
		);

	::SelectObject(hDC, oldFont);
}

void KSuspensionWnd::DrawControls(HDC hDC)
{
	if (!m_hWnd
		|| !m_isShowLookupBtn 
		|| !m_lookupBtn.GetSafeHWND()
		|| !m_pIconImg)
	{
		return;
	}

	RECT rcWnd = {};
	::GetClientRect(m_hWnd, &rcWnd);
	int nWidth = rcWnd.right - rcWnd.left;
	int nHeight = rcWnd.bottom - rcWnd.top;

	RECT rcBtn = rcWnd;
	::GetClientRect(m_lookupBtn.m_hWnd, &rcBtn);
	int nBtnWidth = rcBtn.right - rcBtn.left;
	int nBtnHeight = rcBtn.bottom - rcBtn.top;

	::SetWindowPos(
		m_lookupBtn.m_hWnd,
		NULL,
		(IMAGE_WIDTH + TIP_INFO_TEXT_LEFT_SPACE) * m_lfScale,
		(nHeight - nBtnHeight) / 2,
		0,
		0,
		SWP_NOZORDER | SWP_NOSIZE
		);
	m_lookupBtn.SetBackgroundColor(m_clBackground);
	m_lookupBtn.ShowBtn(TRUE);
}
