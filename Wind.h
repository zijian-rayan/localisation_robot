#ifndef _WIND_H
#define _WIND_H

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#define MODELENAME "modeleCGUI"
class CClsWind
{
	ATOM ClsAtom;
public:
	static const UINT DEF_STYLE = CS_HREDRAW | CS_VREDRAW;
	CClsWind(LPCTSTR name, WNDPROC WProc = DefWindowProc, UINT ClsStyle = DEF_STYLE, COLORREF ClrBack = 0xFFFFFF, LPTSTR CurName = IDC_ARROW) :ClsAtom(0)
	{
		WNDCLASS wclass = { ClsStyle, WProc, 0, 0, ::GetModuleHandle(0), 0, ::LoadCursor(0,CurName),::CreateSolidBrush(ClrBack), 0, name };
		ClsAtom = ::RegisterClass(&wclass);
	}
	CClsWind(WNDPROC WProc, UINT ClsStyle = DEF_STYLE, COLORREF ClrBack = 0xFFFFFF, LPTSTR CurName = IDC_ARROW) :ClsAtom(0)
	{
		TCHAR cls_name[128];
		LARGE_INTEGER count;
		::QueryPerformanceCounter(&count);
		wsprintf(cls_name, _T("CClsWnd#%08lX"), count.LowPart);
		WNDCLASS wclass = { ClsStyle,WProc,0,0,::GetModuleHandle(0), 0, ::LoadCursor(0,CurName),::CreateSolidBrush(ClrBack), 0, cls_name };
		ClsAtom = ::RegisterClass(&wclass);
	}
	CClsWind() :ClsAtom(0) {}
	~CClsWind() { if (ClsAtom) ::UnregisterClass((LPCTSTR)ClsAtom, ::GetModuleHandle(0)); }
	operator LPCTSTR() const { return (LPCTSTR)ClsAtom; }
	bool IsValid() const { return ClsAtom != 0; }

};

class CWind
{
protected:
	HWND m_hWnd;
	CClsWind LocCls;
	bool is_main;
	WNDPROC defproc;
	LPCTSTR   m_menuName;
	LPCTSTR   m_titre;
	HINSTANCE m_hInst;
	INT m_dx;
	INT m_dy;
	static CWind* pBase;
public:
	static const UINT DEF_STYLE = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	virtual LRESULT WmPaint(HWND hWnd, WPARAM wP, LPARAM lP) {
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps); // on obtient le DC dans ps.hdc 
		//....
		EndPaint(hWnd, &ps); 
		return -1; }
	virtual LRESULT WmTimer(HWND hWnd, WPARAM wP, LPARAM lP) { return -1; }
	virtual LRESULT WmClose(HWND hWnd, WPARAM wP, LPARAM lP) { return -1; }
	virtual LRESULT WmCommand(HWND hWnd, WPARAM wP, LPARAM lP) { return -1; }
	virtual LRESULT WmCreate(HWND hWnd, WPARAM wP, LPARAM lP) { return -1; }
	virtual LRESULT WmKeyUp(HWND hWnd, WPARAM wP, LPARAM lP) { return -1; }
	virtual LRESULT WmKeyDown(HWND hWnd, WPARAM wP, LPARAM lP) { return -1; }
	virtual LRESULT WmLButtonDown(HWND hWnd, WPARAM wP, LPARAM lP) { return -1; }
	virtual LRESULT WmMButtonDown(HWND hWnd, WPARAM wP, LPARAM lP) { return -1; }
	virtual LRESULT WmRButtonDown(HWND hWnd, WPARAM wP, LPARAM lP) { return -1; }
	virtual LRESULT WmLButtonUp(HWND hWnd, WPARAM wP, LPARAM lP) { return -1; }
	virtual LRESULT WmMButtonUp(HWND hWnd, WPARAM wP, LPARAM lP) { return -1; }
	virtual LRESULT WmRButtonUp(HWND hWnd, WPARAM wP, LPARAM lP) { return -1; }
	virtual LRESULT WmMouseMove(HWND hWnd, WPARAM wP, LPARAM lP) { return -1; }
	virtual LRESULT WmDropFiles(HWND hWnd, WPARAM wP, LPARAM lP) { return -1; }
	CWind(HINSTANCE _hInst, LPCTSTR _titre = "", LPCTSTR _menuName = "", int _dx = 300, int _dy = 300)
	{
		m_dx = _dx;
		m_dy = _dy;
		m_hInst = _hInst;
		m_titre = _titre;
		m_menuName = _menuName;
		pBase = this;
		if (!Enregister()) ExitProcess(0);
		//CreateWindow déplac?dans la fonction Creation()
	}
	bool Enregister()
	{// enregistrement du modele(class) de fenetre
		WNDCLASS wndC;
		ZeroMemory(&wndC, sizeof(wndC));
		//**** définition de classe fenêtre avant la création
		wndC.style = CS_HREDRAW | CS_VREDRAW;
		wndC.lpfnWndProc = (WNDPROC)WndProc;
		wndC.cbClsExtra = 0;
		wndC.cbWndExtra = 0;
		wndC.hInstance = m_hInst;
		wndC.hIcon = NULL;
		wndC.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndC.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wndC.lpszMenuName = m_menuName;
		wndC.lpszClassName = MODELENAME;

		return RegisterClass(&wndC) != 0;
	}
	LRESULT WndMeth(HWND hWnd, UINT msgID, WPARAM wP, LPARAM lP)
	{
		LRESULT res = -1;
		switch (msgID)
		{
		case WM_PAINT:   res = WmPaint(hWnd, wP, lP); break;
		case WM_TIMER:   res = WmTimer(hWnd, wP, lP); break;
		case WM_CLOSE:   res = WmClose(hWnd, wP, lP); break;
		case WM_COMMAND: res = WmCommand(hWnd, wP, lP); break;
		case WM_CREATE:  res = WmCreate(hWnd, wP, lP); break;
		case WM_KEYUP:   res = WmKeyUp(hWnd, wP, lP); break;
		case WM_KEYDOWN:     res = WmKeyDown(hWnd, wP, lP); break;
		case WM_LBUTTONDOWN: res = WmLButtonDown(hWnd, wP, lP); break;
		case WM_MBUTTONDOWN: res = WmMButtonDown(hWnd, wP, lP); break;
		case WM_RBUTTONDOWN: res = WmRButtonDown(hWnd, wP, lP); break;
		case WM_LBUTTONUP: res = WmLButtonUp(hWnd, wP, lP); break;
		case WM_MBUTTONUP: res = WmMButtonUp(hWnd, wP, lP); break;
		case WM_RBUTTONUP: res = WmRButtonUp(hWnd, wP, lP); break;
		case WM_MOUSEMOVE: res = WmMouseMove(hWnd, wP, lP); break;
		case WM_DROPFILES: res = WmDropFiles(hWnd, wP, lP); break;
		case WM_DESTROY:   if (is_main) { m_hWnd = 0; ::PostQuitMessage(0); } break;
		}
		return res == 0 ? res : (defproc ? defproc(hWnd, msgID, wP, lP) : ::DefWindowProc(hWnd, msgID, wP, lP));
	}
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msgID, WPARAM wP, LPARAM lP)
	{
		//printf("WndProc: msg=%04lX\n",msgID);
		CWind* ptr = (CWind*)(LONG_PTR)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (ptr == 0 && cur_wnd != 0)
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)(LONG_PTR)(ptr = cur_wnd));
		cur_wnd = 0;
		return ptr ? ptr->WndMeth(hWnd, msgID, wP, lP) : ::DefWindowProc(hWnd, msgID, wP, lP);
	}
	static int MsgLoop()
	{
		MSG msg;
		while (::GetMessage(&msg, 0, 0, 0))
		{
			//printf("MsgLoop: msg=%04lX\n",msg.message);
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		return (int)msg.wParam;
	}
	void SetMain(bool _is_main = true) { is_main = _is_main; }
	CWind(LPCTSTR WndName = 0, const LPRECT pRect = 0, UINT WndStyle = DEF_STYLE, UINT WndExStyle = 0, LPCTSTR ClsName = 0, WORD MenuID = 0) :
		m_hWnd(0), is_main(true), defproc(0)
	{
		cur_wnd = this;
		if (ClsName == 0) ClsName = DefClsWind;
		RECT* pR = pRect ? pRect : &DefRect;
		m_hWnd = ::CreateWindowEx(WndExStyle, ClsName, WndName, WndStyle, pR->left, pR->top, pR->right, pR->bottom, 0, (MenuID ? LoadMenu(::GetModuleHandle(0), MAKEINTRESOURCE(MenuID)) : 0), ::GetModuleHandle(0), 0);
		if (ClsName != DefClsWind)
		{
			defproc = (WNDPROC)(LONG_PTR)::GetWindowLongPtr(m_hWnd, GWLP_WNDPROC);
			if (defproc == WndProc) defproc = 0;
			::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG)(LONG_PTR)WndProc);
			::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG)(LONG_PTR)this);
		}
	}
	void Creation()
	{
		m_hWnd = CreateWindow(MODELENAME,
			m_titre,
			WS_OVERLAPPEDWINDOW,
			200, 100,
			m_dx, m_dy,
			NULL,
			NULL,
			m_hInst,
			NULL);

		if (!m_hWnd) ExitProcess(0);

		ShowWindow(m_hWnd, SW_SHOWNORMAL);
		UpdateWindow(m_hWnd);
	}



	// a utiliser pour des fenetres enfant, passer l'ID de l'enfant dans ChildID et l'adresse de l'objet parent dans Parent
	CWind(CWind* Parent, WORD ChildID, LPCTSTR WndName = _T(""), const LPRECT pRect = 0, UINT WndStyle = DEF_STYLE, UINT WndExStyle = 0, LPCTSTR ClsName = 0) :
		m_hWnd(0), is_main(false), defproc(0)
	{
		cur_wnd = this;
		if (ClsName == 0) ClsName = DefClsWind;
		RECT* pR = pRect ? pRect : &DefRect;
		m_hWnd = ::CreateWindowEx(WndExStyle, ClsName, WndName, WndStyle, pR->left, pR->top, pR->right, pR->bottom,
			(Parent ? Parent->m_hWnd : 0), (HMENU)ChildID, ::GetModuleHandle(0), 0);
		if (ClsName != DefClsWind)
		{
			defproc = (WNDPROC)(LONG_PTR)::GetWindowLongPtr(m_hWnd, GWLP_WNDPROC);
			if (defproc == WndProc) defproc = 0;
			::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG)(LONG_PTR)WndProc);
			::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG)(LONG_PTR)this);
		}
	}
	CWind(COLORREF Color, LPCTSTR WndName = 0, const LPRECT pRect = 0, UINT WndStyle = DEF_STYLE, UINT WndExStyle = 0, UINT ClsStyle = CClsWind::DEF_STYLE) :
		m_hWnd(0), is_main(true), LocCls(WndProc, ClsStyle, Color), defproc(0)
	{
		cur_wnd = this;
		RECT* pR = pRect ? pRect : &DefRect;
		m_hWnd = ::CreateWindowEx(WndExStyle, LocCls, WndName, WndStyle, pR->left, pR->top, pR->right, pR->bottom, 0, 0, ::GetModuleHandle(0), 0);
	}
	virtual ~CWind()
	{
		if (m_hWnd && ::IsWindow(m_hWnd))
		{
			is_main = false;
			::DestroyWindow(m_hWnd);
		}
	}
	HWND GetHandle() { return m_hWnd; }
	LONG GetStyle() { return (LONG)::GetWindowLongPtr(m_hWnd, GWL_STYLE); }
	LONG GetExStyle() { return (LONG)::GetWindowLongPtr(m_hWnd, GWL_EXSTYLE); }
	void SetStyle(LONG s)
	{
		::SetWindowLongPtr(m_hWnd, GWL_STYLE, (LONG)(LONG_PTR)s);
		::SetWindowPos(m_hWnd, 0, 0, 0, 0, 0, SWP_NOMOVE |
			SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}
	void SetExStyle(LONG s)
	{
		::SetWindowLongPtr(m_hWnd, GWL_EXSTYLE, (LONG)(LONG_PTR)s);
		::SetWindowPos(m_hWnd, 0, 0, 0, 0, 0, SWP_NOMOVE |
			SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}

	static RECT Rect(LONG x0, LONG y0, LONG dx, LONG dy) { RECT rect = { x0, y0, dx, dy }; return rect; }

	static CClsWind DefClsWind;
	static RECT DefRect;
	static CWind* cur_wnd;
};

__declspec(selectany) CClsWind CWind::DefClsWind(_T("DefClsName"), CWind::WndProc);
__declspec(selectany) RECT CWind::DefRect = { 0,0,CW_USEDEFAULT,CW_USEDEFAULT };
__declspec(selectany) CWind* CWind::cur_wnd = 0;

#endif _WIND_H
