/*
* Name: General Definitions library
* Author: Mehdi
* Version: 01
* Description:
*				The library was developed to introduce constants, Bitwise Operations and 
*				the auxiliary routines which have general purpose.
*/



#ifndef AUXLIB_H_
#define AUXLIB_H_

#include <string.h>
#include <stdlib.h>


// Characters
#define FALSE 0
#define TRUE  1

#define  ODD		3
#define  EVEN		2
#define  RESERVE    1
#define  NONE		0

#define shortdelay 1
#define longdelay 3

/**********************************************************************
*							Type definition Block
***********************************************************************/
typedef char			C8;
typedef unsigned char   U8;
typedef signed char		S8;
typedef unsigned short	U16;
typedef signed short	S16;
typedef unsigned long	U32;
typedef signed long		S32;

// Pointers
typedef char*				C8_ptr;
typedef unsigned char*		U8_ptr;
typedef signed char*		S8_ptr;
typedef unsigned short*		U16_ptr;
typedef signed short*		S16_ptr;
typedef unsigned long*		U32_ptr;
typedef signed long*		S32_ptr;


/**********************************************************************
*							 Bitwise Operations
***********************************************************************/
#define NOT(x)   (~(x))
#define OR(x,y)  ((x) | (y))
#define AND(x,y) ((x) & (y))
#define NOR(x,y) ((x) | (~(y)))
#define XOR(x,y) ((x) ^ (y))


// Bitwise Assignment Operations
#define BIT(x)         (1 << (x))

#define SETBIT(x,y)   ((x) |= (y))
#define CLEARBIT(x,y) ((x) &= (~(y)))
#define EXCLBIT(x,y)  ((x) ^= (y))

#define SETBITS(x,y)    (SETBITS((x),(BIT(y))))
#define CLEARBITS(x,y)  (CLEARBITS((x),(BIT(y))))
#define EXCLBITS(x,y)   ((x) ^= (BIT(y)))

#define QUERYBITS(x,y)  ((x) & (BIT(y)))

int8_t LenString(char *String);
 
char* Find_Char(char* String, char crt1, char crt2, char crt3);

char* Find_Str(char* str1, char* str2, int JumpStep, int LengthofRet);

char* Concat(char* str1, char* str2);
 
void Halt(void);



#endif /* AUXLIB_H_	*/
