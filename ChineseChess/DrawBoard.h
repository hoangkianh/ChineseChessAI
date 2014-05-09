#include <Windows.h>
void DrawStar(HDC hdc, int x, int y, int i);
void MakeWood(HDC hdc, int cx, int cy, COLORREF color, int x0, int y0, int z0, float angle0, int dark);
COLORREF WoodGrain(FLOAT u, FLOAT v, FLOAT w, COLORREF color, int dark);
void MakeBoard(HDC hdc, COLORREF cr);