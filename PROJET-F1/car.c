#include "car.h"

int lapp = 0;
int tempsEnCourse = 0;
Car *voiture;
Session session;

/**
 * Chance pour que la voiture ait un crash
 * Si le nombre aléatoire généré vaut 1, la voiture est considérée hors course
 * 
 * @param proba : chance pour que la voiture se crash (1/proba)
 */
void crashproba(int proba){
  if(rand() % proba == 1){
    voiture->out = 1;
    exit(0);
  } 
}

/**
 * Pobabilité pour que la voiture aille au stand
 * Si le nombre généré vaut 1, la voiture passe au stand
 * 
 * @param proba : chance pour que la voiture se rendes au stand (1/proba)
 */
int standproba(int proba){
  return (rand() % proba);
}

/**
 * Gestion du tour des voitures
 * 
 * @param car : pointeur sur la structure car (notre voiture)
 * @param carNum : pointeur sur l'index correspondant du tableau numeroCar qui contient les numéros de toutes les voitures
 */
void carRun(Car *car, int *carNum){

  voiture = car;
  voiture->idCar = *carNum;
  
  srand(getpid()); 
  /*Utilisation du pid comme "graine" du générateur de nombre aléatoire.
    Il en faut une différente pour chaque voiture (processus) afin que tous les S1, S2 ou S3 n'ayent pas la même valeur. 
    Le pid du processus est le seul élément qui permet de les différenciés puisqu'ils sont tous les mêmes lors de leur création
    Pour plus d'infos, cfr. Gestion des processus, p.2.17 du syllabus UNIX 2*/

  // voiture tourne le temps de la session
  while (tempsEnCourse <= session.tempsSession) {
    
    //Gestion secteur 1
    voiture->s1 = generateTimeSector();
    if(voiture->bestS1 == 0 || voiture->bestS1 > voiture->s1){
      voiture->bestS1 = voiture->s1;
    }
    crashproba(10000);

    //Gestion secteur 2
    voiture->s2 = generateTimeSector();
    if(voiture->bestS2 == 0 || voiture->bestS2 > voiture->s2){ 
      voiture->bestS2 = voiture->s2; 
    }
    crashproba(10000);

    //Gestion secteur 3
    voiture->s3 = generateTimeSector();
    voiture->stand = 0;
    if(standproba(100) == 1){ //Si la voiture se rends au stand, elle est toujours considérée dans le secteur 3, sont temps est donc plus long
      voiture->s3 += tpsStand(1, 10);
      voiture->stand = 1;
    }
    if(voiture->bestS3 == 0 || voiture->bestS3 > voiture->s3) { 
      voiture->bestS3 = voiture->s3; 
    }
    crashproba(10000);

    usleep(800000); // pour désynchroniser les voitures afin d'éviter qu'elle passe (aux yeux du programme) la ligne d'arrivée toutes en même temps

    //Mise à jour des résultat du tour venant de se terminer
    voiture->tempsTour = voiture->s1 + voiture->s2 + voiture->s3;
    tempsEnCourse += voiture->tempsTour;
    if(voiture->bestTempsTour == 0 || voiture->bestTempsTour > voiture->tempsTour){
      voiture->bestTempsTour = voiture->tempsTour;
    }
    voiture->stand=0;

    voiture->lap++;
    lapp += voiture->lap;

    //Si la voiture a fini sa session, elle a fini et on en informe l'afficheur
    if(tempsEnCourse >= session.tempsSession){
      voiture->fini = 1;
    } else {
      voiture->fini = 0;
    }

    sleep(1);
  }
}
