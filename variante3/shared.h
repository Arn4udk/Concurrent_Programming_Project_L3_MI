#ifndef SHARED_H
#define SHARED_H

#define SEM_PATH "/mutex" //partagé

#define MAX_ETAGE 10 //DEFINIT LE NOMBRE D'ETAGE /!/

#define PASSENGER 100

#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

typedef struct
{
    int var1; //un int pour les étages
    int var2; //nb de passagers
    int var3; //appel
    int tabent[MAX_ETAGE]; //tableau des appels (entrée)
    int tabsor[MAX_ETAGE];
    sem_t mutex; //les passagers
    sem_t portes; //sémaphore de la porte.
    sem_t tableau;
} shared_data; //notre structure de sémaphore

char retour[9]; //pour afficher les heures

int appel (shared_data *ptr,int etageentree);
int sortie(shared_data *ptr,int etagesortie);
void affiche_appels(shared_data *ptr);
void affiche_sorties(shared_data *ptr);
int check_portes(shared_data *ptr);
int check_mutex(shared_data *ptr);
int etage_enc(shared_data *ptr);
void change_etage(shared_data *ptr, int etage);
int check_sortie(shared_data *ptr);
int check_entree(shared_data *ptr);
char* get_time_str();
void red();
void green();
void yellow();
void reset ();

#endif
