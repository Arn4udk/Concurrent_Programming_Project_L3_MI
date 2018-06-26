#include "shared.h"

int appel(shared_data *ptr, int etageentree) {
  if (etageentree >= 0 && etageentree <= MAX_ETAGE) {
    sem_wait(&ptr->tableau);
    ptr->tabent[etageentree] = 1;
    sem_post(&ptr->tableau);
    return 1;
  } else {
    printf("Etage d'entrée non valide");
    return 0;
  }
}

int sortie(shared_data *ptr, int etagesortie) {
  if (etagesortie > 0 && etagesortie <= MAX_ETAGE) {
    sem_wait(&ptr->tableau);
    ptr->tabsor[etagesortie]++;
    sem_post(&ptr->tableau);
    return 1;
  } else {
    printf("Etage de sortie non valide");
    return 0;
  }
}

void affiche_appels(shared_data *ptr) {
  int i; // boucle de parcours

  yellow();
  printf("Called : ");
  reset();
  printf("[");
  for (i = 0; i < MAX_ETAGE; i++) {
    sem_wait(&ptr->tableau);
    if (ptr->tabent[i]) {
      printf("%d", i);
    } else {
      printf("_");
    }
    sem_post(&ptr->tableau);
  }
  printf("]\n");
}

void affiche_sorties(shared_data *ptr) {
  int i; // boucle de parcours

  yellow();
  printf("Exit   : ");
  reset();
  printf("[");
  for (i = 0; i < MAX_ETAGE; i++) {
    sem_wait(&ptr->tableau);
    if (ptr->tabsor[i]) {
      printf("%d", i);
    } else {
      printf("_");
    }
    sem_post(&ptr->tableau);
  }
  printf("]\n");
}

int check_portes(shared_data *ptr) {
  int value;

  sem_getvalue(&ptr->portes, &value);

  return value;
}

int check_tableau(shared_data *ptr) {
  int value;

  sem_getvalue(&ptr->tableau, &value);

  return value;
}

int check_mutex(shared_data *ptr) {
  int value;

  sem_getvalue(&ptr->mutex, &value);

  return value;
}

int etage_enc(shared_data *ptr) {
  int etage;

  sem_wait(&ptr->tableau);

  etage = ptr->var1;

  sem_post(&ptr->tableau);

  return etage;
}

void change_etage(shared_data *ptr, int etage) {
  sem_wait(&ptr->tableau);
  ptr->var1 = etage;
  sem_post(&ptr->tableau);
}

int check_sortie(shared_data *ptr) {

  sem_wait(&ptr->tableau);

  if (ptr->tabsor[ptr->var1] != 0) {
    sem_post(&ptr->tableau);
    return 1;
  }
  sem_post(&ptr->tableau);

  return 0;
}

int check_entree(shared_data *ptr) {

  sem_wait(&ptr->tableau);

  if (ptr->tabent[ptr->var1] != 0) {
    sem_post(&ptr->tableau);
    return 1;
  }
  sem_post(&ptr->tableau);

  return 0;
}

char *get_time_str() {
    time_t rawtime;
    time(&rawtime);
    memset(retour,'\0',sizeof(retour));
    struct tm *tm_struct = localtime(&rawtime);
    int hour = tm_struct->tm_hour;
    int min = tm_struct->tm_min;
    int sec = tm_struct->tm_sec;
    sprintf(retour,"%02d:%02d:%02d", hour, min, sec);
    return retour;
  }


void red()
{
  printf("\033[1;31m");
}

void green()
{
  printf("\033[1;32m");
}

void yellow()
{
  printf("\033[1;33m");
}

void reset () {
  printf("\033[0m");
}
