//#pragma option -v-

#if !defined(OEMRESOURCE)
#define OEMRESOURCE
#endif

#include "smothdlg.hpp"
#include "threshol.hpp"
#include "mymdich.hpp"
#include "mymdi.hpp"
#include "ync.hpp"
#include "INC\img256.inc"
#include "INC\menu.inc"
#include "INC\dlg.inc"
#include "INC\icon.inc"
#include "INC\mdchbmp.inc"
#include "grext.hpp"
#include "status.hpp"
#include "scena.hpp"
#include "bpdlg.hpp"
#include "fheader.hpp"
#include "aligndlg.hpp"
#include "omnidl.hpp"
#include "hilitedl.hpp"
#include "autorc.hpp"
#include "fullscr.hpp"
#include "INC\io_em.inc"

#include <bwcc.h>
#include <dir.h>
#include <stdio.h>

extern HINSTANCE hInstRC;
extern PTModule pMRC;

extern BOOL bFlLogging;

TCurs::~TCurs()
 {
   //if( bFlDel && hc ) DestroyCursor( hc );
   if( hc ) DestroyCursor( hc );
   hc = NULL;
 }

int TCurs::isEqual( const Object& rObj ) const
 {
   if( (TCurs huge *)this != (TCurs huge *)(&rObj) ) return FALSE;
   return TRUE;
 }

TCurs TMyMDIChild::tcDisableCursor,
      TMyMDIChild::tcHand;

TDrawTitleBuf TMyMDIChild::dtbBuf;

classType TMyMDIChild::ctActionClass = TYP_FACE;

DoubleList TMyMDIChild::dblCommonFacets;
DoubleListIterator TMyMDIChild::dbiFacets( TMyMDIChild::dblCommonFacets );


PTClassFilterFunc TMyMDIChild::pCurFilter = FilterBody;

TAutoGDIRc<HBRUSH> TMyMDIChild::grcBrushFon;

List TMyMDIChild::listCurs;
PTFaceData  TMyMDIChild::pfdFace;
ModeChild TMyMDIChild::mcMode = MC_Default;
int TMyMDIChild::phazeOfCurMode;
ModeChild TMyMDIChild::mcKeyMode = MC_Default,
	  TMyMDIChild::mcKeyNextMode = MC_Default;
ModeChild TMyMDIChild::mcNextOp = MC_Default,
	  TMyMDIChild::mcNextOp2 = MC_Default;
double TMyMDIChild::dScaleX = 100, TMyMDIChild::dScaleY = 100,
       TMyMDIChild::dScaleZ = 100;
double TMyMDIChild::dDX = 0, TMyMDIChild::dDY = 0, TMyMDIChild::dDZ = 0;
double TMyMDIChild::anX = 0, TMyMDIChild::anY = 0, TMyMDIChild::anZ = 0;
//RotExis TMyMDIChild::reCurExis = AX_x;

TScaleAxisSeqvensor TMyMDIChild::sasScaleAxis;
TRotExisSeqvensor TMyMDIChild::resCurExis;
TMoveAxisSeqvensor TMyMDIChild::masMoveAxis;

//TScaleAxis TMyMDIChild::saScaleAxis = SA_3d;

PTFacet    TMyMDIChild::pfacSelected = NULL;
PTSurface  TMyMDIChild::psfSelected = NULL;
PTVertex   TMyMDIChild::pvSelected = NULL;
PTObol     TMyMDIChild::psfObol = NULL;
PTScena    TMyMDIChild::pscObol = NULL;
POINT      TMyMDIChild::ptSelected = {0, 0};

BOOL TMyMDIChild::bFlSelectWithWindowFrameActive = FALSE;
BOOL TMyMDIChild::bFlFitActive = FALSE;
BOOL TMyMDIChild::bFlShiftActive = FALSE;
BOOL TMyMDIChild::bFlUserUCSSetupSta;

BOOL TMyMDIChild::bFlTransformSelected = FALSE;
BOOL TMyMDIChild::bFlTransformAsTransforming = TRUE;

BOOL TMyMDIChild::bFlUseGlobalBasePoint = FALSE;

BOOL TMyMDIChild::bFlUseCrossing = FALSE;
//BOOL TMyMDIChild::bFlRefresh;

int TMyMDIChild::cActivate = 1;

TCallAfterProc  TMyMDIChild::callAfter = NULL;

TCurMouseHandler TMyMDIChild::tcmhHandler = DefMouseHandler;
TCurMouseLButtonHandler TMyMDIChild::tcmLBHand = DefMouseLBHandler;
TCurMouseRButtonHandler TMyMDIChild::tcmRBHand = DefMouseRBHandler;


int TMyMDIChild::cursorInit[ NUMB_MODES ] =
 {
   CSR_Default,
   CSR_SetupUCS,
   CSR_ShiftUCS,
   CSR_CreCylinder,
   CSR_Cone,
   CSR_Sphere,
   CSR_Torus,
   CSR_Tube,

   CSR_GetWindow,

   CSR_SelectObj,
   CSR_MoveObj,
   CSR_DelObj,
   CSR_3DScaleObj,
   CSR_2DScaleObj,
   CSR_RotateBody,
   CSR_SelectBody,

   CSR_GetPoint,
   CSR_SetupGlobalBP,
   CSR_Default,      //MC_AligmentVertex
   CSR_Default,      //MC_AligmentFacet
   CSR_Default,

   CSR_Omni,
   CSR_Default,
   CSR_Default,
   CSR_Default,
   CSR_Default
 };

TModeHand TMyMDIChild::mhTable[ NUMB_MODES ] =
 {
   {DefMouseHandler, DefMouseLBHandler, DefMouseRBHandler, NULL},
   {UCSSetupMouseHandler, UCSSetupMouseLBHandler, UCSSetupMouseRBHandler, NULL},
   {UCSShiftMouseHandler, UCSShiftMouseLBHandler, UCSShiftMouseRBHandler, NULL},
   {CreCylinderMouseHandler, CreCylinderMouseLBHandler, CreCylinderMouseRBHandler, NULL},
   {CreCylinderMouseHandler, CreCylinderMouseLBHandler, CreCylinderMouseRBHandler, NULL},
   {CreSphereMouseHandler, CreSphereMouseLBHandler, CreSphereMouseRBHandler},
   {CreTorusMouseHandler, CreTorusMouseLBHandler, CreTorusMouseRBHandler},
   {CreTubeMouseHandler, CreTubeMouseLBHandler, CreTubeMouseRBHandler},

   {GetWinMouseHandler, GetWinMouseLBHandler, GetWinMouseRBHandler},

   {SelectMouseHandler, SelectMouseLBHandler, SelectMouseRBHandler},
   {MoveMouseHandler, MoveMouseLBHandler, MoveMouseRBHandler},
   {DelMouseHandler, DelMouseLBHandler, DelMouseRBHandler},
   {Sc3MouseHandler, Sc3MouseLBHandler, Sc3MouseRBHandler},
   {Sc2MouseHandler, Sc2MouseLBHandler, Sc2MouseRBHandler},
   {RotMouseHandler, RotMouseLBHandler, RotMouseRBHandler},

   //{SelectBodyMouseHandler, SelectBodyMouseLBHandler, SelectBodyMouseRBHandler},
   {NULL, NULL, NULL},

   {GetPointMouseHandler, GetPointMouseLBHandler, GetPointMouseRBHandler},

   {NULL, NULL, NULL},
   {NULL, NULL, NULL},
   {NULL, NULL, NULL},
   {SelectFacetMouseHandler, SelectFacetMouseLBHandler, SelectFacetMouseRBHandler},

   {NULL, NULL, NULL},
   {NULL, NULL, NULL},
   {NULL, NULL, NULL},
   {NULL, NULL, NULL},
   {NULL, NULL, NULL}
   //and cone
 };



TMyMDIChild::TKeyMode TMyMDIChild::kmTbl[ NUMB_MODES ] =
 {
   {MC_Default, TRUE},
   {MC_SetupUCS, FALSE},
   {MC_ShiftUCS, TRUE},
   {MC_CreCylinder, TRUE},

   {MC_CreCone, TRUE},
   {MC_CreSphere, TRUE},
   {MC_CreTorus, TRUE},
   {MC_CreTube, TRUE},

   {MC_GetWindow, TRUE},
   {MC_Select, TRUE},
   {MC_MoveBody, TRUE},
   {MC_DeleteBody, TRUE},

   {MC_3DScaleBody, TRUE},
   {MC_2DScaleBody, TRUE},
   {MC_RotateBody, TRUE},
   {MC_SelectBody, TRUE},

   {MC_GetPoint, FALSE},
   {MC_SetupGlobalBP, FALSE},
   {MC_AligmentVertex, FALSE},
   {MC_AligmentFacet, FALSE},
   {MC_SelectFacet, FALSE},

   {MC_CreateOmni, TRUE},
   {MC_OmniAdjust, TRUE},

   {MC_VPortAligment, FALSE},

   {MC_PlaceHilite, FALSE},
   {MC_Smoth, FALSE}
 };

BOOL TMyMDIChild::GetModeEnblKeyFlag( ModeChild mcM )
 {
   for( int i = 0; i < NUMB_MODES; i++ )
     if( TMyMDIChild::kmTbl[i].mcMode == mcM ) return TMyMDIChild::kmTbl[i].enableKey;

   return FALSE;
 }


extern PTFileManeger pGlobalFManeger;

static int findCursor( const Object& rObj, Pvoid pPar )
 {
   return  (((RTCurs)rObj).id == (int)pPar );
 }

HINSTANCE TCurs::hinst;

TCurs::TCurs( int id, BOOL bFlDelete )
 {
   this->id = id;
   if( id == -1 ) hc = LoadCursor( NULL, IDC_ARROW );
   else hc = LoadCursor( TCurs::hinst, MAKEINTRESOURCE(id) );
   /*
   hc = LoadCursor( TCurs::hinst,
     ((id == -1) ? IDC_ARROW:MAKEINTRESOURCE(id)) );*/

   bFlDel = (id == -1) ? FALSE:bFlDelete;
   if( !hc ) Status = ER_CURSORLOAD;
   else Status = 0;
 }

ModeChild TMyMDIChild::GetMode_() { return TMyMDIChild::mcMode; }

BOOL TMyMDIChild::IsTransformSelected()
 { return TMyMDIChild::bFlTransformSelected; }

BOOL TMyMDIChild::IsTransformAsTransforming()
 { return TMyMDIChild::bFlTransformAsTransforming; }

void TMyMDIChild::SetTransformSelected( BOOL bFl )
 { TMyMDIChild::bFlTransformSelected = bFl; }

void TMyMDIChild::SetTransformAsTransforming( BOOL bFl )
 { TMyMDIChild::bFlTransformAsTransforming = bFl; }


BOOL TMyMDIChild::UsedGlobalPoint()
 {
   return bFlUseGlobalBasePoint;
 }

void TMyMDIChild::SetUsedPoint( BOOL bFl )
 {
   bFlUseGlobalBasePoint = bFl;
 }

BOOL TMyMDIChild::UsedCrossing()
 { return TMyMDIChild::bFlUseCrossing; }

void TMyMDIChild::SetUsedCrossing( BOOL bFl )
 { TMyMDIChild::bFlUseCrossing = bFl; }


int TMyMDIChild::GetButtonId() const
 {
   if( (void huge *)(TMyMDIChild::callAfter) == (void huge *)SpecialFitToWindow )
     return UCM_TB_FIT_TO_WINDOW;
   else if( (void huge *)(TMyMDIChild::callAfter) == (void huge *)SpecialSelectWF )
     return UCM_TB_SELECTWINDOW;
   else return -1;
 }


TMyMDIChild::TMyMDIChild( PTWindowsObject AParent, PTMyRect pRect,
PTScena pScena, TViewPoint vpView, int iDivIndex, BOOL bFlVisible, PTModule AModule ):
  TBWindow( AParent, "", AModule ),
  TUCS( vpView ),
  grcRegErase( CreateRectRgn( 0, 0, 0, 0 ) ),
  grcRegClient( CreateRectRgn( 0, 0, 0, 0 ) ),
  grcRegBitmap( CreateRectRgn( 0, 0, 0, 0 ) )
 {
   TCurs::hinst = hInstRC;
   hpalAlias = NULL;

   bFlShowOnCreate = bFlVisible;
    
   iHitTest = HTCLIENT;

   bFlRefresh = TRUE;
   bFlLockReenter = FALSE;
   bFlRequiestCallMouseHandl = FALSE;
   //TMyMDIChild::mcMode = MC_Default;

   this->pScena = pScena;   
   ttFlTitleType = NoShowCopy;
   bFlPhaze = TRUE;
   Attr.X = pRect->left; Attr.Y = pRect->top;
   Attr.W = pRect->right - pRect->left;
   Attr.H = pRect->bottom - pRect->top;   

   bFlHaveFocus = FALSE;

   bFlLBDown = bFlRBDown = FALSE;


   hdcOwn = NULL;
   this->iDivIndex = iDivIndex;

   bFlShiftActive = FALSE;
   bFlModeActive = FALSE;

   //hbmRender = NULL;
   pbmpDIB = NULL;
   bAutoFit = FALSE;

   bFlPopupAllViews = FALSE;

   hrgExt = CreateRectRgn( 0, 0, 0, 0 );
   hrgInt = CreateRectRgn( 0, 0, 0, 0 );
   hrgTtl = CreateRectRgn( 0, 0, 0, 0 );
   hrgNC =  CreateRectRgn( 0, 0, 0, 0 );
   hrgTtlAbs = CreateRectRgn( 0, 0, 0, 0 );


   hbmMask = NULL;
   hbrMask = NULL;
   hbrMask2 = NULL;

   if( !TMyMDIChild::tcDisableCursor.GetHendl() )
     TMyMDIChild::tcDisableCursor = TCurs( CSR_OT_Disabled, TRUE );
   if( !TMyMDIChild::tcHand.GetHendl() )
     TMyMDIChild::tcHand = TCurs( CSR_OT_Hand, TRUE );

   bFlNCActivated = 2;

   if( !TMyMDIChild::grcBrushFon )
    {
      HBITMAP hbmBrush = LoadBitmap( hInstRC, MAKEINTRESOURCE(BKGNDBMP) );
      TMyMDIChild::grcBrushFon = CreatePatternBrush( hbmBrush );
      DeleteObject( hbmBrush );
    }
 };                                 


void TMyMDIChild::ChangeVPoint( TViewPoint vpView, double angleH, double angleV )
 {
   TUCS::ChangeVPoint( vpView, angleH, angleV );

   SetupMenuState();
 }

void TMyMDIChild::SetupMenuState()
 {
   HMENU hmenuSys = GetSystemMenu(HWindow, FALSE);
   UINT uiFl = (GetVpoint_() == TUCS::VP_Camera ||
		GetVpoint_() == TUCS::VP_None ||
		pbmpDIB ||
		GetCriticalLock() == TRUE) ? (MF_DISABLED | MF_GRAYED):MF_ENABLED;

   if( !hmenuSys ) return;

   EnableMenuItem( hmenuSys, UCM_RESTORE_VIEW, MF_BYCOMMAND |
     (pbmpDIB ? MF_ENABLED:(MF_DISABLED | MF_GRAYED)) );
   EnableMenuItem( hmenuSys, UCM_FULLSCREEN_BMP, MF_BYCOMMAND |
     (pbmpDIB ? MF_ENABLED:(MF_DISABLED | MF_GRAYED)) );

   EnableMenuItem( hmenuSys, UCM_RESTORE_CHILD_DIV, MF_BYCOMMAND |
     ((GetCriticalLock() == FALSE && !pbmpDIB) ? MF_ENABLED:(MF_DISABLED | MF_GRAYED)) );
   EnableMenuItem( hmenuSys, UCM_AUTO_FIT, MF_BYCOMMAND | uiFl );
   EnableMenuItem( hmenuSys, 4/*SC_MINIMIZE*/, MF_BYPOSITION | uiFl );
   EnableMenuItem( hmenuSys, 5/*SC_MAXIMIZE*/, MF_BYPOSITION | uiFl );
   EnableMenuItem( hmenuSys, 3/*SC_SIZE*/, MF_BYPOSITION | uiFl );


   if( IsZoomed(HWindow) == TRUE )
    {
      EnableMenuItem( hmenuSys, 5, MF_BYPOSITION | (MF_DISABLED | MF_GRAYED) );
      EnableMenuItem( hmenuSys, 3, MF_BYPOSITION | (MF_DISABLED | MF_GRAYED) );
    }

   if( IsIconic(HWindow) == TRUE )
    {
      EnableMenuItem( hmenuSys, 4,  MF_BYPOSITION | (MF_DISABLED | MF_GRAYED) );
      EnableMenuItem( hmenuSys, 3,  MF_BYPOSITION | (MF_DISABLED | MF_GRAYED) );
    }
 }

 LPSTR TMyMDIChild::GetClassName()
  {
    return "3DW:ChildWindow";
  }
                 //WM_INIT SetupWindow 
 void TMyMDIChild::GetWindowClass( WNDCLASS & AWndClass )//TApplication
  {
     TWindow::GetWindowClass( AWndClass );

     AWndClass.style |= CS_OWNDC;
     AWndClass.hbrBackground =  CreateSolidBrush( PALETTEINDEX(6) );
     AWndClass.hIcon = LoadIcon( hInstRC, MAKEINTRESOURCE(_MDICH_ICON) );
  }

BOOL TMyMDIChild::ValidWindowOper( ModeChild mc ) const
 {
   if( GetCriticalLock() == TRUE ) return FALSE;

   if( ((GetVpoint_() == TUCS::VP_Camera || GetVpoint_() == TUCS::VP_None) &&
	mcMode != MC_Default) ||
	AskBitmap() == TRUE
     )
    {
      TAutoStringRc str_( hInstRC, ID_MSG_6 );
      TAutoStringRc ttl_( hInstRC, ID_TITLE_ERROR );
      BWCCMessageBox( HWindow, str_(),
	ttl_(), MB_ICONSTOP | MB_OK | MB_APPLMODAL );
      //"Invalid viewport for that operation"
      return FALSE;
    }

   return TRUE;
 }

void RedrawInst( Pvoid pCh, Pvoid pId )
 {
   PTMyMDIChild  pChild = (PTMyMDIChild)pCh;
   int  id = (int)pId;

   if( pChild->isA() != classtypeMDIChWindow || pChild->GetScId() != id )
     return;
   
   InvalidateRect( pChild->HWindow, NULL, TRUE );
 }

void TMyMDIChild::RedrawVports()
 {
   GetApplication()->MainWindow->ForEach( RedrawInst, (Pvoid)GetScId() );
 }


BOOL TMyMDIChild::IsDisabledZone( int iHitCode ) const
 {
   int t1 = ( bFlHaveFocus == FALSE &&
	      (iHitCode == HTSYSMENU || iHitCode == HTMAXBUTTON ||
	       iHitCode == HTMINBUTTON)
	    );

   int t2 = ( (pbmpDIB || pScena->GetCriticalLock() == TRUE) &&
	      (iHitCode == HTZOOM ||
	       iHitCode == HTBOTTOM || iHitCode == HTBOTTOMLEFT ||
	       iHitCode == HTBOTTOMRIGHT || iHitCode == HTLEFT ||
	       iHitCode == HTRIGHT || iHitCode == HTTOP ||
	       iHitCode == HTTOPLEFT || iHitCode == HTTOPRIGHT)
            );

   return BOOL( t1 || t2  );
 }

void TMyMDIChild::WMSetCursor( RTMessage msg )//WM_SETCURSOR
 {                                            //WM_NCHITTEST
   HCURSOR hcCurs = NULL;

  if( bFlHaveFocus == TRUE && IsDisabledZone(iHitTest) == TRUE )
    hcCurs = TMyMDIChild::tcDisableCursor.GetHendl();
  else if( pbmpDIB )
   {
     if( GetCapture() == HWindow && LOWORD(msg.LParam) == HTCLIENT )
       hcCurs = TMyMDIChild::tcHand.GetHendl();
     else hcCurs = TMyMDIChild::mhTable[ MC_Default ].cursor;
   }
  else if( LOWORD(msg.LParam) == HTCLIENT && (HWND)msg.WParam == HWindow )
    hcCurs = TMyMDIChild::mhTable[ ((bFlHaveFocus == FALSE) ? MC_Default:TMyMDIChild::mcMode) ].cursor;

  if( hcCurs )
   {
     SetCursor( hcCurs );
     msg.Result = TRUE;
     return;
   }

  TBWindow::DefWndProc( msg );
 }

//void TMyMDIChild::WMCreate( RTMessage msg )
BOOL TMyMDIChild::Create()
 {

   //TBWindow::WMCreate( msg );
   TBWindow::Create();
   hdcOwn = GetDC( HWindow );


   if( !TMyMDIChild::mhTable[0].cursor )
    {                          //TModule
      TCurs::hinst = hInstRC;

      for( int i = 0; i < NUMB_MODES; i++ )
       {
         HCURSOR hc;
	 RTCurs rCurs = (RTCurs)listCurs.firstThat( findCursor, (Pvoid)TMyMDIChild::cursorInit[i] );
	 if( rCurs == NOOBJECT )
	  {
	    PTCurs pCurs;
	    pCurs = new TCurs( TMyMDIChild::cursorInit[i], TRUE );

	    if( !pCurs ) {GetApplication()->Error( ER_CREATEOBJECT ); return FALSE; }
	    if( pCurs->Status ) {GetApplication()->Error( pCurs->Status ); return FALSE; }

	    TMyMDIChild::listCurs.add( *pCurs );
	    hc = pCurs->GetHendl();
	  }
	 else hc = rCurs.GetHendl();

	 TMyMDIChild::mhTable[ i ].cursor = hc;
       }
    }

   SetModeChild( MC_Default );
   SetupMenuState();

   return TRUE;
 }

void TMyMDIChild::GenTitle( TFileInfo *fiInfo )
 {
   char chTmp[256], chTmp2[80];

   if( fiInfo->IdFile == GetScId() )
    {
      double percent = GetScale() * 100.0;

      if( fiInfo->CCopy > 1 || ttFlTitleType == ShowCopy ) wsprintf( chTmp2, "Copy:%u, ", fiInfo->CCopy );
      else *chTmp2 = 0;
      sprintf( chTmp, "%s %.2f%% [%s%s]", fiInfo->LpFileName, float(percent),
	chTmp2, (pbmpDIB) ? "Render":GetVPointDescr() );
      SetCaption( chTmp );

    }
 }


TMyMDIChild::~TMyMDIChild()
 {
   if( hpalAlias ) { DeleteObject( hpalAlias ); hpalAlias = NULL; }

   if( pbmpDIB )
    {     
      delete pbmpDIB; pbmpDIB = NULL;
    }

   DeleteRgn( hrgExt );
   DeleteRgn( hrgInt );
   DeleteRgn( hrgTtl );
   DeleteRgn( hrgNC );
   hrgExt = hrgInt = hrgTtl = hrgNC = NULL;

   //if( hbmMask ) { DeleteObject( hbmMask ); hbmMask = NULL; }
   if( hbrMask ) { DeleteObject( hbrMask ); hbrMask = NULL; }
   if( hbrMask2 ) { DeleteObject( hbrMask2 ); hbrMask2 = NULL; }

   if( hbmSys )    { DeleteObject( hbmSys ); hbmSys = NULL; }
   if( hbmZoom )   { DeleteObject( hbmZoom ); hbmZoom = NULL; }
   if( hbmReduce ) { DeleteObject( hbmReduce ); hbmReduce = NULL; }

   if( CalcSelf() < 2 )
    {
      if( PTMyMDIFrame(GetApplication()->MainWindow)->CountChildren() < 2 )
	PTMyMDIFrame(GetApplication()->MainWindow)->ActiveChild_ = NULL;

      if( pScena->CanClose() == TRUE )
        PostMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, GetScId(),
	  MAKELONG(0, CM_FIRST+UCM_CLOSE_SCENE) );
    }

 }

void TMyMDIChild::SetupWindow()
 {   

   TBWindow::SetupWindow();
   SetupRegions();

   HMENU hmenuSys;
   PTFileInfo pInfo = pGlobalFManeger->GetInfo( GetScId() );

   if( !pInfo )
    {
      TAutoStringRc str_( hInstRC, ID_MSG_7 );
      TAutoStringRc ttl_( hInstRC, ID_TITLE_ERROR );
      BWCCMessageBox( HWindow, str_(),
	ttl_(), MB_ICONSTOP | MB_OK | MB_APPLMODAL );
      //"Internal application error: child cann't get INFO for self file"
      return;
    }

   UINT flCheck = (pInfo->bFlWndGrowAsParent == TRUE) ? MF_CHECKED:
							MF_UNCHECKED;
   UINT flCheck2 = (bAutoFit == TRUE) ? MF_CHECKED:
                                        MF_UNCHECKED;

   if( (hmenuSys = GetSystemMenu(HWindow, FALSE)) )
    {
      TAutoStringRc s1_( hInstRC, ID_MSG_35, 50 ),
		    s2_( hInstRC, ID_MSG_36, 50 ),
		    s3_( hInstRC, ID_MSG_37, 50 ),
		    s4_( hInstRC, ID_MSG_38, 50 ),
		    s5_( hInstRC, ID_MSG_39, 50 );

      AppendMenu( hmenuSys, MF_SEPARATOR, 0, 0 );
      AppendMenu( hmenuSys, MF_BYCOMMAND | MF_ENABLED, UCM_RESTORE_CHILD_DIV,
	s1_() );
      //"Restore scr&een division\tCtr+Alt+R"
      AppendMenu( hmenuSys, MF_BYCOMMAND | MF_ENABLED | flCheck,
	UCM_ADAPTIVE_DIVISION, s2_() );
      //"&Grow as main window"
      AppendMenu( hmenuSys, MF_BYCOMMAND | MF_ENABLED | flCheck2,
	UCM_AUTO_FIT, s3_() );
      //"&Auto fit to window"

      AppendMenu( hmenuSys, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED,
	UCM_RESTORE_VIEW, s4_() );
      //"Restore &view (remove bitmap)"
      AppendMenu( hmenuSys, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED,
	UCM_FULLSCREEN_BMP, "Full screen (for bitmap)" );

      AppendMenu( hmenuSys, MF_BYCOMMAND | MF_ENABLED |
	(bFlPopupAllViews == TRUE) ? MF_CHECKED:MF_UNCHECKED,
	UCM_SET_POPUPFLAG, s5_() );
      //"Popup all &views"
    }

   hbmMask = LoadBitmap( hInstRC, MAKEINTRESOURCE(MDIBMP_MASK) );
   hbrMask = CreatePatternBrush( hbmMask );
   DeleteObject( hbmMask );

   hbmMask = LoadBitmap( hInstRC, MAKEINTRESOURCE(MDIBMP_MASK2) );
   hbrMask2 = CreatePatternBrush( hbmMask );
   DeleteObject( hbmMask );

   hbmSys = LoadBitmap( NULL, MAKEINTRESOURCE(OBM_CLOSE) );
   hbmZoom = LoadBitmap( NULL, MAKEINTRESOURCE(OBM_ZOOM) );
   hbmReduce = LoadBitmap( NULL, MAKEINTRESOURCE(OBM_REDUCE) );
   //TGrid::SetupMapMode( HWindow );
 }


void TMyMDIChild::EnableInSysMenu( BOOL bFl )
 {
   HMENU hmenuSys = GetSystemMenu( HWindow, FALSE );
   if( !hmenuSys ) return;

   UINT uiFl = (bFl == FALSE) ? (MF_DISABLED | MF_GRAYED):MF_ENABLED;

   EnableMenuItem( hmenuSys, UCM_RESTORE_CHILD_DIV, MF_BYCOMMAND | uiFl );
   EnableMenuItem( hmenuSys, UCM_AUTO_FIT, MF_BYCOMMAND | uiFl );
 }

BOOL TMyMDIChild::IsMyCommand( UINT com ) const
 {
   static UINT uiTblCommands[ NUMB_SYS_COMMANDS ] =
    { UCM_ADAPTIVE_DIVISION, UCM_RESTORE_CHILD_DIV, UCM_AUTO_FIT,
      UCM_RESTORE_VIEW, UCM_FULLSCREEN_BMP, UCM_SET_POPUPFLAG
    };

   for( int i = 0; i < NUMB_SYS_COMMANDS; i++ )
     if( uiTblCommands[i] == com ) return TRUE;

   return FALSE;
 }


BOOL TMyMDIChild::IsIconicRectSelfAndThis( WINDOWPOS _FAR *lpWp ) const
 {
   int w = GetSystemMetrics( SM_CXICON ) + 4,
       h = GetSystemMetrics( SM_CYICON ) + 4;

   RECT r; GetWindowRect( HWindow, &r );
   int W = r.right - r.left,
       H = r.bottom - r.top;

   return BOOL((W == w && H == h) ||
               (lpWp->cx == w && lpWp->cy == h));
 }

BOOL TMyMDIChild::IsZoomedRectSelfAndThis( WINDOWPOS _FAR *lpWp ) const
 {
   MINMAXINFO mxInf;
   TMessage msg; _fsetmem( &msg, sizeof(TMessage), 0 );
   msg.Message = WM_GETMINMAXINFO;
   msg.LParam = (LPARAM)(MINMAXINFO _FAR *)&mxInf;

   TBWindow::DefWndProc( msg );
   //SendMessage( HWindow, WM_GETMINMAXINFO, 0, (LPARAM)(LPMINMAXINFO)&mxInf );

   RECT r; GetWindowRect( HWindow, &r );
   int w = r.right - r.left,
       h = r.bottom - r.top;

   return BOOL((w == mxInf.ptMaxSize.x && h == mxInf.ptMaxSize.y) ||
	       (lpWp->cx == mxInf.ptMaxSize.x && lpWp->cy == mxInf.ptMaxSize.y));
 }



void TMyMDIChild::DefWndProc( RTMessage Msg )//WM_COMMAND
 {
   if( Msg.Message == WM_NCHITTEST )
    {
      if( HWindow != GetFocus() ) TMyMDIChild::cActivate = 1;
      else TMyMDIChild::cActivate = 0;

      TBWindow::DefWndProc( Msg );
      iHitTest = Msg.Result;

      if( IsDisabledZone(Msg.Result) == TRUE )
	Msg.Result = HTCLIENT;

      return;
    }
   else if( Msg.Message == WM_NCACTIVATE )
    {                        //TBWindow::DefWndProc( Msg ); //&&&&&&&&&&&&&&&&&
			     //return;
                                 
      if( IsIconic(HWindow) || IsZoomed(HWindow) )
       {
	 bFlNCActivated = 2;
	 TBWindow::DefWndProc( Msg );
         return;
       }

      if( !(bFlNCActivated == 2 || bFlNCActivated != Msg.WParam) &&
	  IsWindowVisible(HWindow) == TRUE
	)
       {
	 //Msg.Result = (Msg.WParam == FALSE) ? FALSE:TRUE;
	 Msg.Result = TRUE;
         return;
       }

      TBWindow::DefWndProc( Msg );

      TMessage msg; _fsetmem( &msg, sizeof(TMessage), 0 );
      bFlNCActivated = msg.WParam = Msg.WParam;

      WMNCPaint( msg );
      //Msg.Result = msg.Result;
      Msg.Result = TRUE;

      return;
    }
   else if( Msg.Message == WM_WINDOWPOSCHANGING )
    {      
      if( bFlShowOnCreate == FALSE )
       {
	 ((WINDOWPOS FAR *)Msg.LParam)->flags &= ~SWP_SHOWWINDOW;
	 ((WINDOWPOS FAR *)Msg.LParam)->flags |= SWP_NOACTIVATE;
       }

      if( !(((WINDOWPOS FAR *)Msg.LParam)->flags & SWP_NOSIZE ) && !pbmpDIB &&
	  pScena->GetCriticalLock() == FALSE
        )
       {
	 if( IsIconicRectSelfAndThis( (WINDOWPOS FAR *)Msg.LParam ) == FALSE &&
	     IsZoomedRectSelfAndThis( (WINDOWPOS FAR *)Msg.LParam ) == FALSE )
          {
	    Attr.W = ((WINDOWPOS FAR *)Msg.LParam)->cx;
            Attr.H = ((WINDOWPOS FAR *)Msg.LParam)->cy;
	    SetupRegions();
          }
       }
      else if( (pbmpDIB || pScena->GetCriticalLock() == TRUE ) &&
	       !(((WINDOWPOS FAR *)Msg.LParam)->flags & (SWP_NOSIZE | SWP_NOMOVE)) &&
	       IsIconicRectSelfAndThis( (WINDOWPOS FAR *)Msg.LParam ) == FALSE &&
	       IsZoomedRectSelfAndThis( (WINDOWPOS FAR *)Msg.LParam ) == FALSE
	     )
	((WINDOWPOS FAR *)Msg.LParam)->flags |= (SWP_NOSIZE | SWP_NOMOVE);

    }
   else if( Msg.Message == WM_SETTEXT )
    {		       

      TMessage msg; _fsetmem( &msg, sizeof(TMessage), 0 );      
      msg.WParam = bFlHaveFocus;

      TBWindow::DefWndProc( Msg );

      WMNCPaint( msg );

      Msg.Result = 0;
      return;
    }
   else if( Msg.Message == WM_GETTEXT )
    {
      /*LPSTR lpTSend = Title ? Title:"";
      lstrcpyn( (LPSTR)Msg.LParam, lpTSend, int(Msg.WParam) - 1 );
      Msg.Result = min( lstrlen(lpTSend), int(Msg.WParam - 1) );*/

      TBWindow::DefWndProc( Msg );

      return;
    }
   else

   if( Msg.Message == WM_SYSCOMMAND )
     switch( Msg.WParam & 0xFFF0 )
      {
	case SC_MAXIMIZE:
	  if( pbmpDIB || pScena->GetCriticalLock() == TRUE )
	   {
	     Msg.Result = 0;
             return;
	   };
	  break; 

	case SC_RESTORE:
	  if( (pbmpDIB || pScena->GetCriticalLock() == TRUE) && IsZoomed(HWindow) )
	   {
	     Msg.Result = 0;
             return;
	   };
	  break;

	case UCM_ADAPTIVE_DIVISION:	 

	case UCM_RESTORE_CHILD_DIV:
	  //if( !Parent ) break;
	  GetApplication()->MainWindow->DefWndProc( Msg );
	  Msg.Result = 0;
	  return;

	case UCM_AUTO_FIT:
	  bAutoFit = (bAutoFit == TRUE) ? FALSE:TRUE;
	  if( bAutoFit == TRUE && AskBitmap() == FALSE && IsIconic(HWindow) == FALSE )
	   {           
	     pScena->SetHardwareEnviron( (PTUCS)this, hdcOwn, (PTWindow)this );

	     pScena->ZoomToFit();

	     InvalidateRect( HWindow, NULL, TRUE );	     
	     GenTitle( pGlobalFManeger->GetInfo(GetScId()) );
           }
	  UINT flCheck = (bAutoFit == TRUE) ? MF_CHECKED:
					      MF_UNCHECKED;
	  HMENU hmenuSys;
	  if( (hmenuSys = GetSystemMenu(HWindow, FALSE)) )
	    CheckMenuItem( hmenuSys, UCM_AUTO_FIT, MF_BYCOMMAND | flCheck );

	  Msg.Result = 0;
	  return;

	case UCM_FULLSCREEN_BMP:
	  if( pbmpDIB )
           {
	     PTFullScrWindow pfswWin = new TFullScrWindow( this, NULL, pbmpDIB, hpalAlias, GetModule() );
	     if( pfswWin && pfswWin->Status )
	      {
		GetApplication()->Error( pfswWin->Status );
		delete pfswWin;
	      }
	     else GetApplication()->MakeWindow( pfswWin );
           }
	  break;

	case UCM_RESTORE_VIEW:
	  if( pbmpDIB )
	   {
	     if( hpalAlias ) { DeleteObject( hpalAlias ); hpalAlias = NULL; }
	     delete pbmpDIB;
	     pbmpDIB = NULL;
           }
          else break;

	  SetupMenuState();
	  GenTitle( pGlobalFManeger->GetInfo(GetScId()) );
	  InvalidateRect( HWindow, NULL, TRUE );
	  SendMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, GetScId(),
	    MAKELONG(0, CM_FIRST+UCM_SET_CHECKED_IN_MENU) );

	  Msg.Result = 0;
	  return;

	case UCM_SET_POPUPFLAG:
	  bFlPopupAllViews = (bFlPopupAllViews == FALSE) ? TRUE:FALSE;
	  flCheck = (bFlPopupAllViews == TRUE) ? MF_CHECKED:
						      MF_UNCHECKED;
	  if( (hmenuSys = GetSystemMenu(HWindow, FALSE)) )
	    CheckMenuItem( hmenuSys, UCM_SET_POPUPFLAG, MF_BYCOMMAND | flCheck );

	  Msg.Result = 0;
	  return;
      }
  
                 
   TBWindow::DefWndProc( Msg );
 }

void TMyMDIChild::WMSize( RTMessage Msg )
 {
   TBWindow::WMSize( Msg );

   if( bAutoFit == TRUE && AskBitmap() == FALSE && IsIconic(HWindow) == FALSE )
    {
      pScena->SetHardwareEnviron( (PTUCS)this, hdcOwn, (PTWindow)this );
      pScena->ZoomToFit();

      InvalidateRect( HWindow, NULL, TRUE );
      GenTitle( pGlobalFManeger->GetInfo(GetScId()) );
    }

 }


static int calc;

void CalcInstance( Pvoid pCh, Pvoid id )
 {
   PTMyMDIChild pChild = (PTMyMDIChild)pCh;

   if( PTWindow(pCh)->isA() == classtypeMDIChWindow && pChild->GetScId() == int(id) ) calc++;
 }


int TMyMDIChild::CalcSelf()
 {
   calc = 0;
   GetApplication()->MainWindow->ForEach( CalcInstance, Pvoid(GetScId()) );

   return calc;
 }

void TMyMDIChild::EnableShowInstance( Pvoid pCh, Pvoid id )
 {
   if( PTWindow(pCh)->isA() == classtypeMDIChWindow && PTMyMDIChild(pCh)->GetScId() == (int)id &&
       PTMyMDIChild(pCh)->bFlShowOnCreate == FALSE )
    {     
      PTMyMDIChild(pCh)->bFlShowOnCreate = TRUE;
      PTMyMDIChild(pCh)->Show( SW_SHOW );
    }
 }

void TMyMDIChild::ShutDownWindow()
 {
   if( CanClose(0) == TRUE )
     TBWindow::ShutDownWindow();
 }           

BOOL TMyMDIChild::CanClose()
 {
   return CanClose( 0 );
 }


BOOL TMyMDIChild::CanClose( int iReq ) //TWindow
 {
   if( bFlLockReenter == TRUE )
    {
      TAutoStringRc str_( hInstRC, ID_MSG_8 );
      TAutoStringRc ttl_( hInstRC, ID_TITLE_MESSAGE );
      BWCCMessageBox( HWindow, str_(),
	ttl_(), MB_ICONSTOP | MB_OK | MB_APPLMODAL );
      //"Window cann't close. It is locked by child process",
      return FALSE;
    }

   if( pScena->CanClose() == FALSE ) return TRUE; //if lock scena

   //if( CalcSelf() < 2 && pScena->GetChangeFlag() == TRUE ) &&&&
   if( pScena->GetChangeFlag() == TRUE )
    {
      if( !iReq && CalcSelf() > 1 ) return TRUE;

      PTFileInfo pInfo = pGlobalFManeger->GetInfo( GetScId() );

      if( !pInfo )
       {
	 TAutoStringRc str_( hInstRC, ID_MSG_7 );
	 TAutoStringRc ttl_( hInstRC, ID_TITLE_ERROR );
	 BWCCMessageBox( HWindow, str_(),
	   ttl_(), MB_ICONSTOP | MB_OK | MB_APPLMODAL );
	 //"Internal application error: child cann't get INFO for self file"
         return FALSE;
       }

      char chTmp[MAXPATH + MAXEXT + 50];                           //MessageBox
      TAutoStringRc s1_( hInstRC, ID_MSG_40 );
      wsprintf( chTmp, s1_(), pInfo->LpFileNameFull );
      //"File [%s] is CHANGED and not saved\nYou want to save ?"

      /*LPSTR lpPath;
      lpPath = (LPSTR)SendMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, GetScId(),
        MAKELONG(0, UCM_GET_MAINPATH) );*/

      int res;
      GetApplication()->ExecDialog(
	new TYNCDialog( GetApplication()->MainWindow, DlgYNC, &res, chTmp, DDB256_Dead1, FALSE, pMRC )
				  );

      if( res == IDCANCEL ) return FALSE;
      else if( res == IDYES )
       {
	 if( SendMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, GetScId(),
	   MAKELONG(0, CM_FIRST+UCM_SAVE) ) != -1 )
	   pScena->SetChangeFlag( FALSE );
	 else return FALSE;
       }
      else
        pScena->SetChangeFlag( FALSE );

      bFlPhaze = FALSE;
      /*if( CalcSelf() < 2 )
	PostMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, GetScId(),
	  MAKELONG(0, CM_FIRST+UCM_CLOSE_SCENE) );*/
    }
   else if( CalcSelf() < 2 )
    {
      bFlPhaze = FALSE;
      //PostMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, GetScId(),
	//MAKELONG(0, CM_FIRST+UCM_CLOSE_SCENE) );
    }
 
   return TRUE;
 }

void TMyMDIChild::WMEraseBkGnd( RTMessage Msg )
 {
   if( AskBitmap() == TRUE && /*IsZoomed(HWindow) == FALSE &&*/
       IsIconic(HWindow) == FALSE
     )
    {
      RECT r; GetClientRect( HWindow, &r );
      if( iBmpWidth < r.right || iBmpHigh < r.bottom )
       {
	 SetRectRgn( grcRegClient(), 0, 0, r.right, r.bottom );
	 SetRectRgn( grcRegBitmap(), 0, 0, iBmpWidth, iBmpHigh );
	 CombineRgn( grcRegErase(), grcRegClient(), grcRegBitmap(), RGN_DIFF );

	 UnrealizeObject( TMyMDIChild::grcBrushFon() );
	 FillRgn( hdcOwn, grcRegErase(), TMyMDIChild::grcBrushFon() );
       }

      Msg.Result = 0;
      return;
    }

   /*int iMapOld = GetMapMode( hdcOwn );
   DWORD dwOrgOld = GetViewportOrg( hdcOwn );
   DWORD dwExtOld;
   if( iMapOld != MM_TEXT ) dwExtOld = GetWindowExt( hdcOwn );

   SetMapMode( hdcOwn, MM_TEXT );
   SetViewportOrg( hdcOwn, 0, 0 );

   TBWindow::DefWndProc( Msg );

   SetMapMode( hdcOwn, iMapOld );
   SetViewportOrg( hdcOwn, LOWORD(dwOrgOld), HIWORD(dwOrgOld) );
   if( iMapOld != MM_TEXT ) SetWindowExt( hdcOwn, LOWORD(dwExtOld), HIWORD(dwExtOld) );
     */

   SetMapMode( hdcOwn, MM_TEXT );
   SetViewportOrg( hdcOwn, 0, 0 );

   TBWindow::DefWndProc( Msg );

   if( AskBitmap() == FALSE )
     SetupMapMode( HWindow, hdcOwn );
 }


void TMyMDIChild::SetupRegions()
 {
   SetRectRgn( hrgExt, 0, 0, Attr.W, Attr.H );
   SetRectRgn( hrgInt,
               GetSystemMetrics( SM_CXFRAME ),
               GetSystemMetrics( SM_CYFRAME ),
               Attr.W - GetSystemMetrics( SM_CXFRAME ),
	       Attr.H - GetSystemMetrics( SM_CYFRAME )
	     );
   CombineRgn( hrgExt, hrgExt, hrgInt, RGN_DIFF );

   SetRectRgn( hrgTtl, GetSystemMetrics( SM_CXFRAME ),
               GetSystemMetrics( SM_CYFRAME ),
               Attr.W - GetSystemMetrics( SM_CXFRAME ),
	       GetSystemMetrics( SM_CYFRAME ) + GetSystemMetrics( SM_CYCAPTION ) - 1
	     );

   CombineRgn( hrgNC, hrgExt, hrgTtl, RGN_OR );

   SetRectRgn( hrgTtlAbs, 0, 0, Attr.W - 2 * GetSystemMetrics( SM_CXFRAME ),
	       GetSystemMetrics( SM_CYCAPTION ) - 1
             );
 }

void TMyMDIChild::Mask( HDC hdc, RECT _FAR &r )
 { 
   int iOpOld = SetROP2( hdc, R2_MASKPEN );
   HBRUSH hbrOld = SelectBrush( hdc, hbrMask );

   UnrealizeObject( hbrMask );
   //FillRect( hdc, &r, hbrMask );
   //Rectangle( hdc, r.left, r.top, r.right, r.bottom );
   PaintRgn( hdc, hrgTtlAbs );

   int res = SetROP2( hdc, R2_MERGEPEN );
   SelectBrush( hdc, hbrMask2 );
   UnrealizeObject( hbrMask2 );
   //FillRect( hdc, &r, hbrMask2 );
   //Rectangle( hdc, r.left, r.top, r.right, r.bottom );
   PaintRgn( hdc, hrgTtlAbs );

   SetROP2( hdc, iOpOld );
   SelectBrush( hdc, hbrOld );
 }


void TMyMDIChild::WMNCPaint( RTMessage msg ) //WM_NCPAINT
 {                      

   if( IsIconic(HWindow) || IsZoomed(HWindow) || IsWindowVisible(HWindow) == FALSE )
     TBWindow::DefWndProc( msg );
   else
    {    

      msg.Result = 0;

      BOOL bFlKeyF = bFlHaveFocus;
      if( !msg.Message )
       {
         bFlHaveFocus = msg.WParam;
	 //if( msg.WParam == FALSE ) msg.Result = TRUE;
       } 


      HDC hdc = GetWindowDC( HWindow );

      if( !TMyMDIChild::dtbBuf.hbm )
	TMyMDIChild::dtbBuf.InitBuff( hdc, GetSystemMetrics( SM_CXSCREEN ),
	  GetSystemMetrics( SM_CYCAPTION ) );


      HDC hdcTmp = CreateCompatibleDC( hdc );
      TMyMDIChild::dtbBuf.RequiestSize( hdc, Attr.W, GetSystemMetrics( SM_CYCAPTION ) );
      HBITMAP hbmOld = (HBITMAP)SelectObject( hdcTmp, TMyMDIChild::dtbBuf.hbm );


      HBRUSH hbrFrame = CreateSolidBrush( GetSysColor((bFlHaveFocus == TRUE) ? COLOR_ACTIVEBORDER:COLOR_INACTIVEBORDER) );
      HBRUSH hbrTtl = CreateSolidBrush( GetSysColor((bFlHaveFocus == TRUE) ?  COLOR_ACTIVECAPTION:COLOR_INACTIVECAPTION) );     

      if( msg.Message == WM_NCPAINT )
       {
         FillRgn( hdc, hrgExt, hbrFrame );
	 FrameRgn( hdc, hrgExt, (HBRUSH)GetStockObject(BLACK_BRUSH), 1, 1 );
       }

      COLORREF cOld = SetBkColor( hdcTmp, GetSysColor((bFlHaveFocus == TRUE) ? COLOR_ACTIVECAPTION:COLOR_INACTIVECAPTION) );
      int iBkmOld = SetBkMode( hdcTmp, OPAQUE );
      HFONT hfOld = SelectFont( hdcTmp, (HFONT)GetStockObject(SYSTEM_FONT) );
      COLORREF cTxtOld = SetTextColor( hdcTmp, GetSysColor((bFlHaveFocus == TRUE) ? COLOR_CAPTIONTEXT:COLOR_INACTIVECAPTIONTEXT) );

      /*RECT r; r.left = r.top = 0;
      r.right = Attr.W - 2 * GetSystemMetrics( SM_CXFRAME );
      r.bottom = GetSystemMetrics( SM_CYCAPTION ) - 1;*/
      RECT r;
      GetRgnBox( hrgTtlAbs, &r );
      
      RECT rAct = r;
      rAct.left = GetSystemMetrics( SM_CXSIZE );
      //rAct.top = 0;
      rAct.right = r.right - 2 * GetSystemMetrics( SM_CXSIZE ) + 2;
      //rAct.bottom = r.bottom;

      RECT &rTTr = (bFlHaveFocus == TRUE) ? rAct:r;
      LPSTR lpTtl = Title ? Title:"";
      FillRect( hdcTmp, &rTTr, hbrTtl );
      DrawText( hdcTmp,
		lpTtl,
		lstrlen(lpTtl),
		&rTTr,
		DT_SINGLELINE | DT_CENTER | DT_VCENTER
	      );

      r.top--;  r.left--; r.right++;
      FrameRect( hdcTmp, &r, (HBRUSH)GetStockObject(BLACK_BRUSH) );
      r.top++;  r.left++; r.right--;


      int w = GetSystemMetrics( SM_CXFRAME ),
          h = GetSystemMetrics( SM_CYFRAME ),
	  hB = GetSystemMetrics( SM_CYCAPTION ),
	  wB = GetSystemMetrics( SM_CXSIZE );


      HPEN hpDraw = NULL;
      if( bFlHaveFocus == FALSE )
	hpDraw = CreatePen( PS_SOLID, 0, GetSysColor(COLOR_INACTIVEBORDER) );
      HPEN hpOld = SelectPen( hdc, (bFlHaveFocus == FALSE) ? hpDraw:(HPEN)GetStockObject(BLACK_PEN) );


      MoveTo( hdc, 1, h + hB - 2 );
      LineTo( hdc, w - 1, h + hB - 2 );

      MoveTo( hdc, Attr.W - 2, h + hB - 2 );
      LineTo( hdc, Attr.W - w, h + hB - 2 );


      MoveTo( hdc, 1, Attr.H - h - hB + 2 );
      LineTo( hdc, w - 1, Attr.H - h - hB  + 2);

      MoveTo( hdc, Attr.W - 2, Attr.H - h - hB + 2 );
      LineTo( hdc, Attr.W - w, Attr.H - h - hB + 2 );

      MoveTo( hdc, w + wB, 1 );
      LineTo( hdc, w + wB, h - 1 );

      MoveTo( hdc, Attr.W - w - wB - 1, 1 );
      LineTo( hdc, Attr.W - w - wB - 1, h - 1 );


      MoveTo( hdc, w + wB, Attr.H - 2 );
      LineTo( hdc, w + wB, Attr.H - h );

      MoveTo( hdc, Attr.W - w - wB - 1, Attr.H - 2 );
      LineTo( hdc, Attr.W - w - wB - 1, Attr.H - h );

      SelectPen( hdc, hpOld );
      if( hpDraw ) DeleteObject( hpDraw );
      
        
      if( bFlHaveFocus == TRUE )
       {
	 RECT r_;
	 HBITMAP hbm;
	 BITMAP bm;

	 GetObject( hbmZoom, sizeof(BITMAP), &bm );

	 r_.left = 0;
	 r_.right = wB;
	 r_.top = 0;
	 r_.bottom = GetSystemMetrics( SM_CYSIZE ) + 1;
	 DrawBmpInRect( hdcTmp, hbmSys, r_ );

	 r_.right = bm.bmWidth;
	 r_.left = r.right - bm.bmWidth;
	 r_.right++;
	 DrawBmpInRect( hdcTmp, hbmZoom, r_ );

	 r_.left -= (bm.bmWidth);
	 DrawBmpInRect( hdcTmp, hbmReduce, r_ );
       }  

      if( pScena->GetCriticalLock() == TRUE )
        Mask( hdcTmp, r );


      SetTextColor( hdcTmp, cTxtOld );
      SelectFont( hdcTmp, hfOld );
      SetBkMode( hdcTmp, iBkmOld );
      SetBkColor( hdcTmp, cOld );

      DeleteObject( hbrFrame );
      DeleteObject( hbrTtl );

      SelectObject( hdcTmp, hbmOld );

      r.left = GetSystemMetrics( SM_CXFRAME );
      r.top = GetSystemMetrics( SM_CYFRAME );
      DrawBmpInRect( hdc, TMyMDIChild::dtbBuf.hbm, r );

      DeleteDC( hdcTmp );
      ReleaseDC( HWindow, hdc );


      bFlHaveFocus = bFlKeyF;
    }


   if( !pbmpDIB ) SetupMapMode( HWindow, hdcOwn );
 }

void TMyMDIChild::Paint( HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo )
 {
   //TBWindow::Paint( PaintDC, PaintInfo );

   if( pbmpDIB )
    {
      //pbmpDIB->RGBDraw( PaintDC, 0, 0, 0, iBmpHigh );
      pbmpDIB->DrawBitmapDIBWithPalette( PaintDC, hpalAlias, 0, 0 );
      return;
    }

   SetupMapMode( HWindow, hdcOwn );
   TUCS::Draw( PaintDC, HWindow );

   if( GetVpoint_() != TUCS::VP_None )
    {
      pScena->SetHardwareEnviron( (PTUCS)this, hdcOwn, (PTWindow)this );
      pScena->Draw();
    }

   bFlRefresh = TRUE;

   if( bFlRequiestCallMouseHandl == TRUE )
    {                           //********
      /*if( TMyMDIChild::tcmhHandler )
       {
	 POINT p;
         GetCursorPos( &p );
	 TMessage msg;
	 msg.Receiver = HWindow;
	 msg.Message = WM_MOUSEMOVE;
	 msg.WParam = 0;
	 msg.LParam = MAKELONG( p.x, p.y );
	 msg.Result = 1;

         (TMyMDIChild::tcmhHandler)( msg, this, hdcOwn );
       }*/

      bFlRequiestCallMouseHandl = FALSE;
    }

 }

void TMyMDIChild::FillInfoDlg( List& rList )
 {
   PTFileInfo pInfo = pGlobalFManeger->GetInfo( GetScId() );

   if( !pInfo ) return;

   if( pScena )
     pScena->FillInfoDlg( rList, pInfo->LpFileNameFull );
 }

void TMyMDIChild::ToggleGeometry()
 {
   pScena->SetHideGeom( (pScena->GetHideGeom() == FALSE) ? TRUE:FALSE );
   RedrawVports();
 }

void TMyMDIChild::ToggleFullDetal()
 {
   pScena->SetDetal( (pScena->GetDetal() == FALSE) ? TRUE:FALSE );
   RedrawVports();
 }


BOOL TMyMDIChild::ToggleGreed()
 {
   //GridOnOff( ((GetGridState() == TRUE) ? FALSE:TRUE) );
   TGrid::ToggleGrid();
   InvalidateRect( HWindow, NULL, TRUE );

   return GetGridState();
 }

BOOL TMyMDIChild::ToggleUCS()
 {
   AxisOnOff( ((GetVisState() == TRUE) ? FALSE:TRUE) );
   InvalidateRect( HWindow, NULL, TRUE );

   return GetVisState();
 }

BOOL TMyMDIChild::ToggleSnap()
 {
   TGrid::ToggleSnap();

   return GetSnapState();
 }


void TMyMDIChild::WMPaletteChanged( RTMessage Msg )
 {
   if( (HWND)(Msg.WParam) == HWindow )
    {
      Msg.Result = 0;
      return;
    }
   else WMQueryNewPalette( Msg );
 }


void TMyMDIChild::WMQueryNewPalette( RTMessage Msg )
 {
   //WM_QUERYNEWPALETTE
   //HDC  Hdc;
   HPALETTE HpalOld;
   int i;


   if( !pbmpDIB || !pbmpDIB->GetPal() ) { Msg.Result = 0; return; }

   HPALETTE hpal = hpalAlias ? hpalAlias:(pbmpDIB ? pbmpDIB->GetPal():NULL);

   //Hdc = GetDC( HWindow );
   HpalOld = SelectPalette( hdcOwn, hpal, (bFlHaveFocus == TRUE) ? FALSE:TRUE );

   i = RealizePalette( hdcOwn );

   SelectPalette( hdcOwn, HpalOld, FALSE);
   //ReleaseDC( HWindow, Hdc);

   if (i > 0)
     InvalidateRect( HWindow, NULL, TRUE);


   Msg.Result = i;
 }


void TMyMDIChild::WMKillFocus( RTMessage Msg )
 {
   TBWindow::DefWndProc( Msg );

   /*if( bFlHaveFocus == TRUE )
     PostMessage( HWindow, WM_NCPAINT, 0 , 0 );*/
    
   bFlHaveFocus = FALSE;   
 }

void TMyMDIChild::ActivationResponse( WORD Activated,  BOOL IsIconified )
 {
   PTMyMDIFrame(GetApplication()->MainWindow)->ActiveChild_ = Activated ? this:NULL;

   TBWindow::ActivationResponse( Activated, IsIconified );

   if( Activated )
    {
      PTMyMDIFrame(GetApplication()->MainWindow)->SetCheckedInMenu();
      PTMyMDIFrame(GetApplication()->MainWindow)->CheckToolbarIndepend();
      if( bFlPopupAllViews == TRUE )
	PTMyMDIFrame(GetApplication()->MainWindow)->InternalChildActivate();
    }
 }

void TMyMDIChild::WMSetFocus( RTMessage Msg )//WM_COMMAND
 {
   TBWindow::DefWndProc( Msg );//TMDIClient TMDIFrame
   //if( IsWindowVisible(HWindow) == FALSE ) return;

   
   /*SendMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, (WPARAM)Attr.Id,
     MAKELONG(HWindow, UCM_INTERNAL_ACTIVATE_CHILD) );*/

   /*if( bFlPopupAllViews == TRUE )
     PostMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, (WPARAM)Attr.Id,
       MAKELONG(HWindow, UCM_INTERNAL_FOCUS_CHILD) );*/

  
   POINT p; GetCursorPos( &p );
   ScreenToClient( HWindow, &p );
   RefreshMousePos( hdcOwn, p.x, p.y );


   BOOL bFlKey = bFlHaveFocus;
   bFlHaveFocus = TRUE;

   //PostMessage( HWindow, WM_NCACTIVATE, TRUE, 0 );
   TMessage msg; _fsetmem( &msg, sizeof(TMessage), 0 );
   msg.WParam = TRUE;
   msg.Message = WM_NCACTIVATE;
   DefWndProc( msg );
   /*if( bFlKey == FALSE )
     SetWindowPos( HWindow, NULL, 0, 0, 0, 0,
       SWP_DRAWFRAME | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER );*/

   if( !pbmpDIB ) SetupMapMode( HWindow, hdcOwn );

   if( pbmpDIB )
    {
      TMessage msg; _fsetmem( &msg, sizeof(TMessage), 0 );
      msg.Message = WM_QUERYNEWPALETTE;
      WMQueryNewPalette( msg );
    }
   else
     pScena->SetHardwareEnviron( (PTUCS)this, hdcOwn, (PTWindow)this );
      /*
      if( pbmpDIB->RealizePaletteDIB( hdcOwn ) > 0 )
	InvalidateRect( HWindow, NULL, FALSE );*/
 }

void TMyMDIChild::SetGridExtents()
 {
   PTGrExtentsDlg pDlg;
   int res;
              //TDialog TWindow
   if( (pDlg = new TGrExtentsDlg( GetApplication()->MainWindow, MAKEINTRESOURCE(DlgGridExtents), (PTGrid)this, pMRC)) &&
     !pDlg->Status )
     res = GetApplication()->ExecDialog( pDlg );
   else GetApplication()->Error( pDlg ? pDlg->Status:ER_CREATEOBJECT );

   if( res == IDOK && GetGridState() == TRUE )
     InvalidateRect( HWindow, NULL, TRUE );
 }

void TMyMDIChild::SaveBmp() const
 {
   SetErrorMode( SEM_FAILCRITICALERRORS );

   TMyOpenDialogTransfer saveDlgTransfer;

   saveDlgTransfer.pComboBoxInput->AddString( "*.bmp", TRUE );
   getcwd( saveDlgTransfer.chPath, MAXPATH );
   if( saveDlgTransfer.chPath[_fstrlen(saveDlgTransfer.chPath) - 1] != '\\' )
     _fstrcat( saveDlgTransfer.chPath, "\\" );

   TAutoStringRc s1_( hInstRC, ID_MSG_52 );
   saveDlgTransfer.pComboBoxType->AddString( /*"*.*     All files"*/s1_(), FALSE );
   s1_ = TAutoStringRc( hInstRC, ID_MSG_64 );
   saveDlgTransfer.pComboBoxType->AddString( /*"*.BMP   Windows DIB bitmap file"*/s1_(), TRUE );

   saveDlgTransfer.bFlCheckName = FALSE;

   saveDlgTransfer.MakeInit();

   PTCenterFileDialog  pFileDial;
   int res;
                                          //TFileDialog  TWindowsObject
   if( (pFileDial = new TCenterFileDialog( GetApplication()->MainWindow,
      DlgSaveBmp, &saveDlgTransfer,
      TCenterFileDialog::Init, pMRC) ) &&
     !pFileDial->Status )
    {
      if( (res = GetApplication()->ExecDialog( pFileDial )) == IDOK )
	pbmpDIB->Save( saveDlgTransfer.chFilePath );
    }
   else
    {
      GetApplication()->Error( pFileDial ? pFileDial->Status:ER_CREATEOBJECT );
      if( pFileDial ) pFileDial->CloseWindow();
    }
 }


TMDIChSave::TMDIChSave( RTMyMDIChild rCh )
 {
   bFlGr = rCh.GetGridState();
   bFlUCS =  rCh.GetVisState(),
   bFlSnap = rCh.GetSnapState();
   bFlVisScene = rCh.GetScena()->IsVisyal();

   rCh.GetVpoint( vpKeyView, dKeyAngleH, dKeyAngleV );
   mcMode = rCh.GetModeChild();
   rCh.GetShift( shiftX, shiftY );
   scale = rCh.GetScale();
   
   globalBasePoint = rCh.GetGlobalBase();
   bFlIsBasePointVisible = rCh.IsVisibleBasePoint();

   //phazeOfCurMode_ = TMyMDIChild::phazeOfCurMode;
 }

TMDIChSave::Restore( RTMyMDIChild rCh, BOOL bFlNoChangeMode )
 {
   rCh.ChangeVPoint( vpKeyView, dKeyAngleH, dKeyAngleV );

   rCh.GridOnOff( bFlGr );
   rCh.GridSnapOnOff( bFlSnap );
   rCh.AxisOnOff( bFlUCS );
   rCh.GetScena()->SceneOnOff( bFlVisScene );

   if( bFlNoChangeMode == FALSE )
    {
      rCh.SetModeChild( mcMode );
      rCh.GenTitle( pGlobalFManeger->GetInfo(rCh.GetScId()) );
    }

   //TMyMDIChild::phazeOfCurMode = phazeOfCurMode_;

   rCh.ShiftAbs( shiftX, shiftY );
   rCh.ChangeScale( scale ); 


   rCh.SetGlobalBase( globalBasePoint );
   rCh.SetVisibleBasePoint( bFlIsBasePointVisible );

   InvalidateRect( rCh.HWindow, NULL, TRUE );//&&&
 }

void TMyMDIChild::GetFacetAndOp( ModeChild mcNext )
 {
   if( ValidWindowOper( MC_SetupUCS ) == FALSE ) return;

   TMyMDIChild::mcNextOp = MC_SelectFacet;
   TMyMDIChild::mcNextOp2 = mcNext;
   TMyMDIChild::ptSelected.x = 0; TMyMDIChild::ptSelected.y = 0;
   TMyMDIChild::phazeOfCurMode = 0;

   SetModeChild( MC_GetPoint );
   bFlModeActive = TRUE;

   TSendTxt stSend;
   stSend.handle = 0;
   TAutoStringRc str_( hInstRC, ID_MSG_16 );
   //stSend.lpTxt = "Select vertex";
   stSend.lpTxt = str_();

   SendMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
 }

void TMyMDIChild::GetPointAndOp( ModeChild mcNext )
 {
   if( ValidWindowOper( MC_SetupUCS ) == FALSE ) return;

   TMyMDIChild::mcNextOp = mcNext; 
   TMyMDIChild::ptSelected.x = 0; TMyMDIChild::ptSelected.y = 0;
   TMyMDIChild::phazeOfCurMode = 0;

   SetModeChild( MC_GetPoint );
   bFlModeActive = TRUE;

   TSendTxt stSend;
   stSend.handle = 0;
   TAutoStringRc str_( hInstRC, ID_MSG_17 );
   //stSend.lpTxt = "Select point or vertex";
   stSend.lpTxt = str_();

   SendMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
 }


void TMyMDIChild::SetFilter( ModeChild mcM )
 {
   struct TFilterItem
    {
      ModeChild mcM;
      PTClassFilterFunc pFilter;
    };

   static TFilterItem fiTblFilter[ NUMB_MODES ] =
    {
      { MC_Default,         FilterBody },
      { MC_SetupUCS,        FilterBody },
      { MC_ShiftUCS,        FilterBody },
      { MC_CreCylinder,     FilterBody },
      { MC_CreCone,         FilterBody },
      { MC_CreSphere,       FilterBody },
      { MC_CreTorus,        FilterBody },
      { MC_CreTube,         FilterBody },
      { MC_GetWindow,       FilterBody },
      { MC_Select,          FilterBodyAndLights },
      { MC_MoveBody,        FilterBodyAndLights },
      { MC_DeleteBody,      FilterBodyAndLights },
      { MC_3DScaleBody,     FilterBodyAndLights },
      { MC_2DScaleBody,     FilterBodyAndLights },
      { MC_RotateBody,      FilterBodyAndLights },
      { MC_SelectBody,      FilterBodyAndLights },
      { MC_GetPoint,        FilterBody },
      { MC_SetupGlobalBP,   FilterAll },
      { MC_AligmentVertex,  FilterBody },
      { MC_AligmentFacet,   FilterBody },
      { MC_SelectFacet,     FilterBody },
      { MC_CreateOmni,      FilterBody },
      { MC_OmniAdjust,      FilterLights },
      { MC_VPortAligment,   FilterBody },
      { MC_PlaceHilite,     FilterBodyAndLights },
      { MC_Smoth,           FilterBody }
    };

   for( int i = 0; i < NUMB_MODES; i++ )
     if( fiTblFilter[i].mcM == mcM )
      {
	TMyMDIChild::pCurFilter = fiTblFilter[i].pFilter;
	break;
      }
   
 }

void TMyMDIChild::SelectAndOp( ModeChild mcNext )
 {
   if( ValidWindowOper( MC_SetupUCS ) == FALSE ) return;

   TMyMDIChild::mcNextOp = mcNext;
   

   TMyMDIChild::psfSelected = NULL;

   TMyMDIChild::phazeOfCurMode = 0;

   SetModeChild( MC_Select );
   SetFilter( mcNext );
   bFlModeActive = TRUE;

   if( GetScena()->SelectedCount(TMyMDIChild::pCurFilter) && TMyMDIChild::IsTransformSelected() == TRUE )
    {
      GetScena()->SetHardwareEnviron( (PTUCS)this, hdcOwn, (PTWindow)this );

      TMyMDIChild::bFlShiftActive = FALSE;
      TMyMDIChild::phazeOfCurMode = 0;

      InitMode( TMyMDIChild::mcNextOp );
      return;
    }

   TSendTxt stSend;
   stSend.handle = 0;
   TAutoStringRc str_( hInstRC, ID_MSG_18 );
   //stSend.lpTxt = "Select object";
   stSend.lpTxt = str_();

   SendMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
 }


void TMyMDIChild::UserUCSSetup()
 {
   if( ValidWindowOper( MC_SetupUCS ) == FALSE ) return;
   

   if( //TMyMDIChild::bFlShiftActive == TRUE ||
       TMyMDIChild::bFlUserUCSSetupSta == TRUE )
    {
      TAutoStringRc str_( hInstRC, ID_MSG_9 );
      TAutoStringRc ttl_( hInstRC, ID_TITLE_ERROR );
      BWCCMessageBox( HWindow, str_(),
	ttl_(), MB_ICONSTOP | MB_OK | MB_APPLMODAL );
      //"Mode is already active. Click start point in any window"
      return;
    }


   //tcmhHandler = UCSSetupMouseHandler;

   SetModeChild( MC_SetupUCS );
   bFlUserUCSSetupSta = TRUE;
   mcsSave = TMDIChSave( *this );

   SetCapture( HWindow );
   ShowCursor( FALSE );

   ChangeScale( 1 );
   ShiftAbs( 0, 0 );

   //if( pScena->GetDetal() == TRUE )
     pScena->SceneOnOff( FALSE );
   GridOnOff( FALSE );
   GridSnapOnOff( FALSE ); 
   AxisOnOff( TRUE );

   SetVisibleBasePoint( FALSE );

   InvalidateRect( HWindow, NULL, TRUE );

   TSendTxt stSend;
   stSend.handle = 0;
   TAutoStringRc str_( hInstRC, ID_MSG_19 );
   //stSend.lpTxt = "Move mouse for change angles";
   stSend.lpTxt = str_();


   SendMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
 }

void TMyMDIChild::FitToZoom()
 {
   if( ValidWindowOper( MC_GetWindow ) == FALSE ) return;

   pScena->SetHardwareEnviron( (PTUCS)this, hdcOwn, (PTWindow)this );
   pScena->ZoomToFit();

   //pCh->GenTitle( pGlobalFManeger->GetInfo(pCh->GetScId()) );

   PostMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, (WPARAM)TMyMDIChild::mcMode,
      MAKELONG(HWindow, UCM_CHANGED_CHILD_MODE) );
 }

void SpecialSelectWF( PTMyMDIChild pCh, RECT& rRect )
 {
   if( pCh->GetApplication()->MainWindow && pCh->GetApplication()->MainWindow->isA() == classtypeMDIFrame )
    {
      BOOL bFlInvert = BOOL(SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, pCh->GetScId(), MAKELONG(0, UCM_GET_INVERTSELMODE_FLAG) ));
      	   
      pCh->GetScena()->SetHardwareEnviron( (PTUCS)pCh, pCh->hdcOwn, (PTWindow)pCh );

      BOOL bRes;
      if( TMyMDIChild::UsedCrossing() == TRUE )
	bRes = pCh->GetScena()->SelectWithCrossing( rRect, (bFlInvert == FALSE) ? TRUE:FALSE );
      else
       bRes = pCh->GetScena()->SelectWithWindow( rRect, (bFlInvert == FALSE) ? TRUE:FALSE );

      if( bRes == TRUE ) pCh->RedrawVports();
    }
 }

void SpecialFitToWindow( PTMyMDIChild pCh, RECT& rRect )
 {
   pCh->pScena->SetHardwareEnviron( (PTUCS)pCh, pCh->hdcOwn, (PTWindow)pCh );
   pCh->pScena->ZoomToFit( &rRect );
   pCh->GenTitle( pGlobalFManeger->GetInfo(pCh->GetScId()) );
 }

void  TMyMDIChild::FitToWindow()
 {
   if( ValidWindowOper( MC_GetWindow ) == FALSE ) return;



  /* if( TMyMDIChild::bFlFitActive == TRUE )
    {
      BWCCMessageBox( HWindow, "Mode is already active. Click start point in any window",
	"Error", MB_ICONSTOP | MB_OK | MB_APPLMODAL );
      return;
    }*/


   TMyMDIChild::callAfter = SpecialFitToWindow;
   SetModeChild( MC_GetWindow );
   TMyMDIChild::bFlFitActive = TRUE;

   TMyMDIChild::phazeOfCurMode = 0;

   TSendTxt stSend;
   stSend.handle = 0;
   TAutoStringRc str_( hInstRC, ID_MSG_20 );
   //stSend.lpTxt = "Click left corner of window";
   stSend.lpTxt = str_();

   SendMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
 }

void TMyMDIChild::SelectWithWindowFrame()
 {
   if( ValidWindowOper( MC_GetWindow ) == FALSE ) return;


                                                  
   /*if( TMyMDIChild::bFlSelectWithWindowFrameActive == TRUE )
    {
      BWCCMessageBox( HWindow, "Mode is already active. Click start point in any window",
	"Error", MB_ICONSTOP | MB_OK | MB_APPLMODAL );
      return;
    }*/


   TMyMDIChild::callAfter = SpecialSelectWF;
   SetModeChild( MC_GetWindow );
   TMyMDIChild::bFlSelectWithWindowFrameActive = TRUE;

   TMyMDIChild::phazeOfCurMode = 0;

   TSendTxt stSend;
   stSend.handle = 0;
   TAutoStringRc str_( hInstRC, ID_MSG_20 );
   //stSend.lpTxt = "Click left corner of window";
   stSend.lpTxt = str_();

   SendMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
 }

void TMyMDIChild::UserUCSShift()
{
   if( ValidWindowOper( MC_ShiftUCS ) == FALSE ) return;



   /*if( TMyMDIChild::bFlShiftActive == TRUE ||
       TMyMDIChild::bFlUserUCSSetupSta == TRUE )
    {
      BWCCMessageBox( HWindow, "Mode is already active. Click start point in any window",
	"Error", MB_ICONSTOP | MB_OK | MB_APPLMODAL );
      return;
    }*/


   SetModeChild( MC_ShiftUCS );
   bFlShiftActive = TRUE;

   TMyMDIChild::phazeOfCurMode = 0;

   /*GridOnOff( FALSE );
   GridSnapOnOff( FALSE );
   AxisOnOff( TRUE );

   InvalidateRect( HWindow, NULL, TRUE );
     */
   TSendTxt stSend;
   stSend.handle = 0;
   TAutoStringRc str_( hInstRC, ID_MSG_21 );
   //stSend.lpTxt = "Select base point for shift";
   stSend.lpTxt = str_();

   SendMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
 }

void TMyMDIChild::CreateCylinder( PTFaceData pFD )
 {
   //if( ValidWindowOper( MC_CreCylinder ) == FALSE ) return;



   TMyMDIChild::phazeOfCurMode = 0;

   SetModeChild( (pFD->isA() == FT_Cylinder) ? MC_CreCylinder:MC_CreCone );
   bFlModeActive = TRUE;///////

   pfdFace = pFD;

   TSendTxt stSend;
   stSend.handle = 0;
   //stSend.lpTxt = "Select center of cylinder";
   char cBuf[128];
   TAutoStringRc s1_( hInstRC, ID_MSG_41 );
   sprintf( cBuf, s1_(), ((PTCylinderInit)pFD)->nSides,
   //"Select base center of %d sides %s"
     TMyMDIChild::pfdFace->GetNameStr() );
   stSend.lpTxt = cBuf;
   SendMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
 }

void TMyMDIChild::CreateSphere( PTFaceData pFD )
 {
   //if( ValidWindowOper( MC_CreCylinder ) == FALSE ) return;



   TMyMDIChild::phazeOfCurMode = 0;

   SetModeChild( MC_CreSphere );
   bFlModeActive = TRUE;///////

   pfdFace = pFD;

   TSendTxt stSend;
   stSend.handle = 0;
   //stSend.lpTxt = "Click in viewport for select center of sphere";
   char cBuf[128];
   TAutoStringRc s1_( hInstRC, ID_MSG_42 );
   sprintf( cBuf, s1_(),
     ((PTSphereInit)pFD)->nHoriz, ((PTSphereInit)pFD)->nMerid
	  );
   //"Click in viewport for select center of %d horiz. & %d merid. sphere"
   stSend.lpTxt = cBuf;
   SendMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
 }

void TMyMDIChild::CreateTorus( PTFaceData pFD )
 {
   //if( ValidWindowOper( MC_CreCylinder ) == FALSE ) return;



   TMyMDIChild::phazeOfCurMode = 0;

   SetModeChild( MC_CreTorus );
   bFlModeActive = TRUE;///////

   pfdFace = pFD;

   TSendTxt stSend;
   stSend.handle = 0;
   //stSend.lpTxt = "Click in viewport for select center of torus";
   char cBuf[128];
   TAutoStringRc s1_( hInstRC, ID_MSG_43 );
   sprintf( cBuf, s1_(),
     ((PTTorusInit)pFD)->nHoriz, ((PTTorusInit)pFD)->nMerid
	  );
   //"Click in viewport for select center of %d horiz. & %d merid. torus"
   stSend.lpTxt = cBuf;
   SendMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
 }

void TMyMDIChild::CreateTube( PTFaceData pFD )
 {
   //if( ValidWindowOper( MC_CreCylinder ) == FALSE ) return;



   TMyMDIChild::phazeOfCurMode = 0;

   SetModeChild( MC_CreTube );
   bFlModeActive = TRUE;///////

   pfdFace = pFD;

   TSendTxt stSend;
   stSend.handle = 0;
   //stSend.lpTxt = "Click in viewport for select center of base tube";
   char cBuf[128];
   TAutoStringRc s1_( hInstRC, ID_MSG_44 );
   sprintf( cBuf, s1_(),
     ((PTTubeInit)pFD)->nSides
	  );
   //"Click in viewport for select center of %d sides tube"
   stSend.lpTxt = cBuf;
   SendMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
 }

/*
static void SetCursorInst( Pvoid pCh, Pvoid pPar )
 {
   PTMyMDIChild  pChild = (PTMyMDIChild)pCh;
   if( pChild->isA() != classtypeMDIChWindow ) return;

   pChild->SetCurs( (HCURSOR)pPar );
 }*/
 

ModeChild TMyMDIChild::SetModeChild( ModeChild mcM )
 {
   //TMyMDIChild::pCurFilter = FilterBody;

   if( TMyMDIChild::mcMode == mcM )
    {
      PostMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, (WPARAM)TMyMDIChild::mcMode,
        MAKELONG(HWindow, UCM_CHANGED_CHILD_MODE) );
      return TMyMDIChild::mcKeyMode;
    }

   if( TMyMDIChild::GetModeEnblKeyFlag(TMyMDIChild::mcMode) == TRUE )
    {
      TMyMDIChild::mcKeyMode = TMyMDIChild::mcMode;
      TMyMDIChild::mcKeyNextMode = TMyMDIChild::mcNextOp;
    }
   else
    {
      TMyMDIChild::mcKeyMode = MC_Default;
      TMyMDIChild::mcKeyNextMode = MC_Default;
    }


   if( mcM < MC_Default || mcM > MC_End - 1 )
    {
       ModeChild mcKey = TMyMDIChild::mcMode;
       SetModeChild( MC_Default );
       SetFilter( TMyMDIChild::mcMode );
       return mcKey;
    }

    ModeChild mcKey = TMyMDIChild::mcMode;
    TMyMDIChild::tcmhHandler = TMyMDIChild::mhTable[ mcM ].tcmhHandler;
    TMyMDIChild::tcmLBHand   = TMyMDIChild::mhTable[ mcM ].tcmLBHand;
    TMyMDIChild::tcmRBHand   = TMyMDIChild::mhTable[ mcM ].tcmRBHand;
    TMyMDIChild::mcMode = mcM;

    PostMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, (WPARAM)TMyMDIChild::mcMode,
      MAKELONG(HWindow, UCM_CHANGED_CHILD_MODE) );

    TMyMDIChild::bFlShiftActive = (mcM == MC_ShiftUCS) ? TRUE:FALSE;
    TMyMDIChild::bFlFitActive = FALSE;
    TMyMDIChild::bFlSelectWithWindowFrameActive = FALSE;

    SetFilter( TMyMDIChild::mcNextOp );

    return mcKey;
 }



void TMyMDIChild::WMMouseMove( RTMessage msg )//WM_MOUSEMOVE
 {
   static BOOL bFlLock = FALSE;

   if( pbmpDIB && bFlHaveFocus == TRUE && GetCriticalLock() == FALSE )
    {
      if( (msg.WParam & MK_LBUTTON) && GetCapture() == HWindow )
       {
	 GetClientRect( HWindow, &rBmpView );
	 POINT pt; pt.x = LOWORD(msg.LParam); pt.y = HIWORD(msg.LParam);

	 if( PtInRect( &rBmpView, pt ) == FALSE )
	  {
	    ReleaseCapture();
	    SetCursor( TMyMDIChild::mhTable[ MC_Default ].cursor );
	    //msg.Result = 0;
	    TBWindow::DefWndProc( msg );
	    return;
	  }

	 //LPtoDP( hdcOwn, &pt, 1 );

         int iStaOrgX = LOWORD(GetViewportOrg( hdcOwn )),
	     iStaOrgY = HIWORD(GetViewportOrg( hdcOwn ));
         int iPoW = iStaOrgX,
             iPoH = iStaOrgY;
         int iDx = iBmpWidth - rBmpView.right,
	     iDy = iBmpHigh - rBmpView.bottom;
		 //mouseX, mouseY
         iPoW -= (mouseX - pt.x);
         iPoH -= (mouseY - pt.y);

         if( iDx > 0 || iDy > 0 )
          {
            if( iPoW > 0 ) iPoW = 0;
            else if( iPoW < -iDx ) iPoW = -iDx;
            if( iPoH > 0 ) iPoH = 0;
	    else if( iPoH < -iDy ) iPoH = -iDy;

	    if( iPoW != iStaOrgX || iPoH != iStaOrgY )
	     {
	       SetViewportOrg( hdcOwn, (iDx> 0) ? iPoW:0, (iDy > 0) ? iPoH:0 );
	       InvalidateRect( HWindow, NULL, TRUE );
	     }
          } 
       }
      RefreshMousePos( hdcOwn, LOWORD(msg.LParam), HIWORD(msg.LParam) );
      //msg.Result = 0;
      TBWindow::DefWndProc( msg );
      return;
    }
  

   if( bFlLock == FALSE )
    {
       bFlLock = TRUE;

      if( bFlHaveFocus == TRUE )
       {
         RefreshMousePos( hdcOwn, LOWORD(msg.LParam), HIWORD(msg.LParam) );      
         msg.LParam = MAKELONG( mouseX, mouseY );
	(TMyMDIChild::tcmhHandler)( msg, this, hdcOwn );
       }

      bFlLock = FALSE;
    }

   TBWindow::DefWndProc( msg );   
 }

void TMyMDIChild::WMLButtonUp( RTMessage msg )
 {
   if( pbmpDIB && bFlHaveFocus == TRUE && GetCapture() == HWindow )
    {
      ReleaseCapture();
      SetCursor( TMyMDIChild::mhTable[ MC_Default ].cursor );
      //msg.Result = 0;
      //TBWindow::DefWndProc( msg );
      //return;
    }

   TBWindow::DefWndProc( msg );
 }

void TMyMDIChild::WMLButtonDown( RTMessage msg )
 {
   static BOOL bFlLock = FALSE;

   if( GetCriticalLock() == TRUE )
    {
      MessageBeep( -1 );
      TBWindow::WMLButtonDown( msg );
      return;
    }

   if( pbmpDIB && bFlHaveFocus == TRUE && TMyMDIChild::cActivate == 0 )
    {
      GetClientRect( HWindow, &rBmpView );
      POINT pt; pt.x = LOWORD(msg.LParam); pt.y = HIWORD(msg.LParam);
      if( PtInRect(&rBmpView, pt) == TRUE && !GetCapture() )
       {
         SetCapture( HWindow );
	 SetCursor( TMyMDIChild::tcHand.GetHendl() );
       }

      //msg.Result = 0;
      TBWindow::WMLButtonDown( msg );
      return;
    }

   if( bFlLock == FALSE )
    {
       bFlLock = TRUE;

       if( GetCapture() == HWindow ) TMyMDIChild::cActivate = 0;
       bFlLBDown = TRUE;
       if( bFlHaveFocus == TRUE )
        {
	  if( TMyMDIChild::cActivate == 0 && ValidWindowOper( mcMode ) == FALSE)
	   {
	     bFlLock = FALSE;
	     //TBWindow::WMLButtonDown( msg );
	     return;
           }

          if( TMyMDIChild::cActivate == 0 )
	    (TMyMDIChild::tcmLBHand)( msg, this );
	}

      bFlLock = FALSE;
    }

   TBWindow::WMLButtonDown( msg );
 }

void TMyMDIChild::WMRButtonDown( RTMessage msg )
 {
   static BOOL bFlLock = FALSE;

   if( GetCriticalLock() == TRUE )
    {
      MessageBeep( -1 );
      TBWindow::WMLButtonDown( msg );
      return;
    }


   if( bFlLock == FALSE )
    {
       bFlLock = TRUE;

       if( GetCapture() == HWindow ) TMyMDIChild::cActivate = 0;
       bFlRBDown = TRUE;   
       if( TMyMDIChild::cActivate == 0 )
	 (TMyMDIChild::tcmRBHand)( msg, this );

       bFlLock = FALSE;
    }

   TBWindow::WMLButtonDown( msg );
 }

void DefMouseLBHandler( RTMessage msg, PTMyMDIChild pCh )
 {
   //pCh->TBWindow::WMLButtonDown( msg );
 }

void DefMouseRBHandler( RTMessage msg, PTMyMDIChild pCh )
 {
   //pCh->TBWindow::WMLButtonDown( msg );
 }

void DefMouseHandler( RTMessage msg, PTMyMDIChild pWin, HDC hdc )
 {
   if( pWin->ShowBitmap() == TRUE ) return;

   char cBuf[50];

   POINT p; p.x = LOWORD(msg.LParam); p.y = HIWORD(msg.LParam);
   DPtoLP( hdc, &p, 1 );
   T3DPoint pt = T3DPoint( p.x, p.y, 0 );
   pWin->RecalcBaseToUser( &pt, 1 );

   sprintf( cBuf, "X=%5.2f  Y=%5.2f  Z=%5.2f",
     float( TGrid::CheckTinyCoord(pt.x) ),
     float( TGrid::CheckTinyCoord(pt.y) ),
     float( TGrid::CheckTinyCoord(pt.z) )
	  );

   TSendTxt stSend;
   stSend.handle = 2;
   stSend.lpTxt = cBuf;
   SendMessage( pWin->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
 }



BOOL TMyMDIChild::ReturnCursor( int x, int y )
 {                                               
   int iWid = GetSystemMetrics( SM_CXSCREEN ),
       iHigh = GetSystemMetrics( SM_CYSCREEN );

   POINT p; p.x = x; p.y = y;
   ClientToScreen( HWindow, &p );

   if( p.x < 1 || p.x >= iWid - 1 || p.y < 1 || p.y >= iHigh - 1 )
    {
      SetCursorPos( iWid / 2, iHigh / 2 );
      return TRUE;
    }

   return FALSE;
 }

BOOL TMyMDIChild::ScrollForShowBody( RTObol rOb )
 {
   T3DPoint pt = rOb.GetGeomCenterObol();
   RecalcUserToBase( &pt, 1 );
   POINT p;
   p.x = pt.x; p.y = pt.y;
   LPtoDP( hdcOwn, &p, 1 );

   RECT r; GetClientRect( HWindow, &r );
   int left = p.x < 2, right = p.x > r.right - 2,
       top = p.y < 2, bottom = p.y > r.bottom - 2;

   if( !(left || right || top || bottom) ) return FALSE;

   int dx, dy;  dx = dy = 0;

   int dH = double(r.right) * 0.2,
       dV = double(r.bottom) * 0.2;
   p.x = abs(rOb.iMinX - rOb.iMaxX);
   p.y = abs(rOb.iMinY - rOb.iMaxY);
   double dCH = (double)dH * 0.5, dCV = (double)dV * 0.5;
   if( p.x < dCH ) p.x = dCH;
   if( p.y < dCV ) p.y = dCV;
   int dw = double(p.x) / 2.0 + dH,
       dh = double(p.y) / 2.0 + dV;


   if( left ) dx += dw;
   if( right ) dx -= dw;
   if( top ) dy += dh;
   if( bottom ) dy -= dh;

   Shift( dx, dy );

   return dx || dy;
 }

int TMyMDIChild::staXSh, TMyMDIChild::staYSh,
    TMyMDIChild::curXSh, TMyMDIChild::curYSh;

BOOL TMyMDIChild::ScrollForShow( int x, int y, BOOL bFlSetCursor )
 {
   RECT r;
   int hS = 0, vS = 0;
   GetClientRect( HWindow, &r );
   int dH = double(r.right) * MDI_d_AutoScrollH,
       dV = double(r.bottom) * MDI_d_AutoScrollV;

   if( x <= r.left ) hS = r.left - x + dH;
   else if( x >= r.right ) hS = r.right - x - dH;

   if( y <= r.top ) vS = r.top - y + dV;
   else if( y >= r.bottom ) vS = r.bottom - y - dV;

   Shift( hS, vS );
   if( hS || vS )
    {
      //static const int hFeed = 15, vFeed = 15;

      int shX = hS, shY = vS;      
      if( shX < 0 ) shX -= MDI_d_AutoScrollFeedH;
      else if( shX > 0 ) shX += MDI_d_AutoScrollFeedH;
      if( shY < 0 ) shY -= MDI_d_AutoScrollFeedV;
      else if( shY > 0 ) shY += MDI_d_AutoScrollFeedV;
      POINT p; p.x = mouseX + shX; p.y = mouseY + shY;
      ClientToScreen( HWindow, &p );
      if( bFlSetCursor == TRUE ) SetCursorPos( p.x , p.y );    
    } 

   RequiestCallMouseHandl();

   return hS || vS;
 }


void TMyMDIChild::DrawArrow( HDC hdc, int iX1, int iY1, int iX2, int iY2 )
 {
   const double dMinDistance = MDI_d_DrawArrowMinDistance;
   const double dCorner = GRAD_TO_RAD(180 - 20);
   //const int iLength = 15.0 / ((GetScale() < 1 && GetScale()) ? GetScale():1);
   const int iLength = MDI_i_DrawArrowLenght;

   double dXs = iLength * cos( dCorner ),
	  dYs = iLength * sin( dCorner );
   int iDx = iX2 - iX1, iDy = iY2 - iY1;
   double dD = sqrt( long(iDx) * long(iDx) + long(iDy) * long(iDy) );
   if( dD < dMinDistance ) return;
   double dCosM = double(iDx) / dD,
          dSinM = double(iDy) / dD;

   double dX = dXs * dCosM - dYs * dSinM,
	  dY = dXs * dSinM + dYs * dCosM;

   int x1 = iX2 + dX, y1 = iY2 + dY;

   dXs = iLength * cos( -dCorner );
   dYs = iLength * sin( -dCorner );
   dX = dXs * dCosM - dYs * dSinM;
   dY = dXs * dSinM + dYs * dCosM;

   int x2 = iX2 + dX, y2 = iY2 + dY;

   MoveTo( hdc, iX2, iY2 );
   LineTo( hdc, x1, y1 );
   MoveTo( hdc, iX2, iY2 );
   LineTo( hdc, x2, y2 );
 }


void TMyMDIChild::CreationAligment( PTSurface pSf )
 {
   TVector vAlign;
   TVertex verAlign;   

   /*pSf->Move( TMyMDIChild::pfdFace->insertPoint.x, TMyMDIChild::pfdFace->insertPoint.y,
	      TMyMDIChild::pfdFace->insertPoint.z );*/

   if( TMyMDIChild::pfdFace->bFlAligmented == TRUE )
    {
      if( pScena->IsUsedBaseUSC() == TRUE ) pScena->RecalcBodyUserToBase( *pSf );
      pSf->CalcNormals();  
      pSf->GetCreationAlignData( vAlign, verAlign );
      pSf->Move( -verAlign.x, -verAlign.y, -verAlign.z );
      verAlign = TVertex( -1, 0, 0, 0 );
      if( pScena->IsUsedBaseUSC() == TRUE ) pScena->RecalcBodyBaseToUser( *pSf );
      
      Aligment( *pSf, vAlign, verAlign, NULL, FALSE );
    }

   pSf->Move( TMyMDIChild::pfdFace->insertPoint.x, TMyMDIChild::pfdFace->insertPoint.y,
       TMyMDIChild::pfdFace->insertPoint.z );
 }

int TMyMDIChild::Aligment( RTSurface rSf, RTVector rVect, RTVertex rVBody, PTFacet pF, BOOL bFlReqv )
 {
   BOOL bFlForward = TRUE;
   PTAligmentDialog pAlignDlg;
   int res;

   if( bFlReqv == TRUE )
    {
      rSf.bFlObolVis = TRUE;
      GetScena()->DrawObol( rSf );

      if( (pAlignDlg = new TAligmentDialog( this, (LPSTR)MAKEINTRESOURCE(DlgAligment), bFlForward, pMRC )) && !pAlignDlg->Status )
        res = GetApplication()->ExecDialog( pAlignDlg );
      else
       {
	 GetApplication()->Error( pAlignDlg ? pAlignDlg->Status:ER_CREATEOBJECT );
	 rSf.bFlObolVis = FALSE;
	 InvalidateRect( HWindow, NULL, TRUE );
         if( pAlignDlg ) pAlignDlg->CloseWindow();

         return 0;
       }

      rSf.bFlObolVis = FALSE;
    }
   else res = ALIGNDLG_IDALIGMENT_AND_CONNECT;

   if( res == IDCANCEL )
    {
      InvalidateRect( HWindow, NULL, TRUE );
      return 0;
    }


   if( pScena->IsUsedBaseUSC() == TRUE ) pScena->RecalcBodyUserToBase( rSf );

   if( bFlForward == FALSE )  rVect = -rVect;


   T3DPoint ptKey = pF ? pF->GetGeomCenter():(T3DPoint)rVBody;
   rSf.Move( -ptKey.x, -ptKey.y, -ptKey.z );
   GetScena()->AligmentZ( rSf, rVect );
   rSf.Move( ptKey.x, ptKey.y, ptKey.z );
   if( res == ALIGNDLG_IDALIGMENT_AND_CONNECT )
     rSf.Move( 0, 0, -rVBody.z );

   if( pScena->IsUsedBaseUSC() == TRUE ) pScena->RecalcBodyBaseToUser( rSf );

   if( rSf.NeedCreateObol() == TRUE ) GetScena()->NewObol( rSf );

   return 1;
 }

static void near pascal CorrectAngle( double& dA, double dSin, double dCos )
 {
   if( dSin < 0 && dCos > 0 ) dA = -dA;
   else if( dSin > 0 && dCos < 0 ) dA = M_PI - dA;
   else if( dSin < 0 && dCos < 0 ) dA = dA - M_PI;
 }

void TMyMDIChild::VPortAligment( RTSurface, RTFacet rF )
 {
   TVector vNorm = rF.GetNewelNormal();

   TViewPoint vp;
   double anH, anV;
   GetVpoint( vp, anH, anV );
   double r = sqrt( vNorm.y * vNorm.y + vNorm.z * vNorm.z ),
	  //p = sqrt( vNorm.x * vNorm.x + vNorm.y * vNorm.y + vNorm.z * vNorm.z );
	  p = !vNorm;

   if( r < MDI_d_VPortAligmentNormZero ) ChangeVPoint( vp, 0, GRAD_TO_RAD(90) );
   else
    {
      double dSinA = vNorm.y / r,
	     dSinB = -vNorm.x / p,
	     dCosA = vNorm.z / r,
	     dCosB = r / p;

      double dA = fabs( asin(dSinA) ),
	     dB = fabs( asin(dSinB) );

      CorrectAngle( dA, dSinA, dCosA );
      CorrectAngle( dB, dSinB, dCosB );

      if( !dA && !dB ) vp = TUCS::VP_Front;
      ChangeVPoint( vp, dA, dB );
    }


   GenTitle( pGlobalFManeger->GetInfo(GetScId()) );
   InvalidateRect( HWindow, NULL, TRUE );
 }

void TMyMDIChild::Smoth( RTSurface rSf, RTFacet rF )
 {
   PTSmothDialog pSmothDlg;

   if( !GetScena() ) return;

   int iAngleSmoth = GetScena()->AngleSmoth();
   int iSmothGrp = (rF.iSmothGroup > -1) ? rF.iSmothGroup:0;
   int iIdBtn;
                 
   int res;
   if( (pSmothDlg = new TSmothDialog( this, (LPSTR)MAKEINTRESOURCE(DlgSmoth),
     iAngleSmoth, iSmothGrp, iIdBtn, pMRC )) && !pSmothDlg->Status )
     res = GetApplication()->ExecDialog( pSmothDlg );
   else
    {
      GetApplication()->Error( pSmothDlg ? pSmothDlg->Status:ER_CREATEOBJECT );
      if( pSmothDlg ) pSmothDlg->CloseWindow();
      return;
    }

   if( res == IDCANCEL ) return;


   GetScena()->AngleSmoth( iAngleSmoth );

   SetCapture( HWindow );
   HCURSOR hcOld = SetCursor( LoadCursor(NULL, IDC_WAIT) );

   switch( iIdBtn )
    {      
      case SMOTHDLG_BTN_AUTOSMOTH:
	rSf.CalcNormals();
	rSf.DetectCommonFacets();
	rSf.AutoSmoth( iAngleSmoth );	
        break;       

      case SMOTHDLG_BTN_SMOTHALL:
	rSf.SmothAllInOneGroup( 1 );
	break;

      case SMOTHDLG_BTN_CLEARALL:
	rSf.SmothAllInOneGroup( 0 );
	break;

      case SMOTHDLG_BTN_ASSIGNSMOTH:
	rF.iSmothGroup = iSmothGrp;
	break;
    }

   SetCursor( hcOld );
   ReleaseCapture();
 }

void TMyMDIChild::AligmentFacet( RTSurface rSf, RTFacet rF, BOOL bFlReqv )
 {
   TVector vVertexNormal = rF.GetNewelNormal( (pScena->IsUsedBaseUSC() == TRUE) ? PTUCS(this):NULL );
   //DoubleListIterator dblIt( rF.listAgesItem );

   Aligment( rSf, vVertexNormal, *(RTAdgeItem(rF.listAgesItem.peekAtHead()).pAdge->pV1),
             &rF, bFlReqv );
 }

void TMyMDIChild::OmniAdjust( PTSurface pSf )
 {
   if( !pSf || pSf->isA() != TYP_TOMNI ) return;


   PTOmniDialog pOmniDlg;

   PTOmni pOTmp;
   PTOmni pO = TSPtr( pSf, pOTmp );
   RGBQUAD rgbOmni;
   rgbOmni << TRGB( pO->R(), pO->G(), pO->B() );
   double dMultOmni = pO->GetMult();
   BOOL bFlOn = pO->IsOn(),
	bFlCastShadows = pO->Shadow();
   char cName[ 128 ];
   sprintf( cName, "Omni#%d", (int)pO->Index() );


   int res;
   if( (pOmniDlg = new TOmniDialog( this, (LPSTR)MAKEINTRESOURCE(DlgOmniAdjust),
     rgbOmni, dMultOmni, bFlOn, bFlCastShadows, cName, pMRC )) && !pOmniDlg->Status )
     res = GetApplication()->ExecDialog( pOmniDlg );
   else
    {
      GetApplication()->Error( pOmniDlg ? pOmniDlg->Status:ER_CREATEOBJECT );
      if( pOmniDlg ) pOmniDlg->CloseWindow();
      return;
    }

   if( res == IDOK )
    {
      pO->RGBColor( rgbOmni );
      pO->Mult( dMultOmni );
      pO->SetSwitch( bFlOn );
      pO->Shadow( bFlCastShadows );
    }
 }

int TMyMDIChild::CreateOmni( POINT _FAR& pt_ )
 {
   DPtoLP( hdcOwn, &pt_, 1 );
   T3DPoint pt = T3DPoint( pt_.x, pt_.y, 0 );
   RecalcBaseToUser( &pt, 1 );
   RGBQUAD rgb;
   rgb.rgbRed =   200;
   rgb.rgbGreen = 200;
   rgb.rgbBlue =  200;

   TOmniInit oiInit( pt, rgb, 1 );

   PTOmni pOmni = new TOmni( GetScena()->GetFreeIndex(), &oiInit );   
   if( pOmni && !pOmni->Status )
    {
      GetScena()->AddBody( pOmni );
      pOmni->pfdKey = NULL;
    }
   else if( pOmni )
   {
     GetApplication()->Error( pOmni->Status );
     delete pOmni;
     return FALSE;
   }

 return TRUE;
}

int TMyMDIChild::AligmentVertex( POINT _FAR &pt, BOOL bFlReqv )
 {
   POINT ptTmp[3]; ptTmp[0] = pt;
   PTVertexData pVD = GetNearstVertex( ptTmp );

   if( !pVD ) return 0;

   if( !pVD->pSf  || !pVD->pVert ||
       FilterBody( pVD->pSf ) == FALSE
     )
     return 0;
   

   if( pScena->IsUsedBaseUSC() == TRUE ) pScena->RecalcBodyUserToBase( *pVD->pSf );
   pVD->pSf->CalcNormals();
   TVector vVertexNormal = pVD->pSf->CalcVertexNormal( *pVD->pVert );
   if( pScena->IsUsedBaseUSC() == TRUE ) pScena->RecalcBodyBaseToUser( *pVD->pSf );

   int res = Aligment( *pVD->pSf, vVertexNormal, *pVD->pVert, NULL, bFlReqv );

   delete pVD;
   return res;
 }


PTVertexData TMyMDIChild::GetNearstVertex( LPPOINT pPoint )
 {
   int widt = GetSystemMetrics( SM_CXCURSOR ) - 3,
       high = GetSystemMetrics( SM_CXCURSOR ) - 3;

   TRECT rCursor( pPoint[0].x - widt / 2, pPoint[0].y - high / 2,
		  pPoint[0].x - widt / 2 + widt, pPoint[0].y - high / 2 + high
		);

   pPoint[1].x = rCursor.left;
   pPoint[1].y = rCursor.top;
   pPoint[2].x = rCursor.right;
   pPoint[2].y = rCursor.bottom;

   DPtoLP( hdcOwn, pPoint, 3 );

   rCursor.left = pPoint[1].x;
   rCursor.top = pPoint[1].y;
   rCursor.right = pPoint[2].x;
   rCursor.bottom = pPoint[2].y;
   rCursor.Normalyze();

   GetScena()->SetHardwareEnviron( (PTUCS)this, hdcOwn, (PTWindow)this );

   return GetScena()->GetNearstVertexInRange( rCursor, pPoint[0] );
 }


void TMyMDIChild::PlaceHilite( POINT pt, RTSurface rOmni_ )
 {
   PTOmni pTmpGet;
   RTOmni rOmni = *TSPtr( &rOmni_, pTmpGet );   

   TOPData dta( &pt, (PTUCS)this, hdcOwn );
   GetScena()->SetHardwareEnviron( (PTUCS)this, hdcOwn, (PTWindow)this );
   GetScena()->FindObjectAndPoint( pt.x, pt.y, dta );

   if( !dta.pSfFound )
    {
      BWCCMessageBox( HWindow, "No surface in this point", "Message",
	  MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL );
      return;
    }

   char cBuf[256];
   sprintf( cBuf, "Distance on Omni#%d --> Surface#%d", (int)rOmni.Index(),
     (int)dta.pSfFound->Index() );
   int iDistance;
   PTPlaceHiliteDlg pPHDlg;
   int res;
   if( (pPHDlg = new TPlaceHiliteDlg( this, (LPSTR)MAKEINTRESOURCE(DlgPlaceHilite),
     iDistance, cBuf, pMRC )) && !pPHDlg->Status )
     res = GetApplication()->ExecDialog( pPHDlg );
   else
    {
      GetApplication()->Error( pPHDlg ? pPHDlg->Status:ER_CREATEOBJECT );
      if( pPHDlg ) pPHDlg->CloseWindow();
      return;
    }

   if( res != IDOK ) return;


   TVector vNorm = dta.pFacetFound->GetNewelNormal();
   double t = double(iDistance) / sqrt(vNorm.x * vNorm.x + vNorm.y * vNorm.y + vNorm.z * vNorm.z);
   T3DPoint ptPlace( t * vNorm.x + dta.ptFound.x,
		     t * vNorm.y + dta.ptFound.y,
		     t * vNorm.z + dta.ptFound.z
		   );
   rOmni.Place( ptPlace );

   RedrawVports();
 }

void TMyMDIChild::SetupGlobalBP( POINT _FAR &pt )
 {
/*   int widt = GetSystemMetrics( SM_CXCURSOR ) - 3,
       high = GetSystemMetrics( SM_CXCURSOR ) - 3;

   TRECT rCursor( pt.x - widt / 2, pt.y - high / 2,
		  pt.x - widt / 2 + widt, pt.y - high / 2 + high
		);

   POINT ptTmp[3];
   ptTmp[0] = pt;
   ptTmp[1].x = rCursor.left;
   ptTmp[1].y = rCursor.top;
   ptTmp[2].x = rCursor.right;
   ptTmp[2].y = rCursor.bottom;

   DPtoLP( hdcOwn, ptTmp, 3 );

   rCursor.left = ptTmp[1].x;
   rCursor.top = ptTmp[1].y;
   rCursor.right = ptTmp[2].x;
   rCursor.bottom = ptTmp[2].y;
   rCursor.Normalyze();

   GetScena()->SetHardwareEnviron( (PTUCS)this, hdcOwn, (PTWindow)this );
   PTVertexData pVD = GetScena()->GetNearstVertexInRange( rCursor, ptTmp[0] );
   */

   POINT ptTmp[3]; ptTmp[0] = pt;
   PTVertexData pVD = GetNearstVertex( ptTmp );

   if( !pVD )
    {   
      T3DPoint ptPoint = T3DPoint( ptTmp[0].x, ptTmp[0].y, 0 );
      RecalcBaseToUser( &ptPoint, 1 );
      SetGlobalBase( ptPoint );
    }
   else
    {
      pVD->pSf->bFlObolVis = TRUE;
      GetScena()->DrawObol( *(pVD->pSf) );

      TPlaicement plaicLocation = PL_Center;
      PTBPDialog pBPDlg = new TBPDialog( this, "DlgBP", plaicLocation, pMRC );
      int res;
      //TDialog


      if( pBPDlg && !pBPDlg->Status ) res = GetApplication()->ExecDialog( pBPDlg );
      else
       {
	 GetApplication()->Error( pBPDlg ? pBPDlg->Status:ER_CREATEOBJECT );
	 pVD->pSf->bFlObolVis = FALSE;
	 InvalidateRect( HWindow, NULL, TRUE );

	 if( pBPDlg ) pBPDlg->CloseWindow();//delete pBPDlg;
	 delete pVD;
         return;
       }

      pVD->pSf->bFlObolVis = FALSE;

      if( res == ID_POINT || res == IDOK )
       {
	 T3DPoint ptPoint = T3DPoint( ptTmp[0].x, ptTmp[0].y, 0 );
         RecalcBaseToUser( &ptPoint, 1 );
         SetGlobalBase( ptPoint );
       }
      else if( res == ID_VERTEX )
       {
         T3DPoint ptPoint = T3DPoint( pVD->pVert->x, pVD->pVert->y, pVD->pVert->z );      
	 SetGlobalBase( ptPoint );
       }
      else if( res == ID_BODY )
       {
	 T3DPoint ptCenter = pVD->pSf->GetGeomCenter();

	 switch( plaicLocation )
	  {
	    case PL_Center:
	      break;

            case PL_TopLeft:
	      ptCenter.x = pVD->pSf->minX;
	      ptCenter.y = pVD->pSf->maxY;
	      break;

	    case PL_Top:
	      ptCenter.y = pVD->pSf->maxY;
	      break;

	    case PL_TopRight:
	      ptCenter.x = pVD->pSf->maxX;
	      ptCenter.y = pVD->pSf->maxY;
	      break;

	    case PL_Left:
	      ptCenter.x = pVD->pSf->minX;
	      break;

	    case PL_Right:
	      ptCenter.x = pVD->pSf->maxX;
	      break;

	    case PL_BottomLeft:
	      ptCenter.x = pVD->pSf->minX;
	      ptCenter.y = pVD->pSf->minY;
	      break;

	    case PL_Bottom:
	      ptCenter.y = pVD->pSf->minY;
	      break;

	    case PL_BottomRight:
	      ptCenter.x = pVD->pSf->maxX;
	      ptCenter.y = pVD->pSf->minY;
	      break;
	  };
	 SetGlobalBase( ptCenter );
       }
    
      delete pVD;
      if( res == IDCANCEL ) return;
    }

   SetVisibleBasePoint( TRUE );
   InvalidateRect( HWindow, NULL, TRUE );   
 }

void TMyMDIChild::InitMode( ModeChild mcM )
 {
   char cBuf[80];
   if( ValidWindowOper( MC_SetupUCS ) == FALSE ) return;

   //TMyMDIChild::psfSelected = NULL;

   if( GetScena()->SelectedCount(TMyMDIChild::pCurFilter) && TMyMDIChild::IsTransformSelected() == TRUE &&
       !TMyMDIChild::psfSelected && mcM != MC_MoveBody &&
				    mcM != MC_3DScaleBody &&
				    mcM != MC_2DScaleBody &&
				    mcM != MC_RotateBody &&
				    mcM != MC_SetupGlobalBP &&
				    mcM != MC_AligmentVertex &&
				    mcM != MC_AligmentFacet &&

				    mcM != MC_SelectFacet &&
				    mcM != MC_CreateOmni &&
				    mcM != MC_OmniAdjust &&

				    mcM != MC_VPortAligment &&
				    mcM != MC_PlaceHilite
				    
     )
     return;

   TSendTxt stSend;
   stSend.handle = 0;
   stSend.lpTxt = "";
   TAutoStringRc s1_;

   switch( mcM )
    {
      case MC_MoveBody:
	s1_ = TAutoStringRc( hInstRC, ID_MSG_46 );
	stSend.lpTxt = s1_();
	//stSend.lpTxt = "Move mouse for drag object. ENTER - change move axis, SHIFT - clone";
	TMyMDIChild::dDX = TMyMDIChild::dDY = TMyMDIChild::dDZ = 0;
	break;

      case MC_DeleteBody:
	if( !TMyMDIChild::psfSelected ) return;

	TMyMDIChild::psfSelected->bFlObolVis = TRUE;
	GetScena()->DrawObol( *TMyMDIChild::psfSelected );

	TAutoStringRc ss1_( hInstRC, ID_MSG_65 ),
		      ss2_( hInstRC, ID_TITLE_WARNING, 20 );
	sprintf( cBuf, /*"Delete object %d  ?"*/ss1_(), TMyMDIChild::psfSelected->Index() );
	int res = BWCCMessageBox( HWindow, cBuf, ss2_(),
	  MB_ICONEXCLAMATION | MB_YESNO | MB_APPLMODAL );
	if( res != IDYES )
	 {
	   TMyMDIChild::psfSelected->bFlObolVis = FALSE;
	   InvalidateRect( HWindow, NULL, TRUE );
           return;
         }

	TMyMDIChild::psfSelected->bFlObolVis = FALSE;
	GetScena()->DeleteBody( *TMyMDIChild::psfSelected );
	RedrawVports();	
	return;

      case MC_3DScaleBody:
	s1_ = TAutoStringRc( hInstRC, ID_MSG_47 );
	stSend.lpTxt = s1_();
	//stSend.lpTxt = "Move mouse for select new scale. ENTER - change scale axis, SHIFT - clone";
	TMyMDIChild::dScaleX = TMyMDIChild::dScaleY = TMyMDIChild::dScaleZ = 100;
	break;

      case MC_2DScaleBody:
	s1_ = TAutoStringRc( hInstRC, ID_MSG_48 );
	stSend.lpTxt = s1_();
	//stSend.lpTxt = "Move mouse for select scale. SHIFT - clone";
	TMyMDIChild::dScaleX = 100;
	break;

      case MC_RotateBody:
	s1_ = TAutoStringRc( hInstRC, ID_MSG_49 );
	stSend.lpTxt = s1_();
	//stSend.lpTxt = "Move mouse for rotate object. ENTER - change axis, SHIFT - clone";
	TMyMDIChild::anX = TMyMDIChild::anY = TMyMDIChild::anZ = 0;
	//TMyMDIChild::reCurExis = AX_x;
	break;

      case MC_SelectBody:
	if( !TMyMDIChild::psfSelected ) return;

	if( GetApplication()->MainWindow && GetApplication()->MainWindow->isA() == classtypeMDIFrame )
	 {
	   BOOL bFlInvert = BOOL(SendMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, GetScId(), MAKELONG(0, UCM_GET_INVERTSELMODE_FLAG) ));

	   BOOL bFlKeySelState = TMyMDIChild::psfSelected->IsSelected();
	   TMyMDIChild::psfSelected->Select( (bFlInvert == FALSE) ? TRUE:FALSE );


	   TAutoStringRc ss1_( hInstRC, ID_MSG_65, 50 ),
			 ss2_( hInstRC, ID_MSG_67, 20 ),
			 ss3_( hInstRC, ID_MSG_68, 20 );
	   wsprintf( cBuf, /*"Body %d is %s"*/ss1_(), TMyMDIChild::psfSelected->Index(),
	     (TMyMDIChild::psfSelected->IsSelected() == FALSE) ? /*"unselected"*/ss2_():ss3_()/*"selected"*/
		);
	   stSend.lpTxt = cBuf;
	   SendMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );

	   if( bFlKeySelState != TMyMDIChild::psfSelected->IsSelected() ) RedrawVports();

           return;
	 }

      case MC_PlaceHilite:
	static POINT ptKeep;
	if( TMyMDIChild::mcMode == MC_GetPoint )
	 {
	   ptKeep = TMyMDIChild::ptSelected;
	   SelectAndOp( MC_PlaceHilite );
	   stSend.lpTxt = "Now, select 'Omni' to place hilite";
	   SendMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );	   
	   TMyMDIChild::pCurFilter = FilterLights;
           return;
	 }
	else if( TMyMDIChild::mcMode == MC_Select )
	 {
	   if( !TMyMDIChild::psfSelected || TMyMDIChild::psfSelected->isA() != TYP_TOMNI )
            {
	      BWCCMessageBox( HWindow, "For place hilite need select Omni",
		"Message", MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL );
	      //SelectAndOp( MC_PlaceHilite );
	      GetPointAndOp( MC_PlaceHilite );
	      return;
            }
	   PlaceHilite( ptKeep, *TMyMDIChild::psfSelected );
	   GetPointAndOp( MC_PlaceHilite );
         }
	return;

      case MC_SetupGlobalBP:
	SetupGlobalBP( TMyMDIChild::ptSelected );
	GetPointAndOp( MC_SetupGlobalBP );
	return;

      case MC_SelectFacet:
	POINT ptTmp[3]; ptTmp[0] = TMyMDIChild::ptSelected;
	PTVertexData pVD = GetNearstVertex( ptTmp );
	if( !pVD )
	 {
	   GetFacetAndOp( TMyMDIChild::mcNextOp2 );
	   return;
         }
	//**********
	TMyMDIChild::pfacSelected = NULL;
	TMyMDIChild::psfSelected =  pVD->pSf;
	TMyMDIChild::pvSelected =   pVD->pVert;
	delete pVD;

	SetCapture( HWindow );
	ShowCursor( FALSE );


	TMyMDIChild::phazeOfCurMode = 0;
	bFlModeActive = TRUE;

	TMyMDIChild::dblCommonFacets.flush( TShouldDelete::Delete );
	TMyMDIChild::psfSelected->GetCommonFacetsList(
	  TMyMDIChild::dblCommonFacets, *TMyMDIChild::pvSelected );

	if( !TMyMDIChild::dblCommonFacets.getItemsInContainer() )
	 {
	   ReleaseCapture();
	   ShowCursor( TRUE );
	   bFlModeActive = FALSE;
	   TMyMDIChild::psfSelected = NULL;
	   TMyMDIChild::pvSelected = NULL;

	   TAutoStringRc str_( hInstRC, ID_MSG_10 );
	   TAutoStringRc ttl_( hInstRC, ID_TITLE_MESSAGE );
	   BWCCMessageBox( HWindow, str_(),
	    ttl_(), MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL );
	   //"This object cann't facet aligmented"

	   GetFacetAndOp( TMyMDIChild::mcNextOp2 );
	   return;
         }

	TMyMDIChild::dbiFacets = DoubleListIterator( TMyMDIChild::dblCommonFacets );
	TMyMDIChild::pfacSelected = RTFacetItem( TMyMDIChild::dbiFacets.current() ).pFacet;
	TMyMDIChild::dDX = MAXINT;

	POINT p; p.x = mouseX; p.y = mouseY;
        DPtoLP( hdcOwn, &p, 1 );
        TMyMDIChild::curXSh = TMyMDIChild::staXSh = p.x;
        TMyMDIChild::curYSh = TMyMDIChild::staYSh = p.y;

	SetModeChild( MC_SelectFacet );
	TMyMDIChild::mcNextOp = TMyMDIChild::mcNextOp2;

	TMessage msg; _fsetmem( &msg, sizeof(TMessage), 0 );
	msg.Message = WM_MOUSEMOVE;
	SelectFacetMouseHandler( msg, this, hdcOwn );

	return;

      case MC_Smoth:
	if( TMyMDIChild::psfSelected  &&
	    FilterBody(TMyMDIChild::psfSelected) == TRUE
	  )	 
	  Smoth( *TMyMDIChild::psfSelected, *TMyMDIChild::pfacSelected );  


	GetFacetAndOp( MC_Smoth );
	return;

      case MC_OmniAdjust:
	OmniAdjust( TMyMDIChild::psfSelected );
	SelectAndOp( MC_OmniAdjust );
        return;

      case MC_CreateOmni:
	if( CreateOmni(TMyMDIChild::ptSelected) ) RedrawVports();

	GetPointAndOp( MC_CreateOmni );
	return;

      case MC_AligmentVertex:	
        if( AligmentVertex( TMyMDIChild::ptSelected ) ) RedrawVports();

	GetPointAndOp( MC_AligmentVertex );
	return;


      case MC_AligmentFacet:
	if( TMyMDIChild::psfSelected  &&
	    FilterBody(TMyMDIChild::psfSelected) == TRUE
	  )
         {
	   AligmentFacet( *TMyMDIChild::psfSelected, *TMyMDIChild::pfacSelected );
	   RedrawVports();
         }

	GetFacetAndOp( MC_AligmentFacet );
	//SetModeChild( TMyMDIChild::mcKeyMode );
	//TMyMDIChild::mcNextOp = TMyMDIChild::mcKeyNextMode;

	return;

      case MC_VPortAligment:
	if( TMyMDIChild::psfSelected  &&
	    FilterBody(TMyMDIChild::psfSelected) == TRUE
	  )         
	  VPortAligment( *TMyMDIChild::psfSelected, *TMyMDIChild::pfacSelected );
	   
	GetFacetAndOp( MC_VPortAligment );
	return;
      
    };

   SetCapture( HWindow );
   ShowCursor( FALSE );

   if( TMyMDIChild::IsTransformSelected() == TRUE && GetScena()->SelectedCount(TMyMDIChild::pCurFilter) )
     TMyMDIChild::psfObol = new TObol( GetScena() );
   else
     //TMyMDIChild::psfObol = new TObol( TMyMDIChild::psfSelected );
     TMyMDIChild::psfObol = TMyMDIChild::psfSelected->CreateObol();
   TMyMDIChild::pscObol = new TScena( -1, GetApplication() );
   TMyMDIChild::pscObol->SetHardwareEnviron( (PTUCS)this, hdcOwn, (PTWindow)this );
   if( TMyMDIChild::psfObol ) TMyMDIChild::pscObol->AddBody( TMyMDIChild::psfObol );
   TMyMDIChild::pscObol->SetHideGeom( FALSE );

   SetCursorInCenter( TMyMDIChild::psfObol );


   POINT p; p.x = mouseX; p.y = mouseY;
   DPtoLP( hdcOwn, &p, 1 );
   TMyMDIChild::curXSh = TMyMDIChild::staXSh = p.x;
   TMyMDIChild::curYSh = TMyMDIChild::staYSh = p.y;


   TMyMDIChild::phazeOfCurMode = 0;
   bFlModeActive = TRUE;///////
   SetModeChild( mcM );

   SendMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );

   TMessage msg; _fsetmem( &msg, sizeof(TMessage), 0 );
   //MoveMouseLBHandler( msg, this );

   switch( mcM )
    {
      case MC_MoveBody:
	MoveMouseHandler( msg, this, hdcOwn );
	break;

      case MC_3DScaleBody:
	Sc3MouseHandler( msg, this, hdcOwn );
	break;

      case MC_2DScaleBody:
	Sc2MouseHandler( msg, this, hdcOwn );
	break;

      case MC_RotateBody:
	RotMouseHandler( msg, this, hdcOwn );
	break;
    }
 }


PTScena TMyMDIChild::GetClone() const
 {
   PTScena pScClon = NULL;

   HCURSOR HcrsorOld = SetCursor( LoadCursor(NULL, IDC_WAIT) );
   ShowCursor( TRUE );

   if( TMyMDIChild::IsTransformSelected() == TRUE && GetScena()->SelectedCount(TMyMDIChild::pCurFilter) )
     pScClon = GetScena()->CloneAllSelected();
   else if( TMyMDIChild::IsTransformAsTransforming() == TRUE )
    {
      if( TMyMDIChild::psfSelected && TMyMDIChild::psfSelected->IsSelected() == FALSE )
        pScClon = GetScena()->Clone( *TMyMDIChild::psfSelected );
      PTScena pScClon2 = GetScena()->CloneAllSelected();

      if( pScClon && pScClon2 ) { pScClon->Merge( *pScClon2 ); delete pScClon2; }
      else if( pScClon2 ) pScClon = pScClon2;
    }
   else
     pScClon = GetScena()->Clone( *TMyMDIChild::psfSelected );

     if( pScClon->Status == -1 )
      {
	TAutoStringRc str_( hInstRC, ID_MSG_11 );
	TAutoStringRc ttl_( hInstRC, ID_TITLE_ERROR );
	BWCCMessageBox( HWindow, str_(),
         ttl_(),
	 MB_ICONSTOP | MB_OK | MB_APPLMODAL
		    );
	//"Not enought memory for create clone"
	delete pScClon; pScClon = NULL;
      }


   SetCursor( HcrsorOld );
   ShowCursor( FALSE );

   return pScClon;
 }


void TMyMDIChild::SetMove( int dx, int dy )
 {
   switch( TMyMDIChild::masMoveAxis.GetCurrent() )
    {
      //TMoveAxis { MA_xy. MA_x, MA_y, MA_z };
      //dDX, dDY, dDZ;
      case TMoveAxisSeqvensor::MA_xy:
	TMyMDIChild::dDX += dx; TMyMDIChild::dDY += dy;
	break;

      case TMoveAxisSeqvensor::MA_x:
	TMyMDIChild::dDX += ((abs(dx) > abs(dy) ) ? dx:dy);
	break;

      case TMoveAxisSeqvensor::MA_y:
	TMyMDIChild::dDY += ((abs(dx) > abs(dy) ) ? dx:dy);
	break;

      case TMoveAxisSeqvensor::MA_z:
	TMyMDIChild::dDZ += ((abs(dx) > abs(dy) ) ? dx:dy);
	break;
    }
 }

void TMyMDIChild::ScaleFunc( RTSurface rSurf )
 {
   T3DPoint pt = GetBP( &rSurf );

   switch( TMyMDIChild::sasScaleAxis.GetCurrent() )
    {
      case TScaleAxisSeqvensor::SA_3d:
	GetScena()->ScaleXYZ( rSurf, TMyMDIChild::dScaleX,
				     TMyMDIChild::dScaleY,
				     TMyMDIChild::dScaleZ,
	pt );
	break;

      case TScaleAxisSeqvensor::SA_x:
	GetScena()->ScaleXYZ( rSurf, TMyMDIChild::dScaleX, 100, 100, pt );
	break;

      case TScaleAxisSeqvensor::SA_y:
	GetScena()->ScaleXYZ( rSurf, 100, TMyMDIChild::dScaleY, 100, pt );
	break;

      case TScaleAxisSeqvensor::SA_z:
	GetScena()->ScaleXYZ( rSurf, 100, 100, TMyMDIChild::dScaleZ, pt );
	break;
    }

 }


void TMyMDIChild::RotFunc( RTSurface rSurf )
 {
   T3DPoint pt = GetBP( &rSurf );

   switch( TMyMDIChild::resCurExis.GetCurrent() )
    {
      case TRotExisSeqvensor::AX_x:
	//GetScena()->RotateX( rSurf, TMyMDIChild::anX );
	GetScena()->RotateXYZ( rSurf, TMyMDIChild::anX, 0, 0, pt );
	break;

      case TRotExisSeqvensor::AX_y:
	//GetScena()->RotateY( rSurf, TMyMDIChild::anY );
	GetScena()->RotateXYZ( rSurf, 0, TMyMDIChild::anY, 0, pt );
	break;

      case TRotExisSeqvensor::AX_z:
	//GetScena()->RotateZ( rSurf, TMyMDIChild::anZ );
	GetScena()->RotateXYZ( rSurf, 0, 0, TMyMDIChild::anZ, pt );
	break;
    }
 }

inline double SetScaleCheck( double dCheck )
 {
   return (fabs(dCheck) < MDI_d_MinScaleAbs) ? MDI_d_MinScaleAbs:dCheck;
 }

void TMyMDIChild::SetScale( double delta )
 {
   switch( TMyMDIChild::sasScaleAxis.GetCurrent() )
    {
      case TScaleAxisSeqvensor::SA_3d:
	TMyMDIChild::dScaleX += delta;
	TMyMDIChild::dScaleX = SetScaleCheck( TMyMDIChild::dScaleX );
	TMyMDIChild::dScaleZ = TMyMDIChild::dScaleY = TMyMDIChild::dScaleX;
	break;

      case TScaleAxisSeqvensor::SA_x:
	TMyMDIChild::dScaleX += delta;
	TMyMDIChild::dScaleX = SetScaleCheck( TMyMDIChild::dScaleX );
	break;

      case TScaleAxisSeqvensor::SA_y:
	TMyMDIChild::dScaleY += delta;
	TMyMDIChild::dScaleY = SetScaleCheck( TMyMDIChild::dScaleY );
	break;

      case TScaleAxisSeqvensor::SA_z:
	TMyMDIChild::dScaleZ += delta;
	TMyMDIChild::dScaleZ = SetScaleCheck( TMyMDIChild::dScaleZ );
	break;
    }
 }

void TMyMDIChild::SetAngle( double delta )
 {
   switch( TMyMDIChild::resCurExis.GetCurrent() )
    {
      case TRotExisSeqvensor::AX_x:
	TMyMDIChild::anX += delta;
	break;

      case TRotExisSeqvensor::AX_y:
	TMyMDIChild::anY += delta;
	break;

      case TRotExisSeqvensor::AX_z:
	TMyMDIChild::anZ += delta;
	break;
    }
 }

double TMyMDIChild::GetScaleSys()
 {
   switch( TMyMDIChild::sasScaleAxis.GetCurrent() )
    {
      case TScaleAxisSeqvensor::SA_3d:
	return TMyMDIChild::dScaleX;

      case TScaleAxisSeqvensor::SA_x:
	return TMyMDIChild::dScaleX;

      case TScaleAxisSeqvensor::SA_y:
	return TMyMDIChild::dScaleY;

      case TScaleAxisSeqvensor::SA_z:
	return TMyMDIChild::dScaleZ;
    }

   return 1;
 }

double TMyMDIChild::GetAngle()
 {
   switch( TMyMDIChild::resCurExis.GetCurrent() )
    {
      case TRotExisSeqvensor::AX_x:
	return TMyMDIChild::anX;

      case TRotExisSeqvensor::AX_y:
	return TMyMDIChild::anY;

      case TRotExisSeqvensor::AX_z:
	return TMyMDIChild::anZ;
    }

   return 0;
 }


void TMyMDIChild::RotateKbUCS( WPARAM wp )
 {
   if( GetVpoint_() == TUCS::VP_Camera || GetVpoint_() == TUCS::VP_None ||
       AskBitmap() == TRUE
     ) return;

   double dAngleH, dAngleV;
   TViewPoint vpView;
   GetVpoint( vpView, dAngleH, dAngleV );
   double deltaY = 0, deltaX = 0;

   const double dGradusStep = MDI_d_GradusStepOnRotate;

   switch( wp )
    {
      case VK_UP:
	deltaX = -dGradusStep;
	break;

      case VK_DOWN:
	deltaX = dGradusStep;
	break;

      case VK_LEFT:
	deltaY = -dGradusStep;
	break;

      case VK_RIGHT:
	deltaY = dGradusStep;
	break;

      default:
	return;
    };

   ChangeVPoint( vpView, dAngleH + TGrid::GradToRad(deltaY), dAngleV + TGrid::GradToRad(deltaX) );
   InvalidateRect( HWindow, NULL, TRUE );
 }


void TMyMDIChild::WMKeyDown( RTMessage msg )//WM_KEYDOWN
 {

   if( pbmpDIB )
    {  
      DWORD dwOrg = GetViewportOrg( hdcOwn );
      int iPoW = LOWORD( dwOrg ),
	  iPoH = HIWORD( dwOrg );
      RECT r; GetClientRect( HWindow, &r );
      int iDx = iBmpWidth - r.right,
	  iDy = iBmpHigh - r.bottom;
      int iVStep = double(r.right) * 0.2,
	  iHStep = double(r.bottom) * 0.2;

      switch( msg.WParam )
       {
         case VK_UP:	   
           iPoH++;
	   break;

         case VK_DOWN:
           iPoH--;
	   break;

         case VK_LEFT:
           iPoW++;
	   break;

	 case VK_RIGHT:
	   iPoW--;
	   break;

	 case VK_HOME:
	   if( GetKeyState(VK_SHIFT) & 0xFF00 )
	     iPoH = 0;
           else
	     iPoW = 0;
	   break;

	 case VK_END:
	   if( GetKeyState(VK_SHIFT) & 0xFF00 )
	     iPoH = -iDy;
           else
	     iPoW = -iDx;
	   break;

	 case VK_PRIOR:
	   if( GetKeyState(VK_SHIFT) & 0xFF00 )
	     iPoW += iHStep;
           else
	     iPoH += iVStep;
	   break;

	 case VK_NEXT:
	   if( GetKeyState(VK_SHIFT) & 0xFF00 )
	     iPoW -= iHStep;
           else
	     iPoH -= iVStep;
	   break;

	 default:	   
	   TBWindow::DefWndProc( msg );	   
           return;
       }


      if( iDx > 0 || iDy > 0 )
       {
         if( iPoW > 0 ) iPoW = 0;
         else if( iPoW < -iDx ) iPoW = -iDx;
         if( iPoH > 0 ) iPoH = 0;
	 else if( iPoH < -iDy ) iPoH = -iDy;

	 if( iPoW != (int)LOWORD( dwOrg ) || iPoH != (int)HIWORD( dwOrg ) )
          {
	    SetViewportOrg( hdcOwn, (iDx> 0) ? iPoW:0,
			            (iDy > 0) ? iPoH:0 );
	    
	    InvalidateRect( HWindow, NULL, TRUE );
	  }
       }

      TBWindow::DefWndProc( msg );
      return;
    }


   switch( msg.WParam )
    {
      case VK_RETURN:
        switch( TMyMDIChild::mcMode )
         {
	   case MC_MoveBody:
	     TMyMDIChild::dDX = TMyMDIChild::dDY = TMyMDIChild::dDZ = 0;
	     TMyMDIChild::masMoveAxis++;
             break;

	   case MC_RotateBody:
	     TMyMDIChild::anX = TMyMDIChild::anY = TMyMDIChild::anZ = 0;
	     TMyMDIChild::resCurExis++;
	     break;

	   case MC_3DScaleBody:
	     TMyMDIChild::dScaleX = TMyMDIChild::dScaleY = TMyMDIChild::dScaleZ = 100;
	     TMyMDIChild::sasScaleAxis++;
	     break;

         };      
        TMessage msgS;
        msgS.Receiver = HWindow;
        msgS.Message = WM_MOUSEMOVE;
        msgS.Result = 1;
        msgS.WParam = 0;
        POINT p;
        GetCursorPos( &p );
        msgS.LParam = MAKELONG( p.x, p.y );

	if( TMyMDIChild::tcmhHandler ) TMyMDIChild::tcmhHandler( msgS, this, hdcOwn );

	break;

      default:
	RotateKbUCS( msg.WParam );
        break;
    };
   
   TBWindow::DefWndProc( msg );
 }

void TMyMDIChild::RenderPhoto()
 {
   RenderView( TRUE );
 }

static void near pascal OutFile( HFILE hf, LPCSTR lp1, LPCSTR lp2 )
 {
   char cBuf[512];

   sprintf( cBuf, "   %-25s:  %s\r\n", lp1, lp2 );
   _lwrite( hf, cBuf, _fstrlen(cBuf) );
 }

void TMyMDIChild::SaveLog( RTimeLog rtlLog, BOOL bFlPhoto )
 {
   OFSTRUCT of;
   PTFileInfo pInfo = pGlobalFManeger->GetInfo( GetScId() );
   char cDrive[MAXDRIVE], cDir[MAXDIR], cName[MAXFILE], cExt[MAXEXT];
   _splitpath( pInfo->LpFileNameFull, cDrive, cDir, cName, cExt );

   char cLogF[ 256 ];
   _makepath( cLogF, cDrive, cDir, cName, ".log" );

   HFILE hf = OpenFile( cLogF, &of, OF_CREATE );

   sprintf( cLogF, "(c)AlexCorp. 1995 3DWorld 1.0a\r\n" );
   _lwrite( hf, cLogF, _fstrlen(cLogF) );

   sprintf( cLogF, "*****  Report for file: %s  *****\r\n\r\n", pInfo->LpFileNameFull );
   _lwrite( hf, cLogF, _fstrlen(cLogF) );

   sprintf( cLogF, "\t\t\tRender %s\r\n\r\n", (bFlPhoto == TRUE) ? "photo":"polygonal" );
   _lwrite( hf, cLogF, _fstrlen(cLogF) );

   sprintf( cLogF, " Tracer:\r\n" );
   _lwrite( hf, cLogF, _fstrlen(cLogF) );

   OutFile( hf, "alhoritm", GetScena()->AlhoNameStr() );
   if( bFlPhoto == TRUE )
    {      
      OutFile( hf, "model", GetScena()->ModelNameStr() );
      OutFile( hf, "antialiasing", GetScena()->AntialiasingStatusStr() );
      OutFile( hf, "shadows", GetScena()->ShadowsStatusStr() );
      OutFile( hf, "strict gamma", GetScena()->GammaStatusStr() );
      OutFile( hf, "shading limit", GetScena()->ShadingNameStr() );
      OutFile( hf, "transparency", (GetScena()->TransparencyTest() == TRUE &&
       GetScena()->Method() == PM_RayTracing)
       ? "Used":"Not used" );
    }
   else
     OutFile( hf, "mark intersect", GetScena()->IntersectStatusStr() );


   sprintf( cLogF, "\r\n Image:\r\n" );
   _lwrite( hf, cLogF, _fstrlen(cLogF) );

   OutFile( hf, "palette", GetScena()->PaletteNameStr() );
   OutFile( hf, "size", GetScena()->SizeNameStr() );
   OutFile( hf, "palette algorithm", GetScena()->PaletteAlhoNameStr() );
   OutFile( hf, "dither 256", GetScena()->DitherStatusStr() );
   OutFile( hf, "noisy", GetScena()->NoisyStatusStr() );

   sprintf( cLogF, "\r\n Backgraund:\r\n" );
   _lwrite( hf, cLogF, _fstrlen(cLogF) );

   OutFile( hf, "type", GetScena()->BkGndNameStr() );

   sprintf( cLogF, "\r\n Scena:\r\n" );
   _lwrite( hf, cLogF, _fstrlen(cLogF) );

   OutFile( hf, "bodies", GetScena()->BodyStr() );
   if( bFlPhoto == TRUE ) OutFile( hf, "lights", GetScena()->LightsStr() );
   OutFile( hf, "vertices", GetScena()->VertexStr() );
   OutFile( hf, "edges", GetScena()->AdgesStr() );
   OutFile( hf, "facets", GetScena()->FacetsStr() );


   sprintf( cLogF, "\r\n Performance:\r\n" );
   _lwrite( hf, cLogF, _fstrlen(cLogF) );

   rtlLog.Save( hf );

   _lclose( hf );
 }

void TMyMDIChild::RenderView( BOOL bFlPhoto )
 {
   //SetMapMode( hdcOwn, MM_TEXT );

   //HBITMAP  hbmRender;

   TimeLog tlLog;

   if( GetLockReenter() == TRUE  || pbmpDIB )
    {
      TAutoStringRc str_( hInstRC, ID_MSG_12 );
      TAutoStringRc ttl_( hInstRC, ID_TITLE_MESSAGE );
      BWCCMessageBox( HWindow, str_(),
	ttl_(), MB_ICONSTOP | MB_OK | MB_APPLMODAL );
      //"Window is locked"
      return;
    }

   LockForReenter( TRUE );

   pScena->SetHardwareEnviron( (PTUCS)this, hdcOwn, (PTWindow)this );
   TAutoStringRc s1_( hInstRC, ID_MSG_69, 50 );
   PTIndicator  pInd = new TIndicator( this, /*"Render model"*/s1_(), pScena->GetBytesSizeWithObol(TRUE, FALSE), pMRC );

   char cBuf[128];
   PTFileInfo pInfo = pGlobalFManeger->GetInfo( GetScId() );
   if( !pInfo )
    {
      TAutoStringRc str_( hInstRC, ID_MSG_7 );
      TAutoStringRc ttl_( hInstRC, ID_TITLE_ERROR );
      BWCCMessageBox( HWindow, str_(),
	ttl_(), MB_ICONSTOP | MB_OK | MB_APPLMODAL );
      //"Internal application error: child cann't get INFO for self file"

      delete pInd;
      return;
    }
   wsprintf( cBuf, "File: " );
   GetWindowText( HWindow, cBuf + _fstrlen(cBuf), 100 );
   pInd->AddStr( cBuf );

   TMDIChFlagHolder  hold( this, &TMyMDIChild::SetCriticalLock, TRUE );

   EnableInSysMenu( FALSE );
   PTColorTable pctTbl = NULL;
   PTDIB pRender = (bFlPhoto == FALSE) ?  pScena->Render( pInd, pctTbl, tlLog ):
					  pScena->RenderPhoto( pInd, pctTbl, tlLog );

   if( bFlLogging == TRUE && pRender ) SaveLog( tlLog, bFlPhoto );

   hold.~TMDIChFlagHolder();

   pInd->Close();
   RedrawVports();

   if( pRender && !pRender->ErStatus )
    {                          //TWindowsObject
      //pbmpDIB = new TDIB( hbmRender, GetApplication() );
      pbmpDIB = pRender;

      RECT r;
      GetClientRect( HWindow, &r );

      SetMapMode( hdcOwn, MM_TEXT );
      SetWindowExt( hdcOwn, r.right, r.bottom );
      SetViewportExt( hdcOwn, r.right, r.bottom );
      SetViewportOrg( hdcOwn, 0, 0 );


      if( pRender )
       {
	 iBmpWidth = pRender->GetWidth();
	 iBmpHigh = pRender->GetHigh();

	 if( pctTbl && !GetDeviceCaps(hdcOwn, SIZEPALETTE) )
	  {
	    delete []pctTbl; pctTbl = NULL;
	  }
	 else if( pctTbl )
	  {    
	    LPLOGPALETTE lpPal = (LPLOGPALETTE)new char[ sizeof(LOGPALETTE) + sizeof(PALETTEENTRY)*256 ];
	    if( !lpPal ) delete []pctTbl;
	    else
             {
               lpPal->palVersion = 0x300;
               lpPal->palNumEntries = 256;

	       for( int i = 0; i < 256; i++ )
                {
		  lpPal->palPalEntry[i].peRed = pctTbl[i].rgbRed;
		  lpPal->palPalEntry[i].peGreen = pctTbl[i].rgbGreen;
		  lpPal->palPalEntry[i].peBlue = pctTbl[i].rgbBlue;
                  lpPal->palPalEntry[i].peFlags = NULL;
                }

	       delete []pctTbl;
               hpalAlias = ::CreatePalette( lpPal );
	       delete []lpPal;
	     }
	  }

	 TMessage msg; _fsetmem( &msg, sizeof(TMessage), 0 );
         msg.Message = WM_QUERYNEWPALETTE;
         WMQueryNewPalette( msg );
       }
      else if( pctTbl )
       {
	 delete []pctTbl; pctTbl = NULL;
       }

      //SendMessage( HWindow, WM_QUERYNEWPALETTE, 0, 0 );

      InvalidateRect( HWindow, NULL, FALSE );
      GenTitle( pGlobalFManeger->GetInfo(GetScId()) );

      SendMessage( GetApplication()->MainWindow->HWindow, WM_COMMAND, GetScId(), MAKELONG(0, CM_FIRST+UCM_SET_CHECKED_IN_MENU) );
    }
   else
    {
      if( pctTbl ) delete []pctTbl; pctTbl = NULL;
      if( pRender )
       {
	 GetApplication()->Error( pRender->ErStatus );
	 delete pRender;
       }

      EnableInSysMenu( TRUE );
    }

   SetupMenuState();
   LockForReenter( FALSE );
 }

/*struct TMDIChRW
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
 };*/


int TMyMDIChild::Save( HFILE hf )
 {

   double shiftX, shiftY;
   GetShift( shiftX, shiftY );

   TViewPoint vp;
   double dH, dV;
   GetVpoint( vp, dH, dV );

   T3DPoint _3d = GetGlobalBase();

   WINDOWPLACEMENT wp;
   wp.length = sizeof(WINDOWPLACEMENT);
   GetWindowPlacement( HWindow, &wp );

   TMDIChHeader ms( bounds, spcX, spcY, GetGridState(), GetScale(),
     shiftX, shiftY, dH, dV, vp, GetVisState(), wp,
     iDivIndex,
     bAutoFit, bFlPopupAllViews,
     _3d, IsVisibleBasePoint()
     );

   ms.InitCRC();

   return (ms.Save( hf ) == HFILE_ERROR) ? -3:1;
 }
                      //***

int TMyMDIChild::Load(HFILE hf )
 {
   TMDIChHeader ms;

   if( ms.Read( hf ) == HFILE_ERROR ) return -3;
   if( ms.CheckCRC() == FALSE ) return -4;  

   bounds = ms.d.bounds;
   spcX = ms.d.spcX;
   spcY = ms.d.spcY;
   GridOnOff( ms.d.bFlVisyalGreed );
   ChangeScale( ms.d.scale );

   ShiftAbs( ms.d.shiftX, ms.d.shiftY );

   ChangeVPoint( ms.d.vpViewPoint, ms.d.h, ms.d.v );

   AxisOnOff( ms.d.bFlVisyalAxis );

   SetWindowPlacement( HWindow, &ms.d.place );

   iDivIndex = ms.d.iDivIndex;

   bAutoFit = ms.d.bAutoFit;
   UINT flCheck = (bAutoFit == TRUE) ? MF_CHECKED:
				       MF_UNCHECKED;
   HMENU hmenuSys;
   if( HWindow )
     if( (hmenuSys = GetSystemMenu(HWindow, FALSE)) )
       CheckMenuItem( hmenuSys, UCM_AUTO_FIT, MF_BYCOMMAND | flCheck );

   bFlPopupAllViews = ms.d.bFlPopupAllViews;

   SetGlobalBase( ms.d.globalBasePoint );
   SetVisibleBasePoint( ms.d.bFlIsBasePointVisible );


   flCheck = (bFlPopupAllViews == TRUE) ? MF_CHECKED:MF_UNCHECKED;

   if( hmenuSys )
     CheckMenuItem( hmenuSys, UCM_SET_POPUPFLAG, MF_BYCOMMAND | flCheck );

   return 1;
 }

void GetPointMouseHandler( RTMessage msg, PTMyMDIChild pCh, HDC hdc )
 {
   if( TMyMDIChild::phazeOfCurMode == 0 ) { DefMouseHandler( msg, pCh, hdc ); return; }
 } 

void GetPointMouseLBHandler( RTMessage msg, PTMyMDIChild pCh )
 {
   pCh->GetScena()->SetHardwareEnviron( (PTUCS)pCh, pCh->hdcOwn, (PTWindow)pCh );

   TMyMDIChild::ptSelected.x = pCh->mouseX;
   TMyMDIChild::ptSelected.y = pCh->mouseY;
   //DPtoLP( pCh->hdcOwn, &TMyMDIChild::ptSelected , 1 );

   TMyMDIChild::bFlShiftActive = FALSE;
   TMyMDIChild::phazeOfCurMode = 0;

   pCh->InitMode( TMyMDIChild::mcNextOp );
 }

void GetPointMouseRBHandler( RTMessage msg, PTMyMDIChild pCh )
 {
   if( TMyMDIChild::mcKeyMode < MC_MoveBody )
     pCh->SetModeChild( TMyMDIChild::mcKeyMode );
   else pCh->SetModeChild( MC_Default );

   TMyMDIChild::mcNextOp = TMyMDIChild::mcKeyNextMode;
 }

static void DrawFacetInstance( Object _FAR &rObj, void _FAR *pSc )
 {
   PTScena(pSc)->DrawFacet( *(RTFacetItem(rObj).pFacet), FALSE );
 }

void SelectFacetMouseHandler( RTMessage msg, PTMyMDIChild pCh, HDC hdc )
 {
   const int iStep = MDI_i_SelectFacetGeometricMouseStep;

   POINT p; p.x = pCh->mouseX; p.y = pCh->mouseY;
   DPtoLP( hdc, &p, 1 );
   int dx = p.x - TMyMDIChild::curXSh,
       dy = p.y - TMyMDIChild::curYSh;

   TMyMDIChild::curXSh = p.x;
   TMyMDIChild::curYSh = p.y;

   TMyMDIChild::dDX += ((abs(dx) > abs(dy)) ? dx:dy);
   if( abs(TMyMDIChild::dDX) > iStep || msg.Message != WM_MOUSEMOVE )
    {
      TMyMDIChild::dDX = 0;

      HPEN hpEra = CreatePen( PS_SOLID, 0, PALETTEINDEX(6) );
      HPEN hpSel = CreatePen( PS_SOLID, 0, PALETTEINDEX(4)/*RGB(0xA6, 0xCA, 0xF0)*/ );

      pCh->GetScena()->SetHardwareEnviron( (PTUCS)pCh, hdc, (PTWindow)pCh );
      HPEN hpOld = SelectPen( hdc, hpEra );
      pCh->GetScena()->DrawFacet( *TMyMDIChild::pfacSelected, TRUE );

      if( !int(TMyMDIChild::dbiFacets) ) TMyMDIChild::dbiFacets.restart();
      TMyMDIChild::pfacSelected = RTFacetItem(TMyMDIChild::dbiFacets++).pFacet;


      SelectPen( hdc, (HPEN)GetStockObject(WHITE_PEN) );
      TMyMDIChild::dblCommonFacets.forEach( DrawFacetInstance, Pvoid(pCh->GetScena()) );

      if( msg.Message == WM_MOUSEMOVE )
       {
         SelectPen( hdc, hpSel );
	 pCh->GetScena()->DrawFacet( *TMyMDIChild::pfacSelected, TRUE );
       }


      SelectObject( hdc, hpOld );
      DeleteObject( hpSel );
      DeleteObject( hpEra );
    }
 }

void SelectFacetMouseLBHandler( RTMessage msg, PTMyMDIChild pCh )
 {
   TMyMDIChild::bFlShiftActive = FALSE;
   TMyMDIChild::phazeOfCurMode = 0;


   PTFacet pfacKey = TMyMDIChild::pfacSelected;   
   SelectFacetMouseHandler( msg, pCh, pCh->hdcOwn );
   TMyMDIChild::pfacSelected = pfacKey;

   pCh->GetScena()->SetHardwareEnviron( (PTUCS)pCh, pCh->hdcOwn, (PTWindow)pCh );
   pCh->GetScena()->Draw( TMyMDIChild::psfSelected );
   

   TMyMDIChild::dblCommonFacets.flush( TShouldDelete::Delete );

   ReleaseCapture();
   ShowCursor( TRUE );

   pCh->InitMode( TMyMDIChild::mcNextOp );
 }

void SelectFacetMouseRBHandler( RTMessage msg, PTMyMDIChild pCh )
 {
   pCh->SetModeChild( TMyMDIChild::mcKeyMode );
   TMyMDIChild::mcNextOp = TMyMDIChild::mcKeyNextMode;
   
   SelectFacetMouseHandler( msg, pCh, pCh->hdcOwn );
   pCh->GetScena()->SetHardwareEnviron( (PTUCS)pCh, pCh->hdcOwn, (PTWindow)pCh );
   pCh->GetScena()->Draw( TMyMDIChild::psfSelected );
   

   TMyMDIChild::dblCommonFacets.flush( TShouldDelete::Delete );

   ReleaseCapture();
   ShowCursor( TRUE );

   pCh->GetFacetAndOp( TMyMDIChild::mcNextOp2 );
 }



/*static void SelectBodyMouseHandler( RTMessage, PTMyMDIChild, HDC )
 {
 }

static void SelectBodyMouseLBHandler( RTMessage, PTMyMDIChild )
 {
 }

static void SelectBodyMouseRBHandler( RTMessage, PTMyMDIChild )
 {
 }*/


void TMyMDIChild::SetCursorInCenter( PTObol pSf )
 {
   if( !pSf ) return;

   POINT p;
   T3DPoint pt = pSf->GetGeomCenterObol();
   RecalcUserToBase( &pt, 1 );
   p.x = pt.x; p.y = pt.y;
   LPtoDP( hdcOwn, &p, 1 );
   mouseX = p.x; mouseY = p.y;
   ClientToScreen( HWindow, &p );
   SetCursorPos( p.x, p.y );
 }

T3DPoint TMyMDIChild::GetBP( PTSurface pSf ) const
 {
   if( TMyMDIChild::UsedGlobalPoint() == TRUE || !GetScena() ) return GetGlobalBase();

   if( TMyMDIChild::IsTransformSelected() == TRUE &&
       GetScena()->SelectedCount(TMyMDIChild::pCurFilter)
     )
     return GetScena()->GetGeomCenterSelected();

   if( pSf ) return pSf->GetGeomCenter();
   else return GetGlobalBase();
 }

struct TSetCritDta {
  int id;
  BOOL bFl;

  TSetCritDta( int id_, BOOL bFl_ )
   {
     id = id_; bFl = bFl_;
   }
 };
typedef TSetCritDta _FAR *PTSetCritDta;

void SetCriticalLockInstance( Pvoid pCh, Pvoid pDta )
 {
   if( PTWindow(pCh)->isA() != classtypeMDIChWindow ||
       PTMyMDIChild(pCh)->GetScId() != PTSetCritDta(pDta)->id
     ) return;

   /*if( IsZoomed(PTMyMDIChild(pCh)->HWindow) == TRUE )
     PTMyMDIChild(pCh)->Show( SW_SHOWNORMAL );
   else*/

   PTMyMDIChild(pCh)->EnableInSysMenu( (PTSetCritDta(pDta)->bFl == FALSE) ? TRUE:FALSE );

   SetWindowPos( PTMyMDIChild(pCh)->HWindow, NULL, 0, 0, 0, 0,
     SWP_DRAWFRAME | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE );
 }

void TMyMDIChild::SetCriticalLock( BOOL bFl )
 {
   BOOL bFlKey = GetCriticalLock();

   pScena->SetCriticalLock( bFl );

   if( bFlKey != GetCriticalLock() )
   //  RedrawVports();
    {
      TSetCritDta scDta( pScena->GetId(), bFl );
      GetApplication()->MainWindow->ForEach( SetCriticalLockInstance, Pvoid(&scDta) );
    }
 }

TMDIChFlagHolder::TMDIChFlagHolder( PTMyMDIChild pObj, PMDIChildFlagFunc pMemFunc, BOOL bHolValue )
 {
   pCh = pObj; pFunc = pMemFunc; bHold = bHolValue;
   (pObj->*pMemFunc)( bHold );
 };

TMDIChFlagHolder::~TMDIChFlagHolder()
 {
   if( pCh ) (pCh->*pFunc)( (bHold == FALSE) ? TRUE:FALSE );
   pCh = NULL;
 }


void TMyMDIChild::ScaleValueBaseToUser( double& dVal )
 {
   dVal /= GetScale();
 }

void TMyMDIChild::ScaleValueBaseToUser( int& iVal )
 {
   iVal = double(iVal) / GetScale();
 }

#pragma option -v.
