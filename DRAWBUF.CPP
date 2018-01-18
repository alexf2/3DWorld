#pragma option -v-

#include "drawbuf.hpp"



TDrawTitleBuf::TDrawTitleBuf()
 {
   hbm = NULL; w = h = 0;
 }

TDrawTitleBuf::~TDrawTitleBuf()
 {
   if( hbm ) { DeleteObject( hbm ); hbm = NULL; }  
 }

void TDrawTitleBuf::InitBuff( HDC hdc, int size, int high )
 {
   if( hbm ) { DeleteObject( hbm ); hbm = NULL; }
   w = size ? size:GetSystemMetrics( SM_CXSCREEN );
   //h = GetSystemMetrics( SM_CYCAPTION );
   h = high;
   hbm = CreateCompatibleBitmap( hdc, w, h );
 }

void TDrawTitleBuf::RequiestSize( HDC hdc, int size, int high )
 {
   if( w < size || h < high )  InitBuff( hdc, size, high );
 }

#pragma option -v.
