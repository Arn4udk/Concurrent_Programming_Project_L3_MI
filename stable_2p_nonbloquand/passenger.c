#include "shared.h"

int main(int argc, char *argv[]) {

  if (argc != 4) {
    printf("Erreur d'arguments\n");
    exit(EXIT_FAILURE);
  }

  //------------DATA DECLARATION-----------------------------------------------

  shared_data *ptr;       // pour la mém mappée
  int value = 0;          // témoin de porte
  size_t shared_seg_size; // pour la mém mappée
  int shmfd;
  int eent = atoi(argv[2]);
  int esor = atoi(argv[3]);
  char nom = argv[1][0]; // on récup le nom du passager

  //------------FUNCTIONS------------------------------------------------------

  // MEMOIRE PARTAGEE ET INITIALISATION DES SEM ET ASCEN

  shared_seg_size = sizeof(shared_data);

  shmfd = shm_open(SEM_PATH, O_RDWR, 0666); // le fd de la zone.

  ptr = (shared_data *)mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE,
                            MAP_SHARED, shmfd, 0);

  if (ptr == MAP_FAILED) {
    int errsv = errno;
    printf("erreur de mapping mémoire(mmap), arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }

  // PROCEDURE POUR RENTRER

  red();
  printf("[Passenger %c]", nom);
  reset();
  printf(" <%s> : wants floor %d -> floor %d\n", get_time_str(), eent, esor);
  red();
  printf("[Passenger %c]", nom);
  reset();
  printf(" <%s> : sees lift at floor %d\n", get_time_str(), etage_enc(ptr));

  if (sem_wait(&ptr->mutex) == -1) // check nb passagers
  {
    int errsv = errno;
    printf("erreur de sem_wait(passa), arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }

  if (!appel(ptr, eent)) { // appel de l'asc
    printf("Erreur dans l'appel passager ! \n");
  }

  red();
  printf("[Passenger %c]", nom);
  reset();
  printf(" <%s> : wait for lift to open doors on floor %d\n", get_time_str(),
  eent);

  sleep(1);

  // while d'attente pour entrer
  while (!value && etage_enc(ptr) != eent) {
    value = check_portes(ptr);
    sleep(1);
  }

  red();
  printf("[Passenger %c]", nom);
  reset();
  printf(" <%s> Ok lift doors open floor %d\n", get_time_str(), etage_enc(ptr));
  red();
  printf("[Passenger %c]", nom);
  reset();
  printf(" <%s> : Enters lift at floor %d.  Press dst button (%d)\n",
  get_time_str(), etage_enc(ptr), esor);

  if (!sortie(ptr, esor)) { // appel pour sortir
    printf("Erreur dans l'appel passager sortie ! \n");
  }

  value = 0; // reset du test des portes pour sortir

  red();
  printf("[Passenger %c]", nom);
  reset();
  printf(" <%s> : wait for lift to open doors on floor %d.\n", get_time_str(),
  esor);

  sleep(1);

  // while d'attente pour sortir
  while (!value && etage_enc(ptr) != esor) {
    value = check_portes(ptr);
    sleep(1);
  }

  red();
  printf("[Passenger %c]", nom);
  reset();
  printf(" <%s> : Leaves lift on floor %d.\n", get_time_str(), etage_enc(ptr));

  sem_post(&ptr->mutex); // sortie du passager

  if (munmap(ptr, sizeof(shared_data)) == -1) {
    int errsv = errno;
    printf("erreur de mapping mémoire(mUNmap), arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }

  if (close(shmfd) == -1) {
    int errsv = errno;
    printf("erreur de mapping mémoire(close), arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }

  return 0;
}
