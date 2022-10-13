/**
 * Project 1 
 * Assembler code fragment for LC-2K 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "math.h"

#define MAXLINELENGTH 1000
const int NUM = 65536;
int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int findLab(char *, char [NUM][7]);
int findOff(int c, int t);
char findSec(int c, int t);
int text = 0;
int data = 0;
int count = 0;
struct symbol{
    char lab[MAXLINELENGTH];
    char tdu;
    int off;
};
struct reloc{
    int off;
    char op[MAXLINELENGTH];
    char lab[MAXLINELENGTH];
};

int findOff(int c, int t){
    if(c>=t)
        return c-t;
    return c;
}
char findSec(int c, int t){
    if(c>=t)
        return 'D';
    return 'T';
}
int findLab(char *string, char l[NUM][7]){
    for(int i = 0; i<NUM; ++i){
        if(!strcmp(l[i], string))
            return i;
    }
    return -1;
}

int main(int argc, char *argv[])
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
            argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }
    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }
    
    /* here is an example for how to use readAndParse to read a line from
        inFilePtr */
    int linee = 0;
    char labels[NUM][7];
    while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        if(findLab(label, labels) >=0 && strlen(label) > 0)
            exit(1);
        strcpy(labels[linee], label);
        if(!strcmp(opcode, ".fill"))
            ++data;
        else
            ++text;
        ++linee;
    }
    const int tdSize = data+text;
    int textData[tdSize];
    struct symbol sTable[tdSize*2];
    struct reloc rTable[tdSize*2];
    int sLen = 0; int rLen = 0;
    linee = 0;
    /* this is how to rewind the file ptr so that you start reading from the
        beginning of the file */
    rewind(inFilePtr);
    count = 0;
    while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
        if(strlen(label)>0){
            if(isupper(label[0])){
                sTable[sLen].tdu = findSec(count, text);
                sTable[sLen].off = findOff(count, text);
                strcpy(sTable[sLen].lab, label);
                sLen++;
            }
        }
        ++count;
    }
    
    linee = 0;
    rewind(inFilePtr);
    while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
        int machine = 0;
        int op = 0, a0 = 0, a1 = 0, a2 = 0;
        int exist = 0;
            if(!strcmp(opcode, "lw")){
                op = 2 << 22;
                a0 = atoi(arg0) << 19;
                a1 = atoi(arg1) << 16;
                if(!isNumber(arg2)){
                    a2 = findLab(arg2, labels);
                    if(isupper(arg2[0])){                        //symbol table
                        for(int i = 0; i<sLen; ++i){
                            if(!strcmp(sTable[i].lab, arg2))
                                exist = 1;
                        }
                    }
                    if(a2 == -1 && isupper(arg2[0])){
                        a2 = 0;
                        if(!exist){
                            strcpy(sTable[sLen].lab, arg2);
                            sTable[sLen].tdu = 'U';
                            sTable[sLen].off = 0;
                            sLen++;
                        }
                    }                                           //
                    else if(a2 == -1 && !isupper(arg2[0])){
                        exit(1);
                    }
                    rTable[rLen].off = findOff(linee, text);
                    strcpy(rTable[rLen].op, opcode);
                    strcpy(rTable[rLen].lab, arg2);
                    rLen++;
                }
                else{
                    a2 = atoi(arg2);
                }
                if(a2 > pow(2, 15) - 1 || a2 < pow(-2,15)){
                    exit(1);
                }
                a2 = a2 & 0xFFFF;
                machine = op+a0+a1+a2;
            }
            else if(!strcmp(opcode, "sw")){
                op = 3 << 22;
                a0 = atoi(arg0) << 19;
                a1 = atoi(arg1) << 16;
                if(!isNumber(arg2)){
                    a2 = findLab(arg2, labels);
                    if(isupper(arg2[0])){
                        for(int i = 0; i<sLen; ++i){
                            if(!strcmp(sTable[i].lab, arg2))
                                exist = 1;
                        }
                    }
                    if(a2 == -1 && isupper(arg2[0])){
                        a2 = 0;
                        if(!exist){
                            strcpy(sTable[sLen].lab, arg2);
                            sTable[sLen].tdu = 'U';
                            sTable[sLen].off = 0;
                            sLen++;
                        }
                    }
                    else if(a2 == -1 && !isupper(arg2[0]))
                        exit(1);
                    rTable[rLen].off = findOff(linee, text);
                    strcpy(rTable[rLen].op, opcode);
                    strcpy(rTable[rLen].lab, arg2);
                    rLen++;
                }
                else{
                    a2 = atoi(arg2);
                }
                if(a2 > pow(2, 15) - 1 || a2 < pow(-2,15)){
                    exit(1);
                }
                a2 = a2 & 0xFFFF;
                machine = op+a0+a1+a2;
            }
            else if(!strcmp(opcode, "beq")){
                op = 4 << 22;
                a0 = atoi(arg0) << 19;
                a1 = atoi(arg1) << 16;
                if(!isNumber(arg2)){
                    a2 = findLab(arg2, labels);
                    if(a2 == -1)
                        exit(1);
                    a2 = a2 - 1 - linee;
                }
                else{
                    a2 = atoi(arg2);
                }
                if(a2 > pow(2, 15) - 1 || a2 < pow(-2,15)){
                    exit(1);
                }
                a2 = a2 & 0xFFFF;
                machine = op+a0+a1+a2;
            }
        
        else if(!strcmp(opcode, "add") || !strcmp(opcode, "nor")){
            if (!strcmp(opcode, "add")) {
                op = 0 << 22;
            }
            else if(!strcmp(opcode, "nor")){
                op = 1 << 22;
            }
            a0 = (atoi(arg0) << 19);
            a1 = (atoi(arg1) << 16);
            a2 = atoi(arg2);
            machine = op + a0 + a1 + a2;
        }
        else if(!strcmp(opcode, "jalr")){
            op = 5 << 22;
            a0 = (atoi(arg0) << 19);
            a1 = (atoi(arg1) << 16);
            machine = op + a0 + a1;
        }
        else if(!strcmp(opcode, "noop") || !strcmp(opcode, "halt")){
            if(!strcmp(opcode, "noop")){
                machine = 7 << 22;
            }
            else if(!strcmp(opcode, "halt")){
                machine = 6 << 22;
            }
        }
        else if (!strcmp(opcode, ".fill")){
            if(isNumber(arg0)){
                machine = atoi(arg0);
            }
            else{
                machine = findLab(arg0, labels);
                if(machine == -1 && isupper(arg0[0])){
                    machine = 0;
                    for(int i = 0; i<sLen; ++i){
                        if(!strcmp(sTable[i].lab, arg0))
                            exist = 1;
                    }
                    if(!exist){
                        strcpy(sTable[sLen].lab, arg0);
                        sTable[sLen].tdu = 'U';
                        sTable[sLen].off = 0;
                        ++sLen;
                    }
                }
                else if(machine == -1 && !isupper(arg0[0])){
                    exit(1);
                }
                rTable[rLen].off = findOff(linee, text);
                strcpy(rTable[rLen].op, opcode);
                strcpy(rTable[rLen].lab, arg0);
                rLen++;
            }
        }
        else{
            exit(1);
        }
        //fprintf(outFilePtr, "%d\n", machine);
        textData[linee] = machine;              //stores machine into text and data array so i can print later
        ++linee;
    }
    fprintf(outFilePtr, "%d %d %d %d\n", text, data, sLen, rLen);
    for(int i = 0; i<linee; ++i){               //prints out text and data portion
        fprintf(outFilePtr, "%d\n", textData[i]);
    }
    for(int s = 0; s<sLen; ++s){
        fprintf(outFilePtr, "%s %c %d\n", sTable[s].lab, sTable[s].tdu, sTable[s].off);
    }
    for(int r = 0; r<rLen; ++r){
        fprintf(outFilePtr, "%d %s %s\n", rTable[r].off, rTable[r].op, rTable[r].lab);
    }
    exit(0);
    return 0;
}


/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int
readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
    char *arg1, char *arg2)
{
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
	/* reached end of file */
        return(0);
    }

    /* check for line too long */
    if (strlen(line) == MAXLINELENGTH-1) {
	printf("error: line too long\n");
	exit(1);
    }

    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label)) {
	/* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
        opcode, arg0, arg1, arg2);
    return(1);
}

int
isNumber(char *string)
{
    /* return 1 if string is a number */
    int i;
    return( (sscanf(string, "%d", &i)) == 1);
}


