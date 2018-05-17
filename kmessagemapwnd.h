/*++ BUILD Version: 0001    

Module Name:

        kmessagemapwnd.h

Abstract:

        窗体基本实现,封装常用消息事件,避免重复的消息分发操作

Author:

        zhouhao3   2018-4-18

Revision History:

--*/

#ifndef _KMESSAGE_MAP_WND_H_
#define _KMESSAGE_MAP_WND_H_

#include "kwindowbase.h"

class KMessageMapWnd : public KWindowBase
{
public:

    KMessageMapWnd();
    virtual ~KMessageMapWnd();

protected:

	 /*++ HandleMessage
	  * 消息处理函数
	  * 1. 一般不继承实现该接口，除非以下接口未提供使用需求时；
	  * 2. 子类继承该接口时，对于除新加的消息响应接口外，其他的消息可采用
	  *    直接调用__super::HandleMessage即可,另外对于已有的消息与处理函数，
		   若需要可实现对应的处理函数接口即可,如OnCreate。
	   示例如下:
	  Note: WM_NCACTIVATE为新需求的消息以及对应需自行实现的OnNcActivate；
	  LRESULT KSomeSubWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	  {
		BOOL bHandled = FALSE;
		LRESULT lResult = 0;
		switch(uMsg)
		{
			case WM_NCACTIVATE:
			{
				lResult = OnNcActivate(wParam, lParam, bHandled);
			}
			break;
			case ...
		}

		if(bHandled)
		{
			return lResult;
		}

		return __super::HandleMessage(uMsg, wParam, lParam);
	  }
	--*/
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:

	virtual LRESULT OnCreate(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnClose(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMove(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnPaint(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnEraseBkgnd(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnCommand(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnGetMinMaxInfo(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnTimer(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnSize(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnKillFocus(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnSetFocus(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnLButtonDBClk(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnRButtonDown(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnRButtonUp(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseWheel(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnKeyUp(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnChar(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnSetCursor(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};


#endif  //_KMESSAGE_MAP_WND_H_
