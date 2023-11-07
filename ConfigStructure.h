#ifndef __CONFIGSTRUCTURE_H__
#define __CONFIGSTRUCTURE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100

typedef struct {
    unsigned long long beta;
    unsigned long long lambda;
    unsigned long long bulk_size;
    unsigned long long r;
    unsigned long long h;
    unsigned long long k;
    unsigned long long n;
    unsigned long long seed;
    unsigned long long file_size;
} Configuracion;

Configuracion readConfigFile(const char *filename);

#endif