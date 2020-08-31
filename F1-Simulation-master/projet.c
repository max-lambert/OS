#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <string.h>
#include <semaphore.h>
#include <sys/sem.h>
#include "utilsProjet.h"


#define ANSI_COLOR_RED     "\x1b[31m"	//Charactère d'échappement pour les couleurs de texte		source : https://en.wikipedia.org/wiki/ANSI_escape_code#Colors
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ASCII_ESC 27					//Charactère d'échappement pour le clear de la console 		source : http://www.climagic.org/mirrors/VT100_Escape_Codes.html

//Fonction de simulation d'étape
void Simulation(int nombreVoiture,int tri,double tempsCourse,struct voiture pilotes[20],int typeCourse){
	int i ;
	int shmId;
	int compteur;
	int semId;
	struct sembuf operation;
	int nbLecteur;
	key_t key = ftok("/dev/null", 7);
	struct voiture *circuit;
	// Initation de la memoire partagée et attachement de celle-ci
	shmId = shmget(key, 20*sizeof(struct voiture), IPC_CREAT|0666);
	if(shmId == -1){
		printf("shmId = -1 ");
		exit(1);
	}
	circuit = shmat(shmId,0,0);
	if(circuit == (struct voiture*)-1){
		printf("shmat = -1");
		exit(1);
	}

	/*
	* Partie semaphore: 
	* Initation
	* Fonction wait, signal, Lecture, 
	*/
	void initSem(){
		key_t semCle;
		semId = semget(semCle, 2, IPC_CREAT | 0666);
		if( semCle < 0 )
		{
			printf("Erreur semid\n");
			exit(0);
		}
		semctl(semId, 0, SETVAL, 1);
		semctl(semId, 1, SETVAL, 1);
	}
	void attendre(int i){
		operation.sem_num = i;
		operation.sem_op = -1;
		operation.sem_flg = SEM_UNDO;
		semop(semId, &operation, 1);
	}
	void signale(int i){
		operation.sem_num = i;
		operation.sem_op = 1;
		operation.sem_flg = SEM_UNDO;
		semop(semId, &operation, 1);
	}
	void commencerLecture(){
		attendre(0);
		nbLecteur++;
		if (nbLecteur == 1) {
			attendre(1);
		}
		signale(0);
	}
	void arreterLecture(){
		attendre(0);
		nbLecteur--;
		if (nbLecteur == 0) {
			signale(1);
		}
		signale(0);
	}
	initSem();
	void trier(int param ){
		struct voiture tempo; //[1]
		int a=0;
		int b=0;
		commencerLecture();
		memcpy(pilotes,circuit,nombreVoiture*sizeof(struct voiture));	
		arreterLecture();	
		if(param <= 0 ){	//trier voiture en fonction de leur meilleur tour
			for( a = 0; a < nombreVoiture ; a++){
				for( b = 0; b < nombreVoiture - 1 ; b++){
					if(pilotes[b].meilleurTour > pilotes[b+1].meilleurTour){
						tempo = pilotes[b+1];
						pilotes[b+1] = pilotes[b];
						pilotes[b] = tempo;
					}		
				}
			}
		}else{	//trier en fonction du numéro de tour
			for( a = 0; a < nombreVoiture ; a++){
				for( b = 0; b < nombreVoiture - 1; b++){
					if(pilotes[b].numeroTour < pilotes[b+1].numeroTour){
						tempo = pilotes[b+1];
						pilotes[b+1] = pilotes[b];
						pilotes[b] = tempo;
					}else if(pilotes[b].numeroTour == pilotes[b+1].numeroTour){
						if(pilotes[b].tour > pilotes[b+1].tour){
							tempo = pilotes[b+1];
							pilotes[b+1] = pilotes[b];
							pilotes[b] = tempo;
						}
					}				
				}	
			}
		}
	}
	void affichage(int trieur){
		trier(trieur);
		double meilleurS1 = 9999999.0;
		double meilleurS2 = 9999999.0;
		double meilleurS3 = 9999999.0;
		double meilleurTour = 99999.0;
		int j;
		int idS1;
		int idS2;
		int idS3;
		int idTour;
		printf("%c[2J", 27);	//Clear de la console
		printf("                     ╔════════════════════════════════════════════════════════════════════════════════════════════╗\n");
		switch(typeCourse)
		{
			case 1: printf("                     ║                        \033[01;36mSéance d'essais libres du vendredi matin | P1\033[0m                       ║\n");
				break;
			case 2 :printf("                     ║                     \033[01;36mSéance d'essais libres du vendredi après-midi | P2\033[0m                     ║\n");
				break;
			case 3 :printf("                     ║                         \033[01;36mSéance d'essais libres du samedi matin | P3\033[0m                        ║\n");
				break;
			case 4 :printf("                     ║                     \033[01;36mSéance de qualifications du samedi après-midi | Q1\033[0m                     ║\n");
				break;	
			case 5 :printf("                     ║                     \033[01;36mSéance de qualifications du samedi après-midi | Q2\033[0m                     ║\n");
				break;	
			case 6 :printf("                     ║                     \033[01;36mSéance de qualifications du samedi après-midi | Q3\033[0m                     ║\n");
				break;			
			case 7 :printf("                     ║                                \033[01;36mCourse du dimanche après-midi\033[0m                               ║\n");
				break;			
		}
		printf("╔════════╦═══════════╬═══════════╦═══════════╦══════════╦═════════════╦═════════════╦═════════════╦═══════════════╬══════╦═══════╦═════════╗\n");
		printf("║ Numéro ║ Secteur 1 ║ Secteur 2 ║ Secteur 3 ║   Tour   ║ Meilleur S1 ║ Meilleur S2 ║ Meilleur S3 ║ Meilleur Tour ║ Tour ║ Arrêt ║ Abandon ║\n");
		printf("╠════════╬═══════════╬═══════════╬═══════════╬══════════╬═════════════╬═════════════╬═════════════╬═══════════════╬══════╬═══════╬═════════╣\n");
		for(j = 0; j < nombreVoiture ; j++){
			if (pilotes[j].ID < 10){
				printf("║    %d   ",pilotes[j].ID);
			}else{printf("║   %d   ",pilotes[j].ID);}
			if (pilotes[j].S1 == 0){
				printf("║    ...    ");
			}else{printf("║   %.3f  ",pilotes[j].S1);}
			if (pilotes[j].S2 == 0){
				printf("║    ...    ");
			}else{printf("║   %.3f  ",pilotes[j].S2);}
			if (pilotes[j].S3 == 0){
				printf("║    ...    ");
			}else{printf("║   %.3f  ",pilotes[j].S3);}
			if (pilotes[j].tour < 120){
				printf("║    ...   ");
			}
			else if (pilotes[j].tour < 100.000){
				printf("║   %.3f ",pilotes[j].tour);
			}else{printf("║  %.3f ",pilotes[j].tour);}
			printf("║    %.3f   ",pilotes[j].meilleurS1);
			printf("║    %.3f   ",pilotes[j].meilleurS2);
			printf("║    %.3f   ",pilotes[j].meilleurS3);
			printf("║     %.3f   ",pilotes[j].meilleurTour);
			if (pilotes[j].numeroTour < 10){
				printf("║   %i  ",pilotes[j].numeroTour);
			}else{printf("║  %i  ",pilotes[j].numeroTour);}
			if(pilotes[j].estPit != 0){
				printf("║   %d   ",pilotes[j].estPit);
			}else{printf("║   0   ");}
			if(pilotes[j].estCrash == 1){
				printf("║    \033[01;31mX\033[0m    ║\n");
			}else{printf("║         ║\n");}			
		}
		printf("╚════════╩═══════════╩═══════════╩═══════════╩══════════╩═════════════╩═════════════╩═════════════╩═══════════════╩══════╩═══════╩═════════╝\n");
		for(j = 0; j<  nombreVoiture ; j++){
			if(meilleurS1 > pilotes[j].meilleurS1){
				idS1 = pilotes[j].ID;
				meilleurS1 = pilotes[j].meilleurS1;
			}
			if(meilleurS2 > pilotes[j].meilleurS2){
				idS2= pilotes[j].ID;
				meilleurS2 = pilotes[j].meilleurS2;
			}
			if(meilleurS3 > pilotes[j].meilleurS3){
				idS3 = pilotes[j].ID;
				meilleurS3 = pilotes[j].meilleurS3;
			}
			if(meilleurTour > pilotes[j].meilleurTour){
				idTour = pilotes[j].ID;
				meilleurTour = pilotes[j].meilleurTour;
			}
		}
		printf("╔════════════════════╦════╦═══════════╗\n");
		if (idS1 < 10){
			printf("║ Meilleur Secteur 1 ║  %d ║   \033[01;33m%.3f\033[0m  ║\n",idS1,meilleurS1);
		}else{printf("║ Meilleur Secteur 1 ║ %d ║   \033[01;33m%.3f\033[0m  ║\n",idS1,meilleurS1);}
		if (idS2 < 10){
			printf("║ Meilleur Secteur 2 ║  %d ║   \033[01;33m%.3f\033[0m  ║\n",idS2,meilleurS2);
		}else{printf("║ Meilleur Secteur 2 ║ %d ║   \033[01;33m%.3f\033[0m  ║\n",idS2,meilleurS2);}
		if (idS3 < 10){
			printf("║ Meilleur Secteur 3 ║  %d ║   \033[01;33m%.3f\033[0m  ║\n",idS3,meilleurS3);
		}else{printf("║ Meilleur Secteur 3 ║ %d ║   \033[01;33m%.3f\033[0m  ║\n",idS3,meilleurS3);}
		if (idTour < 10){
			printf("║    Meilleur Tour   ║  %d ║  \033[01;33m%.3f\033[0m  ║\n",idTour,meilleurTour);
		}else{printf("║    Meilleur Tour   ║ %d ║  \033[01;33m%.3f\033[0m  ║\n",idTour,meilleurTour);}
		printf("╚════════════════════╩════╩═══════════╝\n");
	}
	for(i=0;i < nombreVoiture;i++){
		if(fork() == 0){
			circuit = shmat(shmId,0,0);
			if(circuit == (struct voiture*)-1){
				printf("shmat = -1");
				exit(1);
			}
			circuit[i].tempsEcoule = 0;
			circuit[i].ID =	pilotes[i].ID;
			circuit[i].meilleurS1 = 99;
			circuit[i].meilleurS2 = 99;
			circuit[i].meilleurS3 = 99;
			circuit[i].meilleurTour = 999;
			circuit[i].numeroTour = 0;
			circuit[i].estCrash = 0;
			circuit[i].estPit = 0;
			while(circuit[i].estCrash == 0 && circuit[i].tempsEcoule < tempsCourse){
				circuit[i].tour = 0;
				if(crash() == 1){
					circuit[i].estCrash = 1;
					circuit[i].S1 = 0;
					circuit[i].S2 = 0;
					circuit[i].S3 = 0;
					exit(0);
				}else{
					attendre(1);
					circuit[i].S1 = secteur();
					circuit[i].tempsEcoule += circuit[i].S1;
					circuit[i].tour += circuit[i].S1; 
					circuit[i].meilleurS1 = min(circuit[i].meilleurS1,circuit[i].S1);
					signale(1);
					if(crash() == 1){
						circuit[i].estCrash = 1;
						circuit[i].S2 = 0;
						circuit[i].S3 = 0;
						exit(0);
					}else{
						attendre(1);
						circuit[i].S2 = secteur();
						circuit[i].tempsEcoule += circuit[i].S2;
						circuit[i].tour += circuit[i].S2;
						circuit[i].meilleurS2 = min(circuit[i].meilleurS2,circuit[i].S2); 
						signale(1);
						if(pit() == 1){
							circuit[i].estPit += 1;
							usleep(600000);
							attendre(1);
							circuit[i].S3 = ((double)randomizer(20000,25000)/(double)1000) + secteur();
							signale(1);
						}else{
							attendre(1);
							circuit[i].S3 = secteur();
							signale(1);					
						}
						if(crash() == 1){
							circuit[i].estCrash = 1;
							circuit[i].S3 = 0;
							exit(0);
						}else{
							attendre(1);
							circuit[i].tour += circuit[i].S3; 
							circuit[i].tempsEcoule += circuit[i].S3;
							circuit[i].meilleurS3 = min(circuit[i].meilleurS3,circuit[i].S3);
							circuit[i].meilleurTour = min(circuit[i].meilleurTour,circuit[i].tour);
							circuit[i].numeroTour++;
							signale(1);
							usleep(300000);
						}
					}
				}
			}
			exit(0);
		}
	}
	for(compteur = 0; compteur< ((int)tempsCourse/120*3); compteur++){
		affichage(tri);
		usleep(400000);
	}
	if(shmdt(circuit) == -1) {
    	printf("shmdt");
     	exit(1);
 	}	
}

//Fonction pour demander si oui ou non l'étape suivante doit être simulée.
int PromptEtape(int etape){
	char choixPrompt;
	printf("%c[2J", 27);	//Clear de la console
	switch(etape){
		case 1 : printf("Voulez-vous simuler les essais libres ?\nY pour Oui\nN pour Non \n");	
			break;
		case 2 : printf("Voulez-vous simuler les essais qualificatifs ?\nY pour Oui\nN pour Non \n");
			break;
		case 3 : printf("Voulez-vous simuler la course ?\nY pour Oui\nN pour Non \n");		
			break;			
		}
	scanf(" %c", &choixPrompt);	//Entrée de la réponse par l'utilisateur (Y ou N)		
	printf("%c[2J", 27);	//Clear de la console
	if (choixPrompt == 'Y' || choixPrompt == 'y'){	
		switch(etape)
		{
		case 1 : printf("\r\033[01;33mLancement des essais libres dans quelques instants\033[0m");	
			break;
		case 2 : printf("\r\033[01;33mLancement des essais qualificatifs dans quelques instants\033[0m");
			break;
		case 3 : printf("\r\033[01;33mLancement de la course dans quelques instants\033[0m");
			break;			
		}
		fflush(stdout);	//Permet de vider le buffer du printf, sinon les sleeps s'enchaineraient et les printf se feraient tous d'un coup à la fin. | source : https://stackoverflow.com/questions/13568388/c-sleep-function-not-working
		usleep(1000000);
		printf("\033[01;33m.\033[0m");
		fflush(stdout);
		usleep(1000000);
		printf("\033[01;33m.\033[0m");
		fflush(stdout);
		usleep(1000000);
		printf("\033[01;33m.\033[0m");
		fflush(stdout);
		usleep(1000000);
		printf( "%c[2K", ASCII_ESC );
		switch(etape)
		{
		case 1 : printf("\r\033[01;32mLancement des essais libres !\033[0m\n");
			break;
		case 2 : printf("\r\033[01;32mLancement des essais qualificatifs !\033[0m\n");
			break;
		case 3 : printf("\r\033[01;32mLancement de la course !\033[0m\n");
			break;			
		}
		return 1;
	} 
	else if (choixPrompt == 'N' || choixPrompt == 'n'){	//Si la réponse est Non
		switch(etape)
		{
		case 1 : printf("\033[01;31mEssais libres annulés.\033[0m\n");
			break;
		case 2 : printf("\033[01;31mEssais qualificatifs annulés,\033[0m\n\033[01;33mla grille de départ sera par défaut en cas de lancement de la course.\033[0m\n");
			break;
		case 3 : printf("\033[01;31mCourse annulée.\033[0m\n");
			break;			
		}
		usleep(5000000);
		return 0;
	}
	else {
		printf("\033[01;31mERREUR : Votre réponse n'est pas valide.\n");
		switch(etape)
		{
		case 1 : printf("\033[01;31mEssais libres annulés.\033[0m\n");
			break;
		case 2 : printf("\033[01;31mEssais qualificatifs annulés, la grille de départ sera par défaut en cas de lancement de la ourse.\033[0m\n");
			break;
		case 3 : printf("\033[01;31mCourse annulée.\033[0m\n");
			break;			
		}
		usleep(5000000);  
		return 0;  
	}
}

//Affichage grille des pilotes
void grille(struct voiture automobile[20],int choix){
	printf("%c[2J", 27);	//Clear de la console
	if (choix == 1) {
		printf("╔═══════════════════════════════════════════╗\n║              \033[01;36mLigne de départ\033[0m              ║\n╠════╦════════════════════╦═════════════════╣\n");
	}
	else {printf("╔═══════════════════════════════════════════╗\n║           \033[01;36mRésultat de la course\033[0m           ║\n╠════╦════════════════════╦═════════════════╣\n");}
	for(int i =0; i<20;i++){
		if (i < 9){
			printf("║  %i ",(i+1));
		}else{printf("║ %i ",(i+1));}
		switch(automobile[i].ID){
			case 3 : printf("║ Daniel Ricciardo   ║ Renault F1 Team ║\n");
				break;
			case 4 : printf("║ Lando Norris       ║ McLaren         ║\n");
				break;
			case 5 : printf("║ Sebastian Vettel   ║ Ferrari         ║\n");
				break;
			case 7 : printf("║ Kimi Räikkönen     ║ Sauber          ║\n");
				break;
			case 8 : printf("║ Romain Grosjean    ║ Haas F1 Team    ║\n");
				break;
			case 10 : printf("║ Pierre Gasly       ║ Red Bull Racing ║\n");
				break;
			case 11 : printf("║ Sergio Pérez       ║ Racing Point    ║\n");
				break;
			case 16 : printf("║ Charles Leclerc    ║ Ferrari         ║\n");
				break;
			case 18 : printf("║ Lance Stroll       ║ Racing Point    ║\n");
				break;
			case 20 : printf("║ Kevin Magnussen	  ║ Haas F1 Team    ║\n");
				break;
			case 23 : printf("║ Alexander Albon	  ║ Toro Rosso      ║\n");
				break;
			case 26 : printf("║ Daniil Kvyat       ║ Toro Rosso      ║\n");
				break;
			case 27 : printf("║ Nico Hülkenberg    ║ Renault F1 Team ║\n");
				break;
			case 33 : printf("║ Max Verstappen     ║ Red Bull Racing ║\n");
				break;
			case 44 : printf("║ Lewis Hamilton     ║ Mercedes        ║\n");
				break;
			case 55 : printf("║ Carlos Sainz Jr.   ║ McLaren         ║\n");
				break;
			case 63 : printf("║ George Russell     ║ Williams        ║\n");
				break;
			case 77 : printf("║ Valtteri Bottas    ║ Mercedes        ║\n");
				break;
			case 88 : printf("║ Robert Kubica      ║ Williams        ║\n");
				break;
			case 99 : printf("║ Antonio Giovinazzi ║ Sauber          ║\n");
				break;
		}
		if (i == 19){
			printf("╚════╩════════════════════╩═════════════════╝\n");
		}else{printf("╠════╬════════════════════╬═════════════════╣\n");} 
	}
}

//Création/Ecriture du fichier de resultat de l'étape
int creationFichier(int etape, int nbrVoiture,struct voiture pilotes[20]){
	FILE* fichier = NULL;
	switch (etape){									//Choix du fichier selon l'étape
		case 1 : fichier = fopen("P1.txt", "w+");
			break;
		case 2 : fichier = fopen("P2.txt", "w+");
			break;
		case 3 : fichier = fopen("P3.txt", "w+");
			break;
		case 4 : fichier = fopen("Q1.txt", "w+");
			break;
		case 5 : fichier = fopen("Q2.txt", "w+");
			break;
		case 6 : fichier = fopen("Q3.txt", "w+");
			break;
		case 7 : fichier = fopen("Course.txt", "w+");
			break;
	}
	if (fichier != NULL){
	double meilleurS1 = 9999999.0;
	double meilleurS2 = 9999999.0;
	double meilleurS3 = 9999999.0;
	double meilleurTour = 99999.0;
	int j;
	int idS1;
	int idS2;
	int idS3;
	int idTour;
	fprintf(fichier,"                     ╔════════════════════════════════════════════════════════════════════════════════════════════╗\r\n");
	switch(etape)
	{
		case 1: fprintf(fichier,"                     ║                        Séance d'essais libres du vendredi matin | P1                       ║\r\n");
			break;
		case 2 :fprintf(fichier,"                     ║                     Séance d'essais libres du vendredi après-midi | P2                     ║\r\n");
			break;
		case 3 :fprintf(fichier,"                     ║                         Séance d'essais libres du samedi matin | P3                        ║\r\n");
			break;
		case 4 :fprintf(fichier,"                     ║                     Séance de qualifications du samedi après-midi | Q1                     ║\r\n");
			break;	
		case 5 :fprintf(fichier,"                     ║                     Séance de qualifications du samedi après-midi | Q2                     ║\r\n");
			break;	
		case 6 :fprintf(fichier,"                     ║                     Séance de qualifications du samedi après-midi | Q3                     ║\r\n");
			break;			
		case 7 :fprintf(fichier,"                     ║                                Course du dimanche après-midi                               ║\r\n");
			break;			
	}
	fprintf(fichier,"╔════════╦═══════════╬═══════════╦═══════════╦══════════╦═════════════╦═════════════╦═════════════╦═══════════════╬══════╦═══════╦═════════╗\r\n");
	fprintf(fichier,"║ Numéro ║ Secteur 1 ║ Secteur 2 ║ Secteur 3 ║   Tour   ║ Meilleur S1 ║ Meilleur S2 ║ Meilleur S3 ║ Meilleur Tour ║ Tour ║ Arrêt ║ Abandon ║\r\n");
	fprintf(fichier,"╠════════╬═══════════╬═══════════╬═══════════╬══════════╬═════════════╬═════════════╬═════════════╬═══════════════╬══════╬═══════╬═════════╣\r\n");
	for(j = 0; j < nbrVoiture ; j++){
		if (pilotes[j].ID < 10){
			fprintf(fichier,"║    %d   ",pilotes[j].ID);
		}else{fprintf(fichier,"║   %d   ",pilotes[j].ID);}
		if (pilotes[j].S1 == 0){
			fprintf(fichier,"║    ...    ");
		}else{fprintf(fichier,"║   %.3f  ",pilotes[j].S1);}
		if (pilotes[j].S2 == 0){
			fprintf(fichier,"║    ...    ");
		}else{fprintf(fichier,"║   %.3f  ",pilotes[j].S2);}
		if (pilotes[j].S3 == 0){
			fprintf(fichier,"║    ...    ");
		}else{fprintf(fichier,"║   %.3f  ",pilotes[j].S3);}
		if (pilotes[j].tour == 0){
			fprintf(fichier,"║    ...   ");
		}
		else if (pilotes[j].tour < 100.000){
			fprintf(fichier,"║   %.3f ",pilotes[j].tour);
		}else{fprintf(fichier,"║  %.3f ",pilotes[j].tour);}
		fprintf(fichier,"║    %.3f   ",pilotes[j].meilleurS1);
		fprintf(fichier,"║    %.3f   ",pilotes[j].meilleurS2);
		fprintf(fichier,"║    %.3f   ",pilotes[j].meilleurS3);
		fprintf(fichier,"║     %.3f   ",pilotes[j].meilleurTour);
		if (pilotes[j].numeroTour < 10){
			fprintf(fichier,"║   %i  ",pilotes[j].numeroTour);
		}else{fprintf(fichier,"║  %i  ",pilotes[j].numeroTour);}
		if(pilotes[j].estPit != 0){
			fprintf(fichier,"║   %d   ",pilotes[j].estPit);
		}else{fprintf(fichier,"║   0   ");}
		if(pilotes[j].estCrash == 1){
			fprintf(fichier,"║    X    ║\r\n");
		}else{fprintf(fichier,"║         ║\r\n");}			
	}
	fprintf(fichier,"╚════════╩═══════════╩═══════════╩═══════════╩══════════╩═════════════╩═════════════╩═════════════╩═══════════════╩══════╩═══════╩═════════╝\n");
	fclose(fichier);
 	}
	return 0;
}

//Fonction principale
int main (void){
	printf("\033[01;35mInitialisation de la simulation\033[0m");
	fflush(stdout);	
	sleep(1);
	printf("\033[01;35m.\033[0m");
	fflush(stdout);
	sleep(1);
	printf("\033[01;35m.\033[0m");
	fflush(stdout);
	sleep(1);
	printf("\033[01;35m.\033[0m");
	fflush(stdout);
	sleep(1);
	int ID[20] = {3,4,5,7,8,10,11,16,18,20,23,26,27,33,44,55,63,77,88,99};
	int tempsUnTour = 130;
	int nombreTourCourseFinale = 45;
	struct voiture tri[20];
	for(int i = 0; i < 20 ; i++){
		tri[i].ID = ID[i];
	}	
	int reponsePromptEssai = PromptEtape(1);
	if (reponsePromptEssai == 1){	
		sleep(5);
		Simulation(20,-1,5400,tri,1); //Essais libres - P1
		creationFichier(1,20,tri);
		sleep(5);
		Simulation(20,-1,5400,tri,2); //Essais libres - P2
		creationFichier(2,20,tri);
		sleep(5);
		Simulation(20,-1,3600,tri,3); //Essais libres - P3
		creationFichier(3,20,tri);
		sleep(5);
	}
	int reponsePromptQualif = PromptEtape(2);
	if (reponsePromptQualif == 1){
		sleep(5);
		Simulation(20,-1,1080,tri,4); //Qualification - Q1
		creationFichier(4,20,tri);
		sleep(5);
		Simulation(15,-1,900,tri,5);  //Qualification - Q2
		creationFichier(5,15,tri);
		sleep(5);
		Simulation(10,-1,720,tri,6);  //Qualification - Q3
		creationFichier(6,10,tri);
		sleep(5);
	}
	int reponsePromptCourse = PromptEtape(3);
	if (reponsePromptCourse == 1){
		sleep(5);
		grille(tri,1);
		sleep(10);
		Simulation(20,1,(tempsUnTour*nombreTourCourseFinale),tri,7);  //Course
		creationFichier(7,20,tri);
		sleep(5);
	}
	printf("%c[2J", 27);	//Clear de la console
	if (reponsePromptCourse == 1) {grille(tri,0);}	
	printf("\033[01;35mSimulation terminée\033[0m\n");
}