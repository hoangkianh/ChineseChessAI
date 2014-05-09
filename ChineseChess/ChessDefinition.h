#include <Windows.h>
const int MW = 48, SW = 1;	// MW : Max Width, SW : Space Width
const int BWA = MW + SW;
const int XBW = BWA * 9 , YBW = BWA * 10;

const int MAN = 0;
const int COM = 1;

const int RED = 0;
const int BLACK = 1;

//  RED
const int RED_K = 0;	// King
const int RED_A = 1;	// Advisor
const int RED_E = 2;	// Elephant
const int RED_H = 3;	// Horse
const int RED_R = 4;	// Chariot
const int RED_C = 5;	// Cannon
const int RED_S = 6;	// Soldier

//  BLACK
const int BLACK_K = 7;	// King
const int BLACK_A = 8;	// Advisor
const int BLACK_E = 9;	// Elephant
const int BLACK_H = 10;	// Horse
const int BLACK_R = 11;	// Chariot
const int BLACK_C = 12;	// Cannon
const int BLACK_S = 13;	// Soldier

const int PieceToIcon[33] = { 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 6, 6, 6
							, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 13, 13, 13, -1 };

#define PieceToType PieceToIcon

const int PieceToType7[33] = { 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 6, 6, 6
							 , 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 6, 6, 6, -1 };

int  rnd(const int& n);

const int SideOfPiece[33] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
							, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -1};

const int _defaultmap[11][12] = 
{
//	[0]	 [1] [2] [3] [4] [5] [6] [7] [8] [9] [10][11]
	{32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32},//[0]
	{32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32},//[1]
	{32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32},//[2]
	{32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32},//[3]
	{32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32},//[4]
	{32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32},//[5]
	{32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32},//[6]
	{32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32},//[7]
	{32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32},//[8]
	{32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32},//[9]
	{32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32}//[10]
};

const int FirstOfSide[2] = {0, 16};
const int LastOfSide[2] = {15, 31};
const int MAXMOVE = 1000;

struct MOVEHISTORY
{
	int count;
	int pieces[MAXMOVE];
	POINT from[MAXMOVE];
	POINT to[MAXMOVE];
	int betaken[MAXMOVE];
};