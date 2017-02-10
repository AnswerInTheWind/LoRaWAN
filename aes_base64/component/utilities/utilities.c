/*
(C)2015 NPLink

Description: Helper functions implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
//#include "board.h"
#include "utilities.h"

/*!
 * Redefinition of rand() and srand() standard C functions.
 * These functions are redefined in order to get the same behavior across
 * different compiler toolchains implementations.
 */
// Standard random functions redefinition start
#define RAND_LOCAL_MAX 2147483647

static unsigned long next = 1;

int rand( void )
{
    return ( ( next = next * 1103515245 + 12345 ) % RAND_LOCAL_MAX );
}

void srand( unsigned int seed )
{
    next = seed;
}
// Standard random functions redefinition end

int32_t randr( int32_t min, int32_t max )
{
    return ( int32_t )rand( ) % ( max - min + 1 ) + min;
}

void memcpy1( uint8_t *dst, uint8_t *src, uint16_t size )
{
    while( size-- )
    {
        *dst++ = *src++;
    }
}

void memset1( uint8_t *dst, uint8_t value, uint16_t size )
{
    while( size-- )
    {
        *dst++ = value;
    }
}

int8_t Nibble2HexChar( uint8_t a )
{
    if( a < 10 )
    {
        return '0' + a;
    }
    else if( a < 16 )
    {
        return 'A' + ( a - 10 );
    }
    else
    {
        return '?';
    }
}

uint16_t Find_Str(char* SourceStr,char* TargetStr)
{
  uint16_t index = 0;
  char* s_temp;
  char* m_temp;
  char* t_temp;

  if(SourceStr == 0 || TargetStr == 0)
  {
    return 0xFFFF;
  }

  for(s_temp = SourceStr; *s_temp != '\0'; s_temp++)
  {
    m_temp = s_temp;

    for(t_temp = TargetStr; *t_temp == *m_temp; t_temp++, m_temp++)
    {
    };

    if(*t_temp == '\0')
    {
      return index;
    }

    index++;
  }
  return 0xFFFF;
}

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
int __io_putchar( int c )
#else /* __GNUC__ */
int fputc( int c, FILE *stream )
#endif
{
   //return( ITM_SendChar( c ) );
	return 0 ; //add by zjh
}
