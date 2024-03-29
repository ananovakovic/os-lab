
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

char *dat_stat, *dat_obrada;
int broj = 0;

/* funkcije za rad s datotekama */
int procitaj_status();
void zapisi_status(int broj);
void dodaj_broj(int broj);
int pronadji_zadnji_broj();

void obradi_dogadjaj(int sig);
void obradi_sigterm(int sig);
void obradi_sigint(int sig);

int nije_kraj = 1;

int main(int argc, char *argv[])
{
	int i, x;
	if (argc < 3) {
		printf("Koristenje: %s status.txt obrada.txt",
			argv[0]);
		return 1;
	}
	dat_stat = argv[1];
	dat_obrada = argv[2];

	/* simulacija rada */
	/* pocetak */
	broj = procitaj_status();
	if (broj == 0) {
		printf("program bio prekinut, potraga za brojem...\n");
		broj = pronadji_zadnji_broj();
		broj = (int) sqrt(broj);
	}
	zapisi_status(0); //radim
	printf("krecem s radom, zadnji broj=%d\n", broj);

	//simulacija obrade
	for (i = 0; i < 5; i++) {
		broj++;
		x = broj * broj;
		dodaj_broj(x);
		sleep(5);
	}

	//kraj
	zapisi_status(broj);
	printf("kraj rada, zadnji broj=%d\n", broj);

	return 0;
}

int procitaj_status()
{
	int broj;
	FILE *fp;

	fp = fopen(dat_stat, "r");
	if (!fp) {
		printf("Ne mogu otvoriti %s\n", dat_stat);
		exit(1);
	}
	if (fscanf(fp, "%d", &broj) != 1) {
		printf("Nije procitan broj iz %s!\n", dat_stat);
		exit(1);
	}
	fclose(fp);
	return broj;
}

void zapisi_status(int broj)
{
	FILE *fp;

	fp = fopen(dat_stat, "w");
	if (!fp) {
		printf("Ne mogu otvoriti %s\n", dat_stat);
		exit(1);
	}
	if (fprintf(fp, "%d\n", broj) < 1) {
		printf("Nije upisan broj u %s!\n", dat_stat);
		exit(1);
	}
	fclose(fp);
}

void dodaj_broj(int broj)
{
	FILE *fp;

	fp = fopen(dat_obrada, "a");
	if (!fp) {
		printf("Ne mogu otvoriti %s\n",dat_obrada);
		exit(1);
	}
	if (fprintf(fp, "%d\n", broj) < 1) {
		printf("Nije upisan broj u %s!\n", dat_obrada);
		exit(1);
	}
	fclose(fp);
}

int pronadji_zadnji_broj()
{
	FILE *fp;
	int broj = -1;

	fp = fopen(dat_obrada, "r");
	if (!fp) {
		printf("Ne mogu otvoriti %s\n",dat_obrada);
		exit(1);
	}
	while(!feof(fp))
		if (fscanf(fp, "%d", &broj) != 1)
			break;
	fclose(fp);
	return broj;
}


void obradi_dogadjaj(int sig)
{
	int i;
	printf("Pocetak obrade signala %d\n", sig);
	for (i = 1; i <= 5; i++) {
		printf("Obrada signala %d: %d/5\n", sig, i);
		sleep(1);
	}
	printf("Kraj obrade signala %d\n", sig);
}

void obradi_sigterm(int sig)
{
	printf("Primio signal SIGTERM, pospremam prije izlazska iz programa\n");
	FILE *fp;

        fp = fopen(dat_stat, "a");
        if (!fp) {
                printf("Ne mogu otvoriti %s\n",dat_stat);
                exit(1);
        }
        if (fprintf(fp, "%d\n", broj) > 1) {
                printf("Nije upisan broj u %s!\n", dat_stat);
                exit(1);
        }
        fclose(fp);

	nije_kraj = 0;
}

void obradi_sigint(int sig)
{
	printf("Primio signal SIGINT, prekidam rad\n");
	exit(1);
}
