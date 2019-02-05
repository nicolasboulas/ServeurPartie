#pragma once
#include "pch.h"
#include <string>

using namespace std;

class Partie {


public:

	int grille[3][3];

	void init();
	bool EstCordonneValide(int x, int y);
	bool cocherX(int x, int y);
	bool cocherO(int x, int y);
	bool testO(int x, int y);
	bool testX(int x, int y);
	int tirage();
	bool EstPartieGagne();
};
