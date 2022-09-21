#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define LIMIT 50000		//limit poctu stavov

//struktura pre stav 
typedef struct stav {
	int** stav_pole;								//dvojrozmerne pole
	int m, n, im, jm;								//m,n - rozmery; im,jm - poloha medzery v stave
}STAV;

//sturktura uzlu
typedef struct uzol {
	STAV* stav;
	struct uzol* predchodca;
	int predosli_operator;
} UZOL;

//struktura pre front
typedef struct front {
	int zaciatok, koniec, velkost, max_velkost;
	UZOL** array;
}FRONT;

//funkcia na vytvorenie frontu
FRONT* vytvorFront(int max_velkost) {
	FRONT* front = (FRONT*)malloc(sizeof(FRONT));
	front->max_velkost = max_velkost;
	front->zaciatok = front->velkost = 0;
	front->koniec = max_velkost - 1;
	front->array = (UZOL**)malloc(sizeof(UZOL*) * max_velkost);

	return front;
}

//funkcia na pridanie do fronty
void pridajDoFrontu(FRONT* front, UZOL* uzol) {
	if (front->max_velkost == front->velkost) return;
	front->koniec = (front->koniec + 1) % front->max_velkost;
	front->array[front->koniec] = uzol;
	front->velkost++;
}

//funkcia na odobranie z frontu
UZOL* odoberZFrontu(FRONT* front) {
	if (front->velkost == 0) return NULL;
	UZOL* uzol = (UZOL*)malloc(sizeof(UZOL));
	uzol = front->array[front->zaciatok];
	front->zaciatok = (front->zaciatok + 1) % front->max_velkost;
	front->velkost--;

	return uzol;
}

//funkcia vytvori stavove pole z textoveho suboru
int getStavFromFile(char* nazov_suboru, int*** stav_pole, int* m, int* n) {

	char c;
	int i, j, space = 1, newline = 1;
	FILE* fr;

	//otvorenie suboru
	if ((fr = fopen(nazov_suboru, "r")) == NULL) {							
		printf("Subor \"%s\" sa nepodarilo otvorit.\n", nazov_suboru);
		return 0;
	}

	//zistenie rozmerov stavoveho pola
	while ((c = getc(fr)) != EOF)
	{
		if (c == '\n') newline++;
		if (newline == 1 && c == ' ') space++;
	}

	*m = newline;
	*n = space;

	rewind(fr);

	*stav_pole = (int**)malloc(newline * sizeof(int*));

	//zapisovanie cisel zo suboru do stavoveho pola
	for (i = 0; i < newline; i++) {

		(*stav_pole)[i] = (int*)malloc(space * sizeof(int));

		for (j = 0; j < space; j++)
			fscanf(fr, "%d", &(*stav_pole)[i][j]);

	}


	//vypisanie nacitanych stavov
	printf("Stav zo suboru \"%s\" :\n", nazov_suboru);

	for (i = 0; i < newline; i++) {
		for (j = 0; j < space; j++)
			printf("%d ", (*stav_pole)[i][j]);
		printf("\n");
	}

	printf("\n");

	return 1;
}

//funkcia vytvori strukturu stav zo stavoveho pola
STAV* vytvorStav(int** vstup, int m, int n) {
	int i,j;
	STAV* stav = (STAV*)malloc(sizeof(STAV));
	stav->stav_pole = (int**)malloc(m*sizeof(int));			//alokovanie pamete
	for (i = 0; i < m; i++)
		stav->stav_pole[i] = (int*)malloc(n*sizeof(int));
	stav->m = m;
	stav->n = n;

	//kopirovanie stavoveho pola
	for(i = 0; i < m;i++)
		for (j = 0; j < n; j++) {
			stav->stav_pole[i][j] = vstup[i][j];
			if (vstup[i][j] == 0) {
				stav->im = i;
				stav->jm = j;
			}
		}

	return stav;
}

//funkcia vytvori novy stav z poskytnuteho a posunie medzeru v zadanom smere
STAV* posunStav(STAV* stav, int smer) {
	int i, j;
	STAV* newStav = vytvorStav(stav->stav_pole, stav->m, stav->n);

	//kopirovanie stavoveho pola
	for (i = 0; i < stav->m; i++)
		for (j = 0; j < stav->n; j++) {
			newStav->stav_pole[i][j] = stav->stav_pole[i][j];
		}

	//rozhodovanie o smere
	switch (smer) {
	case 1:	//UP
		newStav->im--;																		//posunutie medzery UP
		newStav->stav_pole[stav->im][stav->jm] = newStav->stav_pole[newStav->im][stav->jm]; //presunutie cisla na miesto medzery
		newStav->stav_pole[newStav->im][stav->jm] = 0;										//zapisanie novej medzery
		break;
	case 2:	//RIGHT
		newStav->jm++;																		//posunutie medzery RIGHT
		newStav->stav_pole[stav->im][stav->jm] = newStav->stav_pole[stav->im][newStav->jm]; //presunutie cisla na miesto medzery
		newStav->stav_pole[stav->im][newStav->jm] = 0;										//zapisanie novej medzery
		break;
	case 3:	//DOWN
		newStav->im++;																		//posunutie medzery DOWN
		newStav->stav_pole[stav->im][stav->jm] = newStav->stav_pole[newStav->im][stav->jm]; //presunutie cisla na miesto medzery
		newStav->stav_pole[newStav->im][stav->jm] = 0;										//zapisanie novej medzery
		break;
	case 4:	//LEFT
		newStav->jm--;																		//posunutie medzery LEFT
		newStav->stav_pole[stav->im][stav->jm] = newStav->stav_pole[stav->im][newStav->jm]; //presunutie cisla na miesto medzery
		newStav->stav_pole[stav->im][newStav->jm] = 0;										//zapisanie novej medzery
		break;
	default:
		printf("\nNeplatny smer\n");
		break;
	}

	return newStav;
}

//funkcia vrati 1 ak su stavy rovnake
int rovnakeStavy(STAV* stav1, STAV* stav2) {
	int i, j;

	for (i = 0; i < stav1->m; i++)
		for (j = 0; j < stav1->n; j++)
			if (stav1->stav_pole[i][j] != stav2->stav_pole[i][j])
				return 0;

	return 1;
}

//funkcia vytvara novy uzol pre dany smer
UZOL* createNewUzol(UZOL* uzol, int smer) {
	UZOL* newUzol = (UZOL*)malloc(sizeof(UZOL));
	
	newUzol->predchodca = uzol;
	newUzol->predosli_operator = smer;
	newUzol->stav = posunStav(uzol->stav, smer);
	
	return newUzol;
}

//funkcia na rozvinutie uzla
//UP = 1, RIGHT = 2, DOWN = 3, LEFT = 4
void rozsirUzol(UZOL* uzol, FRONT* front) {

	if(uzol->predosli_operator != 3)								//UP sa vykona ak predosli operator nebol UP
		if (uzol->stav->im != 0)									//UP sa vykona ak medzera nie je v najvrchnejsom rade (ak sa moze presunut hore)
			pridajDoFrontu(front, createNewUzol(uzol, 1));			//vygenerovanie noveho uzla s novym stavom

	if (uzol->predosli_operator != 4)								//RIGHT sa vykona ak predosli operator nebol RIGHT
		if (uzol->stav->jm != (uzol->stav->n - 1))					//RIGHT sa vykona ak medzera nie je v najpravejsom stlpci (ak sa moze presunut doprava)
			pridajDoFrontu(front, createNewUzol(uzol, 2));			//vygenerovanie noveho uzla s novym stavom

	if (uzol->predosli_operator != 1)								//DOWN sa vykona ak predosli operator nebol DOWN
		if (uzol->stav->im != (uzol->stav->m -1))					//DOWN sa vykona ak medzera nie je v najspodnejsom rade (ak sa moze presunut dole)
			pridajDoFrontu(front, createNewUzol(uzol, 3));			//vygenerovanie noveho uzla s novym stavom

	if (uzol->predosli_operator != 2)								//LEFT sa vykona ak predosli operator nebol LEFT
		if (uzol->stav->jm != 0)									//LEFT sa vykona ak medzera nie je v najlavejsom stlpci (ak sa moze presunut dolava)
			pridajDoFrontu(front, createNewUzol(uzol, 4));			//vygenerovanie noveho uzla s novym stavom

}

//funkcia pre rekurzivne vypisanie stavov
void vypisRekurzivnePoZaciatok(UZOL* uzol) {
	int i, j;

	if (uzol == NULL) return;

	vypisRekurzivnePoZaciatok(uzol->predchodca);

	for (i = 0; i < uzol->stav->m; i++) {
		for (j = 0; j < uzol->stav->n; j++)
			printf("%d ", uzol->stav->stav_pole[i][j]);
		printf("\n");
	}

	printf("\n");

}


//funkcia pre vypis krokov, ich pocet, pocet uzlov a cas riesenia
void vypisRiesenie(UZOL* uzolZac, UZOL* uzolCiel, int pocet_stavov, int cas) {
	int i, j, pocet = 1;

	//vypisanie prvej polovice cesty
	vypisRekurzivnePoZaciatok(uzolZac);

	//pocitanie poctu krokov
	while (uzolZac->predchodca != NULL) {
		pocet++;
		uzolZac = uzolZac->predchodca;
	}

	uzolCiel = uzolCiel->predchodca;

	//pocitanie a vypis druhej polovice cesty
	while (uzolCiel != NULL) {
		pocet++;

		for (i = 0; i < uzolCiel->stav->m; i++) {
			for (j = 0; j < uzolCiel->stav->n; j++)
				printf("%d ", uzolCiel->stav->stav_pole[i][j]);
			printf("\n");
		}

		printf("\n");

		uzolCiel = uzolCiel->predchodca;
	}

	printf("\nPocet krokov: %d\nPocet spracovanych uzlov: %d\nCas: %d ms", --pocet, 2*pocet_stavov, cas);
}

//hlavna funkcia algoritmu; hlada riesenie hlavolamu
void osem_hlavolam_obojsmerne_bfs(STAV* zaciatok, STAV* ciel) {
	int i, cas, c = 0, pocet = 0;
	clock_t t;	//premenna pre vypocet casu riesenia
	
	STAV* temp = (STAV*)malloc(sizeof(STAV));
	FRONT* frontZac = (FRONT*)malloc(sizeof(FRONT));
	FRONT* frontCiel = (FRONT*)malloc(sizeof(FRONT));
	UZOL* uzolZac = (UZOL*)malloc(sizeof(UZOL));
	UZOL* uzolCiel = (UZOL*)malloc(sizeof(UZOL));
	UZOL* uzolNovy = (UZOL*)malloc(sizeof(UZOL));

	t = clock();

	//vytvorenie frontov
	frontZac = vytvorFront(LIMIT); 
	frontCiel = vytvorFront(LIMIT);

	//vytvorenie uzlov
	uzolZac->stav = zaciatok;
	uzolZac->predchodca = NULL;
	uzolZac->predosli_operator = 0;

	uzolCiel->stav = ciel;
	uzolCiel->predchodca = NULL;
	uzolCiel->predosli_operator = 0;

	//pridanie uzlov do frontov
	pridajDoFrontu(frontZac, uzolZac);
	pridajDoFrontu(frontCiel, uzolCiel);

	//kontrola ci nie su rovnake
	if (rovnakeStavy(uzolZac->stav, uzolCiel->stav)) {
		printf("Naslo sa riesenie.\n");
		printf("Zaciatocny stav a cielovy stav su rovnake.\n");
		return;
	}

	//hlavny cyklus
	while (1) {
		c++; //counter pre pocet uzlov

		//vybrat uzly z fronty
		uzolZac = odoberZFrontu(frontZac);
		uzolCiel = odoberZFrontu(frontCiel);

		//najst nove uzly pre uzolZac a pridat ich do frontu
		rozsirUzol(uzolZac, frontZac);

		//najst nove uzly pre uzolCiel a pridat ich do frontu
		rozsirUzol(uzolCiel, frontCiel);

		//porovnat stavy uzlov

		//rozvijany uzol s rozvitymi
		for(i = frontCiel->zaciatok; i <= frontCiel->koniec; i++)
			if (rovnakeStavy(uzolZac->stav, frontCiel->array[i]->stav)) {
				printf("Naslo sa riesenie:\n\n");
				t = clock() - t;
				cas = ((int)t);
				vypisRiesenie(uzolZac, frontCiel->array[i], c, cas);
				return;
			}

		//rozvijany uzol s rozvitymi
		for (i = frontZac->zaciatok; i <= frontZac->koniec; i++)
			if (rovnakeStavy(uzolCiel->stav, frontZac->array[i]->stav)) {
				printf("Naslo sa riesenie:\n\n");
				t = clock() - t;
				cas = ((int)t);
				vypisRiesenie(frontZac->array[i], uzolCiel, c, cas);
				return;
			}

		//rozvijany uzol s rozvijanym
		if (rovnakeStavy(uzolZac->stav, uzolCiel->stav)) {
			printf("Naslo sa riesenie:\n\n");
			t = clock() - t;
			cas = ((int)t);
			vypisRiesenie(uzolZac, uzolCiel, c, cas);
			return;
		}

		//kontrola limitu poctu uzlov
		if (c > (LIMIT/2)) {
			printf("Nenaslo sa riesenie");
			return;
		}
	}
}

int main() {
	int m, n;
	int** zaciatok, **ciel;

	getStavFromFile("zaciatok.txt", &zaciatok, &m, &n);

	getStavFromFile("ciel.txt", &ciel, &m, &n);

	osem_hlavolam_obojsmerne_bfs(vytvorStav(zaciatok,m,n), vytvorStav(ciel,m,n));

	getchar();
	getchar();
	return 0;
}