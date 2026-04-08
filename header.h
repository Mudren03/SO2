
#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>

typedef struct 
{
    char name[20];
    char tipo[20];
    int usageCount;

} variabile;

variabile* estraiVariabili(const char* fileContent, variabile* lista, int* counter);

char* trovaNuoviTipi (const char* testo);
#endif