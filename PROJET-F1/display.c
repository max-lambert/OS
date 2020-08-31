#include "display.h"

#define clear() printf("\033[H\033[J")

Session session;
Car copy[NOMBRE_VOITURES];

/**
 * Trie les voitures selon leur meilleur tour
 */
int triBestTempsTour(const void *a, const void *b){
  const Car *p1 = (Car *) a;
  const Car *p2 = (Car *) b;

  if(p1->bestTempsTour > p2->bestTempsTour){
    return 1;
  } else if(p1->bestTempsTour < p2->bestTempsTour){
    return -1;
  } else {
    return 0;
  }
}

/**
 * Récupère l'index de la voiture ayant le meilleur temps dans le secteur spécifié en paramètre dans le tableau copy
 * 
 * @param sectId : id du secteur pour lequel nous voulons avoir le meilleur temps
 * 
 * @return index : index de la voiture dans le tableau copy
 */
int indexBestOfSector(char sectId[]){
  int index = 0;
  double bestOfThis = 0;

  for(int i=0; i < session.nombreCar; i++){
    if(sectId == "S1"){
      if(bestOfThis == 0 || copy[i].bestS1 < bestOfThis){
        bestOfThis = copy[i].bestS1;
        index = i;
      }

    } else if(sectId == "S2"){
      if(bestOfThis == 0 || copy[i].bestS2 < bestOfThis){
        bestOfThis = copy[i].bestS2;
        index = i;
      }

    } else if(sectId == "S3"){
      if(bestOfThis == 0 || copy[i].bestS3 < bestOfThis){
        bestOfThis = copy[i].bestS3;
        index = i;
      }

    }
  }
  return index;
}

/**
 * Récupère l'index de la voiture ayant fait le meilleur tour dans le tableau copy
 * 
 * @return index : index de la voiture dans le tableau copy
 */
int indexBestOfLap(){
  int index = 0;
  double record = 0;

  for(int i=0; i < session.nombreCar; i++){
    if(record = 0 || copy[i].bestTempsTour < record){
      record = copy[i].bestTempsTour;
      index = i;
    }
  }

  return index;
}

/**
 * Affichage de la grille de course
 * Parcours le tableau copy 
 */
void print(){
  printf("|\tPos.\t|%10s\t|%10s\t|%10s\t|%10s\t|%10s\t|%10s\t|\t%s\t|\t%s\t|\n\n", "No", "S1", "S2", "S3", "TempsTour", "lap", "P", "O");

  for(int i=0; i<session.nombreCar; i++){
    Car pilote = copy[i];

    printf("|\t%d\t|%10d\t|%10.3f\t|%10.3f\t|%10.3f\t|%10.3f\t|\t%d\t|\t%d\t|\t%d\t|\n", 
    i+1, pilote.idCar, pilote.s1, pilote.s2, pilote.s3, pilote.tempsTour, pilote.lap, pilote.stand, pilote.out);
    
  }
  
  printf("\n\tBest S1: %d [%.3f]\t", copy[indexBestOfSector("S1")].idCar, copy[indexBestOfSector("S1")].bestS1);
  printf("Best S2: %d [%.3f]\t", copy[indexBestOfSector("S2")].idCar, copy[indexBestOfSector("S2")].bestS2);
  printf("Best S3: %d [%.3f]\t", copy[indexBestOfSector("S3")].idCar, copy[indexBestOfSector("S3")].bestS3);
  printf("Meilleur tour: %d [%.3f]\n",copy[indexBestOfLap()].idCar, copy[indexBestOfLap()].bestTempsTour);

  sleep(1); //sleep pour chaque refresh de la grille
  clear();
}

/**
 * Gestion centrale de l'affichage
 * 
 * @param data : pointeur sur la structure car
 * @param sema : pointeur sur la sémaphore
 */
void displayGrid(Car *data, sem_t *sema){

  while(1){
    sem_wait(sema); 
    memcpy(copy, data, sizeof(Car) * session.nombreCar); 
    /*Création d'une copie de la structure data.
      Elle est utilisée pour être modifié et trier les voitures selon leur temps de tour
      puisque l'on ne peut pas toucher à la structure originel étant donné que les processus la modifie toujours*/
    sem_post(sema);

    /**
     * Fonction spéciale, cf man 
     * 
     * En résume, on lui passe la structure de donénes à manipuler, le nombre d'éléments présent dans cette structre,
     * la taille de chaque élément de cette structure et enfin le nom de la fonction que nous avons créée
     * contenant l'algorithme de tri (sans param). Après elle fait le reste toute seule .... MAGIE !!!!! ^^
     */
    qsort(copy, session.nombreCar, sizeof(Car), triBestTempsTour);
    
    //Si la dernière voiture de la course a fini, on arrète le programme
    if (copy[session.nombreCar-1].fini){
      break;
    }
    print();
  }
  sleep(3); //Sleep pour fin de course afin de voir le tableau finale
  saveRanks(); //Sauvegarde du résultat de la course
}
