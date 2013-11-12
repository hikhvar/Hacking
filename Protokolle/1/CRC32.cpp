//======================================================== file = crc32.c =====
//=  Program to compute CRC-32 using the "table method" for 8-bit subtracts   =
//=============================================================================
//=  Notes: Uses the standard "Charles Michael Heard" code available from     =
//=         http://cell-relay.indiana.edu/cell-relay/publications/software    =
//=         /CRC which was adapted from the algorithm described by Avarm      =
//=         Perez, "Byte-wise CRC Calculations," IEEE Micro 3, 40 (1983).     =
//=---------------------------------------------------------------------------=
//=  Build:  bcc32 crc32.c, gcc crc32.c                                       =
//=---------------------------------------------------------------------------=
//=  History:  KJC (8/24/00) - Genesis (from Heard code, see above)           =
//=============================================================================
#include "CRC32.h"

//----- Defines ---------------------------------------------------------------
#define POLYNOMIAL 0x04c11db7L      // Standard CRC-32 ppolynomial
#define BUFFER_LEN       4096L      // Length of buffer

//----- Gloabl variables ------------------------------------------------------
static unsigned int crc_table[256];       // Table of 8-bit remainders
//=============================================================================
//=  CRC32 table initialization                                               =
//=============================================================================
void gen_crc_table(void)
{
  register unsigned short int i, j;
  register unsigned int crc_accum;

  for (i=0;  i<256;  i++)
  {
    crc_accum = ( (unsigned int) i << 24 );
    for ( j = 0;  j < 8;  j++ )
    {
      if ( crc_accum & 0x80000000L )
        crc_accum = (crc_accum << 1) ^ POLYNOMIAL;
      else
        crc_accum = (crc_accum << 1);
    }
    crc_table[i] = crc_accum;
  }
}

//=============================================================================
//=  CRC32 generation                                                         =
//=============================================================================
unsigned int update_crc(unsigned int crc_accum, unsigned char *data_blk_ptr, unsigned int data_blk_size)
{
   register unsigned int i, j;

   for (j=0; j<data_blk_size; j++)
   {
     i = ((int) (crc_accum >> 24) ^ *data_blk_ptr++) & 0xFF;
     crc_accum = (crc_accum << 8) ^ crc_table[i];
   }
   crc_accum = ~crc_accum;

   return crc_accum;
}

