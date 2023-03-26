#ifndef _STRUCT_H_
#define _STRUCT_H_

#include <stdio.h>
#define BUFFSIZE 32
typedef struct{
    int first;
    int second;
} pair;

typedef struct{
    char letter;
    char lcode[8];
} code;

struct node {
    char letters[256];
    int lcount;
    int quantity;

    int bit;
    struct node* left;
    struct node* right;
};

#endif