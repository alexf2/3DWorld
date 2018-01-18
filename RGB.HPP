#if !defined(_RGB_)
#define _RGB_

#include "indicat.hpp"

#undef _EXPORT2
#if defined(_RGBDLL) || defined(_ALLCTLDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif

#undef _EXPFUNC2
#if defined(_RGBDLL) || defined(_ALLDLGDLL)
  #define _EXPFUNC2 _export
#else
  #define _EXPFUNC2 _EXPFUNC
#endif

enum TConvertAlho { CA_MedianCutGeom = 1, CA_MedianCutDiscr = 2, CA_Quantize = 4,
		    CA_Diffuzion = 8, CA_System = 16
		  };

enum TNoisy { N_Marginal = 0, N_Uniform = 1, N_None = 2 };

struct TNoisyDta
 {
   TNoisy nNoisy;
   int iMarginalAmplitude,
       iUniformAmplitude,
       iUniformThreshold;
 };
typedef TNoisyDta* PTNoisyDta;
typedef TNoisyDta& RTNoisyDta;

struct _EXPORT2 TIRGB;
typedef TIRGB* PTIRGB;
typedef TIRGB& RTIRGB;
//_PTRDEF(TIRGB)
//_REFDEF(TIRGB)

struct _EXPORT2 TRGB;
typedef TRGB huge* PTRGB;
typedef TRGB huge& RTRGB;
//_PTRDEF(TRGB)
//_REFDEF(TRGB)


inline BYTE BoundsFilterRGB( int iVal )
 {
   return (iVal < 0) ? 0:( (iVal > 255) ? 255:iVal );
 }


struct _EXPORT2 TRGB
 {
   BYTE huge r, huge g, huge b;
   BYTE huge index;

   TRGB()
    {
      index = 0;
    }

   TRGB( RGBQUAD& rgbQ )
    {
      r = rgbQ.rgbRed;
      g = rgbQ.rgbGreen;
      b = rgbQ.rgbBlue;
      index = 0;
    }

   TRGB( double r_, double g_, double b_ )
    {  
      r = r_;
      g = g_;
      b = b_;
      index = 0;
    }

   TRGB( int r_, int g_, int b_ )
    {  
      r = r_;
      g = g_;
      b = b_;
      index = 0;
    }


   TRGB( TRGB huge& rgb )
    {
      r = rgb.r;
      g = rgb.g;
      b = rgb.b;
      index = rgb.index; 
    }

   TRGB huge& operator=( TRGB huge& rgb )
    {
      r = rgb.r;
      g = rgb.g;
      b = rgb.b;
      index = rgb.index;  

      return *(TRGB huge*)this;
    }

   TRGB huge& operator=( RTIRGB rgb );

 };


inline void operator<<( RGBQUAD& rgbQ, const TRGB huge& rgb )
 {
   rgbQ.rgbRed =   rgb.r;
   rgbQ.rgbGreen = rgb.g;
   rgbQ.rgbBlue =  rgb.b;
 }



struct _EXPORT2 TIRGB
 {
   int r, g, b;
   BYTE index;

   TIRGB() { index = 0; };
   TIRGB( RGBQUAD& rgbQ )
    {
      r = rgbQ.rgbRed;
      g = rgbQ.rgbGreen;
      b = rgbQ.rgbBlue;
      index = 0;
    }
   TIRGB( double r_, double g_, double b_ )
    {
      r = r_;
      g = g_;
      b = b_;
      index = 0;
    }
   TIRGB( int r_, int g_, int b_ )
    {
      r = r_;
      g = g_;
      b = b_;
      index = 0;
    }


   RTIRGB  operator=( TRGB huge& rgb )
    {   
      r = rgb.r; g = rgb.g; b = rgb.b;
      index = rgb.index;

      return *this;
    }

 };


inline TRGB huge& TRGB::operator=( RTIRGB rgb )
 {   
   r = BoundsFilterRGB( rgb.r );
   g = BoundsFilterRGB( rgb.g );
   b = BoundsFilterRGB( rgb.b );
   index = rgb.index;

   return *(TRGB huge*)this;
 }



typedef RGBQUAD TColorTable;
typedef TColorTable FAR *PTColorTable;

PTColorTable _EXPFUNC2 TrueColorTo256( TRGB  huge* pBuf, DWORD dwSize, int iTotalIndex, BOOL bFlTrueColor,
				       TConvertAlho caAlho, int iWidth, int iHigh,
				       PTNoisyDta pNoise,
				       PTIndicator pInd );

BOOL _EXPFUNC2 MediumSection( PTColorTable pTbl, TRGB  huge* pBuf, DWORD dwSize, int iPalSize,
			      TConvertAlho caAlho, int iWidth, int iHigh,
			      PTNoisyDta pNoise,
			      PTIndicator pInd );

BOOL _EXPFUNC2 MediumSectionF( PTColorTable pTbl, TRGB  huge* pBuf, DWORD dwSize, int iPalSize,
			       TConvertAlho caAlho, int iWidth, int iHigh,
			       PTNoisyDta pNoise,
			       PTIndicator pInd );


BOOL _EXPFUNC2 Quantization( PTColorTable pTbl, TRGB  huge* pBuf, DWORD dwSize, int iPalSize,
			     TConvertAlho caAlho, int iWidth, int iHigh,
			     PTNoisyDta pNoise,
			     PTIndicator pInd );

						     //in TRGB struct
BOOL _EXPFUNC2 Diffusion( PTColorTable pTbl, TRGB  huge* pBuf, DWORD dwSize, int iPalSize,
			  TConvertAlho caAlho, int iWidth, int iHigh,
			  PTNoisyDta pNoise,
			  PTIndicator pInd );


inline int operator==( TRGB huge& rgb1, TRGB huge& rgb2 )
 {
   return rgb1.r == rgb2.r &&
	  rgb1.g == rgb2.g &&
	  rgb1.b == rgb2.b;
 }

inline int operator==( TRGB huge& rgb, RGBQUAD& rgbQ )
 {  
   return rgb.r == rgbQ.rgbRed &&
	  rgb.g == rgbQ.rgbGreen &&
	  rgb.b == rgbQ.rgbBlue;
 }


inline int operator==( RGBQUAD& rgbQ, TRGB huge& rgb )
 {
   return rgb == rgbQ;
 }

inline int operator==( RGBQUAD& rgbQ1, RGBQUAD& rgbQ2 )
 {
   return rgbQ1.rgbRed == rgbQ2.rgbRed &&
	  rgbQ1.rgbGreen == rgbQ2.rgbGreen &&
	  rgbQ1.rgbBlue == rgbQ2.rgbBlue;
 }



#endif
