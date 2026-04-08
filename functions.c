#include "header.h"
#include <stdlib.h>
#include <string.h>
#include <regex.h>

char* trovaNuoviTipi (const char* testo){

    char* copiacontenuto = strdup(testo);
    char* saveptr;
    char* token = strtok_r(copiacontenuto, "\n", &saveptr);

    const char* tipiValidi[100] = {"int", "float", "char", "double", "long", "short", "unsigned", "signed", "struct"};
    int numTipi = 9; // Numero iniziale di tipi validi

    while (token != NULL) {
        // Rimuovi spazi iniziali e finali
        // printf("il token è: %s\n", token);
        while (*token == ' ' || *token == '\t') token++;

        //printf("token=%s\n", token);

        // Controlla se la riga definisce un nuovo tipo (typedef o struct)
        if (strstr(token, "typedef") == token || strstr(token, "struct") == token) {
            // Estrai il nome del nuovo tipo
            char nuovoTipo[50];
            sscanf(token, "%*s %*s %s", nuovoTipo); // Salta "typedef" e il tipo originale
            nuovoTipo[strlen(nuovoTipo)-1] = '\0';
            tipiValidi[numTipi++] = strdup(nuovoTipo); // Aggiungi il nuovo tipo all'elenco
            // printf("Trovato nuovo tipo: %s\n", nuovoTipo);
        }
        token = strtok_r(NULL, "\n", &saveptr);
    }
    
    int lunghezza = 0;
    for (int i = 0; i < numTipi; i++) {
        lunghezza += strlen(tipiValidi[i]) + 1; // +1 per il separatore '|'
    }

    // Alloca memoria per la stringa regex
    char* regex = (char*)malloc(lunghezza + 1); // +1 per il terminatore '\0'
    if (regex == NULL) {
        perror("Errore di allocazione memoria per la regex");
        return NULL;
    }

    // Costruisci la stringa regex
    regex[0] = '\0';

    // inizio regex
    strcat(regex, "^\\s*(");

    for (int i = 0; i < numTipi; i++) {
        strcat(regex, tipiValidi[i]);
        if (i < numTipi - 1) {
            strcat(regex, "|");
        }
    }

    // chiusura regex
    strcat(regex, ")\\b");

    // Debug: stampa la regex generata
    //printf("Regex generata: %s\n", regex);

    // Debug: verifica i tipi validi
    //printf("Tipi validi trovati:\n");
    for (int i = 0; i < numTipi; i++) {
        // ("- %s\n", tipiValidi[i]);
    }

    return regex;

};

variabile* estraiVariabili(const char* fileContent, variabile* lista, int* counter) {
    char* copiacontenuto = strdup(fileContent);
    char* saveptr;
    char* token = strtok_r(copiacontenuto, "\n", &saveptr);

    regex_t regexTipi;
    const char* patternTipi = trovaNuoviTipi(fileContent);

    if (regcomp(&regexTipi, patternTipi, REG_EXTENDED)) {
        perror("Errore compilazione regex");
        free(copiacontenuto);
        return NULL;
    }

    // printf("Pattern tipi: %s\n", patternTipi);

    while (token != NULL) {
        // printf("\nAnalizzando riga: [%s]\n", token);

        if (regexec(&regexTipi, token, 0, NULL, 0) == 0) {
            if (strchr(token, '(') != NULL) {
                // printf("⛔ È una funzione, salto!\n");
                token = strtok_r(NULL, "\n", &saveptr);
                continue;
            }
            // printf("✔ Riga valida!\n");

            char tipo[50];
            char resto[200];

            int letti = sscanf(token, "%s %[^\n]", tipo, resto);

            // printf("sscanf letti: %d | tipo: [%s] | resto: [%s]\n", letti, tipo, resto);

            if (letti == 2) {
                char* var = strtok(resto, ",");

                while (var != NULL) {
                    while (*var == ' ' || *var == '\t') var++; // trim iniziale

                    // rimuovi ; finale
                    size_t len = strlen(var);
                    if (len > 0 && var[len - 1] == ';') {
                        var[len - 1] = '\0';
                    }

                    /*printf("→ Variabile grezza: [%s]\n", var);

                    printf("DEBUG counter=%d\n", *counter);
                    printf("DEBUG lista ptr=%p\n", (void*)lista);
                    printf("DEBUG elemento ptr=%p\n", (void*)&lista[*counter]);*/

                    strcpy(lista[*counter].tipo, tipo);
                    strcpy(lista[*counter].name, var);
                    lista[*counter].usageCount = 0;

                    /* printf("✔ Salvata: tipo=%s, nome=%s (index=%d)\n",
                           lista[*counter].tipo,
                           lista[*counter].name,
                           *counter);*/

                    (*counter)++;

                    var = strtok(NULL, ",");
                }
            } else {
                // printf("⚠ sscanf non ha funzionato correttamente\n");
            }

        } else {
            // printf("✘ Riga ignorata\n");
        }

        token = strtok_r(NULL, "\n", &saveptr);
    }

    regfree(&regexTipi);
    free(copiacontenuto);

    return lista;
}

