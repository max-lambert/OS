#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include "car.h"
#include "file.h"

int triBestTempsTour(const void *a, const void *b);
int indexBestOfSector(char sectId[]);
int indexBestOfLap();
void print();
void displayGrid(Car *data, sem_t *sema);
