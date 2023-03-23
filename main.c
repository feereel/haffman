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
            res[j]->letters[0] = (char)i;
            res[j]->lcount = 1;
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

void SetLetters(struct node* nodeRoot, struct node* nodeA, struct node* nodeB){
    nodeRoot->lcount = nodeA->lcount + nodeB->lcount;
    for (size_t i = 0; i < nodeA->lcount; i++){
        nodeRoot->letters[i] = nodeA->letters[i];
    }
    int s = nodeA->lcount;
    for (size_t i = 0; i < nodeB->lcount; i++){
        nodeRoot->letters[i+s] = nodeB->letters[i]; 
    }
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

        SetLetters(root, nodes[mininds.first], nodes[mininds.second]);
        
        nodes[mininds.first] = root;
        nodes[mininds.second] = NULL;
        lastind = mininds.first;

        // printf("\n");
        // for (size_t i = 0; i < size; i++)
        // {
        //     if (nodes[i] == NULL){
        //         printf("-\n");
        //         continue;
        //     }
        //     printf("%c\t%d\n", nodes[i]->letters[0], nodes[i]->quantity);
        // }
        c--;
    }
    return lastind;
}

void SetCodes(struct node* nodeRoot,int codes[][8], int depth){
    for (size_t i = 0; i < 256; i++)
    {
        if (nodeRoot->letters[i] == '\0')
            return;
        codes[(int)nodeRoot->letters[i]][depth] = nodeRoot->bit;
    }
}

void GetHaffmanCodes(struct node* root, int codes[256][8], int depth){
    if (depth != -1)
        SetCodes(root, codes, depth);
    if (root->left == NULL || root->right == NULL)
        return;
    
    GetHaffmanCodes(root->left, codes, depth+1);
    GetHaffmanCodes(root->right, codes, depth+1);
}



int main(int argc, char* argv[]){
    if (argc == 1){
        printf("\t filename [-ED] bin\n");
        exit(0);
    }
    int lenght = strlen(argv[1]);
    int size = 0;
    struct node** nodes = GetLettersCount(argv[1], lenght, &size);
    // for (size_t i = 0; i < size; i++)
    // {
    //     printf("%c\t%d\n", nodes[i]->letters[0], nodes[i]->quantity);
    // }

    pair minnodes = GetMinIPair(nodes, size);
    // printf("%d %d\n", minnodes.first, minnodes.second);

    int rootind = GetHaffmanRoot(nodes, size);
    struct node* root = nodes[rootind]->right;
    
    // for (size_t i = 0; i < root->lcount; i++)
    // {
    //     printf("%c", root->letters[i]);
    // }
    // printf("\n%d\n", root->bit);
    


    int codes[256][8];
    for (size_t i = 0; i < 256; i++)
        for (size_t j = 0; j < 8; j++)
            codes[i][j] = 2;
    
    GetHaffmanCodes(nodes[rootind], codes, -1);

    for (size_t i = 0; i < 256; i++)
    {
        if (codes[i][0] == 2)
            continue;
        
        printf("%c: ", (char)i);
        for (size_t j = 0; j < 8; j++)
        {
            if (codes[i][j] == 2)
                break;
            
            printf("%d", codes[i][j]);
        }
        printf("\n");
    }
}