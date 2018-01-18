#if !defined(_MYMDICHILD_)
#define _MYMDICHILD_

#include <owl.h>
#include <bwindow.h>

#include "autorc.hpp"
#include "indicat.hpp"
#include "windiv.hpp"
#include "fmaneger.hpp"
#include "ucs.hpp"
#include "scena.hpp"
#include "facedial.hpp"
#include "polygon.hpp"
#include "drawbuf.hpp"
#include "seqv.hpp"
#include "log.hpp"
#include <list.h>
//#include "fmaneger.hpp"
#include "INC\menu.inc"
#include "INC\classtyp.inc"
#include "INC\mdicurs.inc"
#include <dialog.h>

#define NUMB_SYS_COMMANDS 6

#define NUMB_MODES 26
enum ModeChild { MC_Default, MC_SetupUCS, MC_ShiftUCS, MC_CreCylinder,
  MC_CreCone, MC_CreSphere, MC_CreTorus, MC_CreTube, MC_GetWindow,
  MC_Select, MC_MoveBody, MC_DeleteBody, MC_3DScaleBody,
  MC_2DScaleBody, MC_RotateBody, MC_SelectBody, MC_GetPoint,
  MC_SetupGlobalBP, MC_AligmentVertex, MC_AligmentFacet,
  MC_SelectFacet,
  MC_CreateOmni,
  MC_OmniAdjust,
  MC_VPortAligment,
  MC_PlaceHilite,
  MC_Smoth,
  MC_End
 };



/*enum RotExis { AX_x, AX_y, AX_z };
enum TScaleAxis { SA_3d, SA_x, SA_y, SA_z };
enum TMoveAxis { MA_xy. MA_x, MA_y, MA_z };*/

_CLASSDEF(TMyMDIChild)
_CLASSDEF(TMDIChSave)
_CLASSDEF(TMDIChFlagHolder)



typedef void (*TCurMouseHandler)( RTMessage, PTMyMDIChild, HDC );
typedef void (*TCurMouseLButtonHandler)( RTMessage, PTMyMDIChild );
typedef void (*TCurMouseRButtonHandler)( RTMessage, PTMyMDIChild );

/*static void DefMouseHandler( RTMessage, PTMyMDIChild, HDC );
static void DefMouseLBHandler( RTMessage, PTMyMDIChild );
static void DefMouseRBHandler( RTMessage, PTMyMDIChild );

void UCSSetupMouseHandler( RTMessage, PTMyMDIChild, HDC );
void UCSSetupMouseLBHandler( RTMessage, PTMyMDIChild );
void UCSSetupMouseRBHandler( RTMessage, PTMyMDIChild );

void UCSShiftMouseHandler( RTMessage, PTMyMDIChild, HDC );
void UCSShiftMouseLBHandler( RTMessage, PTMyMDIChild );
void UCSShiftMouseRBHandler( RTMessage, PTMyMDIChild );

void CreCylinderMouseHandler( RTMessage, PTMyMDIChild, HDC );
void CreCylinderMouseLBHandler( RTMessage, PTMyMDIChild );
void CreCylinderMouseRBHandler( RTMessage, PTMyMDIChild );

static void CreSphereMouseHandler( RTMessage, PTMyMDIChild, HDC );
static void CreSphereMouseLBHandler( RTMessage, PTMyMDIChild );
static void CreSphereMouseRBHandler( RTMessage, PTMyMDIChild );

static void CreTorusMouseHandler( RTMessage, PTMyMDIChild, HDC );
static void CreTorusMouseLBHandler( RTMessage, PTMyMDIChild );
static void CreTorusMouseRBHandler( RTMessage, PTMyMDIChild );

static void CreTubeMouseHandler( RTMessage, PTMyMDIChild, HDC );
static void CreTubeMouseLBHandler( RTMessage, PTMyMDIChild );
static void CreTubeMouseRBHandler( RTMessage, PTMyMDIChild );

static void GetWinMouseHandler( RTMessage, PTMyMDIChild, HDC );
static void GetWinMouseLBHandler( RTMessage, PTMyMDIChild );
static void GetWinMouseRBHandler( RTMessage, PTMyMDIChild );

static void SelectMouseHandler( RTMessage, PTMyMDIChild, HDC );
static void SelectMouseLBHandler( RTMessage, PTMyMDIChild );
static void SelectMouseRBHandler( RTMessage, PTMyMDIChild );


static void MoveMouseHandler( RTMessage, PTMyMDIChild, HDC );
static void MoveMouseLBHandler( RTMessage, PTMyMDIChild );
static void MoveMouseRBHandler( RTMessage, PTMyMDIChild );

static void DelMouseHandler( RTMessage, PTMyMDIChild, HDC );
static void DelMouseLBHandler( RTMessage, PTMyMDIChild );
static void DelMouseRBHandler( RTMessage, PTMyMDIChild );

static void Sc3MouseHandler( RTMessage, PTMyMDIChild, HDC );
static void Sc3MouseLBHandler( RTMessage, PTMyMDIChild );
static void Sc3MouseRBHandler( RTMessage, PTMyMDIChild );

static void Sc2MouseHandler( RTMessage, PTMyMDIChild, HDC );
static void Sc2MouseLBHandler( RTMessage, PTMyMDIChild );
static void Sc2MouseRBHandler( RTMessage, PTMyMDIChild );

static void RotMouseHandler( RTMessage, PTMyMDIChild, HDC );
static void RotMouseLBHandler( RTMessage, PTMyMDIChild );
static void RotMouseRBHandler( RTMessage, PTMyMDIChild );
  */
/*static void SelectBodyMouseHandler( RTMessage, PTMyMDIChild, HDC );
static void SelectBodyMouseLBHandler( RTMessage, PTMyMDIChild );
static void SelectBodyMouseRBHandler( RTMessage, PTMyMDIChild );
  */
  /*
static void GetPointMouseHandler( RTMessage, PTMyMDIChild, HDC );
static void GetPointMouseLBHandler( RTMessage, PTMyMDIChild );
static void GetPointMouseRBHandler( RTMessage, PTMyMDIChild );

static void SelectFacetMouseHandler( RTMessage, PTMyMDIChild, HDC );
static void SelectFacetMouseLBHandler( RTMessage, PTMyMDIChild );
static void SelectFacetMouseRBHandler( RTMessage, PTMyMDIChild );

static void SpecialFitToWindow( PTMyMDIChild, RECT& );

static void SpecialSelectWF( PTMyMDIChild, RECT& );

static void SetCriticalLockInstance( Pvoid pCh, Pvoid pDta );
    */

//_CLASSDEF(TModeHand)
struct TModeHand
 {
  TCurMouseHandler        tcmhHandler;
  TCurMouseLButtonHandler tcmLBHand;
  TCurMouseRButtonHandler tcmRBHand;
  HCURSOR                 cursor;
 };

_CLASSDEF(TCurs)
class  TCurs: public Object
 {
public:
   int Status;
   int id;
   static HINSTANCE hinst;

   TCurs( int id, BOOL bFlDelete );
   TCurs() { Status = id = 0; hc = NULL; bFlDel = FALSE; }

   ~TCurs();

   virtual classType isA() const { return 110; }
   virtual hashValueType hashValue() const { return 0; }
   virtual int isEqual( const Object& ) const;
   virtual char *nameOf() const { return "Cursor"; }
   virtual void printOn( ostream& ) const {};

   HCURSOR GetHendl() const { return hc; }
 
private:
  HCURSOR hc;
  BOOL bFlDel;
 };

class  TMDIChSave
 {
public:
  BOOL bFlGr, bFlUCS, bFlSnap;
  BOOL bFlVisScene;
  double dKeyAngleH, dKeyAngleV;

  double scale; //for base UCS
  double shiftX, shiftY;

  TViewPoint vpKeyView;
  ModeChild mcMode;

  int phazeOfCurMode_;

  T3DPoint globalBasePoint;
  BOOL  bFlIsBasePointVisible;

  TMDIChSave() {};
  TMDIChSave( RTMyMDIChild rCh );
  Restore( RTMyMDIChild rCh, BOOL bFlNoChangeMode = FALSE );
 };

typedef void (*TCallAfterProc)( PTMyMDIChild, RECT& );

				 //TWindow
class  TMyMDIChild: public TBWindow, public TUCS
 {



friend void DefMouseHandler( RTMessage, PTMyMDIChild, HDC );
friend void DefMouseLBHandler( RTMessage, PTMyMDIChild );
friend void DefMouseRBHandler( RTMessage, PTMyMDIChild );

friend void UCSSetupMouseHandler( RTMessage, PTMyMDIChild, HDC );
friend void UCSSetupMouseLBHandler( RTMessage, PTMyMDIChild );
friend void UCSSetupMouseRBHandler( RTMessage, PTMyMDIChild );

friend void UCSShiftMouseHandler( RTMessage, PTMyMDIChild, HDC );
friend void UCSShiftMouseLBHandler( RTMessage, PTMyMDIChild );
friend void UCSShiftMouseRBHandler( RTMessage, PTMyMDIChild );

friend void CreCylinderMouseHandler( RTMessage, PTMyMDIChild, HDC );
friend void CreCylinderMouseLBHandler( RTMessage, PTMyMDIChild );
friend void CreCylinderMouseRBHandler( RTMessage, PTMyMDIChild );

friend void CreSphereMouseHandler( RTMessage, PTMyMDIChild, HDC );
friend void CreSphereMouseLBHandler( RTMessage, PTMyMDIChild );
friend void CreSphereMouseRBHandler( RTMessage, PTMyMDIChild );

friend void CreTorusMouseHandler( RTMessage, PTMyMDIChild, HDC );
friend void CreTorusMouseLBHandler( RTMessage, PTMyMDIChild );
friend void CreTorusMouseRBHandler( RTMessage, PTMyMDIChild );

friend void CreTubeMouseHandler( RTMessage, PTMyMDIChild, HDC );
friend void CreTubeMouseLBHandler( RTMessage, PTMyMDIChild );
friend void CreTubeMouseRBHandler( RTMessage, PTMyMDIChild );

friend void GetWinMouseHandler( RTMessage, PTMyMDIChild, HDC );
friend void GetWinMouseLBHandler( RTMessage, PTMyMDIChild );
friend void GetWinMouseRBHandler( RTMessage, PTMyMDIChild );

friend void SelectMouseHandler( RTMessage, PTMyMDIChild, HDC );
friend void SelectMouseLBHandler( RTMessage, PTMyMDIChild );
friend void SelectMouseRBHandler( RTMessage, PTMyMDIChild );

friend void MoveMouseHandler( RTMessage, PTMyMDIChild, HDC );
friend void MoveMouseLBHandler( RTMessage, PTMyMDIChild );
friend void MoveMouseRBHandler( RTMessage, PTMyMDIChild );


friend void DelMouseHandler( RTMessage, PTMyMDIChild, HDC );
friend void DelMouseLBHandler( RTMessage, PTMyMDIChild );
friend void DelMouseRBHandler( RTMessage, PTMyMDIChild );

friend void Sc3MouseHandler( RTMessage, PTMyMDIChild, HDC );
friend void Sc3MouseLBHandler( RTMessage, PTMyMDIChild );
friend void Sc3MouseRBHandler( RTMessage, PTMyMDIChild );

friend void Sc2MouseHandler( RTMessage, PTMyMDIChild, HDC );
friend void Sc2MouseLBHandler( RTMessage, PTMyMDIChild );
friend void Sc2MouseRBHandler( RTMessage, PTMyMDIChild );

friend void RotMouseHandler( RTMessage, PTMyMDIChild, HDC );
friend void RotMouseLBHandler( RTMessage, PTMyMDIChild );
friend void RotMouseRBHandler( RTMessage, PTMyMDIChild );


friend void GetPointMouseHandler( RTMessage, PTMyMDIChild, HDC );
friend void GetPointMouseLBHandler( RTMessage, PTMyMDIChild );
friend void GetPointMouseRBHandler( RTMessage, PTMyMDIChild );

friend void SelectFacetMouseHandler( RTMessage, PTMyMDIChild, HDC );
friend void SelectFacetMouseLBHandler( RTMessage, PTMyMDIChild );
friend void SelectFacetMouseRBHandler( RTMessage, PTMyMDIChild );


friend void SpecialFitToWindow( PTMyMDIChild, RECT& );

friend void SpecialSelectWF( PTMyMDIChild, RECT& );

friend void SetCriticalLockInstance( Pvoid pCh, Pvoid pDta );

friend class TMDIChSave;

public:
  enum TtlType { NoShowCopy, ShowCopy };

  struct TKeyMode {
    ModeChild mcMode;
    BOOL      enableKey;    
 };



  TMyMDIChild( PTWindowsObject AParent, PTMyRect pRect, PTScena pScena, TViewPoint vpView,
    int iDivIndex, BOOL bFlVisible = TRUE, PTModule AModule = NULL );

  virtual ~TMyMDIChild();

  virtual BOOL CanClose();
  BOOL CanClose( int );
  virtual void ShutDownWindow();

  virtual void ChangeVPoint( TViewPoint vpView, double angleH = 0, double angleV = 0 );

  int GetScId() { return pScena->GetId(); }
  void GenTitle( TFileInfo *fiInfo );
  void SetTitleType( TtlType t ) { ttFlTitleType = t; }
  BOOL GetPhaze() { return bFlPhaze; }

  PTScena GetScena() const { return pScena; }
  virtual void SetupWindow();
  virtual void DefWndProc( RTMessage Msg );

  void EnableShowInstance( Pvoid pCh, Pvoid id );

  void ScaleValueBaseToUser( double& );
  void ScaleValueBaseToUser( int& );

  BOOL ToggleGreed();
  BOOL ToggleUCS();
  BOOL ToggleSnap();
  BOOL IsGreedVis() const { return GetGridState(); }
  BOOL IsUCSVis() const { return GetVisState(); }
  BOOL IsSnapUsed() const { return GetSnapState(); }

  BOOL IsMyCommand( UINT com ) const;

  void LockForReenter( BOOL bFlLock ) { bFlLockReenter = bFlLock; }
  BOOL GetLockReenter() const { return bFlLockReenter; }

  BOOL AskBitmap() const { return  pbmpDIB ? TRUE:FALSE; }
  void SaveBmp() const;

  void ToggleGeometry();
  void ToggleFullDetal();
  BOOL IsHideGeometry() const { return pScena->GetHideGeom(); }
  BOOL IsFullDetal() const { return pScena->GetDetal(); }

  void SetGridExtents();
  void UserUCSSetup();
  void UserUCSShift();
  void FitToZoom();
  void FitToWindow();

  void SelectWithWindowFrame();

  void CreateCylinder( PTFaceData pFD );
  void CreateSphere( PTFaceData pFD );
  void CreateTorus( PTFaceData pFD );
  void CreateTube( PTFaceData pFD );

  void SetFilter( ModeChild mcM );
  ModeChild SetModeChild( ModeChild mcM );
  ModeChild GetModeChild() const { return TMyMDIChild::mcKeyMode; }

  int GetDivIndex() const { return iDivIndex; }



  virtual classType isA() const { return classtypeMDIChWindow; }

  void RedrawVports();

  BOOL ValidWindowOper( ModeChild ) const;
  void FillInfoDlg( List& );

  void SelectAndOp( ModeChild );
  void GetPointAndOp( ModeChild );
  void GetFacetAndOp( ModeChild );

  void CreationAligment( PTSurface pSf );
  int Aligment( RTSurface rSf, RTVector rVect, RTVertex rV, PTFacet pF, BOOL bFlReqv = TRUE );
  int AligmentVertex( POINT _FAR &pt, BOOL bFlReqv = TRUE );
  int CreateOmni( POINT _FAR &pt );
  void OmniAdjust( PTSurface );
  void AligmentFacet( RTSurface, RTFacet, BOOL bFlReqv = TRUE );
  void Smoth( RTSurface, RTFacet );
  void VPortAligment( RTSurface, RTFacet );
  void SetupGlobalBP( POINT _FAR &pt );  
  void PlaceHilite( POINT pt, RTSurface rOmni );
  PTVertexData GetNearstVertex( LPPOINT );
  //void SetCurs( HCURSOR hc ) { curs = hc; }

  int GetNextMode() const { return mcNextOp; }

  void RenderView( BOOL bFlPhoto = FALSE );
  void RenderPhoto();

  int Save( HFILE hf );
  int Load( HFILE hf );
  int CalcSelf();

  static ModeChild GetMode_();// { return mcMode; }

  void WMQueryNewPalette( RTMessage ); //calling by up level WMQueryNewPalette
  void WMPaletteChanged( RTMessage ); //calling by up level WMPaletteChanged

  void WMNCPaint( RTMessage ) = [WM_FIRST + WM_NCPAINT];

  int GetButtonId() const;

  BOOL ShowBitmap() const { return pbmpDIB ? TRUE:FALSE; }

  static BOOL IsTransformSelected();
  static BOOL IsTransformAsTransforming();
  static void SetTransformSelected( BOOL );
  static void SetTransformAsTransforming( BOOL );

  static BOOL UsedGlobalPoint();
  static void SetUsedPoint( BOOL bFl );

  T3DPoint GetBP( PTSurface pSf = NULL ) const;

  static BOOL UsedCrossing();
  static void SetUsedCrossing( BOOL bFl );

  void SetCriticalLock( BOOL bFl );
  BOOL GetCriticalLock() const { return pScena->GetCriticalLock(); }

  virtual void ActivationResponse( WORD Activated,  BOOL IsIconified );

  virtual BOOL Create();

  PTDIB GetDIB() const { return pbmpDIB; }

protected:  
  //HBITMAP  hbmRender;

  static ModeChild mcKeyMode;
  static ModeChild mcKeyNextMode;


  BOOL bFlLBDown, bFlRBDown;
  BOOL bFlRefresh;
  static int cActivate;
  static BOOL bFlUserUCSSetupSta;
  TMDIChSave  mcsSave;
  HDC hdcOwn;

  static PTFaceData  pfdFace;

  static int phazeOfCurMode;
  BOOL bFlModeActive;

  static TCallAfterProc  callAfter;

static BOOL bFlFitActive, bFlSelectWithWindowFrameActive;

/*____________________ Shift process data____________________*/
static BOOL bFlShiftActive;
static int staXSh, staYSh, curXSh, curYSh;
/*___________________________________________________________*/

/*____________________ Select data ____________________*/
  static ModeChild mcNextOp, mcNextOp2;
  static PTSurface  psfSelected;
  static PTVertex   pvSelected;
  static PTFacet pfacSelected;
  static POINT ptSelected;
  void InitMode( ModeChild );
  static PTObol psfObol;
  static PTScena pscObol;

  static DoubleList dblCommonFacets;
  static DoubleListIterator dbiFacets;
/*_____________________________________________________*/

/*____________________ Move data ____________________*/
  static double dDX, dDY, dDZ;
  //static TScaleAxis saScaleAxis;
  static TMoveAxisSeqvensor masMoveAxis;
  static void SetMove( int dx, int dy );
/*_____________________________________________________*/


/*____________________ Scale data ____________________*/
  static double dScaleX, dScaleY, dScaleZ;
  //static TScaleAxis saScaleAxis;
  static TScaleAxisSeqvensor sasScaleAxis;
  //LPSTR GetScaleExisLetter();
  void ScaleFunc( RTSurface rSurf );
  static void SetScale( double delta );
  static double GetScaleSys();
/*_____________________________________________________*/

/*____________________ Rotate data ____________________*/
  static double anX, anY, anZ;
  //static RotExis reCurExis;
  static TRotExisSeqvensor resCurExis;
  void RotFunc( RTSurface rSurf );
  static void SetAngle( double delta );
  //LPSTR GetExisLetter();
  static double GetAngle();
/*_____________________________________________________*/

  virtual LPSTR GetClassName();      //TMDIFrame   
  virtual void GetWindowClass( WNDCLASS & AWndClass );
  //virtual void WMCreate( RTMessage ) = [WM_FIRST + WM_CREATE];
  virtual void Paint( HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo );
  virtual void WMSetFocus(RTMessage Msg) = [WM_FIRST + WM_SETFOCUS];
  virtual void WMKillFocus(RTMessage Msg) = [WM_FIRST + WM_KILLFOCUS];
  void WMMouseMove( RTMessage msg ) = [WM_FIRST + WM_MOUSEMOVE];
  void WMLButtonDown( RTMessage ) = [WM_FIRST + WM_LBUTTONDOWN];
  void WMRButtonDown( RTMessage ) = [WM_FIRST + WM_RBUTTONDOWN];
  void WMLButtonUp( RTMessage ) = [WM_FIRST + WM_LBUTTONUP];
  void WMEraseBkGnd( RTMessage ) = [WM_FIRST + WM_ERASEBKGND];
  void WMKeyDown( RTMessage ) = [WM_FIRST + WM_KEYDOWN];
  virtual void WMSize( RTMessage ) = [WM_FIRST + WM_SIZE];

  void WMSetCursor( RTMessage ) = [WM_FIRST + WM_SETCURSOR];

  void RequiestCallMouseHandl() { bFlRequiestCallMouseHandl = TRUE; }

  void SetCursorInCenter( PTObol pSf );

  PTScena GetClone() const;

private:
  //HCURSOR   curs;

  BOOL bFlLockReenter;

  static ModeChild mcMode;

  static TModeHand mhTable[];

  static TKeyMode kmTbl[ NUMB_MODES ];

  static TCurMouseHandler tcmhHandler;
  static TCurMouseLButtonHandler tcmLBHand;
  static TCurMouseRButtonHandler tcmRBHand;
  static int cursorInit[ NUMB_MODES ];
  static List      listCurs;

  static BOOL bFlTransformSelected;
  static BOOL bFlTransformAsTransforming;

  static BOOL bFlUseGlobalBasePoint;
  static BOOL bFlUseCrossing; 


  BOOL bFlRequiestCallMouseHandl;

  TtlType     ttFlTitleType;
  PTScena     pScena;
  BOOL        bFlHaveFocus;

  static BOOL GetModeEnblKeyFlag( ModeChild );
  void SetupMenuState();

  BOOL ScrollForShow( int x, int y, BOOL bFlSetCursor = FALSE );

  BOOL  bFlPhaze;  //FALSE if window shell destroy

  int  iDivIndex;
  BOOL bAutoFit;
  BOOL bFlPopupAllViews;

  PTDIB  pbmpDIB;
  HPALETTE hpalAlias;

  HRGN  hrgExt, hrgInt, hrgTtl, hrgNC, hrgTtlAbs;
  HBITMAP  hbmMask;
  HBRUSH   hbrMask, hbrMask2;
  HBITMAP  hbmSys, hbmZoom, hbmReduce;

  BOOL bFlNCActivated;

  static TDrawTitleBuf dtbBuf;
  static TCurs tcDisableCursor;
  static TCurs tcHand;

  static PTClassFilterFunc pCurFilter;

  int iHitTest;

  BOOL bFlShowOnCreate;

  static classType ctActionClass;

  int iBmpWidth, iBmpHigh;
  RECT rBmpView;

  void SaveLog( RTimeLog rtlLog, BOOL bFlPhoto );

  void SetupRegions();
  void Mask( HDC hdc, RECT _FAR &r );

  BOOL IsZoomedRectSelfAndThis( WINDOWPOS _FAR *lpWp ) const;
  BOOL IsIconicRectSelfAndThis( WINDOWPOS _FAR *lpWp ) const;

  BOOL IsDisabledZone( int iHitCode ) const;

  void EnableInSysMenu( BOOL );
  void DrawArrow( HDC hdc, int iX1, int iY1, int iX2, int iY2 );
  void RotateKbUCS( WPARAM wp );

  BOOL ReturnCursor( int x, int y );
  BOOL ScrollForShowBody( RTObol rOd );

  TAutoGDIRc<HRGN> grcRegErase, grcRegClient, grcRegBitmap;
  static TAutoGDIRc<HBRUSH> grcBrushFon;
 };

typedef void (TMyMDIChild::* _FAR PMDIChildFlagFunc)( BOOL );

class TMDIChFlagHolder
 {
public:
   TMDIChFlagHolder( PTMyMDIChild pObj, PMDIChildFlagFunc pMemFunc, BOOL bHolValue );
   ~TMDIChFlagHolder();

   void InvalidObject() { pCh = NULL; }

private:
  PTMyMDIChild pCh;
  PMDIChildFlagFunc pFunc;
  BOOL bHold;

 };

#endif
