#include "pch.h"

#include <windows.h>  
void SetConsoleColor(int col)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), col);
}