#if !defined(_BMP_)
#define _BMP_

#include <owl.h>
#include <control.h>
#include "INC\errors.inc"
#include "rgb.hpp"

#undef _EXPORT2
#if defined(_BMPDLL) || defined(_ALLCTLDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif

#undef _EXPFUNC2
#if defined(_BMPDLL) || defined(_ALLCTLDLL)
  #define _EXPFUNC2 _export
#else
  #define _EXPFUNC2 _EXPFUNC
#endif


//void DrawBitmapDIB( HDC Hdc, int X, int Y, HBITMAP Hbm );

void _EXPFUNC2 DrawBmp( HDC hdc, HBITMAP hbmp );
void _EXPFUNC2 DrawBmpInRect( HDC hdc, HBITMAP hbmp, RECT& r );


typedef struct
 {
   BITMAPFILEHEADER  bmFile;
   BITMAPINFO        bmInf;
   BYTE              bmBits[1];
 } TDiskDIBBitmap;

typedef TDiskDIBBitmap FAR *LPTDiskDIBBitmap;


struct _EXPORT2 TBitmapFileHeader {   

  UINT   bfType;
  DWORD  bfSize;
  UINT   bfReserved1;
  UINT   bfReserved2;
  DWORD  bfOffBits;

};


struct _EXPORT2 TBitmapInfoHeader {

  DWORD  biSize;
  LONG   biWidth;
  LONG   biHeight;
  WORD   biPlanes;
  WORD   biBitCount;
  DWORD  biCompression;
  DWORD  biSizeImage;
  LONG   biXPelsPerMeter;
  LONG   biYPelsPerMeter;
  DWORD  biClrUsed;
  DWORD  biClrImportant;

public:
  WORD  NumbColors();
  DWORD GetSizeImage();

 };


struct _EXPORT2 TRGBPalCell {

  BYTE  rgbBlue;
  BYTE  rgbGreen;
  BYTE  rgbRed;
  BYTE  rgbReserved;

};


typedef char huge *LPHSTR;

struct _EXPORT2 TBitmapInfo {

  TBitmapFileHeader  bmpFHeader;
  TBitmapInfoHeader  bmpHeader;
  TRGBPalCell        bmpPalette[1];

public:
  BOOL  TestType() { return (BOOL)(bmpFHeader.bfType == 0x4D42); }
  BOOL  IsWindows() { return (BOOL)(bmpHeader.biSize == 40); }
  BOOL  IsValidCode()
   {
     return (BOOL)(bmpHeader.biPlanes == 1 &&
       (bmpHeader.biBitCount <= 8 || bmpHeader.biBitCount == 24) &&
       bmpHeader.biCompression == BI_RGB);
   }

  int CheckData();
  LPHSTR GetBitsPtr();
  TBitmapInfoHeader *GetHeaderPtr() { return &bmpHeader; }

  void RGBDraw( HDC Hdc, LONG X, LONG Y, UINT StaStr=0, UINT NumStr=0 );


  DWORD GetFullSize() const;
};

typedef TBitmapInfo huge* LPHTBitmapInfo;

_CLASSDEF(TBitMap)
_CLASSDEF(TDIB)


class _EXPORT2 TDIB
 {

friend class TBitMap;

public:
  TDIB( LPCSTR lpszFileName, PTApplication pApp );
  TDIB( HGLOBAL hBitmapInfo/*unfixed*/, PTApplication pApp );
  TDIB( int id256, BOOL bFlCompr, PTApplication pApp );
  TDIB( TRGB huge* pBuf, int wid, int high, PTColorTable pctTbl,
        int iPalSize, BOOL bFlOverflip, PTApplication pApp );
  ~TDIB();

  void DrawBitmapDIB( HDC Hdc, int X, int Y );
  void DrawBitmapDIBWithPalette( HDC Hdc, HPALETTE hpalAlias, int X, int Y );
  void DrawStretchBitmapDIB( HDC hdc, int x, int y, int wid, int high );
  void DrawStretchBitmapDIBWithPalette( HDC hdc, int x, int y, int wid, int high );

  HPALETTE MovePalette();
  void CreatePalette( int );
  void CreatePaletteAssign();
  int RealizePaletteDIB( HDC Hdc );
  HPALETTE CreatePalette();
  HPALETTE GetPal() const { return HBitmapPal; }
  HBITMAP   CreateDDB( HDC hdc, UINT uStaScan = 0, UINT uScanLines = 0 );

  void Reset();

  int ErStatus;

  int Save( LPCSTR lpFileName ) const;

  LONG GetWidth() const;
  LONG GetHigh() const;
  DWORD ClrUsed() const;
  WORD  BitsPerPixel() const;
  WORD  NumbColors() const;
  DWORD   Compression() const;

  BOOL  TestType() const;
  BOOL  IsWindows() const;


  void RGBDraw( HDC Hdc, LONG X, LONG Y, UINT StaStr, UINT NumStr  );

  TRGB  huge* GetTRGBBits();

  HGLOBAL GetDIBCopy();

protected:
  HGLOBAL   hBitmap;
  LPHTBitmapInfo lpBitmap;
  DWORD  dwSize;

  HPALETTE  HBitmapPal;


  virtual void ErrorFatal( int ErCode ) { ErStatus = ErCode; }

private:
  HBITMAP HbmBitmap;

  PTApplication pApp;

  BOOL bFlFileCreated;

  int CheckFormat( HFILE hf );
 };
             
class _EXPORT2 TBitMap: public TControl, public TDIB
 {

public:
  TBitMap( PTApplication pApp, HINSTANCE hinst, LPCSTR lpszFileName,
	   PTWindowsObject AParent, int AnId, LPSTR ATitle, int X, int Y,
	   int W = 0, int H = 0 );

  TBitMap( PTApplication pApp, HINSTANCE hinst, int id256, BOOL bFlCompr,
	   PTWindowsObject AParent, int AnId, LPSTR ATitle, int X, int Y,
	   int W = 0, int H = 0 );

  virtual ~TBitMap();

  //int Status;
  virtual void GetWindowClass(WNDCLASS FAR & AWndClass);
  virtual LPSTR GetClassName();
	 //TWindow

  void WMQueryNewPalette( RTMessage );
  void WMPaletteChanged( RTMessage ); 


protected:
  virtual void SetupWindow();
  virtual void WMPaint( RTMessage ) = [WM_FIRST + WM_PAINT];
  virtual void WMEraseBkGnd( RTMessage ) = [WM_FIRST + WM_ERASEBKGND];

  virtual void ErrorFatal( int ErCode ) { Status = ErStatus = ErCode; }

private:
  HINSTANCE hinst;

 };   

#endif
