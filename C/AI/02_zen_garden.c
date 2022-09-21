#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#define	POCET_JEDINCOV	100				//pocet jedincov v jednej generacii
#define POCET_GENERACII 10000			//maximalny pocet generacii pre hladanie riesenia
#define POCET_RODICOV	90				//pocet rodicov vybranych z generacie
#define FAKTOR_KRIZENIA 30				//sanca ze sa vykona simpleKrizenie() je 1 ku FAKTOR_KRIZENIA, inac sa vykona permutationKrizenie()
#define FAKTOR_MUTACIE	30				//pri valueMutacia() urcuje sancu pre zmutovanie casti chromozomu
#define PRAVDEPODOBNOST_BEST	50		//pravdepodobnost vyberu najlepsieho jedinca pri vyberani rodicov pomocou rulety
#define PRAVDEPODOBNOST_WORST	20		//pravdepodobnost vyberu najhorsieho jedinca pri vyberani rodicou pomocou rulety
#define SELEKCIA		0				// 0 - selekcia turnajom
										// 1 - selekcia ohodnotenim

typedef struct zahrada {
	int x, y;									//rozmeri zahrady
	int pocet_kamenov;							//pocet kamenov v zahrade, taktiez velkost pola kamene
	int *kamene;								//pole so suradnicami kamenov
}ZAHRADA;

typedef struct jedinec {
	int vstupSize, smerSize, fitness;			//velkost pola vstupGenes, velkost pola smerGenes a fitness hodnota daneho jedinca
	int *vstupGenes;							//pole s cislami vyjadrujucimi suradnicu vstupu do zahrady
	int *smerGenes;								//pole s cislami vyjadrujucimi smer odbocenia pri rozhodovani
}JEDINEC;

//funkcia pre spravne fungovanie heapsortu, vymeni dvoch jedincov v generacii
void swap(JEDINEC **generacia, int x, int y) {
	JEDINEC *temp = generacia[x];
	generacia[x] = generacia[y];
	generacia[y] = temp;
}

//funkcia pre udrziavanie maxheap
void heapify(JEDINEC **generacia, int n, int i){
	int largest = i, left = (2*i)+1, right = (2*i)+2;

	if (left < n && generacia[left]->fitness > generacia[largest]->fitness)
		largest = left;

	if (right < n && generacia[right]->fitness > generacia[largest]->fitness)
		largest = right;

	if (largest != i){
		swap(generacia, i, largest);
		heapify(generacia, n, largest);
	}
}

//funkcia na usporiadanie generacii heapsortom
void heapSort(JEDINEC **generacia, int n){
	int i;
	for (i = (n/2) - 1; i >= 0; i--)
		heapify(generacia, n, i);

	for (i = n - 1; i >= 0; i--){
		swap(generacia, 0, i);
		heapify(generacia, i, 0);
	}
}

//funckia na vygenerovanie noveho jedinca s nahodnymi vstupmi do zahrady
JEDINEC *nahodnyJedinec(ZAHRADA *zahrada) {
	int i;
	JEDINEC *nahodny = (JEDINEC*)malloc(sizeof(JEDINEC));
	nahodny->fitness = 0;
	nahodny->smerSize = zahrada->pocet_kamenov;
	nahodny->vstupSize = (zahrada->x + zahrada->y);
	nahodny->vstupGenes = (int*)malloc(nahodny->vstupSize * sizeof(int));
	nahodny->smerGenes = (int*)malloc(zahrada->pocet_kamenov * sizeof(int));

	//nahodne generovanie vstupov do zahrady, cislo je od -(x+y) do (x+y), kde x a y su rozmeri zahrady
	for (i = 0; i < nahodny->vstupSize; i++)
		nahodny->vstupGenes[i] = (rand() % 2) ? (rand() % (zahrada->x + zahrada->y)) : -(rand() % (zahrada->x + zahrada->y));

	for (i = 0; i < zahrada->pocet_kamenov; i++)
		nahodny->smerGenes[i] = (rand() % 2);

	return nahodny;
}

//vytvorenie prvotnej generacie s nahodnimi jedincami
JEDINEC **vytvorPrvuGeneraciu(ZAHRADA *zahrada) {
	int i;
	JEDINEC **generacia = (JEDINEC**)malloc(POCET_JEDINCOV * sizeof(JEDINEC*));
	for (i = 0; i < POCET_JEDINCOV; i++)
		generacia[i] = nahodnyJedinec(zahrada);

	return generacia;
}

//pomocna funkcia na vypisanie generacie pri debugovani
void vypisGeneraciu(JEDINEC **generacia) {
	int i, j;
	int vstupSize = generacia[0]->vstupSize;
	int smerSize = generacia[0]->smerSize;
	for (i = 0; i < POCET_JEDINCOV; i++) {
		printf("Jedinec %d\n",i+1);
		for (j = 0; j < vstupSize; j++)
			printf("%d ", generacia[i]->vstupGenes[j]);
		printf("\n");
		for (j = 0; j < smerSize; j++)
			printf("%d ", generacia[i]->smerGenes[j]);
		printf("\n");
	}
}

//funkcia pre nacitanie zahrady z textoveho suboru
ZAHRADA *nacitajZahradu(char *menoSuboru) {
	ZAHRADA *zahrada = (ZAHRADA*)malloc(sizeof(ZAHRADA));
	FILE *subor;
	char c;
	int i, pocet_kamenov = 0;
	if ((subor = fopen(menoSuboru, "r")) == NULL) {
		printf("Subor sa nepodarilo otvorit!\n");
		return NULL;
	}

	/*
	FORMAT VSTUPU:
	- v prvom riadku su cisla x a y oddelene medzerou, kde x a y su rozmeri zahrady
	- v nasledujucich riadkoch su suradnice kamenov, ak nejake na zahrade su

	x y
	x1 y1
	x2 y2
	.
	.
	.
	
	*/

	while ((c = getc(subor)) != EOF)
		if (c == '\n') pocet_kamenov++;

	zahrada->pocet_kamenov = pocet_kamenov;

	zahrada->kamene = (int*)malloc(2 * pocet_kamenov * sizeof(int));

	rewind(subor);

	fscanf(subor, "%d %d", &(zahrada->x), &(zahrada->y));

	for (i = 0; i < pocet_kamenov; i++)
		fscanf(subor, "%d %d", (zahrada->kamene) + i*2, (zahrada->kamene) + i*2 + 1);

	return zahrada;
}

//funckia na uvolnenie pameti alokovanych docasnych zahradach
void freeZahradu(int **tempZahrada, ZAHRADA *zahrada) {
	int i;

	for (i = 0; i < zahrada->y; i++)
		free(tempZahrada[i]);

	free(tempZahrada);
}

//funkcia pre vypocet fitness hodnoty
//vracia pocet pohrabanych policok pre daneho jedinca
int fitness(JEDINEC *jedinec, ZAHRADA *zahrada) {
	int i, j, fit_hodnota = 0;
	int x = 0, y = 0;
	int zvysujem, smer;
	int **tempZahrada = (int **)malloc(zahrada->y * sizeof(int*));

	//vytvorenie zaciatocnej zahrady
	for (i = 0; i < zahrada->y; i++) 
		tempZahrada[i] = (int *)calloc(zahrada->x, sizeof(int));

	//vlozenie kamenov
	for (i = 0; i < zahrada->pocet_kamenov; i++)
		tempZahrada[zahrada->kamene[i*2+1]][zahrada->kamene[i*2]] = -1;

	//pomocne vypisanie zahrady
	/* 
	for (i = 0; i < zahrada->y; i++) {
		for (j = 0; j < zahrada->x; j++)
			printf("%d ", tempZahrada[i][j]);
		printf("\n");
	}
	*/

	//prechadzanie genmi(vstupmi) jedinca
	for (i = 0; i < jedinec->vstupSize; i++) {
		x = 0;
		y = 0;

		//premenna smer->	1 - hore
		//					2 - doprava
		//					3 - dole
		//					4 - dolava

		if (jedinec->vstupGenes[i] < zahrada->x && jedinec->vstupGenes[i] > (zahrada->x)*(-1)) {	//ak je gen x-ova suradnica (cislo medzi -x a x, kde x je x-ovy rozmer zahrady)
			if (jedinec->vstupGenes[i] < 0) {														//ak je gen zaporne cislo tak idem znizovat y-ovu suradnicu => y-ova suradnica bude maximalna
				smer = 1;	//ide sa hore
				y = zahrada->y - 1;
			}
			else {
				smer = 3;	//ide sa dole
				y = 0;
			}

			x = jedinec->vstupGenes[i] < 0 ? (-1)*(jedinec->vstupGenes[i]) : jedinec->vstupGenes[i];

			if (tempZahrada[y][x] != 0) {
				freeZahradu(tempZahrada, zahrada);
				return fit_hodnota;
			}
		}
		else {																						//gen je teda y-ova suradnica
			if (jedinec->vstupGenes[i] < 0) {														//ak je gen zaporne cislo tak idem znizovat y-ovu suradnicu => y-ova suradnica bude maximalna
				smer = 4;	//ide sa dolava
				x = zahrada->x - 1;
			}
			else {
				smer = 2;	//ide sa doprava
				x = 0;
			}

			y = jedinec->vstupGenes[i] < 0 ? ((-1)*(jedinec->vstupGenes[i])) - zahrada->x : jedinec->vstupGenes[i] - zahrada->x;

			if (tempZahrada[y][x] != 0) {
				freeZahradu(tempZahrada, zahrada);
				return fit_hodnota;		//ak sa neda vstupit do zahrady tak koniec
			}
		}

		//cyklus pokial sa nedostane ku okraju
		while (1) {
			if (tempZahrada[y][x] == 0) {	//hrabanie
				tempZahrada[y][x] = 1;
				fit_hodnota++;
			}

			if (smer == 1) {
				if ((y - 1) < 0) break;						//ak sme na konci hrabania tak sa ide na dalsie hrabanie

				if (tempZahrada[y - 1][x] == 0)				//ak mozem ist hore tak idem
					y--;
				else {
					if ((x + 1) >= zahrada->x) break;		//ak mozem skoncit hrabanie v smere doprava tak idem na dalsie hrabanie
					if (tempZahrada[y][x + 1] == 0) {		//ak mozem ist doprava tak idem doprava
						x++;
						smer = 2;
					}
					else {
						if ((x - 1) < 0) break;				//ak mozem skoncit hrabanie v smere dolava tak idem na dalsie hrabanie
						if (tempZahrada[y][x - 1] == 0) {	//ak mozem ist dolava tak idem
							x--;
							smer = 4;
						}
						else {
							freeZahradu(tempZahrada, zahrada);
							return fit_hodnota; 			//uz nemozem ist nikde -> vraciam pocet pohrabanych policok
						}
					}
				}
			}	
			else
			if (smer == 2) {
				if ((x + 1) >= zahrada->x) break;			//ak sme na konci hrabania tak sa ide na dalsie hrabanie

				if (tempZahrada[y][x + 1] == 0)				//ak mozem ist doprava tak idem
					x++;
				else {
					if ((y + 1) >= zahrada->y) break;		//ak mozem skoncit hrabanie v smere dole tak idem na dalsie hrabanie
					if (tempZahrada[y + 1][x] == 0) {		//ak mozem ist dole tak idem
						y++;
						smer = 3;
					}
					else {
						if ((y - 1) < 0) break;				//ak mozem skoncit hrabanie v smere hore tak idem na dalsie hrabanie
						if (tempZahrada[y - 1][x] == 0) {	//ak mozem ist hore tak idem
							y--;
							smer = 1;
						}
						else {
							freeZahradu(tempZahrada, zahrada);
							return fit_hodnota; 			//uz nemozem ist nikde -> vraciam pocet pohrabanych policok
						}
					}
				}
			}
			else
			if (smer == 3) {
				if ((y + 1) >= zahrada->y) break;			//ak sme na konci hrabania tak sa ide na dalsie hrabanie

				if (tempZahrada[y + 1][x] == 0)				//ak mozem ist dole tak idem
					y++;
				else {
					if ((x - 1) < 0) break;					//ak mozem skoncit hrabanie v smere dolava tak idem na dalsie hrabanie
					if (tempZahrada[y][x - 1] == 0) {		//ak mozem ist dolava tak idem
						x--;
						smer = 4;
					}
					else {
						if ((x + 1) >= zahrada->x) break;	//ak mozem skoncit hrabanie v smere doprava tak idem na dalsie hrabanie
						if (tempZahrada[y][x + 1] == 0) {	//ak mozem ist doprava tak idem
							x++;
							smer = 2;
						}
						else {
							freeZahradu(tempZahrada, zahrada);
							return fit_hodnota; 			//uz nemozem ist nikde -> vraciam pocet pohrabanych policok
						}
					}
				}
			}
			else
			if (smer == 4) {
				if ((x - 1) < 0) break;						//ak sme na konci hrabania tak sa ide na dalsie hrabanie

				if (tempZahrada[y][x - 1] == 0)				//ak mozem ist dolava tak idem
					x--;
				else {
					if ((y - 1) < 0) break;					//ak mozem skoncit hrabanie v smere hore tak idem na dalsie hrabanie
					if (tempZahrada[y - 1][x] == 0) {		//ak mozem ist hore tak idem
						y--;
						smer = 1;
					}
					else {
						if ((y + 1) >= zahrada->y) break;	//ak mozem skoncit hrabanie v smere dole tak idem na dalsie hrabanie
						if (tempZahrada[y + 1][x] == 0) {	//ak mozem ist dole tak idem
							y++;
							smer = 3;
						}
						else {
							freeZahradu(tempZahrada, zahrada);
							return fit_hodnota; 			//uz nemozem ist nikde -> vraciam pocet pohrabanych policok
						}
					}
				}
			}
		}
	}

	freeZahradu(tempZahrada, zahrada);

	return fit_hodnota;
}

//funkcia na vybranie jedinca z generacie turnajovou metodou
JEDINEC *turnaj(JEDINEC **generacia) {
	int firstContestant, secondContestant, winner;

	firstContestant = rand() % POCET_JEDINCOV;
	secondContestant = rand() % POCET_JEDINCOV;

	winner = generacia[firstContestant]->fitness > generacia[secondContestant]->fitness ? firstContestant : secondContestant;

	return generacia[winner];
}

//funkcia na vybranie jedinca z generacie ohodnocovacou metodou
JEDINEC *selekciaOhodnotenim(JEDINEC **generacia) {
	int i, j, k = 0;
	int size = ((PRAVDEPODOBNOST_BEST + PRAVDEPODOBNOST_WORST)*(POCET_JEDINCOV)) / 2;					//max velkost pola rulety podla vzorca pre sucet aritmetickej postupnosti
	double iteracia = (double)(PRAVDEPODOBNOST_BEST - PRAVDEPODOBNOST_WORST) / (POCET_JEDINCOV - 2);	//vypocet kroku po ktorom sa bude pravdepodobnost jedincov zvysovat
	double pravdepodobnosti[POCET_JEDINCOV];															//pole s hodnotami pravdepodobnosti pre jedincov v generacii
	int *ruleta = malloc(size * sizeof(int));
	pravdepodobnosti[0] = PRAVDEPODOBNOST_WORST;

	//urcenie pravdepodobnosti jedincov
	for (i = 1; i < POCET_JEDINCOV; i++) 
		pravdepodobnosti[i] = pravdepodobnosti[i - 1] + iteracia;

	//naplnenie pola pre ruletu jedincami podla ich pravdepodobnosti
	for (i = 0; i < POCET_JEDINCOV; i++)
		for (j = 0; j < (int)pravdepodobnosti[i]; j++)
			ruleta[k++] = i;

	//skutocna velkost pola rulety
	size = k;

	//usporiadanie jedincov v generacii podla fitness hodnoty
	heapSort(generacia, POCET_JEDINCOV);

	//vybranie nahodneho jedinca pomocou rulety
	k = ruleta[rand() % size];

	free(ruleta);

	return generacia[k];
}

//funkcia pre jednoduche krizenie
int simpleKrizenie(JEDINEC *newJedinec, JEDINEC **rodicia, int krizenie) {
	int i, rodic1, rodic2;
	rodic1 = rand() % POCET_RODICOV;
	rodic2 = rand() % POCET_RODICOV;

	//bud sa iba skopiruje rodic
	//bud sa skombinuje polovica prveho rodica s polovicou druheho rodica
	//alebo sa skombinuje prva polovica rodica s lepsou fitness hodnotou s druhou polovicou druheho rodica

	if (krizenie == 0) {
		for (i = 0; i < newJedinec->vstupSize; i++)
			newJedinec->vstupGenes[i] = rodicia[rodic1]->vstupGenes[i];			// rodicia[rodicia[rodic1]->fitness > rodicia[rodic2]->fitness ? rodic1 : rodic2]->vstupGenes[i];
	}
	else
		if (krizenie == (FAKTOR_KRIZENIA-1)) {
			for (i = 0; i < (newJedinec->vstupSize / 2); i++)
				newJedinec->vstupGenes[i] = rodicia[rodic1]->vstupGenes[i];
			for(i = (newJedinec->vstupSize / 2); i < newJedinec->vstupSize; i++)
				newJedinec->vstupGenes[i] = rodicia[rodic2]->vstupGenes[i];
		}
		else {
			if (rodicia[rodic1]->fitness > rodicia[rodic2]->fitness) {
				for (i = 0; i < (newJedinec->vstupSize / 2); i++)
					newJedinec->vstupGenes[i] = rodicia[rodic1]->vstupGenes[i];
				for (i = (newJedinec->vstupSize / 2); i < newJedinec->vstupSize; i++)
					newJedinec->vstupGenes[i] = rodicia[rodic2]->vstupGenes[i];
			}
			else {
				for (i = 0; i < (newJedinec->vstupSize / 2); i++)
					newJedinec->vstupGenes[i] = rodicia[rodic2]->vstupGenes[i];
				for (i = (newJedinec->vstupSize / 2); i < newJedinec->vstupSize; i++)
					newJedinec->vstupGenes[i] = rodicia[rodic1]->vstupGenes[i];
			}
		}
	
	return 1;
}

//funkcia pre lepsie krizenie
int permutationKrizenie(JEDINEC *newJedinec, JEDINEC **rodicia, int krizenie) {
	int i, j, k, rodic1, rodic2, crossoverPoint, poc = 0;
	rodic1 = rand() % POCET_RODICOV;
	rodic2 = rand() % POCET_RODICOV;
	crossoverPoint = rand() % rodicia[0]->vstupSize;

	//vyberie sa nahodny bod v ktorom sa rozdelia rodicia
	//skopiruje sa rodic1 po bod rozdelenia
	//doplnaju sa geny z rodic2 take, ktore sa nenachadzaju doteraz v novom jedincovi
	//ak sa minu geny na doplnanie tak sa doplna rovnako z radic1
	//ak stale treba doplnit geny tak sa doplnia nahodne ale iba kladne platne cisla

	for (i = 0; i < crossoverPoint; i++)
		newJedinec->vstupGenes[i] = rodicia[rodic1]->vstupGenes[i];

	for (i = crossoverPoint; i < rodicia[0]->vstupSize; i++) {
		for (j = 0; j < rodicia[0]->vstupSize; j++) {
			for (k = 0; k < i; k++)
				if (newJedinec->vstupGenes[k] == rodicia[rodic2]->vstupGenes[j])
					break;
			if (k == i) {
				poc++;
				newJedinec->vstupGenes[i] = rodicia[rodic2]->vstupGenes[j];
				break;
			}
		}
	}

	if (poc < (rodicia[0]->vstupSize - crossoverPoint)) 
		for (i = crossoverPoint + poc; i < rodicia[0]->vstupSize; i++) {
			for (j = 0; j < rodicia[0]->vstupSize; j++) {
				for (k = 0; k < i; k++)
					if (newJedinec->vstupGenes[k] == rodicia[rodic1]->vstupGenes[j])
						break;
				if (k == i) {
					poc++;
					newJedinec->vstupGenes[i] = rodicia[rodic1]->vstupGenes[j];
					break;
				}
			}
		}

	if (poc < (rodicia[0]->vstupSize - crossoverPoint))
		for (i = crossoverPoint + poc; i < rodicia[0]->vstupSize; i++) 
			newJedinec->vstupGenes[i] = rand() % rodicia[0]->vstupSize;
	
	//debug vypis
	/*
	for (i = 0; i < newJedinec->vstupSize; i++)
		if (newJedinec->vstupGenes[i] > newJedinec->vstupSize || newJedinec->vstupGenes[i] < (-1)*(newJedinec->vstupSize)) {
			printf("WTF\ncrossoverpoint: %d\npoc: %d\n", crossoverPoint, poc);
			printf("rodic1: ");
			for (j = 0; j < newJedinec->vstupSize; j++)
				printf("%d ", rodicia[rodic1]->vstupGenes[j]);
			printf("\nrodic2: ");
			for (j = 0; j < newJedinec->vstupSize; j++)
				printf("%d ", rodicia[rodic2]->vstupGenes[j]);
			printf("\nchild: ");
			for (j = 0; j < newJedinec->vstupSize; j++)
				printf("%d ", newJedinec->vstupGenes[j]);
			getchar();
		}
	*/

}

//funkcia pre mutovanie jedinca pomocou zmeny hodnot genov
int valueMutacia(JEDINEC *newJedinec) {
	int i, mutagen, randomGene, mutacia;

	mutacia = rand() % FAKTOR_MUTACIE;

	//gen sa bud zvacsuje alebo zmensuje o hodnotu 1

	if (mutacia == 0) return 1;
	else
		if (mutacia == (FAKTOR_MUTACIE - 1)) 
			for (i = 0; i < newJedinec->vstupSize; i++) {
				mutagen = rand() % 2;
				if (mutagen)
					if (newJedinec->vstupGenes[i] > 0)
						newJedinec->vstupGenes[i]--;
					else
						newJedinec->vstupGenes[i]++;
				else
					if (newJedinec->vstupGenes[i] < (newJedinec->vstupSize - 1))
						newJedinec->vstupGenes[i]++;
					else
							newJedinec->vstupGenes[i]--;
			}
		else { 
			for (i = 0; i < (mutacia * newJedinec->vstupSize / FAKTOR_MUTACIE); i++) {
				randomGene = rand() % newJedinec->vstupSize;
				mutagen = rand() % 2;
				if (mutagen)
					if (newJedinec->vstupGenes[randomGene] > 0)
						newJedinec->vstupGenes[randomGene]--;
					else
						newJedinec->vstupGenes[randomGene]++;
				else
					if (newJedinec->vstupGenes[randomGene] < (newJedinec->vstupSize - 1))
						newJedinec->vstupGenes[randomGene]++;
					else
							newJedinec->vstupGenes[randomGene]--;
			}
		}
}

//funckia na mutovanie jedinca prehodenim poradia dvoch genov
int permutationMutacia(JEDINEC *newJedinec) {
	int randomGene1, randomGene2, temp;

	randomGene1 = rand() % newJedinec->vstupSize;
	randomGene2 = rand() % newJedinec->vstupSize;

	temp = newJedinec->vstupGenes[randomGene1];
	newJedinec->vstupGenes[randomGene1] = newJedinec->vstupGenes[randomGene2];
	newJedinec->vstupGenes[randomGene2] = temp;

}

//funkcia na vytvorenie noveho jedinca krizenim a mutovanim
JEDINEC *reprodukcia(JEDINEC **rodicia) {
	if (rodicia[0] == NULL) return NULL;

	int i, krizenie, mutacia;
	JEDINEC *newJedinec = (JEDINEC*)malloc(sizeof(JEDINEC));

	newJedinec->fitness = 0;
	newJedinec->smerSize = rodicia[0]->smerSize;
	newJedinec->vstupSize = rodicia[0]->vstupSize;
	newJedinec->vstupGenes = (int*)malloc(newJedinec->vstupSize * sizeof(int));
	newJedinec->smerGenes = (int*)malloc(newJedinec->smerSize * sizeof(int));

	krizenie = rand() % FAKTOR_KRIZENIA;
	mutacia = rand() % 2;

	if (krizenie)
		permutationKrizenie(newJedinec, rodicia, krizenie);
	else
		simpleKrizenie(newJedinec, rodicia, krizenie);

	if (mutacia)
		valueMutacia(newJedinec, mutacia);
	else
		permutationMutacia(newJedinec, mutacia);

	return newJedinec;
}

//funkcia na vytvorenie novej generacie
JEDINEC **vytvorNovuGeneraciu(JEDINEC **generacia) {
	int i;
	JEDINEC **newGeneration = (JEDINEC**)malloc(POCET_JEDINCOV * sizeof(JEDINEC*));
	JEDINEC **rodicia = (JEDINEC**)malloc(POCET_RODICOV * sizeof(JEDINEC*));

	//vytvorenie rodicov pomocou selekcie
	if(SELEKCIA)
		for (i = 0; i < POCET_RODICOV; i++)
			rodicia[i] = selekciaOhodnotenim(generacia);
	else
		for (i = 0; i < POCET_RODICOV; i++)
			rodicia[i] = turnaj(generacia);

	//vytvorenie novych jedincov do novej generacie z rodicov
	for (i = 0; i < POCET_JEDINCOV; i++)
		newGeneration[i] = reprodukcia(rodicia);

	for (i = 0; i < POCET_JEDINCOV; i++) {
		free(generacia[i]->vstupGenes);
		free(generacia[i]->smerGenes);
		free(generacia[i]);
	}
	free(generacia);

	return newGeneration;
}

//funkcia pre vypis pohrabanej zahrady podla daneho jedinca (upravena fitness funkcia)
void vypisZahradu(JEDINEC *jedinec, ZAHRADA *zahrada) {
	int i, j, k = 0;
	int x = 0, y = 0;
	int smer;
	int **tempZahrada = (int **)malloc(zahrada->y * sizeof(int*));

	//vytvorenie zaciatocnej zahrady
	for (i = 0; i < zahrada->y; i++)
		tempZahrada[i] = (int *)calloc(zahrada->x, sizeof(int));

	//vlozenie kamenov
	for (i = 0; i < zahrada->pocet_kamenov; i++)
		tempZahrada[zahrada->kamene[i * 2 + 1]][zahrada->kamene[i * 2]] = -1;

	//vypisanie zahrady
	/*
	for (i = 0; i < zahrada->y; i++) {
	for (j = 0; j < zahrada->x; j++)
	printf("%d ", tempZahrada[i][j]);
	printf("\n");
	}
	*/

	for (i = 0; i < jedinec->vstupSize; i++) {
		x = 0;
		y = 0;

		//premenna smer->	1 - hore
		//					2 - doprava
		//					3 - dole
		//					4 - dolava
		if (jedinec->vstupGenes[i] < zahrada->x && jedinec->vstupGenes[i] > (zahrada->x)*(-1)) {	//ak je gen x-ova suradnica (cislo medzi -x a x, kde x je x-ovy rozmer zahrady)
			if (jedinec->vstupGenes[i] < 0) {														//ak je gen zaporne cislo tak idem znizovat y-ovu suradnicu => y-ova suradnica bude maximalna
				smer = 1;	//ide sa hore
				y = zahrada->y - 1;
			}
			else {
				smer = 3;	//ide sa dole
				y = 0;
			}

			x = jedinec->vstupGenes[i] < 0 ? (-1)*(jedinec->vstupGenes[i]) : jedinec->vstupGenes[i];

		}
		else {																						//gen je teda y-ova suradnica
			if (jedinec->vstupGenes[i] < 0) {														//ak je gen zaporne cislo tak idem znizovat y-ovu suradnicu => y-ova suradnica bude maximalna
				smer = 4;	//ide sa dolava
				x = zahrada->x - 1;
			}
			else {
				smer = 2;	//ide sa doprava
				x = 0;
			}

			y = jedinec->vstupGenes[i] < 0 ? ((-1)*(jedinec->vstupGenes[i])) - zahrada->x : jedinec->vstupGenes[i] - zahrada->x;
		}

		k++;
		//cyklus pokial sa nedostane ku okraju
		while (1) {
			if (tempZahrada[y][x] == 0) {	//hrabanie
				tempZahrada[y][x] = k;
			}

			if (smer == 1) {
				if ((y - 1) < 0) break;		//ak sme na konci hrabania tak sa ide na dalsie hrabanie

				if (tempZahrada[y - 1][x] == 0)			//ak mozem ist hore tak idem
					y--;
				else {
					if ((x + 1) >= zahrada->x) break;		//ak mozem skoncit hrabanie v smere doprava tak idem na dalsie hrabanie
					if (tempZahrada[y][x + 1] == 0) {		//ak mozem ist doprava tak idem doprava
						x++;
						smer = 2;
					}
					else {
						if ((x - 1) < 0) break;				//ak mozem skoncit hrabanie v smere dolava tak idem na dalsie hrabanie
						if (tempZahrada[y][x - 1] == 0) {	//ak mozem ist dolava tak idem
							x--;
							smer = 4;
						}
						else {
							break;			//uz nemozem ist nikde -> vraciam pocet pohrabanych policok
						}
					}
				}
			}
			else
				if (smer == 2) {
					if ((x + 1) >= zahrada->x) break;				//ak sme na konci hrabania tak sa ide na dalsie hrabanie

					if (tempZahrada[y][x + 1] == 0)				//ak mozem ist doprava tak idem
						x++;
					else {
						if ((y + 1) >= zahrada->y) break;		//ak mozem skoncit hrabanie v smere dole tak idem na dalsie hrabanie
						if (tempZahrada[y + 1][x] == 0) {		//ak mozem ist dole tak idem
							y++;
							smer = 3;
						}
						else {
							if ((y - 1) < 0) break;				//ak mozem skoncit hrabanie v smere hore tak idem na dalsie hrabanie
							if (tempZahrada[y - 1][x] == 0) {	//ak mozem ist hore tak idem
								y--;
								smer = 1;
							}
							else {
								break;							//uz nemozem ist nikde -> vraciam pocet pohrabanych policok
							}
						}
					}
				}
				else
					if (smer == 3) {
						if ((y + 1) >= zahrada->y) break;				//ak sme na konci hrabania tak sa ide na dalsie hrabanie

						if (tempZahrada[y + 1][x] == 0)				//ak mozem ist dole tak idem
							y++;
						else {
							if ((x - 1) < 0) break;					//ak mozem skoncit hrabanie v smere dolava tak idem na dalsie hrabanie
							if (tempZahrada[y][x - 1] == 0) {		//ak mozem ist dolava tak idem
								x--;
								smer = 4;
							}
							else {
								if ((x + 1) >= zahrada->x) break;	//ak mozem skoncit hrabanie v smere doprava tak idem na dalsie hrabanie
								if (tempZahrada[y][x + 1] == 0) {	//ak mozem ist doprava tak idem
									x++;
									smer = 2;
								}
								else {
									break;			//uz nemozem ist nikde -> vraciam pocet pohrabanych policok
								}
							}
						}
					}
					else
						if (smer == 4) {
							if ((x - 1) < 0) break;						//ak sme na konci hrabania tak sa ide na dalsie hrabanie

							if (tempZahrada[y][x - 1] == 0)				//ak mozem ist dolava tak idem
								x--;
							else {
								if ((y - 1) < 0) break;					//ak mozem skoncit hrabanie v smere hore tak idem na dalsie hrabanie
								if (tempZahrada[y - 1][x] == 0) {		//ak mozem ist hore tak idem
									y--;
									smer = 1;
								}
								else {
									if ((y + 1) >= zahrada->y) break;	//ak mozem skoncit hrabanie v smere dole tak idem na dalsie hrabanie
									if (tempZahrada[y + 1][x] == 0) {	//ak mozem ist dole tak idem
										y++;
										smer = 3;
									}
									else {
										break; 			//uz nemozem ist nikde -> vraciam pocet pohrabanych policok
									}
								}
							}
						}
		}
	}

	for (i = 0; i < zahrada->y; i++) {
		for (j = 0; j < zahrada->x; j++)
			printf("%2d ", tempZahrada[i][j]);
		printf("\n");
	}
}

int main() {
	int i, pohrabatelne_policka, koniec = 0, pocet = 0;
	ZAHRADA *zahrada = nacitajZahradu("vstup.txt");
	int max = 0, lokalMax, strHodnota;
	int *pohrabanaZahrada = NULL;
	clock_t end, start = clock();
	srand(time(NULL));

	//zistenie poctu pohrabatelnych policok pre danu zahradu
	pohrabatelne_policka = (zahrada->x * zahrada->y) - zahrada->pocet_kamenov;

	//vytvorenie prvej generacie s nahodnymi genmi
	JEDINEC **generacia = vytvorPrvuGeneraciu(zahrada);

	while (koniec == 0 && pocet < POCET_GENERACII) {
		pocet++;
		lokalMax = 0;
		strHodnota = 0;

		//ohodnotenie generacie
		for (i = 0; i < POCET_JEDINCOV; i++) {
			generacia[i]->fitness = fitness(generacia[i], zahrada);
			
			//ak sa naslo riesenie, tak si zapametaj cislo jedinca v generacii
			if (generacia[i]->fitness == pohrabatelne_policka)
				koniec = i + 1;

			strHodnota += generacia[i]->fitness;

			if (lokalMax < generacia[i]->fitness) 
				lokalMax = generacia[i]->fitness;
			
			//vypisovanie zatial najlepsieho riesenia
			if (max < generacia[i]->fitness) {
				max = generacia[i]->fitness;
				printf("New max: %d\n", max);
			}

			if (koniec) break;
			//printf("New max: %d %d\n", max, generacia[i]->fitness);
			//printf("%d - %d\n", pohrabatelne_policka, generacia[i]->fitness);
		}

		strHodnota /= POCET_JEDINCOV;
		/*
		if (pocet < 100)
			printf("%d. GENERATION - MAX %3d - STREDNA HODNOTA %3d\n", pocet, lokalMax, strHodnota);
		*/

		//vytvaranie novej generacie z terajsej generacie
		if(!koniec)
			generacia = vytvorNovuGeneraciu(generacia);
	}

	//vypis riesenia
	if (koniec) {
		end = clock();
		double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

		printf("Naslo sa riesenie!\nGeneracia: %d\nJedinec cislo %d\nCas: %.2f\n\nRiesenie:\n\n", pocet, koniec, time_spent);
		
		vypisZahradu(generacia[koniec - 1], zahrada);
	}
	else 
		printf("Do %d generacii sa nepodarilo najst riesenie.", POCET_GENERACII);

	getchar();
	return 0;
}