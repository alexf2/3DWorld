#if !defined(_SCENA_)
#define _SCENA_

#include <owl.h>
#include <list.h>
#include <object.h>
#include <bwindow.h>
#include <values.h>

#include "tracer.hpp"
#include "phtransf.hpp"
#include "indicat.hpp"
#include "fmaneger.hpp"
#include "surface.hpp"
#include "ucs.hpp"
#include "tracer.hpp"
#include "bkgnd.hpp"
#include "isdlg.hpp"
#include "log.hpp"
//#include "infdlg.hpp"
#include <dialog.h>


#define TYP_SCENE 101


_CLASSDEF(TScena)

#include "autorc.hpp"

typedef int near pascal (TScena::* PTFintIntersectFunc)(
  double& dZ, double clm,
  double str, PTFacet pFacet,
  DoubleList& rdblRemove,
  BOOL bFlRemove
 );


typedef void (TScena::* _FAR PTScenaMemFunc)( PTSurface, Pvoid );

typedef void  (TProectorFunc_)( PCTScena pSc, PT3DPoint pPoint );
typedef TProectorFunc_* TProectorFunc;

typedef BOOL (TClassFilterFunc)( PTSurface pObj );
typedef TClassFilterFunc* PTClassFilterFunc;

TClassFilterFunc  FilterAll;
TClassFilterFunc  FilterBody;
TClassFilterFunc  FilterCameras;
TClassFilterFunc  FilterLights;
TClassFilterFunc  FilterBodyAndLights;

//static void  ProectOrtho( PCTScena, PT3DPoint );
//static void  ProectPerspective( PCTScena, PT3DPoint );
static TProectorFunc_ ProectOrtho;
static TProectorFunc_ ProectPerspective;


struct TOPData
 {   
   POINT     _FAR *pPoint;
   PTUCS     pUcs;
   HDC       hdc;

   PTSurface pSfFound;
   PTFacet   pFacetFound;
   T3DPoint  ptFound;

   double    dZ;

   TOPData( POINT _FAR *pPoint_, PTUCS pUcs_, HDC hdc_ )
    {
      pFacetFound = NULL;
      pSfFound = NULL;
      pUcs = pUcs_;
      hdc = hdc_;
      pPoint = pPoint_;
      dZ = INT_MIN;
    }

 };
typedef TOPData* PTOPData;
typedef TOPData& RTOPData;

struct TDelta {
  double dX, dY, dZ;
  double minX, maxX, minY, maxY, minZ, maxZ;

  TDelta()
   {
     dX = dY = dZ = 0;
     minX = MAXINT; maxX = -MAXINT,
     minY = MAXINT; maxY = -MAXINT,
     minZ = MAXINT; maxZ = -MAXINT;
   }
 };
typedef TDelta _FAR *PTDelta;


struct TVertexData
 {
   PTRECT    pRect;
   POINT     _FAR *pPoint;
   PTSurface pSf;
   PTVertex  pVert;
   double    dDistance;
   PTUCS     pUcs;
   PTSurface  pCurSf;

   TVertexData( PTRECT pR, POINT _FAR *pP, PTUCS pU )
    {
      pRect = pR; pPoint = pP; pCurSf = pSf = NULL; pVert = NULL;
      dDistance = MAXINT;
      pUcs = pU;
    }
 };
typedef TVertexData _FAR *PTVertexData;


_CLASSDEF(TIntersectBuf)
class TIntersectBuf
 {
public:
  TIntersectBuf( int iInitSize = 50, int iDelta_ = 10 );
  //~TIntersectBuf();

  void Reset() { iCurSize = 0; }
  int near pascal AddIntersect( PTSurface  psfSurf, PTFacet    pFac, double     z );
  void near pascal Normalyze();

  void Start( int ind ) { iIter = ind; }
  PTTracData near pascal Next();
  PTTracData near pascal Prev();
  int near pascal BottomIndex() const;
  PTTracData near pascal Get( int ) const;

  void Direction( BOOL bFl ) { bFlDir = bFl; }
  int Status() { return !!intBuf || !!intBufP; }

private:
  int iSize, iDelta, iCurSize;
  int iIter;
  BOOL bFlDir;
  TAutoMemoryRc<PTTracData> intBuf;
  TAutoMemoryRc<PTTracData*> intBufP;

  void near pascal GrowBuf();
 };



class  TScena: public Object
 {

//friend void  ProectOrtho( PCTScena, PT3DPoint );
//friend void  ProectPerspective( PCTScena, PT3DPoint );

friend BOOL near pascal CreateStrScanData( TIndexedDoubleList& rdblPolygon, RTScena rSc, BOOL bFlBackFace, double dZZero, PTIndicator pInd );
friend TProectorFunc_ ProectOrtho;
friend TProectorFunc_ ProectPerspective;
friend void near pascal Trace( PTSurface& pNearstSurf, PTFacet& pNearstFac,
			       double& z,
			       PTScena pScenaRender,
			       double x_, double y_,
			       DoubleList& rdblRemove,
			       BOOL bFlRemove = TRUE
			     );

friend void near pascal TraceAll( PTSurface& pNearstSurf, PTFacet& pNearstFac,
			          double& z,
			          PTScena pScenaRender,
			          double x_, double y_,
			          DoubleList& rdblRemove,
				  BOOL bFlRemove,
				  RTIntersectBuf rintBuf
				);

friend TVector near pascal TraceWarnok(
		     RTIndexedDoubleList dblLights,
		     PTScena pScena, double x_, double y_, double dCellSize,
		     PTRGBKey prgbkeyStrPrev, int iKeyStrInd, BOOL bFlFirstStr,
		     BOOL& bFlObject,
		     TTracerPhoto& tr,
		     int clm, int str, int iLevel
		   );



public:
  int Status;

  TScena( int IdFile, PTApplication pApp );
  TScena( RTScena rScena, PTIndicator pInd, BOOL bFlOnlyFace = FALSE, BOOL bFlOnlySelected = FALSE );
  ~TScena();

  int Copy( RTIndexedDoubleList rD, RTIndexedDoubleList rS, PTIndicator pInd,
    BOOL bFlOnlyFace = FALSE, BOOL bFlOnlySelected = FALSE );

  virtual classType isA() const { return TYP_SCENE; }
  virtual hashValueType hashValue() const { return 0; }
  virtual int isEqual( const Object& ) const;
  virtual char *nameOf() const { return "Scena"; }
  virtual void printOn( ostream& ) const {};

  void CallProector( PT3DPoint pPt )
   {
     proector( this, pPt );
   }

  BOOL IsVisyal() const { return bFlVisyal; }
  BOOL SceneOnOff( BOOL bFlV ) { return (bFlVisyal = bFlV); }

  BOOL GetChangeFlag() { return bFlChanged; }
  void SetChangeFlag( BOOL bFl ) { bFlChanged = bFl; }
  int GetId() { return IdFile; }


  void SetDrawDetal( BOOL backFaces, BOOL fullDetal )
   {
     bFlShowBackFaces = backFaces;
     bFlFullDetal = fullDetal;
   }

  BOOL CanClose() { return bFlCanClose; }
  void Lock( BOOL bFlLock ) { bFlCanClose = (bFlLock == TRUE) ? FALSE:TRUE; }

  void SetHardwareEnviron( PTUCS pUcs, HDC hdc, PTWindow pWin ); //need call for every drawing

  void Draw( PTSurface pSf = NULL ) const; //uses current enviroment
  void DrawSpecial() const;
  //void DrawBody( RTSurface ) const;
  void DrawObol( RTSurface ) const;
  void DrawFacet( RTFacet, BOOL bFlDrawBack ) const;

  void AddBody( PTSurface pSurf );
  int GetFreeIndex() const { return listSurf.GetFreeIndex(); }

  BOOL GetDetal() const { return bFlFullDetal; }
  void SetDetal( BOOL bFlD ) { bFlFullDetal = bFlD; }
  BOOL GetHideGeom() const { return bFlShowBackFaces; }
  void SetHideGeom( BOOL bFlD ) { bFlShowBackFaces = bFlD; }

  void ZoomToFit( RECT *pRect = NULL ) const;

  void FillInfoDlg( List& rList, LPSTR lpName );

  PTSurface FindObject( double x, double y, PTClassFilterFunc pFiltr );
  void FindObjectAndPoint( int x, int y, RTOPData rDta );

  void AligmentZ( RTSurface rSurf, RTVector rV );
  void Move( RTSurface rSurf, double dx, double dy, double dz );
  void MoveAllSelected( double dx, double dy, double dz );
  void RotateZ( RTSurface rSurf, double angle );
  void RotateX( RTSurface rSurf, double angle );
  void RotateY( RTSurface rSurf, double angle );
  void RotateXYZ( RTSurface rSurf, double anX, double anY, double anZ, T3DPoint ptBase );
  void RotateXYZAllSelected( double anX, double anY, double anZ, T3DPoint ptBase );
  void ScaleXYZ( RTSurface rSurf, double scX, double scY, double scZ, T3DPoint pCenterScale );
  void ScaleXYZAllSelected( double scX, double scY, double scZ, T3DPoint pCenterScale );
  void Scale2D( RTSurface rSurf, double sc, T3DPoint pCenterScale );
  void Scale2DAllSelected( double sc, T3DPoint pCenterScale );
  void DeleteBody( RTSurface rSurf );
  void DeleteAllSelected();
  BOOL Flush( BOOL bFlAsk = FALSE );

  PTScena Clone( RTSurface ) const;
  PTScena CloneAllSelected() const;

  void NewObol( RTSurface );
  //void Render( HBITMAP& hbmRender );
  PTDIB Render( PTIndicator pInd, PTColorTable& rpPctTbl, RTimeLog rTLog );
  PTDIB RenderPhoto( PTIndicator pInd, PTColorTable& rpPctTbl, RTimeLog rTLog );
  PTDIB RenderStrScan( PTIndicator pInd, PTColorTable& rpPctTbl, RTimeLog rTLog );
  PTDIB RenderStrScanPhoto( PTIndicator pInd, PTColorTable& rpPctTbl, RTimeLog rTLog );
  int Save( HFILE hf, PTIndicator pInd );
  int Load( HFILE hf, PTIndicator pInd );

  long GetBytesSize( BOOL bFlOnlyFace = TRUE, BOOL bFlOnlySelected = FALSE ) const;
  long GetBytesSizeWithObol( BOOL bFlOnlyFace = TRUE, BOOL bFlOnlySelected = FALSE ) const;

  void SetHold( BOOL bFl = TRUE ) { bFlHold = bFl; }
  BOOL GetHold() const { return bFlHold; }

  BOOL SelectAll( BOOL bFlInvert );
  BOOL SelectWithWindow( RECT&, BOOL );
  BOOL SelectWithCrossing( RECT&, BOOL );
  BOOL SelectInvert();

  void Merge( RTScena );
  unsigned long SelectedCount( PTClassFilterFunc ) const;

  T3DPoint GetGeomCenterSelected() const;
  static TDelta GetDelta( RTSurface rSf );
  TDelta GetDeltaSelected() const;

  void RecalcBodyUserToBase( RTSurface );
  void RecalcBodyBaseToUser( RTSurface );

  PTVertexData GetNearstVertexInRange( TRECT _FAR &rCursor, POINT _FAR &rPoint ) const;

  PTSurface GetSelectedFirst( PTClassFilterFunc ) const;
  void AssignMediumAllSelected( RTMedium );

  void SetCriticalLock( BOOL bFl ) { bFlCriticalLock = bFl; }
  BOOL GetCriticalLock() const { return bFlCriticalLock; }

  BOOL IsUsedBaseUSC() const { return bFlBaseUCS; }
  void SetUsedBaseUCS( BOOL bFl ) { bFlBaseUCS = bFl; }

  PTUCS  GetUcs() const { return pUcs; }
  HDC GetHdc() const { return hdc; }
  PTWindow GetWindow() const { return pWin; }

  PTSurface GetObol( int ind ) const { return (PTSurface)(listObl[ ind ]); }

  double   AmbientMult() const { return dAmbMult; }
  void     AmbientMult( double dAM ) { dAmbMult = dAM; }
  RGBQUAD* Ambient() const { return (RGBQUAD*)&rgbAmbient; }
  RGBQUAD* Adges() const { return (RGBQUAD*)&rgbAdges; }
  RGBQUAD* Intersect() const { return (RGBQUAD*)&rgbIntersect; }

  TPolyhonalMethod Method() const { return polyMethod; }
  BOOL MarkInt() const { return bFlMarkIntersect; }
  void Method( TPolyhonalMethod pmM ) { polyMethod = pmM; }
  void MarkInt( BOOL bFl ) { bFlMarkIntersect = bFl; }


  void InitStrScanParamOfAdgeItems( double dStrScan );

  BOOL ListAdgeNormalyzeRev( PTIndicator pInd );
  BOOL ListAdgeNormalyzeRevInt( PTIndicator pInd );

  void SortFacets( int _export (*pCmpF)(const void *, const void *), PTIndicator pInd );
  void SortAdges( int _export (*pCmpF)(const void *, const void *), PTIndicator pInd );
  void SortSurfFacAdg( PTIndicator );
  DWORD SummOfNumbFacet() const;
  DWORD SummOfNumbAdges() const;

  PTBackGraund BkGnd() { return mrcBackGraund(); }
  void BkGnd( PTBackGraund pBkGnd ) { mrcBackGraund = pBkGnd; }


  int PaletteSize() const { return iPaletteSize; }
  void PaletteSize( int iPalSize ) { iPaletteSize = iPalSize; }
  TImageFormat ImageFormat() const { return ifFormat; }
  void ImageFormat( TImageFormat ifF ) { ifFormat = ifF; }

  void TestToFileBounds( LPSTR lpName );

  RECT GetFormatImage( double& dScale, int& iSx, int& iSy ) const;
  void MakeCompatibleFormatImage( double dScale, double iSx, double iSy );
  TConvertAlho Alho() const { return caAlhoPalette; }
  void Alho( TConvertAlho a ) { caAlhoPalette = a; }

  double AngleSmoth() const { return dAngleSmothGrad; }
  void AngleSmoth( double dA ) { dAngleSmothGrad = dA; }

  /*BOOL AntiAliasing() const { return bFlAntiAliasing; }
  void AntiAliasing( BOOL bFl ) { bFlAntiAliasing = bFl; }
  BOOL Shadows() const { return bFlShadows; }
  void Shadows( BOOL bFl ) { bFlShadows = bFl; }
  BOOL RenderBack() const { return bFlRenderBack; }
  void RenderBack( BOOL bFl ) { bFlRenderBack = bFl; }

  double PixelSize() const { return dPixelSize; }
  void PixelSize( double dPixS ) { dPixelSize = dPixS; }

  TShadingAlho ShadingLimit() const { return saShadingLimit; }
  void ShadingLimit( TShadingAlho sAlho ) { saShadingLimit = sAlho; }
  TTraceModel TraceModel() const { return tmTraceModel; }
  void TraceModel( TTraceModel trM ) { tmTraceModel = trM; }*/

  void GetPhotoVariables( RTPhotoSetupTransfer ) const;
  void SetPhotoVariables( const RTPhotoSetupTransfer );

  void Separate( RTIndexedDoubleList rdblLights, RTIndexedDoubleList rdblCameras ) const;
  void CalcBounds();

  TNoisy  Noisy() const { return nNoisy; }
  void Noisy( TNoisy n ) { nNoisy = n; }
  int  MarginalAmplitude() const { return iMarginalAmplitude; }
  void MarginalAmplitude( int m ) { iMarginalAmplitude = m; }

  int  UniformAmplitude() const { return iUniformAmplitude; }
  void UniformAmplitude( int u ) { iUniformAmplitude = u; }

  int  UniformThreshold() const { return iUniformThreshold; }
  void UniformThreshold( int u ) { iUniformThreshold = u; }

  void GetNoisy( RTNoisyDta rndDta ) const
   {
     rndDta.nNoisy             = nNoisy;
     rndDta.iMarginalAmplitude = iMarginalAmplitude;
     rndDta.iUniformAmplitude  = iUniformAmplitude;
     rndDta.iUniformThreshold  = iUniformThreshold;
   }

  LPCSTR AlhoNameStr() const;
  LPCSTR ModelNameStr() const;
  LPCSTR AntialiasingStatusStr() const;
  LPCSTR ShadowsStatusStr() const;
  LPCSTR GammaStatusStr() const;
  LPCSTR ShadingNameStr() const;
  LPCSTR IntersectStatusStr() const;
  LPCSTR PaletteNameStr() const;
  LPCSTR SizeNameStr() const;
  LPCSTR PaletteAlhoNameStr() const;
  LPCSTR DitherStatusStr() const;
  LPCSTR NoisyStatusStr() const;
  LPCSTR BkGndNameStr() const;

  LPCSTR BodyStr() const;
  LPCSTR LightsStr() const;
  LPCSTR VertexStr() const;
  LPCSTR AdgesStr() const;
  LPCSTR FacetsStr() const;

  BOOL TransparencyTest();


protected:
  PTUCS    pUcs;
  HDC      hdc;
  PTWindow pWin;

private:
  double dMinX, dMaxX, dMinY, dMaxY, dMinZ, dMaxZ;

  TIndexedDoubleList  listSurf;    //list of bodys
  TIndexedDoubleList  listObl;
  RGBQUAD  rgbAdges;
  RGBQUAD  rgbIntersect;
  RGBQUAD  rgbAmbient;
  double   dAmbMult;
  TPolyhonalMethod polyMethod;
  BOOL bFlMarkIntersect;

  BOOL bFlAntiAliasing, bFlShadows;
  int iNumbPointsAntiAlias;
  double dStrengtsAntialias;
  double dAngleSmothGrad;
  TShadingAlho saShadingLimit;
  TTraceModel  tmTraceModel;

  TConvertAlho  caAlhoPalette;

  int iPaletteSize;
  TImageFormat ifFormat;

  TNoisy nNoisy;
  int iMarginalAmplitude;
  int iUniformAmplitude;
  int iUniformThreshold;

  BOOL bFlStrictGamma;

  TAutoMemoryRc<PTBackGraund> mrcBackGraund;
  //PTBackGraund bgBackGraund;

  PTApplication pApplicat;


  BOOL  bFlChanged;
  BOOL  bFlCanClose;
  BOOL  bFlHold;
  int IdFile;

  BOOL  bFlShowBackFaces, bFlFullDetal;
  BOOL  bFlVisyal;

  BOOL  bFlBaseUCS;

  BOOL   bFlCriticalLock; //MDI Childs drawing

  //void  (*proector)( PCTScena, PT3DPoint );
  TProectorFunc proector;

  BOOL bFlLeftUCS; //used by tracing

  int iMaxCornerDelta; //0..255 for one chanel
  int iMaxLevel;

  void RecalcUserToBase();
  void RecalcBaseToUser();

  int near pascal FindIntersect( double& dZ, double clm, double str,
			         PTFacet pFacet,
			         DoubleList& rdblRemove,
				 BOOL bFlRemove = TRUE
			       );
  int near pascal FindIntersectReal( double& dZ, double clm, double str,
			             PTFacet pFacet,
				     DoubleList& rdblRemove,
			             BOOL bFlRemove = TRUE
				   );

  void RecalcLPtoDP();
  void RecalcDPtoLP();


  void GetSwap( PTIndicator pInd );

  void AssignColorIndexes();

  void SpecialPointLPtoDP( double& x, double& y, int w, int h, double dSc ) const;
  void SpecialPointDPtoLP( double& x, double& y, int w, int h, double dSc ) const;
  void SpecialBounds( double& x, double& y, RECT r ) const;

  int GetMaxAdges() const;

  PTFintIntersectFunc pFIntersect;

  BOOL near pascal Shade( RTVector vecOmni, RTVector vecPoint, RTTracData rDta, RTVector vecRay );
  TRGBd near pascal Shadow( TIndexedDoubleList& rdblLights, RTTracData rDta, double x, double y, PTScena pScenaShadow, double& rZNorm );
  BOOL RenderPrepareBodys( PTIndicator pInd );

  BOOL near pascal FlipAdgesItems( PTIndicator pInd );

  void CalcAllTraceObol( PTIndicator pInd );  

  //PTColorTable CreateColorTbl();
 };


template<class T> T TSPtr( PTSurface ptr, T pTyp = 0 )
 {
   typedef T T_;

   if( !ptr ) return NULL;

   switch( ptr->isA() )
    {
      case TYP_FACE:
      case TYP_CYLINDER:
      case TYP_CONE:
      case TYP_TUBE:
      case TYP_SPHERE:
      case TYP_TORUS:
	PTSurface pSf = (PTSurface)ptr->TopPtr();
	return T_( (void*)pSf );

      case TYP_TOBOL:
	PTObol pObl =  (PTObol)ptr->TopPtr();
	return T_( (void*)pObl );

      case TYP_TOMNI:
	PTOmni pOmni =  (PTOmni)ptr->TopPtr();
	return T_( (void*)pOmni );

      case TYP_TCAMERA:
	PTCamera pCam =  (PTCamera)ptr->TopPtr();
	return T_( (void*)pCam );

      case TYP_TOMNIOBOL:
	PTOmniObol pOm =  (PTOmniObol)ptr->TopPtr();
	return T_( (void*)pOm );

      case TYP_TCAMERAOBOL:
	PTCameraObol pCamObl =  (PTCameraObol)ptr->TopPtr();
	return T_( (void*)pCamObl );
    }

   return NULL;
 }


#endif

