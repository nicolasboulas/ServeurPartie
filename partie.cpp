#include "pch.h"
#include "partie.h"
#include <string>
#include <stdio.h>
#include <iostream>
#include "stdlib.h"
#include <time.h>

using namespace std;


void Partie::init() {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			this->grille[i][j] = 0;
		}
	}
}

bool Partie::EstCordonneValide(int x, int y)
{
	if (x < 3 && x >= 0 && y < 3 && y >= 0) {
		if (this->grille[x][y] != 0)
			cout << "Case deja pleine !  " << endl;
		else
			return true;
	}
	else
		cout << "Coordonees Invalide ! " << endl;

	return false;
}

bool Partie::cocherX(int x, int y) {
	if (x < 3 && y < 3 && x >= 0 && y >= 0) {
		this->grille[x][y] = 1;
	}
	return EstCordonneValide(x, y);
}

bool Partie::cocherO(int x, int y) {
	if (x < 3 && y < 3 && x >= 0 && y >= 0) {
		this->grille[x][y] = 2;
	}
	return EstCordonneValide(x, y);
}

bool Partie::EstPartieGagne()
{
	if (this->grille[0][0] == this->grille[0][1] && this->grille[0][0] == this->grille[0][2] && this->grille[0][0] != 0)		//Ligne 1 
		return  true;
	else if (this->grille[1][0] == this->grille[1][1] && this->grille[1][0] == this->grille[1][2] && this->grille[1][0] != 0) //Ligne 2 
		return  true;
	else if (this->grille[2][0] == this->grille[2][1] && this->grille[2][0] == this->grille[2][2] && this->grille[2][0] != 0) //Ligne 3 
		return  true;
	else if (this->grille[0][0] == this->grille[1][0] && this->grille[0][0] == this->grille[2][0] && this->grille[0][0] != 0) // colonne 1 
		return  true;
	else if (this->grille[0][1] == this->grille[1][1] && this->grille[0][1] == this->grille[2][1] && this->grille[0][1] != 0)  // colonne 2
		return  true;
	else if (this->grille[0][2] == this->grille[1][2] && this->grille[0][2] == this->grille[2][2] && this->grille[0][2] != 0)  // colonne 3 
		return  true;
	else if (this->grille[0][0] == this->grille[1][1] && this->grille[0][0] == this->grille[2][2] && this->grille[0][0] != 0) // Diagonale 1 
		return  true;
	else if (this->grille[0][2] == this->grille[1][1] && this->grille[0][2] == this->grille[2][0] && this->grille[0][2] != 0) // Diagonale 2 
		return  true;

	return false;
}

bool Partie::testO(int x, int y) {
	if (this->cocherO(x, y)) {
		this->EstPartieGagne();
		cout << "Le joueur O a gagné";
		return true;
	}
	else {
		return false;
	}
}

bool Partie::testX(int x, int y) {
	if (this->cocherX(x, y)) {
		cout << "Le joueur X a gagné";
		this->EstPartieGagne();
		return true;
	}
	else {
		return false;
	}
}

int Partie::tirage() {
	srand(time(NULL));
	int nb = rand()%2;
	return nb;
}


