#include "rng.h"

#define SEC_MIN_SECTEUR 35
#define SEC_MAX_SECTEUR 50
#define MIN_MS 0
#define MAX_MS 1000

/**
 * Génère le temps pour un secteur du circuit
 * 
 * Génère un entier qui représente les secondes, puis un autre
 * représentant les millisecondes. Ce dernier est divisé par 1000 et 
 * ajouter au premier
 */
double generateTimeSector(){
  double tempsSecteur, ms;
  double sec = (rand() % (SEC_MAX_SECTEUR - SEC_MIN_SECTEUR + 1)) + SEC_MIN_SECTEUR;

  if(sec == SEC_MAX_SECTEUR){ //Le temps maximum par secteur est de 50.000 secondes
    ms = MIN_MS;
  } else {
    ms = (rand() % (MAX_MS - MIN_MS + 1)) + MIN_MS;
  }

  return tempsSecteur = sec + (ms/1000);
}

/**
 * Génère le temps pendant lequel la voiture est au stand
 * 
 * Génère un entier qui représente les secondes, puis un autre
 * représentant les millisecondes. Ce dernier est divisé par 1000 et 
 * ajouter au premier
 * 
 * @param min : nombre de secondes minimum
 * @param max : nombre de secondes maximum
 */
double tpsStand(int min, int max){
  double tpsStand;

  double sec = (rand() % (max - min + 1)) + min;
  double ms = (rand() % (MAX_MS - MIN_MS + 1)) + MIN_MS;

  return tpsStand = sec + (ms/1000);
}

