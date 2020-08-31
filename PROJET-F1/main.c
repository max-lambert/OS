#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include "car.h"
#include "display.h"

Car *car;
Session session;
int numeroCar [NOMBRE_VOITURES] = {7, 99, 5, 16, 8, 20, 4, 55, 10, 26, 44, 77, 11, 18, 23, 33, 3, 27, 63, 88};

/**
 * Paramètre la session en fonction des paramètres de lancement du programme
 * 
 * @param argv : tableau contenant les arguments de lancement du programme
 */
void paramSession(char *argv[]){
  
  if(!strcmp(argv[1], "P1") || !strcmp(argv[1], "P2")){
    sprintf(session.fileName, "%s.txt",argv[1]);
    session.nombreCar = 20;
    session.tempsSession = 5400;

  } else if(!strcmp(argv[1], "P3")) {
    sprintf(session.fileName, "%s.txt",argv[1]);
    session.nombreCar = 20;
    session.tempsSession = 3600;

  } else if(!strcmp(argv[1], "Q1")) {
    sprintf(session.fileName, "%s.txt",argv[1]);
    session.nombreCar = 20;
    session.tempsSession = 1080;

  } else if(!strcmp(argv[1], "Q2")) {
    sprintf(session.fileName, "%s.txt",argv[1]);
    session.nombreCar = 15;
    session.tempsSession = 900;
    makeCurrentGrid(numeroCar, "Q1.txt");

  } else if(!strcmp(argv[1], "Q3")) {
    sprintf(session.fileName, "%s.txt",argv[1]);
    session.nombreCar = 10;
    session.tempsSession = 720;
    makeCurrentGrid(numeroCar, "Q2.txt");

  } else if(!strcmp(argv[1], "Race")) {
    sprintf(session.fileName, "%s.txt",argv[1]);
    session.nombreCar = 20;
    session.tempsSession = 7200;
    makeCurrentGrid(numeroCar, "RaceGrid.txt");

  } else {
    printf("Argument non valide ! Veuillez utiliser P1, P2, P3, Q1, Q2, Q3 ou Race");
  }
}

int main (int argc, char *argv[]){
  //Paramétrage de la session
  paramSession(argv);

  //Allocation d'un segment de mémoire partagé quelconque 
  int shmid = shmget(IPC_PRIVATE, sizeof(Car) * session.nombreCar, 0600 | IPC_CREAT);
  if(shmid == -1){
    printf("shmget failed\n");
    exit(EXIT_FAILURE);
  }

  //Attachement du segment de la mémoire partagé à structure car
  car = shmat(shmid, NULL, 0);
  if(car == (void *) (-1)){
    printf("shmat failed\n");
    exit(EXIT_FAILURE);
  }

  //On crée un sémaphore pour la mémoire partagée dont le flag correpond au flag 0600 (plus besoin de créér une nouvelle mémoire partagée)
  int sem_shmid = shmget(IPC_PRIVATE, sizeof(sem_t), 0600 | IPC_CREAT);
  if(sem_shmid == -1){
    printf("shmget failed\n");
    exit(EXIT_FAILURE);
  }

  //Attachement du sémaphore au segment de mémoire partagée
  sem_t *sema = shmat(sem_shmid, NULL, 0);
  if(sema == (void *)(-1)){
    printf("shmat failed");
    exit(EXIT_FAILURE);
  }

  //Initialisation d'un nouveau sémaphore sur sema
  sem_init(sema, 1, 1);

  //Creation d'un nouveau process fils (la voiture)
  int i =0;
  pid_t pid;
  for(i; i<session.nombreCar; i++){
    pid = fork();
    if (pid == 0){
      break;
    }
  }
  switch (pid) {
    case -1:
      perror("fork");
      exit(EXIT_FAILURE);
    case 0://child aka car
      carRun(&car[i], &numeroCar[i]);
      exit(0);
    default: //parent aka afficheur
      displayGrid(car, sema);
      for(int j=0; j<session.nombreCar; j++){
        wait(NULL);
      }
  }

  //Detachement du segment de la mémoire partagée
  shmdt(car);

  //Effectue l'action spécifié par la cmd (ici IPC_RMID qui marque le segment à détruire)
  shmctl(shmid, IPC_RMID, NULL);

  return EXIT_SUCCESS;
}
