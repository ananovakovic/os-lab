#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "signali.h"
#include "datoteke.h"

char *dat_stat, *dat_obrada, *dat_mreza;
int broj = 0;
int nije_kraj = 1;
int broj_dretvi = 1;
int postavljeni_broj_dretvi = 50;

pthread_mutex_t m;
pthread_cond_t red;

int broj;
int radi = 0;

void *obrada(void *);
void *mreza(void *);
void *dretva (void *p);


void *dretva (void *p)
{
	int id = *((int *) p);
	if (id < 1 || id > 9)
	return NULL;
	while(1) {
		pthread_mutex_lock (&m);
		while (radi || broj % id != 0)
			pthread_cond_wait (&red, &m);
		radi = id;
		pthread_mutex_unlock (&m);
		
		printf ("Dretva %d krece s radom (broj=%d)\n", id, broj);
		sleep(3);
		printf ("Dretva %d staje s radom\n", id);
		
		pthread_mutex_lock (&m);
		radi = 0;
		broj++;
		pthread_mutex_unlock (&m);
		pthread_cond_broadcast (&red);
		sleep(1);
		}
	return NULL;
}

int main(int argc, char *argv[])
{
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
	pthread_t radna, mrezna;

	if (pthread_create(&radna, NULL, &obrada, NULL)) {
		fprintf(stderr, "Ne mogu stvoriti novu dretvu!\n");
		exit(1);
	}
	if (pthread_create(&mrezna, NULL, &mreza, NULL)) {
		fprintf(stderr, "Ne mogu stvoriti novu dretvu!\n");
		exit(1);
	}

	/*while(nije_kraj) {
		scanf("%d", &broj);
		//ako je > 0 onda ga postavi kao broj
		if(broj > 0) 
			zapisi_status(broj);
		//inace prekidni s radom => postavi nije_kraj = 0
		else
			nije_kraj = 0;
	} */

	//cekaj na kraj dretve obrada, ali ne i one druge mrezne
	pthread_join(radna, NULL);

	pthread_t t[9];
	int i, id[9];

	pthread_mutex_init (&m, NULL);
	pthread_cond_init (&red, NULL);

	broj = 1;
	radi = 0;
	
	
	for (i = 0; i < 9; i++) {
	id[i] = i + 1;
	if(id[i] > postavljeni_broj_dretvi)
	{ 
		pthread_exit(NULL);
		int pthread_mutex_unlock(pthread_mutex_t *mutex);
	}
	pthread_create (&t[i], NULL, dretva, &id[i]);
	}

	for (i = 0; i < 9; i++)

	pthread_join (t[i], NULL);
	pthread_mutex_destroy (&m);
	pthread_cond_destroy (&red);
	return 0;


}

void *obrada(void *p)
{
	int i, x, broj, nije_kraj = 1;

	while(nije_kraj) {
		broj = procitaj_status();
		if (broj == 0) {
			printf("program bio prekinut, potraga za brojem...\n");
			broj = pronadji_zadnji_broj();
			broj = (int) sqrt(broj);
	}
	zapisi_status(0); //radim
	printf("krecem s radom, zadnji broj=%d\n", broj);

	for (i = 0; i < 5; i++) {
		broj++;
		x = broj * broj;
		dodaj_broj(x);
		sleep(5);
	}
	}
	zapisi_status(broj);
	printf("kraj rada, zadnji broj=%d\n", broj);
	
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
