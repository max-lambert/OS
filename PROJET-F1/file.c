#include "file.h"

Session session;
Car copy[NOMBRE_VOITURES];

/**
 * Fonction d'écriture de fichier
 * 
 * Sauvegarde le classement dans un fichier
 * Nous n'avons besoin de sauvegarder que le numéro de la voiture et comme ils sont écrit dans l'ordre d'arriver
 * il suffit de lire le fichier depuis le début pour créer la grille de départ de la session suivante.
 */
void saveRanks(){
    FILE* file = fopen(session.fileName, "w");

    if(file != NULL){
        printf("Sauvegarde dans le fichier\n");

    	for (int i = 0; i < session.nombreCar; i++) { 
    		fprintf(file, "%d\n", copy[i].idCar);
    	}  

        fclose(file);
    }

    if(!strcmp(session.fileName, "Q1.txt") || !strcmp(session.fileName, "Q2.txt") || !strcmp(session.fileName, "Q3.txt")){
        makeFinalRaceGrid();
    }
}

/**
 * Fonction d'écriture de fichier
 * 
 * Va ajouter les 5 dernières voitures de la session au début du fichier 
 * Racegrid.txt qui sera lu au début de la course finale pour former la grille de 
 * départ de cette dernière.
 */
void makeFinalRaceGrid(){ 
    FILE* RCGridtxt = fopen("RaceGrid.txt", "a");
    int nbCarToAdd = 0;

    if(!strcmp(session.fileName, "Q1.txt") || !strcmp(session.fileName, "Q2.txt")){
        nbCarToAdd = 5;
    } else {
        nbCarToAdd = 10;
    }

    if(RCGridtxt != NULL){
        printf("Sauvegarde du classement pour course finale\n");

        for(int i=session.nombreCar-1; i>(session.nombreCar-1)-nbCarToAdd ;i--){    //Ajouter les voiture de la session en commençant par la fin
            fprintf(RCGridtxt, "%d\n", copy[i].idCar);
        }

        fclose(RCGridtxt);
    }
}

/**
 * Fonction de lecture de fichier
 * 
 * Va lire le fichier générer par la session précédente pour former le nouveau 
 * tableau numeroCar avec les voiture de la session précédentes
 * 
 * Exemple: Si l'on lance Q3, on va lire le fichier généré par Q2 (Q2.txt) et 
 * former la grille de Q3 à partir de ce dernier fichier.
 * 
 * Comme on lit le fichier depuis le début et qu'on connait déja la limite du nombre
 * de voiture pour la session lors du lancement du programme, il n'y a pas besoin de 
 * manipuler le fichier (juste lire les X premières ligne).
 * 
 * @param numeroCar : tableau contenant tous les numéros des voitures
 * @param previousSess : nom du fichier de la session précédente
 */
void makeCurrentGrid(int numeroCar[], char previousSess[]){
    numeroCar[session.nombreCar]; //On vide le tableau
    FILE* fileToRead = fopen(previousSess, "r");
    char chaine[5] = "";
    int i;

    if(fileToRead != NULL){
        if(!strcmp(session.fileName, "RaceGrid.txt")){ //Lecture spéciale de RaceGrid.txt
            rewind(fileToRead);
            i=session.nombreCar-1;

            while(fgets(chaine, 5, fileToRead)){
                numeroCar[i] = atoi(chaine);
                i--;
            }

            fclose(fileToRead);

        } else { //Lecture classique des autres fichiers
            rewind(fileToRead);

            while(fgets(chaine, 5, fileToRead) != NULL){
            
                numeroCar[i] = atoi(chaine);
                printf("%d\n", numeroCar[i]);
                i++;
            
                if(i == session.nombreCar){
                    break;
                }
            } 

            fclose(fileToRead);

        }
    }
}
