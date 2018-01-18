#if !defined(_FACE_DIAOG_)
#define _FACE_DIAOG_

#include "bmp.hpp"
#include "centerdl.hpp"
#include "status.hpp"
#include "ucs.hpp"
#include "measur.mac"
#include <dbllist.h> //DoubleList
#include <object.h>
#include <scrollba.h>
#include <static.h>

#include "INC\errors.inc"
#include "INC\dlg.inc"
#include "INC\bodybmp.inc"

_CLASSDEF(TFaceDialog)
	  
#define TYP_FACE_ITEM  121

_CLASSDEF(TFaceItem)
_CLASSDEF(TMyScrollBar)

_CLASSDEF(TFaceData)
_CLASSDEF(TCylinderInit)
_CLASSDEF(TConeInit)
_CLASSDEF(TSphereInit)
_CLASSDEF(TTorusInit)
_CLASSDEF(TTubeInit)
_CLASSDEF(TOmniInit)
_CLASSDEF(TCameraInit)

enum  FaceTypes { FT_Base, FT_Cylinder, FT_Cone, FT_Sphere, FT_Torus,
  FT_Tube, FT_Omni, FT_Camera };

#define  CYLINDER_SCROLLBAR_SEGM      300
#define  CYLINDER_SCROLLBAR_SEGM_TEXT 301

#define  CONE_SCROLLBAR_SEGM          302
#define  CONE_SCROLLBAR_SEGM_TEXT     303

#define  SPHERE_SCROLLBAR_MERID       304
#define  SPHERE_SCROLLBAR_MERID_TEXT  305
#define  SPHERE_SCROLLBAR_HORIZ       306
#define  SPHERE_SCROLLBAR_HORIZ_TEXT  307

#define  TORUS_SCROLLBAR_MERID        308
#define  TORUS_SCROLLBAR_MERID_TEXT   309
#define  TORUS_SCROLLBAR_HORIZ        310
#define  TORUS_SCROLLBAR_HORIZ_TEXT   311

#define  TUBE_SCROLLBAR_SEGM          312
#define  TUBE_SCROLLBAR_SEGM_TEXT     313

#undef _EXPORT2
#if defined(_FACEDIALDLL) || defined(_ALLDLGDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif


class _EXPORT2 TFaceData
 {
public:
   BOOL bFlSmoothed;
   BOOL bFlAligmented;
   T3DPoint insertPoint;

   double dAngleSmothGrad;

   TFaceData() { bFlAligmented = bFlSmoothed = TRUE; dAngleSmothGrad = 60;
     _fsetmem(&insertPoint, sizeof(T3DPoint), 0); pOwn = NULL; }

   virtual int isA() const { return FT_Base; }
   virtual void InitControls( HFONT hf, PTApplication pApp, HWND hwnd );
   virtual void DeleteControls() = 0;

   virtual LPSTR GetNameStr() const = 0;

   void SetOwner( PTDialog pOwn ) { this->pOwn = pOwn; }

protected:
  PTDialog pOwn;
 };

class _EXPORT2 TCylinderInit: public TFaceData
 {
public:
  int nSides;

  int radius;
  int high;

  TCylinderInit(): TFaceData()
    { nSides=16; radius = 50; high = 200; pScSegm = NULL; }

  virtual int isA() const { return FT_Cylinder; }
  virtual void InitControls( HFONT hf, PTApplication pApp, HWND hwnd );
  virtual void DeleteControls();

  virtual LPSTR GetNameStr() const
   {
     return "Cylinder";
   }

private:
  PTMyScrollBar pScSegm;
 };

class _EXPORT2 TConeInit: public TFaceData
 {
public:
  int nSides;

  int radius;
  int high;

  TConeInit(): TFaceData()
    { nSides=16; radius=50; high = 200; pScSegm = NULL;}

  virtual int isA() const { return FT_Cone; }
  virtual void InitControls(  HFONT hf, PTApplication pApp, HWND hwnd );
  virtual void DeleteControls();

  virtual LPSTR GetNameStr() const
   {
     return "Cone";
   }


private:
  PTMyScrollBar pScSegm;
 };

class _EXPORT2 TSphereInit: public TFaceData
 {
public:
  int nMerid, nHoriz;

  int radius;

  TSphereInit(): TFaceData()
    { nMerid = 16; nHoriz = 8; radius = 50; pScMerid = pScHoriz = NULL; }

  virtual int isA() const { return FT_Sphere; }
  virtual void InitControls(  HFONT hf, PTApplication pApp, HWND hwnd );
  virtual void DeleteControls();

  virtual LPSTR GetNameStr() const
   {
     return "Sphere";
   }


private:
  PTMyScrollBar pScMerid, pScHoriz;
 };

class _EXPORT2 TTorusInit: public TFaceData
 {
public:
   int nMerid, nHoriz;

   int rad1, rad2;

   TTorusInit(): TFaceData()
     { nMerid = 16;  nHoriz = 8; rad1 = 50; rad2 = 100; pScMerid = pScHoriz = NULL;}

   virtual int isA() const { return FT_Torus; }
   virtual void InitControls(  HFONT hf, PTApplication pApp, HWND hwnd );
   virtual void DeleteControls();

   virtual LPSTR GetNameStr() const
   {
     return "Torus";
   }


private:
  PTMyScrollBar pScMerid, pScHoriz;
 };

class _EXPORT2 TTubeInit: public TFaceData
 {
public:
  int nSides;

  int rad1, rad2, high;

  TTubeInit(): TFaceData()
    { nSides = 16; rad1 = 50; rad2 = 100; high = 200; pScSegm = NULL; }

  virtual int isA() const { return FT_Tube; }
  virtual void InitControls(  HFONT hf, PTApplication pApp, HWND hwnd );
  virtual void DeleteControls();

  virtual LPSTR GetNameStr() const
   {
     return "Tube";
   }


private:
  PTMyScrollBar pScSegm;
 };

class _EXPORT2 TOmniInit: public TFaceData
 {
public:
  T3DPoint pt;
  RGBQUAD rgbColor;
  double mult;
  BOOL bFlOn;

  TOmniInit( RT3DPoint rPt, RGBQUAD _FAR & rgbColor_, double mult_ ):
    TFaceData(),
    pt( rPt ),
    rgbColor( rgbColor_ )
   {
     mult = mult_; bFlOn = TRUE;
   }

  virtual int isA() const { return FT_Omni; }

  virtual void DeleteControls() {};
  virtual LPSTR GetNameStr() const { return "Omni"; }
 };

class _EXPORT2 TCameraInit: public TFaceData
 {
public:
  T3DPoint ptCamera, ptDir;
  double dAOV;
  double dRoll;
  BOOL bFlShowCone;


  TCameraInit( RT3DPoint rPtCamera, RT3DPoint rPtDir, double dAOV_,
               double dRoll_, BOOL bFlShowCone_ ):
    TFaceData(),
    ptCamera( rPtCamera ),
    ptDir( rPtDir )
    {
      dAOV = dAOV_; dRoll = dRoll_;
      bFlShowCone = bFlShowCone_;
    }

  virtual int isA() const { return FT_Camera; }

  virtual void DeleteControls() {};
  virtual LPSTR GetNameStr() const { return "Camera"; }
 };



class _EXPORT2 TFaceItem: public Object
 {
public:
   int Status;
   PTFaceData pFData;

   TFaceItem( int resIdNorm, int resIdHilite, HINSTANCE hInst, PTFaceData pFD );
   ~TFaceItem();

   virtual classType isA() const { return TYP_FACE_ITEM; }
   virtual hashValueType hashValue() const { return 0; }
   virtual int isEqual( const Object& ) const;
   virtual char *nameOf() const { return "FaceItem"; }
   virtual void printOn( ostream& ) const {};

   HBITMAP GetHandleN() const { return hbmN; }
   HBITMAP GetHandleH() const { return hbmH; }

private:
  HBITMAP hbmN, hbmH;
  HINSTANCE hInst;
 };



class _EXPORT2 TMyScrollBar: public TScrollBar
 {
public:
  TMyScrollBar( PTWindowsObject AParent, int AnId, int X, int Y, int W, int H, BOOL IsHScrollBar,
    PTModule  AModule = NULL ):
    TScrollBar( AParent, AnId, X, Y, W, H, IsHScrollBar, AModule )
    {
    }


  void SetPosition_(int ThumbPos);

protected:
  virtual void SBThumbTrack(RTMessage Msg) = [NF_FIRST + SB_THUMBTRACK];

 };

		  ///TDialog
class _EXPORT2 TFaceDialog: public TCenterDialog
 {
public:
  TFaceDialog( PTWindowsObject AParent, DoubleList *pList,
    FaceTypes *pSelect, PTModule AModule = NULL );

  virtual ~TFaceDialog();
  //virtual void CloseWindow( int ARetValue );
  virtual void ShutDownWindow();
  virtual void ShutDownWindow( int ARetValue );
  virtual void DefWndProc( RTMessage msg );

  virtual void DrawItem( DRAWITEMSTRUCT _FAR & DrawInfo );

protected:
  //virtual void WMCreate(RTMessage Msg) = [WM_FIRST + WM_CREATE];
  virtual void SetupWindow();
  virtual void GetWindowClass( WNDCLASS FAR & AWndClass );
  virtual LPSTR GetClassName();
  //void WMCreate( RTMessage ) = [WM_FIRST + WM_CREATE];

private:
  DoubleList *pList;
  //int  mode;

  FaceTypes  *pSelIndex;

  HFONT hf;

  void ReturnSelectAndDestroy();
 };

#endif

