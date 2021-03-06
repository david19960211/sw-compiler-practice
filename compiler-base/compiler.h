#ifndef COMPILER_H_INCLUDED
#define COMPILER_H_INCLUDED

#include <stdio.h>
#include "setting.h"


typedef struct TableStruct
{
    char name[LEN_ID];
    OBJECT kind;
    int val;
    int level;
    int adr;
    int size;
}TableStruct;

typedef struct Instruction
{
	FCT f;	    // code type
	int l;		// level distance
	int a;		// operation number for different code
}Instruction;

/* global variables for compiler */

unsigned char ch;		// store loaded character for getch
SYMBOL sym;		        // current symbol
char id[LEN_ID+1];		// current identity
int num;				// current number
int cc, ll;				// counter for getch
int cx;					// pointer for virtual machine
char line[LEN_L+1]; 	// line buffer
char a[LEN_ID+1];		// symbol buffer
int line_num;           // counter for read line
int err_num;			// counter for known errors

TableStruct table[SIZE_TB];
Instruction code[MAX_CX];

/* output tools */

bool listswitch;		// 1 then shows fct
bool tableswitch;		// 1 then shows symbol table

FILE* fin;
FILE* ftable;
FILE* fcode;
FILE* ferr;
FILE* fresult;
FILE* foutput;

/* function declarations */
void init_proc();
void init_errors();
void init_setting();
void init();

void error(int n);
void getsym();
void getch();
void gen(enum FCT x, int y, int z);
void test(bool* s1, bool* s2, int n);
int inset(int e, bool* s);
int addset(bool* sr, bool* s1, bool* s2, int n);
int subset(bool* sr, bool* s1, bool* s2, int n);
int mulset(bool* sr, bool* s1, bool* s2, int n);

void parsing();
void processing();
void interpret();
void listcode(int cx0);
void listall();
void problem(int lev, int tx, bool* fsys);
void declaration(enum OBJECT tp, int* ptx, int lev, int* pdx);
void statement(bool* fsys, int* ptx, int lev);
void condition(bool* fsys, int* ptx, int lev);
void expression(bool* fsys, int* ptx, int lev);
void term(bool* fsys, int* ptx, int lev);
void factor(bool* fsys, int* ptx, int lev);
void enter(enum OBJECT k, int* ptx, int lev, int* pdx);
int position(char* idt, int tx);
int base(int l, int* s, int b);

#endif // COMPILER_H_INCLUDED
