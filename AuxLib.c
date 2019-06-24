/*
* Name: General Definitions library
* Author: Mehdi
* Version: 01
* Description:
*				The library was developed to introduce constants, Bitwise Operations and
*				the auxiliary routines which have general purpose.
*/


#include <string.h>
#include <stdlib.h>

#include "AuxLib.h"
#include "tm_stm32_hd44780.h"
#include "tm_stm32_delay.h"


/**
 * @name    LenString
 * @brief   The function counts number of char in a given string
 * @author  Mehdi
 *
 * @param	String: the given string
 * @return	cnt: number of char in a given string
*/

int8_t LenString(char *String)
{
    int cnt = 0;
    while(*String++) cnt++;
    return cnt;
}

/**
 * @name    Find_Char
 * @brief   The function tries to find char "PIN" in the received string
 * @author  Mehdi
 *
 * @param	String: the string in which three following chars whould be found
 * @param	Num_Array: Index of char "P" in the received string
 * @param	crt: The chars should be found in the received string
*/

char* Find_Char(char* String, char crt1, char crt2, char crt3)
{
	char *pch1, *pch2, *pch3;

	pch1 = strchr(String,crt1);	// Find the index of char P

	while(pch1 != NULL)
	{
		pch2 = strchr(pch1+1,crt2);
		if (pch1 == pch2 - 1)	// Find the index of char i
		{
			pch3 = strchr(pch2+1,crt3);	// Find the index of char n

			if (pch2 == pch3 -1)
			{
				break;
			}
		}
		pch1 = strchr(pch1+1,crt1);
	}
	return pch3+2;
}

/**
 * @name    Find_Str
 * @brief   The function tries to find specific substring in the received
 *		    string and then returns string according to specific order
 *		    (step of jump forward, and length of string)
 * @author  Mehdi
 *
 * @param	Str1: The string in which str2 whould be found
 * @param	str2: The string should be looked for in str1
 * @param	JumpStep: Number of char jump through str1 to find intended char
 * @param	LengthofRet: Length of chars found and returned by function
*/

char* Find_Str(char* str1, char* str2, int JumpStep, int LengthofRet)
{
    char* pchr;

	char* res = malloc(LengthofRet+1);

	pchr = strstr(str1,str2);
	strncpy(res,pchr+JumpStep,LengthofRet);
	res[LengthofRet]='\0';

	return (res);
}


/**
 * @name    Concat
 * @brief   The Routine Concatenates two strings.
 * @author  Mehdi
 *
 * @param	str1 & str2; the strings that Concatenated in the function
 * @param	result: the string results from Concatenation of the received string
 */

char* Concat(char* str1, char* str2)
{
	char* result = malloc(strlen(str1)+strlen(str2)+1);	// +1 for the zero-terminator

	strcpy(result, str1);
	strcat(result, str2);

	return result;
}


/**
 * @name    Halt
 * @brief   The Routine halts the program.
 * @author  Mehdi
 */

void Halt(void)
{
	volatile int j;

	TM_HD44780_Clear();
	TM_HD44780_Puts(5,1,"E R R O R");
	while(1)
	{
		Delayms(3000);
		j++;
	}
}
