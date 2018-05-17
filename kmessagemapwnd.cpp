#include "stdafx.h"
#include "kmessagemapwnd.h"

KMessageMapWnd::KMessageMapWnd()
{

}

KMessageMapWnd::~KMessageMapWnd()
{

}

LRESULT KMessageMapWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bHandled = FALSE;
	LRESULT lResult = 0;
	switch (uMsg)
	{
	case WM_CREATE:
		{
			lResult = OnCreate(wParam, lParam, bHandled);
		}
		break;
	case WM_CLOSE:
		{
			lResult = OnClose(wParam, lParam, bHandled);
		}
		break;
	case WM_MOVE:
		{
			lResult = OnMove(wParam, lParam, bHandled);
		}
		break;
	case WM_PAINT:
		{
			lResult = OnPaint(wParam, lParam, bHandled);
		}
		break;
	case WM_ERASEBKGND:
		{
			lResult = OnEraseBkgnd(wParam, lParam, bHandled);
		}
		break;
	case WM_COMMAND:
		{
			lResult = OnCommand(wParam, lParam, bHandled);
		}
		break;
	case WM_GETMINMAXINFO:
		{
			lResult = OnGetMinMaxInfo(wParam, lParam, bHandled);
		}
		break;
	case WM_TIMER:
		{
			lResult = OnTimer(wParam, lParam, bHandled);
		}
		break;
	case WM_SIZE:
		{
			lResult = OnSize(wParam, lParam, bHandled);
		}
		break;
	case WM_KILLFOCUS:
		{
			lResult = OnKillFocus(wParam, lParam, bHandled);
		}
		break;
	case WM_SETFOCUS:
		{
			lResult = OnSetFocus(wParam, lParam, bHandled);
		}
		break;
	case WM_LBUTTONDOWN:
		{
			lResult = OnLButtonDown(wParam, lParam, bHandled);
		}
		break;
	case WM_LBUTTONUP:
		{
			lResult = OnLButtonUp(wParam, lParam, bHandled);
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			lResult = OnLButtonDBClk(wParam, lParam, bHandled);
		}
		break;
	case WM_RBUTTONDOWN:
		{
			lResult = OnRButtonDown(wParam, lParam, bHandled);
		}
		break;
	case WM_RBUTTONUP:
		{
			lResult = OnRButtonUp(wParam, lParam, bHandled);
		}
		break;
	case WM_MOUSEMOVE:
		{
			lResult = OnMouseMove(wParam, lParam, bHandled);
		}
		break;
	case WM_MOUSEHWHEEL:
		{
			lResult = OnMouseWheel(wParam, lParam, bHandled);
		}
		break;
	case WM_MOUSEHOVER:
		{
			lResult = OnMouseHover(wParam, lParam, bHandled);
		}
		break;
	case WM_MOUSELEAVE:
		{
			lResult = OnMouseLeave(wParam, lParam, bHandled);
		}
		break;
	case WM_KEYUP:
		{
			lResult = OnKeyUp(wParam, lParam, bHandled);
		}
		break;
	case WM_KEYDOWN:
		{
			lResult = OnKeyDown(wParam, lParam, bHandled);
		}
		break;
	case WM_CHAR:
		{
			lResult = OnChar(wParam, lParam, bHandled);
		}
		break;
	case WM_SETCURSOR:
		{
			lResult = OnSetCursor(wParam, lParam, bHandled);
		}
		break;
	case WM_DESTROY:
		{
			bHandled = TRUE;
			lResult = 0;
			::PostQuitMessage(0);
		}
		break;
	default:
		break;
	}

	if (bHandled)
	{
		return lResult;
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT KMessageMapWnd::OnCreate(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT KMessageMapWnd::OnClose(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT KMessageMapWnd::OnMove(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT KMessageMapWnd::OnPaint(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT KMessageMapWnd::OnEraseBkgnd(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT KMessageMapWnd::OnCommand(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT KMessageMapWnd::OnGetMinMaxInfo(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT KMessageMapWnd::OnTimer(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT KMessageMapWnd::OnSize(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT KMessageMapWnd::OnKillFocus(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT KMessageMapWnd::OnSetFocus(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT KMessageMapWnd::OnLButtonDown(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT KMessageMapWnd::OnLButtonUp(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT KMessageMapWnd::OnLButtonDBClk(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT KMessageMapWnd::OnRButtonDown(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT KMessageMapWnd::OnRButtonUp(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT KMessageMapWnd::OnMouseMove(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT KMessageMapWnd::OnMouseWheel(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT KMessageMapWnd::OnMouseHover(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT KMessageMapWnd::OnMouseLeave(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT KMessageMapWnd::OnKeyUp(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT KMessageMapWnd::OnKeyDown(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT KMessageMapWnd::OnChar(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT KMessageMapWnd::OnSetCursor(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}
