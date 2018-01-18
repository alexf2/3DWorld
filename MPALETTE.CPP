#include "mpalette.hpp"

#include <windows.h>

extern HINSTANCE hInstRC;

HPALETTE CreateMeanPalette()
 {
   LPLOGPALETTE  lpPal;   
   HANDLE        hLogPal;  
   
   if( !(hLogPal = GlobalAlloc( GHND, sizeof(LOGPALETTE) + sizeof(PALETTEENTRY)*236 )) )
     return NULL;

   lpPal = (LPLOGPALETTE)GlobalLock( hLogPal );
   lpPal->palVersion = 0x300;
   lpPal->palNumEntries = 236;

   double dCol, dStep = 256.0 / 78.0;
   dCol = dStep;
   for( int i = 0; i < 234; dCol += dStep )
    {
      lpPal->palPalEntry[i].peRed = (BYTE)dCol;
      lpPal->palPalEntry[i].peGreen = 0;
      lpPal->palPalEntry[i].peBlue = 0;      
      lpPal->palPalEntry[i++].peFlags = NULL;

      lpPal->palPalEntry[i].peRed = 0;
      lpPal->palPalEntry[i].peGreen = (BYTE)dCol;
      lpPal->palPalEntry[i].peBlue = 0;      
      lpPal->palPalEntry[i++].peFlags = NULL;

      lpPal->palPalEntry[i].peRed = 0;
      lpPal->palPalEntry[i].peGreen = 0;
      lpPal->palPalEntry[i].peBlue = (BYTE)dCol;
      lpPal->palPalEntry[i++].peFlags = NULL;
    }

   lpPal->palPalEntry[234].peFlags = PC_RESERVED;
   lpPal->palPalEntry[235].peFlags = PC_RESERVED;

   HPALETTE hpalPalette = CreatePalette( lpPal );
   GlobalUnlock( hLogPal );
   GlobalFree( hLogPal );

   return hpalPalette;
 }
