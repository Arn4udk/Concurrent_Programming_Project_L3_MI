#include "shared.h"

volatile sig_atomic_t stop; // la variable pour la boucle infinie

void inthand(int signum) {
  green();
  printf("\n[Lift]");
  reset();
  printf(" You called end of program. Landing to floor #0\n\n");
  stop = 1;
} // la fct signal handler pour l'arret de l'asce

int main(void) {

  //------------DATA DECLARATION-----------------------------------------------

  int i, j;               // boucle pour les étages (up et down)
  shared_data *ptr;       // pointeur de la zone partagée
  size_t shared_seg_size; // taille de la zone partagée
  int shmfd;              // fd de la zone paragée

  //------------FUNCTIONS------------------------------------------------------

  // MEMOIRE PARTAGEE ET INITIALISATION DES SEM ET ASCEN

  signal(SIGINT, inthand); // le signal handler

  shared_seg_size = sizeof(shared_data);

  shmfd = shm_open(SEM_PATH, O_CREAT | O_RDWR, 0666); // le fd de la zone.
  if (shmfd == -1) {
    int errsv = errno;
    printf("erreur de shm_open, arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }

  // troncature de la zone mém avant mmap
  if (ftruncate(shmfd, shared_seg_size) == -1) {
    int errsv = errno;
    printf("erreur de mapping mémoire, arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }

  // mapping mémoire
  ptr = (shared_data *)mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE,
                            MAP_SHARED, shmfd, 0);
  if (ptr == MAP_FAILED) {
    int errsv = errno;
    printf("erreur de mapping mémoire(mmap), arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }

  // Initialisation de l'ascenseur.
  ptr->var1 = 0; // ascenceur à l'étage 0.
  ptr->var2 = 0; // 0 passagers ici
  ptr->var3 = 0; // 0 appels ici

  for (i = 0; i < MAX_ETAGE; i++) {
    ptr->tabent[i] = 0;
    ptr->tabsor[i] = 0;
  }

  if (sem_init(&ptr->mutex, 1, PASSENGER) ==
      -1) // création de la sémaphore pour les passagers
  {
    int errsv = errno;
    printf("erreur de sem_init mutex, arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }

  // création de la sémaphore pour les
  // portes (1 binaire ouverture
  // /fermeture)
  if (sem_init(&ptr->portes, 1, 1) == -1) {
    int errsv = errno;
    printf("erreur de sem_init portes, arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }

  // création de la sémaphore pour le
  // tableau (1 binaire ouverture
  // /fermeture)
  if (sem_init(&ptr->tableau, 1, 1) == -1) {
    int errsv = errno;
    printf("erreur de sem_init tableau, arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }

  green();
  printf("\n[Lift]");
  reset();
  printf(" Lift initialized and starts (#floors=%d)\n", MAX_ETAGE);

  // PARTIE SUR LA ROTATION DES ETAGES

  if (sem_wait(&ptr->portes) == -1) // fermeture des portes
  {
    int errsv = errno;
    printf("erreur de sem_init portes, arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }

  while (!stop) // la boucle des étages
  {
    for (i = 0; i < MAX_ETAGE; i++) // montée
    {

      change_etage(ptr, i);

      affiche_appels(ptr);
      affiche_sorties(ptr);
      green();
      printf("[Lift]");
      reset();
      printf(" <%s> == floor : %d\n", get_time_str(), etage_enc(ptr));

      if (check_entree(ptr)) // si on veut entrer
      {
        if (sem_wait(&ptr->tableau) == -1) {
          int errsv = errno;
          printf("erreur de sem_wait tableau, arrêt du programme\n");
          printf("l'erreur : %s\n", strerror(errsv));
          exit(EXIT_FAILURE);
        }

        ptr->tabent[i] = 0; // reset de la case d'appel pour cet étage

        if (sem_post(&ptr->tableau) == -1) {
          int errsv = errno;
          printf("erreur de sem_post tableau, arrêt du programme\n");
          printf("l'erreur : %s\n", strerror(errsv));
          exit(EXIT_FAILURE);
        } // libération du tableau

        if (sem_post(&ptr->portes) == -1) {
          int errsv = errno;
          printf("erreur de sem_post portes, arrêt du programme\n");
          printf("l'erreur : %s\n", strerror(errsv));
          exit(EXIT_FAILURE);
        } // transit -> ouverture des portes

        green();
        printf("[Lift]");
        reset();
        printf(" Doors open [floor #%d]\n", etage_enc(ptr));

        if(sem_wait(&ptr->portes) == -1)
        {
          int errsv = errno;
          printf("erreur de sem_wait portes, arrêt du programme\n");
          printf("l'erreur : %s\n", strerror(errsv));
          exit(EXIT_FAILURE);
          } // fermeture des portes

        green();
        printf("[Lift]");
        reset();
        printf(" Doors close [floor #%d]\n", etage_enc(ptr));
        green();
        printf("[Lift]");
        reset();
        printf(" Moves floor %d -> floor %d\n", i, i + 1);
        sleep(1);
      } else if (check_sortie(ptr)) { // si on veut sortir

        if (sem_wait(&ptr->tableau) == -1) {
          int errsv = errno;
          printf("erreur de sem_wait tableau, arrêt du programme\n");
          printf("l'erreur : %s\n", strerror(errsv));
          exit(EXIT_FAILURE);
        }

        ptr->tabsor[i] = 0;
        // reset de la case d'appel des sorties pour cet étage

        if (sem_post(&ptr->tableau) == -1) {
          int errsv = errno;
          printf("erreur de sem_post tableau, arrêt du programme\n");
          printf("l'erreur : %s\n", strerror(errsv));
          exit(EXIT_FAILURE);
        }

        if (sem_post(&ptr->portes) == -1) {
          int errsv = errno;
          printf("erreur de sem_post portes, arrêt du programme\n");
          printf("l'erreur : %s\n", strerror(errsv));
          exit(EXIT_FAILURE);
        } // ouverture des portes

        green();
        printf("[Lift]");
        reset();
        printf(" Doors open [floor #%d]\n", etage_enc(ptr));

        if (sem_wait(&ptr->portes) == -1) {
          int errsv = errno;
          printf("erreur de sem_wait portes, arrêt du programme\n");
          printf("l'erreur : %s\n", strerror(errsv));
          exit(EXIT_FAILURE);
        } // fermeture des portes

        green();
        printf("[Lift]");
        reset();
        printf(" Doors close [floor #%d]\n", etage_enc(ptr));
        green();
        printf("[Lift]");
        reset();
        printf(" Moves floor %d -> floor %d\n", i, i + 1);
        sleep(1);
      } else { // sinon rien
        green();
        printf("[Lift]");
        reset();
        printf(" Moves floor %d -> floor %d\n", i, i + 1);
        sleep(1);
      }
    } // fermeture du for de montée

    for (j = MAX_ETAGE ; j > 0; j--) { // descente

      change_etage(ptr, j);

      affiche_appels(ptr);
      affiche_sorties(ptr);
      green();
      printf("[Lift]");
      reset();
      printf(" <%s> == floor : %d\n", get_time_str(), etage_enc(ptr));

      if (check_entree(ptr)) { // si on veut entrer
        if (sem_wait(&ptr->tableau) == -1) {
          int errsv = errno;
          printf("erreur de sem_wait tableau, arrêt du programme\n");
          printf("l'erreur : %s\n", strerror(errsv));
          exit(EXIT_FAILURE);
        }

        ptr->tabent[j] = 0; // reset du tab d'appel pour cet étage

        if (sem_post(&ptr->tableau) == -1) {
          int errsv = errno;
          printf("erreur de sem_post tableau, arrêt du programme\n");
          printf("l'erreur : %s\n", strerror(errsv));
          exit(EXIT_FAILURE);
        }

        if (sem_post(&ptr->portes) == -1) // transit ouverture des portes
        {
          int errsv = errno;
          printf("erreur de sem_post portes, arrêt du programme\n");
          printf("l'erreur : %s\n", strerror(errsv));
          exit(EXIT_FAILURE);
        }

        green();
        printf("[Lift]");
        reset();
        printf(" Doors open [floor #%d]\n", etage_enc(ptr));

        if (sem_wait(&ptr->portes) == -1) // fermeture des portes
        {
          int errsv = errno;
          printf("erreur de sem_wait portes, arrêt du programme\n");
          printf("l'erreur : %s\n", strerror(errsv));
          exit(EXIT_FAILURE);
        }

        green();
        printf("[Lift]");
        reset();
        printf(" Doors close [floor #%d]\n", etage_enc(ptr));
        green();
        printf("[Lift]");
        reset();
        printf(" Moves floor %d -> floor %d\n", j, j - 1);
        sleep(1);

      } else if (check_sortie(ptr)) { // si on veut sortir

        if (sem_wait(&ptr->tableau) == -1) {
          int errsv = errno;
          printf("erreur de sem_wait tableau, arrêt du programme\n");
          printf("l'erreur : %s\n", strerror(errsv));
          exit(EXIT_FAILURE);
        }

        ptr->tabsor[j] = 0; // reset du tab d'appel des sorties

        if (sem_post(&ptr->tableau) == -1) {
          int errsv = errno;
          printf("erreur de sem_post tableau, arrêt du programme\n");
          printf("l'erreur : %s\n", strerror(errsv));
          exit(EXIT_FAILURE);
        }

        if (sem_post(&ptr->portes) == -1) {
          int errsv = errno;
          printf("erreur de sem_post portes, arrêt du programme\n");
          printf("l'erreur : %s\n", strerror(errsv));
          exit(EXIT_FAILURE);
        }

        green();
        printf("[Lift]");
        reset();
        printf(" Doors open [floor #%d]\n", etage_enc(ptr));

        if (sem_wait(&ptr->portes) == -1) {
          int errsv = errno;
          printf("erreur de sem_wait portes, arrêt du programme\n");
          printf("l'erreur : %s\n", strerror(errsv));
          exit(EXIT_FAILURE);
        }

        green();
        printf("[Lift]");
        reset();
        printf(" Doors close [floor #%d]\n", etage_enc(ptr));
        green();
        printf("[Lift]");
        reset();
        printf(" Moves floor %d -> floor %d\n", j, j - 1);
        sleep(1);

      } else { // sinon rien
        green();
        printf("[Lift]");
        reset();
        printf(" Moves floor %d -> floor %d\n", j, j - 1);
        sleep(1);
      }
    } // stop for descente
  }   // stop while étages

/*  if (sem_close(&ptr->portes) == -1) {
    int errsv = errno;
    printf("erreur de sem_close portes, arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }

  if (sem_close(&ptr->tableau) == -1) {
    int errsv = errno;
    printf("erreur de sem_close tableau, arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }

  if (sem_close(&ptr->mutex) == -1) {
    int errsv = errno;
    printf("erreur de sem_close mutex, arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  } */

  /*if (sem_unlink(SEM_PATH) == -1) {
    int errsv = errno;
    printf("erreur de sem_unlink, arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  }

  if (shm_unlink(SEM_PATH) == -1) {
    int errsv = errno;
    printf("erreur de shm_unlink, arrêt du programme\n");
    printf("l'erreur : %s\n", strerror(errsv));
    exit(EXIT_FAILURE);
  } */

  green();
  printf("\n[Lift] ");
  reset();
  printf("Exiting safely. Thank you.\n");

  return 0;
}
