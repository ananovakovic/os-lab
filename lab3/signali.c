#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "signali.h"
char *dat_stat, *dat_obrada;
/* implementacija funkcija, vec opisanih u lab1 + postavi_signale() */
void postavi_signale() {
struct sigaction act;
act.sa_handler = obradi_dogadjaj;

sigemptyset(&act.sa_mask);
sigaddset(&act.sa_mask, SIGTERM);

act.sa_flags = 0;

sigaction(SIGUSR1, &act, NULL);

act.sa_handler = obradi_sigterm;
sigemptyset(&act.sa_mask);
sigaction(SIGTERM, &act, NULL);

act.sa_handler = obradi_sigint;
sigaction(SIGINT, &act, NULL);
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
	printf("Primio signal SIGTERM, pospremam prije izlaska iz programa\n");
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
