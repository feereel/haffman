#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <math.h>
#include "struct.h"

struct node** GetLettersCount(FILE* file, int* size){
    int letters[256];
    for (size_t i = 0; i < 256; i++){
        letters[i] = 0;
    }
    char c;
    while((c = fgetc(file)) != EOF){
        letters[(int)c]++;
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

void PrintCodes(int codes[][8]){
    for (size_t i = 0; i < 256; i++){
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

int GetCodeLenght(int code[8]){
    int c = 0;
    for (size_t i = 0; i < 8; i++){
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
        shift = 4;
        int tl = lenght;
        for (size_t i = 0; i < 4; i++){
            int bit = tl%2;
            printf("%d ", bit);
            tl >>= 1;
            buffer[(start+3-i)%BUFFSIZE] = bit;
        }
    }
    for (size_t i = 0; i < lenght; i++){
        buffer[(start + shift + i)%BUFFSIZE] = code[i];
        printf("%d ", code[i]);
    }
    printf("\n");
    
}

void AppendBInFile(FILE* file, int buffer[], int pointer){
    char frame = 0;
    for (size_t i = 0; i < 8; i++){
        frame <<= 1;
        frame += buffer[(pointer+i)%BUFFSIZE];
    }
    fwrite(&frame, sizeof(char), 1, file);
}

//write char code in format: lenght[4], code[lenght]
void EncryptHeader(FILE* binfile, int codes[][8]){
    int buffer[BUFFSIZE];
    int start = 0;
    int pointer = 0;
    int lastc = 0;
    for (size_t i = 0; i < 256; i++)
    {
        int c = GetCodeLenght(codes[i]);
        WriteInBuffer(buffer, start, codes[i], c, true);
        start +=  4 + c;
        while (start - pointer >= 8){
            AppendBInFile(binfile, buffer, pointer);
            pointer += 8;
        }
        pointer%=BUFFSIZE;
        start%=BUFFSIZE;       
    }
    if (start % 8){
        AppendBInFile(binfile, buffer, pointer);
    }
}

unsigned long EncryptText(FILE* textfile,FILE* binfile, int codes[][8]){
    fseek(textfile,0,SEEK_SET);

    unsigned long cbits = 0;
    int buffer[BUFFSIZE];
    int start = 0;
    int pointer = 0;
    char letter;
    while((letter = fgetc(textfile)) != EOF)
    {
        int c = GetCodeLenght(codes[(int)letter]);
        cbits += c;
        WriteInBuffer(buffer, start, codes[(int)letter], c, false);
        start += c;
        while (start - pointer >= 8){
            AppendBInFile(binfile, buffer, pointer);
            pointer += 8;
        }
        pointer%=BUFFSIZE;
        start%=BUFFSIZE;       
    }
    AppendBInFile(binfile, buffer, pointer);
    return cbits;
}

char* Encrypt(char filename[]){
    FILE* file = fopen(filename, "r");
    int size = 0;
    struct node** nodes = GetLettersCount(file, &size);

    pair minnodes = GetMinIPair(nodes, size);

    int rootind = GetHaffmanRoot(nodes, size);
    struct node* root = nodes[rootind]->right;
    
    int codes[256][8];
    for (size_t i = 0; i < 256; i++)
        for (size_t j = 0; j < 8; j++)
            codes[i][j] = 2;
    
    GetHaffmanCodes(nodes[rootind], codes, -1);
    char* bfilename = strcat(filename, ".bin");
    FILE* output = fopen(bfilename, "wb");
    
    //for count of bits in binary text
    unsigned long cbits = 0;
    fwrite(&cbits, sizeof(unsigned long), 1, output);

    EncryptHeader(output, codes);
    cbits = EncryptText(file ,output, codes);

    fseek(output, 0, SEEK_SET);
    fwrite(&cbits, sizeof(unsigned long), 1, output);

    fclose(output);
    fclose(file);
    remove(filename);
    
    PrintCodes(codes);

    return bfilename;
}

//read char code in format: lenght[4], code[lenght]
void DecryptHeader(FILE* binfile, int codes[][8]){
    char frame;
    fread(&frame, sizeof(char), 1, binfile);
    int pointer = 0;
    int i = 0;
    for (size_t i = 0; i < 256; i++){    
        int codel = 0;
        for (size_t j = 0; j < 4; j++){
            int bit = abs((frame >> (7-pointer)) % 2); // get next bit
            codel += bit * pow(2,3-j);
            if ((++pointer) == 8){
                pointer = 0;
                fread(&frame, sizeof(char), 1, binfile);
            }
        }
        for (size_t j = 0; j < codel; j++){
            int bit = abs((frame >> (7-pointer)) % 2);
            codes[i][j] = bit;
            if ((++pointer) == 8){
                pointer = 0;
                fread(&frame, sizeof(char), 1, binfile);
            }
        }
    }
}

void DecryptText(FILE* binfile, int codes[][8], char* tFileName){
    FILE* output = fopen(tFileName, "wb");



    fclose(output);
}

void Decrypt(char* bfilename, char* tFileName){
    FILE* bfile = fopen(bfilename, "rb");
    int codes[256][8];
    for (size_t i = 0; i < 256; i++)
        for (size_t j = 0; j < 8; j++)
            codes[i][j] = 2;

    //read the count of bits in binary text
    unsigned long cbits;
    fread(&cbits, sizeof(unsigned long), 1, bfile);
    printf("%lu\n", cbits);

    DecryptHeader(bfile, codes);
    PrintCodes(codes);

    DecryptText(bfile, codes, tFileName);

    fclose(bfile);
}

int main(int argc, char* argv[]){
    if (argc == 3 && strcmp(argv[1], "-D") == 0){
        int bfnlen = strlen(argv[2]);
        if (bfnlen < 5){
            printf("Error of binary name!\n");
            exit(-1);
        }
        char tFileName[bfnlen-4];
        strncpy(tFileName, argv[2], bfnlen - 4);
        printf("%s\n",tFileName);

        Decrypt(argv[2], tFileName);
    } else if (argc == 3 && strcmp(argv[1], "-E") == 0){
        char* bfilename = Encrypt(argv[2]);
    } else{
        printf("\t-E filename|-D filename.bin");
    }
}