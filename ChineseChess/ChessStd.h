#include <Windows.h>
BOOL  CanGo(int manmap[11][12],int man,const POINT &from,const POINT &to);
BOOL  IsNormal(const int & piece,const POINT &point);
void  FixManMap(int map[11][12],POINT manpoint[32],int side);