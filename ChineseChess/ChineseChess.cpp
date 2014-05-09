#include <Windows.h>
#include "resource.h"
#include "ChessDefinition.h"
#include "ChessThink.h"
#include "ChessStd.h"

static TCHAR szAppName[] = TEXT("ChineseChess");
HINSTANCE hInst;
//--------------------------------------------------

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void OnCreate(HWND);
void Reset();
void ShowRect(HDC, LPRECT);
void ShowPoint(HDC, POINT);
BOOL FaceToPoint(POINT &);
void OnMouseMove(POINT);
void OnLButtonDown(HWND, POINT);
void Think(HWND);
BOOL Go(HWND, int, POINT);
void Undo(HWND);
void OnGameGameSetup(HWND);
LRESULT CALLBACK About(HWND, UINT, WPARAM, LPARAM);
//--------------------------------------------------

static POINT g_chessCoordinate[32];		// pieces's coordinate
static int g_iPieceMap[11][12];		
static int g_iSide;
static HCURSOR g_hCurCantGo;
static HCURSOR g_hCurHand;
static HCURSOR g_hCurThinking;
static HICON g_hIconPiece[14];		// icon for pieces
static HICON g_hIconBox;
static HICON g_hIconSelect;
static HICON g_hIconCanMove;
static HICON g_hIconAttack;
static HDC g_hdcChessboard;
static HBITMAP g_hbmpChessboard;
static 	POINT g_pointBoxFrom;
static 	POINT g_pointBoxTo;
static 	int g_iPieceSelect;
static 	int g_iComputerSide;
static  BOOL g_bEndGame;
static  BOOL g_bCanMove[11][12];
static	struct	MOVEHISTORY	g_MoveHistory;

//--------------------------------------------------
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style = CS_VREDRAW | CS_HREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = 0;
	wndclass.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_LOGO);
	wndclass.hCursor = NULL;
	wndclass.hbrBackground = NULL;
	wndclass.lpszMenuName = (LPCTSTR)IDR_MENU;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass (&wndclass))
	{
		MessageBox (NULL, TEXT ("ERROR"), szAppName, MB_ICONERROR) ;
		return 0 ;
	}

	hInst = hInstance;
	hwnd = CreateWindow(szAppName, TEXT("Cờ tướng")
						, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
						, CW_USEDEFAULT
						, CW_USEDEFAULT
						, CW_USEDEFAULT
						, CW_USEDEFAULT
						, NULL
						, NULL
						, hInstance
						, NULL);
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	while(GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	POINT point;
	int wmId, wmEvent;

	switch(message)
	{
		case WM_CREATE:
			OnCreate(hwnd);
			break;
		case WM_PAINT:
			RECT rect2;
			hdc = BeginPaint(hwnd, &ps);
			GetClientRect(hwnd, &rect2);
			ShowRect(hdc, &rect2);
			EndPaint(hwnd, &ps);
			break;
		case WM_MOUSEMOVE:
			point.x = LOWORD(lParam);
			point.y = HIWORD(lParam);
			OnMouseMove(point);
			break;
		case WM_LBUTTONDOWN:
			point.x=LOWORD(lParam);
			point.y=HIWORD(lParam);
			OnLButtonDown(hwnd,point);
			break;
		case WM_DESTROY:
			DeleteObject(g_hbmpChessboard);
			DeleteDC(g_hdcChessboard);
			PostQuitMessage(0);
			break;
		case WM_COMMAND:
			wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			switch(wmId)
			{
				case IDM_ABOUT:
					DialogBox(hInst, (LPCTSTR)IDD_ABOUTDIALOG, hwnd, (DLGPROC)About);
					break;
				case IDM_EXIT:
					DeleteObject(g_hbmpChessboard);
					DeleteDC(g_hdcChessboard);
					PostQuitMessage(0);
					break;
				case IDM_NEW_BLACK:
					Reset();
					g_iComputerSide = RED;
					rect.left = 0;
					rect.top = 0;
					rect.right = XBW;
					rect.bottom = YBW;
					hdc = GetDC(hwnd);
					ShowRect(hdc, &rect);
					SetCursor(g_hCurThinking);
					Think(hwnd);
					SetCursor(g_hCurCantGo);
					break;
				case IDM_NEW_RED:
					Reset();
					g_iComputerSide = BLACK;
					rect.left = 0;
					rect.top = 0;
					rect.right = XBW;
					rect.bottom = YBW;
					hdc = GetDC(hwnd);
					ShowRect(hdc, &rect);
					break;
				case IDM_UNDO:
					Undo(hwnd);
					break;
				default:
					return DefWindowProc(hwnd, message, wParam, lParam);
			}
			break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
		}
	return FALSE;
}
void OnCreate(HWND hwnd)
{
	g_hCurHand = LoadCursor(hInst, MAKEINTRESOURCE(IDC_HAND));
	g_hCurCantGo = LoadCursor(hInst, MAKEINTRESOURCE(IDC_CANTGO));
	g_hCurThinking = LoadCursorFromFile(TEXT("res\\thinking.ani"));
	// Load icon for pieces
	g_hIconPiece[RED_A] = LoadIcon(hInst, MAKEINTRESOURCE(IDI_R_A));
	g_hIconPiece[RED_C] = LoadIcon(hInst, MAKEINTRESOURCE(IDI_R_C));
	g_hIconPiece[RED_E] = LoadIcon(hInst, MAKEINTRESOURCE(IDI_R_E));
	g_hIconPiece[RED_H] = LoadIcon(hInst, MAKEINTRESOURCE(IDI_R_H));
	g_hIconPiece[RED_K] = LoadIcon(hInst, MAKEINTRESOURCE(IDI_R_K));
	g_hIconPiece[RED_R] = LoadIcon(hInst, MAKEINTRESOURCE(IDI_R_R));
	g_hIconPiece[RED_S] = LoadIcon(hInst, MAKEINTRESOURCE(IDI_R_S));

	g_hIconPiece[BLACK_A] = LoadIcon(hInst, MAKEINTRESOURCE(IDI_B_A));
	g_hIconPiece[BLACK_C] = LoadIcon(hInst, MAKEINTRESOURCE(IDI_B_C));
	g_hIconPiece[BLACK_E] = LoadIcon(hInst, MAKEINTRESOURCE(IDI_B_E));
	g_hIconPiece[BLACK_H] = LoadIcon(hInst, MAKEINTRESOURCE(IDI_B_H));
	g_hIconPiece[BLACK_K] = LoadIcon(hInst, MAKEINTRESOURCE(IDI_B_K));
	g_hIconPiece[BLACK_R] = LoadIcon(hInst, MAKEINTRESOURCE(IDI_B_R));
	g_hIconPiece[BLACK_S] = LoadIcon(hInst, MAKEINTRESOURCE(IDI_B_S));

	g_hIconBox = LoadIcon(hInst, MAKEINTRESOURCE(IDI_BOX));
	g_hIconSelect = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SELECT));
	g_hIconCanMove = LoadIcon(hInst, MAKEINTRESOURCE(IDI_VALIDMOVE));
	g_hIconAttack = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ATTACK));

	Reset();
	HDC hdc = GetDC(hwnd);
	g_hdcChessboard = CreateCompatibleDC(hdc);
	g_hbmpChessboard = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BG));
	SelectObject(g_hdcChessboard, g_hbmpChessboard);

	RECT rect = {0, 0, XBW, YBW};
	AdjustWindowRect(&rect, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, TRUE);

	POINT lefttop;
	lefttop.x = (GetSystemMetrics(SM_CXSCREEN) - (rect.right - rect.left)) / 2;
	lefttop.y = (GetSystemMetrics(SM_CYSCREEN) - (rect.bottom - rect.top)) / 2;
	rect.left += lefttop.x;
	rect.right += lefttop.x;
	rect.top += lefttop.y;
	rect.bottom += lefttop.y;
	// move windows to center of the screen
	MoveWindow(hwnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
}

void Reset()
{
	int i, j;

	// RED
	g_chessCoordinate[0].x = 5; g_chessCoordinate[0].y = 10;	// KING
	g_chessCoordinate[1].x = 4; g_chessCoordinate[1].y = 10;	// LEFT ADVISOR
	g_chessCoordinate[2].x = 6; g_chessCoordinate[2].y = 10;	// RIGHT ADVISOR
	g_chessCoordinate[3].x = 3; g_chessCoordinate[3].y = 10;	// LEFT ELEPHANT
	g_chessCoordinate[4].x = 7; g_chessCoordinate[4].y = 10;	// RIGHT ELEPHANT
	g_chessCoordinate[5].x = 2; g_chessCoordinate[5].y = 10;	// LEFT HORSE
	g_chessCoordinate[6].x = 8; g_chessCoordinate[6].y = 10;	// RIGHT HORSE
	g_chessCoordinate[7].x = 1; g_chessCoordinate[7].y = 10;	// LEFT CHARIOT
	g_chessCoordinate[8].x = 9; g_chessCoordinate[8].y = 10;	// RIGHT CHARIOT
	g_chessCoordinate[9].x = 2; g_chessCoordinate[9].y = 8;		// LEFT CANNON
	g_chessCoordinate[10].x = 8; g_chessCoordinate[10].y = 8;	// RIGHT CANNON
	g_chessCoordinate[11].x = 1; g_chessCoordinate[11].y = 7;	// SOLDIER
	g_chessCoordinate[12].x = 3; g_chessCoordinate[12].y = 7;	// SOLDIER
	g_chessCoordinate[13].x = 5; g_chessCoordinate[13].y = 7;	// SOLDIER
	g_chessCoordinate[14].x = 7; g_chessCoordinate[14].y = 7;	// SOLDIER
	g_chessCoordinate[15].x = 9; g_chessCoordinate[15].y = 7;	// SOLDIER

	// BLACK
	g_chessCoordinate[16].x = 5; g_chessCoordinate[16].y = 1;	// KING
	g_chessCoordinate[17].x = 4; g_chessCoordinate[17].y = 1;	// LEFT ADVISOR
	g_chessCoordinate[18].x = 6; g_chessCoordinate[18].y = 1;	// RIGHT ADVISOR
	g_chessCoordinate[19].x = 3; g_chessCoordinate[19].y = 1;	// LEFT ELEPHANT
	g_chessCoordinate[20].x = 7; g_chessCoordinate[20].y = 1;	// RIGHT ELEPHANT
	g_chessCoordinate[21].x = 2; g_chessCoordinate[21].y = 1;	// LEFT HORSE
	g_chessCoordinate[22].x = 8; g_chessCoordinate[22].y = 1;	// RIGHT HORSE
	g_chessCoordinate[23].x = 1; g_chessCoordinate[23].y = 1;	// LEFT CHARIOT
	g_chessCoordinate[24].x = 9; g_chessCoordinate[24].y = 1;	// RIGHT CHARIOT
	g_chessCoordinate[25].x = 2; g_chessCoordinate[25].y = 3;	// LEFT CANNON
	g_chessCoordinate[26].x = 8; g_chessCoordinate[26].y = 3;	// RIGHT CANNON
	g_chessCoordinate[27].x = 1; g_chessCoordinate[27].y = 4;	// SOLDIER
	g_chessCoordinate[28].x = 3; g_chessCoordinate[28].y = 4;	// SOLDIER
	g_chessCoordinate[29].x = 5; g_chessCoordinate[29].y = 4;	// SOLDIER
	g_chessCoordinate[30].x = 7; g_chessCoordinate[30].y = 4;	// SOLDIER
	g_chessCoordinate[31].x = 9; g_chessCoordinate[31].y = 4;	// SOLDIER

	g_iSide = RED;	// Default, man play red side

	FixManMap(g_iPieceMap, g_chessCoordinate, g_iSide);
	g_pointBoxFrom.x = 0;
	g_pointBoxFrom.y = 0;
	g_pointBoxTo.x = 0;
	g_pointBoxTo.y = 0;
	g_iPieceSelect = 32;
	g_iComputerSide = 1;
	g_bEndGame = FALSE;
	g_MoveHistory.count = 0;

	for (i = 0; i < 11; i++)
	{
		for (j = 0; j < 12; j++)
		{
			g_bCanMove[i][j] = FALSE;
		}
	}
}

void ShowRect(HDC hdc, LPRECT prect)
{
	RECT rc1 = { 0, 0, XBW, YBW };
	
	IntersectRect(&rc1, &rc1, prect);
	BitBlt(  hdc, rc1.left, rc1.top
		   , rc1.right - rc1.left
		   , rc1.bottom - rc1.top
		   , g_hdcChessboard, rc1.left, rc1.top, SRCCOPY);

	int left = (rc1.left) / BWA;
	int top = (rc1.top) / BWA;
	int right = (rc1.right) / BWA;
	int bottom = (rc1.bottom) / BWA;

	for (int i = left; i <= right; i++)
	{
		for (int j = top; j <= bottom; j++)
		{
			if (g_bCanMove[i + 1][j + 1])
			{
				if (g_iPieceMap[i + 1][j + 1] == 32)
				{
					DrawIcon(hdc, i*BWA + SW + 7, j*BWA + SW + 5, g_hIconCanMove);
				}
				else
				{
					DrawIcon(hdc, i*BWA + SW + 5, j*BWA + SW + 5, g_hIconAttack);
				}
			}

			if (g_iPieceMap[i + 1][j + 1] != 32)
			{
				DrawIcon(hdc, i*BWA + SW + 5, j*BWA + SW + 5, g_hIconPiece[PieceToIcon[g_iPieceMap[i + 1][j + 1]]]);
			}

			if( g_pointBoxFrom.x == i + 1 && 
				g_pointBoxFrom.y == j + 1 ||
				g_pointBoxTo.x == i + 1 && 
				g_pointBoxTo.y == j + 1
			  )
			{
				DrawIcon(hdc, i*BWA + SW + 6, j*BWA + SW + 6, g_hIconBox);
			}
			if(g_iPieceSelect != 32)
			{
				if( g_chessCoordinate[g_iPieceSelect].x == i + 1 &&
					g_chessCoordinate[g_iPieceSelect].y == j + 1
				  )
				{
					DrawIcon(hdc, i*BWA + SW + 5, j*BWA + SW + 5, g_hIconSelect);
				}
			}
		}
	}
}

void ShowPoint(HDC hdc, POINT point)
{
	RECT rect;
	rect.left = (point.x-1) * BWA;
	rect.top = (point.y-1) * BWA;
	rect.right = rect.left + BWA;
	rect.bottom = rect.top + BWA;
	ShowRect(hdc, &rect);
}

BOOL FaceToPoint(POINT &point)
{
	if ((point.x) % BWA < SW ||
		(point.x) % BWA > BWA - SW ||
		(point.y) % BWA < SW ||
		(point.y % BWA > BWA - SW))
	{
		return FALSE;
	}
	POINT p;
	p.x = (point.x) / BWA + 1;
	p.y = (point.y) / BWA + 1;

	if (p.x < 1 || p.x > 9 || p.y < 1 || p.y > 10)
	{
		return FALSE;
	}

	point = p;
	return TRUE;
}

void OnMouseMove(POINT point)
{
	if (FaceToPoint(point))
	{
		if (g_iPieceSelect != 32)
		{
			if (CanGo(g_iPieceMap, g_iPieceSelect, g_chessCoordinate[g_iPieceSelect], point))
			{
				SetCursor(g_hCurHand);
			}
			else
			{
				SetCursor(g_hCurCantGo);
			}
		}
		else
		{
			if (SideOfPiece[g_iPieceMap[point.x][point.y]] != g_iComputerSide)
			{
				SetCursor(g_hCurHand);
			}
			else
			{
				SetCursor(g_hCurCantGo);
			}
		}
	}
}

void OnLButtonDown(HWND hwnd, POINT point)
{
	int i, j;

	if(g_bEndGame)
	{
		MessageBox(hwnd,TEXT("Game over!"), TEXT("MantisChess"), MB_OK);
	}
	else
	{
		if(FaceToPoint(point))
		{
			if (SideOfPiece[g_iPieceMap[point.x][point.y]] == 1 - g_iComputerSide)
			{
				HDC hdc = GetDC(hwnd);
				POINT p;
				BOOL flag = FALSE;
				if (g_iPieceSelect != 32)
				{
					p = g_chessCoordinate[g_iPieceSelect];
					flag = TRUE;
				}
				g_iPieceSelect = g_iPieceMap[point.x][point.y];

				for (i = 0; i < 11; i++)
				{
					for (j = 0; j < 12; j++)
					{
						POINT pt;
						pt.x = i;
						pt.y = j;
						if (CanGo(g_iPieceMap, g_iPieceSelect, g_chessCoordinate[g_iPieceSelect], pt))
						{
							if(!g_bCanMove[i][j])
							{
								g_bCanMove[i][j] = TRUE;
								ShowPoint(hdc, pt);
							}
						}
						else
						{
							if(g_bCanMove[i][j])
							{
								g_bCanMove[i][j] = FALSE;
								ShowPoint(hdc, pt);
							}
						}
					}
				}

				ShowPoint(hdc, g_chessCoordinate[g_iPieceSelect]);
				if(flag)
					ShowPoint(hdc, p);
			}
			else
			{
				if(g_iPieceSelect != 32)
				{
					Go(hwnd, g_iPieceSelect, point);
				}
			}
		}
	}
}

void Think(HWND hwnd)
{
	int i, j;
	POINT piecesCoordinate[32];
	int side;
	int map[11][12];
	
	for (i = 0; i < 32; i++)
	{
		piecesCoordinate[i] = g_chessCoordinate[i];
	}
	side = g_iSide;
	for (i = 0; i < 11; i++)
	{
		for (j = 0; j < 12; j++)
		{
			map[i][j] = g_iPieceMap[i][j];
		}
	}

	int resultPiece = 32;
	POINT resultPoint = {0, 0};
	BOOL flag = Think(map, piecesCoordinate, side, resultPiece, resultPoint);
	
	if (flag)
	{
		Go(hwnd, resultPiece, resultPoint);
	}
	else
	{
		g_bEndGame = TRUE;
		MessageBox(hwnd, TEXT("Game over!"), TEXT("Chinese Chess"), MB_OK | MB_ICONWARNING);
	}
}

BOOL Go(HWND hwnd, int piece, POINT targetpoint)
{
	int i, j;
	HDC hdc = GetDC(hwnd);

	if (g_bEndGame)
		return FALSE;

	if(g_MoveHistory.count >= MAXMOVE)
		g_MoveHistory.count = 0;

	if (!CanGo(g_iPieceMap, piece, g_chessCoordinate[piece], targetpoint))
		return FALSE;

	// Save to history
	g_MoveHistory.betaken[g_MoveHistory.count] = g_iPieceMap[targetpoint.x][targetpoint.y];
	g_MoveHistory.pieces[g_MoveHistory.count] = piece;
	g_MoveHistory.from[g_MoveHistory.count] = g_chessCoordinate[piece];
	g_MoveHistory.to[g_MoveHistory.count] = targetpoint;
	g_MoveHistory.count++;

	if(g_iPieceMap[targetpoint.x][targetpoint.y] != 32)
	{
		g_chessCoordinate[g_iPieceMap[targetpoint.x][targetpoint.y]].x = 0;
	}

	POINT from, to;
	from = g_pointBoxFrom;
	to = g_pointBoxTo;
	g_pointBoxFrom = g_chessCoordinate[piece];
	g_pointBoxTo = targetpoint;

	POINT oldselect;
	oldselect = g_chessCoordinate[piece];
	g_chessCoordinate[piece] = targetpoint;
	FixManMap (g_iPieceMap, g_chessCoordinate, g_iSide);

	g_iPieceSelect = 32;
	g_iSide = 1 - g_iSide;

	for (i = 0; i < 11; i++)
	{
		for (j = 0; j < 12; j++)
		{
			POINT pt;
			pt.x = i;
			pt.y = j;

			if(CanGo(g_iPieceMap, piece, g_chessCoordinate[piece], pt))
			{
				if (!g_bCanMove[i][j])
				{
					g_bCanMove[i][j] = TRUE;
					ShowPoint(hdc, pt);
				}
			}
			else
			{
				if (g_bCanMove[i][j])
				{
					g_bCanMove[i][j] = FALSE;
					ShowPoint(hdc, pt);
				}
			}
		}
	}

	ShowPoint(hdc, oldselect);
	ShowPoint(hdc, targetpoint);
	ShowPoint(hdc, to);
	ShowPoint(hdc, from);

	if (g_chessCoordinate[FirstOfSide[g_iSide]].x == 0)
	{
		MessageBox(hwnd, TEXT("Game over!"), TEXT("Chinese Chess"), MB_OK | MB_ICONWARNING);
		g_bEndGame = TRUE;
		return TRUE;
	}

	if (g_iComputerSide == g_iSide && !g_bEndGame)
	{	SetCursor(g_hCurThinking);
		Think(hwnd);
		SetCursor(g_hCurCantGo);
	}
	return TRUE;
}

void Undo(HWND hwnd)
{
	int i, j;
	if (g_MoveHistory.count < 2)
		return;

	for (i = g_MoveHistory.count - 1; i >= g_MoveHistory.count - 2; i--)
	{
		g_chessCoordinate[g_MoveHistory.pieces[i]] = g_MoveHistory.from[i];
		g_iPieceSelect = g_MoveHistory.pieces[i];
		g_iPieceMap[g_MoveHistory.from[i].x][g_MoveHistory.from[i].y] = g_MoveHistory.pieces[i];
		g_pointBoxFrom = g_MoveHistory.from[i];
		
		if (g_MoveHistory.betaken[i] != 32)
		{
			g_chessCoordinate[g_MoveHistory.betaken[i]] = g_MoveHistory.to[i];
		}
		
		g_iPieceMap[g_MoveHistory.to[i].x][g_MoveHistory.to[i].y] = g_MoveHistory.betaken[i];
		g_pointBoxTo=g_MoveHistory.to[i];
	}

	g_MoveHistory.count -= 2;
	g_bEndGame = FALSE;

	for (i = 0; i < 11; i++)
	{
		for (j = 0; j < 12; j++)
		{
			g_bCanMove[i][j] = FALSE;
		}

		InvalidateRect (hwnd, NULL, TRUE);
	}
}