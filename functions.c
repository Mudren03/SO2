#include "header.h"


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

variabile* estraiVariabili(const char* fileContent, variabile* lista, int* counter, int* ultimaAssegnazione) {
    char* copiacontenuto = strdup(fileContent);
    char* saveptr;
    char* token = strtok_r(copiacontenuto, "\n", &saveptr);
    int riga = 0;

    regex_t regexTipi;
    const char* patternTipi = trovaNuoviTipi(fileContent);

    if (regcomp(&regexTipi, patternTipi, REG_EXTENDED)) {
        perror("Errore compilazione regex");
        // Libera la memoria solo se allocata
        if (copiacontenuto != NULL) {
            free(copiacontenuto);
            copiacontenuto = NULL; // Evita double free
        }

        // Libera la regex solo se compilata correttamente
        if (patternTipi != NULL) {
            regfree(&regexTipi);
        }
        return NULL;
    }

    // printf("Pattern tipi: %s\n", patternTipi);

    while (token != NULL) {
        riga++;
        // printf("\nAnalizzando riga: [%s]\n", token);

        if (regexec(&regexTipi, token, 0, NULL, 0) == 0) {
            if (strchr(token, '(') != NULL) {

                // printf("⛔ È una funzione, salto!\n");
                token = strtok_r(NULL, "\n", &saveptr);
                riga++;
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

            // 1️⃣ Cerca la parentesi quadra (per gli array)
                char* pParentesi = strchr(var, '[');
                if (pParentesi != NULL) {
                    *pParentesi = '\0'; // Tronca la stringa alla parentesi
                }

                // 2️⃣ Rimuovi il ; finale (se presente e se non era già dopo la parentesi)
                size_t len = strlen(var);
                if (len > 0 && var[len - 1] == ';') {
                    var[len - 1] = '\0';
                }

                // 3️⃣ Trim finale (importante per casi come "int buffer [50];")
                len = strlen(var);
                while (len > 0 && (var[len - 1] == ' ' || var[len - 1] == '\t')) {
                    var[len - 1] = '\0';
                    len--;
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
                    *ultimaAssegnazione = riga;
                    
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
    // Libera la memoria solo se allocata
    /*if (copiacontenuto != NULL) {
        free(copiacontenuto);
        copiacontenuto = NULL; // Evita double free
    }*/

    // Libera la regex solo se compilata correttamente
    if (patternTipi != NULL) {
        regfree(&regexTipi);
    }

    return lista;
}

int isNomeValido(char* nome, Statistiche* stats) {
    regex_t regex;
    int scaduto;
    // Regex: inizia con lettera/underscore, segue con zero o più alfanumerici/underscore
    const char* pattern = "^[a-zA-Z_][a-zA-Z0-9_]*([[:space:]]*\\[[[:space:]]*[0-9]*[[:space:]]*\\])*$";
    
    if (regcomp(&regex, pattern, REG_EXTENDED) != 0) return 0;
    
    scaduto = regexec(&regex, nome, 0, NULL, 0);
    regfree(&regex);
    if (scaduto == 1){
        printf("Errore: nome errato per variabile: %s\n", nome);
        stats->erroriTotali++;
        stats->erroriNomi++;
    }
    return (scaduto == 0); // Ritorna 1 se valido, 0 se errore
}

void calcolaVariabiliNonUsate(variabile* lista, Statistiche* stats, int* counter) {
    for (int i = 0; i < *counter; i++) {
        // Una variabile è considerata "non utilizzata" solo se:
        // 1. Il conteggio usi è 0
        if ((lista[i].usageCount)-1 == 0) {
            stats->nonUsate++;
            stats->erroriTotali++;
            printf("Errore: Variabile non utilizzata '%s' (dichiarata a riga boh)\n", 
                    lista[i].name);
        }
    }
}

void calcolaUtilizzoVariabili (const char* fileContent, variabile* lista, int* counter) {
// Cicliamo su ogni variabile trovata nella fase di estrazione
    for (int i = 0; i < *counter; i++) {
        // 1. CREAZIONE REGEX DINAMICA: \bNomeVar\b
        char pattern[150];
        sprintf(pattern, "\\b%s\\b([[:space:]]*\\[[[:space:]]*[^]]+[[:space:]]*\\])?", lista[i].name);
        regex_t regUso;
        if (regcomp(&regUso, pattern, REG_EXTENDED) != 0) {
            fprintf(stderr, "Errore compilazione regex per %s\n", lista[i].name);
            continue;
        }

        // Rimuovi duplicati delle dichiarazioni
        char* copiacontenuto = strdup(fileContent);
        char* saveptr;
        char* token = strtok_r(copiacontenuto, "\n", &saveptr);
        int rigaAttuale = 0;

        while (token != NULL) {
            rigaAttuale++;
            regmatch_t pmatch;

            while (regexec(&regUso, token, 1, &pmatch, 0) == 0) {
                lista[i].usageCount++;
                token += pmatch.rm_eo;
                if (*token == '\0') break;
            }

            token = strtok_r(NULL, "\n", &saveptr);
        }

        free(copiacontenuto);
        regfree(&regUso);
    }
}




