#ifndef _INTERFACE_H
#define _INTERFACE_H

#include "Wind.h"
#include <windowsx.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <thread>
#include <gdiplus.h>
#include "resource.h"
#define RADIUS 20
#define IDC_LISTR 201
using namespace Gdiplus;

const COLORREF tabColor[] =
{ RGB(255,0,0), RGB(0,255,0), RGB(0,0,255), RGB(255,0,255), RGB(20,20,20), RGB(200,100,30), RGB(75,200,200) };

#define IDC_BUTTON1 200
#define IDC_BUTTON2 201
#define IDC_BUTTON3 202
#define IDC_BUTTON4 203
#ifdef _GDIPLUS_H
using namespace Gdiplus;
class CInitGdiplus
{
	ULONG_PTR token;
public:
	CInitGdiplus() { GdiplusStartupInput inp; GdiplusStartup(&token, &inp, 0); }
	~CInitGdiplus() { GdiplusShutdown(token); }
} globInst;
#endif


//class CEditCopy : public CWind
//{
//	virtual LRESULT WmCommand(HWND hWnd, WPARAM wP, LPARAM lP)
//	{
//		switch (LOWORD(wP))
//		{
//		case 1:
//		{
//			TCHAR buf[256];
//			::GetWindowText(pe1->GetHandle(), buf, 256);
//		}
//		break;
//		}
//		return 0;
//	}
//
//	CWind *pb1, *pe1, *pok1;
//public:
//	CEditCopy(CWind* Parent, DWORD ChildID, LPCTSTR WndName = _T("")) : CWind(Parent, ChildID, WndName,
//	&CWind::Rect(0, 0, 300,120), WS_POPUPWINDOW | WS_VISIBLE | WS_CAPTION), pb1(0), pe1(0)
//	{
//		RECT r = { 20,20,150,20 };
//		pb1 = new CWind(this, 1, _T("Nombre de robots : "), &r, WS_CHILD | WS_VISIBLE, 0, _T("Button"));
//		r.left += 180;
//		pe1 = new CWind(this, 3, _T(""), &r, WS_CHILD | WS_VISIBLE, 0, _T("Edit"));
//		r.top += 30;
//		pok1 = new CWind(this, 3, _T(""), &r, WS_CHILD | WS_VISIBLE, 0, _T("OK"));
//	}
//	~CEditCopy()
//	{
//		if (pb1) delete pb1;
//		if (pe1) delete pe1;
//		if (pok1) delete pe1;
//	}
//};
//
//
//
//int test1(CWind* Parent, WORD ChildID)
//{
//	CEditCopy EditBox(Parent, ChildID, "S閘ection du nombre de robots");
//	return CWind::MsgLoop();
//}



class CPoint
{
protected:
	int x, y;
	COLORREF color;

public:
	static const int radius = RADIUS;
	CPoint(int _x, int _y, COLORREF _color = 0xFFFFFF) : x(_x), y(_y), color(_color) {}
	virtual void SetPoint(int _x, int _y) { x = _x; y = _y; }
	virtual void SetPoint(CPoint& point) { x = point.GetX(); y = point.GetY(); }
	virtual void SetX(int _x) { x = _x; }
	virtual void SetY(int _y) { y = _y; }
	virtual void SetColor(COLORREF _color) { color = _color; }
	virtual const int& GetX() const { return x; }
	virtual const int& GetY() const { return y; }
	virtual const CPoint& GetPoint() const { return *this; }
	virtual const COLORREF& GetColor() const { return color; }
	virtual const int& GetRadius() const { return radius; }
	virtual ~CPoint() {}
};


template <typename CType>
class CTabPoint
{
protected:
	std::vector<CType*> tabPoint;
	CType** currentPoint;

public:
	CTabPoint() : currentPoint(NULL) {}

	virtual bool SelectPoint(CType point)
	{
		for (int i = 0; i < (int)tabPoint.size(); i++)
		{
			if (tabPoint[i]->GetX() - CType::radius <= point.GetX() && point.GetX() <= tabPoint[i]->GetX() + CType::radius
			 && tabPoint[i]->GetY() - CType::radius <= point.GetY() && point.GetY() <= tabPoint[i]->GetY() + CType::radius)
			{
				currentPoint = &tabPoint[i];
				return true;
			}
		}
		return false;
	}

	virtual bool SelectPoint(int index)
	{
		if (0 <= index && index < (int)tabPoint.size())
		{
			currentPoint = &tabPoint[index];
			return true;
		}
		return false;
	}

	virtual bool AddPoint(CType point)
	{
		if (!SelectPoint(point))
		{
			tabPoint.push_back(new CType(point.GetX(), point.GetY(), point.GetColor()));
			currentPoint = &tabPoint[tabPoint.size() - 1];
			return true;
		}
		return false;
	}

	virtual bool DeletePoint(CType point)
	{
		if (SelectPoint(point))
		{
			delete *currentPoint;
			tabPoint.erase(tabPoint.begin() + indexCurrPoint());
			currentPoint = NULL;
			return true;
		}
		return false;
	}

	virtual bool DeleteAllPoint()
	{
		tabPoint.clear();
		return true;
	}

	void SetTabPoint(int index, CType point) 
	{
		if (index < GetNbPoints()) 
			tabPoint[index]->SetPoint(point);
	}

	const CType& GetTabPoint(int index)
	{
		if (index < GetNbPoints()) return tabPoint[index]->GetPoint();
		return tabPoint[0]->GetPoint();
	}

	const int GetNbPoints()
	{
		return tabPoint.size(); 
	}

	virtual ~CTabPoint<CType>() 
	{
		for (int i = 0; i < (int)tabPoint.size(); i++) 
			delete tabPoint[i]; 
	}

private:
	const int indexCurrPoint()
	{
		for (int i = 0; i < (int)tabPoint.size(); i++) 
		{ 
			if (tabPoint[i] == *currentPoint) 
				return i;
		}
		return -1;
	}
};


class CRobot : public CTabPoint<CPoint>, public CPoint
{
protected:
	int w;

public:
	CRobot(int x, int y, COLORREF color = 0xFFFFFF, int _w = 0) : CTabPoint<CPoint>(), w(_w), CPoint(x, y, color) {}

	virtual void SetW(int _w)
	{
		w = _w;
		while (w >= 360) w -= 360;
		while (w < 0) w += 360;
	}
	virtual const int& GetW() const { return w; }
	virtual ~CRobot() {}
};

bool OpenFile(HWND parentWindow, char(&filePath)[MAX_PATH])
{
	//Structure n閏essaire ?la bo顃e de dialogue d'ouverture
	OPENFILENAME toGetFileName;

	//Remplissage de la structure
	ZeroMemory(&toGetFileName, sizeof(OPENFILENAME));
	toGetFileName.lStructSize = sizeof(OPENFILENAME);
	toGetFileName.hwndOwner = parentWindow;
	toGetFileName.lpstrFile = filePath;
	toGetFileName.nMaxFile = MAX_PATH;
	toGetFileName.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	toGetFileName.nFilterIndex = 1;
	toGetFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	//Si l'utilisateur clique sur "Ouvrir"
	return GetOpenFileName(&toGetFileName);
}

bool SaveFile(HWND parentWindow, char(&filePath)[MAX_PATH])
{
	OPENFILENAME toGetFileName;

	//Remplissage de la structure
	ZeroMemory(&toGetFileName, sizeof(OPENFILENAME));
	toGetFileName.lStructSize = sizeof(OPENFILENAME);
	toGetFileName.hwndOwner = parentWindow;
	toGetFileName.lpstrFile = filePath;
	toGetFileName.nMaxFile = MAX_PATH;
	toGetFileName.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	toGetFileName.nFilterIndex = 1;
	toGetFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	//Si l'utilisateur clique sur "Ouvrir"
	return GetSaveFileName(&toGetFileName);
}


class CInterface : public CWind, public CTabPoint<CRobot>
{
	bool edit;
	int* id, *cmd, *x, *y;
	bool* writeRead;
	bool* alive;
	HPEN aPen;
	HDC hdc;
	bool selected = FALSE;
public:

	CInterface(int* _id, int* _cmd, int* _x, int* _y, bool* _alive, bool* _writeRead, LPCTSTR WndName = 0, const LPRECT pRect = 0) :
		CWind(WndName, &CWind::Rect(0, 0, 660, 520), DEF_STYLE, 0, 0, IDR_MENU1), edit(true), id(_id), cmd(_cmd), x(_x), y(_y), alive(_alive), writeRead(_writeRead)
	{
		/*AddPoint(CRobot(100, 100, NewColor()));*/
		::SetTimer(m_hWnd, 0, 50, 0);
	}

	static COLORREF NewColor()
	{
		static int i = 0;
		int nbColor = sizeof(tabColor) / sizeof(COLORREF);
		COLORREF color = tabColor[i%nbColor];
		i++;
		return color;
	}
	HWND hbutton1, hbutton2,hbutton3,hbutton4;

	virtual LRESULT WmCreate(HWND hWnd, WPARAM wP, LPARAM lP) {
		hbutton1 = CreateWindow(
			"BUTTON", "Load Path",
			WS_CHILD | WS_BORDER |
			WS_VISIBLE,
			500, 100, 150, 30,
			hWnd, (HMENU)IDC_BUTTON1,
			m_hInst, NULL);
		hbutton2 = CreateWindow(
			"BUTTON", "SavePath",
			WS_CHILD | WS_BORDER |
			WS_VISIBLE,
			500, 150, 150, 30,
			hWnd, (HMENU)IDC_BUTTON2,
			m_hInst, NULL);
		hbutton3 = CreateWindow(
			"BUTTON", "ShowPath",
			WS_CHILD | WS_BORDER |
			WS_VISIBLE,
			500, 200, 150, 30,
			hWnd, (HMENU)IDC_BUTTON3,
			m_hInst, NULL);
		hbutton4 = CreateWindow(
			"BUTTON", "NewTarget",
			WS_CHILD | WS_BORDER |
			WS_VISIBLE,
			500, 250, 150, 30,
			hWnd, (HMENU)IDC_BUTTON4,
			m_hInst, NULL);
		return 0;
	}
	virtual LRESULT WmRButtonDown(HWND hWnd, WPARAM wP, LPARAM lP)
	{
		/*if (edit)
		{*/
			CRobot robot(GET_X_LPARAM(lP), GET_Y_LPARAM(lP));
			if (!SelectPoint(robot))
			{
				if (currentPoint != NULL)
				{
					if (!(*currentPoint)->AddPoint(CPoint(GET_X_LPARAM(lP), GET_Y_LPARAM(lP), (*currentPoint)->GetColor())))
						//std::cout << GET_X_LPARAM(lP) << GET_Y_LPARAM(lP) << std::endl;
							(*currentPoint)->DeletePoint(CPoint(GET_X_LPARAM(lP), GET_Y_LPARAM(lP)));

					if (tabPoint[*id]->GetNbPoints())
					{
						int old_x = *x;
						int old_y = *y;
						*writeRead = true;
						*cmd = 3;
						//std::cout << old_x << old_y <<std:: endl;
						CPoint point(tabPoint[*id]->GetTabPoint(0));
						*x = point.GetX();
						*y = point.GetY();
						while (*writeRead);
						*x = old_x;
						*y = old_y;
					}
				}
			}

			RECT rect;
			rect.bottom = GET_Y_LPARAM(lP) - CPoint::radius;
			rect.left = GET_X_LPARAM(lP) - CPoint::radius;
			rect.right = GET_X_LPARAM(lP) + CPoint::radius;
			rect.top = GET_Y_LPARAM(lP) + CPoint::radius;
			InvalidateRect(hWnd, &rect, 1);
		//}
		return 0;
	}

	void RefreshArea(HWND hWnd, WPARAM wP, LPARAM lP, bool ALL = false)
	{
		if (ALL)
		{
			InvalidateRect(hWnd,NULL, 1);
		}
		else
		{
			RECT rect;
			rect.bottom = GET_Y_LPARAM(lP) - CPoint::radius;
			rect.left = GET_X_LPARAM(lP) - CPoint::radius;
			rect.right = GET_X_LPARAM(lP) + CPoint::radius;
			rect.top = GET_Y_LPARAM(lP) + CPoint::radius;
			InvalidateRect(hWnd, &rect, 1);
		}
	}

	virtual LRESULT WmCommand(HWND hWnd, WPARAM wP, LPARAM lP)
	{
		switch (LOWORD(wP))
		{
		case ID_DETECTION:   break;
		case ID_EDITION_LOADPATH: LoadPath(hWnd);  break;
		case ID_EDITION_SAVEPATH: SavePath(hWnd);  break;
		case ID_EDITION_SHOWPATH: ShowPath(hWnd);  break;
		case ID_EDITION_NEWTARGET:  NewTarget(hWnd); break;
		case IDC_BUTTON1: LoadPath(hWnd);  break;
		case IDC_BUTTON2: SavePath(hWnd);  break;
		case IDC_BUTTON3: ShowPath(hWnd);  break;
		case IDC_BUTTON4:  NewTarget(hWnd); break;
		case ID_PLAY_ROBOT1:
		{			
			*writeRead = true;
			*id = 0;
			*cmd = 4;
		}
		break;
		case ID_STOP_ROBOT1: 
		{
			*writeRead = true;
			*id = 0;
			*cmd = 1;
		}
		break;
		case ID_PLAY_ROBOT2:
		{
			*writeRead = true;
			*id = 1;
			*cmd = 4;
		}
		break;
		case ID_STOP_ROBOT2:
		{
			*writeRead = true;
			*id = 1;
			*cmd = 1;
		}
		break;
		default:	break;
		}
		return 0;
	}
	int nbmj = 0;
	int nbm = 0;
	virtual LRESULT WmLButtonDown(HWND hWnd, WPARAM wP, LPARAM lP)
	{
		if (selected == 1)
		{
			TCHAR buf[32];
			int x = GET_X_LPARAM(lP), y = GET_Y_LPARAM(lP);
			DlgDirSelectEx(hWnd, buf, sizeof(buf), 201);
			int ivalr = atoi(buf);
			wsprintf(buf, _T("(%d,%d)"), x, y);
			std::cout << nbm << " " << x << " " << y << " " << std::endl;
			//CPoint(tabPoint[nbm]->SetTabPoint(1)).SetX(x);
			//tabPoint[nbm]->SetY(y);
			nbm++;
			HDC dc = ::GetDC(hWnd);
			//::SetTextColor(dc, 0x00FF00);
			//::SetBkMode(dc, TRANSPARENT);
			//aPen = CreatePen(PS_SOLID, 5, RGB(255, 0, 0));
			//SelectObject(dc, aPen);
			//Ellipse(dc, x, y, x + 2, y + 2);
			//::TextOut(dc, x, y, buf, (int)_tcslen(buf));
			//ReleaseDC(hWnd, dc);
			//char szTmpr[10];
			//DlgDirSelectEx(hWnd, szTmpr, sizeof(szTmpr), IDC_LISTR);
			//int ivalr = atoi(szTmpr);
			////fprintf(stream, "BOUTTON GAUCHE APPUYEE AUX COORDONNEES X= %d et Y= %d \n", LOWORD(lP), HIWORD(lP));
			//if (LOWORD(lP) > 180 && LOWORD(lP) < 180 + 2 * ivalr) {
			//	UpdateWindow(hWnd);
			//}
			//std::cout << nbm << " " << LOWORD(lP) << " " << HIWORD(lP) << " " << std::endl;
			//aPen = CreatePen(PS_SOLID, 5, RGB(255, 0, 0));
			//hdc = GetDC(hWnd);
			//SelectObject(hdc, aPen);
			//Ellipse(hdc, LOWORD(lP), HIWORD(lP), LOWORD(lP) + ivalr, HIWORD(lP) + ivalr);
			//ReleaseDC(hWnd, hdc);
			UpdateWindow(hWnd);
			//CPoint(tabPoint[0]->GetTabPoint(nbmj)).SetX(x); 
			//CPoint(tabPoint[0]->GetTabPoint(nbmj)).SetY(y);
			nbmj++;
			RefreshArea(hWnd, wP, lP, true);
		}
		return 0;
	}


	virtual bool SavePath(HWND hWnd)
	{
		char buffer[MAX_PATH] = "";

		if (SaveFile(hWnd, buffer))
		{
			std::ofstream stream(buffer);

			if (stream)
			{
				for (int i = 0; i < (int)tabPoint.size(); i++)
				{					
					for (int j = 0; j < tabPoint[i]->GetNbPoints(); j++)
					{
						stream << i << " " << CPoint(tabPoint[i]->GetTabPoint(j)).GetX() << " " 
						<< CPoint(tabPoint[i]->GetTabPoint(j)).GetY() << " " << std::endl;
					}
				}
				InvalidateRect(hWnd, NULL, 1);
				return true;
			}
			return false;
		}
		return false;
	}

	bool ShowPath(HWND hWnd) {
		HDC dc = ::GetDC(hWnd);
		///HPEN aPen;
		::SetTextColor(dc, 0x00FF00);
		::SetBkMode(dc, TRANSPARENT);
		aPen = CreatePen(PS_SOLID, 5, RGB(0, 255, 0));
		SelectObject(dc, aPen);
			for (int i = 0; i < (int)tabPoint.size(); i++)
			{
				for (int j = 0; j < tabPoint[i]->GetNbPoints(); j++)
				{
					int x = CPoint(tabPoint[i]->GetTabPoint(j)).GetX(), y = CPoint(tabPoint[i]->GetTabPoint(j)).GetY();
					
					UpdateWindow(hWnd);
					
				}
			}
		
		::ReleaseDC(hWnd, dc);
		return 0;
	}
	bool NewTarget(HWND hWnd) {
		selected = 1;
		return TRUE;
	}

	bool LoadPath(HWND hWnd)
	{
		char buffer[MAX_PATH] = "";

		if (OpenFile(hWnd, buffer))
		{
			std::ifstream stream(buffer);

			if (stream)
			{
				for (int i = 0; i < (int)tabPoint.size(); i++) { tabPoint[i]->DeleteAllPoint(); }

				std::string ligne;
				while (std::getline(stream, ligne))
				{
					size_t index = 0, last_index = 0;
					std::string tab[3];

					for (size_t i = 0; i < 3; i++)
					{
						last_index = index;
						index = ligne.find_first_of(' ', index + 1);
						tab[i] = ligne.substr(last_index, index - last_index);
						std::cout << tab[i] << " ";
					}
					std::cout << std::endl;

					if (std::stoi(tab[0]) < GetNbPoints())
					{
						currentPoint = &tabPoint[std::stoi(tab[0])];
						::SendMessage(hWnd, WM_RBUTTONDOWN, MK_RBUTTON, MAKELPARAM(stoi(tab[1]), stoi(tab[2])));
					}
				}
				InvalidateRect(hWnd, NULL, 1);
				return true;
			}
			return false;
		}
		return false;
	}


	virtual LRESULT WmKeyDown(HWND hWnd, WPARAM wP, LPARAM lP)
	{
		if (LOWORD(wP) == VK_RETURN) edit = edit ? false : true;

		RECT rect;
		rect.bottom = GET_Y_LPARAM(lP) - CPoint::radius;
		rect.left = GET_X_LPARAM(lP) - CPoint::radius;
		rect.right = GET_X_LPARAM(lP) + CPoint::radius;
		rect.top = GET_Y_LPARAM(lP) + CPoint::radius;
		InvalidateRect(hWnd, &rect, 1);
		return 0;
	}

	virtual LRESULT WmPaint(HWND hWnd, WPARAM wP, LPARAM lP)
	{
		PAINTSTRUCT ps;
		HDC hdc = ::BeginPaint(hWnd, &ps);
		Graphics gr(hdc);
		PaintGDIPlus(gr);
		::EndPaint(hWnd, &ps);
		return 0;
	}

	virtual void PaintGDIPlus(Graphics& graphics)
	{
		Color color(0, 0, 0);
		SolidBrush brush(color);
		Pen p(color, 2.0);

		for (int i = 0; i < GetNbPoints(); i++)
		{
			color.SetFromCOLORREF(tabPoint[i]->GetColor());
			p.SetColor(color);
			brush.SetColor(color);
			graphics.DrawEllipse(&p, (int)tabPoint[i]->GetX(), (int)tabPoint[i]->GetY(), CPoint::radius, CPoint::radius);
			graphics.FillEllipse(&brush, (int)tabPoint[i]->GetX(), (int)tabPoint[i]->GetY(), CPoint::radius, CPoint::radius);

			for (int j = 0; j < tabPoint[i]->GetNbPoints(); j++)
			{
				int x = CPoint(tabPoint[i]->GetTabPoint(j)).GetX();
				int y = CPoint(tabPoint[i]->GetTabPoint(j)).GetY();
				graphics.DrawLine(&p, (int)(x - CPoint::radius / 2), y - CPoint::radius / 2, x + CPoint::radius / 2, y + CPoint::radius / 2);
				graphics.DrawLine(&p, (int)(x + CPoint::radius / 2), y - CPoint::radius / 2, x - CPoint::radius / 2, y + CPoint::radius / 2);
			}
		}
	}

	virtual LRESULT WmTimer(HWND hWnd, WPARAM wP, LPARAM lP)
	{
		if (!(*writeRead) && *cmd == 2)
		{
			if (*id >= GetNbPoints()) {	AddPoint(CRobot(*x, *y, NewColor())); }
			else { tabPoint[*id]->SetPoint(*x, *y);	}
		}

		int eLin;

		for (int i = 0; i < GetNbPoints(); i++)
		{
			if(tabPoint[i]->GetNbPoints())
			{
				eLin = sqrt((CPoint(tabPoint[i]->GetTabPoint(0)).GetX() -tabPoint[i]->GetX())*(CPoint(tabPoint[i]->GetTabPoint(0)).GetX() - tabPoint[i]->GetX())
						  + (CPoint(tabPoint[i]->GetTabPoint(0)).GetY() - tabPoint[i]->GetY())*(CPoint(tabPoint[i]->GetTabPoint(0)).GetY() - tabPoint[i]->GetY()));

				if (eLin <= 60)
				{
					currentPoint = &tabPoint[i];
					::SendMessage(hWnd, WM_RBUTTONDOWN, MK_RBUTTON, MAKELPARAM(CPoint(tabPoint[i]->GetTabPoint(0)).GetX(), CPoint(tabPoint[i]->GetTabPoint(0)).GetY()));
				}
			}
		}	

		//InvalidateRect(hWnd, NULL, 1);
		return 0;
	}
	virtual ~CInterface() { *alive = false; }
};


int Interface(int* id, int* cmd, int* x, int* y, bool* alive, bool* writeRead)
{
	CInterface simulation(id, cmd, x, y, alive, writeRead, _T("Trajectoires robots"));
	//simulation.Creation();
	return CWind::MsgLoop();
	//system("pause");
}
#endif
