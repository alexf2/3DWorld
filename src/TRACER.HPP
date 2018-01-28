#if !defined(_TRACER_)
#define _TRACER_

#include <alloc.h>

#include "surface.hpp"
#include "rgb.hpp"
#include "bkgnd.hpp"
#include "autorc.hpp"
#include "vector.hpp"


struct TTracData
 {
   PTSurface  psfSurf;
   PTFacet    pFac;
   double     z;

   TTracData() {};
   TTracData( PTSurface  psfSurf_, PTFacet pFac_, double z_ )
    {
      psfSurf = psfSurf_;
      pFac = pFac_;
      z = z_;
    }
 };
typedef TTracData* PTTracData;
typedef TTracData& RTTracData;

struct TStrTracData: public TTracData
 {
   double x, y;

   TStrTracData() {};
   TStrTracData( PTSurface  psfSurf_, PTFacet pFac_, double x_, double y_, double z_  ):
     TTracData( psfSurf_, pFac_, z_ )
    {
      x = x_; y = y_;
    }
 };
typedef TStrTracData* PTStrTracData;
typedef TStrTracData& RTStrTracData;



class  TTracer
 {
public:
  int Status;

  //TTracer( int width, HDC hdcBmp );
  TTracer( RECT& r, HDC hdc, RGBQUAD& rgbAdgeColor, RGBQUAD& rgbIntersectColor );
  ~TTracer();

  void  near pascal Pixel( int x, int y, PTSurface pF, PTFacet pFac, double z );
  void near pascal Start( PTBackGraund pBkGnd, PTIndicator pInd );
  void near pascal End();

  //HGLOBAL near pascal CreateGetBitmap( PTColorTable pctTbl, int iPalSize ) const;

  TRGB huge* Buffer() const { return videoBuf; }
  DWORD SizeBuf() const { return (DWORD)wid * (DWORD)high; }

  void Bounds( RECT& r ) { bounds = r; }

  void near pascal NextStr( int iYCurStr, BOOL bFlInitBuf = FALSE );
  void near pascal NextStrBack( int iYCurStr, BOOL bFlInitBuf = FALSE );
  void near pascal EndBack();
  void  near pascal PixelSmart( int x, PTSurface pF, PTFacet pFac, double z );

  void near pascal PixelAbs( int x, int y, RTRGB rRGB );


private:
  enum TColorClass { TC_Surface, TC_Fon, TC_Adge, TC_Intersect };

  int wid, high;
  RECT bounds;
  HDC hdc;  

  PTTracData pBuf[2];

  TRGB rgbAdge, rgbIntersect;

  int clm, str;

  TRGB  huge* videoBuf;

  //TRGB _FAR* near pascal GetCol( BOOL checkX, BOOL checkY ) const;
  TRGB near pascal GetColor( PTTracData p1, PTTracData p2, TColorClass& tc ) const;
 };

struct huge TRGBd;
typedef TRGBd huge* PTRGBd;
typedef TRGBd huge& RTRGBd;

struct TRGBd
 {
   float huge dR, huge dG, huge dB;

   TRGBd() {};

   TRGBd( double dR_, double dG_, double dB_ )
    {
      dR = dR_;  dG = dG_;  dB = dB_;
    }

   TRGBd( RTRGBd rgb );

   RTRGBd operator=( RTRGBd rgb );
   TRGBd( RGBQUAD rgbQ );

 };

inline double max( double d1, double d2 )
 {
   return (d1 > d2) ? d1:d2;
 }

inline float max( float d1, float d2 )
 {
   return (d1 > d2) ? d1:d2;
 }


class TTracerPhoto
 {
public:
  TTracerPhoto( DWORD dwWid, DWORD dwHigh );

  ~TTracerPhoto();

  int Status() { return !!rgbBuf && pBuf; }
  void Free() { rgbBuf.FreeRc(); delete pBuf; pBuf = NULL; }
  double Max() const { return dMax; }

  void PixelAbs( int x, int y, double dR, double dG, double dB )
   {
     rgbBuf()[ DWORD(y) * dwWidth + DWORD(x) ] = TRGBd( dR, dG, dB );
   }
  void  near pascal PixelSmart( int iX, double x, double y, double z, PTSurface pF, PTFacet pFac );  


  void near pascal CalcMaxVal();
  void near pascal IntBuf( TRGB  huge* pRGBBuf );
  void near pascal ConvertToRGB( TRGB  huge* pRGBBuf, BOOL bFlStrictGamma );

  void StartShading() { pBufIter = pBuf; iCountIter = 0; }
  operator int() { return iCountIter < dwWidth; }
  PTStrTracData operator++(int);
  PTStrTracData Current() const { return pBufIter; }
  void InitStr()
   {
     _fmemset( pBuf, 0, sizeof(TStrTracData) * (dwWidth + 1) );
   }

  TVector near pascal GetVectorBkGnd( int clm, int str );


private:
  TAutoMemoryHugeRc<PTRGBd> rgbBuf;
  double dMax;
  DWORD dwSize;
  DWORD dwWidth;

  PTStrTracData pBuf;

  PTStrTracData pBufIter;
  int iCountIter;
 };

struct TRGBKey
 {
   TVector vLeftTop, vRightTop;
   BOOL bFlO1, bFlO2;

   TRGBKey()
    {
    }

   TRGBKey( RTVector rgbdL, BOOL bFlO1_,
	    RTVector rgbdR, BOOL bFlO2_
	  )
    {
      vLeftTop = rgbdL;
      vRightTop = rgbdR;

      bFlO1 = bFlO1_;
      bFlO2 = bFlO2_;
    }
 };
typedef TRGBKey* PTRGBKey;
typedef TRGBKey& RTRGBKey;


#endif
