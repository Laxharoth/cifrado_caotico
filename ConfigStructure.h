#ifndef __CONFIGSTRUCTURE_H__
#define __CONFIGSTRUCTURE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100

typedef struct {
    unsigned int beta;
    unsigned int lambda;
    unsigned int bulk_size;
    double r;
    unsigned long long seed;
    unsigned long long file_size;
} Configuracion;

Configuracion readConfigFile(const char *filename);

#endif