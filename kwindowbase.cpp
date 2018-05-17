#include "stdafx.h"
#include "kwindowbase.h"

KWindowBase::KWindowBase()
  : m_hWnd(NULL)
  , m_hParent(NULL)
  , m_oldWndProc(::DefWindowProc)
  , m_strClassName(L"")
  , m_strWndName(L"")
  , m_lfScale(1.0)
  , m_bHide(FALSE)
  , m_bRegisterWnd(FALSE)
{

}

KWindowBase::~KWindowBase()
{
	DestroyWnd();
	UnRegisterWnd();
}

BOOL KWindowBase::CreateWnd(
    __in HWND hParentWnd,
	__in const wchar_t* pStrClassName,
	__in const wchar_t* pStrWndName,
	__in DWORD dwStyle,
	__in DWORD dwExStyle,
	__in int x,
	__in int y,
	__in int cx,
	__in int cy,
    __in double lfScale
	)
{
	if (m_hWnd || !pStrClassName || !hParentWnd)
    {
        return FALSE;
    }

	if (!RegisterWnd(pStrClassName))
    {
        return FALSE;
    }

	m_hWnd = ::CreateWindowExW(
		dwExStyle,
		pStrClassName,
		pStrWndName,
		dwStyle,
		x,
		y,
		cx * m_lfScale,
		cy * m_lfScale,
		hParentWnd,
		(HMENU)0,
		NULL,
		this
		);

	if (!m_hWnd)
    {
		return FALSE;
	}
	
	m_hParent = hParentWnd;
	m_lfScale = lfScale;
	m_strWndName = pStrWndName;

	InitializeWnd();

	return TRUE;
}

void KWindowBase::CloseWnd(__in int nRetCode)
{
	if (m_hWnd && ::IsWindow(m_hWnd))
	{
		PostMessage(WM_CLOSE, (WPARAM)nRetCode, (LPARAM)0L);
	}
}

std::wstring KWindowBase::GetWndClassName() const
{
	return m_strClassName;
}

std::wstring KWindowBase::GetWndName() const
{
	return m_strWndName;
}

void KWindowBase::DestroyWnd()
{
    if (m_hWnd && ::IsWindow(m_hWnd))
    {
        ::DestroyWindow(m_hWnd);
    }
	m_hWnd = NULL;
}

BOOL KWindowBase::RegisterWnd(__in const wchar_t* pStrClasName)
{
    if (m_bRegisterWnd)
    {
        return TRUE;
    }

	if (!pStrClasName)
	{
		return FALSE;
	}

	m_strClassName = pStrClasName;

    WNDCLASSEXW wc = {}; 
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS; 
    wc.lpfnWndProc = (WNDPROC)_WndProc; 
    wc.cbClsExtra = 0; 
    wc.cbWndExtra = 0; 
    wc.hInstance = GetModuleHandle(NULL); 
    wc.hIcon = NULL; 
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); 
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); 
    wc.lpszMenuName =  NULL; 
    wc.lpszClassName = pStrClasName; 

    if (!::RegisterClassExW(&wc)) 
    {
        if(ERROR_CLASS_ALREADY_EXISTS == GetLastError())
        {
			m_bRegisterWnd = TRUE;
        }
		else
		{
			return FALSE;
		}
    }
	else
	{
		m_bRegisterWnd = TRUE;
	}

    return TRUE;
}

void KWindowBase::UnRegisterWnd()
{
    if (m_bRegisterWnd)
    {
        ::UnregisterClass(m_strClassName.c_str(), GetModuleHandle(NULL));
        m_bRegisterWnd = FALSE;
    } 
}

void KWindowBase::UpdateWndPos(
    __in const POINT &point,
    __in POS_TOWARD ePosToward
	) const
{
    if (!(m_hWnd && ::IsWindow(m_hWnd)))
    {
        return;
    }

    RECT rcWnd = {};
    ::GetWindowRect(m_hWnd, &rcWnd);
    LONG width = rcWnd.right - rcWnd.left;
    LONG height = rcWnd.bottom - rcWnd.top;

    switch (ePosToward)
    {
    case TOWARD_LEFT_UP:
        {
            rcWnd.left = point.x - width;
            rcWnd.top = point.y - height;
            rcWnd.right = point.x;
            rcWnd.bottom = point.y;
        }
    	break;
    case TOWARD_LEFT_DOWN:
        {
            rcWnd.left = point.x - width;
            rcWnd.top = point.y;
            rcWnd.right = point.x;
            rcWnd.bottom = point.y + height;
        }
        break;
    case TOWARD_RIGHT_UP:
        {
            rcWnd.left = point.x;
            rcWnd.top = point.y - height;
            rcWnd.right = point.x + width;
            rcWnd.bottom = point.y;
        }
        break;
    case TOWARD_RIGHT_DOWN:
        {
            rcWnd.left = point.x;
            rcWnd.top = point.y;
            rcWnd.right = point.x + width;
            rcWnd.bottom = point.y + height;
        }
        break;
    }

    AdjustWndPos(rcWnd);
    ::SetWindowPos(
		m_hWnd,
		NULL, 
        rcWnd.left,
		rcWnd.top,
        rcWnd.right - rcWnd.left, 
        rcWnd.bottom - rcWnd.top,
        SWP_NOZORDER | SWP_NOACTIVATE
		);
}

HWND KWindowBase::GetWindowHandle() const
{
	return m_hWnd;
}

HWND KWindowBase::GetParentWindowHandle() const
{
    return m_hParent;
}

void KWindowBase::MoveWindow(__in const RECT &rect)
{
	if (m_hWnd && ::IsWindow(m_hWnd))
	{
		::SetWindowPos(
			m_hWnd,
			NULL,
			rect.left,
			rect.top,
            rect.right - rect.left,
			rect.bottom - rect.top,
            SWP_NOZORDER
			);
	}
}

void KWindowBase::ShowWindow(
	__in const POINT &ptPos,
	__in POS_TOWARD ePosToward,
	__in BOOL bShow
	)
{
    UpdateWndPos(ptPos, ePosToward);
	ShowWindow(bShow);
}

void KWindowBase::ShowWindow(__in BOOL bShow)
{
	if (!(m_hWnd && ::IsWindow(m_hWnd)))
	{
		return;
	}

    if (bShow)
    {
        ::ShowWindow(m_hWnd, SW_SHOWNORMAL);
        m_bHide = FALSE;
    }
    else
    {
        ::ShowWindow(m_hWnd, SW_HIDE);
        m_bHide = TRUE;
    }
}

void KWindowBase::DoModal()
{
	MsgPump();
}

void KWindowBase::RePaint()
{
    if (m_hWnd && ::IsWindow(m_hWnd))
    {
	    ::InvalidateRect(m_hWnd, NULL, TRUE);
    }
}

BOOL KWindowBase::SendMessage(
	__in UINT nMsg,
	__in WPARAM wParam,
	__in LPARAM lParam
	)
{
	if (m_hWnd && ::IsWindow(m_hWnd))
	{
		return ::SendMessage(m_hWnd, nMsg, wParam, lParam);
	}
	return false;
}

BOOL KWindowBase::PostMessage(
	__in UINT nMsg,
	__in WPARAM wParam,
	__in LPARAM lParam
	)
{
	if (m_hWnd && ::IsWindow(m_hWnd))
	{
		return ::PostMessage(m_hWnd, nMsg, wParam, lParam);
	}
	return false;
}

void KWindowBase::MsgPump()
{
    if (!(m_hWnd && ::IsWindow(m_hWnd)))
    {
        return;
    }

	HWND hWndParent = ::GetWindowOwner(m_hWnd);
	::ShowWindow(m_hWnd, SW_SHOWNORMAL);
	::EnableWindow(hWndParent, FALSE);

    MSG msg = {};
    while (::GetMessage(&msg, NULL, 0, 0))
    {
        if ((msg.message == WM_CLOSE && msg.hwnd == m_hWnd))
        {
			::EnableWindow(hWndParent, TRUE);
			::SetFocus(hWndParent);
        }

		if (!OnTranslateMsg(msg))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			break;
		}
    }

	::EnableWindow(hWndParent, TRUE);
	::SetFocus(hWndParent);

	if (msg.message == WM_QUIT)
	{
		::PostQuitMessage(msg.wParam);
	}
}

void KWindowBase::InitializeWnd()
{

}

bool KWindowBase::OnTranslateMsg(MSG& msg)
{
	return false;
}

LRESULT KWindowBase::HandleMessage(
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
	)
{
	if (!(m_hWnd && ::IsWindow(m_hWnd)))
	{
		return 0;
	}

    if (m_oldWndProc)
    {
        return ::CallWindowProc(m_oldWndProc, m_hWnd, uMsg, wParam, lParam);
    }
    else
    {
        return ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
    }
}

void KWindowBase::OnFinalMessage(HWND hWnd)
{
    UnRegisterWnd();
}

LRESULT KWindowBase::_WndProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
	)
{
    KWindowBase* pThis = nullptr;
    if (message == WM_NCCREATE)
    {
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = static_cast<KWindowBase*>(lpcs->lpCreateParams);
        pThis->m_hWnd = hWnd;

#ifdef WIN64
        ::SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);
#else
        ::SetWindowLongW(hWnd, GWL_USERDATA, (LONG)pThis);
#endif

    } 
    else
    {

#ifdef WIN64
        pThis = (KWindowBase *)::GetWindowLongPtrW(hWnd, GWLP_USERDATA);
#else
        pThis = (KWindowBase *)::GetWindowLongW(hWnd, GWL_USERDATA);
#endif

        if (message == WM_NCDESTROY && pThis)
        {
            LRESULT lRes = ::CallWindowProc(
                pThis->m_oldWndProc,
				hWnd,
                message,
				wParam,
				lParam
				);

            ::SetWindowLongPtr(pThis->m_hWnd, GWLP_USERDATA, 0L);
            pThis->m_hWnd = nullptr;
            pThis->OnFinalMessage(hWnd);
            return lRes;
        }
    }

    if (pThis)
    {
        return pThis->HandleMessage(message, wParam, lParam);
    } 
    else
    {
        return ::DefWindowProc(hWnd, message, wParam, lParam);
    }
}
