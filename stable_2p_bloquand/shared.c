#include "shared.h"

int appel(shared_data *ptr, int etageentree) {
  if (etageentree >= 0 && etageentree <= MAX_ETAGE) {
    if (sem_wait(&ptr->tableau) == -1) {
      int errsv = errno;
      printf("erreur de sem_wait tableau, arrêt du programme\n");
      printf("l'erreur : %s\n", strerror(errsv));
      exit(EXIT_FAILURE);
    }

    ptr->tabent[etageentree] = 1;

    if (sem_post(&ptr->tableau) == -1) {
      int errsv = errno;
      printf("erreur de sem_post tableau, arrêt du programme\n");
      printf("l'erreur : %s\n", strerror(errsv));
      exit(EXIT_FAILURE);
    }

    return 1;

  } else {
    printf("Etage d'entrée non valide\n");
    exit(EXIT_FAILURE);
    return 0;
  }
}

int sortie(shared_data *ptr, int etagesortie) {
  if (etagesortie > 0 && etagesortie <= MAX_ETAGE) {
    if (sem_wait(&ptr->tableau) == -1) {
      int errsv = errno;
      printf("erreur de sem_wait tableau, arrêt du programme\n");
      printf("l'erreur : %s\n", strerror(errsv));
      exit(EXIT_FAILURE);
    }

    ptr->tabsor[etagesortie]++;

    if (sem_post(&ptr->tableau) == -1) {
      int errsv = errno;
      printf("erreur de sem_post tableau, arrêt du programme\n");
      printf("l'erreur : %s\n", strerror(errsv));
      exit(EXIT_FAILURE);
    }

    return 1;

  } else {
    printf("Etage de sortie non valide\n");
    exit(EXIT_FAILURE);
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
    if (sem_wait(&ptr->tableau) == -1) {
      int errsv = errno;
      printf("erreur de sem_wait tableau, arrêt du programme\n");
      printf("l'erreur : %s\n", strerror(errsv));
      exit(EXIT_FAILURE);
    }

    if (ptr->tabent[i]) {
      printf("%d", i);
    } else {
      printf("_");
    }

    if (sem_post(&ptr->tableau) == -1) {
      int errsv = errno;
      printf("erreur de sem_post tableau, arrêt du programme\n");
      printf("l'erreur : %s\n", strerror(errsv));
      exit(EXIT_FAILURE);
    }
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
    if (sem_wait(&ptr->tableau) == -1) {
      int errsv = errno;
      printf("erreur de sem_wait tableau, arrêt du programme\n");
      printf("l'erreur : %s\n", strerror(errsv));
      exit(EXIT_FAILURE);
    }

    if (ptr->tabsor[i]) {
      printf("%d", i);
    } else {
      printf("_");
    }

    if (sem_post(&ptr->tableau) == -1) {
      int errsv = errno;
      printf("erreur de sem_post tableau, arrêt du programme\n");
      printf("l'erreur : %s\n", strerror(errsv));
      exit(EXIT_FAILURE);
    }
  }

  printf("]\n");
}

int check_portes(shared_data *ptr) {

  int value;

  if (sem_getvalue(&ptr->portes, &value) == -1) {
    int errsv = errno;
    printf("erreur de sem_getvalue portes, arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }

  return value;
}

int check_tableau(shared_data *ptr) {

  int value;

  if (sem_getvalue(&ptr->tableau, &value) == -1) {
    int errsv = errno;
    printf("erreur de sem_getvalue tableau, arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }

  return value;
}

int check_mutex(shared_data *ptr) {

  int value;

  if (sem_getvalue(&ptr->mutex, &value) == -1) {
    int errsv = errno;
    printf("erreur de sem_getvalue mutex, arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }

  return value;
}

int etage_enc(shared_data *ptr) {

  int etage;

  if (sem_wait(&ptr->tableau) == -1) {
    int errsv = errno;
    printf("erreur de sem_wait tableau, arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }

  etage = ptr->var1;

  if (sem_post(&ptr->tableau) == -1) {
    int errsv = errno;
    printf("erreur de sem_post tableau, arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }

  return etage;
}

void change_etage(shared_data *ptr, int etage) {

  if (sem_wait(&ptr->tableau) == -1) {
    int errsv = errno;
    printf("erreur de sem_wait tableau, arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }

  ptr->var1 = etage;

  if (sem_post(&ptr->tableau) == -1) {
    int errsv = errno;
    printf("erreur de sem_post tableau, arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }
}

int check_sortie(shared_data *ptr) {

  if (sem_wait(&ptr->tableau) == -1) {
    int errsv = errno;
    printf("erreur de sem_wait tableau, arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }

  if (ptr->tabsor[ptr->var1] != 0) {

    if (sem_post(&ptr->tableau) == -1) {
      int errsv = errno;
      printf("erreur de sem_post tableau, arrêt du programme\n");
      printf("l'erreur : %s\n", strerror(errsv));
      exit(EXIT_FAILURE);
    }

    return 1;
  }

  if (sem_post(&ptr->tableau) == -1) {
    int errsv = errno;
    printf("erreur de sem_post tableau, arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }

  return 0;
}

int check_entree(shared_data *ptr) {

  if (sem_wait(&ptr->tableau) == -1) {
    int errsv = errno;
    printf("erreur de sem_wait tableau, arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }

  if (ptr->tabent[ptr->var1] != 0) {
    if (sem_post(&ptr->tableau) == -1) {
      int errsv = errno;
      printf("erreur de sem_post tableau, arrêt du programme\n");
      printf("l'erreur : %s\n", strerror(errsv));
      exit(EXIT_FAILURE);
    }

    return 1;
  }

  if (sem_post(&ptr->tableau) == -1) {
    int errsv = errno;
    printf("erreur de sem_post tableau, arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }

  return 0;
}

char *get_time_str() {

  time_t rawtime;
  time(&rawtime);

  memset(retour, '\0', sizeof(retour));

  struct tm *tm_struct = localtime(&rawtime);

  int hour = tm_struct->tm_hour;
  int min = tm_struct->tm_min;
  int sec = tm_struct->tm_sec;
  sprintf(retour, "%02d:%02d:%02d", hour, min, sec);

  return retour;
}

void red() { printf("\033[1;31m"); }

void green() { printf("\033[1;32m"); }

void yellow() { printf("\033[1;33m"); }

void reset() { printf("\033[0m"); }
