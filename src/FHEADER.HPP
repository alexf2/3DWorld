#if !defined(_3DWORLD_FILE_HEADER_)
#define _3DWORLD_FILE_HEADER_

#include <windows.h>
#include <object.h>

#include "windiv.hpp"

const int iLenVersion = 12;

struct TData;
/*struct TDataChild;
struct TDataMDISave;
struct TDataMedium;*/

typedef TData _FAR *PTData;
typedef TData _FAR &RTData;

struct TData {
     char cVersion[ iLenVersion ];     

     static PTData ZERO;
     static char c3DWVersion[ iLenVersion ];

     TData() { lstrcpy( cVersion, c3DWVersion ); }
     TData( LPCSTR lpc ) { lstrcpy( cVersion, lpc ); }
   };

inline int operator==( const RTData rD1, const RTData rD2 )
 {
   return (void huge *)&rD1 == (void huge *)&rD2;
 }

inline int operator!=( const RTData rD1, const RTData rD2 )
 {
   return !(rD1 == rD2);
 }

#define NODATA (*(TData::ZERO))


class TDataInit
 {
public:
  RTData d_;

  TDataInit( PTData pD ):
    d_( pD ? *pD:NODATA )
   {
   };

  virtual ~TDataInit();
 };

struct TFileHeader: virtual public TDataInit
 {
public:

   TFileHeader(): TDataInit( new TData() ), ulCRChead( 0 ) {};

   virtual int Save( HFILE ) const;
   virtual int Read( HFILE );

   virtual BOOL CheckCRC() const;

   virtual int GetDataSIze() const; 
   virtual void InitCRC();

private:
  unsigned long ulCRChead;

  unsigned long CalcHeadCRC() const;
 };

struct TDataMedium: public TData
 {
   int iNumbMediums;

   TDataMedium(): TData() {};
 };
typedef TDataMedium _FAR &RTDataMedium;

struct TMediumHeader: public TFileHeader
 {
public:
  RTDataMedium d;

  TMediumHeader( int iNumMed ):
    TDataInit( new TDataMedium() ),
    TFileHeader(),
    d( RTDataMedium(d_) )
   {
     d.iNumbMediums = iNumMed;
   }

  TMediumHeader():
    TDataInit( new TDataMedium() ),
    TFileHeader(),
    d( RTDataMedium(d_) )
   {
   };

  virtual int GetDataSIze() const;
 };


struct TDataMDISave: public TData
 {
   UINT   CCopy;
   TDivStyle DsDivScreen;
   BOOL   bFlWndGrowAsParent;
   int    iNumWnds;

   TDataMDISave(): TData() {};
 };
typedef TDataMDISave _FAR &RTDataMDISave;

struct TMDISave: public TFileHeader
 {
public:
   RTDataMDISave d;

   TMDISave( UINT uiCopy, TDivStyle dsDiv, BOOL bFlGrow, int iNumW ):
     TDataInit( new TDataMDISave() ),
     TFileHeader(),
     d( RTDataMDISave(d_) )
    {
      d.CCopy = uiCopy; d.DsDivScreen = dsDiv; d.bFlWndGrowAsParent = bFlGrow;
      d.iNumWnds = iNumW;
    }

   TMDISave():
     TDataInit( new TDataMDISave() ),
     TFileHeader(),
     d( RTDataMDISave(d_) )
    {
    };

   virtual int GetDataSIze() const;
 };

struct TDataChild: public TData
 {
   RECT bounds;
   int spcX, spcY;
   BOOL bFlVisyalGreed;
   double scale;
   double shiftX, shiftY;

   double h, v;
   TViewPoint vpViewPoint;
   BOOL  bFlVisyalAxis;
  
   WINDOWPLACEMENT  place;

   int iDivIndex;
   BOOL bAutoFit;

   BOOL bFlPopupAllViews;  

   T3DPoint globalBasePoint;
   BOOL  bFlIsBasePointVisible;

   TDataChild(): TData() {};
 };
typedef TDataChild _FAR &RTDataChild;

struct TMDIChHeader: public TFileHeader
 {
public:
  RTDataChild d;

  TMDIChHeader( RECT _FAR &rBounds,
		int iSpcX, int iSpcY,
		BOOL bFlVG,
		double dScale,
		double dShX, double dShY,
		double dH, double dV,
		TViewPoint rVP,
		BOOL bFlVA,
		WINDOWPLACEMENT _FAR rPlace,
		int iDivI,
		BOOL bAbFit,
		BOOL bFlPopup,
		RT3DPoint bp,
		BOOL bFlBPV
	      );

  TMDIChHeader():
    TDataInit( new TDataChild() ),
    TFileHeader(),
    d( RTDataChild(d_) )
   {
   };

  virtual int GetDataSIze() const;
 };

struct TDataScena: public TData
 {
   int   iNumSurf;
   BOOL  bFlShowBackFaces, bFlFullDetal;
   BOOL  bFlVisyal;

   RGBQUAD  rgbAdges;
   RGBQUAD  rgbIntersect;
   RGBQUAD  rgbAmbient;
   double  dAmbMult;
   int  polyMethod;
   BOOL bFlMarkIntersect;

   int iPaletteSize;
   int ifFormat;
   int caAlhoPalette;

   BOOL bFlAntiAliasing, bFlShadows, bFlRenderBack_Reserv;

   int iNumbPointsAntiAlias;
   double dStrengtsAntialias;

   int saShadingLimit;
   int tmTraceModel;

   double dAngleSmothGrad;

   int nNoisy, 
       iMarginalAmplitude, 
       iUniformAmplitude,
       iUniformThreshold;

   BOOL bFlStrictGamma;

   TDataScena(): TData() {};
 };
typedef TDataScena _FAR &RTDataScena;

struct TScenaHdr: public TFileHeader
 {
public:
   RTDataScena d;

   TScenaHdr( int iNumSf, BOOL bFlSBF, BOOL bFlFD, BOOL bFlV,
		 RGBQUAD&  rRgbAdges,
		 RGBQUAD&  rRgbIntersect,
		 RGBQUAD&  rRgbAmbient,
		 double dAmbMult,
                 int  polyMethod,
		 BOOL bFlMarkIntersect,
		 int iPaletteSize,
		 int ifFormat,
		 int caAlhoPalette,

		 BOOL bFlAntiAliasing,
		 BOOL bFlShadows,
		 

		 int iNumbPointsAntiAlias,
                 double dStrengtsAntialias,

                 int saShadingLimit,
		 int tmTraceModel,

		 double dAngleSmothGrad,

		 int nNoisy,
                 int iMarginalAmplitude, 
                 int iUniformAmplitude,
		 int iUniformThreshold,

		 BOOL bFlStrictGamma
             ):

     TDataInit( new TDataScena() ),
     TFileHeader(),
     d( RTDataScena(d_) )
    {
      d.iNumSurf = iNumSf;
      d.bFlShowBackFaces = bFlSBF;
      d.bFlFullDetal = bFlFD;
      d.bFlVisyal = bFlV;  
  
      d.rgbAdges = rRgbAdges;
      d.rgbIntersect = rRgbIntersect;
      d.rgbAmbient = rRgbAmbient;
      d.dAmbMult = dAmbMult;
      d.polyMethod = polyMethod;
      d.bFlMarkIntersect = bFlMarkIntersect;

      d.iPaletteSize = iPaletteSize;
      d.ifFormat = ifFormat;
      d.caAlhoPalette = caAlhoPalette;

      d.bFlAntiAliasing = bFlAntiAliasing;
      d.bFlShadows = bFlShadows;


      d.iNumbPointsAntiAlias = iNumbPointsAntiAlias;
      d.dStrengtsAntialias = dStrengtsAntialias;

      d.saShadingLimit = saShadingLimit;
      d.tmTraceModel = tmTraceModel;

      d.dAngleSmothGrad = dAngleSmothGrad;

      d.nNoisy = nNoisy;
      d.iMarginalAmplitude = iMarginalAmplitude;
      d.iUniformAmplitude = iUniformAmplitude;
      d.iUniformThreshold = iUniformThreshold;

      d.bFlStrictGamma = bFlStrictGamma;

      d.bFlRenderBack_Reserv = 0;
    }

   TScenaHdr():
     TDataInit( new TDataScena() ),
     TFileHeader(),
     d( RTDataScena(d_) )
    {
    };

   virtual int GetDataSIze() const;
 };


struct TDataSurf: public TData
 {   
   int iNumVertex, iNumAdges, iNumFacets;
   double minX, maxX, minY, maxY, minZ, maxZ;
   BOOL bFlCreateObol;
   BOOL bFlSelect;

   TDataSurf(): TData() {};
 };
typedef TDataSurf _FAR &RTDataSurf;

struct TSurfHdr: public TFileHeader
 {
public:
   RTDataSurf d;

   TSurfHdr( int iNumVert, int iNumAdg, int iNumFac,
	     double dMinX, double dMaxX, double dMinY, double dMaxY,
	     double dMinZ, double dMaxZ,
	     BOOL bFlCreObl,
             BOOL bFlSel ):
     TDataInit( new TDataSurf() ),
     TFileHeader(),
     d( RTDataSurf(d_) )
    {
      d.iNumVertex = iNumVert;
      d.iNumAdges = iNumAdg;
      d.iNumFacets = iNumFac;
      d.minX = dMinX;
      d.maxX = dMaxX;
      d.minY = dMinY;
      d.maxY = dMaxY; 
      d.minZ = dMinZ;
      d.maxZ = dMaxZ;
      d.bFlCreateObol = bFlCreObl;
      d.bFlSelect = bFlSel;
    }

   TSurfHdr():
     TDataInit( new TDataSurf() ),
     TFileHeader(),
     d( RTDataSurf(d_) )
    {
    };

   virtual int GetDataSIze() const;
 };




#endif
