//#pragma option -v-

#include "bmp.hpp"
#include <stdlib.h>
#include <alloc.h>
#include <bwcc.h>
#include <io.h>
#include <lzexpand.h>
#include "autorc.hpp"


#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;


void _EXPFUNC2 DrawBmpInRect( HDC hdc, HBITMAP hbmp, RECT& r )
 {
   HDC hdcMemory;
   HBITMAP hbmpOld;
   BITMAP bm;

   if( !hbmp ) return;

   //GetObject( hbmp, sizeof(BITMAP), &bm );

   hdcMemory = CreateCompatibleDC( hdc );

   SetMapMode( hdcMemory, GetMapMode(hdc) );
   SetViewportOrg( hdcMemory, LOWORD( GetViewportOrg(hdc) ),
	           HIWORD( GetViewportOrg(hdc) ) );
   SetWindowOrg( hdcMemory, LOWORD( GetWindowOrg(hdc) ),
	         HIWORD( GetWindowOrg(hdc) ) );
   hbmpOld = (HBITMAP)SelectObject( hdcMemory, hbmp );   

   BitBlt( hdc, r.left, r.top, r.right, r.bottom, hdcMemory, 0, 0, SRCCOPY );

   SelectObject( hdcMemory, hbmpOld );  
   DeleteDC(hdcMemory);
 }


void _EXPFUNC2 DrawBmp( HDC hdc, HBITMAP hbmp )
 {
   HDC hdcMemory;
   HBITMAP hbmpOld;
   BITMAP bm;

   if( !hbmp ) return;

   GetObject( hbmp, sizeof(BITMAP), &bm );

   hdcMemory = CreateCompatibleDC( hdc );

   SetMapMode( hdcMemory, GetMapMode(hdc) );
   SetViewportOrg( hdcMemory, LOWORD( GetViewportOrg(hdc) ),
		   HIWORD( GetViewportOrg(hdc) ) );
   SetWindowOrg( hdcMemory, LOWORD( GetWindowOrg(hdc) ),
                 HIWORD( GetWindowOrg(hdc) ) );
   hbmpOld = (HBITMAP)SelectObject( hdcMemory, hbmp );


   BitBlt( hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMemory, 0, 0, SRCCOPY );

   SelectObject( hdcMemory, hbmpOld );
   DeleteDC(hdcMemory);
 }
   //TButton


HGLOBAL TDIB::GetDIBCopy()
 {
   if( !hBitmap ) return hBitmap;

   void huge* pBmp = GlobalLock( hBitmap );
   DWORD dwSizeMem = ((LPHTBitmapInfo)pBmp)->GetFullSize();

   HGLOBAL hTmp = GlobalAlloc( GMEM_FIXED | GMEM_ZEROINIT, dwSizeMem );
   if( !hTmp )
    {
      GlobalUnlock( hBitmap );
      return NULL;
    }

   void huge* pTmp = GlobalLock( hTmp );   
   if( !pTmp || ! pBmp )
    {
      GlobalFree( hTmp );
      GlobalUnlock( hBitmap );
      return NULL;
    }

   hmemcpy( pTmp, pBmp, dwSizeMem );
   GlobalUnlock( hBitmap );
   GlobalUnlock( hTmp );


   return hTmp;
 }

int TDIB::RealizePaletteDIB( HDC Hdc )
 {
   if( !HBitmapPal ) return 0;

   HPALETTE HpalOld = SelectPalette( Hdc, HBitmapPal, FALSE );
   int iRealized = RealizePalette( Hdc );
   SelectPalette( Hdc, HpalOld, FALSE );

   return iRealized;
 }

HPALETTE TDIB::MovePalette()
 {
   HPALETTE hpalRet = HBitmapPal;
   HBitmapPal = NULL;
   return hpalRet;
 }

void TDIB::DrawStretchBitmapDIB( HDC hdc, int x, int y, int wid, int high )
 {
   LPHTBitmapInfo lpBitmap_ = (LPHTBitmapInfo)GlobalLock( hBitmap );
   if( lpBitmap_ )
    {
      StretchDIBits( hdc, x, y, wid, high, 0, 0,
	lpBitmap_->bmpHeader.biWidth, lpBitmap_->bmpHeader.biHeight,
	lpBitmap_->GetBitsPtr(),
	(LPBITMAPINFO)(lpBitmap_->GetHeaderPtr()),
	DIB_RGB_COLORS, SRCCOPY );


      GlobalUnlock( hBitmap );
    }  
 }


void TDIB::DrawStretchBitmapDIBWithPalette( HDC hdc, int x, int y, int wid, int high )
 {
   HPALETTE HpalOld = SelectPalette( hdc, HBitmapPal, FALSE );

   LPHTBitmapInfo lpBitmap_ = (LPHTBitmapInfo)GlobalLock( hBitmap );
   if( lpBitmap_ )
    {
      StretchDIBits( hdc, x, y, wid, high, 0, 0,
	lpBitmap_->bmpHeader.biWidth, lpBitmap_->bmpHeader.biHeight,
	lpBitmap_->GetBitsPtr(),
	(LPBITMAPINFO)(lpBitmap_->GetHeaderPtr()),
	DIB_RGB_COLORS, SRCCOPY );


      GlobalUnlock( hBitmap );
    }

   SelectPalette( hdc, HpalOld, FALSE );
 }

void TDIB::DrawBitmapDIBWithPalette( HDC Hdc, HPALETTE hpalAlias, int X, int Y )
 {
   HPALETTE hpal = hpalAlias ? hpalAlias:HBitmapPal;

   if( hpal )
    {
      HPALETTE HpalOld = SelectPalette( Hdc, hpal, FALSE );
      //RealizePalette( Hdc );

      DrawBitmapDIB( Hdc, 0, 0 );

      SelectPalette( Hdc, HpalOld, FALSE );
    }
   else DrawBitmapDIB( Hdc, 0, 0 );
 }

void TDIB::Reset()
 {
   if( HbmBitmap ) { DeleteObject( HbmBitmap ); HbmBitmap = NULL; }
 }

void TDIB::DrawBitmapDIB( HDC Hdc, int X, int Y )
 {
   HBITMAP Hbm, HbmOld;
   //HBITMAP HbmBitmap;
   HDC     HdcMem;
   BITMAP  Bm;
   POINT   PtSize, PtOrg;


   if( !HbmBitmap )
    {
      HCURSOR hcOld = SetCursor( LoadCursor(NULL, IDC_WAIT) );
      
      HbmBitmap = CreateDDB( Hdc );

      SetCursor( hcOld );
      if( !HbmBitmap ) return;
    }


   if( !(HdcMem = CreateCompatibleDC( Hdc )) )
    {
      //DeleteObject( HbmBitmap );
      return;
    }

   if( (HbmOld = (HBITMAP)SelectObject( HdcMem, HbmBitmap )) )
    {
      SetMapMode( HdcMem, GetMapMode(Hdc) );
      SetViewportOrg( HdcMem, LOWORD( GetViewportOrg(Hdc) ),
		      HIWORD( GetViewportOrg(Hdc) ) );
      SetWindowOrg( HdcMem, LOWORD( GetWindowOrg(Hdc) ),
		    HIWORD( GetWindowOrg(Hdc) ) );
      //SetMapMode( HdcMem, GetMapMode(Hdc) );
      GetObject( HbmBitmap, sizeof(BITMAP), (LPSTR)&Bm );

      PtSize.x = Bm.bmWidth;
      PtSize.y = Bm.bmHeight;
      //DPtoLP( Hdc, &PtSize, 1 );

      PtOrg.x = PtOrg.y = 0;
      DPtoLP( HdcMem, &PtOrg, 1 );

      BitBlt( Hdc, X, Y, PtSize.x, PtSize.y, HdcMem, PtOrg.x, PtOrg.y, SRCCOPY );

      SelectObject( HdcMem, HbmOld );          
    }

   DeleteDC( HdcMem );
   //DeleteObject( HbmBitmap );
 }

int TDIB::CheckFormat( HFILE hf )
 {
   if( lseek( hf, 0, SEEK_END ) < sizeof(BITMAPFILEHEADER) +
       sizeof(BITMAPINFOHEADER)
     )
     return ER_BITMAP_INVALID;
     
   lseek( hf, 0, SEEK_SET );

   TBitmapFileHeader  bfhFHeader;
   TBitmapInfoHeader  bihInfoHeader;
   int res;
   if( (res = _lread(hf, &bfhFHeader, sizeof(TBitmapFileHeader))) == HFILE_ERROR ||
       res != sizeof(TBitmapFileHeader)
     )  return ER_FILEIO;

   if( (res = _lread(hf, &bihInfoHeader, sizeof(TBitmapInfoHeader))) == HFILE_ERROR ||
       res != sizeof(TBitmapInfoHeader)
     )  return ER_FILEIO;

   lseek( hf, 0, SEEK_SET );

   if( bihInfoHeader.biPlanes != 1 ||
       (bihInfoHeader.biBitCount != 1 && bihInfoHeader.biBitCount != 4 &&
	bihInfoHeader.biBitCount != 8 && bihInfoHeader.biBitCount != 24) ||
       (bihInfoHeader.biCompression != BI_RGB &&
	bihInfoHeader.biCompression != BI_RLE4 &&
	bihInfoHeader.biCompression != BI_RLE8)
     ) return ER_BITMAP_INVALID;

   return 0;
 }

TDIB::TDIB( int id256, BOOL bFlCompr, PTApplication pApp )//*********
 {   
   this->pApp = pApp;
    //HbmDIB = NULL;
   hBitmap = 0;
   ErStatus = 0;
   HBitmapPal = NULL;
   HbmBitmap = NULL;

   TAutoCursorRc crcCursor( LoadCursor(NULL, IDC_WAIT) );

   HRSRC hrc = FindResource( pApp->hInstance, MAKEINTRESOURCE(id256), "BITMAP256" );
   if( !hrc )
    {
      ErStatus = ER_CREATEOBJECT;
      return;
    }
   DWORD dwSizeRc = SizeofResource( pApp->hInstance, hrc );

   TAutoFileRc<HFILE> frcRc = AccessResource( pApp->hInstance, hrc );
   if( frcRc() < 0 )
    {
      ErStatus = ER_CREATEOBJECT;
      return;
    }   

   HGLOBAL hSrc = GlobalAlloc( GMEM_FIXED | GMEM_ZEROINIT, dwSizeRc );
   if( !hSrc )
    {
      ErStatus = ER_CREATEOBJECT;
      return;
    }
   BYTE huge* pRcBuf = (BYTE huge*)GlobalLock( hSrc );
   if( !pRcBuf )
    {
      GlobalFree( hSrc );
      ErStatus = ER_CREATEOBJECT;
      return;
    }

   long resR;
   if( (resR = _hread( frcRc(), pRcBuf, dwSizeRc)) == HFILE_ERROR ||
       resR != dwSizeRc
     )
    {
      GlobalUnlock( hSrc );
      GlobalFree( hSrc );
      ErStatus = ER_FILEIO;
      return;
    }

   frcRc.FreeRc();
   GlobalUnlock( hSrc );

   if( bFlCompr == FALSE )
    {
      hBitmap =  hSrc;
      lpBitmap = NULL;
      HbmBitmap = NULL;
      dwSize = 0; 
      HBitmapPal = NULL;

      CreatePalette( 1 );

      return;
    }

   BYTE huge* pSrc = (BYTE huge*)GlobalLock( hSrc );
   if( !pSrc )
    {
      GlobalFree( hSrc );
      ErStatus = ER_CREATEOBJECT;
      return;
    }

   char cBuf[ 256 ];
   GetTempFileName( 0, "exp", 0, cBuf );
   TAutoFileRc<HFILE> frcHfExp = _lcreat( cBuf, 0 );
   if( frcHfExp() == HFILE_ERROR )
    {
      GlobalUnlock( hSrc );
      GlobalFree( hSrc );
      ErStatus = ER_FILEIO;
      return;
    }
					 
   long res = _hwrite( frcHfExp(), pSrc, dwSizeRc );
   if( res == -1 || res != dwSizeRc )
    {
      GlobalUnlock( hSrc );
      GlobalFree( hSrc );
      ErStatus = ER_FILEIO;
      return;
    }

   frcHfExp.FreeRc();
   GlobalUnlock( hSrc );
   GlobalFree( hSrc );

   OFSTRUCT of;
   TAutoFileZRc<HFILE> frcHf;
   long lPos;         //HFILE LZOpenFile(lpszFile, lpof, style)
   if( (frcHf = LZOpenFile( cBuf, &of, OF_READ ))() < 0 ||
	(lPos = LZSeek(frcHf(), 0, 2)) < 0 )
    {
      ErStatus = ER_FILEIO;      
      return;
    }

   LZSeek( frcHf(), 0, 0 );

   HGLOBAL hbm = GlobalAlloc( GMEM_FIXED | GMEM_ZEROINIT, lPos );
   BYTE huge* pImg = (BYTE huge*)GlobalLock( hbm );
   if( !pImg || !hbm )
    {
      frcHf.FreeRc();
      OpenFile( cBuf, &of, OF_DELETE );
      GlobalFree( hbm );
      ErStatus = ER_CREATEOBJECT;
      //pApp->Error( ER_CREATEOBJECT );
      return;
    }


   ldiv_t ldtDiv = ldiv( lPos, INT_MAX );
   TAutoMemoryRc<BYTE*> mrcTmp = new BYTE[ INT_MAX ];
   if( !mrcTmp )
    {
      frcHf.FreeRc();
      OpenFile( cBuf, &of, OF_DELETE );
      GlobalUnlock( hbm );
      GlobalFree( hbm );

      return;
    }
   BYTE huge* pRead = pImg;
   for( int iCount = ldtDiv.quot;  iCount >= 0; iCount--, pRead += INT_MAX )
    {
      int iLenRead, iLenNeed = (iCount > 0) ? INT_MAX:ldtDiv.rem;
      if( !iLenNeed ) break;

      if( (iLenRead = LZRead( frcHf(), mrcTmp(), iLenNeed  )) == HFILE_ERROR ||
	  iLenRead != iLenNeed
        )
       {
	 frcHf.FreeRc();
	 OpenFile( cBuf, &of, OF_DELETE );
	 ErStatus = ER_FILEIO;

         GlobalUnlock( hbm );
         GlobalFree( hbm );

	 return;
       }

      hmemcpy( pRead, mrcTmp(), iLenNeed );
    };
   

   GlobalUnlock( hbm );
   frcHf.FreeRc();
   OpenFile( cBuf, &of, OF_DELETE );

   hBitmap =  hbm;  
   lpBitmap = NULL;
   HbmBitmap = NULL;
   dwSize = 0;
  
   ErStatus = 0;

   HBitmapPal = NULL;

   CreatePalette( 1 );
 }


TDIB::TDIB( LPCSTR lpszFileName, PTApplication pApp )
 {
   HFILE    Hf;
   HCURSOR  HcrsorOld;

   this->pApp = pApp;
    //HbmDIB = NULL;
   hBitmap = 0;
   ErStatus = 0;
   HBitmapPal = NULL;
   HbmBitmap = NULL;

        
   if( (Hf = _lopen(lpszFileName, READ)) == HFILE_ERROR ) { ErrorFatal(ER_FILEIO); return; }

   int iCheck = CheckFormat( Hf );
   if( iCheck )
    {
      _lclose(Hf);
      ErStatus = iCheck;
      return;
    }

   if( (dwSize = _llseek(Hf, 0L, 2)) == -1 )
    {
      _lclose(Hf); ErrorFatal(ER_FILEIO); return;
    }
   _llseek( Hf, 0L, 0 );

   if( !(hBitmap = GlobalAlloc(GMEM_FIXED, dwSize)) )
    {
      _lclose(Hf); ErrorFatal(EM_OUTOFMEMORY); return;
    }
   if( !(lpBitmap = (LPHTBitmapInfo)GlobalLock(hBitmap)) )
    {
      _lclose( Hf ); GlobalUnlock( hBitmap ); GlobalFree( hBitmap );
      ErrorFatal(EM_OUTOFMEMORY); return;
    }


   HcrsorOld = SetCursor( LoadCursor(NULL, IDC_WAIT));

   LONG lS;
   if( (lS = _hread( Hf, lpBitmap, dwSize )) == -1 || lS != dwSize )
    {
      _lclose(Hf); GlobalUnlock( hBitmap ); GlobalFree(hBitmap);
      SetCursor( HcrsorOld );
      ErrorFatal(ER_FILEIO); return;
    }


   SetCursor( HcrsorOld );
   int res;
   if( (res = lpBitmap->CheckData()) )
    {   
      pApp->Error( res );
    }



   GlobalUnlock( hBitmap );
   _lclose( Hf );

   CreatePalette( 1 );
 }

TDIB::TDIB( HGLOBAL hBitmapInfo, PTApplication pApp )
 {
   hBitmap =  hBitmapInfo;  //unfixed
   lpBitmap = NULL;
   HbmBitmap = NULL;
   dwSize = 0;

   this->pApp = pApp; 
   ErStatus = 0;

   HBitmapPal = NULL;

   CreatePalette( 1 );
 }
  //**********
TDIB::TDIB( TRGB huge* pBuf, int wid, int high, PTColorTable pctTbl,
	    int iPalSize, BOOL bFlOverflip, PTApplication pApp )
 {
   hBitmap =  NULL;  //unfixed
   lpBitmap = NULL;
   HbmBitmap = NULL;
   dwSize = 0;

   this->pApp = pApp; 
   ErStatus = 0;

   HBitmapPal = NULL;


   HGLOBAL      hbmInfo;
   LPTDiskDIBBitmap  pbiInfo;


   DWORD  dwLenPal = iPalSize;
   DWORD  dwBytePerPixel = pctTbl ? 1:3;
   DWORD  dwBitCount = dwBytePerPixel * 8;

  
   DWORD dwSizeBlc;		
   if( !(hbmInfo = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
			       (dwSizeBlc =
			       sizeof(BITMAPINFOHEADER) +
                               sizeof(BITMAPFILEHEADER) +
			       sizeof(RGBQUAD) * dwLenPal +
			       (((DWORD)dwBitCount * (DWORD)wid + 31UL) / 32UL*4UL) * (DWORD)high
			       //(DWORD)wid * (DWORD)high * dwBytePerPixel
				)
			      )
	)
     )  { ErStatus = EM_OUTOFMEMORY; return; }
   pbiInfo = (LPTDiskDIBBitmap)GlobalLock( hbmInfo );

   if( !pbiInfo )
    {
      GlobalUnlock( hbmInfo );
      GlobalFree( hbmInfo );

      BWCCMessageBox( NULL, "Not enought memory for bitmap",
	"Error", MB_ICONSTOP | MB_OK | MB_APPLMODAL );

      ErStatus = EM_OUTOFMEMORY; return;
    }


   pbiInfo->bmFile.bfType = 0x4D42;
   pbiInfo->bmFile.bfSize = dwSizeBlc;
   pbiInfo->bmFile.bfOffBits = 
                               sizeof(BITMAPINFOHEADER) +
                               sizeof(BITMAPFILEHEADER) +
			       sizeof(RGBQUAD) * dwLenPal;

   pbiInfo->bmInf.bmiHeader.biSize           = sizeof(BITMAPINFOHEADER);
   pbiInfo->bmInf.bmiHeader.biWidth          = wid;
   pbiInfo->bmInf.bmiHeader.biHeight         = high;
   pbiInfo->bmInf.bmiHeader.biPlanes         = 1;
   //pbiInfo->bmInf.bmiHeader.biBitCount       = GetDeviceCaps( hdc, BITSPIXEL ) *
   //					       GetDeviceCaps( hdc, PLANES );
   pbiInfo->bmInf.bmiHeader.biBitCount       = dwBitCount;
   pbiInfo->bmInf.bmiHeader.biCompression    = BI_RGB;
   //pbiInfo->bmInf.bmiHeader.biSizeImage      = (DWORD)wid * (DWORD)high;
   //pbiInfo->bmInf.bmiHeader.biSizeImage      = (DWORD)wid * (DWORD)high * dwBytePerPixel;
   pbiInfo->bmInf.bmiHeader.biSizeImage      = (((DWORD)dwBitCount * (DWORD)wid + 31UL) / 32UL*4UL) * (DWORD)high;
   pbiInfo->bmInf.bmiHeader.biXPelsPerMeter  = 0;
   pbiInfo->bmInf.bmiHeader.biYPelsPerMeter  = 0;
   pbiInfo->bmInf.bmiHeader.biClrUsed        = dwLenPal;
   pbiInfo->bmInf.bmiHeader.biClrImportant   = pctTbl ? 0:256;
	


   if( pctTbl )
     _fmemcpy( pbiInfo->bmInf.bmiColors, pctTbl, sizeof(RGBQUAD) * dwLenPal );


   int widAlign = wid * dwBytePerPixel;
   while( widAlign % 4 ) widAlign++;

   if( pctTbl )
    {
      if( bFlOverflip == FALSE )
        for( int iStr = 0; iStr < high; iStr++ )
         {

	   TRGB huge *pSrc = pBuf + (DWORD)iStr * (DWORD)wid;
	   BYTE huge *pBits = ((BYTE huge *)pbiInfo) + (DWORD)pbiInfo->bmFile.bfOffBits +
			      (DWORD)iStr * (DWORD)widAlign;

	   for( int iClm = 0; iClm < wid; iClm++, pBits++, pSrc++  )
	     *pBits = pSrc->index;
	 }
      else
        for( int iStr = 0; iStr < high; iStr++ )
         {

	   TRGB huge *pSrc = pBuf + (DWORD)wid * (DWORD)high -
			     (DWORD)iStr * (DWORD)wid - (DWORD)wid;
	   BYTE huge *pBits = ((BYTE huge *)pbiInfo) + (DWORD)pbiInfo->bmFile.bfOffBits +
			      (DWORD)iStr * (DWORD)widAlign;

	   for( int iClm = 0; iClm < wid; iClm++, pBits++, pSrc++ /*RGB*/ )
	     *pBits = pSrc->index;
         }
    }
   else
    {
      if( bFlOverflip == FALSE )
        for( int iStr = 0; iStr < high; iStr++ )
         {

	   TRGB huge *pSrc = pBuf + (DWORD)iStr * (DWORD)wid;
	   BYTE huge *pBits = ((BYTE huge *)pbiInfo) + (DWORD)pbiInfo->bmFile.bfOffBits +
			      (DWORD)iStr * (DWORD)widAlign;

	   for( int iClm = 0; iClm < wid; iClm++, pBits += 3, pSrc++  )	     
            {
	      *pBits = pSrc->b;
	      *(pBits+1) = pSrc->g;
	      *(pBits+2) = pSrc->r;
	    }
	 }
      else
	for( int iStr = 0; iStr < high; iStr++ )
         {

	   TRGB huge *pSrc = pBuf + (DWORD)wid * (DWORD)high -
			     (DWORD)iStr * (DWORD)wid - (DWORD)wid;
	   BYTE huge *pBits = ((BYTE huge *)pbiInfo) + (DWORD)pbiInfo->bmFile.bfOffBits +
			      (DWORD)iStr * (DWORD)widAlign;

	   for( int iClm = 0; iClm < wid; iClm++, pBits += 3, pSrc++ /*RGB*/ )
            {
	      //*(TRGB huge *)pBits = *pSrc;
	      *pBits = pSrc->b;
	      *(pBits+1) = pSrc->g;
	      *(pBits+2) = pSrc->r;
	    }
         }
    }

   GlobalUnlock( hbmInfo );

   hBitmap = hbmInfo;
   CreatePalette( 1 );
 }


TDIB::~TDIB()
 {
   //if( HbmDIB ) DeleteObject( HbmDIB );

   if( hBitmap )
    {   
      GlobalUnlock( hBitmap );
      GlobalFree( hBitmap );
      hBitmap = NULL;
    }

   if( HBitmapPal ) { DeleteObject( HBitmapPal ); HBitmapPal = NULL; }

   if( HbmBitmap ) { DeleteObject( HbmBitmap ); HbmBitmap = NULL; }
 }
 

  
int TDIB::Save( LPCSTR lpFileName ) const
 {
   OFSTRUCT of;
   LPHTBitmapInfo lpBitmap; 
              
   if( !hBitmap ) return 0;

   HFILE hf = OpenFile( lpFileName, &of, OF_EXIST );
   if( hf != HFILE_ERROR )
    {
      char chBuf[256];

      wsprintf( chBuf, "File [%s] already exist. Owerwrite ?", lpFileName );
      int iRes = BWCCMessageBox( NULL, chBuf, "Warning",
	MB_ICONEXCLAMATION | MB_YESNO | MB_APPLMODAL );

      if( iRes != IDYES ) return 0;
    }

    if( (hf = OpenFile( lpFileName, &of, OF_CREATE )) == HFILE_ERROR )
     {
       char chBuf[256];

       wsprintf( chBuf, "Writing in file [%s] failed", lpFileName );
       BWCCMessageBox( NULL, chBuf, "Error",
	 MB_ICONSTOP | MB_OK | MB_APPLMODAL );

       return -1;
     }

   lpBitmap = (LPHTBitmapInfo)GlobalLock( hBitmap );
   if( !lpBitmap )
    {
      _lclose( hf );
      return -2;
    }

   HCURSOR hcOld = SetCursor( LoadCursor(NULL, IDC_WAIT) );
   long res = _hwrite( hf, lpBitmap, lpBitmap->GetFullSize() );
   SetCursor( hcOld );

   if( res == -1 || (DWORD)res != (DWORD)lpBitmap->GetFullSize() )
    {
      char chBuf[256];

      GlobalUnlock( hBitmap );
      _lclose( hf );

      wsprintf( chBuf, "Error writing in file [%s] failed", lpFileName );
      BWCCMessageBox( NULL, chBuf, "Error",
        MB_ICONSTOP | MB_OK | MB_APPLMODAL );

      OpenFile( lpFileName, &of, OF_DELETE );

      return -1;
    } 

   GlobalUnlock( hBitmap );
   _lclose( hf );
 }

BOOL  TDIB::TestType() const
 {
   BOOL ret;
   LPHTBitmapInfo lpBitmap_ = (LPHTBitmapInfo)GlobalLock( hBitmap );

   ret = lpBitmap_->TestType();

   GlobalUnlock( hBitmap );
   return ret;
 }

BOOL  TDIB::IsWindows() const
 {
   BOOL ret;
   LPHTBitmapInfo lpBitmap_ = (LPHTBitmapInfo)GlobalLock( hBitmap );

   ret = lpBitmap_->IsWindows();

   GlobalUnlock( hBitmap );
   return ret;
 }

WORD  TDIB::BitsPerPixel() const
 {
   WORD ret;
   LPHTBitmapInfo lpBitmap_ = (LPHTBitmapInfo)GlobalLock( hBitmap );

   ret = lpBitmap_->bmpHeader.biBitCount;

   GlobalUnlock( hBitmap );
   return ret;
 }

WORD  TDIB::NumbColors() const
 {
   WORD ret;
   LPHTBitmapInfo lpBitmap_ = (LPHTBitmapInfo)GlobalLock( hBitmap );

   ret = lpBitmap_->bmpHeader.NumbColors();

   GlobalUnlock( hBitmap );
   return ret;
 }

DWORD   TDIB::Compression() const
 {
   DWORD ret;
   LPHTBitmapInfo lpBitmap_ = (LPHTBitmapInfo)GlobalLock( hBitmap );

   ret = lpBitmap_->bmpHeader.biCompression;

   GlobalUnlock( hBitmap );
   return ret;
 }


LONG TDIB::GetWidth() const
 {
   LONG ret;
   LPHTBitmapInfo lpBitmap_ = (LPHTBitmapInfo)GlobalLock( hBitmap );

   ret = lpBitmap_->bmpHeader.biWidth;

   GlobalUnlock( hBitmap );
   return ret;
 }


LONG TDIB::GetHigh() const
 {
   LONG ret;
   LPHTBitmapInfo lpBitmap_ = (LPHTBitmapInfo)GlobalLock( hBitmap );

   ret = lpBitmap_->bmpHeader.biHeight;

   GlobalUnlock( hBitmap );
   return ret;
 }

DWORD TDIB::ClrUsed() const
 {
   DWORD dwRet;
   LPHTBitmapInfo lpBitmap_ = (LPHTBitmapInfo)GlobalLock( hBitmap );

   dwRet = lpBitmap_->bmpHeader.biClrUsed;

   GlobalUnlock( hBitmap );
   return dwRet;
 }


void TDIB::CreatePaletteAssign()
 {
   CreatePalette( 1 );
 }

HPALETTE TDIB::CreatePalette()
 {
   CreatePalette( 1 );

   HPALETTE hpalRet = HBitmapPal;
   HBitmapPal = NULL;

   return hpalRet;
 }

void TDIB::CreatePalette( int )
 {
   LPLOGPALETTE  lpPal;
   HPALETTE      hPal = NULL;
   HANDLE        hLogPal;
   int           i, wNumColors;


   HBitmapPal = NULL;

   if( ErStatus ) return;

   lpBitmap = (LPHTBitmapInfo)GlobalLock( hBitmap );

   wNumColors = lpBitmap->bmpHeader.NumbColors();

   if( !wNumColors ) { GlobalUnlock(hBitmap); return; }

   if( !(hLogPal = GlobalAlloc( GHND, sizeof(LOGPALETTE) + sizeof(PALETTEENTRY)*wNumColors )) )
    {
      GlobalUnlock(hBitmap); return;
    }

   lpPal = (LPLOGPALETTE)GlobalLock( hLogPal );
   lpPal->palVersion = 0x300;
   lpPal->palNumEntries = wNumColors;

   for( i = 0; i < wNumColors; i++ )
    {
      lpPal->palPalEntry[i].peRed = lpBitmap->bmpPalette[i].rgbRed;
      lpPal->palPalEntry[i].peGreen = lpBitmap->bmpPalette[i].rgbGreen;
      lpPal->palPalEntry[i].peBlue = lpBitmap->bmpPalette[i].rgbBlue;
      lpPal->palPalEntry[i].peFlags = NULL;
    }

   hPal = ::CreatePalette( lpPal );
   GlobalUnlock( hBitmap );
   GlobalUnlock( hLogPal ); GlobalFree( hLogPal );


   HBitmapPal = hPal;
 }

int TBitmapInfo::CheckData()
 {
   if( TestType() == FALSE ) return ER_BITMAP_INVALID;
   if( IsWindows() == FALSE ) return ER_BITMAP_NONWINDOWS;
   if( IsValidCode() == FALSE ) return ER_BITMAP_NOTSUPPORT_FORMAT;
   return 0;
 }

DWORD TBitmapInfo::GetFullSize() const
 {
   /*return  bmpHeader.GetSizeImage() +
	   sizeof(BITMAPINFOHEADER) +
           sizeof(BITMAPFILEHEADER) +
	   sizeof(RGBQUAD) * (DWORD)bmpHeader.NumbColors();*/
   return bmpFHeader.bfOffBits + bmpHeader.GetSizeImage();

 }

LPSTR TBitMap::GetClassName()
 {
   return "3DW:BITMAPDIB";
 } 


void TBitMap::GetWindowClass(WNDCLASS FAR & AWndClass)
 {
   TControl::GetWindowClass( AWndClass );

   AWndClass.style |= WS_CHILD | WS_VISIBLE;
 }


TBitMap::TBitMap( PTApplication pApp, HINSTANCE hinst, LPCSTR lpszFileName,
	          PTWindowsObject AParent, int AnId, LPSTR ATitle, int X, int Y,
		  int W, int H ):
  TControl( AParent, AnId, ATitle, X, Y, W, H, NULL ),   //TWindow
  TDIB( lpszFileName, pApp )
 {

    this->hinst = hinst;
    if( !(Status = ErStatus) )
     {
       if( !W ) Attr.W = lpBitmap->bmpHeader.biWidth;
       if( !H ) Attr.H = lpBitmap->bmpHeader.biHeight;
     }
 }

TBitMap::TBitMap( PTApplication pApp, HINSTANCE hinst, int id256, BOOL bFlCompr,
	          PTWindowsObject AParent, int AnId, LPSTR ATitle, int X, int Y,
		  int W, int H ):
  TControl( AParent, AnId, ATitle, X, Y, W, H, NULL ),   //TWindow
  TDIB( id256, bFlCompr, pApp )
 {

    this->hinst = hinst;
    if( !(Status = ErStatus) )
     {
       if( !W ) Attr.W = lpBitmap->bmpHeader.biWidth;
       if( !H ) Attr.H = lpBitmap->bmpHeader.biHeight;
     }
 }


void TBitMap::SetupWindow()
 {
   HPALETTE HpalOld;
   HDC      Hdc;
   HCURSOR  HcrsorOld;


   HcrsorOld = SetCursor( LoadCursor(NULL, IDC_WAIT));
   TControl::SetupWindow();
   
   /*if( (Hdc = GetDC( HWindow )) )    
     HBitmapPal = CreatePalette();
   */
    

/*   if( HBitmapPal && Hdc )
    {
      HpalOld = SelectPalette( Hdc, HBitmapPal, FALSE );
      RealizePalette( Hdc );
      SelectPalette( Hdc, HpalOld, FALSE );
    }*/

   ReleaseDC( HWindow, Hdc );
   SetCursor( HcrsorOld );
 }


void TBitMap::WMEraseBkGnd( RTMessage Msg )
 {
   Msg.Result = 0;
 }


void TBitMap::WMPaletteChanged( RTMessage Msg )
 {
   if( (HWND)(Msg.WParam) == HWindow )
    {
      Msg.Result = 0;
      return;
    }
   else WMQueryNewPalette( Msg );
 }


void TBitMap::WMQueryNewPalette( RTMessage Msg)
 {
  //WM_QUERYNEWPALETTE
   HDC  Hdc;
   HPALETTE HpalOld;
   int i;

   Hdc = GetDC( HWindow );
   HpalOld = SelectPalette( Hdc, HBitmapPal, FALSE );

   i = RealizePalette( Hdc );

   SelectPalette( Hdc, HpalOld, FALSE);  
   ReleaseDC( HWindow, Hdc);

   if (i > 0)
     InvalidateRect( HWindow, NULL, TRUE);


   Msg.Result = i;

 }


void TBitMap::WMPaint( RTMessage Msg )
 {
   PAINTSTRUCT PaintInfo;
   HDC  HdcPaint = BeginPaint( HWindow, &PaintInfo );
   HPALETTE HpalOld;
   //HBITMAP HbmDDB;
   HCURSOR  HcrsorOld;


   HcrsorOld = SetCursor( LoadCursor(NULL, IDC_WAIT));

   PaintInfo.fErase = FALSE;

   if( 1/*HBitmapPal*/ )
    {
      HpalOld = SelectPalette( HdcPaint, HBitmapPal, FALSE );

      //int i = RealizePalette( HdcPaint );
/*      if( (HbmDDB = CreateDDB( HdcPaint )) )
       {*/
	 DrawBitmapDIB( HdcPaint, 0, 0 );
/*	 DeleteObject( HbmDDB );
       }*/

      SelectPalette( HdcPaint, HpalOld, FALSE );
    }

   EndPaint( HWindow, &PaintInfo );
   Msg.Result = 0;
   SetCursor( HcrsorOld );
 }


TBitMap::~TBitMap()
 {
   //if( HBitmapPal ) DeleteObject( HBitmapPal );
 }


WORD TBitmapInfoHeader::NumbColors()
 {
   if( biClrUsed ) return biClrUsed;

   switch( biBitCount )
    {
      case 1:
	return 2;
      case 4:
	return 16;
      case 8:
	return 256;
      default:
	return 0;
    }
 }

void TDIB::RGBDraw( HDC Hdc, LONG X, LONG Y, UINT StaStr, UINT NumStr  )
 {
   LPHTBitmapInfo lpBitmap_ = (LPHTBitmapInfo)GlobalLock( hBitmap );

   lpBitmap_->RGBDraw( Hdc, X, Y, StaStr, NumStr  );

   GlobalUnlock( hBitmap );
 }


void TBitmapInfo::RGBDraw( HDC Hdc, LONG X, LONG Y, UINT StaStr, UINT NumStr  )
 {
   LPHSTR  LphBits;
   DWORD   Count;
   int     Cx, Cy;

   StaStr = (bmpHeader.biHeight < StaStr) ? bmpHeader.biHeight:StaStr;
   LphBits = GetBitsPtr() + StaStr * bmpHeader.biWidth;
   if( !NumStr ) Count = bmpHeader.biHeight;
   else Count = (bmpHeader.biHeight - StaStr < NumStr) ? bmpHeader.biHeight - StaStr:
							 NumStr;

   Cy = Y + Count;
   for( Count *= (DWORD)(bmpHeader.biWidth); Count > 0; Cy-- )
     for( Cx = X; Cx-X <= bmpHeader.biWidth+1 && Count > 0; Cx++, Count--, LphBits++ )
       SetPixel( Hdc, Cx, Cy, PALETTERGB(bmpPalette[*LphBits].rgbRed,
					 bmpPalette[*LphBits].rgbGreen,
					 bmpPalette[*LphBits].rgbBlue) );
 }



LPHSTR TBitmapInfo::GetBitsPtr()
 { 

   if( bmpFHeader.bfOffBits ) return ((LPHSTR)this + bmpFHeader.bfOffBits);

   return (LPHSTR)this + bmpHeader.biSize +
	  (DWORD)(bmpHeader.NumbColors() * sizeof(RGBQUAD));
 }


DWORD TBitmapInfoHeader::GetSizeImage()
 {
   if( biSizeImage ) return biSizeImage;
   return (((DWORD)biBitCount * (DWORD)biWidth + 31UL) / 32UL*4UL) * (DWORD)biHeight;
 }

HBITMAP  TDIB::CreateDDB( HDC hdc, UINT uStaScan, UINT uScanLines )
 {
   HBITMAP  hbmp;


   lpBitmap = (LPHTBitmapInfo)GlobalLock( hBitmap );

   if( !(hbmp = CreateCompatibleBitmap( hdc, lpBitmap->bmpHeader.biWidth,
				  lpBitmap->bmpHeader.biHeight )) )
    {
      GlobalUnlock( hBitmap );
      return NULL;
    }

   SetDIBits( hdc, hbmp, uStaScan, uScanLines ? uScanLines:lpBitmap->bmpHeader.biHeight,
	      lpBitmap->GetBitsPtr(), (LPBITMAPINFO)(lpBitmap->GetHeaderPtr()),
	      DIB_RGB_COLORS );

   GlobalUnlock( hBitmap );


   return hbmp;
 }

TRGB  huge* TDIB::GetTRGBBits()
 {
   TRGB huge* pTmp;

   lpBitmap = (LPHTBitmapInfo)GlobalLock( hBitmap );
   if( !lpBitmap ) return NULL;

   DWORD dwSize = (DWORD)lpBitmap->bmpHeader.biWidth * (DWORD)lpBitmap->bmpHeader.biHeight;
   pTmp = (TRGB  huge*)farcalloc( dwSize, sizeof(TRGB) );
   //new TRGB[ (dwSize = (DWORD)lpBitmap->bmpHeader.biWidth * (DWORD)lpBitmap->bmpHeader.biHeight) ];
   if( !pTmp )
    {
      GlobalUnlock( hBitmap );
      return NULL;
    }

   int iWidtAln = (lpBitmap->bmpHeader.biBitCount / 8) * lpBitmap->bmpHeader.biWidth;
   //int iWidtAln = 3 * lpBitmap->bmpHeader.biWidth;
   while( iWidtAln % 4 ) iWidtAln++;

   TRGB huge* pTmpPtr;
   BYTE huge* pBits;
   for( int iStr = 0; iStr < lpBitmap->bmpHeader.biHeight; iStr++ )
    {
      pTmpPtr = pTmp + (DWORD)iStr * (DWORD)lpBitmap->bmpHeader.biWidth;
      pBits = lpBitmap->GetBitsPtr() + (DWORD)iStr * (DWORD)iWidtAln;

      for( int iClm = 0; iClm < lpBitmap->bmpHeader.biWidth; iClm++, pTmpPtr++, pBits += 3 )
	*pTmpPtr = TRGB( *(pBits+2), *(pBits+1), *pBits );
    }

   GlobalUnlock( hBitmap );

   return pTmp;
 }

#pragma option -v.
