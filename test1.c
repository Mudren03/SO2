#include <stdio.h>

// 🔹 GLOBALI (contigue come da traccia)
int a, b;
float x;
char nome[20];
double _validGlobal;

// ❌ nome non valido
int 1errore;

// ❌ tipo non valido
mioTipo y;

// typedef valido
typedef int mioInt;
mioInt z;

int main() {

    // 🔹 LOCALI (contigue all'inizio del main)
    int c, d;
    float valore;
    char buffer[50];
    int _ok;

    // ❌ nome non valido
    int 9test;

    // ❌ tipo non valido
    tipoFake k;

    // typedef locale valido
    typedef float mioFloat;
    mioFloat w;

    // 🔹 USI REALI
    a = 5;
    b = a + 3;
    c = b + 1;

    valore = 3.14;
    _ok = c;

    // 🔹 NON USATE
    // d
    // x
    // nome
    // z
    // buffer
    // w

    printf("Test completato: %d\n", a);

    return 0;
}