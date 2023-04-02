#ifndef _STRUCT_H_
#define _STRUCT_H_

#include <stdio.h>

#define BUFFSIZE 1024
#define MAXSYMBOLS 256
#define MAXBITS 256
#define MAXSYMLENGHT 8
#define BITSINCHAR 8

typedef struct{
    int first;
    int second;
} pair;

typedef struct{
    char letter;
    char lcode[MAXBITS];
} code;

struct node {
    char letters[MAXSYMBOLS];
    int lcount;
    int quantity;

    int bit;
    struct node* left;
    struct node* right;
};

#endif