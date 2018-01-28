#include "rgb.hpp"
#include "autorc.hpp"
#include "sorte.hpp"

#include <bwcc.h>
#include <applicat.h>
#include <stdio.h>
#include <limits.h>
#include <windowsx.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


//#include "rgb.inc"

#pragma option -G

//#define  _3DWDEBUG_


#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;


typedef BYTE near pascal (*PFindNearstColor)(  RTIRGB rPic, PTColorTable pTbl,
  int iSize, PTNoisyDta pNoise, int* pIndex );

static BYTE near pascal FindNearstColorNoisy( RTIRGB rPic, PTColorTable pTbl,
  int iSize, PTNoisyDta pNoise, int* pIndex );

static BYTE near pascal FindNearstColorNorm( RTIRGB rPic,
  PTColorTable pTbl, int iSize, PTNoisyDta, int* );


static BOOL NoisyMarginal( TRGB  huge* pBuf, DWORD dwSize, PTNoisyDta pNoise, PTIndicator pInd );


inline int abs_( int iVal )
 {
   return (iVal < 0) ? -iVal:iVal; 
 }


inline int max( int v1, int v2 )
 {
   return (v1 > v2) ? v1:v2;
 }

inline BYTE max( BYTE v1, BYTE v2 )
 {
   return (v1 > v2) ? v1:v2;
 }


static HWND GetMainWnd()
 {
   HWND hwnd;
   PTApplication pApp = GetApplicationObject();
   if( pApp && pApp->isA() == applicationClass )
   hwnd = pApp->MainWindow->HWindow;
   else hwnd = NULL;

   return hwnd;
 }


PTColorTable TrueColorTo256( TRGB  huge* pBuf, DWORD dwSize, int iTotalIndex, BOOL bFlTrueColor,
			     TConvertAlho caAlho, int iWidth, int iHigh,
			     PTNoisyDta pNoise,
			     PTIndicator pInd )
 {
   if( iTotalIndex > 256 || iTotalIndex < 16 )
    {
      BWCCMessageBox( GetMainWnd(), "Internal error: bad size palette",
        "Error",
	MB_ICONSTOP | MB_OK | MB_APPLMODAL
		    );
      return NULL;
    }  

    if( caAlho & CA_System )
     {
       if( pInd )
        {
          pInd->UpdateStr( 2, "Get system palette" );
          pInd->Init( 16 );
          pInd->Show();
	  //pInd->WorkMessageAdd( 2 );
	  pInd->AdaptBounds();
        }

       PTColorTable pctTbl = new TColorTable[ 16 ];
       if( !pctTbl )
        {
          PTApplication pApp = GetApplicationObject();
          if( pApp && pApp->isA() == applicationClass )
	    pApp->Error( EM_OUTOFMEMORY );
          return NULL;
        }

       HDC hdc = GetDC( NULL );
       if( GetDeviceCaps( hdc, NUMCOLORS ) <= 20 )
        {         
	  PALETTEENTRY pe[16];
	  GetSystemPaletteEntries( hdc, 0, 16, pe );
	  for( int i = 0; i < 16; i++ )
           {
	     pctTbl[ i ].rgbRed   = pe[ i ].peRed;
	     pctTbl[ i ].rgbGreen = pe[ i ].peGreen;
	     pctTbl[ i ].rgbBlue  = pe[ i ].peBlue;
           }
        }
       else
        {
	  pctTbl[ 0 ] << TRGB( 0, 0, 0 );
	  pctTbl[ 1 ] << TRGB( 0x80, 0, 0);
	  pctTbl[ 2 ] << TRGB( 0, 0x80, 0 );
	  pctTbl[ 3 ] << TRGB( 0x80, 0x80, 0 );
	  pctTbl[ 4 ] << TRGB( 0, 0, 0x80 );
	  pctTbl[ 5 ] << TRGB( 0x80, 0, 0x80 );
	  pctTbl[ 6 ] << TRGB( 0, 0x80, 0x80 );
	  pctTbl[ 7 ] << TRGB( 0xC0, 0xC0, 0xC0 );
	  pctTbl[ 8 ] << TRGB( 0xC0, 0xDC, 0xC0 );
	  pctTbl[ 9 ] << TRGB( 0xA6, 0xCA, 0xF0 );
	  pctTbl[ 10 ] << TRGB( 0xFF, 0xFB, 0xF0 );
	  pctTbl[ 11 ] << TRGB( 0xA0, 0xA0, 0xA4 );
	  pctTbl[ 12 ] << TRGB( 0x80, 0x80, 0x80 );
	  pctTbl[ 13 ] << TRGB( 0xFF, 0, 0 );
	  pctTbl[ 14 ] << TRGB( 0, 0xFF, 0 );
	  pctTbl[ 15 ] << TRGB( 0xFF, 0xFF, 0xFF );
	}
       ReleaseDC( NULL, hdc );

       if( Diffusion( pctTbl, pBuf, dwSize, 16, CA_Diffuzion, iWidth,
		      iHigh, pNoise, pInd ) == FALSE
	 )
        {
	  delete []pctTbl;
	  pctTbl = NULL;
	}

       return pctTbl;
     }


   PTColorTable pctTbl = new TColorTable[ iTotalIndex ];
   if( !pctTbl )
    {
      PTApplication pApp = GetApplicationObject();
      if( pApp && pApp->isA() == applicationClass )
	pApp->Error( EM_OUTOFMEMORY );
      return NULL;
    }

   _fmemset( pctTbl, 0, sizeof(TColorTable) * iTotalIndex );
   
   int iCount = 2;
   char cBuf[128];
   sprintf( cBuf, "Create [%d items] palette...", iTotalIndex );
   if( pInd )
    {
      pInd->UpdateStr( 2, cBuf );
      pInd->Init( dwSize );
      pInd->Show();
      pInd->WorkMessageAdd( 2 );
      pInd->AdaptBounds();
    }

   pctTbl[ 0 ].rgbRed = 0;
   pctTbl[ 0 ].rgbGreen = 0;
   pctTbl[ 0 ].rgbBlue = 0;

   pctTbl[ 1 ].rgbRed = 255;
   pctTbl[ 1 ].rgbGreen = 255;
   pctTbl[ 1 ].rgbBlue = 255;

   PTRGB pBuf_ = pBuf;   
   for( DWORD dwI = dwSize; dwI > 0; dwI--, pBuf_++ )
    {
      BOOL bFlFound = FALSE;

      PTColorTable pTbl = pctTbl;
      for( int i = 0; i < iCount; i++, pTbl++ )
	if( *pTbl == *pBuf_ ) { bFlFound = TRUE; break; }

      pBuf_->index = (bFlFound == TRUE) ? i:iCount;

      if( bFlFound == FALSE ) pctTbl[ iCount++ ] << *pBuf_;

      if( iCount >= iTotalIndex )
       {
         int res;
	 if( caAlho & CA_Quantize )
	   res = Quantization( pctTbl, pBuf, dwSize, iTotalIndex, caAlho, iWidth, iHigh, pNoise, pInd );
	 else
	   //res = MediumSection( pctTbl, pBuf, dwSize, iTotalIndex, caAlho, iWidth, iHigh, pInd );
	   res = MediumSectionF( pctTbl, pBuf, dwSize, iTotalIndex, caAlho, iWidth, iHigh, pNoise, pInd );

	 if( res )
	   return pctTbl;
	 else
	  {
	    delete []pctTbl;
	    return NULL;
          }
       }

      if( pInd && pInd->WorkMessageAdd(1) == TRUE )
       {
         delete []pctTbl;
	 return NULL;
       }
    }

   if( pInd ) pInd->WorkIs100();

   return pctTbl;      
 }

struct TColorCell;
typedef TColorCell* PTColorCell;
typedef TColorCell& RTColorCell;
//_PTRDEF(TColorCell)
//_REFDEF(TColorCell)

struct TColorCell
 {
   BYTE r1, g1, b1,
	r2, g2, b2;
   BYTE rMin, rMax, gMin, gMax, bMin, bMax;
   BYTE bMaxAdge;

   BYTE index;
   BYTE rMedium, gMedium, bMedium;
   DWORD dwNumbPoints;

   TColorCell() {};
   TColorCell( BYTE r1_, BYTE g1_, BYTE b1_, BYTE r2_, BYTE g2_, BYTE b2_ )
    {
      r1 = r1_; g1 = g1_; b1 = b1_;
      r2 = r2_; g2 = g2_; b2 = b2_;
    }

   void ResetBounds()
    {
      rMin = gMin = bMin = 255;
      rMax = gMax = bMax = 0;
      dwNumbPoints = 0;
    }

   void Compress();
 };

void TColorCell::Compress()
 {
   if( dwNumbPoints )
    {
      r1 = rMin; g1 = gMin; b1 = bMin;
      r2 = rMax; g2 = gMax; b2 = bMax;
    }

   BYTE  dR = r2 - r1,
         dG = g2 - g1,
         dB = b2 - b1;

   bMaxAdge = max( dR, max(dG, dB) );
 }

struct TColorCellHold;
typedef TColorCellHold* PTColorCellHold;
typedef TColorCellHold& RTColorCellHold;
//_PTRDEF(TColorCellHold)
//_REFDEF(TColorCellHold)

struct TColorCellHold: public TColorCell
 {
   TAutoMemoryHugeRc<TRGB huge* huge*> mrcIn;
   DWORD dwContain;

   TColorCellHold() {};
   TColorCellHold( BYTE r1_, BYTE g1_, BYTE b1_, BYTE r2_, BYTE g2_, BYTE b2_,
		   DWORD dwContain_ ):
     TColorCell( r1_, g1_, b1_, r2_, g2_, b2_ )
    {
      dwContain = dwContain_;
      mrcIn = (TRGB huge* huge*)farcalloc( dwContain, sizeof(TRGB huge*) );
    }

   void Init( DWORD dwContain_ )
    {      
      dwContain = dwContain_;
      mrcIn = (TRGB huge* huge*)farcalloc( dwContain, sizeof(TRGB huge*) );
    }

   RTColorCellHold operator=( RTColorCellHold rC )
    {
      //*PTColorCell(this) = RTColorCell(rC);
      r1 = rC.r1;
      g1 = rC.g1;
      b1 = rC.b1;
      r2 = rC.r2;
      g2 = rC.g2;
      b2 = rC.b2;
      rMin = rC.rMin;
      rMax = rC.rMax;
      gMin = rC.gMin;
      gMax = rC.gMax;
      bMin = rC.bMin;
      bMax = rC.bMax;
      bMaxAdge = rC.bMaxAdge;

      index = rC.index;
      //BYTE rMedium, gMedium, bMedium;
      dwNumbPoints = rC.dwNumbPoints;

      dwContain = rC.dwContain;
      mrcIn = rC.mrcIn();
      rC.mrcIn.ClearRc();

      return *this;
    }
   //void Compress();
 };
     /*
void TColorCellHold::Compress()
 {
   ResetBounds();

   TRGB huge* huge* pBuf_ = mrcIn();
   for( DWORD i = dwContain; i > 0; i--, pBuf_++ )
    {     
      if( rMin > (*pBuf_)->r ) rMin = (*pBuf_)->r;
         //else
      if( rMax < (*pBuf_)->r ) rMax = (*pBuf_)->r;

      if( gMin > (*pBuf_)->g ) gMin = (*pBuf_)->g;
         //else
      if( gMax < (*pBuf_)->g ) gMax = (*pBuf_)->g;

      if( bMin > (*pBuf_)->b ) bMin = (*pBuf_)->b;
	 //else
      if( bMax < (*pBuf_)->b ) bMax = (*pBuf_)->b;
    }

   TColorCell::Compress();
 }     */

static void near pascal ResetBounds( TAutoMemoryRc<PTColorCell>& rPal, int iSizePal )
 {
   PTColorCell pCell = rPal();

   for( int i = iSizePal; i > 0; i--, pCell++ )
     pCell->ResetBounds();
 }

static void near pascal ResetBounds( TAutoMemoryRc<PTColorCellHold>& rPal, int iSizePal )
 {
   PTColorCellHold pCell = rPal();
   
   for( int i = iSizePal; i > 0; i--, pCell++ )
     pCell->ResetBounds();
 }


inline BOOL PointInCell( RTColorCell rC, TRGB huge& rRGB )
 {
   return rC.r1 <= rRGB.r && rC.g1 <= rRGB.g && rC.b1 <= rRGB.b &&
	  rC.r2 >= rRGB.r && rC.g2 >= rRGB.g && rC.b2 >= rRGB.b;
 }

static PTColorCell near pascal InWhatCell( TRGB huge& rRGB, TAutoMemoryRc<PTColorCell>& rPal, int iSizePal )
 {
   PTColorCell pCell = rPal();

   for( int i = iSizePal; i > 0; i--, pCell++ )
     if( PointInCell( *pCell, rRGB ) == TRUE ) return pCell;



   /*if( iCou > 1 )
    {
      BWCCMessageBox( GetMainWnd(), "Internal application error: Point belong in 2 cell",
	      "Error",
	      MB_ICONSTOP | MB_OK | MB_APPLMODAL );
    }*/ 

   return NULL;
 }

static void near pascal Compress( TAutoMemoryRc<PTColorCell>& rPal, int iSizePal )
 {
   PTColorCell pCell = rPal();

   for( int i = iSizePal; i > 0 ; i--, pCell++ )
     pCell->Compress();
 }

static void near pascal Compress( TAutoMemoryRc<PTColorCellHold>& rPal, int iSizePal )
 {
   PTColorCellHold pCell = rPal();

   for( int i = iSizePal; i > 0 ; i--, pCell++ )
     pCell->Compress();
 }


static char near pascal TheLongest( RTColorCell rC )
 {
#ifdef _3DWDEBUG_
   if( rC.r1 > rC.r2 || rC.g1 > rC.g2 || rC.b1 > rC.b2 )
     BWCCMessageBox( GetMainWnd(), "Internal application error: left bound cell > right",
       "Error",
       MB_ICONSTOP | MB_OK | MB_APPLMODAL );
#endif

   BYTE dR = rC.r2 - rC.r1, dG = rC.g2 - rC.g1, dB = rC.b2 - rC.b1;
   BYTE dMax = max( dR, max(dG, dB) );

   if( dMax == dR ) return 'r';
   else if( dMax == dG ) return 'g';

   return 'b';
 }

//typedef int _EXPFUNC2 comparF( const void _FAR *, const void _FAR * );
/*typedef int TCmpDirect( RTRGB rRGB1, RTRGB rRGB2 );
typedef TCmpDirect* PTCmpDirect;*/
typedef BYTE near pascal TAcess( TRGB huge& rC );
typedef int near pascal TDiv( TRGB huge& rPointDiv, RTColorCell rCSource,
			      RTColorCell rCDest1, RTColorCell rCDest2 );
typedef TDiv near* PTDiv;
typedef TAcess near* PTAcess;


static int CmpR( const void huge* p1, const void huge* p2 )
 {  
   return int(PTRGB(p1)->r) - int(PTRGB(p2)->r);
 }

static int CmpG( const void huge* p1, const void huge* p2 )
 {  
   return int(PTRGB(p1)->g) - int(PTRGB(p2)->g);
 }

static int CmpB( const void huge* p1, const void huge* p2 )
 {
   return int(PTRGB(p1)->b) - int(PTRGB(p2)->b);
 }

static int CmpR2( const void huge* p1, const void huge* p2 )
 {
   return (*((TRGB huge* huge*)p1))->r - (*((TRGB huge* huge*)p2))->r;
 }

static int CmpG2( const void huge* p1, const void huge* p2 )
 {
   return (*((TRGB huge* huge*)p1))->g - (*((TRGB huge* huge*)p2))->g;
 }

static int CmpB2( const void huge* p1, const void huge* p2 )
 {
   return (*((TRGB huge* huge*)p1))->b - (*((TRGB huge* huge*)p2))->b;
 }


static TDiv DivR
 {
   rCDest1 = TColorCell( rCSource.r1, rCSource.g1, rCSource.b1,
			 rPointDiv.r, rCSource.g2, rCSource.b2 );

   rCDest2 = TColorCell( rPointDiv.r + 1, rCSource.g1, rCSource.b1,
			 rCSource.r2, rCSource.g2, rCSource.b2 );

#ifdef _3DWDEBUG_
   return (rPointDiv.r > rCSource.r1 && int(rPointDiv.r) < int(rCSource.r2 - 1));
#else
  return 1;
#endif 
 }

static TDiv DivG
 {
   rCDest1 = TColorCell( rCSource.r1, rCSource.g1, rCSource.b1,
			 rCSource.r2, rPointDiv.g, rCSource.b2 );

   rCDest2 = TColorCell( rCSource.r1, rPointDiv.g + 1, rCSource.b1,
			 rCSource.r2, rCSource.g2, rCSource.b2 );

#ifdef _3DWDEBUG_
   return (rPointDiv.g > rCSource.g1 && int(rPointDiv.g) < int(rCSource.g2 - 1));
#else
  return 1;
#endif 
 }


static TDiv DivB
 {
   rCDest1 = TColorCell( rCSource.r1, rCSource.g1, rCSource.b1,
			 rCSource.r2, rCSource.g2, rPointDiv.b );

   rCDest2 = TColorCell( rCSource.r1, rCSource.g1, rPointDiv.b + 1,
			 rCSource.r2, rCSource.g2, rCSource.b2 );

#ifdef _3DWDEBUG_
   return (rPointDiv.b > rCSource.b1 && int(rPointDiv.b) < int(rCSource.b2 - 1));
#else
  return 1;
#endif
 }

static TAcess AR
 {
   return rC.r;
 }

static TAcess AG
 {
   return rC.g;
 }

static TAcess AB
 {
   return rC.b;
 }

#ifdef _3DWDEBUG_
void DumpDispers( TAutoMemoryRc<PTColorCell>& rPal, int iPalSize, DWORD dwSize,
	           LPSTR lpName )
 {
   FILE *f = fopen( lpName , "w+t" );
   if( !f ) return;

   DWORD dwSummPoints = 0;
   for( int i = 0; i < iPalSize; i++ )
    {
      char cBuf[1024];

      sprintf( cBuf, "%3d   points: %lu",
        i,	
	rPal()[i].dwNumbPoints );

      dwSummPoints += rPal()[i].dwNumbPoints;

      fprintf( f, "%s\n", cBuf );
    }
   fprintf( f, "\n\nSumm of points: %lu\n\n", dwSummPoints );

   fclose( f );
 }
#endif

static int CmpDw( const void _FAR* p1, const void _FAR* p2 )
 {
   return *(DWORD*)p1 - *(DWORD*)p2;
 }

#ifdef _3DWDEBUG_
void DumpDispers( TAutoMemoryRc<PTColorCellHold>& rPal, int iPalSize, DWORD dwSize,
	           LPSTR lpName )
 {
   FILE *f = fopen( lpName , "w+t" );
   if( !f ) return;

   DWORD *pdwDsp = new DWORD[ iPalSize ];
   if( !pdwDsp ) { fclose( f ); return; }
   DWORD dwMax = 0;
   for( int k = 0; k < iPalSize; k++ )
    {
      pdwDsp[ k ] = rPal()[ k ].dwNumbPoints;
      dwMax = (dwMax < pdwDsp[ k ]) ? pdwDsp[ k ]:dwMax;
    }

   DWORD dwDiv = 1;
   if( dwMax > DWORD(INT_MAX) ) dwDiv = dwMax / INT_MAX;
   dwMax /= dwDiv;

   qsort( (BYTE*)pdwDsp, iPalSize, sizeof(DWORD), CmpDw );

   PTWindow pWnd = new TWindow( NULL, "Dispersion" );
   pModule->MakeWindow( pWnd );
   pWnd->Show( SW_SHOW );
   HWND hwndDiv = pWnd->HWindow;
   SetWindowPos( hwndDiv, NULL, 0, 0, 256, 256, SWP_NOZORDER | SWP_NOMOVE );
   HDC hdc = GetDC( hwndDiv );

   RECT r; GetClientRect( hwndDiv, &r );
   SetMapMode( hdc, MM_ISOTROPIC );
   SetWindowExt( hdc, 256, dwMax );
   SetViewportExt( hdc, r.right, -r.bottom );
   SetViewportOrg( hdc, 0, r.bottom );


   int x, y; x = y = 0;
   DWORD dwSummPoints = 0;
   for( int i = 0; i < iPalSize; i++ )
    {
      char cBuf[1024];

      sprintf( cBuf, "%3d   points: %lu",
        i,	
	pdwDsp[ i ] );

      dwSummPoints += pdwDsp[ i ];

      fprintf( f, "%s\n", cBuf );

      MoveTo( hdc, x, y );
      y = pdwDsp[ i ] / dwDiv; x = i;
      LineTo( hdc, x, y );
    }
   fprintf( f, "\n\nSumm of points: %lu\n\n", dwSummPoints );

   ReleaseDC( hwndDiv, hdc );
   fclose( f );  
   delete pdwDsp;
 }
#endif

#ifdef _3DWDEBUG_
void Dump( TAutoMemoryRc<PTColorCell>& rPal, int iPalSize, TRGB huge* pBits,
	   DWORD dwSize, LPSTR lpName )
 {
   FILE *f = fopen( lpName , "w+t" );
   if( !f ) return;

   DWORD dwSummPoints = 0;
   for( int i = 0; i < iPalSize; i++ )
    {
      char cBuf[1024];

      sprintf( cBuf, "%3d  (%3u,%3u,%3u)-(%3u,%3u,%3u)  min max:(%3u,%3u,%3u)-(%3u,%3u,%3u)  index: %u, points: %lu",
        i,
	(int)(rPal()[i].r1), (int)(rPal()[i].g1), (int)(rPal()[i].b1),
	(int)(rPal()[i].r2), (int)(rPal()[i].g2), (int)(rPal()[i].b2),
	(int)(rPal()[i].rMin), (int)(rPal()[i].gMin), (int)(rPal()[i].bMin),
	(int)(rPal()[i].rMax), (int)(rPal()[i].gMax), (int)(rPal()[i].bMax),
	(int)(rPal()[i].index),
	rPal()[i].dwNumbPoints );

      dwSummPoints += rPal()[i].dwNumbPoints;

      fprintf( f, "%s\n", cBuf );
    }

   fprintf( f, "\n\nSumm of points: %lu\n\n", dwSummPoints );

   DWORD c = 0;
   while( c < dwSize )
    {
      fprintf( f, " " );
      for( int i = 0; i < 5 && c < dwSize; i++, c++ )
       {
	 char cBuf[128];
	 sprintf( cBuf, "%3u %3u %3u   ", pBits[c].r, pBits[c].g, pBits[c].b );
	 fprintf( f, "%s", cBuf );
       }
      fprintf( f, "\n" );
    } 

   fclose( f );
 }
#endif

#ifdef _3DWDEBUG_
void DumpArr( TRGB huge* pArr, DWORD dwS, RTRGB rDivPix, RTColorCell pCDiv,
	      RTColorCell cc1, LPSTR lpName )
 {
   FILE *f = fopen( lpName , "w+t" );
   if( !f ) return;

   DWORD dwSummPoints = 0;
   DWORD dwC1, dwC2;  dwC1 = dwC2 = 0;
   for( DWORD i = 0; i < dwS; i++ )
    {
      char cBuf[1024];

      BOOL bFl;
      sprintf( cBuf, "%7lu:  %3u,%3u,%3u - %s %s", i,
        (int)pArr[i].r,
	(int)pArr[i].g,
	(int)pArr[i].b,
	( (bFl = PointInCell(cc1, pArr[i])) == TRUE ) ? "cc1":"cc2",
	((pArr + i) == (TRGB huge*)&rDivPix) ? "Div":" "
       );

      if( bFl == TRUE ) dwC1++; else dwC2++;

      fprintf( f, "%s\n", cBuf );
    }

   fprintf( f, "\n\nSumm of points: %lu + %lu = %lu (%lu)\n\n",
     dwC1, dwC2, dwS, dwC1 + dwC2 );

   fclose( f );

   BWCCMessageBox( GetMainWnd(), "Internal application error: Zero cell",
	      "Error",
	      MB_ICONSTOP | MB_OK | MB_APPLMODAL );
 } 

void DumpArr( TRGB huge* huge* pArr, DWORD dwS, RTRGB rDivPix, RTColorCell pCDiv,
	      RTColorCell cc1, RTColorCell cc2, LPSTR lpName, PTRGB pBuf, DWORD dwSize )
 {
   FILE *f = fopen( lpName , "w+t" );
   if( !f ) return;

   DWORD dwSummPoints = 0;
   DWORD dwC1, dwC2;  dwC1 = dwC2 = 0;
   for( DWORD i = 0; i < dwS; i++ )
    {
      char cBuf[1024];

      BOOL bFl;
      sprintf( cBuf, "%7lu:  %3u,%3u,%3u - %s %s", i,
        (int)pArr[i]->r,
	(int)pArr[i]->g,
	(int)pArr[i]->b,
	( (bFl = PointInCell(cc1, *(pArr[i]))) == TRUE ) ? "cc1":"cc2",
	(*(pArr + i) == (TRGB huge*)&rDivPix) ? "Div":" "
       );

      if( bFl == TRUE ) dwC1++; else dwC2++;

      fprintf( f, "%s\n", cBuf );
    }

   fprintf( f, "\n\nSumm of points: %lu + %lu = %lu (%lu)\n\n",
     dwC1, dwC2, dwS, dwC1 + dwC2 );
   fprintf( f, "\nDiv point: %3u,%3u,%3u\n",
	(int)rDivPix.r,
	(int)rDivPix.g,
	(int)rDivPix.b
        );


   fclose( f );

   for( DWORD k = 0; k < dwS; k++ )
    {
      BOOL bFl = FALSE;

      for( DWORD f = 0; f < dwSize; f++ )
	if( *(pArr[k]) == pBuf[f] ) { bFl = TRUE; break; }

      if( bFl == FALSE )
       {
	 BWCCMessageBox( GetMainWnd(), "Internal application error: Invalid point!!!!!",
	      "Error",
	      MB_ICONSTOP | MB_OK | MB_APPLMODAL );
	 return;
       }
    }

   BWCCMessageBox( GetMainWnd(), "Step is OK",
	      "Msg",
	      MB_OK | MB_APPLMODAL );

 } 
#endif

void Proect( TRGB huge& rgb, char cDir, int& x, int& y )
 {
   if( cDir == 'r' || cDir == 'b' ) { x = rgb.r; y = rgb.b; }
   else { x = rgb.g; y = rgb.b; }
 }

#ifdef _3DWDEBUG_
void ShowDiv( HWND hwnd, TRGB huge* pArr, RTRGB rDivPix, RTColorCell pCDiv,
	      RTColorCell cc1, RTColorCell cc2, char cDir, DWORD dwSize,
	      TRGB huge* pBuf, DWORD dwSBuf )
 {
   DWORD c1, c2; c1 = c2 = 0;
   for( DWORD j = 0; j < dwSBuf; j++ )
     if( PointInCell(cc1, pBuf[j]) == TRUE ) c1++;
     else if( PointInCell(cc2, pBuf[j]) == TRUE ) c2++;


   InvalidateRect( hwnd, NULL, TRUE );
   UpdateWindow( hwnd );
   RECT r; GetClientRect( hwnd, &r );

   HDC hdc = GetDC( hwnd );
   SetMapMode( hdc, MM_ISOTROPIC );
   SetWindowExt( hdc, 255, 255 );
   SetViewportExt( hdc, r.right, -r.bottom );
   SetViewportOrg( hdc, 0, r.bottom );

   LPSTR ax, ay;
   if( cDir == 'r' || cDir == 'b' ) { ax = "R"; ay = "B"; }
   else { ax = "G"; ay = "B"; }

   TextOut(hdc, 5, 255, ay, 1 );
   TextOut(hdc, 200, 15, ax, 1 );

   char cBuf[128];
   sprintf( cBuf, "%lu - %lu", c1, c2 );
   TextOut(hdc, 125, 125, cBuf, _fstrlen(cBuf) );

   for( DWORD i = 0; i < dwSize; i++ )
    {
      int x, y;
      Proect( pArr[i], cDir, x, y );
      SetPixel( hdc, x, y, RGB( 255, 0, 0 ) );
    }

   HBRUSH hbrOld = SelectBrush( hdc, (HBRUSH)GetStockObject(NULL_BRUSH) );
   HPEN hp = CreatePen( PS_SOLID, 0, RGB(255,0,0) ),
	hp1 = CreatePen( PS_SOLID, 0, RGB(0,255,0) ),
	hp2 = CreatePen( PS_SOLID, 0, RGB(0,0,255) ),
	hpOld;
   hpOld = SelectPen( hdc, hp );

   int x1, y1, x2, y2,
       x1_, y1_, x2_, y2_;
   TRGB p1(cc1.r1, cc1.g1, cc1.b1),
	p2(cc1.r2, cc1.g2, cc1.b2),
	p1_(cc2.r1, cc2.g1, cc2.b1),
	p2_(cc2.r2, cc2.g2, cc2.b2);
   int x, y;
   Proect( rDivPix, cDir, x, y );
   Proect( p1, cDir, x1, y1 ); Proect( p2, cDir, x2, y2 );
   Proect( p1_, cDir, x1_, y1_ ); Proect( p2_, cDir, x2_, y2_ );
   if( cDir == 'r' || cDir == 'g' )
    {
      MoveTo( hdc, x, 0 ); LineTo( hdc, x, 256 );
    }
   else
    {
      MoveTo( hdc, 0, y ); LineTo( hdc, 256, y );
    }

   SelectPen( hdc, hp1 );
   Rectangle( hdc, x1, y1, x2, y2 );

   SelectPen( hdc, hp2 );
   Rectangle( hdc, x1_, y1_, x2_, y2_ );

   SelectPen( hdc, hpOld );
   SelectBrush( hdc, hbrOld );
   DeleteObject( hp ); DeleteObject( hp1 ); DeleteObject( hp2 );
   ReleaseDC( hwnd, hdc );
 }
#endif

static BOOL near pascal CompressCycle( TRGB  huge* pBuf, DWORD dwSize,
  TAutoMemoryRc<PTColorCell>& mrcPalWork,
  int iCurSizePal )
 {
   ResetBounds( mrcPalWork, iCurSizePal );

   TRGB huge* pBuf_ = pBuf;
   for( DWORD i = dwSize; i > 0 ; i--, pBuf_++ )
    {
      PTColorCell pCell = InWhatCell( *pBuf_, mrcPalWork, iCurSizePal );
#ifdef _3DWDEBUG_
      if( !pCell )
       {
	 //Dump( mrcPalWork, iCurSizePal, pBuf, dwSize, "e:\\rgb.dmp" );

	 //mrcPalWork.FreeRc(); //mrcDiv.FreeRc(); mrcSortArray.FreeRc();

	 BWCCMessageBox( GetMainWnd(), "Internal application error: Medium section - invalid cell1",
	   "Error",
	   MB_ICONSTOP | MB_OK | MB_APPLMODAL );

	 return FALSE;
       }
#endif

      if( pCell->rMin > pBuf_->r ) pCell->rMin = pBuf_->r;
         //else
      if( pCell->rMax < pBuf_->r ) pCell->rMax = pBuf_->r;

      if( pCell->gMin > pBuf_->g ) pCell->gMin = pBuf_->g;
         //else
      if( pCell->gMax < pBuf_->g ) pCell->gMax = pBuf_->g;

      if( pCell->bMin > pBuf_->b ) pCell->bMin = pBuf_->b;
	 //else
      if( pCell->bMax < pBuf_->b ) pCell->bMax = pBuf_->b;
      pCell->dwNumbPoints++;
    }

#ifdef _3DWDEBUG_
      DWORD dwChk = 0;
      for( i = 0; i < iCurSizePal; i++ )
	dwChk += mrcPalWork()[ i ].dwNumbPoints;

      if( dwChk != dwSize )
       {	 
         mrcPalWork.FreeRc(); //mrcDiv.FreeRc(); mrcSortArray.FreeRc();

	 BWCCMessageBox( GetMainWnd(), "Internal application error: Medium section - lost points1",
	   "Error",
	   MB_ICONSTOP | MB_OK | MB_APPLMODAL );

	 return FALSE;
       }
#endif

  Compress( mrcPalWork, iCurSizePal );

  return TRUE;
 }

static BOOL near pascal CompressCycle( TAutoMemoryRc<PTColorCellHold>& mrcPalWork,
				       int iCurSizePal, DWORD dwSize )
 {
   ResetBounds( mrcPalWork, iCurSizePal );

   PTColorCellHold pCell = mrcPalWork();
   for( int i = iCurSizePal; i > 0; i--, pCell++ )
    {
      TRGB huge* huge* pRgb = pCell->mrcIn();
      for( DWORD dwPoint = pCell->dwContain; dwPoint > 0; dwPoint--, pRgb++ )
       {
	 if( pCell->rMin > (*pRgb)->r ) pCell->rMin = (*pRgb)->r;
	 if( pCell->rMax < (*pRgb)->r ) pCell->rMax = (*pRgb)->r;
	 if( pCell->gMin > (*pRgb)->g ) pCell->gMin = (*pRgb)->g;
	 if( pCell->gMax < (*pRgb)->g ) pCell->gMax = (*pRgb)->g;
	 if( pCell->bMin > (*pRgb)->b ) pCell->bMin = (*pRgb)->b;
	 if( pCell->bMax < (*pRgb)->b ) pCell->bMax = (*pRgb)->b;
	 pCell->dwNumbPoints++;
       }
    }

#ifdef _3DWDEBUG_
      DWORD dwChk = 0;
      for( i = 0; i < iCurSizePal; i++ )
	dwChk += mrcPalWork()[ i ].dwNumbPoints;

      if( dwChk != dwSize )
       {	 
         mrcPalWork.FreeRc(); //mrcDiv.FreeRc(); mrcSortArray.FreeRc();

	 BWCCMessageBox( GetMainWnd(), "Internal application error: Medium section - lost points1",
	   "Error",
	   MB_ICONSTOP | MB_OK | MB_APPLMODAL );

	 return FALSE;
       }
#endif

  Compress( mrcPalWork, iCurSizePal );

  return TRUE;
 }

#ifdef _3DWDEBUG_
void TestOfData( TAutoMemoryRc<PTColorCell>& mrcPalWork, int iCurSizePal,
		 TRGB  huge* pBuf, DWORD dwSize )
 {
   ResetBounds( mrcPalWork, iCurSizePal );

   for( DWORD i = 0; i < dwSize; i++ )
    {
      PTColorCell pCell = InWhatCell( pBuf[i], mrcPalWork, iCurSizePal );
      if( !pCell )
       {
	 //Dump( mrcPalWork, iCurSizePal, pBuf, dwSize, "e:\\rgb.dmp" );

	 //mrcPalWork.FreeRc(); //mrcDiv.FreeRc(); mrcSortArray.FreeRc();

	 BWCCMessageBox( GetMainWnd(), "Internal application error: Test - lost points",
	   "Error",
	   MB_ICONSTOP | MB_OK | MB_APPLMODAL );

	 //return FALSE;
	 continue;
       }

      pCell->dwNumbPoints++;
    }

   DWORD dwC = 0;
   for( int j = 0; j < iCurSizePal; j++ )
     dwC += mrcPalWork()[j].dwNumbPoints;

   if( dwC != dwSize )
     BWCCMessageBox( GetMainWnd(), "Internal application error: Test - lost points2",
	   "Error",
	   MB_ICONSTOP | MB_OK | MB_APPLMODAL );
 }
#endif

static PTColorCellHold near pascal GetMaxCell( TAutoMemoryRc<PTColorCellHold>& mrcPalWork,
					       int iCurSizePal
					     )
 {
   PTColorCellHold pC = mrcPalWork(),
		   pCMax = mrcPalWork();

   for( int i = iCurSizePal; i > 0; i--, pC++ )
     if( pCMax->bMaxAdge < pC->bMaxAdge ) pCMax = pC;

   return pCMax;
 }

static BOOL near pascal CalcPalGeometricF( TRGB  huge* pBuf, DWORD dwSize, int iPalSize,
  TAutoMemoryRc<PTColorCellHold>& mrcPalWork,
  int& iCurSizePal,
  PTIndicator pInd
                                         )
 {
   BOOL bFlEnableZero = FALSE;
   int iCountZero = 0;

   if( CompressCycle( mrcPalWork, iCurSizePal, dwSize ) == FALSE )
     return FALSE;

   while( iCurSizePal < iPalSize )
    {
      PTColorCellHold pCWork = GetMaxCell( mrcPalWork, iCurSizePal );

      char cDirection = TheLongest( *pCWork );

      PTDiv pDiv = (cDirection == 'r') ? DivR:
		   ( (cDirection == 'g') ? DivG:DivB );

      TRGB rgbDiv( pCWork->r1 + (pCWork->r2 - pCWork->r1) / 2,
		   pCWork->g1 + (pCWork->g2 - pCWork->g1) / 2,
		   pCWork->b1 + (pCWork->b2 - pCWork->b1) / 2
	         );
      TColorCellHold cc1, cc2;
      pDiv( rgbDiv, *pCWork, cc1, cc2 );
      //cc1.Compress();  cc2.Compress();

      DWORD dwC1, dwC2; dwC1 = dwC2 = 0;
      TRGB huge* huge* pRgb = pCWork->mrcIn();
      for( DWORD c = pCWork->dwContain; c > 0; c--, pRgb++ )
	if( PointInCell(cc1, **pRgb) == TRUE )  dwC1++;
	else dwC2++;


      if( !dwC1 || !dwC2 ) iCountZero++;
      else iCountZero = 0;
      if( iCountZero > iCurSizePal ) bFlEnableZero = TRUE;      

      BOOL bFlInit1 = dwC1 || bFlEnableZero == TRUE,
	   bFlInit2 = (dwC2 || bFlEnableZero == TRUE) && iCurSizePal < iPalSize;
      if( bFlInit1 ) cc1.Init( dwC1 );
      if( bFlInit2 ) cc2.Init( dwC2 );

      if( (bFlInit1 == TRUE && !cc1.mrcIn) || (bFlInit2 == TRUE && !cc2.mrcIn) )
       {
	 cc1.mrcIn.FreeRc(); cc2.mrcIn.FreeRc();
	 BWCCMessageBox( GetMainWnd(), "Not enought memory for create palette",
          "Error",
	  MB_ICONSTOP | MB_OK | MB_APPLMODAL );

         return FALSE;
       }

      cc1.ResetBounds(); cc2.ResetBounds();
      pRgb = pCWork->mrcIn();
      TRGB huge* huge* p1 = cc1.mrcIn();
      TRGB huge* huge* p2 = cc2.mrcIn();
      for( c = pCWork->dwContain; c > 0; c--, pRgb++ )
	if( PointInCell(cc1, **pRgb) == TRUE )  
	 {
	   if( !p1 ) continue;
	   *p1++ = *pRgb;

	   if( cc1.rMin > (*pRgb)->r ) cc1.rMin = (*pRgb)->r;
	   if( cc1.rMax < (*pRgb)->r ) cc1.rMax = (*pRgb)->r;
	   if( cc1.gMin > (*pRgb)->g ) cc1.gMin = (*pRgb)->g;
	   if( cc1.gMax < (*pRgb)->g ) cc1.gMax = (*pRgb)->g;
	   if( cc1.bMin > (*pRgb)->b ) cc1.bMin = (*pRgb)->b;
	   if( cc1.bMax < (*pRgb)->b ) cc1.bMax = (*pRgb)->b;
	   cc1.dwNumbPoints++;
	 }
	else
	 {
	   if( !p2 ) continue;
	   *p2++ = *pRgb;

	   if( cc2.rMin > (*pRgb)->r ) cc2.rMin = (*pRgb)->r;
	   if( cc2.rMax < (*pRgb)->r ) cc2.rMax = (*pRgb)->r;
	   if( cc2.gMin > (*pRgb)->g ) cc2.gMin = (*pRgb)->g;
	   if( cc2.gMax < (*pRgb)->g ) cc2.gMax = (*pRgb)->g;
	   if( cc2.bMin > (*pRgb)->b ) cc2.bMin = (*pRgb)->b;
	   if( cc2.bMax < (*pRgb)->b ) cc2.bMax = (*pRgb)->b;
	   cc2.dwNumbPoints++;
	 }

#ifdef _3DWDEBUG_
      if( cc1.dwNumbPoints != dwC1 || cc2.dwNumbPoints != dwC2 ||
	  dwC1 + dwC2 != pCWork->dwContain )
       {
	 BWCCMessageBox( GetMainWnd(), "Bad points CRC",
         "Error",
         MB_ICONSTOP | MB_OK | MB_APPLMODAL );

	 return FALSE;
       }       
#endif
      cc1.Compress();  cc2.Compress();

#ifdef _3DWDEBUG_
      if( cc1.bMaxAdge != max(cc1.r2- cc1.r1, max(cc1.g2- cc1.g1,cc1.b2- cc1.b1)) ||
	 cc2.bMaxAdge != max(cc2.r2- cc2.r1, max(cc2.g2- cc2.g1,cc2.b2- cc2.b1)) )
       {
	 BWCCMessageBox( GetMainWnd(), "Bad points CRC2",
         "Error",
         MB_ICONSTOP | MB_OK | MB_APPLMODAL );

	 return FALSE;
       }
#endif

      if( bFlInit1 && !bFlInit2 ) *pCWork = cc1;
      else if( !bFlInit1 && bFlInit2 ) *pCWork = cc2;
      else if( bFlInit1 && bFlInit2 )
       {
	 *pCWork = cc1;
         mrcPalWork()[ iCurSizePal++ ] = cc2;
       }

      if( (dwC1 && dwC2) || bFlEnableZero == TRUE )
       {
	 if( pInd && pInd->WorkMessageAdd(1) == TRUE ) return FALSE;
       }
      else if( pInd ) pInd->WorkMessage( iCurSizePal );
    }

   return TRUE;
 }


static BOOL near pascal CalcPalDiscreticF( TRGB  huge* pBuf, DWORD dwSize, int iPalSize,
  TAutoMemoryRc<PTColorCellHold>& mrcPalWork,
  int& iCurSizePal,
  PTIndicator pInd                                         )
 {
   BOOL bFlEnableZero = FALSE;
   int iCountZero = 0;

   if( CompressCycle( mrcPalWork, iCurSizePal, dwSize ) == FALSE )
     return FALSE;

   while( iCurSizePal < iPalSize )
    {
      PTColorCellHold pCWork = GetMaxCell( mrcPalWork, iCurSizePal );

      char cDirection = TheLongest( *pCWork );
                                               //**********
      PTDiv pDiv = (cDirection == 'r') ? DivR:
		   ( (cDirection == 'g') ? DivG:DivB );
      comparF* pCmp = (cDirection == 'r') ? CmpR2:
		      ( (cDirection == 'g') ? CmpG2:CmpB2 );      

      
      TColorCellHold cc1, cc2;

      if( pCWork->dwContain < 2 )
       {
	 TRGB rgbDiv( pCWork->r1 + (pCWork->r2 - pCWork->r1) / 2,
		      pCWork->g1 + (pCWork->g2 - pCWork->g1) / 2,
		      pCWork->b1 + (pCWork->b2 - pCWork->b1) / 2
	            );
	    
	 pDiv( rgbDiv, *pCWork, cc1, cc2 );
       }
      else
       {
	 SortQuick( (BYTE huge*)(pCWork->mrcIn()), pCWork->dwContain, sizeof(PTRGB), pCmp );

	 PTAcess pAcess = (cDirection == 'r') ? AR:
		          ( (cDirection == 'g') ? AG:AB );

	 TRGB rgbDiv = *(pCWork->mrcIn()[(pCWork->dwContain - 1) / 2]);
#ifdef _3DWDEBUG_
    BOOL bFlT = FALSE;
#endif
	 if( pAcess(*(pCWork->mrcIn()[pCWork->dwContain - 1]))  == pAcess(rgbDiv) )
	   for( DWORD k = (pCWork->dwContain - 1) / 2; k >= 0; k-- )
	     if( pAcess(*(pCWork->mrcIn()[k]))  != pAcess(rgbDiv) )
	      {
#ifdef _3DWDEBUG_
		MessageBeep( -1 );  bFlT = TRUE;
#endif
		rgbDiv = *(pCWork->mrcIn()[k]); break;  
              }

	 pDiv( rgbDiv, *pCWork, cc1, cc2 );

	 //if( bFlT )
	 //DumpArr( pCWork->mrcIn(), pCWork->dwContain, rgbDiv, *pCWork, cc1, cc2,
           //"e:\\d.dmp", pBuf, dwSize ); 
       }


      DWORD dwC1, dwC2; dwC1 = dwC2 = 0;
      TRGB huge* huge* pRgb = pCWork->mrcIn();
      for( DWORD c = pCWork->dwContain; c > 0; c--, pRgb++ )
	if( PointInCell(cc1, **pRgb) == TRUE )  dwC1++;
	else dwC2++;


      if( !dwC1 || !dwC2 ) iCountZero++;
      else iCountZero = 0;
      if( iCountZero > iCurSizePal ) bFlEnableZero = TRUE;      

      BOOL bFlInit1 = dwC1 || bFlEnableZero == TRUE,
	   bFlInit2 = (dwC2 || bFlEnableZero == TRUE) && iCurSizePal < iPalSize;
      if( bFlInit1 ) cc1.Init( dwC1 );
      if( bFlInit2 ) cc2.Init( dwC2 );

      if( (bFlInit1 == TRUE && !cc1.mrcIn) || (bFlInit2 == TRUE && !cc2.mrcIn) )
       {
	 cc1.mrcIn.FreeRc(); cc2.mrcIn.FreeRc();
	 BWCCMessageBox( GetMainWnd(), "Not enought memory for create palette",
          "Error",
	  MB_ICONSTOP | MB_OK | MB_APPLMODAL );

         return FALSE;
       }

      cc1.ResetBounds(); cc2.ResetBounds();
      pRgb = pCWork->mrcIn();
      TRGB huge* huge* p1 = cc1.mrcIn();
      TRGB huge* huge* p2 = cc2.mrcIn();
      for( c = pCWork->dwContain; c > 0; c--, pRgb++ )
	if( PointInCell(cc1, **pRgb) == TRUE )  
	 {
	   if( !p1 ) continue;
	   *p1++ = *pRgb;

	   if( cc1.rMin > (*pRgb)->r ) cc1.rMin = (*pRgb)->r;
	   if( cc1.rMax < (*pRgb)->r ) cc1.rMax = (*pRgb)->r;
	   if( cc1.gMin > (*pRgb)->g ) cc1.gMin = (*pRgb)->g;
	   if( cc1.gMax < (*pRgb)->g ) cc1.gMax = (*pRgb)->g;
	   if( cc1.bMin > (*pRgb)->b ) cc1.bMin = (*pRgb)->b;
	   if( cc1.bMax < (*pRgb)->b ) cc1.bMax = (*pRgb)->b;
	   cc1.dwNumbPoints++;
	 }
	else
	 {
	   if( !p2 ) continue;
	   *p2++ = *pRgb;

	   if( cc2.rMin > (*pRgb)->r ) cc2.rMin = (*pRgb)->r;
	   if( cc2.rMax < (*pRgb)->r ) cc2.rMax = (*pRgb)->r;
	   if( cc2.gMin > (*pRgb)->g ) cc2.gMin = (*pRgb)->g;
	   if( cc2.gMax < (*pRgb)->g ) cc2.gMax = (*pRgb)->g;
	   if( cc2.bMin > (*pRgb)->b ) cc2.bMin = (*pRgb)->b;
	   if( cc2.bMax < (*pRgb)->b ) cc2.bMax = (*pRgb)->b;
	   cc2.dwNumbPoints++;
	 }

#ifdef _3DWDEBUG_
      if( cc1.dwNumbPoints != dwC1 || cc2.dwNumbPoints != dwC2 ||
	  dwC1 + dwC2 != pCWork->dwContain )
       {
	 BWCCMessageBox( GetMainWnd(), "Bad points CRC",
         "Error",
         MB_ICONSTOP | MB_OK | MB_APPLMODAL );

	 return FALSE;
       }       
#endif
      cc1.Compress();  cc2.Compress();

#ifdef _3DWDEBUG_
      if( cc1.bMaxAdge != max(cc1.r2- cc1.r1, max(cc1.g2- cc1.g1,cc1.b2- cc1.b1)) ||
	 cc2.bMaxAdge != max(cc2.r2- cc2.r1, max(cc2.g2- cc2.g1,cc2.b2- cc2.b1)) )
       {
	 BWCCMessageBox( GetMainWnd(), "Bad points CRC2",
         "Error",
         MB_ICONSTOP | MB_OK | MB_APPLMODAL );

	 return FALSE;
       }
#endif

      if( bFlInit1 && !bFlInit2 ) *pCWork = cc1;
      else if( !bFlInit1 && bFlInit2 ) *pCWork = cc2;
      else if( bFlInit1 && bFlInit2 )
       {
	 *pCWork = cc1;
         mrcPalWork()[ iCurSizePal++ ] = cc2;
       }

      if( (dwC1 && dwC2) || bFlEnableZero == TRUE )
       {
	 if( pInd && pInd->WorkMessageAdd(1) == TRUE ) return FALSE;
       }
      else if( pInd ) pInd->WorkMessage( iCurSizePal );
    }

   return TRUE;
 }


static BOOL near pascal CalcPalGeometric( TRGB  huge* pBuf, DWORD dwSize, int iPalSize,
  TAutoMemoryRc<PTColorCell>& mrcPalWork,
  TAutoMemoryRc<PTColorCell>& mrcDiv,
  int& iCurSizePal,
  //TAutoMemoryHugeRc<TRGB huge*>& mrcSortArray,
  PTIndicator pInd
                                        )
 {
   BOOL bFlRemoveZero = TRUE;

   if( CompressCycle( pBuf, dwSize, mrcPalWork, iCurSizePal ) == FALSE )
     return FALSE;

   while( iCurSizePal < iPalSize )
    {
      PTColorCell pC = mrcPalWork();
      PTColorCell pDivC = mrcDiv();
      int  iIndexPut = 0;
      int iSizeCount = iCurSizePal;
      for( int iIndexGet = 0; iIndexGet < iCurSizePal &&
	   iSizeCount < iPalSize && iIndexPut < iPalSize - 1;
	   iIndexGet++, pC++ )
       {     
	 //RTColorCell rC = mrcPalWork()[ iIndexGet ];
	 char cDirection = TheLongest( *pC );

	 comparF*    pCmp = (cDirection == 'r') ? CmpR:
			    ( (cDirection == 'g') ? CmpG:CmpB );
	 PTDiv pDiv = (cDirection == 'r') ? DivR:
		      ( (cDirection == 'g') ? DivG:DivB );



	 TRGB rgbDiv( pC->r1 + (pC->r2 - pC->r1) / 2,
		      pC->g1 + (pC->g2 - pC->g1) / 2,
		      pC->b1 + (pC->b2 - pC->b1) / 2
		    );
	 TColorCell cc1, cc2;
	 pDiv( rgbDiv, *pC, cc1, cc2 );

	 *pDivC++ = cc1;
	 *pDivC++ = cc2;
	 iIndexPut += 2;
	    /*mrcDiv()[ iIndexPut++ ] = cc1;
	    mrcDiv()[ iIndexPut++ ] = cc2;*/
	 iSizeCount++;
       }

      pDivC = &(mrcDiv()[ iIndexPut ]);
      PTColorCell pWork = &(mrcPalWork()[ iIndexGet ]);
      for( int k = iIndexGet; k < iCurSizePal; k++, iIndexPut++, pDivC++, pWork++ )
       {
	 if( iIndexPut >= iPalSize )
	  {    
	    BWCCMessageBox( GetMainWnd(), "Internal application error: Medium section - overflow div array",
	      "Error",
	      MB_ICONSTOP | MB_OK | MB_APPLMODAL );

            return FALSE;
	  }

	 //mrcDiv()[ iIndexPut ] = mrcPalWork()[ k ]; 
	 *pDivC = *pWork;
       }

      int iKeySizePal = iCurSizePal;
      iCurSizePal = iSizeCount;
      _fmemcpy( mrcPalWork(), mrcDiv(), iCurSizePal * sizeof(TColorCell) );

      if( CompressCycle( pBuf, dwSize, mrcPalWork, iCurSizePal ) == FALSE )
	return FALSE;

      if( bFlRemoveZero == TRUE )
       {
	 PTColorCell pCGet = mrcPalWork();
	 PTColorCell pCPut = mrcDiv();
	 for( int i = 0, iIndPut = 0; i < iCurSizePal; i++, pCGet++ )
	   if( pCGet->dwNumbPoints )
	    {
	      *pCPut++ = *pCGet; iIndPut++;
	    }

	 _fmemcpy( mrcPalWork(), mrcDiv(), iIndPut * sizeof(TColorCell) );

	 if( iKeySizePal >= iIndPut )
	  {
	    bFlRemoveZero = FALSE;
	    /*BWCCMessageBox( GetMainWnd(), "Enables zero cells in palette",
	      "Warning",
	      MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL );*/
          }

	 iCurSizePal = iIndPut;
       }
      if( pInd && pInd->WorkMessage(iCurSizePal) == TRUE ) return FALSE;
#ifdef _3DWDEBUG_
      //TestOfData( mrcPalWork, iCurSizePal, pBuf, dwSize );
#endif

    }

  return TRUE;
 }


static int CmpColorCellAdges( const void _FAR* p1, const void _FAR* p2 )
 {
   /*BYTE dR = PTColorCell(p1)->r2 - PTColorCell(p1)->r1,
        dG = PTColorCell(p1)->g2 - PTColorCell(p1)->g1,
	dB = PTColorCell(p1)->b2 - PTColorCell(p1)->b1;
   BYTE dMax1 = max( dR, max(dG, dB) );

   dR = PTColorCell(p2)->r2 - PTColorCell(p2)->r1,
   dG = PTColorCell(p2)->g2 - PTColorCell(p2)->g1,
   dB = PTColorCell(p2)->b2 - PTColorCell(p2)->b1;
   BYTE dMax2 = max( dR, max(dG, dB) );*/
	       
   return int(PTColorCell(p2)->bMaxAdge) - int(PTColorCell(p1)->bMaxAdge);
 }

static BOOL near pascal CalcPalDiscretic2( TRGB  huge* pBuf, DWORD dwSize, int iPalSize,
  TAutoMemoryRc<PTColorCell>& mrcPalWork,
  //TAutoMemoryRc<PTColorCell>& mrcDiv,
  int& iCurSizePal,
  TAutoMemoryHugeRc<TRGB huge*>& mrcSortArray,
  PTIndicator pInd
                                        )
 {
   BOOL bFlEnableZero = FALSE;
   int iCountZero = 0;
   if( CompressCycle( pBuf, dwSize, mrcPalWork, iCurSizePal ) == FALSE )
     return FALSE;

   while( iCurSizePal < iPalSize )
    {
      TColorCell ccWork = mrcPalWork()[ 0 ];
      if( iCurSizePal > 1 )
	_fmemmove( mrcPalWork(), mrcPalWork() + 1, (iCurSizePal - 1) * sizeof(TColorCell) );
      --iCurSizePal;

      char cDirection = TheLongest( ccWork );

      TRGB  huge* pBuf_ = pBuf;
      TRGB  huge* pArr = mrcSortArray();
      for( DWORD i = 0, iIndArr = 0; i < dwSize; i++, pBuf_++ )
	if( PointInCell(ccWork, *pBuf_) == TRUE )
         {
	   *pArr++ = *pBuf_; iIndArr++;
	 }

      TColorCell cc1, cc2;
      PTDiv pDiv = (cDirection == 'r') ? DivR:
		   ( (cDirection == 'g') ? DivG:DivB );
      comparF* pCmp = (cDirection == 'r') ? CmpR:
		      ( (cDirection == 'g') ? CmpG:CmpB );

      if( iIndArr < 2 )
       {
	 TRGB rgbDiv( ccWork.r1 + (ccWork.r2 - ccWork.r1) / 2,
		      ccWork.g1 + (ccWork.g2 - ccWork.g1) / 2,
		      ccWork.b1 + (ccWork.b2 - ccWork.b1) / 2
	            );
	    
	 pDiv( rgbDiv, ccWork, cc1, cc2 );
       }
      else
       {
	 SortQuick( (BYTE huge*)(mrcSortArray()), iIndArr, sizeof(TRGB), pCmp );

	 PTAcess pAcess = (cDirection == 'r') ? AR:
		          ( (cDirection == 'g') ? AG:AB );

	 TRGB rgbDiv = mrcSortArray()[(iIndArr - 1) / 2];
	 if( pAcess(mrcSortArray()[iIndArr - 1])  == pAcess(rgbDiv) )
	   for( DWORD k = (iIndArr - 1) / 2; k > 0; k-- )
	     if( pAcess(mrcSortArray()[k])  != pAcess(rgbDiv) )
	      {
		rgbDiv = mrcSortArray()[k]; break;
              }

	 pDiv( rgbDiv, ccWork, cc1, cc2 );
       }

      cc1.ResetBounds(); cc2.ResetBounds();
      pArr = mrcSortArray();
      for( DWORD h = 0; h < iIndArr; h++, pArr++ )
       {
         if( PointInCell(cc1, *pArr) == TRUE )
	  {
	    if( cc1.rMin > pArr->r ) cc1.rMin = pArr->r;
	    if( cc1.rMax < pArr->r ) cc1.rMax = pArr->r;
	    if( cc1.gMin > pArr->g ) cc1.gMin = pArr->g;
	    if( cc1.gMax < pArr->g ) cc1.gMax = pArr->g;
	    if( cc1.bMin > pArr->b ) cc1.bMin = pArr->b;
	    if( cc1.bMax < pArr->b ) cc1.bMax = pArr->b;
	    cc1.dwNumbPoints++;
	  }
	 else
	  {
	    if( cc2.rMin > pArr->r ) cc2.rMin = pArr->r;
	    if( cc2.rMax < pArr->r ) cc2.rMax = pArr->r;
	    if( cc2.gMin > pArr->g ) cc2.gMin = pArr->g;
	    if( cc2.gMax < pArr->g ) cc2.gMax = pArr->g;
	    if( cc2.bMin > pArr->b ) cc2.bMin = pArr->b;
	    if( cc2.bMax < pArr->b ) cc2.bMax = pArr->b;
	    cc2.dwNumbPoints++;
          }
       }

      /*if( !cc1.dwNumbPoints || !cc2.dwNumbPoints )
       {
	 //Dump( mrcPalWork, iCurSizePal, pBuf, dwSize, "e:\\rgb.dmp" );
	 DumpArr( pArr, iIndArr, mrcSortArray()[(iIndArr - 1) / 2],
		  ccWork, cc1.dwNumbPoints ? cc2:cc1, "e:\\rgb.dmp" );

	 return FALSE;
       }*/


      if( !cc1.dwNumbPoints || !cc2.dwNumbPoints ) iCountZero++;
      else iCountZero = 0;
      if( iCountZero > iCurSizePal ) bFlEnableZero = TRUE;

      cc1.Compress();  cc2.Compress();

      if( cc1.dwNumbPoints || bFlEnableZero == TRUE ) mrcPalWork()[ iCurSizePal++ ] = cc1;
      if( iCurSizePal >= iPalSize ) break;

      if( cc2.dwNumbPoints || bFlEnableZero == TRUE ) mrcPalWork()[ iCurSizePal++ ] = cc2;
      if( (cc1.dwNumbPoints && cc2.dwNumbPoints) || bFlEnableZero == TRUE )
       {
	 if( pInd && pInd->WorkMessageAdd(1) == TRUE ) return FALSE;
       }
      else if( pInd ) pInd->WorkMessage( iCurSizePal );

      //SortQuick( (BYTE huge*)(mrcSortArray()), iIndArr, sizeof(TRGB), pCmp );
      qsort( mrcPalWork(), iCurSizePal, sizeof(TColorCell), CmpColorCellAdges );
    }//while


   return TRUE;
 }      


static BOOL near pascal CalcPalDiscretic( TRGB  huge* pBuf, DWORD dwSize, int iPalSize,
  TAutoMemoryRc<PTColorCell>& mrcPalWork,
  TAutoMemoryRc<PTColorCell>& mrcDiv,
  int& iCurSizePal,
  TAutoMemoryHugeRc<TRGB huge*>& mrcSortArray,
  PTIndicator pInd
                                        )
 { 
   if( CompressCycle( pBuf, dwSize, mrcPalWork, iCurSizePal ) == FALSE )
     return FALSE;

   while( iCurSizePal < iPalSize )
    {      
      PTColorCell pC = mrcPalWork();
      PTColorCell pDivC = mrcDiv();
      int  iIndexPut = 0;
      int iSizeCount = iCurSizePal;
      for( int iIndexGet = 0; iIndexGet < iCurSizePal &&
	   iSizeCount < iPalSize && iIndexPut < iPalSize - 1;
	   iIndexGet++, pC++ )
       {     
	 //RTColorCell rC = mrcPalWork()[ iIndexGet ];
	 char cDirection = TheLongest( *pC );
	 char cDirection2, cDirection3;
	 if( cDirection == 'r' ) { cDirection2 = 'g'; cDirection3 = 'b'; }
	 else if( cDirection == 'g' )
	  { cDirection2 = 'r'; cDirection3 = 'b'; }
	 else if( cDirection == 'b' )
	  { cDirection2 = 'r'; cDirection3 = 'g'; }
	 char cDir[3] = { cDirection, cDirection2, cDirection3 };

	 TRGB  huge* pBuf_ = pBuf;
	 TRGB  huge* pArr = mrcSortArray();       
	 for( DWORD i = 0, iIndArr = 0; i < dwSize; i++, pBuf_++ )
	   if( PointInCell(*pC, *pBuf_) == TRUE )
            {
	      *pArr++ = *pBuf_; iIndArr++;
            }

	 if( iIndArr < 2 )
	  {
	    TRGB rgbDiv( pC->r1 + (pC->r2 - pC->r1) / 2,
		         pC->g1 + (pC->g2 - pC->g1) / 2,
		         pC->b1 + (pC->b2 - pC->b1) / 2
		       );
	    TColorCell cc1, cc2;
	    PTDiv pDiv = (cDirection == 'r') ? DivR:
			 ( (cDirection == 'g') ? DivG:DivB );
	    pDiv( rgbDiv, *pC, cc1, cc2 );

	    *pDivC++ = cc1;
	    *pDivC++ = cc2;
	    iIndexPut += 2;	    
	    iSizeCount++;
	  }
	 else
          {	       
	    for( int j = 0; j < 3; j++ )
	     {
	       comparF* pCmp = (cDir[j] == 'r') ? CmpR:
			       ( (cDir[j] == 'g') ? CmpG:CmpB );
	       PTDiv pDiv = (cDir[j] == 'r') ? DivR:
			    ( (cDir[j] == 'g') ? DivG:DivB );

	       SortQuick( (BYTE huge*)mrcSortArray(), iIndArr, sizeof(TRGB), pCmp );

	       TColorCell cc1, cc2;
	       //if( pDiv( mrcSortArray()[(iIndArr - 1) / 2], *pC, cc1, cc2 ) == FALSE &&
		 //  j < 2 ) continue;
	       pDiv( mrcSortArray()[(iIndArr - 1) / 2], *pC, cc1, cc2 );
	
		


	       cc1.ResetBounds(); cc2.ResetBounds();
	       TRGB  huge* pArr = mrcSortArray();
	       //TRGB  huge* pArr = pBuf;
	       for( DWORD h = 0; h < iIndArr; h++, pArr++ )
		{
		  if( PointInCell(cc1, *pArr) == TRUE )
		   {
		     if( cc1.rMin > pArr->r ) cc1.rMin = pArr->r;
		     if( cc1.rMax < pArr->r ) cc1.rMax = pArr->r;
		     if( cc1.gMin > pArr->g ) cc1.gMin = pArr->g;
		     if( cc1.gMax < pArr->g ) cc1.gMax = pArr->g;
		     if( cc1.bMin > pArr->b ) cc1.bMin = pArr->b;
		     if( cc1.bMax < pArr->b ) cc1.bMax = pArr->b;
		     cc1.dwNumbPoints++;
		   }
		  else
		   {
		     if( cc2.rMin > pArr->r ) cc2.rMin = pArr->r;
		     if( cc2.rMax < pArr->r ) cc2.rMax = pArr->r;
		     if( cc2.gMin > pArr->g ) cc2.gMin = pArr->g;
		     if( cc2.gMax < pArr->g ) cc2.gMax = pArr->g;
		     if( cc2.bMin > pArr->b ) cc2.bMin = pArr->b;
		     if( cc2.bMax < pArr->b ) cc2.bMax = pArr->b;
		     cc2.dwNumbPoints++;
                   }
		}

	       //if( (!cc1.dwNumbPoints || !cc2.dwNumbPoints) && j < 2 ) continue;

	       cc1.Compress();  cc2.Compress();
	       *pDivC++ = cc1;
	       *pDivC++ = cc2;
	       iIndexPut += 2;
	       iSizeCount++;

	       //ShowDiv( hwnd, mrcSortArray(), mrcSortArray()[(iIndArr - 1) / 2],
		 //*pC, cc1, cc2, cDir[j], iIndArr, pBuf, dwSize );

               break;
	     }
	  }
	 if( pInd && pInd->WorkMessageAdd(1) == TRUE ) return FALSE;
       }

      pDivC = &(mrcDiv()[ iIndexPut ]);
      PTColorCell pWork = &(mrcPalWork()[ iIndexGet ]);
      for( int k = iIndexGet; k < iCurSizePal; k++, iIndexPut++, pDivC++, pWork++ )
       {
	 if( iIndexPut >= iPalSize )
	  {    
	    BWCCMessageBox( GetMainWnd(), "Internal application error: Medium section - overflow div array",
	      "Error",
	      MB_ICONSTOP | MB_OK | MB_APPLMODAL );

            return FALSE;
	  }

	 //mrcDiv()[ iIndexPut ] = mrcPalWork()[ k ]; 
	 *pDivC = *pWork;
       }

      iCurSizePal = iSizeCount;
      _fmemcpy( mrcPalWork(), mrcDiv(), iCurSizePal * sizeof(TColorCell) );

#ifdef _3DWDEBUG_
      //TestOfData( mrcPalWork, iCurSizePal, pBuf, dwSize );
#endif
    }

  return TRUE;
 }
      


                    //always 256  0, 1 - black and white - reserved                                     //16, 236, 256 or other
BOOL MediumSection( PTColorTable pTbl, TRGB  huge* pBuf, DWORD dwSize, int iPalSize,
		    TConvertAlho caAlho, int iWidth, int iHigh,
		    PTNoisyDta pNoise,
		    PTIndicator pInd )
 {
   PTColorTable pTblKey = pTbl;
   int iPalSizeKey = iPalSize;

   pTbl += 2; iPalSize -= 2;

   BOOL bFlCutGeom = (caAlho & CA_MedianCutGeom) ? TRUE:FALSE;
				//iPalSize = 16;
   if( pInd )
    {
      char cBuf[50];
      sprintf( cBuf, "Calc median cut %s palette...",
	bFlCutGeom ? "Geometric":"Dicretic" );
      pInd->UpdateStr( 2, cBuf );
      pInd->Init( iPalSize );
      //pInd->Show();
      for( int i = 0; i < 50; i++ )
	if( pInd->WorkMessage( 1 ) == TRUE ) return FALSE;
      //pInd->Update();
    }

   TAutoMemoryRc<PTColorCell> mrcPalWork = new TColorCell[ iPalSize ],
			      mrcDiv = new TColorCell[ iPalSize ];
   int iCurSizePal = 1;
   TAutoMemoryHugeRc<TRGB huge*> mrcSortArray;
   if( !bFlCutGeom ) mrcSortArray = (TRGB huge*)farcalloc( dwSize, sizeof(TRGB) );

   if( !mrcPalWork || !mrcDiv || !mrcSortArray )
    {    
      mrcPalWork.FreeRc(); mrcDiv.FreeRc(); mrcSortArray.FreeRc();

      BWCCMessageBox( GetMainWnd(), "Not enought memory for create palette",
        "Error",
	MB_ICONSTOP | MB_OK | MB_APPLMODAL );

      return FALSE;
    }

   mrcPalWork()[ 0 ] = TColorCell( 0, 0, 0, 255, 255, 255 );

   /*PTWindow pWnd = new TWindow( NULL, "Div" );
   pModule->MakeWindow( pWnd );
   pWnd->Show( SW_SHOW );
   HWND hwndDiv = pWnd->HWindow;
   SetWindowPos( hwndDiv, NULL, 0, 0, 256, 256, SWP_NOZORDER | SWP_NOMOVE );*/

   BOOL bRes;
   if( caAlho & CA_MedianCutGeom )
     bRes = CalcPalGeometric( pBuf, dwSize, iPalSize, mrcPalWork, mrcDiv, iCurSizePal,
			      pInd );
   else
     bRes = CalcPalDiscretic2( pBuf, dwSize, iPalSize, mrcPalWork, iCurSizePal,
			      mrcSortArray, pInd );
   if( bRes == FALSE ) return FALSE;


   if( pInd && pInd->WorkMessage( iCurSizePal ) == TRUE ) return FALSE;

#ifdef _3DWDEBUG_
   //TestOfData( mrcPalWork, iCurSizePal, pBuf, dwSize );
#endif

   if( pInd )
    {
      pInd->UpdateStr( 2, "Indexsing true color image..." );
      pInd->Redraw();
    }
   //pInd->Init( iPalSize );

   HCURSOR hcOld = SetCursor( LoadCursor(NULL, IDC_WAIT) );

   //CompressCycle( pBuf, dwSize, mrcPalWork, iCurSizePal );

#ifdef _3DWDEBUG_
   DumpDispers( mrcPalWork, iCurSizePal, dwSize, "e:\\rgb.dmp" );

   DWORD dwChk = 0;
   for( int i = 0; i < iCurSizePal; i++ )
     dwChk += mrcPalWork()[ i ].dwNumbPoints;

   if( dwChk != dwSize )
    {	 
      mrcPalWork.FreeRc(); mrcDiv.FreeRc(); mrcSortArray.FreeRc();

      BWCCMessageBox( GetMainWnd(), "Internal application error: Medium section - lost points2",
        "Error",
	MB_ICONSTOP | MB_OK | MB_APPLMODAL );

      return FALSE;
    }
#endif


   PTColorCell pWork = mrcPalWork();
   PTColorTable pTbl_ = pTbl;
   for( int c = 0; c < iCurSizePal; c++, pWork++, pTbl_++ )
    {
      pWork->index = c + 2; // 0, 1 - reserved

      pTbl_->rgbRed = pWork->rMedium = pWork->rMin +
        (pWork->rMax - pWork->rMin) / 2;
      pTbl_->rgbGreen = pWork->gMedium = pWork->gMin +
        (pWork->gMax - pWork->gMin) / 2;
      pTbl_->rgbBlue = pWork->bMedium = pWork->bMin +
	(pWork->bMax - pWork->bMin) / 2;
      
      /*pTbl_->rgbRed = pWork->rMedium;
      pTbl_->rgbGreen = pWork->gMedium;
      pTbl_->rgbBlue = pWork->bMedium;*/
    }

   SetCursor( hcOld );
   if( pInd ) pInd->Init( dwSize );

   if( caAlho & CA_Diffuzion )
     return Diffusion( pTblKey, pBuf, dwSize, iPalSizeKey, caAlho, iWidth, iHigh, pNoise, pInd );


   TRGB huge* pBuf_ = pBuf;
   for( DWORD j = 0; j < dwSize; j++, pBuf_++ )
    {
      PTColorCell pCell = InWhatCell( *pBuf_, mrcPalWork, iCurSizePal );
      if( pCell ) pBuf_->index = pCell->index;
      else	    
       {
         mrcPalWork.FreeRc(); mrcDiv.FreeRc(); mrcSortArray.FreeRc();

	 BWCCMessageBox( GetMainWnd(), "Internal application error: Medium section - invalid cell3",
	   "Error",
	   MB_ICONSTOP | MB_OK | MB_APPLMODAL );

         return FALSE;
       }

      if( pInd && pInd->WorkMessage(j+1) == TRUE ) return FALSE;
    }   


   return TRUE;
 }

BOOL MediumSectionF( PTColorTable pTbl, TRGB  huge* pBuf, DWORD dwSize, int iPalSize,
		    TConvertAlho caAlho, int iWidth, int iHigh,
		    PTNoisyDta pNoise,
		    PTIndicator pInd )
 {
   PTColorTable pTblKey = pTbl;
   int iPalSizeKey = iPalSize;

   pTbl += 2; iPalSize -= 2;

   BOOL bFlCutGeom = (caAlho & CA_MedianCutGeom) ? TRUE:FALSE;
				//iPalSize = 16;
   if( pInd )
    {
      char cBuf[50];
      sprintf( cBuf, "Calc median cut %s palette...",
	bFlCutGeom ? "Geometric":"Dicretic" );
      pInd->UpdateStr( 2, cBuf );
      pInd->Init( iPalSize );
      pInd->Redraw();
      //pInd->Show();
      //for( int i = 0; i < 50; i++ )
	//if( pInd->WorkMessage( 1 ) == TRUE ) return FALSE;
    }

   TAutoMemoryRc<PTColorCellHold> mrcPalWork = new TColorCellHold[ iPalSize ];

   int iCurSizePal = 1;


   if( !mrcPalWork )
    {    
      mrcPalWork.FreeRc(); 

      BWCCMessageBox( GetMainWnd(), "Not enought memory for create palette",
        "Error",
	MB_ICONSTOP | MB_OK | MB_APPLMODAL );

      return FALSE;
    }

   mrcPalWork()[ 0 ] = TColorCellHold( 0, 0, 0, 255, 255, 255, dwSize );
   TRGB huge* pBuf_ = pBuf;
   TRGB huge* huge* pRgb = mrcPalWork()[ 0 ].mrcIn();
   if( !pRgb )
    {    
      mrcPalWork.FreeRc(); 

      BWCCMessageBox( GetMainWnd(), "Not enought memory for create palette",
        "Error",
	MB_ICONSTOP | MB_OK | MB_APPLMODAL );

      return FALSE;
    }

   TAutoCursorRc crcCurs( LoadCursor(NULL, IDC_WAIT) );
   for( DWORD k = dwSize; k > 0; k--, pBuf_++, pRgb++ )
     *pRgb = pBuf_;
   crcCurs.FreeRc();


   //if( NoisyMarginal( pBuf, dwSize, pInd ) == FALSE ) return FALSE;


   BOOL bRes;
   if( caAlho & CA_MedianCutGeom )
     bRes = CalcPalGeometricF( pBuf, dwSize, iPalSize, mrcPalWork, iCurSizePal, pInd );
   else
     bRes = CalcPalDiscreticF( pBuf, dwSize, iPalSize, mrcPalWork, iCurSizePal, pInd );
   if( bRes == FALSE ) return FALSE;


   if( pInd && pInd->WorkMessage( iCurSizePal+2 ) == TRUE ) return FALSE;


   if( pInd )
    {
      pInd->UpdateStr( 2, "Indexsing true color image..." );
      pInd->Redraw();
    }
   crcCurs = LoadCursor(NULL, IDC_WAIT);
   


#ifdef _3DWDEBUG_
   DumpDispers( mrcPalWork, iCurSizePal, dwSize, "e:\\rgb.dmp" );

   DWORD dwChk = 0;
   for( int i = 0; i < iCurSizePal; i++ )
     dwChk += mrcPalWork()[ i ].dwNumbPoints;

   if( dwChk != dwSize )
    {	 
      mrcPalWork.FreeRc(); 

      BWCCMessageBox( GetMainWnd(), "Internal application error: Medium section - lost points2",
        "Error",
	MB_ICONSTOP | MB_OK | MB_APPLMODAL );

      return FALSE;
    }
#endif


   PTColorCellHold pWork = mrcPalWork();
   PTColorTable pTbl_ = pTbl;
   for( int c = 0; c < iCurSizePal; c++, pWork++, pTbl_++ )
    {
      pWork->index = c + 2; // 0, 1 - reserved

      pTbl_->rgbRed = pWork->rMedium = pWork->rMin +
	(pWork->rMax - pWork->rMin) / 2;
       pTbl_->rgbGreen = pWork->gMedium = pWork->gMin +
        (pWork->gMax - pWork->gMin) / 2;
      pTbl_->rgbBlue = pWork->bMedium = pWork->bMin +
	(pWork->bMax - pWork->bMin) / 2;

      /*TRGB rgbCent( pWork->rMin + (pWork->rMax - pWork->rMin) / 2,
		    pWork->gMin + (pWork->gMax - pWork->gMin) / 2,
		    pWork->bMin + (pWork->bMax - pWork->bMin) / 2
		  );
      TRGB huge* huge* pPixBuf = pWork->mrcIn();
      TRGB huge* pPix = *pPixBuf;
      int iDist = INT_MAX;
      for( DWORD k = pWork->dwContain; k > 0; k--, pPixBuf++ )
       {
	 int iTmpDst = abs_( (int)(*pPixBuf)->r - (int)rgbCent.r ) +
		       abs_( (int)(*pPixBuf)->g - (int)rgbCent.g ) +
		       abs_( (int)(*pPixBuf)->b - (int)rgbCent.b );

	 if( iTmpDst < iDist )
	  {
	    iDist = iTmpDst;
	    pPix = *pPixBuf;
          }
       }

      pTbl_->rgbRed = pWork->rMedium = pPix->r;
      pTbl_->rgbGreen = pWork->gMedium = pPix->g;
      pTbl_->rgbBlue = pWork->bMedium = pPix->b;*/
    }

   crcCurs.FreeRc();
   if( pInd ) pInd->Init( iCurSizePal );

   if( caAlho & CA_Diffuzion )
     return Diffusion( pTblKey, pBuf, dwSize, iPalSizeKey, caAlho, iWidth, iHigh, pNoise, pInd );


   PFindNearstColor pFNFunc = FindNearstColorNorm;
   TAutoMemoryRc<int*> mrcIndex;
   
   if( pNoise->nNoisy == N_Marginal )
    {
      if( NoisyMarginal( pBuf, dwSize, pNoise, pInd ) == FALSE ) return FALSE;
    }
   else if( pNoise->nNoisy == N_Uniform )
    {
      randomize();
      pFNFunc = FindNearstColorNoisy;
      mrcIndex = new int[ pNoise->iUniformAmplitude ];
      if( !mrcIndex )  return FALSE;
    }

  if( pNoise->nNoisy != N_None )
   {
     if( pInd ) pInd->Init( dwSize );
     pInd->UpdateStr( 2, "Indexing true colour image..." );

     TRGB  huge* pBuf_ = pBuf;
     for( DWORD dwI = dwSize; dwI > 0; dwI--, pBuf_++ )
      {
	TIRGB irgb; irgb = *pBuf_;
	pBuf_->index = pFNFunc( irgb, pTbl, iCurSizePal, pNoise, mrcIndex() );

	if( pInd && pInd->WorkMessageAdd(1) == TRUE ) return FALSE;
      }

     if( pInd ) pInd->WorkIs100();
     return TRUE;
   }


   pWork = mrcPalWork();
   for( c = 0; c < iCurSizePal; c++, pWork++ )
    {
      TRGB huge* huge* pRgb = pWork->mrcIn();
      for( DWORD dwPoint = pWork->dwContain; dwPoint > 0; dwPoint--, pRgb++ )
	(*pRgb)->index = pWork->index;

      if( pInd && pInd->WorkMessage(c+1) == TRUE ) return FALSE;
    }

   if( pInd && pInd->WorkMessage(iCurSizePal+2) == TRUE ) return FALSE;

   if( pInd ) pInd->WorkIs100();

   return TRUE;
 }


inline void GammaCorrect( PTIRGB pPix, int errR, int errG, int errB,
			  int iMul, int iDiv )
 {
   pPix->r += (errR * iMul) / iDiv;
   pPix->g += (errG * iMul) / iDiv;
   pPix->b += (errB * iMul) / iDiv;
 }


inline void GammaCorrectX_16( PTIRGB pPix, int errR, int errG, int errB,
			      int iMul )
 {
   pPix->r += ((errR * iMul) / 16);
   pPix->g += ((errG * iMul) / 16);
   pPix->b += ((errB * iMul) / 16);
 }

inline void GammaCorrect1_16( PTIRGB pPix, int errR, int errG, int errB )			 
 { 
   pPix->r += (errR / 16);
   pPix->g += (errG / 16);
   pPix->b += (errB / 16);
 }


static BYTE near pascal FindNearstColorNoisy( RTIRGB rPic, PTColorTable pTbl,
  int iSize, PTNoisyDta pNoise, int* pIndex )
 {
   _fmemset( pIndex, -1, pNoise->iUniformAmplitude * sizeof(int) );
   pIndex[0] = INT_MAX;

   PTColorTable pTblKey = pTbl;
   for( int iMinDist = INT_MAX, i = 0; i < iSize; i++, pTbl++ )
    {
      int iDist = abs_( rPic.r - int(pTbl->rgbRed) ) +
		  abs_( rPic.g - int(pTbl->rgbGreen) ) +
		  abs_( rPic.b - int(pTbl->rgbBlue) );
 
      if( iDist < iMinDist )
       {
	 iMinDist = iDist;
	 for( int k = pNoise->iUniformAmplitude - 1; k > 0; k-- ) pIndex[k] = pIndex[k - 1];
	 pIndex[0] = i;
       }
    }

   int iInd = random( pNoise->iUniformAmplitude );
   if( pIndex[iInd] != -1 )
    {
      int iDist = abs_( rPic.r - int(pTblKey[pIndex[iInd]].rgbRed) ) +
		  abs_( rPic.g - int(pTblKey[pIndex[iInd]].rgbGreen) ) +
		  abs_( rPic.b - int(pTblKey[pIndex[iInd]].rgbBlue) );

      if( iDist > pNoise->iUniformThreshold ) return pIndex[0];
      else return pIndex[iInd];
    }
   else return pIndex[0];
 }


static BYTE near pascal FindNearstColorNorm( RTIRGB rPic,
  PTColorTable pTbl, int iSize, PTNoisyDta, int* )
 {
   BYTE bIndex = 0;

   for( int iMinDist = INT_MAX, i = 0; i < iSize; i++, pTbl++ )
    {
      int iDist = abs_( rPic.r - int(pTbl->rgbRed) ) +
		  abs_( rPic.g - int(pTbl->rgbGreen) ) +
		  abs_( rPic.b - int(pTbl->rgbBlue) );

      if( iDist < iMinDist )
       {
	 iMinDist = iDist;
	 bIndex = i;
       }
    }

   return bIndex;
 }


static BYTE near pascal FindNearstColor( TRGB huge& rPic, PTColorTable pTbl, int iSize )
 {
   BYTE bIndex = 0;

   for( int iMinDist = INT_MAX, i = 0; i < iSize; i++, pTbl++ )
    {
      int iDist = abs_( int(rPic.r) - int(pTbl->rgbRed) ) +
		  abs_( int(rPic.g) - int(pTbl->rgbGreen) ) +
		  abs_( int(rPic.b) - int(pTbl->rgbBlue) );

      if( iDist < iMinDist )
       {
	 iMinDist = iDist;
	 bIndex = i;
       }
    }

   return bIndex;
 }


void LoadStr( PTIRGB p, TRGB huge* pBuf, int iWidth )
 {   
   for( int i = iWidth; i > 0; i--, pBuf++, p++ )
     *p = *pBuf;
 }

void SaveStr( PTIRGB p, TRGB huge* pBuf, int iWidth )
 {
   for( int i = iWidth; i > 0; i--, pBuf++, p++ )
     *pBuf = *p;
 }

BOOL Diffusion( PTColorTable pTbl, TRGB  huge* pBuf, DWORD dwSize, int iPalSize,
		TConvertAlho caAlho, int iWidth, int iHigh,
		PTNoisyDta pNoise,
		PTIndicator pInd )
 {  
   PFindNearstColor pFNFunc = FindNearstColorNorm;
   TAutoMemoryRc<int*> mrcIndex;
   
   if( pNoise->nNoisy == N_Marginal )
    {
      if( NoisyMarginal( pBuf, dwSize, pNoise, pInd ) == FALSE ) return FALSE;
    }
   else if( pNoise->nNoisy == N_Uniform )
    {
      randomize();
      pFNFunc = FindNearstColorNoisy;
      mrcIndex = new int[ pNoise->iUniformAmplitude ];
      if( !mrcIndex )  return FALSE;
    }


   if( pInd )
    {
      pInd->UpdateStr( 2, "Diffusion dithering filter" );
      pInd->Init( iHigh );
      pInd->Redraw();
    }

   TAutoMemoryRc<PTIRGB> mrcSti = new TIRGB[ 2 * iWidth ];
   if( !mrcSti ) return FALSE;

   PTIRGB p1 = mrcSti(), p2 = mrcSti() + iWidth;

   LoadStr( p1, pBuf, iWidth );
   //int iDir = 0;
   for( int iStr = 0; iStr < iHigh; iStr++/*, iDir = ~iDir*/ )
    {

      PTIRGB pStr, pStr2;
      pStr = p1; 
      if( iStr < iHigh - 1 )
       {
	 LoadStr( p2, pBuf + (DWORD)(iStr+1) * (DWORD)iWidth, iWidth );
	 pStr2 = p2;
       }
      else pStr2 = NULL;

      /*if( iDir )
       {
	 pStr  += (iWidth - 1);
	 if( pStr2 ) pStr2 += (iWidth - 1);

	 for( int iClm = iWidth; iClm > 0; iClm--, pStr-- )
	  {
	    //pStr->index = FindNearstColor( *pStr, pTbl, iPalSize );
	    pStr->index = pFNFunc( *pStr, pTbl, iPalSize, pNoise, mrcIndex() );

	    int  errR = pStr->r - pTbl[ pStr->index ].rgbRed,
		 errG = pStr->g - pTbl[ pStr->index ].rgbGreen,
		 errB = pStr->b - pTbl[ pStr->index ].rgbBlue;

	    if( iClm > 1 ) GammaCorrectX_16( pStr - 1, errR, errG, errB, 7 );
	    if( pStr2 )
	     {
	       if( iClm < iWidth ) GammaCorrectX_16( pStr2 + 1, errR, errG, errB, 3 );
	       GammaCorrectX_16( pStr2, errR, errG, errB, 5 );
	       if( iClm > 1 ) GammaCorrect1_16( pStr2 - 1, errR, errG, errB );
	       pStr2--;
	     }
	  }
       }
      else*/
       //{
	 for( int iClm = iWidth; iClm > 0; iClm--, pStr++ )
	  {	    
	    pStr->index = pFNFunc( *pStr, pTbl, iPalSize, pNoise, mrcIndex() );

	    int  errR = pStr->r - pTbl[ pStr->index ].rgbRed,
		 errG = pStr->g - pTbl[ pStr->index ].rgbGreen,
		 errB = pStr->b - pTbl[ pStr->index ].rgbBlue;

	    if( iClm > 1 ) GammaCorrectX_16( pStr + 1, errR, errG, errB, 7 );
	    if( pStr2 )
	     {
	       if( iClm < iWidth ) GammaCorrectX_16( pStr2 - 1, errR, errG, errB, 3 );
	       GammaCorrectX_16( pStr2, errR, errG, errB, 5 );
	       if( iClm > 1 ) GammaCorrect1_16( pStr2 + 1, errR, errG, errB );
	       pStr2++;
	     }
	  }
       //}

      SaveStr( p1, pBuf + (DWORD)iStr * (DWORD)iWidth, iWidth );
      _fmemcpy( p1, p2, sizeof(TIRGB) * iWidth );

      if( pInd && pInd->WorkMessage(iStr) == TRUE ) return FALSE;
    }

   if( pInd ) pInd->WorkIs100();

   return TRUE;
 }

static BOOL NoisyMarginal( TRGB  huge* pBuf, DWORD dwSize,
			   PTNoisyDta pNoise, PTIndicator pInd )
 {
   if( pInd )
    {
      pInd->UpdateStr( 2, "Noisy..." );
      pInd->Init( dwSize );
      pInd->Redraw();
    }
	 //iMarginalAmplitude
   //int iDitherF[ 5 ] = {-1, -2, 0, 2, 1};
   int iLen = (pNoise->iMarginalAmplitude + 1) * 2 + 1;
   TAutoMemoryRc<int*> mrcNoisy = new int[ iLen ];

   int iPow = pNoise->iMarginalAmplitude;
   for( int k = 0; k < iLen / 2; k++, iPow-- )
     mrcNoisy()[ k ] = -pow( 2, iPow );

   mrcNoisy()[ iLen / 2 ] = 0;

   iPow = 0;
   for( k = iLen / 2 + 1; k < iLen; k++, iPow++ )
     mrcNoisy()[ k ] = pow( 2, iPow );

   randomize();

   TRGB huge* pBuf_ = pBuf;
   for( DWORD i = dwSize; i > 0; i--, pBuf_++ )
    {
      int iIndex = random( iLen );

      pBuf_->r = BoundsFilterRGB( int(pBuf_->r) + mrcNoisy()[iIndex] );
      pBuf_->g = BoundsFilterRGB( int(pBuf_->g) + mrcNoisy()[iIndex] );
      pBuf_->b = BoundsFilterRGB( int(pBuf_->b) + mrcNoisy()[iIndex] );

      if( pInd && pInd->WorkMessageAdd(1) == TRUE ) return FALSE;
    }

   if( pInd ) pInd->WorkIs100();
   return TRUE;
 }


struct TColorData
 {
   int iHue, iFreq;
 };
//_PTRDEF(TColorData)
typedef TColorData* PTColorData;
typedef TColorData& RTColorData;


int ColorDataCmp( const void *v1, const void *v2 )
 {
   return ((TColorData *)v2)->iFreq - ((TColorData *)v1)->iFreq;
 }

BOOL  Quantization( PTColorTable pTbl, TRGB  huge* pBuf, DWORD dwSize, int iPalSize,
		    TConvertAlho caAlho, int iWidth, int iHigh,
		    PTNoisyDta pNoise,
		    PTIndicator pInd )
 {
   TAutoMemoryRc<BYTE*> arcColorTrans = new BYTE[ 32768 ];
   TAutoMemoryRc<PTColorData> arcColorTable = new TColorData[ 8192 ];
   if( !arcColorTrans || !arcColorTable ) return FALSE;

   PTColorTable pTblKey = pTbl;
   int iPalSizeKey = iPalSize;

   pTbl += 2; iPalSize -= 2;

   if( pInd )
    {  
      pInd->UpdateStr( 2, "Quantization true colour image..." );
      pInd->Init( (DWORD)iWidth * (DWORD)iHigh + 32768L );
      //for( int i = 0; i < 50; i++ )
	//if( pInd->WorkMessage( 1 ) == TRUE ) return FALSE;
      pInd->Redraw();
    }

   HCURSOR hcOld = SetCursor( LoadCursor(NULL, IDC_WAIT) );

   int iMax = 0;
   TRGB huge* pBuf_ = pBuf;
   for( DWORD dwI = (DWORD)iWidth * (DWORD)iHigh; dwI > 0; dwI--, pBuf_++ )
     iMax = max( iMax, (int)max(pBuf_->r, max(pBuf_->g, pBuf_->b)) );

   SetCursor( hcOld );

   _fmemset( arcColorTrans(), 0, 32768 );
   pBuf_ = pBuf;
   double dMult = 1.0/ 255.0;
   for( dwI = (DWORD)iWidth * (DWORD)iHigh; dwI > 0; dwI--, pBuf_++ )
    {
      int r = int(double(pBuf_->r) * dMult * 31.0),
	  g = int(double(pBuf_->g) * dMult * 31.0),
	  b = int(double(pBuf_->b) * dMult * 31.0);
      int index = r | (g << 5) | (b << 10);

      if( arcColorTrans()[ index ] < 255 ) (arcColorTrans()[ index ])++;

      if( pInd && pInd->WorkMessageAdd(1) == TRUE ) return FALSE;
    }

   unsigned int i = 0;
   for( int iColorsCount = 0; i < 32768; i++ )
    {
      if( pInd && pInd->WorkMessageAdd(1) == TRUE ) return FALSE;
      if( arcColorTrans()[ i ] > 0 && iColorsCount < 8192 )
       {
         arcColorTable()[ iColorsCount ].iHue = i;
	 arcColorTable()[ iColorsCount++ ].iFreq = arcColorTrans()[ i ];
       }
    }

   qsort( arcColorTable(), iColorsCount, sizeof(TColorData), ColorDataCmp );
   _fmemset( pTbl, 0, iPalSize * sizeof(TColorTable) );

   for( i = 0; i < iPalSize && i < iColorsCount; i++ )
    {                                                   //& 0x1F
      pTbl[ i ].rgbRed = 2 * (arcColorTable()[ i ].iHue & 0x1F );
      pTbl[ i ].rgbGreen = 2 * ((arcColorTable()[ i ].iHue >> 5) & 0x1F);
      pTbl[ i ].rgbBlue = 2 * ((arcColorTable()[ i ].iHue >> 10) & 0x1F);
    }

   if( caAlho & CA_Diffuzion )
     return Diffusion( pTblKey, pBuf, dwSize, iPalSizeKey, caAlho, iWidth, iHigh, pNoise, pInd );


   if( pInd )
    {  
      pInd->UpdateStr( 2, "Indexing true colour image..." );
      pInd->Init( (DWORD)iWidth * (DWORD)iHigh );
      pInd->Redraw();
    }

   pBuf_ = pBuf;
   for( dwI = (DWORD)iWidth * (DWORD)iHigh; dwI > 0; dwI--, pBuf_++ )
    {
      pBuf_->index = FindNearstColor( *pBuf_, pTblKey, iPalSizeKey );
      if( pInd && pInd->WorkMessageAdd(1) == TRUE ) return FALSE;
    }

   if( pInd ) pInd->WorkIs100();
             
   return TRUE;
 }

