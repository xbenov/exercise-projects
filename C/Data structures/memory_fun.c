#include <stdio.h>

void *ptr_pole;

void *memory_alloc(unsigned int size) {
	void *ptr=NULL;
	char *root = (int*)ptr_pole + 1;
	char *akt=root;
	int needed = size + 8;
	int i;

	//najdenie vhodneho volneho bloku
	while (*akt != 0)
	{
		if (*(int*)(akt + *(int*)akt - 4) > (size + 7)) {
			//akt ukazuje na vhodny volny
			akt += *akt;
			//ak je dost zvysneho miesta, splitni
			if ((*(int*)(akt - 4) - size) > 16) {
				//nastavenie velkosti noveho volneho bloku
				*(int*)(akt + needed - 4) = *(int*)(akt - 4) - needed;
				*(int*)(akt + *(int*)(akt - 4) - 8) = *(int*)(akt - 4) - needed;
				//nastavenie velkosti pridelovaneho bloku
				*(int*)(akt - 4) = -needed;
				*(int*)(akt + needed - 8) = -needed;
				//ak bol prideleny prvy volny
				if ((akt + *(akt + 4) + 4) == root)
					*root = *root + needed;
				else 
					//aktualizovanie smerniku predchadzajuceho volneho
					*(int*)(akt + *(int*)(akt + 4)) += needed;
				//ak nie je nasledujuci volny
				if (*akt == 0)
					*(int*)(akt + needed) = 0;
				else{
				
					//aktualizovanie smerniku dalsieho volneho
					*(int*)(akt + (*(int*)akt) + 4) += needed;
					//vytvorenie smernikov noveho volneho bloku
					*(int*)(akt + needed) = *(int*)(akt)-needed;
				}
				//vytvorenie smernikov noveho volneho bloku
				*(int*)(akt + 4 + needed) = *(int*)(akt + 4) - needed;
			}
			else {
				//oznacenie velkosti ze je pridelena
				*(int*)(akt - 4) = -*(int*)(akt - 4);
				*(int*)(akt + (*(int*)(akt - 4) - 8)) = *(int*)(akt - 4);
				//ak bol prideleny prvy volny
				if ((akt + *(akt + 4) + 4) == root)
					*root = *root + *akt;
				else
					//aktualizovanie smerniku predchadzajuceho volneho
					*(int*)(akt + *(int*)(akt + 4)) += *akt;
				//ak nie je nasledujuci volny
				if (*akt == 0)
					*(int*)(akt + *(int*)(akt + 4)) = 0;
				else
					//aktualizovanie smerniku dalsieho volneho
					*(int*)(akt + (*(int*)akt) + 4) += needed;
			}
			//vrati smernik na prideleny blok
			ptr = akt;
			break;
		}
		akt = (char*)akt + *akt;
	}
	return ptr;
}

int memory_free(void *valid_ptr) {
	if (*(int*)((char*)valid_ptr - 4) < 0) {
		char *root = (int*)ptr_pole + 1;
		char *akt = valid_ptr;
		int offset_root = 0;
		//najdenie offsetu k root
		while (akt != root) {
			akt--;
			offset_root--;
		}
		akt = valid_ptr;
		int velkost = -*(int*)(akt - 4);
		int prev_velkost;
		int next_velkost;
		if ((akt - 8) == root)
			prev_velkost = -1;
		else
			prev_velkost = *(int*)(akt - 8);
		if ((akt + velkost - 4) == (root + *(int*)(root - 4)))
			next_velkost = -1;
		else
			next_velkost = *(int*)(akt + velkost - 4);

		//case1 iba pridat na zaciatok free listu
		if ((prev_velkost < 0) && (next_velkost < 0)) {
			//aktualizovat velkost na volnu
			*(int*)(akt - 4) = velkost;
			*(int*)(akt + velkost - 8) = velkost;
			//nastavenie smernika na predchadzajuceho
			*(int*)(akt + 4) = offset_root - 4;
			//nastavenie smernika na nasledujuceho
			*(int*)akt = *root + offset_root;
			//nastavenie noveho root
			*root = -offset_root;
			//nastavenie predchadzajuceho dalsieho
			*(int*)(akt + *(int*)akt + 4) += offset_root;
		}
		//case2 iba predchadzajuci je volny, merge s predchadzajucim
		if ((prev_velkost >= 0) && (next_velkost < 0)) {
			//nastavenie akt noveho volneho bloku
			akt -= prev_velkost;
			//new offset k root
			offset_root = 0;
			while (akt != root) {
				akt--;
				offset_root--;
			}
			//znovu nastavenie akt na novy free blok
			akt -= offset_root;
			//aktualizovanie velkosti noveho volneho bloku
			*(int*)(akt - 4) += velkost;
			*(int*)(akt + prev_velkost + velkost - 8) = prev_velkost + velkost;
			//ak je novy blok zaroven aj prvy v free liste nemusime vyclenovat a pridavat na zaciatok
			if ((akt + *(int*)(akt + 4) + 4) == root)
				//ak je predchadzajuci posledny vo free liste nemozem menit next free
				if (*(int*)akt == 0) {
					//previous free bude posledny
					*(int*)(akt + *(int*)(akt + 4)) = 0;
					//pridanie noveho volneho bloku na zaciatok free listu
					*(int*)akt = *(int*)root + offset_root;
					*(int*)(akt + *(int*)akt + 4) -= offset_root + 4;
					*(int*)root = -offset_root;
					*(int*)(akt + 4) = offset_root - 4;
				}
				else {
					//vyclenenie predchadzajuceho bloku z free listu
					*(int*)(akt + *(int*)(akt + 4)) += *(int*)akt;
					*(int*)(akt + *(int*)akt + 4) += *(int*)(akt + 4);
					//pridanie noveho volneho bloku na zaciatok free listu
					*(int*)akt = *(int*)root + offset_root;
					*(int*)(akt + *(int*)akt + 4) -= offset_root + 4;
					*(int*)root = -offset_root;
					*(int*)(akt + 4) = offset_root - 4;
				}
		}
		//case3 iba nasledujuci je volny, merge s nasledujucim
		if ((prev_velkost < 0) && (next_velkost >= 0)) {
			//aktualizovanie velkosti noveho volneho bloku
			*(int*)(akt - 4) = velkost + next_velkost;
			*(int*)(akt + next_velkost + velkost - 8) = next_velkost + velkost;
			//vyclenenie nasledujuceho bloku z free listu
			//ak je posledny vo free liste tak nemozem menit next free a previous free bude posledny 
			if (*(int*)(akt + velkost) == 0)
				*(int*)(akt + velkost + *(int*)(akt + velkost + 4)) = 0;
			else {
				*(int*)(akt + velkost + *(int*)(akt + velkost + 4)) += *(int*)(akt + velkost);
				*(int*)(akt + velkost + *(int*)(akt + velkost) + 4) += *(int*)(akt + velkost + 4);
			}
			//pridanie noveho volneho bloku na zaciatok free listu
			*(int*)akt = *(int*)root + offset_root;
			*(int*)(akt + *(int*)akt + 4) -= offset_root + 4;
			*(int*)root = -offset_root;
			*(int*)(akt + 4) = offset_root - 4;
		}
		//case4 aj nasledujuci aj predchadzajuci je volny, merge s oboma
		if ((prev_velkost >= 0) && (next_velkost >= 0)) {
			//nastavenie akt noveho volneho bloku
			akt -= prev_velkost;
			//new offset k root
			offset_root = 0;
			while (akt != root) {
				akt--;
				offset_root--;
			}
			//znovu nastavenie akt na novy free blok
			akt -= offset_root;
			//aktualizovanie velkosti noveho volneho bloku
			*(int*)(akt - 4) += velkost + next_velkost;
			*(int*)(akt + prev_velkost + velkost + next_velkost - 8) = prev_velkost + velkost + next_velkost;
			//ak je novy blok zaroven aj prvy vo free liste nemusime pridavat na zaciatok a vyclenovat predchadzajuci blok
			if ((akt + *(int*)(akt + 4) + 4) == root) {
				//vyclenenie nasledujuceho bloku z free listu
				//ak je posledny vo free liste tak previous bude posledny
				if (*(int*)(akt + prev_velkost + velkost) == 0)
					*(int*)(akt + velkost + prev_velkost + *(int*)(akt + velkost + prev_velkost + 4)) = 0;
				else {
					*(int*)(akt + velkost + prev_velkost + *(int*)(akt + velkost + prev_velkost + 4)) += *(int*)(akt + velkost + prev_velkost);
					*(int*)(akt + velkost + prev_velkost + *(int*)(akt + velkost + prev_velkost) + 4) += *(int*)(akt + velkost + prev_velkost + 4);
				}
			}
			else {
				//ak bol nasledujuci blok prvy vo free liste
				if ((akt + velkost + prev_velkost + *(int*)(akt + velkost + prev_velkost + 4) + 4) == root) {
					//vyclenenie predchadzajuceho bloku z free listu
					//ak je posledny vo free liste tak previous bude posledny
					if (*(int*)akt == 0)
						*(int*)(akt + *(int*)(akt + 4)) = 0;
					else {
						*(int*)(akt + *(int*)(akt + 4)) += *(int*)akt;
						*(int*)(akt + *(int*)akt + 4) += *(int*)(akt + 4);
					}
					//pridanie noveho volneho bloku na zaciatok free listu a zaroven vyclenenie nasledujuceho
					*(int*)akt = velkost + prev_velkost + *(int*)(akt + velkost + prev_velkost);
					*(int*)(akt + *(int*)akt + 4) = -*(int*)akt;
					*(int*)root = -offset_root;
					*(int*)(akt + 4) = offset_root - 4;
				}
				else {
					//vyclenenie predchadzajuceho bloku z free listu
					//ak je posledny vo free liste tak previous bude posledny
					if (*(int*)akt == 0)
						*(int*)(akt + *(int*)(akt + 4)) = 0;
					else {
						*(int*)(akt + *(int*)(akt + 4)) += *(int*)akt;
						*(int*)(akt + *(int*)akt + 4) += *(int*)(akt + 4);
					}
					//vyclenenie nasledujuceho bloku z free listu
					//ak je posledny vo free liste tak previous bude posledny
					if (*(int*)(akt + prev_velkost + velkost) == 0)
						*(int*)(akt + velkost + prev_velkost + *(int*)(akt + velkost + prev_velkost + 4)) = 0;
					else {
						*(int*)(akt + velkost + prev_velkost + *(int*)(akt + velkost + prev_velkost + 4)) += *(int*)(akt + velkost + prev_velkost);
						*(int*)(akt + velkost + prev_velkost + *(int*)(akt + velkost + prev_velkost) + 4) += *(int*)(akt + velkost + prev_velkost + 4);
					}
					//pridanie noveho volneho bloku na zaciatok free listu
					*(int*)akt = *(int*)root + offset_root;
					*(int*)(akt + *(int*)akt + 4) -= offset_root + 4;
					*(int*)root = -offset_root;
					*(int*)(akt + 4) = offset_root - 4;
				}
			}
		}
		
		return 0;
	}
	return 1;
}

int memory_check(void *ptr) {
	char *akt = (char*)ptr_pole + 8;
	while (akt != ((char*)ptr_pole + *(int*)ptr_pole))
	{
		if (*(int*)akt < 0) {
			if ((akt + 4) == ptr)return 1;
			akt += -*(int*)akt;
		}
		else
			akt += *(int*)akt;
	}
	return 0;
}

void memory_init(void *ptr, unsigned int size) {
	if (ptr == NULL)return;
	ptr_pole = ptr;
	*(int*)ptr_pole = size;
	*(int*)((int*)ptr_pole + 1) = 300;
	*((int*)ptr_pole + 2) = size - 2 * sizeof(int);
	*((int*)ptr_pole + 3) = 0;
	*((int*)ptr_pole + 4) = -12;
	*(int*)((char*)ptr_pole + (size - 4)) = *(int*)((int*)ptr_pole + 2);
}


int main() {
	int i;
	char nieco[200];
	void* ptr1,*ptr2,*ptr3,*ptr4,*ptr5,*ptr6,*ptr7,*ptr8,*ptr9;
	memory_init(nieco,200);
	for (i = 0; i < 200; i++) {
		printf("%d\n", *((char*)ptr_pole + i));
	}
	/*
	ptr1 = memory_alloc(10);
	printf("magic1\n");
	ptr2 = memory_alloc(10);
	printf("magic2\n");
	ptr3 = memory_alloc(10);
	printf("magic3\n");
	ptr4 = memory_alloc(10);
	printf("magic4\n");
	ptr5 = memory_alloc(10);
	printf("magic5\n");
	ptr6 = memory_alloc(10);
	printf("magic6\n");
	ptr7 = memory_alloc(10);
	printf("magic7\n");
	for (i = 0; i < 200; i++) {
		printf("%d\n", *((char*)ptr_pole + i));
	}
	ptr8 = memory_alloc(10);
	printf("magic8\n");
	ptr9 = memory_alloc(10);
	printf("magic9\n");
	memory_free(ptr1);
	memory_free(ptr3);
	memory_free(ptr7);
	memory_free(ptr1);
	memory_free(ptr8);
	memory_free(ptr5);
	memory_free(ptr2);
	memory_free(ptr4);
	memory_free(ptr6);
	memory_free(ptr9);
	for (i = 0; i < 100; i++) {
		printf("%d\n", *((char*)ptr_pole + i));
	}
	
	int i;
	char nieco[100];
	void* ptr1, *ptr2, *ptr3;
	memory_init(nieco, 100);
	ptr1 = memory_alloc(10);
	ptr2 = memory_alloc(10);
	ptr3 = memory_alloc(10);
	memory_free(ptr1);
	memory_free(ptr3);
	printf("%d", memory_check(ptr3));
	memory_free(ptr2);
	for (i = 0; i < 100; i++) {
		printf("%d\n", *((char*)ptr_pole + i));
	}
	*/
	getchar();
	getchar();
	return 0;
}