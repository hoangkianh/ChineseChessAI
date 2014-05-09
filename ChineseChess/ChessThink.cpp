#include <Windows.h>
#include "ChessDefinition.h"
#include "ChessThink.h"

#define S_WIDTH 8
#define S_DEPTH 6

////////////////////// King - Advisor - Elephant - Horse - Chariot - Cannon - Soldier
const int base[7] =  { 300,      400,        300,	600,      1000,     600,    300};
const int range[7] = {   0,        0,         0,	 20,        10,       0,   50};

const int contactpercent1 = 20;	// emphasis on defend
const int contactpercent2 = 25; // emphasis on attack

const int BasicValue[7] = {  
						base[0] - base[0] * range[0] / 100,
						base[1] - base[1] * range[1] / 100,
						base[2] - base[2] * range[2] / 100,
						base[3] - base[3] * range[3] / 100,
						base[4] - base[4] * range[4] / 100,
						base[5] - base[5] * range[5] / 100,
						base[6] - base[6] * range[6] / 100
					};
// value for Solider in different places
const int BasicValue_Soldier[5] =
					{
						0,
						1 * 2 * base[6] * range[6] / 100 / 4,
						2 * 2 * base[6] * range[6] / 100 / 4,
						3 * 2 * base[6] * range[6] / 100 / 4,
						4 * 2 * base[6] * range[6] / 100 / 4,
					};

const int BonusForSoldier[2][12][11]=
{
	{
		{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
		{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
		{  0,  1,  2,  3,  4,  4,  4,  3,  2,  1,  0},
		{  0,  1,  2,  3,  4,  4,  4,  3,  2,  1,  0},
		{  0,  1,  2,  3,  3,  3,  3,  3,  2,  1,  0},
		{  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0},
		{  0,  0,  0,  1,  0,  0,  0,  1,  0,  0,  0},
		{  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0},
		{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
		{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
		{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
		{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}
	},
	{
		{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
		{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
		{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
		{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
		{  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0},
		{  0,  0,  0,  1,  0,  0,  0,  1,  0,  0,  0},
		{  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0},
		{  0,  1,  2,  3,  3,  3,  3,  3,  2,  1,  0},
		{  0,  1,  2,  3,  4,  4,  4,  3,  2,  1,  0},
		{  0,  1,  2,  3,  4,  4,  4,  3,  2,  1,  0},
		{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
		{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}
	}
};


#define NORED(i,j) (SideOfPiece[map[i][j]] != RED)
#define NOBLACK(i, j) (SideOfPiece[map[i][j]] != BLACK)
#define EMPTY(i, j) (map[i][j] == 32)

void Swap(int & a, int & b)
{
	int c = a;
	a = b;
	b = c;
}

void SwapPoint(POINT& a, POINT& b)
{
	POINT c = a;
	a = b;
	b = c;
}

void QuickSort(int value[], int pieces[], POINT targetpoint[], int low, int high)
{
	int pivot;
	int pivot_piece;
	int mid;
	int scanUp, scanDown;
	POINT pivot_point;

	if (high - low <= 0)
	{
		return;
	}
	else
	{
		if (high - low == 1)
		{
			if (value[high] > value[low])
			{
				Swap(value[high], value[low]);
				Swap(pieces[high], pieces[low]);
				SwapPoint(targetpoint[high], targetpoint[low]);
				return;
			}
		}
	}

	mid = (low +high) / 2;
	pivot = value[mid];
	pivot_piece = pieces[mid];
	pivot_point = targetpoint[mid];

	Swap(value[mid], value[low]);
	Swap(pieces[mid], pieces[low]);
	SwapPoint(targetpoint[mid], targetpoint[low]);

	scanUp = low+1;
	scanDown = high;

	do {
		while (scanUp <= scanDown && value[scanUp] >= pivot)
			scanUp++;
		while (pivot > value[scanDown])
			scanDown--;
		if (scanUp < scanDown)
		{
			Swap(value[scanUp], value[scanDown]);
			Swap(pieces[scanUp], pieces[scanDown]);
			SwapPoint(targetpoint[scanUp], targetpoint[scanDown]);
		}
	}while (scanUp < scanDown);

	value[low] = value[scanDown];
	value[scanDown] = pivot;
	pieces[low] = pieces[scanDown];
	pieces[scanDown] = pivot_piece;
	targetpoint[low] = targetpoint[scanDown];
	targetpoint[scanDown] = pivot_point;

	if(low < scanDown - 1)
		QuickSort(value, pieces, targetpoint, low, scanDown - 1);
	if(scanDown+1<high)
		QuickSort(value, pieces, targetpoint, scanDown + 1, high);
}

// Evaluate board value
int Value(int map[11][12], POINT pieceCoordinate[32], int &side)
{
	int k;
	int PieceExtValue[32];
	int PieceBaseValue[32];
	int PieceContact[32][32];
	int BeAteCount[32];
	BOOL OwnSee[32];
	
	memset(PieceContact, 0, sizeof(int) << 10);
	memset(PieceBaseValue, 0, sizeof(int) << 5);
	memset(PieceExtValue, 0, sizeof(int) << 5);
	memset(BeAteCount, 0, sizeof(int) << 5);
	memset(OwnSee, 0, sizeof(int) << 5);

	int maxvalue = 0;
	int i, j;

	// We evaluate (MAX)
	for (i = FirstOfSide[side]; i <= LastOfSide[side]; i++)
	{
		if (PieceContact[i][FirstOfSide[!side]] != 0)
		{
			maxvalue = 9700;
			return maxvalue;
		}
	}

	for (i = 0; i < 32; i++)
	{
		k = PieceToType7[i];
		PieceBaseValue[i] = BasicValue[k] + PieceBaseValue[i];// * BV2[k];
		// if is soldier
		if (k == 6)		
		{
			PieceBaseValue[i] += BasicValue_Soldier[BonusForSoldier[SideOfPiece[i]][pieceCoordinate[i].y][pieceCoordinate[i].x]];
		}
	}

	for (i = 0; i < 32; i++)
	{
		for (j = 0; j < 32; j++)
		{
			if (PieceContact[i][j] != 0)
			{
				if (SideOfPiece[i] == SideOfPiece[j])
				{
					BeAteCount[j]++;
					if (!OwnSee[j])
					{
						PieceExtValue[i] += PieceBaseValue[j] * contactpercent1 / 100;	//one's own
						OwnSee[j] = TRUE;
					}
				}
				else
				{
					PieceExtValue[i] += PieceBaseValue[j] * contactpercent2 / 100;		//other side
					BeAteCount[j]--;
				}
			}
		}
	}

	// (MAX)
	for (i = FirstOfSide[side]; i <= LastOfSide[side]; i++)
	{
		if (pieceCoordinate[i].x)
			maxvalue += PieceBaseValue[i] + PieceExtValue[i];			
	}

	// opponent evaluate (MIN)
	BOOL flag = FALSE;
	k = 32;
	for (i = FirstOfSide[1-side]; i <= LastOfSide[1 - side]; i++)
	{
		if (pieceCoordinate[i].x)
			maxvalue -= PieceBaseValue[i] + PieceExtValue[i];
		
		if (PieceContact[i][FirstOfSide[side]] != 0)
		{
			flag = TRUE;
			k = i;
			break;
		}
	}

	if (flag && BeAteCount[k] >= 0)
	{
		j = 0;
		for (i = FirstOfSide[side]; i <= LastOfSide[side]; i++)
		{
			if(BeAteCount[i] < 0 && PieceBaseValue[i] > j)
				j = PieceBaseValue[i];
		}
		maxvalue -= j;
	}
	else
	{
		j = 0;
		for (i = FirstOfSide[!side]; i <= LastOfSide[!side]; i++)
		{
			if (BeAteCount[i] < 0 && PieceBaseValue[i] > j)
				j = PieceBaseValue[i];
		}
		maxvalue += j;
	}
	return maxvalue;
}

// Generate valid move
BOOL EnumList(int map[11][12], POINT pieceCoordinate[32], int &side, int *piecesArray, POINT *move, int &count)
{	
	#define ADD(p, tx, ty) {piecesArray[count] = p, move[count].x = tx, move[count].y = ty; count++;if (map[tx][ty] == FirstOfSide[1-side]) goto _NOKING;}
	
	int i, j, n, x, y;
	BOOL flag;
	count = 0;
	
	for(n = FirstOfSide[side]; n <= LastOfSide[side]; n++)
	{
		x = pieceCoordinate[n].x;
		if(x == 0) continue;
		y = pieceCoordinate[n].y;

		switch(n)
		{
			case 0:// RED KING
				if (pieceCoordinate[0].x == pieceCoordinate[16].x)
				{
					flag = FALSE;
					for (j = pieceCoordinate[16].y + 1; j < pieceCoordinate[0].y; j++)
					{
						if (map[x][j] != 32)
						{
							flag = TRUE;
							break;
						}
					}
					if (!flag)
					{
						ADD(0, x, pieceCoordinate[16].y)
					}
				}
				j = y + 1; if (j <= 10 && NORED(x, j))	ADD(0, x, j)
				j = y - 1; if (j >=  8 && NORED(x, j))	ADD(0, x, j)
				i = x + 1; if (i <=  6 && NORED(i, y))	ADD(0, i, y)
				i = x - 1; if (i >=  4 && NORED(i, y))	ADD(0, i, y)
				break;
			case 16: // BLACK KING
				if (pieceCoordinate[0].x == pieceCoordinate[16].x)
				{
					flag = FALSE;
					for (j = pieceCoordinate[16].y + 1; j < pieceCoordinate[0].y; j++)
					{
						if(map[x][j] != 32)
						{
							flag = TRUE;
							break;
						}
					}
					if(!flag)
					{
						ADD(16, x, pieceCoordinate[0].y);
					}
				}
				j = y + 1; if (j <= 3 && NOBLACK(x, j))	ADD(16, x, j)
				j = y - 1; if (j >= 1 && NOBLACK(x, j))	ADD(16, x, j)
				i = x + 1; if (i <= 6 && NOBLACK(i, y))	ADD(16, i, y)
				i = x - 1; if (i >= 4 && NOBLACK(i, y))	ADD(16, i, y)
				break;
			// RED ADVISOR
			case 1: 
			case 2:
				i = x + 1; j = y + 1; if (i <= 6 && j <= 10 && NORED(i, j))		ADD(n, i, j)
				i = x + 1; j = y - 1; if (i <= 6 && j >= 8  && NORED(i, j))		ADD(n, i, j)
				i = x - 1; j = y + 1; if (i >= 4 && j <= 10 && NORED(i, j))		ADD(n, i, j)
				i = x - 1; j = y - 1; if (i >= 4 && j >= 8  && NORED(i, j))		ADD(n, i, j)
				break;
			// BLACK ADVISOR
			case 17:
			case 18:
				i = x + 1; j = y + 1; if (i <= 6 && j <= 3 && NOBLACK(i, j))	ADD(n, i, j)
				i = x + 1; j = y - 1; if (i <= 6 && j >= 1 && NOBLACK(i, j))	ADD(n, i, j)
				i = x - 1; j = y + 1; if (i >= 4 && j <= 3 && NOBLACK(i, j))	ADD(n, i, j)
				i = x - 1; j = y - 1; if (i >= 4 && j >= 1 && NOBLACK(i, j))	ADD(n, i, j)
				break;
			// RED ELEPHANT
			case 3:
			case 4:
				i = x + 2; j = y + 2; if (i <= 9 && j <= 10 && NORED(i, j))	if (EMPTY(x + 1, y + 1)) ADD(n, i, j)
				i = x + 2; j = y - 2; if (i <= 9 && j >= 6  && NORED(i, j))	if (EMPTY(x + 1, y - 1)) ADD(n, i, j)
				i = x - 2; j = y + 2; if (i >= 1 && j <= 10 && NORED(i, j))	if (EMPTY(x - 1, y + 1)) ADD(n, i, j)
				i = x - 2; j = y - 2; if (i >= 1 && j >= 6  && NORED(i, j))	if (EMPTY(x - 1, y - 1)) ADD(n, i, j)
				break;
			// BLACK ELEPHANT
			case 19:
			case 20:
				i = x + 2; j = y + 2; if (i <= 9 && j <= 5  && NOBLACK(i, j)) if (EMPTY(x + 1, y + 1)) ADD(n, i, j)
				i = x + 2; j = y - 2; if (i <= 9 && j >= 1  && NOBLACK(i, j)) if (EMPTY(x + 1, y - 1)) ADD(n, i, j)
				i = x - 2; j = y + 2; if (i >= 1 && j <= 5  && NOBLACK(i, j)) if (EMPTY(x - 1, y + 1)) ADD(n, i, j)
				i = x - 2; j = y - 2; if (i >= 1 && j >= 1  && NOBLACK(i, j)) if (EMPTY(x - 1, y - 1)) ADD(n, i, j)
				break;
			// RED HORSE
			case 5:
			case 6:
				i = x + 1;
				if (EMPTY(i, y))
				{
					i = x + 2; j = y + 1; if (i <= 9 && j <= 10 && NORED(i, j))	ADD(n, i, j)
					i = x + 2; j = y - 1; if (i <= 9 && j >= 1  && NORED(i, j))	ADD(n, i, j)
				}
				i = x - 1;
				if (EMPTY(i, y))
				{
					i = x - 2; j = y + 1; if (i >= 1 && j <= 10 && NORED(i, j))	ADD(n, i, j)
					i = x - 2; j = y - 1; if (i >= 1 && j >= 1  && NORED(i, j))	ADD(n, i, j)
				}
				j = y + 1;
				if (EMPTY(x, j))
				{
					i = x + 1; j = y + 2; if (i <= 9 && j <= 10 && NORED(i, j))	ADD(n, i, j)
					i = x - 1; j = y + 2; if (i >= 1 && j <= 10 && NORED(i, j))	ADD(n, i, j)
				}
				j = y - 1;
				if (EMPTY(x, j))
				{
					i = x + 1; j = y - 2; if (i <= 9 && j >= 1 && NORED(i, j))	ADD(n, i, j)
					i = x - 1; j = y - 2; if (i >= 1 && j >= 1 && NORED(i, j))	ADD(n, i, j)
				}
				break;
			// BLACK HORSE
			case 21:
			case 22:
				i = x + 1;
				if (EMPTY(i, y))
				{
					i = x + 2; j = y + 1; if (i <= 9 && j <= 10 && NOBLACK(i, j))	ADD(n, i, j)
					i = x + 2; j = y - 1; if (i <= 9 && j >= 1  && NOBLACK(i, j))	ADD(n, i, j)
				}
				i = x - 1;
				if (EMPTY(i, y))
				{
					i = x - 2; j = y + 1; if (i >= 1 && j <= 10 && NOBLACK(i, j))	ADD(n, i, j)
					i = x - 2; j = y - 1; if (i >= 1 && j >= 1  && NOBLACK(i, j))	ADD(n, i, j)
				}
				j = y + 1;
				if (EMPTY(x, j))
				{
					i = x + 1; j = y + 2; if (i <= 9 && j <= 10 && NOBLACK(i, j))	ADD(n, i, j)
					i = x - 1; j = y + 2; if (i >= 1 && j <= 10 && NOBLACK(i, j))	ADD(n, i, j)
				}
				j = y - 1;
				if (EMPTY(x, j))
				{
					i = x + 1; j = y - 2; if (i <= 9 && j >= 1 && NOBLACK(i, j))	ADD(n, i, j)
					i = x - 1; j = y - 2; if (i >= 1 && j >= 1 && NOBLACK(i, j))	ADD(n, i, j)
				}
				break;
			// RED CHARIOT
			case 7:
			case 8:
				i = x + 1;
				while (i <= 9)
				{
					if (EMPTY(i, y))	ADD(n, i, y)
					else
					{
						if (NORED(i, y))	ADD(n, i, y)
						break;
					}
					i++;
				}
				i = x - 1;
				while (i >= 1)
				{
					if (EMPTY(i, y))	ADD(n, i, y)
					else
					{
						if (NORED(i, y))	ADD(n, i, y)
						break;
					}
					i--;
				}
				j = y + 1;
				while (j <= 10)
				{
					if (EMPTY(x, j))	ADD(n, x, j)
					else
					{
						if (NORED(x, j))	ADD(n, x, j)
						break;
					}
					j++;
				}
				j = y - 1;
				while(j >= 1)
				{
					if (EMPTY(x, j))	ADD(n, x, j)
					else
					{
						if (NORED(x, j))	ADD(n, x, j)
						break;
					}
					j--;
				}
				break;
			// BLACK CHARIOT
			case 23:
			case 24:
				i = x + 1;
				while (i <= 9)
				{
					if (EMPTY(i, y))	ADD(n, i, y)
					else
					{
						if (NOBLACK(i, y))	ADD(n, i, y)
						break;
					}
					i++;
				}
				i = x - 1;
				while (i >= 1)
				{
					if (EMPTY(i, y))	ADD(n, i, y)
					else
					{
						if (NOBLACK(i, y))	ADD(n, i, y)
						break;
					}
					i--;
				}
				j = y + 1;
				while (j <= 10)
				{
					if (EMPTY(x, j))	ADD(n, x, j)
					else
					{
						if (NOBLACK(x, j))	ADD(n, x, j)
						break;
					}
					j++;
				}
				j = y - 1;
				while(j >= 1)
				{
					if (EMPTY(x, j))	ADD(n, x, j)
					else
					{
						if (NOBLACK(x, j))	ADD(n, x, j)
						break;
					}
					j--;
				}
				break;
			// RED CANNON
			case 9:
			case 10:
				i = x + 1; flag = FALSE;
				while (i <= 9)
				{
					if (EMPTY(i, y))
					{
						if (!flag)	ADD(n, i, y)
					}
					else
					{
						if (!flag) flag = TRUE;
						else 
						{
							if (NORED(i, y))	ADD(n, i, y)
							break;
						}
					}
					i++;
				}

				i = x - 1; flag = FALSE;
				while(i >= 1)
				{
					if (EMPTY(i, y))
					{
						if (!flag)	ADD(n, i, y)
					}
					else
					{
						if (!flag)  flag = TRUE;
						else 
						{
							if (NORED(i, y))	ADD(n, i, y)
							break;
						}
					}
					i--;
				}

				j = y + 1; flag = FALSE;
				while (j <= 10)
				{
					if (EMPTY(x, j)) 
					{
						if (!flag)	ADD(n, x, j)
					}
					else
					{
						if (!flag) flag = TRUE;
						else 
						{
							if (NORED(x, j))	ADD(n, x, j)
							break;
						}
					}
					j++;
				}

				j = y - 1; flag = FALSE;
				while (j >= 1)
				{
					if (EMPTY(x, j))
					{
						if (!flag)	ADD(n, x, j)
					}
					else
					{
						if (!flag) flag = TRUE;
						else 
						{
							if (NORED(x, j))	ADD(n, x, j)
							break;
						}
					}
					j--;
				}
				break;
			// BLACK CANNON
			case 25:
			case 26:
				i = x + 1; flag = FALSE;
				while (i <= 9)
				{
					if (EMPTY(i, y))
					{
						if (!flag)	ADD(n, i, y)
					}
					else
					{
						if (!flag) flag = TRUE;
						else
						{
							if (NOBLACK(i, y))	ADD(n,i,y)
							break;
						}
					}
					i++;
				}

				i = x - 1; flag = FALSE;
				while (i >= 1)
				{
					if (EMPTY(i, y)) 
					{
						if (!flag)	ADD(n, i, y)
					}
					else
					{
						if (!flag) flag = TRUE;
						else 
						{
							if (NOBLACK(i, y))	ADD(n, i, y)
							break;
						}
					}
					i--;
				}

				j = y + 1; flag = FALSE;
				while (j <= 10)
				{
					if (EMPTY(x, j))
					{
						if (!flag)	ADD(n, x, j)
					}
					else
					{
						if (!flag) flag = TRUE;
						else 
						{
							if (NOBLACK(x, j))	ADD(n, x, j)
							break;
						}
					}
					j++;
				}

				j = y - 1; flag = FALSE;
				while (j >= 1)
				{
					if (EMPTY(x, j))
					{
						if (!flag)	ADD(n, x, j)
					}
					else
					{
						if (!flag) flag = TRUE;
						else 
						{
							if (NOBLACK(x, j))	ADD(n, x, j)
							break;
						}
					}
					j--;
				}
				break;
			// RED SOLDIER
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
				j = y - 1;
				if (j >= 1 && NORED(x, j))	ADD(n, x, j)
					if (y <= 5)
					{
						i = x + 1; if (i <= 9 && NORED(i, y))	ADD(n, i, y)
						i = x - 1; if (i >= 1 && NORED(i,y))	ADD(n, i, y)
					}
					break;				
				break;
			case 27:
			case 28:
			case 29:
			case 30:
			case 31:
				j = y + 1;
				if (j <= 10 && NOBLACK(x, j))	ADD(n, x, j)
					if (y >= 6)
					{
						i = x + 1; if (i <= 9 && NOBLACK(i, y))	ADD(n, i, y)
						i = x - 1; if (i >= 1 && NOBLACK(i, y))	ADD(n, i, y)
					}
				break;
		}
	}

	return TRUE;
_NOKING:
	return FALSE;
}

int Search(int map[11][12], POINT pieceCoordinate[32], int &side, int piece, POINT point, int upmax, int depth)
{
	int ate, cur, maxvalue, curvalue, xs, ys;
	int count;

	ate = 32;	// that't mean piece is not captured

	// Piece is moved
	xs = pieceCoordinate[piece].x;	//original position
	ys = pieceCoordinate[piece].y;
	
	if(SideOfPiece[map[point.x][point.y]] == 1-side)	//piece already exits in position?
	{
		ate = map[point.x][point.y];	// record captured piece
		if (ate == 0 || ate == 16)		// Yeah, capture KING!!
		{
			return 9999;
		}
		pieceCoordinate[ate].x = 0;
	}

	depth--;
	map[point.x][point.y] = piece;
	map[xs][ys] = 32;				// move on map
	pieceCoordinate[piece] = point;
	side = 1 - side;

	if (depth > 0)
	{
		int piecesArray[125];
		POINT targetpoint[125];
		if (EnumList(map, pieceCoordinate, side, piecesArray, targetpoint, count))
		{
			if (depth >= 2 && count > S_WIDTH + 2)
			{
				cur = 0;
				maxvalue = -10000;
				int value[125];
				while(cur < count)
				{
					curvalue = Search(map, pieceCoordinate, side, piecesArray[cur], targetpoint[cur], -10000, depth - 2);
					value[cur] = curvalue;
					maxvalue = curvalue > maxvalue ? curvalue : maxvalue;					
					cur++;
				}
				QuickSort(value, piecesArray, targetpoint, 0, count - 1);
				count = S_WIDTH;
			}

			cur = 0;
			maxvalue = -10000;
			while(cur < count)
			{
				curvalue = Search(map, pieceCoordinate, side, piecesArray[cur], targetpoint[cur], maxvalue, depth);
				maxvalue = curvalue > maxvalue ? curvalue : maxvalue;
				if(curvalue >= -upmax) goto _ENDSUB;
				cur++;
			}
		}
		else
			maxvalue = 9800;
	}
	else
	{
		maxvalue = Value(map, pieceCoordinate, side);
	}
_ENDSUB:
	pieceCoordinate[piece].x = xs;
	pieceCoordinate[piece].y = ys;
	map[xs][ys] = piece;
	if (ate != 32)
	{
		pieceCoordinate[ate] = point;
		map[point.x][point.y] = ate;
	}
	else
		map[point.x][point.y] = 32;
	side = 1 - side;
	return -maxvalue;
}

BOOL Think(int map[11][12],POINT pieceCoordinate[32],int &side,int &resultPiece, POINT &resultpoint)
{
	int piecesArray[125];
	POINT targetpoint[125];
	int count, maxvalue, cur, curvalue;
	
	if (EnumList(map, pieceCoordinate, side, piecesArray, targetpoint, count))
	{
		if (S_DEPTH >= 2 && count > S_WIDTH + 2)
		{
			int value[125];
			cur = 0;
			maxvalue = -10000;
			while(cur < count)
			{
				curvalue = Search(map, pieceCoordinate, side, piecesArray[cur], targetpoint[cur], -10000, S_DEPTH - 2);
				value[cur] = curvalue;
				maxvalue = curvalue > maxvalue ? curvalue : maxvalue;
				cur++;
			}
			QuickSort(value, piecesArray, targetpoint, 0, count - 1);
			count = S_WIDTH;
		}

		cur = 0;
		maxvalue = -10000;
		while(cur < count)
		{
			curvalue = Search(map, pieceCoordinate, side, piecesArray[cur], targetpoint[cur], maxvalue, S_DEPTH);
			if (curvalue > maxvalue)
			{
				maxvalue = curvalue;
				resultPiece = piecesArray[cur];
				resultpoint = targetpoint[cur];
			}
			cur++;
		}
		return TRUE;
	}
	else
	{
		if (count > 0)
		{
			resultPiece = piecesArray[count - 1];
			resultpoint = targetpoint[count - 1];
			return TRUE;
		}
		else
		{
			resultPiece = 32;
			resultpoint.x = 0;
			return FALSE;
		}
	}
}