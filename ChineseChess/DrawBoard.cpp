#include <math.h>
#include "ChessDefinition.h"
#include "DrawBoard.h"

const float PI = (float)3.1415927;
const int XC[2] = {BWA / 2, BWA / 2 - 1};
const int YC[2] = {BWA / 2, BWA / 2 - 1};

/************************************************************************
 DrawStar: draw 5 star mark place of Soldier & Cannon
 x, y: position
 side: red / black
************************************************************************/
void DrawStar(HDC hdc, int x, int y, int side)
{
	int left   = XC[side] + BWA*x-3;
	int right  = XC[side] + BWA*x+3;
	int bottom = YC[side] + BWA*y-3;
	int top    = YC[side] + BWA*y+3;

	if(x != 0)
	{
		MoveToEx(hdc, left, bottom, NULL);
		LineTo(hdc, left-3, bottom);

		MoveToEx(hdc, left, bottom, NULL);
		LineTo(hdc, left, bottom - 3);

		MoveToEx(hdc, left, top, NULL);
		LineTo(hdc, left-3, top);

		MoveToEx(hdc, left, top, NULL);
		LineTo(hdc, left, top + 3);
	}
	if(x!=8)
	{
		MoveToEx(hdc, right, bottom, NULL);
		LineTo(hdc, right + 3, bottom);

		MoveToEx(hdc, right, bottom, NULL);
		LineTo(hdc, right, bottom - 3);

		MoveToEx(hdc, right, top, NULL);
		LineTo(hdc, right + 3, top);

		MoveToEx(hdc, right, top, NULL);
		LineTo(hdc, right, top+3);
	}
}

COLORREF WoodGrain(FLOAT u, FLOAT v, FLOAT w,COLORREF color,int dark)
{
	int r,g,b;
	FLOAT radius,angle;
	int grain;
	radius = (FLOAT)sqrt(u*u+w*w);
	if(w==0)
		angle = PI/2;
	else
		angle =(FLOAT) atan2(u,w);
	radius = FLOAT(radius+1.1*sin(20*angle+v/150));
	grain =int(radius)%10;

	r=GetRValue(color);
	g=GetGValue(color);
	b=GetBValue(color);

	switch(grain)
	{
	case 0:
		r=max(0,r-dark/2);
		g=max(0,g-dark/2);
		b=max(0,b-dark/2);
		color=RGB(r,g,b);
		break;
	case 1:
	case 2:
	case 3:
		r=max(0,r-dark);
		g=max(0,g-dark);
		b=max(0,b-dark);
		color=RGB(r,g,b);
		break;
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	default: 
		break;
	}
	return color;
}

void MakeWood(HDC hdc, int cx, int cy, COLORREF color, int x0, int y0, int z0, float angle0, int dark)
{
	int i, j;
	FLOAT r, angle, u, v, w;

	for (j = 0; j < cy; j++)
	{
		for (i = 0; i < cx; i++)
		{
			w = 0;
			u = (FLOAT)j*2;

		}
	}
}