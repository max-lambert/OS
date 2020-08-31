#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "rng.h"

#define NOMBRE_VOITURES 20

typedef struct{
  char fileName[10]; //Pour déjà déterminer le nom du fichier dés le début
  double tempsSession;
  int nombreCar;
}Session;

typedef struct {
  int idCar;
  double s1;
  double s2;
  double s3;
  int lap;
  double tempsTour;
  int out;
  int stand;;
  double bestS1;
  double bestS2;
  double bestS3;
  double bestTempsTour;
  int fini; //Permet simplement d'informer le parent (ici l'afficheur) que la voiture a fini de rouler
} Car;

int tempsEnCourse;
void carRun(Car *car, int *carNum);
