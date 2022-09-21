#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HASH 919
#define max(a,b) (((a) > (b)) ? (a):(b))

//struktura pre prvky avl stromu
typedef struct AvlPrvok {
	char meno[50];
	int vyska;
	int pocetL;
	int pocetR;
	struct AvlPrvok *L;
	struct AvlPrvok *R;
} AVLNODE;

//NEPOUZITE - struktura pre prvky bvs stromu
typedef struct BvsPrvok {
	char meno[100];
	int vyska;
	int pocetL;
	int pocetR;
	struct BvsPrvok *L;
	struct BvsPrvok *R;
} BVSNODE;

//struktura pre prvky hash tabulky - stranky
typedef struct page {
	char pageName[100];
	struct page * dalsi;
	AVLNODE *usersAVL;
	BVSNODE *usersBVS;
}PAGE;

//hash funcia pouzitim hornerovej schemy
int toHash(char *page) {
	int i, len = strlen(page);
	long long hashed = 0;
	for (i = 0; i < len; i++)
		hashed = 2 * hashed + page[i];
	i = ((int)(hashed % HASH));
	return i < 0 ? -i : i;
}

//premenna hash tabulky
PAGE **hashTab;

//NEPOUZITE - vyska bvs node, v tomto pripade rekurzivne prechadza podstromy -> nevhodne pomale riesenie
int vyskaBVS(BVSNODE *x) {
	if (x == NULL || (x->L == NULL && x->R == NULL)) return 0;
	return 1 + max(vyskaBVS(x->R), vyskaBVS(x->L));
}

//vyska avl node, max hodnota z vysok deti + 1
int vyskaAVL(AVLNODE *x) {
	if (x == NULL) return 0;
	return 1 + max(x->R != NULL ? x->R->vyska : 0, x->L != NULL ? x->L->vyska : 0);
}

//funkcia pre rotaciu dolava AVL
AVLNODE *rotaciaL(AVLNODE **x) {
	AVLNODE *y = (*x)->R;
	AVLNODE *z = y->L;

	y->L = *x;
	(*x)->R = z;

	//pre udrzanie poctu nalavo a napravu treba kopirovat a pripocitavat hodnoty
	(*x)->pocetR = y->pocetL;
	y->pocetL += (*x)->pocetL + 1;

	//aktualizovanie vysky
	(*x)->vyska = vyskaAVL(*x);
	y->vyska = vyskaAVL(y);

	return y;
}

//funkcia pre rotaciu doprava AVL, obdobne ako pri rotacii dolava
AVLNODE *rotaciaR(AVLNODE **x) {
	AVLNODE *y = (*x)->L;
	AVLNODE *z = y->R;

	y->R = (*x);
	(*x)->L = z;
	(*x)->pocetL = y->pocetR;
	y->pocetR += (*x)->pocetR + 1;

	(*x)->vyska = vyskaAVL(*x);
	y->vyska = vyskaAVL(y);

	return y;
}

//funkcia na vlozenie do prvku do AVL, rekurzivne
AVLNODE** vlozAVL(AVLNODE **x, char *meno) {
	//ak sa nasiel prazdny prvok tak vloz prvok
	if ((*x) == NULL) {
		*x = (AVLNODE*)malloc(sizeof(AVLNODE));
		strcpy((*x)->meno, meno);
		(*x)->R = NULL;
		(*x)->L = NULL;
		(*x)->pocetL = 0;
		(*x)->pocetR = 0;
		(*x)->vyska = 1;
		return *x;
	}
	else
	{
		//rozhoduje sa ci sa ide do laveho podstromu alebo do praveho, zvysenie hodnoty poctu prvkov
		if (strcmp(meno, (*x)->meno) < 0)
		{
			(*x)->L = vlozAVL(&(*x)->L, meno);
			(*x)->pocetL++;
		}
		else
		{
			(*x)->R = vlozAVL(&(*x)->R, meno);
			(*x)->pocetR++;
		}

		//aktualizovanie vysky aktualneho prvku
		(*x)->vyska = vyskaAVL(*x);

		//vypocet balance faktoru aktualneho prvku
		int bf = bfAVL(*x);

		//urcenie potrebnych rotacii
		if (bf < -1 && strcmp(meno, (*x)->L->meno) < 0)
			return rotaciaR(&*x);

		if (bf > 1 && strcmp(meno, (*x)->R->meno) > 0)
			return rotaciaL(&*x);

		if (bf < -1 && strcmp(meno, (*x)->L->meno) > 0) {
			(*x)->L = rotaciaL(&(*x)->L);
			return rotaciaR(&*x);
		}

		if (bf > 1 && strcmp(meno, (*x)->R->meno) < 0) {
			(*x)->R = rotaciaR(&(*x)->R);
			return rotaciaL(&*x);
		}

		return *x;
	}
}

//vypocet balance faktoru prvku
int bfAVL(AVLNODE *x) {
	if (x->L == NULL && x->R == NULL)return 0;
	if (x->L == NULL)return x->R->vyska;
	if (x->R == NULL)return -(x->L->vyska);
	return (x->R->vyska) - (x->L->vyska);
}

//NEPOUZITE - funkcia pre vlozenie prvku bvs
void vlozBVS(BVSNODE **x, char *meno) {
	if ((*x) == NULL) {
		*x = (BVSNODE*)malloc(sizeof(BVSNODE));
		strcpy((*x)->meno, meno);
		(*x)->R = NULL;
		(*x)->L = NULL;
		(*x)->pocetL = 0;
		(*x)->pocetR = 0;
		(*x)->vyska = 0;
	}
	else
	{
		if (strcmp(meno, (*x)->meno) < 0)
		{
			vlozBVS(&(*x)->L, meno);
			(*x)->pocetL++;
		}
		else
		{
			vlozBVS(&(*x)->R, meno);
			(*x)->pocetR++;
		}
		(*x)->vyska = vyskaBVS(*x);
	}
}

//NEPOUZITE - funkcia pre vymazanie prvku bvs
void vymazBVS(BVSNODE **x, char *meno) {
	BVSNODE *y, *z;
	int i;
	if (*x == NULL) return;
	if (strcmp((*x)->meno, meno) == 0)
	{
		if ((*x)->L == NULL)
		{
			y = (*x)->R;
			free(*x);
			*x = y;
		}
		else
			if ((*x)->R == NULL)
			{
				y = (*x)->L;
				free(*x);
				*x = y;
			}
			else
			{
				y = (*x)->R;
				while (y->L != NULL)
					y = y->L;
				for (i = 0; i<100; i++)
					(*x)->meno[i] = '\0';
				strcpy((*x)->meno, y->meno);
				z = y->R;
				free(y);
				y = z;
			}

	}
	else
		if (strcmp(meno, (*x)->meno) < 0)
		{
			vymazBVS(&(*x)->L, meno);
			(*x)->pocetL--;
		}
		else
		{
			vymazBVS(&(*x)->R, meno);
			(*x)->pocetR--;
		}
}

//funkcia pre vymazanie prvku avl, rekurzivne
AVLNODE **vymazAVL(AVLNODE **x, char *meno) {
	AVLNODE *y, *z;
	int i;
	if (*x == NULL) return *x;
	//ak sa nasiel prvok ktory sa ma vymazat
	if (strcmp((*x)->meno, meno) == 0)
	{
		//ak je lave dieta NULL tak prvok na vymazanie nastav na jeho prave dieta
		if ((*x)->L == NULL)
		{
			//y = (*x)->R;
			//free(*x);
			*x = (*x)->R;
		}
		else
			//ak je prave dieta NULL tak prvok na vymazanie nastav na jeho lave dieta
			if ((*x)->R == NULL)
			{
				//y = (*x)->L;
				//free(*x);
				*x = (*x)->L;
			}
			else
			{
				//ak ma obe deti tak sa hlada nasledovnik a jeho udaje sa skopiruju do vymazavaneho prvku a nasledovnik sa vymaze
				y = (*x)->R;
				//pocet prvkov sa zmensi o 1 pri vymazani
				(*x)->pocetR--;
				while (y->L != NULL)
					y = y->L;
				for (i = 0; i<50; i++)
					(*x)->meno[i] = '\0';
				strcpy((*x)->meno, y->meno);
				(*x)->R = vymazAVL(&(*x)->R, y->meno);
			}
	}
	else
		//rozhodovanie smeru a aktualizovanie poctu prvkov
		if (strcmp(meno, (*x)->meno) < 0)
		{
			(*x)->L = vymazAVL(&(*x)->L, meno);
			(*x)->pocetL--;
		}
		else
		{
			(*x)->R = vymazAVL(&(*x)->R, meno);
			(*x)->pocetR--;
		}

	if (*x == NULL) return *x;

	//aktualizovanie vysky
	(*x)->vyska = vyskaAVL(*x);

	//vypocet balance faktoru
	int bf = bfAVL(*x);

	//potrebne rotacie
	if (bf < -1 && bfAVL((*x)->L) <= 0)
		return rotaciaR(&*x);

	if (bf > 1 && bfAVL((*x)->R) >= 0)
		return rotaciaL(&*x);

	if (bf < -1 && bfAVL((*x)->L) > 0) {
		(*x)->L = rotaciaL(&(*x)->L);
		return rotaciaR(&*x);
	}

	if (bf > 1 && bfAVL((*x)->R) < 0) {
		(*x)->R = rotaciaR(&(*x)->R);
		return rotaciaL(&*x);
	}

	return *x;
}

//NEPOUZITE - vracia meno k-teho v bvs
char* kthBVS(BVSNODE **x, int k) {
	if (k > ((*x)->pocetL + (*x)->pocetR + 1)) return NULL;
	if (k <= (*x)->pocetL)
		return kthBVS(&(*x)->L, k);
	else
		if (k == ((*x)->pocetL + 1))
			return (*x)->meno;
		else
			return kthBVS(&(*x)->R, (k - (*x)->pocetL) - 1);
}

//vracia meno k-teho v avl pouzitim udajov o pocte prvok v lavom podstrome a pravom podstrome pre urychlenie, rekurzivne
char* kthAVL(AVLNODE **x, int k) {
	if (k > ((*x)->pocetL + (*x)->pocetR + 1)) return NULL;
	if (k <= (*x)->pocetL)
		return kthAVL(&(*x)->L, k);
	else
		if (k == ((*x)->pocetL + 1))
			return (*x)->meno;
		else
			return kthAVL(&(*x)->R, (k - (*x)->pocetL) - 1);
}

//NEPOUZITE - vypis bvs inorder
void inorderBVS(BVSNODE *x) {
	if (x != NULL) {
		inorderBVS(x->L);
		printf(" %s", x->meno);
		inorderBVS(x->R);
	}
}

//NEPOUZITE - vypis avl inorder
void inorderAVL(AVLNODE *x) {
	if (x != NULL) {
		inorderAVL(x->L);
		printf(" %d-%s", x->vyska, x->meno);
		inorderAVL(x->R);
	}
}

//NEPOUZITE - kontrolna funkcia na vypis hash tabulky
void printHashTab() {
	int i;
	PAGE *page;
	for (i = 0; i < HASH; i++)
		if (*(hashTab + i) != NULL) {
			page = *(hashTab + i);
			while (page != NULL) {
				printf("%d %s ->", i, page->pageName);
				inorderAVL(page->usersAVL);
				page = page->dalsi;
			}
			printf("\n");
		}
}

//inicializacia hash tabulky
void init()
{
	int i;
	hashTab = (PAGE**)malloc(HASH * sizeof(PAGE*));
	// "vynulovanie"
	for (i = 0; i < HASH; i++)
		*(hashTab + i) = NULL;
}

void like(char *page, char *user)
{
	PAGE *akt = NULL;
	PAGE *prev = NULL;

	//vypocet hash hodnoty
	int hash = toHash(page);

	//vyhladat stranku v hash tab a pridat usera
	//ak nie je, tak pridat do hash tab
	if (*(hashTab + hash) != NULL) {
		akt = *(hashTab + hash);
		while (akt != NULL) {
			if (strcmp(akt->pageName, page) == 0) {
				//add user to avl list
				akt->usersAVL = vlozAVL(&(akt->usersAVL), user);
				return;
			}
			prev = akt;
			akt = akt->dalsi;
		}
		//pridat na koniec 
		prev->dalsi = (PAGE*)malloc(sizeof(PAGE));
		akt = prev->dalsi;
		strcpy(akt->pageName, page);
		akt->dalsi = NULL;
		akt->usersAVL = NULL;
		//pridat do stromu usera
		akt->usersAVL = vlozAVL(&(akt->usersAVL), user);
	}
	else
	{
		*(hashTab + hash) = (PAGE*)malloc(sizeof(PAGE));
		akt = *(hashTab + hash);
		strcpy(akt->pageName, page);
		akt->dalsi = NULL;
		akt->usersAVL = NULL;
		//pridat do stromu usera
		akt->usersAVL = vlozAVL(&(akt->usersAVL), user);

	}
}

void unlike(char *page, char *user)
{
	//vyhladat stranku v hash tab a odobrat usera
	int hash = toHash(page);
	PAGE *akt;
	akt = *(hashTab + hash);
	while (akt != NULL) {
		if (strcmp(akt->pageName, page) == 0) {
			//delete user from avl list
			akt->usersAVL = vymazAVL(&(akt->usersAVL), user);
			return;
		}
		akt = akt->dalsi;
	}
}

char *getuser(char *page, int k)
{
	//vyhladat stranku v hash tab a najst v strome k-teho usera
	int hash = toHash(page);
	PAGE *akt;
	int len;
	akt = *(hashTab + hash);
	while (akt != NULL) {
		if (strcmp(akt->pageName, page) == 0) {
			//find k-th user in avl list and return him
			return kthAVL(&(akt->usersAVL), k);
		}
		akt = akt->dalsi;
	}
	return NULL;
}

// Vlastna funkcia main() je pre vase osobne testovanie. Dolezite: pri testovacich scenaroch sa nebude spustat!
int main()
{
	init();

	like("Star Trek", "Jana");
	like("Star Wars", "Marek");
	//printHashTab();
	like("Star Trek", "Filip");
	like("Star Trek", "Adam");
	like("Star Trek", "Andrej");
	like("Star Trek", "AAAdam");
	like("Star Trek", "Xavier");
	like("Star Trek", "Savier");

	printHashTab();
	unlike("Star Trek","Filip");
	printHashTab();
	printf("%s\n", getuser("Star Trek", 1)); // Filip
	printf("%s\n", getuser("Star Trek", 2)); // Jana
	unlike("Star Trek", "Filip");
	printf("%s\n", getuser("Star Trek", 1)); // Jana
	getchar();
	return 0;
}