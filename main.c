#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "struct.h"


struct node** GetLettersCount(char* string, int length, int* size){
    int letters[256];
    for (size_t i = 0; i < 256; i++){
        letters[i] = 0;
    }
    
    for (size_t i = 0; i < length; i++){
        letters[(int)string[i]]++;
    }

    for (size_t i = 0; i < 256; i++){
        if (letters[i] != 0){
            (*size)++;
        }
    }
    struct node** res = (struct node**)calloc((*size), sizeof(struct node*));
    for (size_t i = 0; i < *size; i++)
    {
        res[i] = (struct node*)calloc(1, sizeof(struct node));
    }
    
    size_t j = 0;
    for (size_t i = 0; i < 256; i++)
    {
        if (letters[i] != 0){
            res[j]->letter = (char)i;
            res[j]->quantity = letters[i];
            j++;
        }
    }
    return res;
}

pair GetMinIPair(struct node** nodes, int size){
    pair mi, mv;
    mi.first = 0, mi.second = 0;
    mv.first = INT_MAX, mv.second = INT_MAX;
    for (int i = 0; i < size; i++){
        if (nodes[i] == NULL)
            continue;

        if (nodes[i]->quantity < mv.first){
            mv.second = mv.first;
            mv.first = nodes[i]->quantity;
            mi.second = mi.first;
            mi.first = i;
        } else if (nodes[i]->quantity < mv.second){
            mv.second = nodes[i]->quantity;
            mi.second = i;
        }
    }
    if (mv.first > mv.second){
        int t = mi.second;
        mi.second = mi.first;
        mi.first = t;
    }
    return mi;
}

int GetHaffmanRoot(struct node** nodes, size_t size){
    int c = size-1;
    int lastind = 0;
    while (c){
        pair mininds = GetMinIPair(nodes, size);
        struct node* root = (struct node*)calloc(1, sizeof(struct node));
        root->right = nodes[mininds.first];
        root->right->bit = 1;
        root->left = nodes[mininds.second];
        root->left->bit = 0;
        root->quantity = root->left->quantity + root->right->quantity;
        nodes[mininds.first] = root;
        nodes[mininds.second] = NULL;
        lastind = mininds.first;

        printf("\n");
        for (size_t i = 0; i < size; i++)
        {
            if (nodes[i] == NULL){
                printf("-\n");
                continue;
            }
            printf("%c\t%d\n", nodes[i]->letter, nodes[i]->quantity);
        }
        c--;
    }
    return lastind;
}

void GetHaffmanCodes(struct node* root, size_t size, code* codes, int level, int *k){
    if (root->left == NULL || root->right == NULL){
        codes[(*k)].letter = root->letter;
        (*k)++;
    } 
}



int main(int argc, char* argv[]){
    if (argc == 1){
        printf("\t filename [-ED] bin\n");
        exit(0);
    }
    int lenght = strlen(argv[1]);
    int size = 0;
    struct node** nodes = GetLettersCount(argv[1], lenght, &size);
    for (size_t i = 0; i < size; i++)
    {
        printf("%c\t%d\n", nodes[i]->letter, nodes[i]->quantity);
    }

    pair minnodes = GetMinIPair(nodes, size);
    printf("%d %d\n", minnodes.first, minnodes.second);

    int rootind = GetHaffmanRoot(nodes, size);
    
}