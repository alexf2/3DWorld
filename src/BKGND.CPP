#include "bkgnd.hpp"
#include "autorc.hpp"
#include "ucs.hpp"
#include <math.h>

#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;


inline double max( double m1, double m2 )
 {
   return (m1 > m2) ? m1:m2;
 } 

static int near pascal AddColor( PTColorTable pTbl_, RTRGB rRGB,
				  int iPalSize, int& iCount
			        )
 {
   if( !pTbl_ || !iPalSize || iCount >= iPalSize ) return 0;


   PTColorTable pTbl = pTbl_;
   for( int i = 0; i < iCount; i++, pTbl++ )
     if( *pTbl == rRGB ) return i;     

   int iKey = iCount;
   if( iCount < iPalSize ) pTbl_[ iCount++ ] << rRGB;
   return iKey;
 }

PTBackGraund  _EXPFUNC2 CreateBkForClass( classType cl )
 {
   PTBackGraund pBkTmp = NULL;

   switch( cl )
    {
      case BK_Flat:
        pBkTmp = new TBackGraundFlat();
	break;

      case BK_Linear:
        pBkTmp = new TBackGraundLinear();
	break;

      case BK_Rectangular:
        pBkTmp = new TBackGraundRectangular();
	break;

      case BK_Circular:
        pBkTmp = new TBackGraundCircular();
	break;
    }

  return pBkTmp;
 }


PTBackGraund  _EXPFUNC2 CreateCopy( const RTBackGraund rObj )
 {
   PTBackGraund pTmp = NULL;

   switch( rObj.isA() )
    {
      case BK_Flat:
	pTmp = new TBackGraundFlat( (RTBackGraundFlat)rObj );
	break;

      case BK_Linear:
	pTmp = new TBackGraundLinear( (RTBackGraundLinear)rObj );
	break;

      case BK_Rectangular:
	pTmp = new TBackGraundRectangular( (RTBackGraundRectangular)rObj );
	break;

      case BK_Circular:
	pTmp = new TBackGraundCircular( (RTBackGraundCircular)rObj );
	break;
    }

   return pTmp;
 }

int TBackGraund::operator==( RTBackGraund rObj )
 {
   return iPaletteSize == rObj.iPaletteSize &&
	  rgbBkSta == rObj.rgbBkSta &&
	  rgbBkEnd == rObj.rgbBkEnd &&
	  bFlSmoth == rObj.bFlSmoth &&
	  bFlAutoTrackPreview == rObj.bFlAutoTrackPreview;
 }

TBackGraund::TBackGraund( int iPalS, RGBQUAD& rgbBkSta_, RGBQUAD& rgbBkEnd_,
			  BOOL bFlSmoth_ )
 {
   iPaletteSize = iPalS;
   rgbBkSta = rgbBkSta_;
   rgbBkEnd = rgbBkEnd_;
   bFlSmoth = bFlSmoth_;
   bFlAutoTrackPreview = FALSE;
 }

TBackGraund::TBackGraund( const RTBackGraund rObj )
 {
   iPaletteSize = rObj.iPaletteSize;
   rgbBkSta = rObj.rgbBkSta;
   rgbBkEnd = rObj.rgbBkEnd;
   bFlSmoth = rObj.bFlSmoth;
   bFlAutoTrackPreview = rObj.bFlAutoTrackPreview;
 }

TBackGraund::TBackGraund()
 {
   iPaletteSize = 236;
   rgbBkSta.rgbRed =   128;
   rgbBkSta.rgbGreen = 0;
   rgbBkSta.rgbBlue =  10;

   rgbBkEnd.rgbRed =   10;
   rgbBkEnd.rgbGreen = 0;
   rgbBkEnd.rgbBlue =  200;

   bFlSmoth = FALSE;
   bFlAutoTrackPreview = FALSE;
 }

struct TBkGndDta
 { 
   int iPaletteSize;
   RGBQUAD rgbBkSta, rgbBkEnd;
   BOOL bFlSmoth;
   BOOL bFlAutoTrackPreview;
 };

int TBackGraund::Save( HFILE hf )
 {
   TBkGndDta dta;
   dta.iPaletteSize = iPaletteSize;
   dta.rgbBkSta = rgbBkSta;
   dta.rgbBkEnd = rgbBkEnd;
   dta.bFlAutoTrackPreview = bFlAutoTrackPreview;

   if( _lwrite(hf, &dta, sizeof(dta)) == HFILE_ERROR ) return HFILE_ERROR;

   return 1;
 }

int TBackGraund::Load( HFILE hf )
 {
   TBkGndDta dta;
   UINT l;
   if( (l = _lread(hf, &dta, sizeof(dta))) == HFILE_ERROR ||
	l != sizeof(dta) ) return HFILE_ERROR;

   iPaletteSize = dta.iPaletteSize;
   rgbBkSta = dta.rgbBkSta;
   rgbBkEnd = dta.rgbBkEnd;
   bFlAutoTrackPreview = dta.bFlAutoTrackPreview;

   return 1;
 }

int TBackGraundLinear::Save( HFILE hf )
 {
   if( TBackGraund::Save(hf) == HFILE_ERROR ) return HFILE_ERROR;
   if( _lwrite(hf, &bAngleGradient, sizeof(BYTE)) == HFILE_ERROR )
     return HFILE_ERROR;

   return 1;
 }

int TBackGraundLinear::Load( HFILE hf )
 {
   if( TBackGraund::Load(hf) == HFILE_ERROR ) return HFILE_ERROR;

   UINT l;
   if( (l = _lread(hf, &bAngleGradient, sizeof(BYTE))) == HFILE_ERROR ||
	l != sizeof(BYTE)
     )
     return HFILE_ERROR;

   return 1;
 }
//iXCenter, iYCenter;
int TBackGraundRectangular::Save( HFILE hf )
 {
   if( TBackGraund::Save(hf) == HFILE_ERROR ) return HFILE_ERROR;
   if( _lwrite(hf, &iXCenter, sizeof(int)) == HFILE_ERROR )
     return HFILE_ERROR;
   if( _lwrite(hf, &iYCenter, sizeof(int)) == HFILE_ERROR )
     return HFILE_ERROR;

   return 1;
 }

int TBackGraundRectangular::Load( HFILE hf )
 {
   if( TBackGraund::Load(hf) == HFILE_ERROR ) return HFILE_ERROR;

   UINT l;
   if( (l = _lread(hf, &iXCenter, sizeof(int))) == HFILE_ERROR ||
	l != sizeof(int)
     )
     return HFILE_ERROR;
   if( (l = _lread(hf, &iYCenter, sizeof(int))) == HFILE_ERROR ||
	l != sizeof(int)
     )
     return HFILE_ERROR;

   return 1;
 }


double TBackGraund::Distance( int x1, int y1, int x2, int y2 )
 {
   return sqrt( DWORD(x1 - x2) * DWORD(x1 - x2) +
     DWORD(y1 - y2) * DWORD(y1 - y2) );
 }

BOOL TBackGraund::Isopoint( int x1, int y1, TRGB rgb, TRGB  huge* pBuf,
				int iWid, int iHigh, int iIndexC )
 {
   if( x1 > -1 && x1 < iWid && y1 > -1 && y1 < iHigh )
    {
      DWORD dwI;
      pBuf[ (dwI = (DWORD)y1 * (DWORD)iWid + (DWORD)x1) ] = rgb;
      pBuf[ dwI ].index = iIndexC;

      return TRUE;
    }

   return FALSE;
 }

void TBackGraund::IsolineH( int xSta, int ySta, int xEnd, TRGB rgb, TRGB  huge* pBuf,
	       int iWid, int iHigh, int iIndexC )
 {
   if( xSta >= iWid || xEnd <= 0 || ySta < 0 || ySta >= iHigh ) return;
   if( xSta < 0 ) xSta = 0;
   if( xEnd >= iWid ) xEnd = iWid - 1;

   TRGB  huge* pBuf_ = pBuf + (DWORD)ySta * (DWORD)iWid + (DWORD)xSta;
   for( int i = xSta; i <= xEnd; i++, pBuf_++ )
    {
      /*DWORD dwI;
      pBuf[ (dwI = (DWORD)ySta * (DWORD)iWid + (DWORD)i) ] = rgb;
      pBuf[ dwI ].index = iIndexC;*/
      *pBuf_ = rgb;
      pBuf_->index = iIndexC;
    }
 }

void TBackGraund::IsolineV( int xSta, int ySta, int yEnd, TRGB rgb, TRGB  huge* pBuf,
	       int iWid, int iHigh, int iIndexC )
 {
   if( xSta >= iWid || xSta < 0 || ySta >= iHigh || yEnd <= 0 ) return;
   if( ySta < 0 ) ySta = 0;
   if( yEnd >= iHigh ) yEnd = iHigh - 1;

   for( int i = ySta; i <= yEnd; i++ )
    {
      DWORD dwI;
      pBuf[ (dwI = (DWORD)i * (DWORD)iWid + (DWORD)xSta) ] = rgb;
      pBuf[ dwI ].index = iIndexC;
    }
 }


void TBackGraundFlat::Init( TRGB  huge* pInitZone, PTColorTable pTbl, int iPalSize,
			    int iWid, int iHigh, PTIndicator pInd )
 {
   DWORD iEnd = (DWORD)iWid * (DWORD)iHigh;
   if( pInd ) pInd->Init( iEnd );

   TRGB rgbTmp( rgbBkSta );
   if( pTbl ) pTbl[0] << rgbTmp;

   TAutoCursorRc crcCursor( LoadCursor(NULL, IDC_WAIT) );

   for( DWORD i = 0; i < iEnd; i++, pInitZone++ )
    {
      *pInitZone = rgbTmp;
      pInitZone->index = 0;
      if( pInd && pInd->WorkMessage( i ) == TRUE ) return;
    }
 }

static double near pascal GetM( double dAx, double dAy, double dAz,
				double dBx, double dBy, double dBz,
				double dCx, double dCy, double dCz,
                                double x, double y
			      )
 {
   return
     (x-dAx) * ((dBy-dAy) * (dCz-dAz) - (dCy-dAy) * (dBz-dAz)) -
     (y-dAy) * ((dBx-dAx) * (dCz-dAz) - (dCx-dAx) * (dBz-dAz));
 }

int TBackGraundLinear::operator==( RTBackGraund rObj )
 {
   return TBackGraund::operator==( rObj ) &&
	  bAngleGradient == RTBackGraundLinear(rObj).bAngleGradient;
 }

void TBackGraundLinear::Init( TRGB  huge* pInitZone, PTColorTable pTbl, int iPalSize,
			      int iWid, int iHigh, PTIndicator pInd )
 {
   //enum { AN_0, AN_90, AN_180, AN_0_45, AN_45_90, AN_90_135, AN_135_180 };


				  //dAngleGradient = 135;
   if( bAngleGradient < 0 || bAngleGradient > 180 ) return;

   TAutoCursorRc crcCursor( LoadCursor(NULL, IDC_WAIT) );

   int x2 = iWid - 1, y2 = iHigh - 1;
   double dAx, dAy, dAz,
          dBx, dBy, dBz,
	  dCx, dCy, dCz;

   if( bAngleGradient <= 90 )
    {
      dAx = dAy = 0;
      dBx = x2; dBy = y2;          
      dCy = (double)x2 * sin( GRAD_TO_RAD(-bAngleGradient) );
    }
   else
    {
      dAx = 0; dAy = y2;
      dBx = x2; dBy = 0;      
      dCy = (double)x2 * sin( GRAD_TO_RAD(-bAngleGradient) ) + y2;
    }
   dCx = (double)x2 * cos( GRAD_TO_RAD(-bAngleGradient) );
   

   double dK = (dBx - dAx) * (dCy - dAy) - (dCx - dAx) * (dBy - dAy);

   DWORD dwIterSize = (DWORD)iWid * (DWORD)iHigh;
   DWORD dwIterMak = 0;
   if( pInd ) pInd->Init( dwIterSize );

   int iColorCount = 0;
   TRGB huge* pB = pInitZone;
   for( int iStr = 0; iStr < iHigh; iStr++ )
     for( int iClm = 0; iClm < iWid; iClm++, pB++ )
      {
	double dZR, dZG, dZB;

	dCz = dAz = rgbBkEnd.rgbRed; dBz = rgbBkSta.rgbRed;
	dZR = dAz - GetM( dAx, dAy, dAz,
			  dBx, dBy, dBz,
			  dCx, dCy, dCz,
			  iClm, iStr
			) / dK;

	dCz = dAz = rgbBkEnd.rgbGreen; dBz = rgbBkSta.rgbGreen;
	dZG = dAz - GetM( dAx, dAy, dAz,
			  dBx, dBy, dBz,
			  dCx, dCy, dCz,
			  iClm, iStr
			) / dK;

	dCz = dAz = rgbBkEnd.rgbBlue; dBz = rgbBkSta.rgbBlue;
	dZB = dAz - GetM( dAx, dAy, dAz,
			  dBx, dBy, dBz,
			  dCx, dCy, dCz,
			  iClm, iStr
			) / dK;

	TRGB rgb = TRGB( dZR, dZG, dZB );
	rgb.index = AddColor( pTbl, rgb, iPalSize, iColorCount );
	*pB = rgb;	
	//pInitZone[ (DWORD)iStr * (DWORD)iWid + (DWORD)iClm ] = rgb;

	if( pInd && pInd->WorkMessage( ++dwIterMak ) == TRUE ) return;
      } 

 }

void TBackGraundLinear::Isoline( int x, int y, int xop, int yop, TRGB rgb, TRGB  huge* pBuf,
    int iWid, int iHigh, int iIndexC )
 {
/*   double b = double(y) + tan( GRAD_TO_RAD(dAngleGradient) ) * double(x);
   int xs1 = xop, ys1 = -tan( GRAD_TO_RAD(dAngleGradient) ) * double(xop) + b;
   int xs2 = (b - yop) / tan( GRAD_TO_RAD(dAngleGradient) ),
       ys2 = yop;*/

   int xs1 = x, ys1 = y, xs2 = xop, ys2 = yop;

   Isopoint( xs1, ys1, rgb, pBuf, iWid, iHigh, iIndexC );

   int iXIncr = (xs1 < xs2) ? 1:-1,
       iYIncr = (ys1 < ys2) ? 1:-1;
   int iDx = abs( xs1 - xs2 ), iDy = abs( ys1 - ys2 );

   if( iDx > iDy )
    {
      int iCumul = iDx / 2;
      for( int i = 1; i <= iDx; i++ )
       {
         xs1 += iXIncr;
	 iCumul += iDy;
	 if( iCumul >= iDx )
	  {
	    iCumul -= iDx;
            ys1 += iYIncr;
	  }	 
	 Isopoint( xs1, ys1, rgb, pBuf, iWid, iHigh, iIndexC );
       }	 
    }
   else
    {
      int iCumul = iDy / 2;
      for( int i = 1; i <= iDy; i++ )
       {
         ys1 += iYIncr;
	 iCumul += iDx;
	 if( iCumul >= iDy )
	  {
	    iCumul -= iDy;
            xs1 += iXIncr;
	  }	    
	 Isopoint( xs1, ys1, rgb, pBuf, iWid, iHigh, iIndexC );
       }
    }
 }

int TBackGraundRectangular::operator==( RTBackGraund rObj )
 {
   return TBackGraund::operator==( rObj ) &&
	  iXCenter == RTBackGraundRectangular(rObj).iXCenter &&
	  iYCenter == RTBackGraundRectangular(rObj).iYCenter;
 }

void TBackGraundRectangular::Init( TRGB  huge* pInitZone, PTColorTable pTbl, int iPalSize,
			    int iWid, int iHigh, PTIndicator pInd )
 {
   TAutoCursorRc crcCursor( LoadCursor(NULL, IDC_WAIT) );

   int iXCenter_ = double(iWid) * double(iXCenter) / 100.0,
       iYCenter_ = double(iHigh) * double(iYCenter) / 100.0;

   int left = iXCenter_ - 1, right = iXCenter_ + 1,
       top = iYCenter_ - 1, bottom = iYCenter_ + 1;

   double dIterSize;
   int w = max( iXCenter_, iWid - iXCenter_ + 1 ),
       h = max( iYCenter_, iHigh- iYCenter_ + 1 );
   dIterSize = max(w, h);
   if( pInd ) pInd->Init( dIterSize );
   DWORD dwIterMak = 0;

   double rgb_r = rgbBkSta.rgbRed,
	  rgb_g = rgbBkSta.rgbGreen,
	  rgb_b = rgbBkSta.rgbBlue;
   double dRStep = double(rgbBkEnd.rgbRed - rgbBkSta.rgbRed) / dIterSize,
          dGStep = double(rgbBkEnd.rgbGreen - rgbBkSta.rgbGreen) / dIterSize,
	  dBStep = double(rgbBkEnd.rgbBlue - rgbBkSta.rgbBlue) / dIterSize;

   Isopoint( iXCenter_, iYCenter_, TRGB( rgb_r, rgb_g, rgb_b ), pInitZone, iWid, iHigh, 0 );
   int iColorCount = 0;
   while( left > 0 || right < iWid || top > 0 || bottom < iHigh )
    {
      TRGB rgb = TRGB( rgb_r, rgb_g, rgb_b );
      int iIndC = AddColor( pTbl, rgb, iPalSize, iColorCount );

      IsolineH( left, top, right, rgb, pInitZone, iWid, iHigh, iIndC );
      IsolineH( left, bottom, right, rgb, pInitZone, iWid, iHigh, iIndC );

      IsolineV( left, top, bottom, rgb, pInitZone, iWid, iHigh, iIndC );
      IsolineV( right, top, bottom, rgb, pInitZone, iWid, iHigh, iIndC );

      rgb_r += dRStep;
      rgb_g += dGStep;
      rgb_b += dBStep;

      left--; right++; top--; bottom++;

      if( pInd && pInd->WorkMessage( ++dwIterMak ) == TRUE ) return;
    }
 }

inline double ConeZ( double x, double y, double dCX, double dCY, double dTan )
 {
   return sqrt( (x - dCX) * (x - dCX) + (y - dCY) * (y - dCY) ) / dTan;
 }

void TBackGraundCircular::Init( TRGB  huge* pInitZone, PTColorTable pTbl, int iPalSize,
			    int iWid, int iHigh, PTIndicator pInd )
 {
   TAutoCursorRc crcCursor( LoadCursor(NULL, IDC_WAIT) );

   int iXCenter_ = double(iWid) * double(iXCenter) / 100.0,
       iYCenter_ = double(iHigh) * double(iYCenter) / 100.0;


   double d1 = Distance( 0, 0, iXCenter_, iYCenter_ ),
	  d2 = Distance( iXCenter_, iYCenter_, iWid - 1, 0 ),
	  d3 = Distance( iXCenter_, iYCenter_, 0, iHigh - 1 ),
	  d4 = Distance( iXCenter_, iYCenter_, iWid - 1, iHigh - 1 );
   int iXM, iYM;
   double dMax = max( d1, max(d2, max(d3, d4)) );
   if( dMax == d1 )
    {
      iXM = iYM = 0;
    }
   else if( dMax == d2 )
    {
      iXM = iWid - 1; iYM = 0;
    }
   else if( dMax == d3 )
    {
      iXM = 0; iYM = iHigh - 1;
    }
   else
    {
      iXM = iWid - 1; iYM = iHigh - 1;
    }


   DWORD dwIterSize = (DWORD)iWid * (DWORD)iHigh;
   DWORD dwIterMak = 0;
   if( pInd ) pInd->Init( dwIterSize );

   //double dRad = max( max(max(d1, d2), d3), d4 );
   double dTan = tan( GRAD_TO_RAD(30) );
   double dRad = ConeZ( iXM, iYM, iXCenter_, iYCenter_, dTan );
   double dSR = double(-int(rgbBkEnd.rgbRed) + int(rgbBkSta.rgbRed)) / dRad,
	  dSG = double(-int(rgbBkEnd.rgbGreen) + int(rgbBkSta.rgbGreen)) / dRad,
	  dSB = double(-int(rgbBkEnd.rgbBlue) + int(rgbBkSta.rgbBlue)) / dRad;
   //double dRad2 = dRad * dRad;

   int iColorCount = 0;
   TRGB huge* pB = pInitZone;
   for( int iStr = 0; iStr < iHigh; iStr++ )
     for( int iClm = 0; iClm < iWid; iClm++, pB++ )
      {
	/*double dZ = sqrt( dRad2 -
		    double(iClm - iXCenter) * double(iClm - iXCenter) -
		    double(iStr - iYCenter) * double(iStr - iYCenter) );*/
	double dZ = ConeZ( iClm, iStr, iXCenter_, iYCenter_, dTan );

	TRGB rgb = TRGB( int(dZ * dSR) + int(rgbBkEnd.rgbRed),
			 int(dZ * dSG) + int(rgbBkEnd.rgbGreen),
			 int(dZ * dSB) + int(rgbBkEnd.rgbBlue) );
	rgb.index = AddColor( pTbl, rgb, iPalSize, iColorCount );
	*pB = rgb;
	//pInitZone[ (DWORD)iStr * (DWORD)iWid + (DWORD)iClm ] = rgb;

	if( pInd && pInd->WorkMessage( ++dwIterMak ) == TRUE ) return;
      }

 }
