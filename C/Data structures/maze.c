// zadanie3.c -- Viktor Beno, 22.11.2018 17:10


// NxMx(2^7) graf
// najprv graf normalny so stavom 0000000
// potom gen alebo drak a potm princezne
// velky graf so stavmi
// ukladat si info o dlzke trasy
// stavy v halde
// nepotrebne z haldy von
// pripravit si premennu kde budu teleporty a ich body


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define INFINITY 1000000000
// #define DRAGONMASK 1000000
// #define GENMASK 0100000
// #define P1MASK 0010000
// #define P2MASK 0001000
// #define P3MASK 0000100
// #define P4MASK 0000010
// #define P5MASK 0000001

typedef struct adj_list_prvok {
	char typ;
	int id, weight;
	struct adj_list_prvok *dalsi;
} ADJ_LIST_PRVOK;

typedef struct Vrchol {
	char typ;
	unsigned char stav;
	int x, y, id, distance;
	ADJ_LIST_PRVOK *zac;
	int predchodca;
} VRCHOL;

typedef struct graph {
	int pocet_vrcholov;
	VRCHOL *vrcholy;
} GRAPH;

//inicializuje premennu typu GRAPH*
GRAPH *init_graph() {
	GRAPH *graf = (GRAPH*)malloc(sizeof(GRAPH));
	graf->pocet_vrcholov = 0;
	graf->vrcholy = NULL;
	return graf;
}

void showbits(unsigned char x) {
	for (int i = 7; i >= 0; i--) {
		(x & (1u << i)) ? putchar('1') : putchar('0');
	}
	//printf("\n");
}

int getD(unsigned char stav) {
	unsigned char a = stav << 2;
	return a >> 7 > 0 ? 1 : 0;
}

int getG(unsigned char stav) {
	unsigned char a = stav << 1;
	return a >> 7 > 0 ? 1 : 0;
}

int getP1(unsigned char stav) {
	unsigned char a = stav << 3;
	return a >> 7 > 0 ? 1 : 0;
}

int getP2(unsigned char stav) {
	unsigned char a = stav << 4;
	return a >> 7 > 0 ? 1 : 0;
}

int getP3(unsigned char stav) {
	unsigned char a = stav << 5;
	return a >> 7 > 0 ? 1 : 0;
}

int getP4(unsigned char stav) {
	unsigned char a = stav << 6;
	return a >> 7 > 0 ? 1 : 0;
}

int getP5(unsigned char stav) {
	unsigned char a = stav << 7;
	return a >> 7 > 0 ? 1 : 0;
}

void setD(unsigned char *stav) {
	*stav = *stav | 32;
}
void setG(unsigned char *stav) {
	*stav = *stav | 64;
}
void setP1(unsigned char *stav) {
	*stav = *stav | 16;
}
void setP2(unsigned char *stav) {
	*stav = *stav | 8;
}
void setP3(unsigned char *stav) {
	*stav = *stav | 4;
}
void setP4(unsigned char *stav) {
	*stav = *stav | 2;
}
void setP5(unsigned char *stav) {
	*stav = *stav | 1;
}

unsigned char unsetD(unsigned char stav) {
	stav &= ~(1UL << 5);
	return stav;
}
unsigned char unsetG(unsigned char stav) {
	stav &= ~(1UL << 6);
	return stav;
}
unsigned char unsetP1(unsigned char stav) {
	stav &= ~(1UL << 4);
	return stav;
}
unsigned char unsetP2(unsigned char stav) {
	stav &= ~(1UL << 3);
	return stav;
}
unsigned char unsetP3(unsigned char stav) {
	stav &= ~(1UL << 2);
	return stav;
}
unsigned char unsetP4(unsigned char stav) {
	stav &= ~(1UL << 1);
	return stav;
}
unsigned char unsetP5(unsigned char stav) {
	stav &= ~(1UL << 0);
	return stav;
}

//funkcia na pridanie noveho vrcholu, vracia jeho id
int add_new_vertice(GRAPH *graf, char typ, int x, int y, unsigned char stav) {
	graf->pocet_vrcholov++;		//zvys pocet vrcholov

	if (graf->vrcholy != NULL)	//ak uz bol malloc tak zvacsi reallocom
	{
		graf->vrcholy = (VRCHOL*)realloc(graf->vrcholy, graf->pocet_vrcholov * sizeof(VRCHOL));
	}
	else
		graf->vrcholy = (VRCHOL*)malloc(sizeof(VRCHOL));

	//nastavit hodnoty noveho vrcholu
	graf->vrcholy[graf->pocet_vrcholov - 1].typ = typ;
	graf->vrcholy[graf->pocet_vrcholov - 1].x = x;
	graf->vrcholy[graf->pocet_vrcholov - 1].y = y;
	graf->vrcholy[graf->pocet_vrcholov - 1].stav = stav;
	graf->vrcholy[graf->pocet_vrcholov - 1].id = graf->pocet_vrcholov - 1;
	graf->vrcholy[graf->pocet_vrcholov - 1].zac = NULL;

	return graf->pocet_vrcholov - 1;
}

//zjednodusena funkcia na pridanie hrany pre funkciu graphtomaze (pridava hranu k najnovsiemu vrcholu)
GRAPH *add_edge_simplified(GRAPH *graf, int x, int y) {
	int i = graf->pocet_vrcholov - 2, weight = 1;
	int j = graf->pocet_vrcholov - 1;					//j oznacuje index posledneho vrcholu
	ADJ_LIST_PRVOK *akt;

	//k vrcholu so suradnicami x, y pridaj hranu k najnovsiemu vrcholu
	while (graf->vrcholy[i].x != x || graf->vrcholy[i].y != y)	//hladanie vrcholu je O(m), m je sirka mapy
		i--;

	if (graf->vrcholy[i].typ == 'H')
		weight = 2;

	akt = graf->vrcholy[i].zac;

	if (akt == NULL)
	{
		graf->vrcholy[i].zac = (ADJ_LIST_PRVOK*)malloc(sizeof(ADJ_LIST_PRVOK));
		graf->vrcholy[i].zac->dalsi = NULL;
		graf->vrcholy[i].zac->id = graf->vrcholy[j].id;
		graf->vrcholy[i].zac->weight = weight;
	}
	else
	{
		while (akt->dalsi != NULL)
			akt = akt->dalsi;
		akt->dalsi = (ADJ_LIST_PRVOK*)malloc(sizeof(ADJ_LIST_PRVOK));
		akt = akt->dalsi;
		akt->dalsi = NULL;
		akt->id = graf->vrcholy[j].id;
		akt->weight = weight;
	}



	//k najnovsiemu vrcholu pridaj hranu k vrcholu so suradnicami x, y
	if (graf->vrcholy[j].typ == 'H')		//podla typu najnovsieho vrcholu urci vahu hrany
		weight = 2;
	else
		weight = 1;

	akt = graf->vrcholy[j].zac;

	if (akt == NULL)
	{
		graf->vrcholy[j].zac = (ADJ_LIST_PRVOK*)malloc(sizeof(ADJ_LIST_PRVOK));
		graf->vrcholy[j].zac->dalsi = NULL;
		graf->vrcholy[j].zac->id = graf->vrcholy[i].id;
		graf->vrcholy[j].zac->weight = weight;
	}
	else
	{
		while (akt->dalsi != NULL)
			akt = akt->dalsi;
		akt->dalsi = (ADJ_LIST_PRVOK*)malloc(sizeof(ADJ_LIST_PRVOK));
		akt = akt->dalsi;
		akt->dalsi = NULL;
		akt->id = graf->vrcholy[i].id;
		akt->weight = weight;
	}


	return graf;
}

GRAPH *addZeroEdge(GRAPH *graf, int from, int to) {
	ADJ_LIST_PRVOK *akt = graf->vrcholy[from].zac;

	//printf("PRIDAVAM ZE: %d[%c] -> %d[%c]\n",from, graf->vrcholy[from].typ, to, graf->vrcholy[to].typ);

	if (akt == NULL)
	{
		graf->vrcholy[from].zac = (ADJ_LIST_PRVOK*)malloc(sizeof(ADJ_LIST_PRVOK));
		graf->vrcholy[from].zac->dalsi = NULL;
		graf->vrcholy[from].zac->id = to;
		graf->vrcholy[from].zac->weight = 0;
	}
	else
	{

		while (akt->dalsi != NULL)
			akt = akt->dalsi;
		akt->dalsi = (ADJ_LIST_PRVOK*)malloc(sizeof(ADJ_LIST_PRVOK));
		akt = akt->dalsi;
		akt->dalsi = NULL;
		akt->id = to;
		akt->weight = 0;
	}

	return graf;
}

//prevedie bludisko na graf
GRAPH *graphFromMaze(char **mapa, int height, int width, unsigned char *koniec) {
	//zostrojit z mapy graf, v tomto pripade je vyhodnejsie adjacency list
	int i, j, s = 0, id = -1, vnStav = 0, pocetP = 0;
	int drakZivi = 1;
	int p1 = -1, p2 = -1, p3 = -1, p4 = -1, p5 = -1, d = -1, g;

	GRAPH *graf = init_graph();

	// zistenie id vrcholu draka, princezien a vytvorenie stavu 0
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			if (mapa[i][j] == 'N')
				continue;
			
			id = add_new_vertice(graf, mapa[i][j], j, i, (unsigned char)0);	//pridaj vrchol

			if (i > 0)										//ak sa da pozret na policko nad nim
				if (mapa[i - 1][j] != 'N')					//policko je c,d,h,p,g alebo cislica 0 az 9 (alebo iny znak ale predpoklada sa ze sa pouzivaju iba znaku urcene v zadani) okrem znaku n
					add_edge_simplified(graf, j, i - 1);

			if (j > 0) {									//ak sa da pozret na policko vlavo
				//podla policka na lavo vytvor hranu
				if (mapa[i][j - 1] != 'N')					//policko je c,d,h,p,g alebo cislica 0 az 9 (alebo iny znak ale predpoklada sa ze sa pouzivaju iba znaku urcene v zadani) okrem znaku n
					add_edge_simplified(graf, j - 1, i);
			}

			if (mapa[i][j] == 'D')
				d = id;

			if (mapa[i][j] == 'P') {
				if (p5 < 0) {
					p5 = id;
					//printf("p5: %d\n",p5);
					pocetP++;
					continue;
				}
				if (p4 < 0) {
					p4 = id;
					//printf("p4: %d\n", p4);
					pocetP++;
					continue;
				}
				if (p3 < 0) {
					p3 = id;
					//printf("p3: %d\n", p3);
					pocetP++;
					continue;
				}
				if (p2 < 0) {
					p2 = id;
					pocetP++;
					continue;
				}
				if (p1 < 0) {
					p1 = id;
					pocetP++;
					continue;
				}
			}
		}
	}

	// pocet vrcholov grafu v jednom stave
	vnStav = graf->pocet_vrcholov;

	ADJ_LIST_PRVOK *akt;
	//printf("foo %d\n",vnStav);
	//prechadzat kazdym polickom v bludisku z lava do prava a z hora dole
	for (s = 32; s < (32 + (int)pow(2,pocetP)); s++) {
		//printf("stav: %d pocetP: %d\n",s, pocetP);
		// vytvorenie grafu so stavom s
		for (i = 0; i < height; i++) {
			for (j = 0; j < width; j++) {
				if (mapa[i][j] != 'N') {							//ak je prave skumane policko vrcholom grafu
					id = add_new_vertice(graf, mapa[i][j], j, i, (unsigned char)s);	//pridaj vrchol

					if (i > 0)										//ak sa da pozret na policko nad nim
						if (mapa[i - 1][j] != 'N')					//policko je c,d,h,p,g alebo cislica 0 az 9 (alebo iny znak ale predpoklada sa ze sa pouzivaju iba znaku urcene v zadani) okrem znaku n
							add_edge_simplified(graf, j, i - 1);

					if (j > 0) {									//ak sa da pozret na policko vlavo
						//podla policka na lavo vytvor hranu
						if (mapa[i][j - 1] != 'N')					//policko je c,d,h,p,g alebo cislica 0 az 9 (alebo iny znak ale predpoklada sa ze sa pouzivaju iba znaku urcene v zadani) okrem znaku n
							add_edge_simplified(graf, j - 1, i);
					}

				}
			}
		}
		/*
		for (i = 0; i < graf->pocet_vrcholov; i++) {
			printf("\nid: %d\nsusedia:\n",i);
			akt = graf->vrcholy[i].zac;
			while (akt != NULL)
			{
				printf("\tid: %d\n", akt->id);
				akt = akt->dalsi;
			}
		}
		*/




		// spajanie stavov podla potreby

		if (drakZivi && getD((unsigned char)s)) {
			// zabijeme draka
			//printf("id d-0: %d   id d-1: %d\n", d, d + vnStav * (unsigned char)s);
			addZeroEdge(graf, d, d + vnStav * (unsigned char)(s - 31));
			drakZivi = 0;
		}
		//printf("STAV: ");
		//showbits((unsigned char)s);
		//printf("%d %d %d\n", vnStav, (unsigned char)s, (unsigned char)(s - 31));
		// ak je uz zabity drak, spajaj princezne v stavoch
		if (getD((unsigned char)s)) {
			
			if (p5 >= 0 && getP5((unsigned char)s))
				addZeroEdge(graf, p5 + vnStav * (unsetP5((unsigned char)(s - 32))+1), p5 + vnStav * ((unsigned char)(s - 32)+1));

			//printf("%d + %d * %d\n",p5,vnStav, unsetP5((unsigned char)(s - 32))+1);
			
			if (p4 >= 0 && getP4((unsigned char)s))
				addZeroEdge(graf, p4 + vnStav * (unsetP4((unsigned char)(s - 32))+1), p4 + vnStav * ((unsigned char)(s - 32)+1));
			if (p3 >= 0 && getP3((unsigned char)s))
				addZeroEdge(graf, p3 + vnStav * (unsetP3((unsigned char)(s - 32))+1), p3 + vnStav * ((unsigned char)(s - 32)+1));
			if (p2 >= 0 && getP2((unsigned char)s))
				addZeroEdge(graf, p2 + vnStav * (unsetP2((unsigned char)(s - 32))+1), p2 + vnStav * ((unsigned char)(s - 32)+1));
			if (p1 >= 0 && getP1((unsigned char)s))
				addZeroEdge(graf, p1 + vnStav * (unsetP1((unsigned char)(s - 32))+1), p1 + vnStav * ((unsigned char)(s - 32)+1));
		}
	}

	/*
	for (i = 0; i < graf->pocet_vrcholov; i++) {
		if (graf->vrcholy[i].typ != 'p')
			continue;
		printf("\nP id: %d\nSTAV:  ",i);
		showbits(graf->vrcholy[i].stav);
		printf("susedia:\n");
		akt = graf->vrcholy[i].zac;
		while (akt != NULL)
		{
			printf("\tid: %d\n", akt->id);
			akt = akt->dalsi;
		}

	}
	*/

	*koniec = 32 + (unsigned char)pow(2, pocetP)-1;

	return graf;
}

typedef struct minHeapPrvok {
	int id, distance;
} MINHEAPPRVOK;

void swap(MINHEAPPRVOK **x, MINHEAPPRVOK **y) {

	MINHEAPPRVOK *temp = *x;
	*x = *y;
	*y = temp;

}

void minHeapify(MINHEAPPRVOK ***minHeap, int size, int **pos, int index)
{
	MINHEAPPRVOK *x_prvok;
	MINHEAPPRVOK *index_prvok;

	int x = index;
	int left = 2 * index + 1;
	int right = 2 * index + 2;

	if (left < size && (*minHeap)[left]->distance < (*minHeap)[x]->distance)
		x = left;

	if (right < size && (*minHeap)[right]->distance < (*minHeap)[x]->distance)
		x = right;

	if (x != index)
	{
		x_prvok = (*minHeap)[x];
		index_prvok = (*minHeap)[index];

		(*pos)[x_prvok->id] = index;
		(*pos)[index_prvok->id] = x;

		swap(&(*minHeap)[x], &(*minHeap)[index]);

		minHeapify(minHeap, size, pos, x);
	}
}

void decreaseKey(MINHEAPPRVOK ***minHeap, int size, int **pos, int v, int dist)
{
	int i = (*pos)[v];

	(*minHeap)[i]->distance = dist;

	while (i && (*minHeap)[i]->distance < (*minHeap)[(i - 1) / 2]->distance)
	{

		(*pos)[(*minHeap)[i]->id] = (i - 1) / 2;
		(*pos)[(*minHeap)[(i - 1) / 2]->id] = i;

		swap(&(*minHeap)[i], &(*minHeap)[(i - 1) / 2]);

		i = (i - 1) / 2;
	}
}

MINHEAPPRVOK* extractMin(MINHEAPPRVOK ***minHeap, int *size, int **pos)
{
	MINHEAPPRVOK* min;
	MINHEAPPRVOK* posledny;

	if (!size)
		return NULL;

	min = (*minHeap)[0];

	posledny = (*minHeap)[*size - 1];
	(*minHeap)[0] = posledny;

	(*pos)[min->id] = *size - 1;
	(*pos)[posledny->id] = 0;

	--(*size);

	minHeapify(minHeap, *size, pos, 0);

	return min;
}

// hladat nie po id vrchola ale po typ vrchola
int *dijkstra(GRAPH *graf, int zaciatok, unsigned char koniec, int *dlzka) {
	ADJ_LIST_PRVOK *akt;
	ADJ_LIST_PRVOK *akt2;
	MINHEAPPRVOK* minHeapNode;
	int i, u, pocet_vrcholov = graf->pocet_vrcholov;
	int vzdialenost = 0;
	int *suradnice;
	int *dist = (int*)malloc(pocet_vrcholov * sizeof(int));
	int *pos = (int*)malloc(pocet_vrcholov * sizeof(int));
	int size_min_heap = pocet_vrcholov;

	//printf("zaciatok: %d\nkoniec: %d\npocet vrcholov: %d\n",zaciatok,koniec,pocet_vrcholov);

	MINHEAPPRVOK **minHeap = (MINHEAPPRVOK**)malloc(pocet_vrcholov * sizeof(MINHEAPPRVOK*));

	for (i = 0; i < pocet_vrcholov; i++) {
		minHeap[i] = (MINHEAPPRVOK*)malloc(sizeof(MINHEAPPRVOK));
		dist[i] = INFINITY;
		minHeap[i]->distance = INFINITY;
		minHeap[i]->id = i;
		pos[i] = i;
	}

	minHeap[zaciatok] = (MINHEAPPRVOK*)malloc(sizeof(MINHEAPPRVOK));
	minHeap[zaciatok]->distance = dist[zaciatok];
	minHeap[zaciatok]->id = zaciatok;

	pos[zaciatok] = zaciatok;
	dist[zaciatok] = 0;
	decreaseKey(&minHeap, size_min_heap, &pos, zaciatok, dist[zaciatok]);

	graf->vrcholy[zaciatok].predchodca = -1;

	while (size_min_heap) {
		minHeapNode = extractMin(&minHeap, &size_min_heap, &pos);
		u = minHeapNode->id;
		//printf("vzdialenost: %d\n",minHeapNode->distance);
		
		// printf("stav: ");
		// showbits(graf->vrcholy[u].stav);
		// printf(" vrchol: %d  == ",u);
		// showbits(koniec);
		// printf(" <- koniec ?\n");

		// akt2 = graf->vrcholy[u].zac;
		// while (akt2 != NULL)
		// {
		// 	printf("\tid: %d\n", akt2->id);
		// 	akt2 = akt2->dalsi;
		// }
		
		if (graf->vrcholy[u].stav == koniec)
			break;

		akt = graf->vrcholy[u].zac;
		while (akt != NULL)
		{
			int v = akt->id;

			if (pos[v] < size_min_heap && dist[u] != INFINITY && akt->weight + dist[u] < dist[v])
			{
				dist[v] = dist[u] + akt->weight;

				decreaseKey(&minHeap, size_min_heap, &pos, v, dist[v]);

				if (akt->weight == 0)
					graf->vrcholy[v].predchodca = graf->vrcholy[u].predchodca;
				else
					graf->vrcholy[v].predchodca = u;
			}
			akt = akt->dalsi;
		}
	}

	vzdialenost = dist[u];
	//printf("%d\n", vzdialenost);
	pocet_vrcholov = 0;

	int temp = u;

	while (temp >= 0) {
		pocet_vrcholov++;
		temp = graf->vrcholy[temp].predchodca;
	}

	suradnice = (int*)malloc(2 * pocet_vrcholov * sizeof(int));

	for (i = pocet_vrcholov - 1; i >= 0; i--) {
		suradnice[2 * i] = graf->vrcholy[u].x;
		suradnice[2 * i + 1] = graf->vrcholy[u].y;

		u = graf->vrcholy[u].predchodca;
	}
	//printf("pocet vrch2: %d\n", pocet_vrcholov);
	*dlzka = pocet_vrcholov;

	return suradnice;
}

int *zachran_princezne(char **mapa, int n, int m, int t, int *dlzka_cesty)
{
	int i, j, zac = 0;
	unsigned char koniec = 0;
	GRAPH *graf;
	graf = graphFromMaze(mapa, n, m, &koniec);


	/*
	for(i = 0; i < n; i++){
	  for(j = 0; j < m; j++)
		printf("%c ",mapa[i][j]);
	  printf("\n");
	}

	printf("%d %d %d %d\n",drak,p1,p2,p3);

	  ADJ_LIST_PRVOK *akt;
	  for (i = 0; i < graf->pocet_vrcholov; i++) {
		  printf("V %d: ", i+1);
		  akt = graf->vrcholy[i].zac;
		  while (akt != NULL) {
			  printf("%d ", akt->id + 1);
			  akt = akt->dalsi;
		  }
		  printf("\n");
	  }
	  */

	int *suradnice;

	suradnice = dijkstra(graf, zac, koniec, dlzka_cesty);

	return suradnice;
}




// Vlastna funkcia main() je pre vase osobne testovanie. Dolezite: pri testovacich scenaroch sa nebude spustat!
int main()
{
	// 	unsigned char stav = 0;
	// 	showbits(stav);
	// 	printf("GET: %d\n",getD(stav));
	// 	setD(&stav);
	// 	printf("GET: %d\n",getD(stav));
	// 	stav = unsetD(stav);
	// 	printf("GET: %d\n",getD(stav));
	// 	setD(&stav);
	// 	printf("GET: %d\n",getD(stav));
	// 	showbits(stav);

	// return 0;

	int map_w = 15, j;
	int map_h = 10;
	char map[10][15] = {
		{ 'c', 'c', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n' },
		{ 'n', 'c', 'n', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'n', 'c', 'n', 'n' },
		{ 'n', 'c', 'n', 'n', 'n', 'c', 'n', 'n', 'n', 'c', 'n', 'c', 'p', 'c', 'n' },
		{ 'n', 'c', 'h', 'h', 'h', 'p', 'h', 'h', 'h', 'h', 'c', 'c', 'n', 'c', 'n' },
		{ 'n', 'c', 'n', 'n', 'c', 'n', 'n', 'h', 'n', 'h', 'n', 'n', 'n', 'c', 'n' },
		{ 'n', 'c', 'c', 'c', 'c', 'n', 'h', 'n', 'n', 'h', 'n', 'c', 'c', 'c', 'n' },
		{ 'n', 'c', 'n', 'n', 'h', 'h', 'h', 'h', 'h', 'h', 'n', 'n', 'c', 'n', 'n' },
		{ 'n', 'c', 'n', 'n', 'p', 'n', 'n', 'n', 'n', 'c', 'c', 'c', 'c', 'n', 'n' },
		{ 'n', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'n', 'd', 'n', 'n', 'c', 'c', 'n' },
		{ 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n' }
	};
	char * map_d_r[10] = { map[0], map[1], map[2], map[3], map[4], map[5], map[6], map[7], map[8], map[9] };
	char ** map_d = map_d_r;
	int *suradnice, dlzka, i;

	suradnice = zachran_princezne(map_d, map_h, map_w, 10000, &dlzka);

	printf("Dlzka cesty: %d\n", dlzka);
	for (i = 0; i < dlzka; i++)
		printf("%d %d\n", suradnice[2 * i], suradnice[2 * i + 1]);

	getchar();

	return 0;
}