#if !defined(_SURFACE_)
#define _SURFACE_

#include <owl.h>
#include <dbllist.h>
#include "vector.hpp"
#include "facedial.hpp"
#include "ucs.hpp"
#include "indicat.hpp"
#include "ind_dbl.hpp"
#include "medium.hpp"
#include "rgb.hpp"
#include <memmgr.h>

#include "threshol.hpp"

#define TYP_VERTEX   122
#define TYP_ADGE     123
#define TYP_FACET    124
#define TYP_FACE     125
#define TYP_ADGEITEM 126

#define TYP_CYLINDER 127
#define TYP_CONE     128
#define TYP_TUBE     129
#define TYP_SPHERE   130
#define TYP_TORUS    131
#define TYP_TOBOL    132

#define TYP_TOMNI     133
#define TYP_TCAMERA   134
#define TYP_TOMNIOBOL 135
#define TYP_TCAMERAOBOL 136

#define TYP_FACETITEM 140


_CLASSDEF(TScena);

typedef void (TUCS::* _FAR PTMemberRecalcFunc)(PT3DPoint, unsigned long);

_CLASSDEF(TVertex)
_CLASSDEF(TAdge)
_CLASSDEF(TFacet)
_CLASSDEF(TFacetItem)
_CLASSDEF(TAdgeItem)

_CLASSDEF(TInitShape)

_CLASSDEF(TSurface)
_CLASSDEF(TCylinder)
_CLASSDEF(TCone)
_CLASSDEF(TTube)
_CLASSDEF(TSphere)
_CLASSDEF(TTorus)

_CLASSDEF(TObol)
_CLASSDEF(TExtFaceData)

_CLASSDEF(TOmni)
_CLASSDEF(TCamera)
_CLASSDEF(TOmniObol)
_CLASSDEF(TCameraObol)


int _export CompareFacetY( const void *p1, const void *p2 );
int _export CompareFacetYRev( const void *p1, const void *p2 );
int _export CompareFacetYRevInt( const void *p1, const void *p2 );

int _export CompareAdgeItemY( const void *p1, const void *p2 );
int _export CompareAdgeItemYRev( const void *p1, const void *p2 );
int _export CompareAdgeItemYRevInt( const void *p1, const void *p2 );

int _export CompareAdgeY( const void *p1, const void *p2 );

int _export CompareSurfaceY( const void *p1, const void *p2 );
int _export CompareSurfaceYRev( const void *p1, const void *p2 );
int _export CompareSurfaceYRevInt( const void *p1, const void *p2 );

typedef TVector (TFacet::* _FAR PTGetNormalFunc)( double x, double y );

class  TVertex: public TIndexedItem, public T3DPoint
 {
public:
  TVector  vecMeanPhongNormal;

  TVertex(): TIndexedItem(-1), T3DPoint( 0, 0, 0 ) {};
  TVertex( int ind, double vX, double vY, double vZ ):
    TIndexedItem(ind),
    T3DPoint( vX, vY, vZ )
   {
   }
  TVertex( T3DPoint pt ): TIndexedItem(-1), T3DPoint( pt ) {};
  TVertex( int ind, RT3DPoint pt ):
    TIndexedItem(ind),
    T3DPoint( pt )
    {
    };
  TVertex( RTVertex rV ):
    TIndexedItem( rV.Index() ),
    T3DPoint( rV.x, rV.y, rV.z )
   {
     vecMeanPhongNormal = rV.vecMeanPhongNormal;
   }
/*  TVertex( RTVertex rVert )
   {
     x = rVert.x; y = rVert.y; z = rVert.z;
   }*/

   virtual classType isA() const { return TYP_VERTEX; }
   virtual hashValueType hashValue() const { return 0; }
   virtual int isEqual( const Object& ) const;
   virtual char *nameOf() const { return "Vertex"; }
   virtual void printOn( ostream& ) const {};

   int Save( HFILE );
   int Load( HFILE );

   static long SizeData();

   void _FAR* operator new( size_t size );
   void operator delete( void _FAR* pToDel );

private:
  static MemBlocks *mgr;
  static long countMemMgr;

 };

class  TAdge: public TIndexedItem
 {
public:
  PTVertex  pV1, pV2;  

  TAdge(): TIndexedItem(-1)  { pV1 = pV2 = NULL;  }
  TAdge( int ind, PTVertex v1, PTVertex v2 ):
    TIndexedItem(ind)
   {
     pV1 = v1; pV2 = v2;
   }

  TAdge( int ind, PTIndexedItem v1, PTIndexedItem v2 ):
    TIndexedItem(ind)
   {
     pV1 = (PTVertex)v1; pV2 = (PTVertex)v2;
   }
  /*TAdge( RTAdge rAdge )
   {
     pV1 = rAdge.pV1; pV2 = rAdge.pV2;
     signClockVise = rAdge.signClockVise;
   }*/

   int CompareY( RTAdge );

   virtual classType isA() const { return TYP_ADGE; }
   virtual hashValueType hashValue() const { return 0; }
   virtual int isEqual( const Object& ) const;
   virtual char *nameOf() const { return "Adge"; }
   virtual void printOn( ostream& ) const {};

   int Save( HFILE );

   static long SizeData();

   void _FAR* operator new( size_t size );
   void operator delete( void _FAR* pToDel );

private:
  static MemBlocks *mgr;
  static long countMemMgr;

 };


class  TAdgeItem: public TIndexedItem
 {
public:
  int clockVise;
  PTAdge  pAdge;
  PTFacet pFCommon;
  double dAngleRadNormalsSelfAndFCommon;
  TVector vecNorm1, vecNorm2;

  double dStrScan;   //uses by FindIntersect of TRACE
  double dTestParam; //uses by FindIntersect of TRACE
  BOOL bTestParam; //uses by FindIntersect of TRACE


  TAdgeItem(): TIndexedItem(-1)  { pAdge = NULL; clockVise = 1; pFCommon = NULL; }
  TAdgeItem( int ind, PTAdge pAg, int direct ):
    TIndexedItem(ind)
   {
     pAdge = pAg; clockVise = direct; pFCommon = NULL; dAngleRadNormalsSelfAndFCommon = 0;
   }

  TAdgeItem( int ind, PTIndexedItem pAg, int direct ):
    TIndexedItem(ind)
   {
     pAdge = (PTAdge)pAg; clockVise = direct; pFCommon = NULL; dAngleRadNormalsSelfAndFCommon = 0;
   }

   TVector PhongN( PTVertex pV )
    {
      return (pV == pAdge->pV1) ? vecNorm1:vecNorm2;
    }

   int CompareY( RTAdgeItem );

   virtual classType isA() const { return TYP_ADGEITEM; }
   virtual hashValueType hashValue() const { return 0; }
   virtual int isEqual( const Object& ) const;
   virtual char *nameOf() const { return "AdgeItem"; }
   virtual void printOn( ostream& ) const {};

   static long SizeData();

   void _FAR* operator new( size_t size );
   void operator delete( void _FAR* pToDel );

private:
  static MemBlocks *mgr;
  static long countMemMgr;

 };


class  TFacet: public TIndexedItem
 {
public:
  TIndexedDoubleList listAgesItem;
  TVector  vecOutNormal;
  int iMinX, iMaxX, iMinY, iMaxY, iMinZ, iMaxZ;
  double minY, minZ, minX, maxY, maxZ, maxX;
  double d;

  int iSmothGroup; //0 - no smoth

  PTFacet pSPParent; //special uses


  TFacet():TIndexedItem(-1) { iSmothGroup = 0; pSPParent = NULL; };
  TFacet(int ind):TIndexedItem( ind ) { iSmothGroup = 0; pSPParent = NULL; };

  int CompareY( RTFacet );

   virtual classType isA() const { return TYP_FACET; }
   virtual hashValueType hashValue() const { return 0; }
   virtual int isEqual( const Object& ) const;
   virtual char *nameOf() const { return "Facet"; }
   virtual void printOn( ostream& ) const {};

   int Save( HFILE hf );
   long GetBytesSize() const;

   static long SizeData();

   //TVector GetNewelNormal() const;
   TVector GetNewelNormal( PTUCS pUCS = NULL ) const;
   T3DPoint GetGeomCenter() const;
   void CalcBounds();

   TVector GetTraceNormal( double x, double y );
   TVector GetTraceNormalPhong( double x, double y );

   void _FAR* operator new( size_t size );
   void operator delete( void _FAR* pToDel );

private:
  static MemBlocks *mgr;
  static long countMemMgr;

 };

class  TFacetItem: public TIndexedItem
 {
public:
  PTFacet  pFacet;  

  
  TFacetItem( RTFacet rF ):
    TIndexedItem(-1)
   {
     pFacet = &rF;  
   }

   virtual classType isA() const { return TYP_FACETITEM; }
   virtual hashValueType hashValue() const { return 0; }
   virtual int isEqual( const Object& ) const;
   virtual char *nameOf() const { return "FacetItem"; }
   virtual void printOn( ostream& ) const {};

   void _FAR* operator new( size_t size );
   void operator delete( void _FAR* pToDel );

private:
  static MemBlocks *mgr;
  static long countMemMgr;

 };


class  TInitShape
 {
public:
  TInitShape( void (*initShape)(PTSurface, PTFaceData) )
   {
     this->initShape = initShape;
   }

protected:
  void (*initShape)(PTSurface, PTFaceData);
 };


typedef float (TSurface::* PTFuncTransparency)( float fZNormal ) const;

                         //DoubleList
class  TSurface: public TIndexedItem, virtual public TInitShape
 {
public:
  int Status;
  PTFaceData pfdKey;
  double minX, maxX, minY, maxY, minZ, maxZ;
  int iMinX, iMaxX, iMinY, iMaxY, iMinZ, iMaxZ;
  BOOL bFlObolVis;

  double dRadSphricObol; //used by tracer
  TVector vecCenter; //used by tracer

  TMedium medium;

  TIndexedDoubleList  listVertex;
  TIndexedDoubleList  listAdges;
  TIndexedDoubleList  listFacets;

  PTFacet  pfacInitAlignFac;
  PTVertex pvInitAlignVert;


  TSurface( PTFaceData pFD );
  TSurface( int ind, PTFaceData pFD );
  TSurface( RTSurface, PTIndicator );
  

  virtual Pvoid TopPtr() { return this; }
  //int (TIndexedItem::* PTCompareFunc)( RTIndexedItem rObjForCmp );
  int CompareY( RTSurface );

  BOOL IsTransparency() const
   {
     return (medium.GdKt() > TRACER_d_TransparencyZero) ||
	    ((medium.GdRefr() > TRACER_d_TransparencyZero ||
	     medium.GdBetta() > TRACER_d_TransparencyZero) &&
	     medium.IsLineTransp() == FALSE);
   }

  float GdKt( float fZNorm ) const
   {
     return (this->*pFuncTransparency)( fZNorm );
   }

  void AssignTransparencyFunc();

  float TransparencyLinear( float fZNorm ) const;
  float TransparencyFalloff( float fZNorm ) const;

  void DetectCommonFacets();
  void AutoSmoth( double dGradAngleMinSmoth );
  void ResetSmothGroups();
  void SmothAllInOneGroup( int idGrp );

  virtual void Draw( BOOL, PTScena ) const;
  virtual void DrawSpecial( BOOL, PTScena );

  virtual PTObol CreateObol() const;

  virtual BOOL IsSelectable() const { return TRUE; }

  virtual int Save( HFILE hf, PTIndicator pInd );
  virtual int Load( HFILE hf, PTIndicator pInd  );
  virtual long GetBytesSize() const;

  virtual BOOL GetAbsFlag() const {return TRUE;}
  virtual void SetAbsFlag( BOOL ) {};

  static void InitShape( PTSurface pSurface, PTFaceData pFD );

  void TestToFile( LPSTR lpName );

  void CalcNormals();
  void CalcNormalsAllVertex(  PTIndicator pInd  );
  void CalcNormalsAllVertexPhongAdges();
  TVector CreateNormal( PTAdge a1, PTAdge a2 );

  T3DPoint GetGeomCenter();

  virtual void Move( double dx, double dy, double dz );
  virtual void RotateZ( double angleRad, BOOL bFlFlipNormals = TRUE );
  virtual void RotateX( double angleRad, BOOL bFlFlipNormals = TRUE );
  virtual void RotateY( double angleRad, BOOL bFlFlipNormals = TRUE );
  virtual void ScaleXYZ( double scX, double scY, double scZ, T3DPoint center );
  virtual void Scale2D( double sc, T3DPoint center );

  TVector CalcVertexNormal( RTVertex, int iSmothGrp = -1 ) const;

   virtual classType isAPrepare() const { return TYP_FACE; }
   virtual classType isA() const { return TYP_FACE; }
   virtual hashValueType hashValue() const { return 0; }
   virtual int isEqual( const Object& ) const;
   virtual char *nameOf() const { return "Face"; }
   virtual void printOn( ostream& ) const {};

  BOOL NeedCreateObol() const { return bFlCreateObol; }
  void SetObol( BOOL fl ) { bFlCreateObol = fl; }

  void CalcBounds();
  void CalcBoundsAllFacets();

  virtual void ResetVertex() {};
  virtual void GetVertex() {};

  void CreateTraceObol();

  void Select( BOOL bFlSel ) { bFlSelect = (IsSelectable() == TRUE) ? bFlSel:FALSE; }
  BOOL IsSelected() const { return bFlSelect; }


  void AssignFaceColorIndex( BYTE index )
   {
     medium.AssignColorIndex( index );
   }

  BYTE GetFaceColorIndex() const
   {
     return medium.GetColorIndex();
   }

  RGBQUAD _FAR & GetFaceRGBColor() const
   {
     return medium.GetRGBColor();
   }

  void GetRGB( TRGB _FAR &rgb ) const
   {
     rgb = TRGB( medium.GetRGBColor() );
     /*rgb.r = medium.GetRGBColor().rgbRed;
     rgb.g = medium.GetRGBColor().rgbGreen;
     rgb.b = medium.GetRGBColor().rgbBlue;*/
   }

  void GetCommonFacetsList( DoubleList _FAR &rdblList, RTVertex rV ) const;

  virtual void GetCreationAlignData( RTVector rvAlign, RTVertex rvertAlign ) const
   {
     rvAlign = TVector();
     //pvertAlign = NULL;
   }

 // ********* Uses by tracing *********
  PTGetNormalFunc pGetTraceNormalFunc;
  TShadingAlho tsAlho;
  double dIAmbientR, dIAmbientG, dIAmbientB;
 // ********* Uses by tracing *********

protected:
  void GetCreationAlignDataFacet( RTVector rvAlign, RTVertex rvertAlign ) const;
  void GetCreationAlignDataVertex( RTVector rvAlign, RTVertex rvertAlign ) const;


private:
  BOOL bFlCreateObol;
  BOOL bFlSelect;

  PTFuncTransparency pFuncTransparency;
 };

class  TCylinder: public TSurface
 {
public:
  TCylinder( PTFaceData pFD );
  TCylinder( int ind, PTFaceData pFD );
  TCylinder( RTCylinder rCyl, PTIndicator pInd ):
    TSurface( rCyl, pInd ),
    TInitShape( NULL )
   {
   };
  //TCylinder():TInitShape( NULL ), TSurface( NULL ) {};
  //TCylinder( int ind, PTSurface );

  virtual Pvoid TopPtr() { return this; }
  virtual classType isA() const { return TYP_CYLINDER; }

  static void InitShape( PTSurface pSurface, PTFaceData pFD );

  virtual void GetCreationAlignData( RTVector rvAlign, RTVertex rvertAlign ) const;
 };

class  TCone: public TSurface
 {
public:
  TCone( PTFaceData pFD );
  TCone( int ind, PTFaceData pFD );
  TCone( RTCone rCone, PTIndicator  pInd ):
    TSurface( rCone, pInd ),
    TInitShape( NULL )
   {
   };

  virtual Pvoid TopPtr() { return this; }
  virtual classType isA() const { return TYP_CONE; }

  static void InitShape( PTSurface pSurface, PTFaceData pFD );

  virtual void GetCreationAlignData( RTVector rvAlign, RTVertex rvertAlign ) const;
    
 };

class  TTube: public TSurface
 {
public:
  TTube( PTFaceData pFD );
  TTube( int ind, PTFaceData pFD );
  TTube( RTTube rTube, PTIndicator  pInd  ):
    TSurface( rTube, pInd ),
    TInitShape( NULL )
   {
   };

  virtual Pvoid TopPtr() { return this; }
  virtual classType isA() const { return TYP_TUBE; }

  static void InitShape( PTSurface pSurface, PTFaceData pFD );

  virtual void GetCreationAlignData( RTVector rvAlign, RTVertex rvertAlign ) const;

 };

class  TSphere: public TSurface
 {
public:
  TSphere( PTFaceData pFD );
  TSphere( int ind, PTFaceData pFD );
  TSphere( RTSphere rSphere, PTIndicator  pInd  ):
    TSurface( rSphere, pInd ),
    TInitShape( NULL )
   {
   };

  virtual Pvoid TopPtr() { return this; }
  virtual classType isA() const { return TYP_SPHERE; }

  static void InitShape( PTSurface pSurface, PTFaceData pFD );

  virtual void GetCreationAlignData( RTVector rvAlign, RTVertex rvertAlign ) const;
 };


class  TTorus: public TSurface
 {
public:
  TTorus( PTFaceData pFD );
  TTorus( int ind, PTFaceData pFD );
  TTorus( RTTorus rTorus, PTIndicator  pInd  ):
    TSurface( rTorus, pInd ),
    TInitShape( NULL )
   {
   };

  virtual Pvoid TopPtr() { return this; }
  virtual classType isA() const { return TYP_TORUS; }

  static void InitShape( PTSurface pSurface, PTFaceData pFD );

  virtual void GetCreationAlignData( RTVector rvAlign, RTVertex rvertAlign ) const;

 };

class  TObol: virtual public TSurface 
 {
public:
  BOOL bFlAbsTransf;
  TIndexedDoubleList  listVK;

  TObol( PTSurface psfSurf );
  TObol( int ind, PTSurface psfSurf );
  TObol( RTObol rObol, PTIndicator  pInd  );
  TObol( PTScena psfSurf );

  virtual Pvoid TopPtr() { return this; }
  virtual classType isA() const { return TYP_TOBOL; }

  virtual PTObol CreateObol() const
   {
     return NULL;
   }

  virtual BOOL GetAbsFlag() const {return bFlAbsTransf;}
  virtual void SetAbsFlag( BOOL fl ) {bFlAbsTransf = fl;}

  static void InitShape( PTSurface pSurface, PTFaceData pFD );

  void Setup( PTExtFaceData pFd );

  virtual void Move( double dx, double dy, double dz );
  virtual void RotateZ( double angleRad, BOOL bFlFlipNormals = TRUE );
  virtual void RotateX( double angleRad, BOOL bFlFlipNormals = TRUE );
  virtual void RotateY( double angleRad, BOOL bFlFlipNormals = TRUE );  
  virtual void ScaleXYZ( double scX, double scY, double scZ, T3DPoint center );
  virtual void Scale2D( double sc, T3DPoint center );

  virtual void ResetVertex();
  virtual void GetVertex();

  T3DPoint GetGeomCenterObol();
 };

class   TExtFaceData: public TFaceData
 {
public:
  PTSurface pSf;
  PTScena pScena;

  TExtFaceData( PTSurface pSf ): TFaceData() { this->pSf = pSf; pScena = NULL; }
  TExtFaceData( PTScena pSc ): TFaceData() { pSf = NULL; pScena = pSc; }

  virtual void DeleteControls() {};
  virtual void InitControls( HFONT hf, PTApplication pApp, HWND hwnd ) {};
  virtual LPSTR GetNameStr() const { return NULL; }
  virtual int isA() const { return FT_Base + 100; }

 };


class  TOmni: virtual public TSurface
 {
public:
  /*
  TIndexedDoubleList  listVertex;  index 1 - point
  TIndexedDoubleList  listAdges;   empty
  TIndexedDoubleList  listFacets;  empty
  */

  TOmni( PTOmniInit pInit );
  TOmni( int ind, PTOmniInit pInit );
  TOmni( RTOmni rOmni, PTIndicator pInd ):
    TSurface( rOmni, pInd ),
    TInitShape( NULL )
   {
     rgbColor = rOmni.rgbColor;
     mult = rOmni.mult;
     bFlOn = rOmni.bFlOn;
     SetObol( FALSE );
     bFlCastShadows = rOmni.bFlCastShadows;
     r = rOmni.r;
     g = rOmni.g;
     b = rOmni.b;
   }

  virtual Pvoid TopPtr() { return this; }

  virtual PTObol CreateObol() const;

  virtual int Save( HFILE hf, PTIndicator pInd );
  virtual int Load( HFILE hf, PTIndicator pInd  );

  virtual void Draw( BOOL, PTScena ) const;
  virtual void DrawSpecial( BOOL, PTScena );

  virtual classType isAPrepare() const { return TYP_TOMNI; }
  virtual classType isA() const { return TYP_TOMNI; }

  BYTE R() const { return rgbColor.rgbRed; }
  BYTE G() const { return rgbColor.rgbGreen; }
  BYTE B() const { return rgbColor.rgbBlue; }
  float Rd() const { return r; }
  float Gd() const { return g; }
  float Bd() const { return b; }
  RGBQUAD GetReductedColor() const;
  void RGBColor( RGBQUAD& rRGB )
   {
     rgbColor = rRGB;
     r = float(rRGB.rgbRed) / 255.0;
     g = float(rRGB.rgbGreen) / 255.0;
     b = float(rRGB.rgbBlue) / 255.0;
   }

  void Mult( double m ) { mult = m; }
  double GetMult() const { return mult; }
  BOOL IsOn() const { return bFlOn; }
  void SetSwitch( BOOL bFl ) { bFlOn = bFl; }
  BOOL Shadow() const { return bFlCastShadows; }
  void Shadow( BOOL bFlCS ) { bFlCastShadows = bFlCS; }

  static void InitShape( PTSurface pSurface, PTFaceData pFD );  

  virtual BOOL IsSelectable() const { return TRUE; }
  virtual long GetBytesSize() const
   {
     return (unsigned long)TSurface::GetBytesSize() + sizeof(double) +
	    sizeof(BOOL) + sizeof(RGBQUAD);
   }

  void Place( RT3DPoint );
  T3DPoint Place() const
   {
     return  (RT3DPoint)RTVertex(listVertex.peekAtHead());
   }

private:
  RGBQUAD rgbColor;
  float r, g, b;

  double mult;
  BOOL bFlOn;
  BOOL bFlCastShadows;

 };

class  TCamera: virtual public TSurface
 {
public:
  /*
  TIndexedDoubleList  listVertex;  index 1 - camera, index 2 - dir
  TIndexedDoubleList  listAdges;   index 1 - vector
  TIndexedDoubleList  listFacets;  empty
  */

  TCamera( PTCameraInit pInit );
  TCamera( int ind, PTCameraInit pInit );
  TCamera( RTCamera rCamera, PTIndicator pInd ):
    TSurface( rCamera, pInd ),
    TInitShape( NULL )
   {
     dAOV = rCamera.dAOV;
     dRoll = rCamera.dRoll;
     bFlShowCone = rCamera.bFlShowCone;
     SetObol( FALSE );
   }

  virtual Pvoid TopPtr() { return this; }

  virtual PTObol CreateObol() const;

  virtual int Save( HFILE hf, PTIndicator pInd );
  virtual int Load( HFILE hf, PTIndicator pInd  );

  virtual void Draw( BOOL, PTScena ) const;
  virtual void DrawSpecial( BOOL, PTScena );

  virtual classType isAPrepare() const { return TYP_TCAMERA; }
  virtual classType isA() const { return TYP_TCAMERA; }

  static void InitShape( PTSurface pSurface, PTFaceData pFD ); 

  virtual BOOL IsSelectable() const { return TRUE; }

  virtual long GetBytesSize() const
   {
     return (unsigned long)TSurface::GetBytesSize() + 2*sizeof(double) +
            sizeof(BOOL);
   }

private:
  double dAOV;
  double dRoll;
  BOOL bFlShowCone;
 };

class  TOmniObol: public TOmni, public TObol//, virtual public TSurface
 {
public:
  TOmniObol( PTOmni pOmni );
  TOmniObol( int ind, PTOmni pOmni );
  

  virtual Pvoid TopPtr() { return this; }

  virtual void Draw( BOOL bFlShowBackFaces, PTScena pSc ) const
   {
     TOmni::Draw( bFlShowBackFaces, pSc );
   }

  virtual void DrawSpecial( BOOL bFlShowBackFaces, PTScena pSc ) 
   {
     TOmni::DrawSpecial( bFlShowBackFaces, pSc );
   }

  virtual PTObol CreateObol() const
   {
     return NULL;
   }

  //static void InitShape( PTSurface pSurface, PTFaceData pFD );
  virtual classType isA() const { return TYP_TOMNIOBOL; }
  virtual int isEqual( const Object& rObj ) const
   {
     return (TSurface huge *)this == (TSurface huge *)&rObj;
   }
  virtual long GetBytesSize() const
   {
     return TOmni::GetBytesSize(); 
   }

 };

class  TCameraObol: public TCamera, public TObol, virtual public TSurface
 {
public:
  TCameraObol( PTCamera pCamera );
  TCameraObol( int ind, PTCamera pCamera );

  virtual Pvoid TopPtr() { return this; }

  virtual PTObol CreateObol() const
   {
     return NULL;
   }

  virtual void Draw( BOOL bFlShowBackFaces, PTScena pSc ) const
   {
     TCamera::Draw( bFlShowBackFaces, pSc );
   }

  virtual void DrawSpecial( BOOL bFlShowBackFaces, PTScena pSc ) 
   {
     TCamera::DrawSpecial( bFlShowBackFaces, pSc );
   }

  //static void InitShape( PTSurface pSurface, PTFaceData pFD );
  virtual classType isA() const { return TYP_TCAMERAOBOL; }
  virtual int isEqual( const Object& rObj ) const
   {
     return (TSurface huge *)this == (TSurface huge *)&rObj;
   }
  virtual long GetBytesSize() const
   {
     return TCamera::GetBytesSize();
   }

 };



inline long TVertex::SizeData()
 {
   return sizeof(T3DPoint) + sizeof(TVector);
 }

inline long TAdge::SizeData() 
 {
   return 2 * sizeof(int);
 }


inline long TFacet::SizeData() 
 {
   return  sizeof(TVector) + sizeof(int);
 }

inline long TAdgeItem::SizeData() 
 {
   return 2 * sizeof(int);
 }

       
#endif
