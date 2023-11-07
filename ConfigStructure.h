#ifndef __CONFIGSTRUCTURE_H__
#define __CONFIGSTRUCTURE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100

typedef struct {
    // parametros de renyi
    unsigned long long beta;
    unsigned long long lambda;
    // parametro de logistico
    unsigned long long r;
    // parametros de parabola
    unsigned long long h;
    unsigned long long k;
    unsigned long long n;
    // semilla aleatoria
    unsigned long long seed;
    // numero de bytes a escribir
    unsigned long long file_size;
    unsigned long long bulk_size;
} Configuracion;

Configuracion readConfigFile(const char *filename);

#endif