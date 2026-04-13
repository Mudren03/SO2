
#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <getopt.h>

typedef struct 
{
    char name[20];
    char tipo[20];
    int usageCount;

} variabile;

typedef struct {
    int totali;
    int erroriNomi;
    int erroriTipi;
    int nonUsate;
    int erroriTotali;
} Statistiche;

variabile* estraiVariabili(const char* fileContent, variabile* lista, int* counter, int* );

char* trovaNuoviTipi (const char* testo);

int isNomeValido(char* nome, Statistiche* stats);

void calcolaUtilizzoVariabili (const char* fileContent, variabile* lista, int* counter);

void calcolaVariabiliNonUsate(variabile* lista, Statistiche* stats, int* counter);
#endif