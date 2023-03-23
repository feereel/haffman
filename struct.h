#ifndef _STRUCT_H_
#define _STRUCT_H_

#include <stdio.h>

typedef struct{
    int first;
    int second;
} pair;

typedef struct{
    char letter;
    char lcode[7];
} code;

struct node {
    char letter;
    int quantity;

    int bit;
    struct node* left;
    struct node* right;
};

#endif