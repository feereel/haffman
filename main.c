#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <math.h>
#include "struct.h"


void GetLettersCount(FILE* file, unsigned long letters[MAXSYMBOLS], unsigned long* size){
    for (size_t i = 0; i < MAXSYMBOLS; i++){
        letters[i] = 0;
    }
    char c;
    while((c = fgetc(file)) != EOF){
        letters[(int)c]++;
    }

    for (size_t i = 0; i < MAXSYMBOLS; i++){
        if (letters[i] != 0){
            (*size)++;
        }
    }
}

struct node** GetNodesFromLC(unsigned long letters[MAXSYMBOLS], unsigned long size){
    
    struct node** res = (struct node**)calloc((size), sizeof(struct node*));
    for (size_t i = 0; i < size; i++)
    {
        res[i] = (struct node*)calloc(1, sizeof(struct node));
    }
    
    size_t j = 0;
    for (size_t i = 0; i < MAXSYMBOLS; i++)
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

pair GetMinIPair(struct node** nodes, unsigned long size){
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

struct node* GetHaffmanRoot(struct node** nodes, size_t size){
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
        c--;
    }
    return nodes[lastind];
}

void SetCodes(struct node* nodeRoot,int codes[][MAXBITS], int depth){
    for (size_t i = 0; i < MAXSYMBOLS; i++)
    {
        if (nodeRoot->letters[i] == '\0')
            return;
        codes[(int)nodeRoot->letters[i]][depth] = nodeRoot->bit;
    }
}

void GetHaffmanCodes(struct node* root, int codes[MAXSYMBOLS][MAXBITS], int depth){
    if (depth != -1)
        SetCodes(root, codes, depth);
    if (root->left == NULL || root->right == NULL)
        return;
    
    GetHaffmanCodes(root->left, codes, depth+1);
    GetHaffmanCodes(root->right, codes, depth+1);
}

void PrintCodes(int codes[][MAXBITS]){
    for (size_t i = 0; i < MAXSYMBOLS; i++){
        if (codes[i][0] == 2)
            continue;
        
        printf("%c: ", (char)i);
        for (size_t j = 0; j < MAXBITS; j++)
        {
            if (codes[i][j] == 2)
                break;
            
            printf("%d", codes[i][j]);
        }
        printf("\n");
    }
}

int GetCodeLenght(int code[MAXBITS]){
    int c = 0;
    for (size_t i = 0; i < MAXBITS; i++){
        if (code[i] != 2){
            c++;
        } else{
            break;
        }
    }
    return c;
}

void WriteInBuffer(int buffer[], int start, int code[], int lenght, bool withBitsCount){
    int shift = 0;
    if (withBitsCount){
        shift = MAXSYMLENGHT;
        int tl = lenght;
        for (size_t i = 0; i < MAXSYMLENGHT; i++){
            int bit = tl%2;
            tl >>= 1;
            buffer[(start+MAXSYMLENGHT-1-i)%BUFFSIZE] = bit;
        }
    }
    for (size_t i = 0; i < lenght; i++){
        buffer[(start + shift + i)%BUFFSIZE] = code[i];
    }
    
}

void AppendBInFile(FILE* file, int buffer[], int pointer){
    char frame = 0;
    for (size_t i = 0; i < BITSINCHAR; i++){
        frame <<= 1;
        frame += buffer[(pointer+i)%BUFFSIZE];
    }
    fwrite(&frame, sizeof(char), 1, file);
}

//write the number of letter in a text
void EncryptHeader(FILE* binfile, unsigned long letters[MAXSYMBOLS]){
    for (size_t i = 0; i < MAXSYMBOLS; i++){
        unsigned long val = letters[i];
        fwrite(&val, sizeof(unsigned long), 1, binfile);
    }
}

unsigned long EncryptText(FILE* textfile,FILE* binfile, int codes[][MAXBITS]){
    fseek(textfile,0,SEEK_SET);

    unsigned long cwords = 0;
    int buffer[BUFFSIZE];
    int start = 0;
    int pointer = 0;
    char letter;
    while((letter = fgetc(textfile)) != EOF)
    {
        int c = GetCodeLenght(codes[(int)letter]);
        WriteInBuffer(buffer, start, codes[(int)letter], c, false);
        start += c;
        while (start - pointer >= BITSINCHAR){
            AppendBInFile(binfile, buffer, pointer);
            pointer += BITSINCHAR;
        }
        pointer%=BUFFSIZE;
        start%=BUFFSIZE;       

        cwords++;
    }
    AppendBInFile(binfile, buffer, pointer);
    return cwords;
}

char* Encrypt(char filename[]){
    FILE* file = fopen(filename, "r");
    unsigned long size = 0;

    //here maybe memery leak
    char* tfn = strdup(filename);
    char* bfilename = strcat(tfn, ".bin");
    FILE* output = fopen(bfilename, "wb");

    unsigned long letters[MAXSYMBOLS];
    GetLettersCount(file, letters, &size);
    struct node** nodes = GetNodesFromLC(letters, size);

    struct node* root = GetHaffmanRoot(nodes, size);

    int codes[MAXSYMBOLS][MAXBITS];
    for (size_t i = 0; i < MAXSYMBOLS; i++)
        for (size_t j = 0; j < MAXBITS; j++)
            codes[i][j] = 2;
    
    GetHaffmanCodes(root, codes, -1);

    //for count of words in binary text
    unsigned long cwords = 0;
    fwrite(&cwords, sizeof(unsigned long), 1, output);
    fwrite(&size, sizeof(unsigned long), 1, output);

    EncryptHeader(output, letters);
    cwords = EncryptText(file ,output, codes);

    fseek(output, 0, SEEK_SET);
    fwrite(&cwords, sizeof(unsigned long), 1, output);

    PrintCodes(codes);

    fclose(output);
    fclose(file);
    remove(filename);
    
    return bfilename;
}

struct node* DecryptHeader(FILE* binfile, unsigned long size){
    unsigned long letters[MAXSYMBOLS];
    for (size_t i = 0; i < MAXSYMBOLS; i++){
        fread(&letters[i], sizeof(unsigned long), 1, binfile);
    }
    struct node** nodes = GetNodesFromLC(letters, size);
    return GetHaffmanRoot(nodes, size);
}

void DecryptText(FILE* binfile, struct node* root, unsigned long cwords, char tFileName[]){
    FILE* tfile = fopen(tFileName, "w");

    char frame;
    fread(&frame, sizeof(char), 1, binfile);
    int pointer = 0;

    struct node* nodeptr = root;

    while (cwords){
      
        if (nodeptr->lcount==1){
            fwrite(&(nodeptr->letters[0]), sizeof(char), 1, tfile);
            nodeptr = root;
            cwords--;
            continue;
        }

        int bit = abs((frame >> (BITSINCHAR-pointer-1)) % 2);
        
        if (nodeptr->right->bit == bit){
            nodeptr = nodeptr->right;
        } else {
            nodeptr = nodeptr->left;
        }

        if ((++pointer) == 8){
            pointer = 0;
            fread(&frame, sizeof(char), 1, binfile);
        }
    }

    fclose(tfile);
}

void Decrypt(char* bfilename, char tFileName[]){
    FILE* bfile = fopen(bfilename, "rb");

    //read the count of bits in binary text
    unsigned long cwords, size;
    fread(&cwords, sizeof(unsigned long), 1, bfile);
    fread(&size, sizeof(unsigned long), 1, bfile);

    struct node* root = DecryptHeader(bfile, size);

    DecryptText(bfile, root, cwords, tFileName);

    fclose(bfile);
    remove(bfilename);
}

int main(int argc, char* argv[]){
    if (argc == 3 && strcmp(argv[1], "-D") == 0){
        int bfnlen = strlen(argv[2]);
        if (bfnlen < 5){
            printf("Error of binary name!\n");
            exit(-1);
        }
        char tFileName[bfnlen];
        strncpy(tFileName, argv[2], bfnlen - 4);
        tFileName[bfnlen-4] = '\0';

        Decrypt(argv[2], tFileName);
    } else if (argc == 3 && strcmp(argv[1], "-E") == 0){
        char* bfilename = Encrypt(argv[2]);
    } else{
        printf("\t-E filename|-D filename.bin");
    }
}