#include "compiler.h"

/*
 * add new to symbol table
 *
 * k:		type of symbol
 * ptx:		tail pointer of symbol table
 * lev:		symbol is in main block or function body
 * pdx:		relative address for current variable
 */
void enter(OBJECT k, int* ptx, int lev, int* pdx)
{
	(*ptx)++;
	strcpy(table[(*ptx)].name, id);
	table[(*ptx)].kind = k;
	switch(k)
	{
		case variable:
			table[(*ptx)].level = lev;
			table[(*ptx)].adr = (*pdx);
			(*pdx)++;
			break;
		case function:
			table[(*ptx)].level = lev;
			break;
	}
}

/*
 * test current symbol's legality
 *
 * s1:		need words
 * s2:		words for recovering stage
 * n:		error type
 */
void test(bool* s1, bool* s2, int n)
{
 	if (!inset(sym, s1))
 	{
 		error(n);
 		/* skip symbols until get need words or recovering words */
 		while ((!inset(sym, s1)) && (!inset(sym, s2)))
 		{
 			getsym();
 		}
 	}
 }

/*
 * searching for the identity's position in symbol table
 * 0 represents not exists
 *
 * id:      name of identity
 * tx:      current tail pointer of symbol table
 */
int position(char* id, int tx)
{
    int i = tx;
    strcpy(table[0].name, id);
    while (strcmp(table[i].name, id) != 0)
        --i;
    return i;
}

/*
 * find base address for function with difference of level
 */
int base(int l, int* s, int b)
{
    int b1;
    b1 = b;
    while (l > 0)
    {
        b1 = s[b1];
        l--;
    }
    return b1;
}


/*
 * generate fct codes
 *
 * x: Instruction.f
 * y: Instruction.l
 * z: Instruction.a
 */
void gen(FCT x, int y, int z)
{
    if (cx >= MAX_CX)
    {
        error(60);
    }
    if (z >= BOUND_ADR)
    {
        error(61);
    }
    code[cx].f = x;
    code[cx].l = y;
    code[cx].a = z;
    cx++;
}

/*
 * interpret the intermediate code
 */
void interpret()
{
    int p = 0;      /* pointer of code */
    int b = 1;      /* base address of code */
    int t = 0;      /* pointer of stack top */
    Instruction i;  /* store current code */
    int s[SIZE_STACK];  /* stack */

    s[0] = 0;
    s[1] = 0;
    s[2] = 0;
    s[3] = 0;
    do {
        i = code[p];    // load current code
        p = p + 1;
        switch (i.f)
        {
            case lit:   /* load a onto stack top */
                t = t + 1;
                s[t] = i.a;
                break;
            case opr:   /* math & logic operation */
                switch (i.a)
                {
                    case 0: /* return after function calling */
                        t = b - 1;
                        p = s[t + 3];
                        b = s[t + 2];
                        break;
                    case 1: /* inverse of stack top */
                        s[t] = -s[t];
                        break;
                    case 2: /* push stack top and second sum into stack */
                        t = t - 1;
                        s[t] = s[t] + s[t + 1];
                        break;
                    case 3: /* stack second minus top */
                        t = t - 1;
                        s[t] = s[t] - s[t + 1];
                        break;
                    case 4: /* stack second times top */
                        t = t - 1;
                        s[t] = s[t] * s[t + 1];
                        break;
                    case 5: /* stack second slash or mod top */
                        t = t - 1;
                        if (i.l == 0)
                        {
	                        s[t] = s[t] / s[t + 1];
                        }
                        else if (i.l == 1)
                        {
                        	s[t] = s[t] % s[t + 1];
                        }
                        break;
                    case 6: /* odd judgement of stack top */
                        s[t] = s[t] & 1;
                        break;
                    case 8: /* stack top and second equality judgement */
                        t = t - 1;
                        s[t] = (s[t] == s[t + 1]);
                        break;
                    case 9: /* stack top and second not-equality judgement */
                        t = t - 1;
                        s[t] = (s[t] != s[t + 1]);
                        break;
                    case 10:/* stack second less than top judgement */
                        t = t - 1;
                        s[t] = (s[t] < s[t + 1]);
                        break;
                    case 11:/* stack second larger than or equal to top judgement */
                        t = t - 1;
                        s[t] = (s[t] >= s[t + 1]);
                        break;
                    case 12:/* stack second larger than top judgement */
                        t = t - 1;
                        s[t] = (s[t] > s[t + 1]);
                        break;
                    case 13:/* stack second less than or equal to top judgement */
                        t = t - 1;
                        s[t] = (s[t] <= s[t + 1]);
                        break;
                    case 14:/* output stack top */
                        printf("%d", s[t]);
                        fprintf(fresult, "%d", s[t]);
                        t = t - 1;
                        break;
                    case 15:/* output line break */
                        puts("");
                        fprintf(fresult, "\n");
                        break;
                    case 16:/* read an input into stack top */
                        t = t + 1;
                        printf("Enter the variable:\n");
                        scanf("%d", &(s[t]));
                        //fprintf(fresult, "%d\n", s[t]);
                        break;
                }
                break;
            case lod:   /* push the value in memory into stack with current function base and offset address */
                t = t + 1;
                s[t] = s[base(i.l, s, b) + i.a];
                break;
            case sto:   /* save the stack top value into memory with current function base and offset address */
                s[base(i.l, s, b) + i.a] = s[t];
                t = t - 1;
                break;
            case cal:   /* call function */
                s[t + 1] = base(i.l, s, b);
                s[t + 2] = b;
                s[t + 3] = p;
                b = t + 1;
                p = i.a;
                break;
            case ini:   /* initialize a size data space for function */
                t = t + i.a;
                break;
            case jmp:   /* unconditional jump */
                p = i.a;
                break;
            case jne:   /* not-equal conditional jump */
                if (s[t] == 0)
                    p = i.a;
                t = t - 1;
                break;
            case jeq:   /* equal conditional jump */
                if (s[t] != 0)
                    p = i.a;
                t = t - 1;
                break;
            case in:	/* read an input into stack top */
                t = t + 1;
                printf("Enter the variable:\n");
                scanf("%d", &(s[t]));
                //fprintf(fresult, "%d\n", s[t]);
                break;
            case out:	/* output stack top */
                printf("%d\n", s[t]);
                fprintf(fresult, "%d", s[t]);
				t = t - 1;
				break;
        }
    } while (p != 0);
}



/*
 * output code list
 */
void listcode(int cx0)
{
    if(listswitch)
    {
        puts("");
        for(int i = cx0; i < cx; ++i)
        {
            // printf("%d %s %d %d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
            fprintf(fcode,"%d %s %d %d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
        }
    }
}

/*
 * output all codes
 */
void listall()
{
    if(listswitch)
    {
        for (int i = 0; i < cx; ++i)
        {
            // printf("%d %s %d %d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
            fprintf(fcode,"%d %s %d %d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
        }
    }
}

int inset(int e, bool* s)
{
	return s[e];
}

int addset(bool* sr, bool* s1, bool* s2, int n)
{
	int i;
	for (i=0; i<n; i++)
	{
		sr[i] = s1[i]||s2[i];
	}
	return 0;
}

int subset(bool* sr, bool* s1, bool* s2, int n)
{
	int i;
	for (i=0; i<n; i++)
	{
		sr[i] = s1[i]&&(!s2[i]);
	}
	return 0;
}

int mulset(bool* sr, bool* s1, bool* s2, int n)
{
	int i;
	for (i=0; i<n; i++)
	{
		sr[i] = s1[i]&&s2[i];
	}
	return 0;
}
