#include "header.h"

int main(int argc, char **argv) {

    int maxVariabili = 100; // o quello che vuoi
    variabile *lista = calloc(maxVariabili, sizeof(variabile));
    int count = 0;
    int ultimaAssegnazione = 0;
    Statistiche *stats = calloc(maxVariabili, sizeof(variabile));

    //controlla prima argc =3
    if (argc!=3){
        perror("gli argomenti sono diversi da 3\n");
        return 1;
    }

    char *inputFileName = argv[1];
    char *outputFileName = argv[2];

    FILE *inputFile = fopen(inputFileName, "rb");
    if (inputFile == NULL) {
        perror("Errore nell'apertura del file di input");
        return 1;
    }

    FILE *outputFile = fopen(outputFileName, "w");
    if (outputFile == NULL) {
        perror("Errore nell'apertura del file di output");
        fclose(inputFile);
        return 1;
    }


    // Determina la dimensione del file
    fseek(inputFile, 0, SEEK_END); // Vai alla fine del file
    long fileSize = ftell(inputFile); // Ottieni la posizione corrente (dimensione del file)
    rewind(inputFile); // Torna all'inizio del file

    // Alloca memoria per il contenuto del file
    char *fileContent = (char *)malloc((fileSize + 1) * sizeof(char)); 
 
    if (fileContent == NULL) {
        perror("Errore di allocazione memoria");
        fclose(inputFile);
        return 1;
    }

    // Leggi il contenuto del file nella variabile
    size_t bytesRead = fread(fileContent, 1, fileSize, inputFile);
    fileContent[bytesRead] = '\0'; // Aggiungi il terminatore di stringa

    // Chiudi il file
    fclose(inputFile);
    
    // 🔥 chiama funzione che estrae variabili
    lista = estraiVariabili(fileContent, lista, &count, &ultimaAssegnazione);

    calcolaUtilizzoVariabili(fileContent, lista, &count);

    calcolaVariabiliNonUsate(lista, stats, &count);

    // printf("\n%lu\n", sizeof(lista));
    for (size_t i = 0; i < count; i++)
    {
        // printf("la variabile cosiderata è: %s\n", lista[i].name);
        isNomeValido(lista[i].name, stats);
    }
    
    

    printf("Variabili trovate: %d\n", count);
    // printf("Ultima riga contatore: %i\n", ultimaAssegnazione);
    printf("Errori totali: %d\n", stats->erroriTotali);
    printf("Errori nomi: %d\n", stats->erroriNomi);
    printf("Errori utilizzo: %d\n", stats->nonUsate);

    for (int i = 0; i < count; i++) {
        /*printf("%s %s (utilizzo %d)\n",
            lista[i].tipo,
            lista[i].name,
            lista[i].usageCount);*/

        fprintf(outputFile, "%s %s (utilizzo %d)\n",
            lista[i].tipo,
            lista[i].name,
            lista[i].usageCount);
    }
    free(lista);
    free(fileContent);
    // Chiudi i file
    fclose(outputFile);


    return 0;
}