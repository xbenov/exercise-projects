#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FAKTY_MAX_STRING	100
#define PRAVIDLA_MAX_STRING 100

/*struct pre pravidlo*/
typedef struct pravidlo {
	int pocet_podmienok, pocet_akcii;
	char *meno;
	char **podmienky;
	char **akcie;
}PRAVIDLO;

/*struct pre aplikovatelne akcie pravidiel*/
typedef struct aplikPravidlo {
	int pocet_akcii;
	char *meno;
	char **akcie;
}APLIKPRAVIDLO;

char **PPam;			//fakty
PRAVIDLO **BPrav;		//pravidla
APLIKPRAVIDLO **Aplik;	//aplikovatelne akcie pravidiel
int pocet_faktov;
int pocet_pravidiel;
int pocet_aplik;

/*prida string na koniec pola*/
int pridaj_string_do_pola(char ***pole_stringov, char *string, int *size){
	char *temp;

	if (*pole_stringov == NULL)
		*pole_stringov = malloc((++(*size)) * sizeof(char*));
	else
		*pole_stringov = realloc(*pole_stringov, (++(*size)) * sizeof(char*));

	if (*pole_stringov == NULL)
	{
		printf("Failed adding string %s\n", string);
		return 1;
	}

	*((*pole_stringov) + *size - 1) = malloc(strlen(string) * sizeof(char));

	if ((temp = strchr(string, '\n')) != NULL)
		string[temp - string] = '\0';

	strcpy(*((*pole_stringov) + *size - 1), string);

	return 0;
}

/*rozdeli string podla tokenu na pole stringov*/
char **split(char *string, char *delim, int *pocet)
{
	char **result = NULL;
	char *temp, *temp2;
	temp2 = malloc(strlen(string) * sizeof(char));
	strcpy(temp2, string);
	temp = strtok(temp2, delim);

	while (temp != NULL)
	{
		pridaj_string_do_pola(&result, temp, pocet);
		temp = strtok(NULL, delim);
	}

	return result;
}

/*funckia pre nacitanie faktov a pravidiel zo suboru a ich vypis*/
int init()
{
	FILE *f;
	char buffer[100];
	char *temp;
	char c, prevc;
	int i, len, fakty = 0, pravidla = 0;
	Aplik = NULL;
	pocet_aplik = 0;

	if ((f = fopen("init.txt", "r")) == NULL)
	{
		printf("Failed to open file init\n");
		return 1;
	}

	//zistenie poctu pravidiel a faktov
	while ((c = fgetc(f)) != EOF)
	{
		if (c == '\n' && !pravidla)
			if (prevc == '\n')
				pravidla++;
			else
				fakty++;

		if (c == '\n' && pravidla)
			pravidla++;

		prevc = c;
	}

	pravidla /= 4;
	rewind(f);

	pocet_faktov = fakty;
	pocet_pravidiel = pravidla;
	pocet_aplik = 0;

	//printf("Pocet faktov:\t\t%2d \nPocet pravidiel:\t%2d\n\n", fakty, pravidla);

	PPam = malloc(fakty * sizeof(char*));
	BPrav = malloc(pravidla * sizeof(PRAVIDLO*));

	//inicializacia pola pravidiel
	for (len = 0; len < fakty; len++)
		PPam[len] = calloc(FAKTY_MAX_STRING, sizeof(char));
	for (len = 0; len < pravidla; len++)
	{
		BPrav[len] = malloc(sizeof(PRAVIDLO));
		BPrav[len]->meno = calloc(PRAVIDLA_MAX_STRING, sizeof(char));
		BPrav[len]->podmienky = NULL;
		BPrav[len]->akcie = NULL;
		BPrav[len]->pocet_podmienok = 0;
		BPrav[len]->pocet_akcii = 0;
	}

	//nacitanie faktov
	for (len = 0; len < fakty; len++)
	{
		fgets(buffer, FAKTY_MAX_STRING, f);
		strcpy(PPam[len], buffer);
		PPam[len][strlen(buffer) - 1] = '\0';
	}

	printf("FAKTY:\n\n");
	for (len = 0; len < fakty; len++)
		printf("%s\n", PPam[len]);

	c = fgetc(f);

	for (len = 0; len < pravidla; len++)
	{
		//pridanie mena
		fgets(buffer, PRAVIDLA_MAX_STRING, f);
		buffer[strlen(buffer) - 1] = '\0';
		strcpy(BPrav[len]->meno, buffer + 6);

		//pridavanie podmienok
		fgets(buffer, PRAVIDLA_MAX_STRING, f);

		temp = strtok(buffer + 6, ",");

		if (temp == NULL)
		{
			if (pridaj_string_do_pola(&(BPrav[len]->podmienky), buffer + 6, &(BPrav[len]->pocet_podmienok)))
			{
				printf("Pravidlo cislo %d\n", len + 1);
				return 1;
			}
		}
		while (temp != NULL)
		{
			pridaj_string_do_pola(&(BPrav[len]->podmienky), temp, &(BPrav[len]->pocet_podmienok));
			temp = strtok(NULL, ",");
		}
		
		//pridavanie akcii
		fgets(buffer, PRAVIDLA_MAX_STRING, f);

		temp = strtok(buffer + 6, ",");

		if (temp == NULL)
		{
			if (pridaj_string_do_pola(&(BPrav[len]->akcie), buffer + 6, &(BPrav[len]->pocet_akcii)))
			{
				printf("Pravidlo cislo %d\n", len + 1);
				return 1;
			}
		}
		while (temp != NULL)
		{
			pridaj_string_do_pola(&(BPrav[len]->akcie), temp, &(BPrav[len]->pocet_akcii));
			temp = strtok(NULL, ",");
		}


		fgets(buffer, PRAVIDLA_MAX_STRING, f);
	}

	//vypis
	printf("\nPRAVIDLA:\n\n");
	for (len = 0; len < pravidla; len++)
	{
		printf("Meno:\t%s\nAK\t", BPrav[len]->meno);
		for (i = 0; i < BPrav[len]->pocet_podmienok; i++)
			if (i == BPrav[len]->pocet_podmienok - 1)
				printf("%s\nPOTOM\t", BPrav[len]->podmienky[i]);
			else
				printf("%s,", BPrav[len]->podmienky[i]);

		for (i = 0; i < BPrav[len]->pocet_akcii; i++)
			if (i == BPrav[len]->pocet_akcii - 1)
				printf("%s\n\n", BPrav[len]->akcie[i]);
			else
				printf("%s,", BPrav[len]->akcie[i]);
	}

	return 0;
}

/*naviazanie premennych podla faktu*/
const char *premenne1(char *podmienka, char *fakt)
{
	int i, P1_size = 0, F1_size = 0;
	char **P1 = split(podmienka, " ", &P1_size);
	char **F1 = split(fakt, " ", &F1_size);
	char *nav;
	nav = malloc(1500*sizeof(char));
	strcpy(nav, "");

	if (P1_size != F1_size)
		return "f";
	else
	{
		for (i = 0; i < P1_size; i++) {
			if (strcmp(P1[i], F1[i]) != 0) {
				if (strstr(P1[i], "?") != NULL)
				{
					strcat(nav, P1[i]);
					strcat(nav, "=");
					strcat(nav, F1[i]);
					strcat(nav, " ");
				}
				else
				{
					return "f";
				}
			}
		}

		//nav[strlen(nav) - 1] = '\0';
		return nav;
	}
}
/*naviazanie premennych*/
const char **premenne(char *podmienka, int *size)
{
	int i, temp_size = 0;
	char **temp = NULL;
	char *pom = NULL;

	//naviazania el. podmienky na fakty
	for (i = 0; i < pocet_faktov; i++)
	{
		if (strcmp((pom = premenne1(podmienka, PPam[i])), "f") != 0)
			pridaj_string_do_pola(&temp, pom, &temp_size);
	}
	if (temp == NULL)
		return NULL;
	else 
	{
		*size = temp_size;
		return temp;
	}
}/*funkcia vrati f ak nie su stringy rovnake a t ak su*/const char *rovnaky_string(char *podmienka) 
{
	int i;
	char **t = malloc(sizeof(char*));
	t[0] = malloc(2 * sizeof(char));
	strcpy(t[0], "f");

	for (i = 0; i < pocet_faktov; i++) 
	{
		if (strcmp(PPam[i], podmienka) == 0) 
		{
			strcpy(t[0], "t");
			return t;
		}
	}

	return t;
}
/*funkcia na rozlisenie podmienky a mozne naviazanie premennych*/
char **zhoda(char *podmienka, int *size) 
{
	if (strstr(podmienka, "<>") != NULL)
	{
		*size = 1;
		char **s = malloc(sizeof(char*));
		s[0] = malloc(2 * sizeof(char));
		strcpy(s[0], "s");
		return s;
	}
	else
		if (strstr(podmienka, "?") == NULL)
		{
			*size = 1;
			return rovnaky_string(podmienka);
		}
		else
			return premenne(podmienka, size);
}/*zbavenie sa t a s v naviazaniach*/char ***cisti(char ***nav, int *size, int *sizes, int *temp_sizes) 
{
	char ***cnav = NULL;
	int i, n = 1;

	//printf("SIZE: %d",*size);

	for (i = 0; i < *size; i++) 
	{
		if ((strcmp(nav[i][0], "t") != 0) && (strcmp(nav[i][0], "s") != 0))
			if (cnav != NULL)
			{
				cnav = realloc(cnav, ++n * sizeof(char**));
				cnav[n - 1] = nav[i];
				*(temp_sizes + n - 1) = *(sizes + i);
			}
			else
			{
				cnav = malloc(sizeof(char**));
				cnav[n - 1] = nav[i];
				*(temp_sizes) = *(sizes + i);
			}
	}

	if (cnav == NULL)
		*size = 0;
	else
		*size = n;

	return cnav;
}/*funkcia na vytvorenie kombinacii moznych naviazani*/char **kombajn(char ***nav, int *size, int *sizes) 
{
	int i, j, k, komb_length = 0, komb2_length = 0;
	char **komb = NULL;
	char **komb2 = NULL;
	char *temp = malloc(200, sizeof(char));
	char **t = malloc(sizeof(char*));
	t[0] = malloc(2*sizeof(char));
	strcpy(t[0], "t");
	strcpy(temp, "");

	//printf("\nNEW PRAVIDLO\n");

	if (*size == 0)
		return t;
	else
		if (*size == 1)
			return nav[0];
		else
		{
			for (i = 0; i < sizes[0]; i++)
			{
				pridaj_string_do_pola(&komb, nav[0][i], &komb_length);
			}

			for (i = 1; i < *size; i++)
			{         
				for (j = 0; j < sizes[i]; j++)
				{     
					for (k = 0; k < komb_length; k++)
					{    
						strcpy(temp, komb[k]);
						pridaj_string_do_pola(&komb2, strcat(temp, nav[i][j]), &komb2_length);
						memset(temp, 0, 200);
					}
				}
				komb = komb2; 
				komb_length = komb2_length;

				komb2 = NULL;                                     
				komb2_length = 0;
			}

			*size = komb_length;

			/*
			for (k = 0; k < komb_length; k++)
			{       //pre kazdu doteraz vytvorenu kombinaciu
				printf("%s\n", komb[k]);
			}
			*/
			return komb;
		} //else
}/*funkcia na zistenie ci sa string nachadza v poli stringov*/int nie_je_v_poli(char *prvok, char **pole, int pole_length)
{
	int i;
	for (i = 0; i < pole_length; i++)
		if (strcmp(prvok, pole[i]) == 0)
			return 0;

	return 1;
}/*funkcia na zistenie ci sa string nenachadza v poli poli stringov*/int nie_je_v_poli_poli(char *prvok, char ***pole, int pole_pole_length, int *pole_lengths)
{
	int i, j;
	for (i = 0; i < pole_pole_length; i++)
		for(j = 0; j < pole_lengths[i]; j++)
			if (strcmp(prvok, pole[i][j]) == 0)
				return 0;

	return 1;
}/*necha iba tie naviazania ktore sedia*/char *spravny(char *knav)
{
	int i, n1_length = 0, n2_length = 0;
	char **n1 = NULL;
	char **n2 = NULL;
	char *nx = malloc(100 * sizeof(char));

	int P1_size = 0;
	char **P1 = split(knav, " ", &P1_size);

	for (i = 0; i < P1_size; i++)
	{
		if (nie_je_v_poli(P1[i], n1, n1_length))
			pridaj_string_do_pola(&n1, P1[i], &n1_length);
	}

	//printf("\n");
	for (i = 0; i < n1_length; i++)
	{
		//printf("%s\n",n1[i]);
		memset(nx, 0, 30);
		strcpy(nx, n1[i]);
		strtok(nx, "=");
		if (nie_je_v_poli(nx, n2, n2_length))
			pridaj_string_do_pola(&n2, nx, &n2_length);
	}

	

	if (n1_length == n2_length)
	{
		memset(nx, 0, 30);
		strcpy(nx, "");
		for (i = 0; i < n1_length; i++)
		{
			strcat(nx, n1[i]);
			strcat(nx, " ");
		}
		//printf("%s\n",nx);
		/*
		for (i = 0; i < n1_length; i++)
		{
			printf("%s\n", n1[i]);
		}
		*/
		return nx;         //pocet naviazani musi byt taky ako pocet premennych
	}
	else
		return NULL;
}/*funkcia vybera iba platne naviazania premennych*/char **cisti_kombajn(char **komb, int *size) 
{
	int i, k2_length = 0;
	char **pom;
	char **k2 = NULL;
	char **t = malloc(sizeof(char*));
	t[0] = malloc(2 * sizeof(char));
	strcpy(t[0], "t");
	/*
	for (i = 0; i < *size; i++)
	{      
		printf("%s\n", komb[i]);
	}
	*/
	if (strcmp(komb[0], "t") == 0)
		return t;                  
	else
	{
		for (i = 0; i < *size; i++)
		{
			if ((pom = spravny(komb[i])) != NULL) 
			{
				pridaj_string_do_pola(&k2, pom, &k2_length);
			}
		}

		*size = k2_length;

		if (k2_length == 0)
		{
			strcpy(t[0], "f");
			return t;            
		}
		else
			return k2;
	} //else
}/*nahradi premennu instanciou*/char *nahrad(char *navlist, char *slovo) 
{
	int i, navlist2_length = 0, pom_length = 0;
	char *preloz = NULL;
	char **pom = NULL;
	char **navlist2 = split(navlist, " ", &navlist2_length);

	if (strstr(slovo, "?") == NULL)
		return slovo;
	else
	{
		for (i = 0; i < navlist2_length; i++)
		{
			pom_length = 0;
			pom = split(navlist2[i], "=", &pom_length);
			if (strcmp(pom[0], slovo) == 0)
			{
				preloz = pom[1];
				break;
			}
		}

		return preloz;
	}
}/*funkcia naviaze instanciu premennej*/char *nav1(char *navlist, char **vzor, int vzor_length)
{
	int i, novyfakt_length = 0;
	char **novyfakt = NULL;
	char *novyfakt2 = malloc(30*sizeof(char));

	for (i = 0; i < vzor_length; i++)
	{
		pridaj_string_do_pola(&novyfakt, nahrad(navlist, vzor[i]), &novyfakt_length);
	}

	memset(novyfakt2, 0, 30);
	strcpy(novyfakt2, "");
	for (i = 0; i < novyfakt_length; i++)
	{
		strcat(novyfakt2, novyfakt[i]);
		strcat(novyfakt2, " ");
	}
	return novyfakt2;
}/*funkcia naviaze instancie premennych*/char **naviaz(char *navlist, char **vzorlist, int vzorlist_length, int *size)
{
	int i, vysl_length = 0, pocet;
	char **vysl = NULL;
	char **vzor = NULL;
	char *pom;

	for (i = 0; i < vzorlist_length; i++)
	{
		pocet = 0;
		vzor = split(vzorlist[i], " ", &pocet);
		pom = nav1(navlist, vzor, pocet);
		pridaj_string_do_pola(&vysl, pom, &vysl_length);
	}

	*size = vysl_length;

	return vysl;
}/*naviazanie instanci v pripade specialnych podmienok*/char **specialP(char **nav, char **podm, int *nav_length, int podm_length) 
{
	int i, j, podm1_length = 0, nav1_length = 0, npod_length = 0, npom_length = 0;
	char **podm1 = NULL;
	char **nav1 = NULL;
	char **npod;
	char **npom;
	char **t = malloc(sizeof(char*));
	t[0] = malloc(2 * sizeof(char));
	strcpy(t[0], "f");
	int test = 1;

	for (i = 0; i < podm_length; i++)
	{
		if (strstr(podm[i], "<>") != NULL)
			pridaj_string_do_pola(&podm1, podm[i], &podm1_length);
	}

	for (i = 0; i < *nav_length; i++)
	{             
		npod = naviaz(nav[i], podm1, podm1_length, &npod_length);
		test = 1;
		for (j = 0; j < npod_length; j++)
		{
			npom_length = 0;
			npom = split(npod[j], " ", &npom_length);
			if (strcmp(npom[1], npom[2]) == 0)
				test = 0;
		}
		if (test)
			pridaj_string_do_pola(&nav1, nav[i], &nav1_length);
	}

	*nav_length = nav1_length;

	if (nav1_length == 0)
		return t;
	else
		return nav1;
}/*funkcia na pridanie akcii pravidla do pola akcii pravidiel*/void pridaj_do_aplik(char **akcie, int akcie_length, char *meno) {	int i, x = 0;	if (Aplik == NULL)	{		Aplik = malloc(sizeof(APLIKPRAVIDLO*));		pocet_aplik = 1;		Aplik[0] = malloc(sizeof(APLIKPRAVIDLO));		Aplik[0]->akcie = akcie;		Aplik[0]->meno = meno;		Aplik[0]->pocet_akcii = akcie_length;	}	else
	{
		for (i = 0; i < pocet_aplik; i++)
		{
			if (strcmp(Aplik[i]->meno, meno) == 0)
				x = i+1;
		}

		if (x)
		{
			x--;
			for (i = 0; i < akcie_length; i++)
				pridaj_string_do_pola(&(Aplik[x]->akcie), akcie[i], &(Aplik[x]->pocet_akcii));
		}
		else
		{
			Aplik = realloc(Aplik, (++pocet_aplik) * sizeof(APLIKPRAVIDLO*));			Aplik[pocet_aplik - 1] = malloc(sizeof(APLIKPRAVIDLO));			Aplik[pocet_aplik - 1]->akcie = akcie;			Aplik[pocet_aplik - 1]->meno = meno;			Aplik[pocet_aplik - 1]->pocet_akcii = akcie_length;
		}

	}}/*funkcia na pridanie akcii pravidla do pola akcii pravidiel*/APLIKPRAVIDLO **pridaj_do_aplik2(APLIKPRAVIDLO **aplik_pole, APLIKPRAVIDLO *aplik, int *length){	int i, x = 0;	if (aplik_pole == NULL)	{		aplik_pole = malloc(sizeof(APLIKPRAVIDLO*));		*length = 1;		aplik_pole[0] = malloc(sizeof(APLIKPRAVIDLO));		aplik_pole[0]->akcie = aplik->akcie;		aplik_pole[0]->meno = aplik->meno;		aplik_pole[0]->pocet_akcii = aplik->pocet_akcii;		return aplik_pole;	}	else
	{
		for (i = 0; i < *length; i++)
		{
			if (strcmp(aplik_pole[i]->meno, aplik->meno) == 0)
				x = i+1;
		}

		if (x)
		{
			x--;
			for (i = 0; i < aplik->pocet_akcii; i++)
				pridaj_string_do_pola(&(aplik_pole[x]->akcie), aplik->akcie[i], &(aplik_pole[x]->pocet_akcii));
			return aplik_pole;
		}
		else
		{
			aplik_pole = realloc(aplik_pole, (++(*length)) * sizeof(APLIKPRAVIDLO*));			aplik_pole[(*length) - 1] = malloc(sizeof(APLIKPRAVIDLO));			aplik_pole[(*length) - 1]->akcie = aplik->akcie;			aplik_pole[(*length) - 1]->meno = aplik->meno;			aplik_pole[(*length) - 1]->pocet_akcii = aplik->pocet_akcii;

			return aplik_pole;
		}

	}}/*generovanie aplikovatelnych akcii (naviazanie akcii)*/void generuj(char *Meno, char **Akcie, char **Nav, int nav_length, int akcie_length)
{
	int i, nak_length = 0;
	char **Nav1;
	char **nak;

	if (strcmp(Nav[0], "t") == 0)
	{
		Nav1 = malloc(sizeof(char*));
		Nav1[0] = malloc(2 * sizeof(char));
		strcpy(Nav1[0], "t");
		nav_length = 1;
	}
	else
		Nav1 = Nav;

	for (i = 0; i < nav_length; i++)
	{
		nak = naviaz(Nav1[i], Akcie, akcie_length, &nak_length);
		pridaj_do_aplik(nak, nak_length, Meno);
	}
}/*funkcia na zistenie ci su akcie pouzitelne*/int mozne(APLIKPRAVIDLO *AkciePravidla)
{
	int i, j, pom_length = 0;
	char **pom = NULL;
	char *temp = malloc(100 * sizeof(char));
	int test = 0;

	for (i = 0; i < AkciePravidla->pocet_akcii; i++)
	{
		pom_length = 0;
		memset(temp, 0, 100);

		pom = split(AkciePravidla->akcie[i], " ", &pom_length);

		if (strcmp(pom[0], "pridaj") == 0)
		{
			strcpy(temp, "");
			for (j = 1; j < pom_length; j++) 
			{
				strcat(temp, pom[j]);
				strcat(temp, " ");
			}

			temp[strlen(temp) - 1] = '\0';

			if (nie_je_v_poli(temp, PPam, pocet_faktov))
			{
				test = 1;
				break;
			}
		}
		else
			if (strcmp(pom[0], "vymaz") == 0)
			{
				strcpy(temp, "");
				for (j = 1; j < pom_length; j++)
				{
					strcat(temp, pom[j]);
					strcat(temp, " ");
				}

				temp[strlen(temp) - 1] = '\0';

				if (!nie_je_v_poli(temp, PPam, pocet_faktov))
				{
					test = 1;
					break;
				}
			}
	}
	return test;
}/*funkcia na zbavenie sa nepotrebnych akcii*/void VyhodZbytocne()
{
	int i,j, apl_length = 0;
	APLIKPRAVIDLO **Apl = NULL;

	for (i = 0; i < pocet_aplik; i++)
	{
		if (mozne(Aplik[i]))
		{
			Apl = pridaj_do_aplik2(Apl, Aplik[i], &apl_length);
		}
	}

	pocet_aplik = apl_length;
	Aplik = Apl;
}
/*funkcia na vykonavanie akcii*/
void vykonaj_akcie() 
{
	int i, j,k, pom_length = 0, PP_length = 0;
	char **pom;
	char **PP;
	char *temp = malloc(50 * sizeof(char));

	for (i = 0; i < Aplik[0]->pocet_akcii; i++)
	{
		pom_length = 0;
		memset(temp, 0, 50);
		pom = split(Aplik[0]->akcie[i], " ", &pom_length);
		if (strcmp(pom[0], "pridaj") == 0)					//pridaj do faktov
		{
			strcpy(temp, "");
			for (j = 1; j < pom_length; j++)
			{
				strcat(temp, pom[j]);
				strcat(temp, " ");
			}

			temp[strlen(temp) - 1] = '\0';

			if (nie_je_v_poli(temp, PPam, pocet_faktov))
			{
				pridaj_string_do_pola(&PPam, temp, &pocet_faktov);
			}
		}
		else
			if (strcmp(pom[0], "vymaz") == 0)				//vymaz z faktov
			{
				strcpy(temp, "");
				for (j = 1; j < pom_length; j++)
				{
					strcat(temp, pom[j]);
					strcat(temp, " ");
				}

				temp[strlen(temp) - 1] = '\0';
				PP = NULL;
				PP_length = 0;

				for (j = 0; j < pocet_faktov; j++)
				{
					if (strcmp(temp, PPam[j]) != 0)
						pridaj_string_do_pola(&PP, PPam[j], &PP_length);
				}
				PPam = PP;
				pocet_faktov = PP_length;
			}
			else											//vypis spravu
			{
				strcpy(temp, "");
				for (j = 1; j < pom_length; j++)
				{
					strcat(temp, pom[j]);
					strcat(temp, " ");
				}

				temp[strlen(temp) - 1] = '\0';
				printf("SPRAVA: %s\n",temp);
			}
	}
}
/*vytvori aplikovatelne akcie z pravidiel*/
void najdi_aplik_instancie(PRAVIDLO *pravidlo) 
{
	char ***naviazania = NULL;
	char **naviazania2 = NULL;
	char **temp = NULL;
	int i, j, size, temp_size, podmienky = pravidlo->pocet_podmienok, naviazania_pole_size = 0;
	int *sizes, *temp_sizes;

	//printf("\nMENO: %s\npodmienky: %d\n", pravidlo->meno, podmienky);

	naviazania = malloc(podmienky * sizeof(char**));
	sizes = calloc(podmienky, sizeof(int));
	temp_sizes = calloc(podmienky, sizeof(int));

	for (i = 0; i < podmienky; i++)
	{
		naviazania[i] = zhoda(pravidlo->podmienky[i], &size);
		if (naviazania[i] == NULL)
			return NULL;
		sizes[i] = size;
	}

	temp_size = podmienky;

	naviazania2 = cisti_kombajn(kombajn(cisti(naviazania, &temp_size, sizes, temp_sizes), &temp_size, temp_sizes), &temp_size);
	
	if (strcmp(naviazania2[0], "f") != 0)
	{
		if (!nie_je_v_poli_poli("s", naviazania, podmienky, sizes))
			naviazania2 = specialP(naviazania2, pravidlo->podmienky, &temp_size, pravidlo->pocet_podmienok);    //osetri specialnu podmienku

		if (strcmp(naviazania2[0], "f") != 0)																	//ak daco ostalo, generuj akcie pravidla
			generuj(pravidlo->meno, pravidlo->akcie, naviazania2, temp_size, pravidlo->pocet_akcii);
		
	}
	/*
	for (i = 0; i < temp_size; i++)
		printf("%s\n", naviazania2[i]);
	*/

}

/*vytvor zoznam vsetkych aplikovatelnych instancii pravidiel*/
int aplik() 
{
	int i, j, naviazania_size;
	char **naviazania = NULL;

	//pre kazde pravidlo
	for (i = 0; i < pocet_pravidiel; i++) 
	{
		//najdi naviazania pre podmienky pravidla
		najdi_aplik_instancie(BPrav[i]);
	}

	return 0;
}

int main() {
	int i, j, poKrokoch = 0;
	char c;

	//krok 1
	init();

	printf("Krokovanie - 1\nDo konca - 2\n");
	scanf("%c", &c);
	if (c == '1')
		poKrokoch = 1;
	getchar();

	do 
	{
		Aplik = NULL;
		pocet_aplik = 0;

		//krok 2
		aplik();

		/*
		printf("APLIK pred vyhodenim zbytocnych\n\n");
		for (i = 0; i < pocet_aplik; i++)
		{
			printf("PRAVIDLO %d\n",i+1);
			for (j = 0; j < Aplik[i]->pocet_akcii; j++)
				printf("Pocet akcii - %d\n%s\n", Aplik[i]->pocet_akcii, Aplik[i]->akcie[j]);
			printf("\n");
		}
		*/

		//krok 3
		VyhodZbytocne();

		printf("\n\nAPLIKOVATELNE AKCIE\n\n");
		for (i = 0; i < pocet_aplik; i++)
		{
			for (j = 0; j < Aplik[i]->pocet_akcii; j++)
				printf("%s\n", Aplik[i]->akcie[j]);
			printf("\n");
		}

		if (poKrokoch) {
			printf("\nStlac ENTER pre pokracovanie ... ");
			getchar();
		}

		//krok 4
		if (pocet_aplik > 0)
			vykonaj_akcie();

	} while (pocet_aplik > 0);

	printf("\nKoniec\n\nStlac ENTER pre zobrazenie konecnych faktov ... ");
	getchar();

	for (i = 0; i < pocet_faktov; i++)
		printf("%s\n",PPam[i]);

	printf("\nPress ENTER to exit ... ");
	getchar();
	return 0;
}