// 31-01-2024: Based on ergodic_ChaoticCycles
//  //3/25/2010: Same as v1_sizechaoticCycles.c
//  //Using Ergodic property for cycle detection
/* This program detects cycles >= 3 by using a hash table implementation

   modArray:
   modSize:
*/

/*
Notes: get rid of foundCycle
       think of future implementation (function or part of main prog)
       draw a diagram of the abstract data type
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ConfigStructure.h"
#include "fn.c"

#define TTNULL (struct HASHTABLE *)0

typedef unsigned long long ull;

struct HASHTABLE {
    struct HASHTABLE *pred, *suc, *timeNext;
    ull key, iterationVal;
    char head;  // first element in the mode ordered list?
};

void InitializeLinkedList(struct HASHTABLE *start, int initVal) {
    if (start == TTNULL) {
        printf("ERROR InitializeLinkedList\n");
        exit(1);
    }

    while (start->timeNext != TTNULL) {
        start->pred = TTNULL;
        start->suc = TTNULL;
        start->key = initVal;
        start->iterationVal = 0;
        start->head = 0;
        start = start->timeNext;
    }
}

struct HASHTABLE *CreateLinkedList(int numElem, int initVal,
                                   struct HASHTABLE **last) {
    struct HASHTABLE *start, *temp;
    int i;

    // first node
    start = (struct HASHTABLE *)malloc(sizeof(struct HASHTABLE));
    start->pred = start->suc = TTNULL;
    start->key = initVal;
    start->iterationVal = 0;
    start->head = 0;
    // linked list
    for (temp = start, i = 1; i < numElem; i++) {
        temp->timeNext = (struct HASHTABLE *)malloc(sizeof(struct HASHTABLE));
        temp = temp->timeNext;
        temp->pred = TTNULL;
        temp->suc = TTNULL;
        temp->key = initVal;
        temp->iterationVal = 0;
        temp->head = 0;
    }
    *last = temp;
    temp->timeNext = TTNULL;
    return (start);
}

void printList(struct HASHTABLE *start, char *type) {
    if (strcmp(type, "suc") == 0) {
        while (start != TTNULL) {
            printf("%llu,", start->key);
            start = start->suc;
        }
    }
    if (strcmp(type, "timeNext") == 0)
        while (start != TTNULL) {
            printf("%llu,", start->key);
            start = start->timeNext;
        }
}

void printHash(struct HASHTABLE **hash, ull sizeHash) {
    ull i;

    for (i = 0; i < sizeHash; i++)
        if (*(hash + i) != TTNULL) {
            printf("\ni = %llu: ", i);
            printList(*(hash + i), "suc");
        }
}

void generate_random_file_13(unsigned char *const buffer,
                             const Configuracion *config, ull *Xn,
                             ull *parametros, chaotic_lookup_table *roulete,
                             const ull numeroMapas) {
    const ull num_mapas_mask = numeroMapas - 1;
    unsigned char *ptr_buffer = buffer;
    ull epsilon = 65535, lambda = 5, H = 0;

    const ull chaotic_table_size_mask = numeroMapas * config->n - 1;
    srand(config->seed);
    const ull table_mask = config->n - 1;

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull position = 0;
    ull i_lut_pos_list_index = 0;

    while (remaining_bytes > 0) {
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        const ull generated = random_select_coupled_chaotic_map_lookuptable(
            &position,
            (Xn[(i_lut_pos_list_index++) & chaotic_table_size_mask]) &
                table_mask,
            roulete, parametros, lambda, num_mapas_mask, epsilon, &H);
        memcpy(ptr_buffer, &generated, chunk_bytes);
        ptr_buffer += chunk_bytes;
        remaining_bytes -= chunk_bytes;
    }
}

unsigned char *global_buffer;
ull *global_table, *global_parameters;
chaotic_lookup_table *global_roulete;
Configuracion *global_config;

ull generator() {
    static ull buffer_index = (~(0ull)) - 24;
    buffer_index += 8;
    const ull numeroMapas = 4;
    if (buffer_index + 16 >= global_config->file_size) {
        buffer_index = 0;
        generate_random_file_13(global_buffer, global_config, global_table,
                                global_parameters, global_roulete, numeroMapas);
    }
    return *((ull *)(global_buffer + buffer_index));
}

int main() {
    struct HASHTABLE **modArray;  // array for mod(key,modSize)
    ull _modSize;                 // size of modArray (prime number)
    ull _numElem;  //  Number of elements for the detection of cycles
    ull samplingInterval, copySampInt, modIndex;
    struct HASHTABLE *lastOrderNode, *startEmptyList, *timeOrderList;
    struct HASHTABLE *replace,
        *antReplace;  // point to replace (ant) node in the timeOrderList.
    float chaoticVal, copyChaoticVal, chaoticPar, cV;
    ull cVInt, beta, renyi_lambda;
    float raise32 = pow(2., 32);
    struct HASHTABLE **modIndexPtr, **indexPtr;  // pointer to a modArray cell
    ull i, j, k, l, n, foundCycle, upSampling, cte;
    struct HASHTABLE *temp, *antTemp;
    ull globalCounter =
        0;  // iteration counter: number of processed chaotic values
    ull cycleLength, round;
    ull a, b;
    Configuracion config;
    ull const numeroMapas = 4;
    const ull lup_size = config.n;
    const ull table_mask = config.n - 1;

    ull *Xn = malloc(sizeof(ull) * numeroMapas * lup_size),
        *parametros = malloc(sizeof(ull) * numeroMapas);
    chaotic_lookup_table *roulete =
        malloc(sizeof(chaotic_lookup_table) * numeroMapas);
    readConfigFile("config.txt", &config);

    global_buffer = malloc(config.file_size + 200);
    global_table = Xn;
    global_parameters = parametros;
    global_roulete = roulete;
    global_config = &config;

    for (size_t i = 0; i < numeroMapas; i++) {
        Xn[i * lup_size] = rand();
        Xn[i * lup_size] = (Xn[i] << 32) | rand();
        parametros[i] = (rand() % 3000) + 5;
        for (size_t j = 1; j < lup_size; ++j) {
            Xn[i * lup_size + j] = RenyiMap(Xn[i * lup_size + j - 1],
                                            parametros[i], config.lambda);
        }
        roulete[i].lookup_table = Xn;
        roulete[i].lu_table_size = lup_size;
        roulete[i].lu_table_mask = table_mask;
        roulete[i].last_generated = Xn[lup_size - 1];
    }

    // Input parameters
    printf("Number of Elements (odd) for the estimation of the cycle (101):");
    scanf("%llu", &_numElem);
    if (_numElem == 0) {
        printf("numElem > 0\n");
        exit(1);
    }
    printf("numElem mod 2 = %llu\n", _numElem % 2);
    if (!(_numElem % 2)) _numElem++;
    const ull numElem = _numElem;
    printf("numElem = %llu\n", numElem);

    printf("Number of cells in hashTable (prime number-599):");
    scanf("%llu", &_modSize);
    const ull modSize = _modSize;
    printf("Initial Sampling Interval Size (2): ");
    scanf("%llu", &samplingInterval);
    copySampInt = samplingInterval;
    printf("No de UpSamplings: ");
    scanf("%llu", &upSampling);

    //    scanf("%f %f",&chaoticPar,&chaoticVal); // 3.989 o 3.898 y 0.3154 o
    //    0.198 printf("chaoticPar = %f, chaoticVal =
    //    %f\n",chaoticPar,chaoticVal);
    chaoticPar = 3.688;  // 3.828427124;
    copyChaoticVal = chaoticVal = 0.1;
    srand(0xF3);

    cVInt = rand();
    cVInt = (cVInt << 32) | rand();
    beta = rand();
    beta = (cVInt << 32) | rand();
    renyi_lambda = rand() & 63;

    // chaotic interval
    a = 0xFFFFFF;
    b = 0xFFFFFFFFFF;

    round = 1;           // first round
    while (round < 2) {  // because of the increment 0.01
        if (round == 1) {
            // Memory allocation
            // creating numElem nodes
            if ((startEmptyList =
                     CreateLinkedList(numElem, -1, &lastOrderNode)) == TTNULL) {
                printf("**** ERROR in CreateLinkedList() \n");
                exit(1);
            }
            // timeOrderList always points to first element in the linked
            // list
            timeOrderList =
                startEmptyList;  // it always points to first elem-list

            // Allocating memory for *modArray
            modArray = (struct HASHTABLE **)malloc(modSize *
                                                   sizeof(struct HASHTABLE *));
        } else {
            InitializeLinkedList(timeOrderList, -1);
            startEmptyList =
                timeOrderList;  // it always points to first elem-list
            upSampling = 20;    // modify to take given value
            globalCounter = 0;
            cycleLength = 0;
            foundCycle = 0;
            samplingInterval = copySampInt;  // modify to take given value
            chaoticVal = copyChaoticVal + 0.01;
            copyChaoticVal = chaoticVal;
            n = 0;
        }
        // printf("chaoticVal = %f\n",chaoticVal);

        // initizalizing array to NULL
        for (i = 0; i < modSize; i++) *(modArray + i) = TTNULL;

        // First part: creation and insertion
        // first key insertion
        foundCycle = 0;

        while (cVInt = generator(), globalCounter++,
               !((cVInt <= b) && (cVInt >= a)))
            ;

        modIndexPtr = modArray + (cVInt % modSize);
        temp = *modIndexPtr;   // content of modIndexPtr
        if (temp == TTNULL) {  // insert data
            *modIndexPtr = startEmptyList;
            startEmptyList->pred = (struct HASHTABLE *)modIndexPtr;
            startEmptyList->suc = TTNULL;
            startEmptyList->key = cVInt;
            startEmptyList->iterationVal = globalCounter;
            startEmptyList->head =
                1;  // first element to this corresponding cell in modArray
            startEmptyList =
                startEmptyList->timeNext;  // 2nd elem becomes the start of
                                           // the original empty linked list
        } else {
            antTemp = temp->pred;  // points to previous node to temp
            while (temp != TTNULL) {
                if (temp->key == cVInt) {
                    foundCycle = 1;

                    printf("\nFOUND A CYCLE(1) \n");
                    printf("Cycle length = %llu\n",
                           cycleLength = globalCounter - temp->iterationVal);
                    printf("globalCounter= %llu\n", globalCounter);
                    printf("numPasos = %llu, samplingInterval = %llu\n", n,
                           samplingInterval);
                    printf("Approx. Transition Interval = %llu\n",
                           temp->iterationVal);

                    goto tag;
                    exit(1);  // break;
                }
                antTemp = temp;
                temp = temp->suc;
            }
            if (!foundCycle) {  // no match was found
                antTemp->suc = startEmptyList;
                startEmptyList->pred = antTemp;
                startEmptyList->suc = TTNULL;
                startEmptyList->key = cVInt;
                startEmptyList->iterationVal = globalCounter;
                startEmptyList->head = 0;
                startEmptyList =
                    startEmptyList->timeNext;  // 2nd elem becomes the start
                                               // of linked list
                // note that timeNext pointer in startEmptyList has no been
                // touched
            }
        }

        // Doing the rest of numElem
        n = 1;
        while (n < numElem) {
            // verifying for cycles only in the next samplingInterval
            // iterations
            for (j = 1; j < samplingInterval; j++) {
                // modIndexPtr is a double pointer

                while (cVInt = generator(), globalCounter++,
                       !((cVInt <= b) && (cVInt >= a)))
                    ;

                modIndexPtr = modArray + (cVInt % modSize);
                temp = *modIndexPtr;
                while (temp != TTNULL) {
                    if (temp->key == cVInt) {
                        foundCycle = 1;

                        printf("\nFOUND A CYCLE(2) \n");
                        printf(
                            "Cycle length = %llu\n",
                            cycleLength = globalCounter - temp->iterationVal);
                        printf("globalCounter= %llu\n", globalCounter);
                        printf("numPasos = 1, samplingInterval = %llu\n",
                               samplingInterval);
                        printf("Approx. Transition Interval = %llu\n",
                               temp->iterationVal);
                        goto tag;
                        exit(1);  // break;
                    }
                    temp = temp->suc;
                }
            }

            // key insertion every samplingInterval elements
            while (cVInt = generator(), globalCounter++,
                   !((cVInt <= b) && (cVInt >= a)))
                ;

            modIndexPtr = modArray + (cVInt % modSize);
            temp = *modIndexPtr;   // content of modIndexPtr
            if (temp == TTNULL) {  // insert data
                *modIndexPtr = startEmptyList;
                startEmptyList->pred = (struct HASHTABLE *)modIndexPtr;
                startEmptyList->suc = TTNULL;
                startEmptyList->key = cVInt;
                startEmptyList->iterationVal = globalCounter;
                startEmptyList->head = 1;  // first element to this
                                           // corresponding cell in modArray
                startEmptyList =
                    startEmptyList->timeNext;  // 2nd elem becomes the start of
                                               // the original empty linked list
            } else {
                // at least there is one node
                while (temp != TTNULL) {
                    if (temp->key == cVInt) {
                        foundCycle = 1;

                        printf("\nFOUND A CYCLE(3) \n");
                        printf(
                            "Cycle length = %llu\n",
                            cycleLength = globalCounter - temp->iterationVal);
                        printf("globalCounter= %llu\n", globalCounter);
                        printf("numPasos = %llu, samplingInterval = %llu\n", n,
                               samplingInterval);
                        printf("Approx. Transition Interval = %llu\n",
                               temp->iterationVal);
                        printf("Error Relativo = %f\n",
                               ((float)samplingInterval) / cycleLength + 1);

                        goto tag;
                        exit(1);  // break;
                    }
                    antTemp = temp;
                    temp = temp->suc;
                }
                antTemp->suc = startEmptyList;
                startEmptyList->pred = antTemp;
                startEmptyList->suc = TTNULL;
                startEmptyList->key = cVInt;
                startEmptyList->iterationVal = globalCounter;
                startEmptyList->head = 0;
                startEmptyList =
                    startEmptyList->timeNext;  // 2nd elem becomes the start
                                               // of linked list
                // note that timeNext pointer in startEmptyList has no been
                // touched because data is stored in the same order as
                // startEmptyList was created
            }
            n++;
        }

        /*
        printf("Nodes in Order\n");
        printList(timeOrderList,"timeNext");

        printf("\nHash Table\n");
        printHash(modArray,modSize);
        */

        // 'last' ptr points to the last node of timeOrderList
        // replace: points to the node to be replaced.
        // antReplace: points to the previous node of replace in time order.
        // Upsampling Algorithm
        n = 2;  // first upsampling + 1 (first round already done)
        cte = numElem / 2;
        while (n <= upSampling) {
            antReplace = timeOrderList;  // first node in the ordered list
            replace = timeOrderList->timeNext;
            samplingInterval += 1;  // upsampling by a factor of 2^n
            // verify that the next cte elements are in the HT
            i = 0;
            while (i < cte) {
                for (j = 0; j < (samplingInterval - 1); j++) {
                    while (cVInt = generator(), globalCounter++,
                           !((cVInt <= b) && (cVInt >= a)))
                        ;

                    modIndexPtr = modArray + (cVInt % modSize);
                    temp = *modIndexPtr;
                    while (temp != TTNULL) {
                        if (temp->key == cVInt) {
                            foundCycle = 1;

                            printf("\nFOUND A CYCLE(4): %llu\n", cVInt);
                            printf("Cycle length = %llu\n",
                                   cycleLength =
                                       globalCounter - temp->iterationVal);
                            printf("globalCounter= %llu\n", globalCounter);
                            printf(
                                "numPasos = %llu, samplingInterval = "
                                "%llu\n",
                                n, samplingInterval);
                            printf("Approx. Transition Interval = %llu\n",
                                   temp->iterationVal);

                            goto tag;
                            exit(1);  // break;
                        }
                        temp = temp->suc;
                    }
                }
                // doing one more chaoticVal and then insert it in the HT

                while (cVInt = generator(), globalCounter++,
                       !((cVInt <= b) && (cVInt >= a)))
                    ;

                modIndexPtr = modArray + (cVInt % modSize);
                temp = *modIndexPtr;
                antTemp = TTNULL;
                while (temp != TTNULL) {
                    if (temp->key == cVInt) {
                        foundCycle = 1;

                        printf("\nFOUND A CYCLE(5) \n");
                        printf(
                            "Cycle length = %llu\n",
                            cycleLength = globalCounter - temp->iterationVal);
                        printf("globalCounter= %llu\n", globalCounter);
                        printf("numPasos = %llu, samplingInterval = %llu\n", n,
                               samplingInterval);
                        printf("Approx. Transition Interval = %llu\n",
                               temp->iterationVal);
                        printf("Error Relativo = %f\n",
                               ((float)samplingInterval) / cycleLength + 1);

                        goto tag;
                        exit(1);  // break;
                    }
                    antTemp = temp;
                    temp = temp->suc;
                }
                // antTemp non-null if cell is not empty.

                // antTemp points to the last module ordered node where new
                // chaoticVal will be appended to; this last node points to
                // node replace (that is antTemp->suc points to node
                // 'replace'

                // NOTES
                // unlinking node replace from timeOrderList
                // checking if node to be replaced from timeOrderList is
                // head modIndexPtr is active pointing to the current
                // chaoticVal cell
                if (replace->head == 1) {
                    indexPtr = (struct HASHTABLE **)replace->pred;
                    if ((*indexPtr = replace->suc) != TTNULL) {
                        replace->suc->pred = replace->pred;
                        replace->suc->head = 1;  // pred and suc set ahead
                    }
                } else {  // replace is not head node
                    replace->pred->suc =
                        replace->suc;  // this is not in time order
                    if (replace->suc != TTNULL)
                        replace->suc->pred = replace->pred;
                }

                // connecting 'replace' to last position of HT
                if (antTemp == temp) {  // cell pointed by *modIndexPtr is empty
                    *modIndexPtr = replace;
                    replace->pred = (struct HASHTABLE *)modIndexPtr;
                    replace->head = 1;
                } else {  // at least one node in cell
                    antTemp->suc = replace;
                    replace->pred = antTemp;
                    replace->head = 0;
                }
                replace->suc = TTNULL;
                replace->key = cVInt;
                replace->iterationVal = globalCounter;

                // finishing up 'replace' and
                // getting it ready for next node replacement
                antReplace->timeNext =
                    replace->timeNext;  // time node 'replace' unlinked
                antReplace = replace->timeNext;
                lastOrderNode->timeNext =
                    replace;              // sending replace to the last node
                lastOrderNode = replace;  // last node in the time ordered list
                lastOrderNode->timeNext = TTNULL;
                replace = antReplace->timeNext;  // new node to be replaced

                i++;
            }  // while(i < cte)
            n++;
            //     printf("\n\n");
            //     printList(timeOrderList,"timeNext");
            //     printf("\nFinal Hash Table\n");
            //     printHash(modArray,modSize);
        }  // while(n <= upSampling)

    tag:
        round++;
        //   printf("\n\n");

    }  // while(round)
    free(global_buffer);
    free(Xn);
    free(parametros);
    free(roulete);
}  // end program
