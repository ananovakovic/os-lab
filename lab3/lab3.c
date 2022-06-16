#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "signali.h"
#include "datoteke.h"

char *dat_stat, *dat_obrada, *dat_mreza;
int broj;
int nije_kraj = 1;
int broj_dretvi = 1;
int postavljeni_broj_dretvi  = 50;
int radi  = 0;
int x, moj_broj;
pthread_mutex_t m;
pthread_cond_t red;

/* funkcije koje rade dretve */
void *obrada(void *);
void *mreza(void *);
void *kontrola_broja_dretvi(void *);
void *dretva (void *p);

int main(int argc, char *argv[])
{
	pthread_t t[9], mrezna;
	int i, id[9];
	
	
	if (argc < 4) {
		fprintf(stderr, "Koristenje: %s <status-datoteka> "
		 "<datoteka-s-podacima> <cjevovod>\n", argv[0]);
		exit(1);
	}
	dat_stat = argv[1];
	dat_obrada = argv[2];
	dat_mreza = argv[3];

	postavi_signale();
	scanf("%d" , &broj);
	zapisi_status(0);
	
	pthread_mutex_init(&m, NULL);
	pthread_cond_init(&red, NULL);
	radi = 0;
	
	/*if (pthread_create(&mrezna, NULL, mreza, NULL)) {
		fprintf(stderr, "Ne mogu stvoriti novu dretvu!\n");
		exit(1);
	}
	*/
	
	for (i = 0; i < 9; i++) {
		id[i] = i + 1;
		pthread_create (&t[i], NULL, dretva, &id[i]);
	}
	
	for (i = 0; i < 9; i++)
		pthread_join (t[i], NULL);


	while(nije_kraj) {
		scanf("%d", &broj);
		//ako je > 0 onda ga postavi kao broj
		if(broj > 0) 
			zapisi_status(broj);
		//inace prekidni s radom => postavi nije_kraj = 0
		else
			nije_kraj = 0;
	}

	//cekaj na kraj dretve obrada, ali ne i one druge mrezne

	pthread_mutex_destroy(&m);
	pthread_cond_destroy(&red);
	
	return 0;
}


void *kontrola_broja_dretvi(void *p) {
	pthread_t t[9];
	int id[9];
	while(1) {
		x = broj_dretvi;
		if(x > 1 && x < 100) {
			pthread_mutex_lock(&m);
			postavljeni_broj_dretvi = x;
		
				while(broj_dretvi < postavljeni_broj_dretvi) {
					pthread_create(&t[x], NULL, dretva, &id[x]);
					broj_dretvi++;
				}	
			pthread_mutex_unlock(&m);
		}
	}
	return NULL;
}

		

void *mreza(void *p)
{	
	int i, x, broj, nije_kraj = 1;
	broj = dohvati_iz_cijevi();
	zapisi_status(broj);

	//u petlji cekaj da se nesto pojavi u cijevi

	while(nije_kraj) {
		if (broj == 0) {
			printf("\n Broj u konzoli je 0...\n");
			printf("\n Završavam s radom...\n");
			nije_kraj=0;
		}
		
		//ako je procitano > 0 onda ga postavi u broj
		else {
			zapisi_status(broj); 
			printf("krecem s radom, zadnji broj=%d\n", broj);
		}
		for (i = 0; i < 5; i++) {
			broj++;
			x = broj * broj;
			dodaj_broj(x);
			sleep(5);
		}
	}
	zapisi_status(broj);
	printf("kraj rada, zadnji broj=%d\n", broj);
	
	broj = dohvati_iz_cijevi();	
	
	return NULL;
}




void *dretva (void *p)
{
	int id = *((int *) p);
	if (id < 1 || id > 9)
		return NULL;
	
	while(1) 
	{
		pthread_mutex_lock (&m);
		if (broj_dretvi > postavljeni_broj_dretvi){
			broj_dretvi--;
			pthread_mutex_unlock (&m);
			return NULL;
			}
		while (radi || broj % id != 0)
			pthread_cond_wait (&red, &m);

			radi = id;
			pthread_mutex_unlock (&m);
		
		x = broj*broj;
		dodaj_broj(x);
		broj++;
		zapisi_status(broj);
		printf ("Dretva %d krece s radom (broj=%d)\n", id, x);
		sleep(3);
		printf ("Dretva %d staje s radom\n", id);
		sleep(5);
		radi = 0;
		pthread_mutex_unlock(&m);
		pthread_cond_broadcast(&red);
		sleep(1);
	}
	return NULL;
}
