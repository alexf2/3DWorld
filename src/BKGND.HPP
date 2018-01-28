#if !defined(_BACKGRAUND_)
#define _BACKGRAUND_

#include <windows.h>
#include "rgb.hpp"
#include "indicat.hpp"

#undef _EXPORT2
#if defined(_BKGNDDLL) || defined(_ALLDLGDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif

#undef _EXPFUNC2
#if defined(_BKGNDDLL) || defined(_ALLDLGDLL)
  #define _EXPFUNC2 _export
#else
  #define _EXPFUNC2 _EXPFUNC
#endif



_CLASSDEF(TBackGraund)
_CLASSDEF(TBackGraundFlat)
_CLASSDEF(TBackGraundLinear)
_CLASSDEF(TBackGraundRectangular)
_CLASSDEF(TBackGraundCircular)

enum TBKTypes { BK_Base = 1, BK_Flat, BK_Linear, BK_Rectangular,
		BK_Circular };

PTBackGraund  _EXPFUNC2 CreateBkForClass( classType cl );
PTBackGraund  _EXPFUNC2 CreateCopy( const RTBackGraund rObj );


class _EXPORT2 TBackGraund
 {
public:
  TBackGraund();
  TBackGraund( int iPalS, RGBQUAD& rgbBkSta_, RGBQUAD& rgbBkEnd_,
	       BOOL bFlSmoth_ = FALSE );
  TBackGraund( const RTBackGraund rObj );

  virtual void Init( TRGB  huge* pInitZone, PTColorTable pTbl, int iPalSize,
		     int iWid, int iHigh, PTIndicator pInd ) = 0;
  virtual classType isA() const { return BK_Base; }

  virtual int Save( HFILE hf );
  virtual int Load( HFILE hf );

  virtual BOOL SupportSmoth() const { return FALSE; }

  RGBQUAD& RGBSta()  { return rgbBkSta; }
  RGBQUAD& RGBEnd()  { return rgbBkEnd; }
  void RGBSta( const RGBQUAD& RGB ) { rgbBkSta = RGB; }
  void RGBEnd( const RGBQUAD& RGB ) { rgbBkEnd = RGB; }

  virtual int operator==( RTBackGraund rObj );

  BOOL Track() const { return bFlAutoTrackPreview; }
  void Track( BOOL bTr ) { bFlAutoTrackPreview = bTr; }

protected:
  int iPaletteSize;
  RGBQUAD rgbBkSta, rgbBkEnd;
  BOOL bFlSmoth;
  BOOL bFlAutoTrackPreview;

  BOOL Isopoint( int x1, int y1, TRGB rgb, TRGB  huge* pBuf,
    int iWid, int iHigh, int iIndexC );
  void IsolineH( int xSta, int ySta, int xEnd, TRGB rgb, TRGB  huge* pBuf,
    int iWid, int iHigh, int iIndexC );
  void IsolineV( int xSta, int ySta, int yEnd, TRGB rgb, TRGB  huge* pBuf,
    int iWid, int iHigh, int iIndexC );

  double Distance( int x1, int y1, int x2, int y2 );
 };

class _EXPORT2 TBackGraundFlat: public TBackGraund
 {
public:
  TBackGraundFlat(): TBackGraund()
   {
     rgbBkSta.rgbRed =   0;
     rgbBkSta.rgbGreen = 128;
     rgbBkSta.rgbBlue =  128;
     bFlAutoTrackPreview = TRUE;
   };

  TBackGraundFlat( int iPalS, RGBQUAD& rgbBkSta_, RGBQUAD& rgbBkEnd_,
		   BOOL bFlSmoth_ = FALSE ):
    TBackGraund( iPalS, rgbBkSta_, rgbBkEnd_, bFlSmoth_ )
   {
     bFlAutoTrackPreview = TRUE;
   }
  TBackGraundFlat( const RTBackGraundFlat rObj ):
    TBackGraund( rObj )
   {
   }

  virtual void Init( TRGB  huge* pInitZone, PTColorTable pTbl, int iPalSize,
		     int iWid, int iHigh, PTIndicator pInd );
  virtual classType isA() const { return BK_Flat; }

 };

class _EXPORT2 TBackGraundLinear: public TBackGraund
 {
public:
  TBackGraundLinear(): TBackGraund() { bAngleGradient = 0; }
  TBackGraundLinear( int iPalS, RGBQUAD& rgbBkSta_, RGBQUAD& rgbBkEnd_,
		     BYTE bAngleGradient_,
		     BOOL bFlSmoth_ = FALSE ):
    TBackGraund( iPalS, rgbBkSta_, rgbBkEnd_, bFlSmoth_ )
   {
     bAngleGradient = bAngleGradient_;
   }

  TBackGraundLinear( const RTBackGraundLinear rObj ):
    TBackGraund( rObj )
   {
     bAngleGradient = rObj.bAngleGradient;
   }


  virtual void Init( TRGB  huge* pInitZone, PTColorTable pTbl, int iPalSize,
		     int iWid, int iHigh, PTIndicator pInd );
  virtual classType isA() const { return BK_Linear; }

  virtual int Save( HFILE hf );
  virtual int Load( HFILE hf );

  virtual BOOL SupportSmoth() const { return TRUE; }

  BYTE& Angle() { return bAngleGradient; }
  void Angle( BYTE bAngl ) { bAngleGradient = bAngl; }

  virtual int operator==( RTBackGraund rObj );

private:
  BYTE bAngleGradient;

  void Isoline( int x, int y, int xop, int yop, TRGB rgb, TRGB  huge* pBuf,
		int iWid, int iHigh, int iIndexC );
 };

class _EXPORT2 TBackGraundRectangular: public TBackGraund
 {
public:
  TBackGraundRectangular(): TBackGraund() { iXCenter = iYCenter = 50; }
  TBackGraundRectangular( int iPalS, RGBQUAD& rgbBkSta_, RGBQUAD& rgbBkEnd_,
		          int iXCenter_, int iYCenter_,
		          BOOL bFlSmoth_ = FALSE ):
    TBackGraund( iPalS, rgbBkSta_, rgbBkEnd_, bFlSmoth_ )
   {
     iXCenter = iXCenter_;
     iYCenter = iYCenter_;
   }

  TBackGraundRectangular( const RTBackGraundRectangular rObj ):
    TBackGraund( rObj )
   {
     iXCenter = rObj.iXCenter;
     iYCenter = rObj.iYCenter;
   }

  virtual void Init( TRGB  huge* pInitZone, PTColorTable pTbl, int iPalSize,
		     int iWid, int iHigh, PTIndicator pInd );
  virtual classType isA() const { return BK_Rectangular; }

  virtual int Save( HFILE hf );
  virtual int Load( HFILE hf );

  int& XCenter() { return iXCenter; }
  int& YCenter() { return iYCenter; }
  void XCenter( int iXC )  { iXCenter = iXC; }
  void YCenter( int iYC )  { iYCenter = iYC; }

  virtual int operator==( RTBackGraund rObj );

protected:
  int iXCenter, iYCenter;
 };

class _EXPORT2 TBackGraundCircular: public TBackGraundRectangular
 {
public:
  TBackGraundCircular() {};
  TBackGraundCircular( int iPalS, RGBQUAD& rgbBkSta_, RGBQUAD& rgbBkEnd_,
		       int iXCenter_, int iYCenter_,
		       BOOL bFlSmoth_ = FALSE ):
    TBackGraundRectangular( iPalS, rgbBkSta_, rgbBkEnd_, iXCenter_, iYCenter_, bFlSmoth_ )
   {    
   }

  TBackGraundCircular( const RTBackGraundCircular rObj ):
    TBackGraundRectangular( rObj )
   {
   }

  virtual void Init( TRGB  huge* pInitZone, PTColorTable pTbl, int iPalSize,
		     int iWid, int iHigh, PTIndicator pInd );
  virtual classType isA() const { return BK_Circular; }

  virtual BOOL SupportSmoth() const { return TRUE; }
 };


#endif
