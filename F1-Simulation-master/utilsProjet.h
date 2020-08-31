//Variable globale qui s'incrémente à chaque secteur de chaque voiture afin d'avoir une variable en plus dans randomizer()
int compteurSecteur;

//Structure d'une voiture
struct voiture{
	int ID;
	double S1;
	double S2;
	double S3;
	double tour;
	double meilleurS1;
	double meilleurS2;
	double meilleurS3;
	double meilleurTour;
	int numeroTour;
	int estCrash;
	int estPit;
	double tempsEcoule;
}voiture[20];

//Fonction qui procure le random
int randomizer(int min,int max){
	int nombre_aleatoire = 0;
	srand(time(NULL) * (getpid()) * compteurSecteur);
	nombre_aleatoire = rand()%(max-min)+min;
	return nombre_aleatoire;
}

//Fonction pour générer le temps d'un secteur
double secteur(){
	if(!compteurSecteur){
		compteurSecteur = 0;
	}
	compteurSecteur++;
	usleep(15000);
	int min = 40000;
	int max = 50000;
	return ((double)randomizer(min,max)/(double)1000);
}

//Fonction qui génére l'arrêt aux stands
int pit(){
	if(randomizer(0,20) == 1) return 1;		
	return 0;
}

//Fonction qui génère le crash
int crash(){
	if(randomizer(0,500) == 1) return 1;		
	return 0;
}

//Fonction qui retourne le plus petit double
double min(double a,double b){
		if (a > b) return b;
		if (a < b) return a;
		return a;
}