#include "ChessStd.h"
#include "ChessDefinition.h"

BOOL IsNormal(const int &piece,const POINT &point)
{
	if (point.x < 1 || point.x > 9 || point.y < 1 || point.y > 10)
	{
		return FALSE;
	}

	switch(piece)
	{
		case RED_K:
			if (point.x > 6 || point.x < 4 || point.y < 8) return FALSE;
			break;
		case RED_A:
			if(!(
				(point.x == 4 && point.y == 10) ||
				(point.x == 4 && point.y == 8)  ||
				(point.x == 5 && point.y == 9)  ||
				(point.x == 6 && point.y == 10) ||
				(point.x == 6 && point.y == 8)
				))
				return FALSE;
			break;
		case RED_E:
			if(!(
				( point.x == 1 && point.y == 8) ||
				( point.x == 3 && point.y == 10)||
				( point.x == 3 && point.y == 6) ||
				( point.x == 5 && point.y == 8) ||
				( point.x == 7 && point.y == 10)||
				( point.x == 7 && point.y == 6) ||
				( point.x == 9 && point.y == 8)
				))
				return FALSE;
			break;
		case RED_S:
			if(point.y > 7) return FALSE;
			if(point.y > 5 && point.x % 2 == 0) return FALSE;
			break;
		case BLACK_K:
			if (point.x > 6 || point.x < 4 || point.y > 3) return FALSE;
			break;
		case BLACK_A:
			if(!(
				(point.x == 4 && point.y == 1) ||
				(point.x == 4 && point.y == 3) ||
				(point.x == 5 && point.y == 2) ||
				(point.x == 6 && point.y == 1) ||
				(point.x == 6 && point.y == 3)
				))
				return FALSE;
			break;
		case BLACK_E:
			if(!(
				( point.x == 1 && point.y == 3) ||
				( point.x == 3 && point.y == 1) ||
				( point.x == 3 && point.y == 5) ||
				( point.x == 5 && point.y == 3) ||
				( point.x == 7 && point.y == 1) ||
				( point.x == 7 && point.y == 5) ||
				( point.x == 9 && point.y == 3)
				))
				return FALSE;
			break;
		case BLACK_S:
			if(point.y < 4) return FALSE;
			if(point.y < 6 && point.x % 2 == 0) return FALSE;
			break;
		default:break;
	}
	return TRUE;
}

BOOL CanGo(int manmap[11][12],int piece,const POINT &from,const POINT &to)
{
	static int i, j;

	if(!IsNormal(PieceToType[piece], to))
	{
		if (PieceToType[piece] != RED_K && PieceToType[piece] != BLACK_K) 
			return FALSE;
		else
		{
			if (PieceToType[piece] == RED_K && PieceToType[manmap[to.x][to.y]] == BLACK_K)
			{
				BOOL flag = FALSE;
				for (j = from.y - 1; j > 0; j--)
				{
					if (manmap[from.x][j] != 32)
					{
						if (PieceToType[manmap[from.x][j]] == BLACK_K)
							flag = TRUE;
						break;
					}
				}
				if (flag) return TRUE;
				else return FALSE;
			}
			else
			{
				if (PieceToType[manmap[to.x][to.y]] == RED_K)
				{
					BOOL flag = FALSE;
					for (j = from.y + 1; j < 11; j++)
					{
						if (manmap[from.x][j] != 32)
						{
							if (PieceToType[manmap[from.x][j]] == RED_K)
								flag = TRUE;	
							break;
						}
					}
					if (flag) return TRUE;
					else return FALSE;
				}
				else
					return FALSE;
			}
		}
	}

	if (SideOfPiece[piece] == 0)
	{
		if(manmap[to.x][to.y] != 32 && SideOfPiece[manmap[to.x][to.y]] == 0)
			return FALSE;
	}
	else
	{
		if(SideOfPiece[piece] == 1)
			if(manmap[to.x][to.y] != 32 && SideOfPiece[manmap[to.x][to.y]] == 1)
				return FALSE;
	}

	switch(PieceToType[piece])
	{
		// Soldier move one point and can not move back
		case RED_S:
			if (to.y > from.y) return FALSE;
			if(from.y - to.y + abs(to.x - from.x) > 1) return FALSE;
			break;
		case BLACK_S:
			if(to.y < from.y) return FALSE;
			if(to.y - from.y + abs(to.x - from.x) > 1) return FALSE;
			break;
		// Advisor piece must move one point diagonally
		case RED_A:
		case BLACK_A:
			if(abs(from.y - to.y) > 1 || abs(to.x - from.x) > 1) return FALSE;
			break;
		// Elephant piece must move 2 point diagonally
		case RED_E:
		case BLACK_E:
			if (abs (from.x - to.x) != 2 || abs ( from.y - to.y ) != 2)  return FALSE;
			if (manmap[(from.x + to.x) / 2][(from.y + to.y ) / 2] != 32) return FALSE;
			break;
		// King
		case RED_K:
		case BLACK_K:
			if (abs (from.y - to.y) + abs ( to.x - from.x ) > 1)return FALSE;
			break;
		// Chariot must move orthogonally
		case RED_R:
		case BLACK_R:
			if (from.y != to.y && from.x != to.x) return FALSE;
			
			if (from.y == to.y)
			{
				if (from.x < to.x)
				{
					for (i = from.x + 1; i < to.x; i++)
						if(manmap[i][from.y] != 32) return FALSE;
				}
				else
				{
					for (i= to.x + 1; i < from.x; i++)
						if (manmap[i][from.y] != 32) return FALSE;
				}
			}
			else
			{
				if (from.y < to.y)
				{
					for (j = from.y + 1; j < to.y; j++)
						if(manmap[from.x][j] != 32) return FALSE;
				}
				else
				{
					for (j = to.y + 1; j < from.y; j++)
						if (manmap[from.x][j] != 32) return FALSE;
				}
			}
			break;
		// Cannon must move orthogonally
		case RED_C:
		case BLACK_C:
			if (from.y != to.y && from.x != to.x) return FALSE;
			
			if (manmap[to.x][to.y] == 32)
			{
				if (from.y == to.y)
				{
					if (from.x < to.x)
					{
						for (i = from.x + 1; i < to.x; i++)
							if (manmap[i][from.y] != 32) return FALSE;
					}
					else
					{
						for (i = to.x + 1; i < from.x; i++)
							if (manmap[i][from.y] != 32) return FALSE;
					}
				}
				else
				{
					if (from.y < to.y)
					{
						for (j = from.y + 1; j < to.y; j++)
							if (manmap[from.x][j] != 32) return FALSE;
					}
					else
					{
						for (j = to.y + 1; j < from.y; j++)
							if (manmap[from.x][j] != 32) return FALSE;
					}
				}
			}
			// capture a piece of other side
			else	
			{
				int count = 0;
				if (from.y == to.y)
				{
					if (from.x < to.x)
					{
						for (i = from.x + 1; i < to.x; i++)
							if(manmap[i][from.y] != 32) count++;
						if (count != 1) return FALSE;
					}
					else
					{
						for (i = to.x + 1; i < from.x; i++)
							if (manmap[i][from.y] != 32) count++;
						if (count != 1) return FALSE;
					}
				}
				else
				{
					if (from.y < to.y)
					{
						for (j = from.y + 1; j < to.y; j++)
							if (manmap[from.x][j] != 32) count++;
						if (count != 1) return FALSE;
					}
					else
					{
						for (j = to.y + 1; j < from.y; j++)
							if (manmap[from.x][j] != 32) count++;
						if (count != 1) return FALSE;
					}
				}
			}
			break;
		case RED_H:
		case BLACK_H:
			if(!(
				(abs (to.x - from.x) == 1 && abs (to.y - from.y) == 2) ||
				(abs (to.x - from.x) == 2 && abs (to.y - from.y) == 1)
				))
				return FALSE;
			// check blocking horse piece (called "hobbling the horse's leg")
			if		(to.x - from.x == 2) {i = from.x +1; j = from.y;}
			else if	(from.x - to.x == 2) {i = from.x -1; j = from.y;}
			else if	(to.y - from.y == 2) {i = from.x; j = from.y + 1;}
			else if	(from.y - to.y == 2) {i = from.x; j = from.y - 1;}

			if (manmap[i][j] != 32) 
				return FALSE;
			break;
		default:
			break;
	}

	return TRUE;	// Move success
}

void FixManMap(int map[11][12],POINT manpoint[32],int side)
{
	memcpy(map, _defaultmap, 132 * sizeof(int));
	static POINT* pman;
	static int i;
	for (i = 0; i < 32; i++)
	{
		pman = & manpoint[i];
		if (pman->x)
		{
			map[pman->x][pman->y] = i;
		}
	}
}