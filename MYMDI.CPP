#pragma option -v-

#include "mymdi.hpp"
#include "indicat.hpp"
//#include "ucs.hpp"
#include <filedial.h>
//#include "facedial.hpp"
#include <dir.h>
#include <io.h>


#include "INC\img256.inc"
#include "INC\systbbmp.inc"
#include "INC\str.inc"
#include "INC\bodybmp.inc"
#include "INC\classtyp.inc"
#include "infdlg.hpp"
#include "ambdlg.hpp"
#include "2darray.hpp"
#include "bkgnddlg.hpp"
#include "isdlg.hpp"

#include "phdlg.hpp"
#include "phtransf.hpp"
#include "medium.hpp"
#include "fheader.hpp"
#include "io_msg.hpp"
#include "autorc.hpp"
#include "prdlg.hpp"

#include <shellapi.h>

extern HINSTANCE hInstRC;
PTFileManeger pGlobalFManeger = NULL;
extern PTModule pMRC;


BOOL bFlLogging = FALSE;

int MenuPosIncr( HWND HWindow )
 {
   return (!HWindow || IsZoomed(HWindow) == FALSE) ? 0:1;
 }

//TStrategyTable[SYSTOOL_NUMB_BTN][3];
TMyMDIFrame::TStrategyTable TMyMDIFrame::iStrateg =
 {
      { UCM_TB_SETUP_UCS,         TS_Default,    MC_SetupUCS },
      { UCM_TB_SHIFT000,          TS_Selectable, MC_ShiftUCS },
      { UCM_TB_SCALE_VIEW_DOWN,   TS_Default,    -1 },
      { UCM_TB_SCALE_VIEW_UP,     TS_Default,    -1 },
      { UCM_TB_FIT_TO_ZOOM,       TS_Default,    -1 },
      { UCM_TB_FIT_TO_WINDOW,     TS_Selectable, MC_GetWindow },
      { UCM_TB_HOLD,              TS_Default,    -1 },
      { UCM_TB_FETCH,             TS_Default,    -1 },
      { UCM_TB_CREATE_SIMPLEOBJ,  TS_Selectable, -2 },
      { UCM_TB_MOVEBODY,          TS_Selectable, MC_MoveBody },
      { UCM_TB_ROTATEBODY,        TS_Selectable, MC_RotateBody },
      { UCM_TB_2DSCALEBODY,       TS_Selectable, MC_2DScaleBody },
      { UCM_TB_3DSCALEBODY,       TS_Selectable, MC_3DScaleBody },
      { UCM_TB_DELETEBODY,        TS_Selectable, MC_DeleteBody },
      { UCM_TB_SELECT,            TS_Selectable, MC_Select },
      { UCM_TB_SELECTWINDOW,      TS_Selectable, -2 },
      { UCM_TB_INVERTSELMODE,     TS_IndependSwt,-1 },
      { UCM_TB_COPY,              TS_Default,    -1 },
      { UCM_TB_CUT,               TS_Default,    -1 },
      { UCM_TB_PASTE,             TS_Default,    -1 },
      { UCM_TB_CONECT,            TS_IndependSwt,-1 },
      { UCM_TB_GROUP,             TS_IndependSwt,-1 },
      { UCM_TB_USE_BASEUCS,       TS_IndependSwt,-1 },
 };

//TTypeModesTable[NUMB_MODES][3];
TMyMDIFrame::TTypeModesTable TMyMDIFrame::iTypeModes =
 {
   { MC_Default,                    -1, -1 },
   { MC_SetupUCS,    TS_Default,    UCM_TB_SETUP_UCS },
   { MC_ShiftUCS,    TS_Selectable, UCM_TB_SHIFT000 },

   { MC_CreCylinder, TS_Selectable, UCM_TB_CREATE_SIMPLEOBJ },
   { MC_CreCone,     TS_Selectable, UCM_TB_CREATE_SIMPLEOBJ },
   { MC_CreSphere,   TS_Selectable, UCM_TB_CREATE_SIMPLEOBJ },
   { MC_CreTorus,    TS_Selectable, UCM_TB_CREATE_SIMPLEOBJ },
   { MC_CreTube,     TS_Selectable, UCM_TB_CREATE_SIMPLEOBJ },

   { MC_GetWindow,   TS_Selectable, -3 },
   { MC_Select,      TS_Selectable, -2 },

   { MC_MoveBody,    TS_Selectable, UCM_TB_MOVEBODY },
   { MC_DeleteBody,  TS_Selectable, UCM_TB_DELETEBODY },
   { MC_3DScaleBody, TS_Selectable, UCM_TB_3DSCALEBODY },
   { MC_2DScaleBody, TS_Selectable, UCM_TB_2DSCALEBODY },
   { MC_RotateBody,  TS_Selectable, UCM_TB_ROTATEBODY },
   { MC_SelectBody,  TS_Selectable, -2 },

   { MC_GetPoint,    TS_Selectable, -2 },
   { MC_SetupGlobalBP, -1, -1 },

   { MC_AligmentVertex, -1, -1 },
   { MC_AligmentFacet, -1, -1 },
   { MC_SelectFacet, -1, -1 },

   { MC_CreateOmni, -1, -1 },
   { MC_OmniAdjust, -1, -1 },

   { MC_VPortAligment, -1, -1 },
   { MC_PlaceHilite, -1, -1 },
   { MC_Smoth, -1, -1 }
 };


int* TMyMDIFrame::FindTypeMode( ModeChild mcM )
 {
   for( int i = 0; i < NUMB_MODES; i++ )
     if( TMyMDIFrame::iTypeModes[i][0] == mcM ) return TMyMDIFrame::iTypeModes[i];

   return NULL;
 }

int* TMyMDIFrame::FindTypeMode2( int idTbBtn )
 {
   for( int i = 0; i < SYSTOOL_NUMB_BTN; i++ )
     if( TMyMDIFrame::iStrateg[i][0] == idTbBtn ) return TMyMDIFrame::iStrateg[i];

   return NULL;
 }

int* TMyMDIFrame::FindBtn( ModeChild idMode )
 {
   for( int i = 0; i < SYSTOOL_NUMB_BTN; i++ )
     if( TMyMDIFrame::iStrateg[i][2] == idMode ) return TMyMDIFrame::iStrateg[i];

   return NULL;
 }



void TMyMDIFrame::CMMDINext( RTMessage msg )//WM_MDINEXT
 {                
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( !pMDICh || PTWindow(pMDICh)->isA() != classtypeMDIChWindow ) return;
   PTMDIClient pC = GetClient();

   if( !pC ) return;

   msg.Result = SendMessage( pC->HWindow, WM_MDINEXT, (WPARAM)(pMDICh->HWindow), (LPARAM)TRUE );
 }

void TMyMDIFrame::CMMDIPrev( RTMessage msg )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( !pMDICh || PTWindow(pMDICh)->isA() != classtypeMDIChWindow ) return;
   PTMDIClient pC = GetClient();

   if( !pC ) return;

   msg.Result = SendMessage( pC->HWindow, WM_MDINEXT, (WPARAM)(pMDICh->HWindow), (LPARAM)FALSE );
 }

void TMyMDIFrame::CMZoom( RTMessage msg )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( !pMDICh || PTWindow(pMDICh)->isA() != classtypeMDIChWindow )
    {
      if( IsIconic(HWindow) == TRUE ) Show( SW_SHOWNORMAL );
      else if( IsZoomed(HWindow) == FALSE ) Show( SW_SHOWMAXIMIZED );        
    }
   else if( GetClient() )
    {
      if( IsIconic(pMDICh->HWindow) == TRUE )
	msg.Result = SendMessage( GetClient()->HWindow, WM_MDIRESTORE, (WPARAM)(pMDICh->HWindow), 0L );
      else if( IsZoomed(pMDICh->HWindow) == FALSE ) 
	msg.Result = SendMessage( GetClient()->HWindow, WM_MDIMAXIMIZE, (WPARAM)(pMDICh->HWindow), 0L );
      else Show( SW_SHOWMAXIMIZED );
    }
 }

void TMyMDIFrame::CMUnZoom( RTMessage msg )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( !pMDICh || PTWindow(pMDICh)->isA() != classtypeMDIChWindow )
    {
      if( IsZoomed(HWindow) == TRUE ) Show( SW_SHOWNORMAL );
      else if( IsIconic(HWindow) == FALSE ) Show( SW_SHOWMINIMIZED );        
    }
   else if( GetClient() )
    {
      if( IsZoomed(pMDICh->HWindow) == TRUE )
	msg.Result = SendMessage( GetClient()->HWindow, WM_MDIRESTORE, (WPARAM)(pMDICh->HWindow), 0L );
      else if( IsIconic(pMDICh->HWindow) == FALSE )
	pMDICh->Show( SW_SHOWMINIMIZED );
      else Show( SW_SHOWMINIMIZED );
    }

 }

static BOOL FindChInstance( Pvoid pCh, Pvoid hwnd )
 {
   if( PTWindow(pCh)->isA() != classtypeMDIChWindow ||
       PTWindow(pCh)->HWindow != (HWND)hwnd
     ) return FALSE;

   return TRUE;
 }


void TMyMDIClient::WMMDIRestore( RTMessage msg )//WM_MDIRESTORE
 {
   PTMyMDIChild pCh = (PTMyMDIChild)Parent->FirstThat( FindChInstance, Pvoid(msg.WParam) );

   if( pCh && (pCh->AskBitmap() == TRUE || pCh->GetCriticalLock() == TRUE) )
    {
      msg.Result = 0;
      return;
    }

   DefWndProc( msg );
 }

void TMyMDIClient::WMMDIMaximize( RTMessage msg ) //WM_MDIMAXIMIZE
 {
   PTMyMDIChild pCh = (PTMyMDIChild)Parent->FirstThat( FindChInstance, Pvoid(msg.WParam) );

   if( pCh && (pCh->AskBitmap() == TRUE || pCh->GetCriticalLock() == TRUE) )
    {
      msg.Result = 0;
      return;
    }


   DefWndProc( msg );
 }

BOOL TMyMDIClient::Create()
 {
   //Attr.Style &= ~(WS_HSCROLL | WS_VSCROLL);
   return TMDIClient::Create();
 }

/*static BOOL GetChildPtrForHWNDInstance( Pvoid pCh, Pvoid hwnd )
 {
   if( PTWindow(pCh)->isA() != classtypeMDIChWindow ) return FALSE;

   return BOOL(PTWindow(pCh)->HWindow == (HWND)hwnd);
 }

PTMyMDIChild TMyMDIClient::GetChildPtrForHWND( HWND hwnd ) const
 {
   if( Parent->isA() != classtypeMDIFrame ) return NULL;

   return (PTMyMDIChild)Parent->FirstThat( GetChildPtrForHWNDInstance, Pvoid(hwnd) );
 }


void TMyMDIClient::WMMDIDestroy( RTMessage msg )//WM_MDIDESTROY
 {
   PTMyMDIChild pCloseCh = NULL;
   if( Parent && Parent->isA() == classtypeMDIFrame )
     pCloseCh = GetChildPtrForHWND( (HWND)msg.WParam );

   int id = 0, numb = 0;
   if( pCloseCh )
    {
      id = pCloseCh->GetScId();
      numb = pCloseCh->CalcSelf();
    } 

   TMDIClient::DefWndProc( msg );


   //if( id && numb == 1 )  PTMyMDIFrame(Parent)->CloseScene( id );
   if( id && numb == 1 )
    {
      //PTMDIFrame(Parent)->ActiveChild_ = NULL;
      PostMessage( Parent->HWindow, WM_COMMAND, id,
	MAKELONG(0, CM_FIRST+UCM_CLOSE_SCENE) );
    }
 }*/


void TMyMDIFrame::InternalChildActivate()
 { 
   //if( CondVisyalMDI(ActiveChild_) == FALSE ) return;
   MakeChildTop( *PTMyMDIChild(ActiveChild_) );
 }


classType TMyMDIFrame::isA() const
 {
   return classtypeMDIFrame;
 }

BOOL TMyMDIFrame::AssignMenu( LPSTR MenuName )
 {
   BOOL ReturnVal = TRUE;
   HMENU NewHMenu;
   HMENU OldHMenu;
   LPSTR TempMenuName = NULL;    // handle case where MenuName==Attr.Menu

   if ( HIWORD(Attr.Menu) )
      TempMenuName = Attr.Menu;

   if ( HIWORD(MenuName) )   // Not NULL and not an int in disguise
     Attr.Menu = _fstrdup(MenuName);
   else
     Attr.Menu = MenuName;

   if (TempMenuName)
       farfree(TempMenuName);

   if ( HWindow )
    {
      OldHMenu = GetMenu(HWindow);
      NewHMenu = LoadMenu( hInstRC, Attr.Menu );
      ReturnVal = SetMenu(HWindow, NewHMenu);
      if ( OldHMenu )
        DestroyMenu(OldHMenu);
    }
   return ReturnVal;
 }

void TMyMDIFrame::SetupWindow()//TMDIFrame WM_SIZE
 {
   HMENU HmenuSys;

   AssignMenu( "MainMenu" );

   TMDIFrame::SetupWindow();
   pStatusWnd  = new  TStatusWindow( this, GetModule() );  

   if( IsIconic(HWindow) == FALSE && IsZoomed(HWindow) == FALSE ) DefaultMove( FALSE );



   if( (HmenuSys = GetSystemMenu(HWindow, FALSE)) )
    {
      TAutoStringRc s1_( hInstRC, ID_MSG_50 );

      AppendMenu( HmenuSys, MF_SEPARATOR, 0, 0 );
      AppendMenu( HmenuSys, MF_BYCOMMAND | MF_ENABLED, UCM_SYSABOUT,
	s1_() );
      //"&About 3D World...\tCtr+Alt+A"
      s1_ = TAutoStringRc( hInstRC, ID_MSG_51 );
      AppendMenu( HmenuSys, MF_BYCOMMAND | MF_ENABLED, UCM_DEFAULTPOS_MAINWINDOW,
	s1_() );
      //"&Default position window"
    }

   SetCheckedInMenu( TRUE );

   DragAcceptFiles( HWindow, TRUE );
   //CreateChild();
 }

void TMyMDIFrame::Destroy()
 {
   DragAcceptFiles( HWindow, FALSE );
   TMDIFrame::Destroy();
 }


void TMyMDIFrame::WMEnable( RTMessage msg )//WM_ENABLE
 {
   TMDIFrame::DefWndProc( msg );
   if( pSysTool )
     EnableWindow( pSysTool->HWindow, msg.WParam );
 }


struct TQueryNewPaletteData
 {
   PTMyMDIChild  pActive;
   TMessage      FAR *pMsg;
   PTMyMDIFrame  pMWin;
 };
typedef  TQueryNewPaletteData FAR *PTQueryNewPaletteData;

void QueryNewPaletteInstance( Pvoid pChWin, Pvoid pMsgDta )
 {
   PTMyMDIChild pCh = PTMyMDIChild(pChWin);
   PTQueryNewPaletteData  pDta = (PTQueryNewPaletteData)pMsgDta;

   if( PTWindow(pCh)->isA() != classtypeMDIChWindow ||
       pDta->pMWin->CondVisyalMDI(pCh) == FALSE ||
       (void huge *)(pDta->pActive) == (void huge *)pCh
     ) return;

   pCh->WMQueryNewPalette( *(pDta->pMsg) );
 }

void TMyMDIFrame::WMQueryNewPalette( RTMessage Msg )
 {   
   TMDIFrame::DefWndProc( Msg );
   return;

   /*TQueryNewPaletteData  qnpdData;
   qnpdData.pActive = (PTMyMDIChild)ActiveChild_;
   qnpdData.pMsg = &Msg;
   qnpdData.pMWin = this;
   ForEach( QueryNewPaletteInstance, &qnpdData );
      
   if( CondVisyalMDI((PTMyMDIChild)ActiveChild_) == FALSE )
    {
      Msg.Result = 0;
      return;
    }

   PTMyMDIChild(ActiveChild_)->WMQueryNewPalette( Msg );*/
 }

typedef  TQueryNewPaletteData TPaletteChangedData;
typedef  PTQueryNewPaletteData PTPaletteChangedData;

void PaletteChangedInstance( Pvoid pChWin, Pvoid pMsgDta )
 {
   PTMyMDIChild pCh = PTMyMDIChild(pChWin);
   PTPaletteChangedData  pDta = (PTPaletteChangedData)pMsgDta;

   if( PTWindow(pCh)->isA() != classtypeMDIChWindow ||
       pDta->pMWin->CondVisyalMDI(pCh) == FALSE //||
       //oid huge *)(pDta->pActive) == (void huge *)pCh
     ) return;

   pCh->WMPaletteChanged( *(pDta->pMsg) );
 }


void TMyMDIFrame::WMPaletteChanged( RTMessage Msg )
 {
   //TMDIFrame::DefWndProc( Msg );
   //return;
   
   TPaletteChangedData  qnpdData;
   qnpdData.pActive = (PTMyMDIChild)ActiveChild_;
   qnpdData.pMsg = &Msg;
   qnpdData.pMWin = this;

   ForEach( PaletteChangedInstance, &qnpdData );

   Msg.Result = 0;
   //if( CondVisyalMDI(qnpdData.pActive) == FALSE ) return;

   //qnpdData.pActive->WMPaletteChanged( Msg );
 }

void TMyMDIFrame::WMNcCalcSize( RTMessage msg )//WM_NCCALCSIZE
 {
   TMDIFrame::DefWndProc( msg );

   NCCALCSIZE_PARAMS *pNcParams = (NCCALCSIZE_PARAMS *)msg.LParam;
   if( pStatusWnd )
     pNcParams->rgrc[0].bottom -= (pStatusWnd->GetHigh() - 1);
 }

int TMyMDIFrame::GetStatH()
 {
   if( pStatusWnd ) return pStatusWnd->GetHigh();
   else return 0;
 }

void TMyMDIClient::SetupWindow()//TWindow
 {
   RECT cRect;
   PTMyMDIFrame pParent = (PTMyMDIFrame)Parent;

   GetClientRect( Parent->HWindow, &cRect );
   rtStatusLine = TRECT( 0, cRect.bottom-pParent->GetStatH(), cRect.right-1, cRect.bottom - 1 );

   TMDIClient::SetupWindow();
 }

//TRECT( int left, int top, int right, int bottom )
/*void TWindow::WMSize(TMessage& Msg)
{
  RECT WndRect;

  if ( Scroller && (Msg.WParam != SIZEICONIC) )
  {
    Scroller->SetPageSize();
	Scroller->SetSBarRange();
  }
  if ( Msg.WParam == SIZENORMAL )
  {
    GetWindowRect(HWindow, &WndRect);
    Attr.H = WndRect.bottom - WndRect.top;
    Attr.W = WndRect.right - WndRect.left;
  }
  DefWndProc(Msg);
}
*/
void TMyMDIClient::WMSize( RTMessage msg )//WM_SIZE
 {
  /*
  fwSizeType = wParam;   
nWidth = LOWORD(lParam); 
nHeight = HIWORD(lParam);     //SB_PAGEUP
  */
  PTMyMDIFrame pParent = (PTMyMDIFrame)Parent;


   if( (msg.WParam & SIZE_MINIMIZED) != SIZE_MINIMIZED )
    {
      UINT high = HIWORD(msg.LParam) - pParent->GetStatH();
      UINT lo = LOWORD(msg.LParam);
      //msg.LParam = MAKELONG( lo, high );
      rtStatusLine = TRECT( 0, high-pParent->GetStatH(), lo-1, high - 1 );
    }  

   TMDIClient::WMSize( msg );
 }

void TMyMDIFrame::InitClientWindow()
 {
   ClientWnd = new TMyMDIClient( this, GetModule() );
 }


BOOL TMyMDIFrame::IsUserCommand( int cm ) const
 {
   const int iNumbCom = 104;
   static char bFlInit = 0;
   static T2DArray<int> d2aCom( 1, iNumbCom );
   if( !bFlInit )
    {
      bFlInit = 1;

      d2aCom,
	UCM_ABOUT, TLineDef(0),
	UCM_SYSABOUT, TLineDef(0),
	UCM_EXIT,     TLineDef(0),
	UCM_CANCEL,   TLineDef(0),
	UCM_OPEN,     TLineDef(0),
	UCM_NEW_FILE, TLineDef(0),
	UCM_SAVE,     TLineDef(0),
	UCM_SAVE_AS,  TLineDef(0),
	UCM_CLOSE,    TLineDef(0),
	UCM_CLOSE_ALL,TLineDef(0),
	UCM_RETRIVE_SOURCE, TLineDef(0),
	UCM_CLOSE_SCENE,    TLineDef(0),
	UCM_GETINFO, TLineDef(0);

      d2aCom,
	UCM_SELECT_SCRVPORTS, TLineDef(0),
	UCM_GRID_EXTENTS,   TLineDef(0),
	UCM_USE_SNAP,       TLineDef(0),
	UCM_USE_GRID,       TLineDef(0),
	UCM_USERUCS_SETUP,  TLineDef(0),
	UCM_AXIS_ON_OFF,    TLineDef(0),
	UCM_INTERNAL_ACTIVATE_CHILD,   TLineDef(0),
	UCM_INTERNAL_FOCUS_CHILD,      TLineDef(0),
	UCM_SYSTOOLBAR_DESTROYED,      TLineDef(0),
	UCM_CREATE_SIMPLEOBJ,          TLineDef(0),
	UCM_HIDEGEOMETRY,              TLineDef(0),
	UCM_FULLDETALL,                TLineDef(0),
	UCM_SCENEINFO, TLineDef(0);

      d2aCom,
	UCM_MOVEBODY,     TLineDef(0),
	UCM_ROTATEBODY,   TLineDef(0),
	UCM_2DSCALEBODY,  TLineDef(0),
	UCM_3DSCALEBODY,  TLineDef(0),
	UCM_DELETEBODY,   TLineDef(0),
	UCM_COPYBODY,     TLineDef(0),
	UCM_PASTEBODY,    TLineDef(0),
	UCM_MDINEXT,      TLineDef(0),
	UCM_MDIPREV,      TLineDef(0),
	UCM_ZOOM,         TLineDef(0),
	UCM_UNZOOM, TLineDef(0);

      d2aCom,
	UCM_CHANGED_CHILD_MODE, TLineDef(0),
	UCM_TOGGLE_VPORTTOOL,   TLineDef(0),
	UCM_TOGGLE_STATUSBAR,   TLineDef(0),
	UCM_RENDER,              TLineDef(0),
	UCM_SET_CHECKED_IN_MENU, TLineDef(0),
	UCM_SELECT,              TLineDef(0),
	UCM_SELECTWINDOW,        TLineDef(0),
	UCM_SELECTALL,           TLineDef(0),
	UCM_INVERTSELMODE,       TLineDef(0),
	UCM_GET_INVERTSELMODE_FLAG, TLineDef(0),
	UCM_GET_MAINPATH,          TLineDef(0),
	UCM_CUTBODY,               TLineDef(0),
	SYSTOOL_FIRST_COMMAND, TLineDef(0);

      d2aCom,
	UCM_TB_SETUP_UCS,          TLineDef(0),
	UCM_TB_SHIFT000,           TLineDef(0),
	UCM_TB_SCALE_VIEW_DOWN,    TLineDef(0),
	UCM_TB_SCALE_VIEW_UP,      TLineDef(0),
	UCM_TB_FIT_TO_ZOOM,        TLineDef(0),
	UCM_TB_FIT_TO_WINDOW,      TLineDef(0),
	UCM_TB_HOLD,               TLineDef(0),
	UCM_TB_FETCH,              TLineDef(0),
	UCM_TB_CREATE_SIMPLEOBJ,   TLineDef(0),
	UCM_TB_MOVEBODY,           TLineDef(0),
	UCM_TB_ROTATEBODY,         TLineDef(0),
	UCM_TB_2DSCALEBODY,        TLineDef(0),
	UCM_TB_3DSCALEBODY, TLineDef(0);

      d2aCom,
	UCM_TB_DELETEBODY,         TLineDef(0),
	UCM_TB_SELECT,             TLineDef(0),
	UCM_TB_SELECTWINDOW,       TLineDef(0),
	UCM_TB_INVERTSELMODE,      TLineDef(0),
	UCM_TB_COPY,               TLineDef(0),
	UCM_TB_CUT,                TLineDef(0),
	UCM_TB_PASTE,              TLineDef(0),
	UCM_TB_CONECT,             TLineDef(0),
	UCM_TB_GROUP,              TLineDef(0),
	UCM_TB_USE_BASEUCS,        TLineDef(0),
	SYSTOOL_END_COMMAND, TLineDef(0);

      d2aCom,
	UCM_F_ZOOM100,             TLineDef(0),
	UCM_F_ZOOM200,             TLineDef(0),
	UCM_F_ZOOM300,             TLineDef(0),
	UCM_F_ZOOM400,             TLineDef(0),
	UCM_F_ZOOM500,             TLineDef(0),
	UCM_F_ZOOM75,              TLineDef(0),
	UCM_F_ZOOM50,              TLineDef(0),
	UCM_F_ZOOM25,              TLineDef(0),
	UCM_F_ZOOM10,              TLineDef(0),
	UCM_F_ZOOM5,               TLineDef(0),
	UCM_F_FIT_TO_ZOOM,         TLineDef(0),
	UCM_INVERTSELECTING,       TLineDef(0),
	UCM_SET_BP_VISIBLE,        TLineDef(0),
	UCM_USE_GLOBAL_BP,         TLineDef(0),
	UCM_SETUP_GLOBAL_BP,       TLineDef(0),
	UCM_USE_CROSSING_FRAME,    TLineDef(0),
	UCM_ASSIGN_MATERIAL,       TLineDef(0),
	UCM_SAVE_MEDIUM,           TLineDef(0),
	UCM_LOAD_MEDIUM, TLineDef(0);

      d2aCom,
	UCM_VPORT_ALIGN,           TLineDef(0),
	UCM_BKGND_SETUP,           TLineDef(0),
	UCM_IMAGE_OPTIONS,         TLineDef(0),
	UCM_RENDER_FULL,           TLineDef(0),
	UCM_CAMERA_DOLLY,          TLineDef(0),
	UCM_CAMERA_AOV,            TLineDef(0),
	UCM_CAMERA_ROLL,           TLineDef(0),
	UCM_CAMERA_MOVE,           TLineDef(0),
	UCM_CAMERA_ADJUST,         TLineDef(0),
	UCM_CAMERA_CREATE,         TLineDef(0),
	UCM_PLACEHILITE, TLineDef(0);
    }


   for( int i = 0; i < iNumbCom; i++ )
     if( ((int*)d2aCom())[i] == cm ) return TRUE;

   return FALSE;
 }

void TMyMDIFrame::DefWndProc( RTMessage Msg )//WM_COMMAND
 {

   if( Msg.Message == WM_COMMAND )
    {
      PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
                      
      if( !ActiveChild_ || ActiveChild_->isA() != classtypeMDIChWindow )
       {
	 if( IsUserCommand(Msg.WParam) == FALSE )
	  {
            TMDIFrame::DefWndProc( Msg );
            return;
          }

	 int *pTypeInfo = TMyMDIFrame::FindTypeMode2( Msg.WParam );
	 if( pTypeInfo && pTypeInfo[1] != TS_IndependSwt ) return;
	 else
	  {
	    SetCheckedInMenu();
	    return;
          }
       }

       
      if( pMDICh && IsZoomed(pMDICh->HWindow) == TRUE &&
	  pMDICh->IsMyCommand(Msg.WParam) == TRUE
	)
       {                 
         TMessage msg;
         _fmemcpy( &msg, &Msg, sizeof(TMessage) );
	 msg.Message = WM_SYSCOMMAND;
	 pMDICh->DefWndProc( msg );
	 Msg.Result = 0;
         return;
       }

      int cm = Msg.WParam;
      if( cm >= UCM_F_ZOOM100 &&
	  cm <= UCM_F_ZOOM5 &&
	  CondVisyalMDI(pMDICh) == TRUE
	)
       {
	 static int itblScale[] = {100, 200, 300, 400, 500, 75, 50, 25, 10, 5};

	 Msg.Result = 0;

         if( pMDICh->ValidWindowOper(MC_SetupUCS) == FALSE ) return; 

	 pMDICh->ChangeScale( double(itblScale[cm - UCM_F_ZOOM100]) / 100.0 );
         InvalidateRect( pMDICh->HWindow, NULL, TRUE );
	 pMDICh->GenTitle( fmManeger->GetInfo(pMDICh->GetScId()) );
         PostMessage( HWindow, WM_COMMAND, (WPARAM)TMyMDIChild::GetMode_(),
	   MAKELONG(HWindow, UCM_CHANGED_CHILD_MODE) );

	 return;
       }


      switch( HIWORD(Msg.LParam)  )
       {
         /*case UCM_INTERNAL_ACTIVATE_CHILD:
	   SetCheckedInMenu();
	   Msg.Result = 0;
	   return;*/

	 /*case UCM_INTERNAL_FOCUS_CHILD:
	   InternalChildActivate();
	   Msg.Result = 0;
	   return;*/

	 case UCM_CHANGED_CHILD_MODE:
	   CheckToolbar( Msg.WParam );
	   Msg.Result = 0;
	   return;

	 case UCM_TOGGLE_VPORTTOOL:
	   CMToggleVTool( Msg );
	   Msg.Result = 0;
	   return;

	 case UCM_GET_INVERTSELMODE_FLAG:
	   Msg.Result = IsInvertSelectMode();
	   return;

	 case UCM_GET_MAINPATH:
	   Msg.Result = (long)MainPath();
	   return;

	 case UCM_SYSTOOLBAR_DESTROYED:
	   pSysTool = NULL;
	   Msg.Result = 0;
	   return;
       };

      switch( Msg.WParam  )
       {
         case UCM_TB_SETUP_UCS:////////
	   CMUserUcsSetup( Msg );
	   Msg.Result = 0;
	   return;

	 case UCM_TB_SHIFT000:
	   CMUserUcsShift( Msg );
	   Msg.Result = 0;
	   return;

	 case UCM_TB_SCALE_VIEW_DOWN:
           Msg.Result = -1;
	   CMUserUcsScale( Msg );
	   Msg.Result = 0;
	   return;

	 case UCM_TB_SCALE_VIEW_UP:
	   Msg.Result = 1;
	   CMUserUcsScale( Msg );
	   Msg.Result = 0;
	   return;

	 case UCM_TB_FIT_TO_ZOOM:	   
	   CMFitToZoom( Msg );
	   Msg.Result = 0;
	   return;

	 case UCM_TB_FIT_TO_WINDOW:
	   CMFitToWindow( Msg );
	   Msg.Result = 0;
	   return;

	 case UCM_TB_CREATE_SIMPLEOBJ:
	   CMCreateSimpleObj( Msg );
	   Msg.Result = 0;
	   return;

	 case UCM_TB_MOVEBODY:
	   CMMoveBody( Msg );
	   Msg.Result = 0;
	   return;

	 case  UCM_TB_ROTATEBODY:
	   CMRotateBody( Msg );
	   Msg.Result = 0;
	   return;

	 case UCM_TB_2DSCALEBODY:
	   CM2DScaleBody( Msg );
	   Msg.Result = 0;
	   return;

	 case UCM_TB_3DSCALEBODY:
	   CM3DScaleBody( Msg );
	   Msg.Result = 0;
	   return;

	 case UCM_TB_DELETEBODY:
	   CMDeleteBody( Msg );
	   Msg.Result = 0;
	   return;

	 case UCM_TB_HOLD:
	   CMHold( Msg );
	   Msg.Result = 0;
	   return;

	 case UCM_TB_FETCH:
	   CMFetch( Msg );
	   Msg.Result = 0;
	   return;

	 case UCM_TB_SELECT:
           CMSelect( Msg );
	   return;

	 case UCM_TB_SELECTWINDOW:
	   CMSelectWindow( Msg );
	   return;

	 case UCM_TB_INVERTSELMODE:
	   CMInvertSelMode( Msg );
	   return;

	 case UCM_TB_COPY:
           CMCopy( Msg );
	   return;

	 case UCM_TB_CUT:
           CMCut( Msg );
	   return;

	 case UCM_TB_PASTE:
           CMPaste( Msg );
	   return;

	 case UCM_TB_CONECT:
	   CMConnect( Msg );
           return;

	 case UCM_TB_GROUP:
	   CMGroup( Msg );
	   return;

	 case UCM_TB_USE_BASEUCS:
	   CMUseBaseUCS( Msg );
	   return;

       }
    }

   if( Msg.Message == WM_SYSCOMMAND )
     switch( Msg.WParam & 0xFFF0 )
      {
	case UCM_SYSABOUT:
	  SendMessage( HWindow, WM_COMMAND, UCM_ABOUT, 0 );
	  Msg.Result = 0;
	  return;

	case UCM_DEFAULTPOS_MAINWINDOW:
	  DefaultMove( TRUE );
	  Msg.Result = 0;
	  return;

	case UCM_RESTORE_CHILD_DIV:
	  TMessage msg;
          _fsetmem( &msg, sizeof(TMessage), 0 );
	  msg.LParam = UCM_RESTORE_CHILD_DIV;
	  CMSelectScrVports( msg );
	  Msg.Result = 0;
	  return;

	case UCM_ADAPTIVE_DIVISION://TMDIFrame
	  if( ActiveChild_->isA() != classtypeMDIChWindow ) break;

	  ToggleAdaptDiv( fmManeger->ToggleFlAdaptDiv(
	     ((PTMyMDIChild)ActiveChild_)->GetScId() )
			);
	  Msg.Result = 0;
	  break;
      }
                 
   TMDIFrame::DefWndProc( Msg );
 }

void ToggleInstance( Pvoid Child, Pvoid pInf )
 {
   PTMyMDIChild  pChild = (PTMyMDIChild)Child;
   PTFileInfo  pInfo = (PTFileInfo)pInf;

   if( PTWindow(pChild)->isA() != classtypeMDIChWindow ) return;
   if( pChild->GetScId() != pInfo->IdFile ) return;

   HMENU hm = GetSystemMenu( pChild->HWindow, FALSE );


   CheckMenuItem( hm, UCM_ADAPTIVE_DIVISION, MF_BYCOMMAND |
     ((pInfo->bFlWndGrowAsParent == TRUE) ? MF_CHECKED:MF_UNCHECKED)
		);
 }

void TMyMDIFrame::ToggleAdaptDiv( PTFileInfo pInfo )
 {
   if( !pInfo ) return;
   ForEach( ToggleInstance, (Pvoid)pInfo );
 }

void TMyMDIFrame::DefaultMove( BOOL FlMove )//TMDIFrame IsIconic TWindow
 {
   const int dSysToolX = 160, dSysToolY = 120;
   const double PerWidth = 0.97, PerHigh = 0.75;
   int ScrH = GetSystemMetrics(SM_CYSCREEN),
       ScrW = GetSystemMetrics(SM_CXSCREEN);

   if( IsIconic(HWindow) == TRUE || IsZoomed(HWindow) == TRUE )
     ShowWindow( HWindow, SW_SHOWNORMAL );

   Attr.W = (double)ScrW * PerWidth;
   Attr.H = (double)ScrH * PerHigh;
   Attr.X = (ScrW - Attr.W) / 2;
   Attr.Y = (ScrH - Attr.H) / 2;

   MoveWindow( HWindow, Attr.X, Attr.Y, Attr.W, Attr.H, FlMove );

   if( pSysTool )
    {
      if( pSysTool->HWindow )
        SetWindowPos( pSysTool->HWindow, HWND_TOP,
          Attr.X + Attr.W - dSysToolX,
          Attr.Y + Attr.H - dSysToolY,
          pSysTool->Attr.W,
	  pSysTool->Attr.H,  SWP_DRAWFRAME );
       else
	{
	  pSysTool->Attr.X = Attr.X + Attr.W - dSysToolX;
	  pSysTool->Attr.Y = Attr.Y + Attr.H - dSysToolY;
        }
    }
 }

TStrategy TMyMDIFrame::GetCommandStrategy( int command )
 {

   for( int i = 0; i < SYSTOOL_NUMB_BTN; i++ )
     if( iStrateg[i][0] == command ) return iStrateg[i][1];

   return 0;
 }



TMyMDIFrame::TMyMDIFrame(LPSTR ATitle, LPSTR MenuId, PTModule AModule ):
  TMDIFrame( ATitle, MenuId, AModule )
 {
   ActiveChild_ = NULL;

   if( (lpMainPath = new char[MAXPATH + 1]) )
    {
      GetModuleFileName( GetApplication()->hInstance, lpMainPath, MAXPATH );

      char drive[MAXDRIVE], dir[MAXDIR], name[MAXFILE], ext[MAXEXT];
      _splitpath( lpMainPath, drive, dir, name, ext );      
      _makepath( lpMainPath, drive, dir, "", "" );
    }

   bFlInvertSelectMode = FALSE;
   ChildMenuPos = 8;
   DsDefaultScreenLayout = DS_4;
				 //TMDIFrame
   char cPath[MAXPATH + 1];   
   pGlobalFManeger = fmManeger = new TFileManeger( this, 5 );

   OpenDlgTransferInit();
   SaveDlgTransferInit();
   OpenMedDlgTransferInit();
   SaveMedDlgTransferInit();

   pStatusWnd = NULL;//new TStatusWindow( this, "" );

   vdtTransfer.pWD = NULL; vdtTransfer.iUseAsDefault = 0;
   vdtTransfer.iInitAlways = 0;

   int iIdN = SYSTOOL_FIRST_HANDL_BMPN + 1,
       iIdH = SYSTOOL_FIRST_HANDL_BMPH + 1,
       iCommand = SYSTOOL_FIRST_COMMAND + 1,
       idHint = SYSTOOL_FIRST_HINT_ID + 1;
   for( int count = 0; count < SYSTOOL_NUMB_BTN; count++, iIdN++,
     iIdH++, iCommand++, idHint++ )
    {
      PTButtonInit pInit = new TButtonInit( iIdN, iIdH, iCommand, hInstRC,
	GetCommandStrategy(iCommand), idHint  );
	//(iCommand < UCM_TB_CREATE_SIMPLEOBJ) ? FALSE:TRUE
      if( !pInit ) break;
      if( pInit->Status )
       {
	 GetApplication()->Error( pInit->Status );
	 delete pInit;
	 break;
       }
      listSysToolBarBmp.add( *pInit );
    }


   hfHint = CreateFont( -11, 0, 0, 0, FW_LIGHT,
     0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_STROKE_PRECIS,
     DEFAULT_QUALITY, DEFAULT_PITCH, "Arial" );

   pSysTool = new TToolBarWindow( this, "Vtools", &listSysToolBarBmp, 4, 6, hfHint, GetModule() );



   iIdN = BODYBMP_FIRST_HANDL_N + 1,
   iIdH = BODYBMPL_FIRST_HANDL_H + 1;

   HCURSOR  HcursOld = SetCursor( LoadCursor(NULL, IDC_WAIT) );  

         
   for( count = FT_Cylinder; count <= BODYBMP_NUMB; count++, iIdN++,
     iIdH++ )
    {
      PTFaceData pBody;

      switch( count )
       {
	 case FT_Cylinder:
	   pBody = new TCylinderInit();
	   break;

	 case FT_Cone:
	   pBody = new TConeInit();
	   break;

	 case FT_Sphere:
	   pBody = new TSphereInit();
	   break;

	 case FT_Torus:
	   pBody = new TTorusInit();
	   break;

	 case FT_Tube:
	   pBody = new TTubeInit();
	   break;
       }


      PTFaceItem pInit = new TFaceItem( iIdN, iIdH, hInstRC,
        pBody );
      if( !pInit ) break;
      if( pInit->Status )
       {
	 SetCursor( HcursOld );
	 GetApplication()->Error( pInit->Status );
	 delete pInit;
	 return;
       }
      listBodysBmp.add( *pInit );
    }

   SetCursor( HcursOld );

 };

void TMyMDIFrame::MoveSysToolbar() //call first when TMDIFrame::WMMove()
 {
   if( !pSysTool || IsZoomed(HWindow) ) return;

   if( IsIconic(HWindow) || IsIconic(pSysTool->HWindow) )
     { /*ShowWindow( pSysTool->HWindow, SW_HIDE );*/ return; }


   RECT r;

   int dSysToolX = Attr.X + Attr.W - pSysTool->Attr.X,
       dSysToolY = Attr.Y + Attr.H - pSysTool->Attr.Y;
   GetWindowRect( HWindow, &r );
   MoveWindow( pSysTool->HWindow, r.right - dSysToolX,
     r.bottom - dSysToolY,
     pSysTool->Attr.W,
     pSysTool->Attr.H, TRUE );
 }


//void TMyMDIFrame::( RTMessage msg )//WM_SIZE WM_MOVE

void TMyMDIFrame::WMMove( RTMessage msg )//WM_MOVE
 {
    //MoveSysToolbar();
    if( !(!pSysTool || IsZoomed(HWindow) ||
	IsIconic(HWindow) || (pSysTool && IsIconic(pSysTool->HWindow)))
      )
     {
       //RECT r;

       int keyX = Attr.X, keyY = Attr.Y;
       TMDIFrame::WMMove( msg );
       ChangeBounds();
       int deltaX = Attr.X - keyX,
	   deltaY = Attr.Y - keyY;

       //GetWindowRect( pSysTool->HWindow, &r );
       MoveWindow( pSysTool->HWindow,
		   pSysTool->Attr.X + deltaX,
		   pSysTool->Attr.Y + deltaY,
		   pSysTool->Attr.W,
		   pSysTool->Attr.H,
		   TRUE
                 );
     }
    else
     {
       TMDIFrame::WMMove( msg );
       ChangeBounds();
     }
 }

void TMyMDIFrame::ChangeBounds( )
 {
   RECT rect;
   POINT p1;

   if( !pStatusWnd ) return;
   if( IsIconic(HWindow) || IsIconic(pStatusWnd->HWindow) )
     { /*ShowWindow(pStatusWnd->HWindow, SW_HIDE);*/ return; }
		  //TMDIFrame
   GetClientRect( HWindow, &rect );

   int x1 = 0;
   int y1 = rect.bottom; //- pStatusWnd->GetHigh();
   if( y1 < 0 ) y1 = 0;
   int x2 = rect.right;
   if( x2 < 0 ) x2 = 0;
   int y2 = min( rect.bottom, pStatusWnd->GetHigh() );

   p1.x = x1; p1.y = y1;// p2.x = x2; p2.y = y2;
   ClientToScreen( HWindow, &p1 );
   //ClientToScreen( HWindow, &p2 );
   x1 = p1.x; y1 = p1.y; //x2 = p2.x; y2 = p2.y;

   MoveWindow( pStatusWnd->HWindow, x1, y1, x2, y2, TRUE );
 }

static int H, W;
void TMyMDIFrame::WMSize( RTMessage msg )//WM_SIZE WM_MOVE
 {
   TRECT  rectAfter;
   //GetClientRect( HWindow, &rectSource );
   W = Attr.W, H = Attr.H;

   MoveSysToolbar();
   TMDIFrame::WMSize( msg );
   ChangeBounds();

   ResizeAdaptiveChildren();
 }

void ResizeInstance( Pvoid Child, Pvoid pManeger )
 {
   int dKx, dKy;

   PTMyMDIChild  pChild = (PTMyMDIChild)Child;
   if( PTWindow(pChild)->isA() != classtypeMDIChWindow ) return;

   PTFileManeger pMan = (PTFileManeger)pManeger;
   PTFileInfo  pInfo = pMan->GetInfo( pChild->GetScId() );
   if( !pInfo || pInfo->bFlWndGrowAsParent == FALSE ||
     IsIconic(pChild->HWindow) == TRUE ||
     IsZoomed(pChild->HWindow) == TRUE
     )  return;
                      
   RECT rectOwn;
   GetClientRect( pChild->Parent->HWindow, &rectOwn );
   TWindowDivision  wdDiv = TWindowDivision( rectOwn, pInfo->DsDivScreen );
   
   PTMyRect pRectCur = wdDiv[ pChild->GetDivIndex() ];
   if( !pRectCur ) return;

   MoveWindow( pChild->HWindow, pRectCur->left, pRectCur->top,
     pRectCur->right - pRectCur->left, pRectCur->bottom - pRectCur->top, TRUE );

/*   RECT rect, rectOwn;
   GetClientRect( pChild->Parent->HWindow, &rectOwn );
   GetWindowRect( pChild->HWindow, &rect );

   dKx = W - rect.right;
   dKy = H - rect.bottom;

   rect.right += (rectOwn.right - dKx);
   rect.bottom += (rectOwn.bottom - dKy);
////////
   MoveWindow( pChild->HWindow, rect.left, rect.top,
     rect.right - rect.left, rect.bottom - rect.top, TRUE );*/

 }

void TMyMDIFrame::ResizeAdaptiveChildren()
 {
   ForEach( ResizeInstance, (Pvoid)fmManeger );
 }

void TMyMDIFrame::WMCreate( RTMessage Msg )//TMDIFrame
 {   
   TMDIFrame::WMCreate( Msg ); 
   //return;
   if( pStatusWnd )
     GetApplication()->MakeWindow( pStatusWnd );
   else
    {
      Status = ER_CREATEOBJECT;
      return;
    }

   if( pSysTool )
    {
      SetWindowPos( pSysTool->HWindow, HWND_TOP,
        0,
        0,
        0,
	0,  SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE );
      ShowWindow( pSysTool->HWindow, SW_SHOW );
    }

   RECT r;
   GetWindowRect( HWindow, &r );
   MoveWindow( HWindow, r.left, r.top, r.right-r.left+1, r.bottom-r.top+1, FALSE );

   //int widtIt = (Attr.W - 200 > 0) ? (Attr.W - 200):10;
   int widtIt = Attr.W-200;
   dafaultHandleStatusLine = SendMessage( pStatusWnd->HWindow, WM_COMMAND,
					  CM_ADDITEM, (LPARAM)widtIt
					);
   commonHandleStatusLine = SendMessage( pStatusWnd->HWindow, WM_COMMAND,
					  CM_ADDITEM, (LPARAM)(200)
					);
   SendMessage( pStatusWnd->HWindow, WM_COMMAND, CM_DEF_HANDLE,
		(LPARAM)dafaultHandleStatusLine );

   if( !pSysTool ) { GetApplication()->Error( ER_CREATEOBJECT ); return; }
   if( pSysTool->Status )
    {
      GetApplication()->Error( pSysTool->Status );
      //delete pSysTool;
      pSysTool->CloseWindow();
      pSysTool = NULL;
      return;
    }


   if( CountChildren() < 1) EnableWinConnectMenu( MF_DISABLED | MF_GRAYED );
   //GetApplication()->MakeWindow( pSysTool );

   //hStatus = pStatusWnd->GetHigh();
 }

TMyMDIFrame::~TMyMDIFrame()
 {
   if( fmManeger ) { delete fmManeger; fmManeger = NULL; }
   if( hfHint ) DeleteObject( hfHint );
   if( lpMainPath ) { delete []lpMainPath; lpMainPath = NULL; }
 }


void TMyMDIFrame::OpenDlgTransferInit()
 {
   openDlgTransfer.pComboBoxInput->AddString( "*.3dw", TRUE );
   getcwd( openDlgTransfer.chPath, MAXPATH );
   if( openDlgTransfer.chPath[_fstrlen(openDlgTransfer.chPath) - 1] != '\\' )
     _fstrcat( openDlgTransfer.chPath, "\\" );

   TAutoStringRc s1_( hInstRC, ID_MSG_52, 50 );
   openDlgTransfer.pComboBoxType->AddString( /*"*.*     All files"*/s1_(), FALSE );
   s1_ = TAutoStringRc( hInstRC, ID_MSG_53, 50 );
   openDlgTransfer.pComboBoxType->AddString( /*"*.MLI   Matirial script"*/s1_(), FALSE );
   s1_ = TAutoStringRc( hInstRC, ID_MSG_54, 50 );
   openDlgTransfer.pComboBoxType->AddString( /*"*.3DW   3D World scene"*/s1_(), TRUE );
   s1_ = TAutoStringRc( hInstRC, ID_MSG_55, 50 );
   openDlgTransfer.pComboBoxType->AddString( /*"*.DXF   CAD metafile"*/s1_(), FALSE );

   openDlgTransfer.bFlCheckName = TRUE;
 }


void TMyMDIFrame::SaveDlgTransferInit()
 {
   saveDlgTransfer.pComboBoxInput->AddString( "*.3dw", TRUE );
   getcwd( saveDlgTransfer.chPath, MAXPATH );
   if( saveDlgTransfer.chPath[_fstrlen(saveDlgTransfer.chPath) - 1] != '\\' )
     _fstrcat( saveDlgTransfer.chPath, "\\" );

   TAutoStringRc s1_( hInstRC, ID_MSG_52, 50 );
   saveDlgTransfer.pComboBoxType->AddString( /*"*.*     All files"*/s1_(), FALSE );
   s1_ = TAutoStringRc( hInstRC, ID_MSG_53, 50 );
   saveDlgTransfer.pComboBoxType->AddString( /*"*.MLI   Matirial script"*/s1_(), FALSE );
   s1_ = TAutoStringRc( hInstRC, ID_MSG_54, 50 );
   saveDlgTransfer.pComboBoxType->AddString( /*"*.3DW   3D World scene"*/s1_(), TRUE );
   s1_ = TAutoStringRc( hInstRC, ID_MSG_55, 50 );
   saveDlgTransfer.pComboBoxType->AddString( /*"*.DXF   CAD metafile"*/s1_(), FALSE );

   saveDlgTransfer.bFlCheckName = FALSE;
 }


void TMyMDIFrame::OpenMedDlgTransferInit()
 {
   openMedDlgTransfer.pComboBoxInput->AddString( "*.mli", TRUE );
   getcwd( openMedDlgTransfer.chPath, MAXPATH );
   if( openMedDlgTransfer.chPath[_fstrlen(openMedDlgTransfer.chPath) - 1] != '\\' )
     _fstrcat( openMedDlgTransfer.chPath, "\\" );

   TAutoStringRc s1_( hInstRC, ID_MSG_52, 50 );
   openMedDlgTransfer.pComboBoxType->AddString( /*"*.*     All files"*/s1_(), FALSE );
   s1_ = TAutoStringRc( hInstRC, ID_MSG_53, 50 );
   openMedDlgTransfer.pComboBoxType->AddString( /*"*.MLI   Matirial script"*/s1_(), TRUE );

   openMedDlgTransfer.bFlCheckName = TRUE;
 }


void TMyMDIFrame::SaveMedDlgTransferInit()
 {
   saveMedDlgTransfer.pComboBoxInput->AddString( "*.mli", TRUE );
   getcwd( saveMedDlgTransfer.chPath, MAXPATH );
   if( saveMedDlgTransfer.chPath[_fstrlen(saveMedDlgTransfer.chPath) - 1] != '\\' )
     _fstrcat( saveMedDlgTransfer.chPath, "\\" );

   TAutoStringRc s1_( hInstRC, ID_MSG_52, 50 );
   saveMedDlgTransfer.pComboBoxType->AddString( /*"*.*     All files"*/s1_(), FALSE );
   s1_ = TAutoStringRc( hInstRC, ID_MSG_53, 50 );
   saveMedDlgTransfer.pComboBoxType->AddString( /*"*.MLI   Matirial script"*/s1_(), TRUE );

   saveMedDlgTransfer.bFlCheckName = FALSE;
 }


LPSTR TMyMDIFrame::GetClassName()
 {
   return "3DW:MDI_WINDOW";
 }

void TMyMDIFrame::GetWindowClass( WNDCLASS _FAR & wc )
 {
 
   TMDIFrame::GetWindowClass( wc );
   if( !(wc.hIcon = LoadIcon( hInstRC, MAKEINTRESOURCE(_3DW_ICON) )) )
     GetApplication()->Error( ER_ICON_LOAD );

 }

//TMDIClient CLIENTCREATESTRUCT TMDIFrame WM_GET
PTWindowsObject TMyMDIFrame::InitChild()
 {                                                   
   PTMyMDIChild pTmpCh = new TMyMDIChild( this, pRectCur,
     &(RTScena)(listOfScene.peekHead()),
     vpCurrentInit, iDivIndexCurrent, bFlCurrentVisibleFlag, GetModule() );
 
   return pTmpCh;
 }


void TMyMDIFrame::CMAbout( RTMessage Msg )//TWindow TMessage
 {
   PTAboutDialog  PAbout = NULL;
  
   if( (PAbout = new TAboutDialog( this, "DlgAbout", DDB256_About1, TRUE, pMRC )) && !PAbout->Status )
     GetApplication()->ExecDialog( PAbout );
   else
    {
      GetApplication()->Error( PAbout ? PAbout->Status:ER_CREATEOBJECT );
      if( PAbout ) PAbout->CloseWindow();//delete PAbout;
    }
 }

/*static void CheckCloseInstance( Pvoid pChWin, Pvoid pFlag )
 {
   PTMyMDIChild pCh = PTMyMDIChild(pChWin);

   if( pCh->isA() != classtypeMDIChWindow ) return;

   if( *(BOOL *)pFlag == FALSE ) return;
   if( pCh->CanClose() == FALSE )  *(BOOL *)pFlag = FALSE;
   else if( pCh->isA() != classtypeStatusWindow )
     pCh->CloseWindow();
 }*/


BOOL TMyMDIFrame::CanClose()
 {
   /*BOOL bFlCanExit = TRUE;

   ForEach( CheckCloseInstance, &bFlCanExit );

   return bFlCanExit;*/
   return CloseChildren();
 }

void TMyMDIFrame::CMExit( RTMessage Msg )
 {

   if( CanClose() == FALSE ) return;

   TMDIFrame::CMExit( Msg );
 }


void TMyMDIFrame::CMSelectScrVports( RTMessage Msg )
 {
   BOOL bFlPredefined = (Msg.LParam == UCM_RESTORE_CHILD_DIV) ? TRUE:FALSE;

   PTViewsDlg pVD = NULL;
   PTFileInfo  pFileInf;      //WM_

   if( !CountChildren() ) return;//TMDIFrame
   PTMyMDIChild pActChild = (PTMyMDIChild)ActiveChild_;
   if( !pActChild || ActiveChild_->isA() != classtypeMDIChWindow ) return;
   if( pActChild->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   if( !(pFileInf = fmManeger->GetInfo( pActChild->GetScId() )) )
    {
      TAutoStringRc str_( hInstRC, ID_MSG_1 );
      TAutoStringRc ttl_( hInstRC, ID_TITLE_ERROR, 10 );
      BWCCMessageBox( HWindow, str_(), ttl_(), MB_ICONSTOP | MB_OK | MB_APPLMODAL );
      //"Internal application error: MDIFrame cann't get INFO for active child"
      return;
    }

   RECT rect;
   GetClientRect( HWindow, &rect );
   TWindowDivision  winDiv = TWindowDivision( rect, pFileInf->DsDivScreen );
   LoadVPoints( pActChild, winDiv );
////////

   vdtTransfer.pWD = &winDiv; vdtTransfer.iUseAsDefault = 0;
   //vdtTransfer.iInitAlways = 0;

   if( bFlPredefined || ((pVD = new TViewsDlg( this, MAKEINTRESOURCE(DlgViewsDiv),
       &vdtTransfer, pMRC)) && !pVD->Status) )
    {      
      if( bFlPredefined == TRUE || pMRC->ExecDialog( pVD ) == IDOK )
       {
	 if( bFlPredefined == FALSE && vdtTransfer.iUseAsDefault )
	   DsDefaultScreenLayout = winDiv.Style;

	 PTFileInfo pInfo = fmManeger->GetInfo( pActChild->GetScId() );
         

	 if( bFlPredefined == FALSE && !vdtTransfer.iInitAlways &&
	     winDiv.Style == pInfo->DsDivScreen )
	  {
	    AssignNewVPoints( pActChild, winDiv );
	    return;
          }

	 PTScena pScena = pActChild->GetScena();

	 pScena->Lock( TRUE );

	 
	 TMessage msg; _fsetmem( &msg, sizeof(TMessage), 0 );
	 msg.LParam = MAKELONG( 7, 7 );
	 msg.WParam = pFileInf->IdFile;
         CMClose( msg );   

	 listOfScene.detach( *pScena, TShouldDelete::NoDelete );
	 listOfScene.add( *pScena );
	 CreateScreenLayoutFor( pFileInf, winDiv, pScena );

	 pScena->Lock( FALSE );
       }
    }
   else
    {
      GetApplication()->Error( pVD ? pVD->Status:ER_CREATEOBJECT );
      if( pVD ) pVD->CloseWindow();//delete pVD;
    }

 }


struct TAssinData
 {
   PTWindowDivision pWD;
   int Id;
   PTFileInfo pInfo;
 };

void AssignInstance( Pvoid pCh, Pvoid pPar )
 {
   PTMyMDIChild  pChild = (PTMyMDIChild)pCh;
   TAssinData *pasdData = (TAssinData *)pPar;
   if( PTWindow(pChild)->isA() != classtypeMDIChWindow ||
     pChild->GetScId() != pasdData->Id ) return;

   double dAngleH, dAngleV;
   TViewPoint vpView;

   PTMyRect pRect = (*(pasdData->pWD))[ pChild->GetDivIndex() ];
   if( !pRect ) return;

   pChild->GetVpoint( vpView, dAngleH, dAngleV );
   //pChild->ChangeVPoint( pRect->VpVPoint, dAngleH, dAngleV );
   pChild->ChangeVPoint( pRect->VpVPoint );
   pChild->GenTitle( pasdData->pInfo );
   InvalidateRect( pChild->HWindow, NULL, TRUE );
 }

void TMyMDIFrame::AssignNewVPoints( PTMyMDIChild pCh, RTWindowDivision rWD )
 {
   TAssinData asdData;

   asdData.pWD = &rWD; asdData.Id = pCh->GetScId();
   asdData.pInfo = fmManeger->GetInfo( pCh->GetScId() );
   if( !asdData.pInfo )
    {
      TAutoStringRc str_( hInstRC, ID_MSG_1 );
      TAutoStringRc ttl_( hInstRC, ID_TITLE_ERROR, 10 );
      BWCCMessageBox( HWindow, str_(), ttl_(), MB_ICONSTOP | MB_OK | MB_APPLMODAL );
      //"Internal application error: MDIFrame cann't get INFO for active child"
      return;
    }
   ForEach( AssignInstance, (Pvoid)(&asdData) );
 }

void LoadVPInstance( Pvoid pCh, Pvoid pPar )
 {
   PTMyMDIChild  pChild = (PTMyMDIChild)pCh;
   TAssinData *pasdData = (TAssinData *)pPar;
   if( PTWindow(pChild)->isA() != classtypeMDIChWindow ||
     pChild->GetScId() != pasdData->Id ) return;

   double dAngleH, dAngleV;
   TViewPoint vpView;

   PTMyRect pRect = (*(pasdData->pWD))[ pChild->GetDivIndex() ];
	    
   if( !pRect ) return;

   pChild->GetVpoint( vpView, dAngleH, dAngleV );
   pRect->VpVPoint = vpView;
   //pasdData->pWD->
 }


void TMyMDIFrame::LoadVPoints( PTMyMDIChild pCh, RTWindowDivision rWD )
 {
   TAssinData asdData;

   asdData.pWD = &rWD; asdData.Id = pCh->GetScId();
   asdData.pInfo = fmManeger->GetInfo( pCh->GetScId() );
   if( !asdData.pInfo )
    {
      TAutoStringRc str_( hInstRC, ID_MSG_1 );
      TAutoStringRc ttl_( hInstRC, ID_TITLE_ERROR, 10 );
      BWCCMessageBox( HWindow, str_(), ttl_(), MB_ICONSTOP | MB_OK | MB_APPLMODAL );
      //"Internal application error: MDIFrame cann't get INFO for active child"
      return;
    }
   ForEach( LoadVPInstance, (Pvoid)(&asdData) );
 }




void TMyMDIFrame::EnableWinConnectMenu( UINT uiFl )//enbl/dsbl child connect menu items
 {
   HWND   hwndCh; 
   if( ActiveChild_ && ActiveChild_->isA() == classtypeMDIChWindow )
     hwndCh = ActiveChild_->HWindow;
   else hwndCh = NULL;

   int incr = MenuPosIncr(hwndCh);
   HMENU  hMain;                                                                                        
   HMENU  hFile = GetSubMenu( (hMain = GetMenu(GetApplication()->MainWindow->HWindow)), 0 + incr ),
	  hOpt  = GetSubMenu( hMain, 7 + incr );

   EnableMenuItem( hMain, 1 + incr, MF_BYPOSITION | uiFl );
   EnableMenuItem( hMain, 2 + incr, MF_BYPOSITION | uiFl );
   EnableMenuItem( hMain, 8 + incr, MF_BYPOSITION | uiFl );

   EnableMenuItem( hMain, 3 + incr, MF_BYPOSITION | uiFl );
   EnableMenuItem( hMain, 4 + incr, MF_BYPOSITION | uiFl );
   EnableMenuItem( hMain, 5 + incr, MF_BYPOSITION | uiFl );
   EnableMenuItem( hMain, 6 + incr, MF_BYPOSITION | uiFl );

   EnableMenuItem( hFile, UCM_SAVE, MF_BYCOMMAND | uiFl );
   EnableMenuItem( hFile, UCM_SAVE_AS, MF_BYCOMMAND | uiFl );
   EnableMenuItem( hFile, UCM_CLOSE, MF_BYCOMMAND | uiFl );
   EnableMenuItem( hFile, UCM_CLOSE_ALL, MF_BYCOMMAND | uiFl );
   EnableMenuItem( hFile, UCM_RETRIVE_SOURCE, MF_BYCOMMAND | uiFl );

   EnableMenuItem( hOpt, UCM_SCENEINFO, MF_BYCOMMAND | uiFl );

   DrawMenuBar( GetApplication()->MainWindow->HWindow );
 }
            
void TMyMDIFrame::CreateScreenLayoutFor( TFileInfo *pFileInf, RTWindowDivision dsLayput, PTScena pScena )
 {
   RECT rectMainClient;
   GetClientRect( HWindow, &rectMainClient );
   TWindowDivision  wdDiv = TWindowDivision( rectMainClient, dsLayput.Style );
   pFileInf->DsDivScreen = dsLayput.Style;


   PTMyMDIChild pmcNextWindow = NULL;
   bFlCurrentVisibleFlag = FALSE;
   for( int i = 0; (pRectCur = wdDiv[i]); i++ )
    {
      vpCurrentInit = dsLayput[i]->VpVPoint;
					
      iDivIndexCurrent = i;
      pmcNextWindow = (PTMyMDIChild)CreateChild();
      pmcNextWindow->GenTitle( pFileInf );      
    }

   if( pmcNextWindow )
     ForEach( (TActionMemFunc)&TMyMDIChild::EnableShowInstance, Pvoid(pmcNextWindow->GetScId()) );    

 }


void TMyMDIFrame::CMNewFile( RTMessage )
 {
   pfiCurrentInit = fmManeger->NewFile( DsDefaultScreenLayout );
   PTScena  pNewScena = new TScena( pfiCurrentInit->IdFile, GetApplication() );
   listOfScene.add( *pNewScena );

   RECT rect; _fsetmem( &rect, sizeof(RECT), -1 );
   TWindowDivision wDiv = TWindowDivision( rect, DsDefaultScreenLayout );
   CreateScreenLayoutFor( pfiCurrentInit, wDiv, pNewScena );

   EnableWinConnectMenu( MF_ENABLED );
 }


void TMyMDIFrame::CMRestoreFromSource( RTMessage )
 {
   SetErrorMode( SEM_FAILCRITICALERRORS );
  
   PTMyMDIChild pCh = ((PTMyMDIChild)ActiveChild_);
   if( !pCh || ActiveChild_->isA() != classtypeMDIChWindow ) return;
   if( pCh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   PTFileInfo pFInfo = fmManeger->GetInfo( pCh->GetScId() );
   if( !pFInfo )
    {
      TAutoStringRc str_( hInstRC, ID_MSG_1 );
      TAutoStringRc ttl_( hInstRC, ID_TITLE_ERROR, 10 );
      BWCCMessageBox( HWindow, str_(), ttl_(), MB_ICONSTOP | MB_OK | MB_APPLMODAL );
      //"Internal application error: MDIFrame cann't get INFO for active child"
      return;
    }


   OFSTRUCT of; 
   if( OpenFile(pFInfo->LpFileNameFull, &of, OF_EXIST) == HFILE_ERROR )
    {
      MakeMsgErr( hInstRC, HWindow, IO_ERR_READ,
	      ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
	      pFInfo->LpFileNameFull );      
      return;	    
    }

   if( pCh->GetScena()->Flush( TRUE ) == FALSE ) return;

   
   TAutoFileRc<HFILE> hf( OpenFile( pFInfo->LpFileNameFull, &of, OF_READ ) );
   //HFILE hf = OpenFile( pFInfo->LpFileNameFull, &of, OF_READ );

   if( hf() == HFILE_ERROR )
    {
      MakeMsgErr( hInstRC, HWindow, IO_ERR_READ,
        ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
	pFInfo->LpFileNameFull );
     return;
    }


   TMDIChFlagHolder  hold( pCh, &TMyMDIChild::SetCriticalLock, TRUE );
   pCh->LockForReenter( TRUE );

   long sizeFile;
   lseek( hf(), 0, SEEK_END );
   sizeFile = tell( hf() );
   lseek( hf(), 0, SEEK_SET );
   PTIndicator  pInd = new TIndicator( this, "Restore", sizeFile, pMRC );
   pInd->AddStr( pFInfo->LpFileNameFull );
   pInd->Show();
   pInd->SetStatusButton( FALSE );


   int res1;
   res1 = pCh->GetScena()->Load( hf(), pInd );
   pInd->WorkIs100();
   pInd->Close();

   pCh->LockForReenter( FALSE );
                          

   if( res1 < 0 )
    {
      pCh->GetScena()->Flush( FALSE );
      pCh->RedrawVports();

      switch( res1 )
       {
	 case -1:
	   TAutoStringRc str_( hInstRC, ID_MSG_2 );
	   TAutoStringRc ttl_( hInstRC, ID_TITLE_ERROR, 10 );
	   BWCCMessageBox( HWindow, str_(), ttl_(), MB_ICONSTOP | MB_OK | MB_APPLMODAL );
	   //"Internal application error: From 'Open'"
	   break;

	 case -2:
	   GetApplication()->Error( EM_OUTOFMEMORY );
	   break;

	 case -3:
	   MakeMsgErr( hInstRC, HWindow, IO_ERR_READ,
             ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
	     pFInfo->LpFileNameFull );
	   break;

	 case -4:
	   MakeMsg( hInstRC, HWindow, IO_ERR_INVALIDFORMAT,
	      ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
	      pFInfo->LpFileNameFull );
	   break;
       }

      //_lclose( hf );
      return;
    }

   //_lclose( hf );
   pCh->RedrawVports();
 }





void TMyMDIFrame::CMHold( RTMessage Msg )
 {
   SetErrorMode( SEM_FAILCRITICALERRORS );

   CheckToolbar( TMyMDIChild::GetMode_() );

   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( !pMDICh || PTWindow(pMDICh)->isA() != classtypeMDIChWindow ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   PTFileInfo pFInfo = fmManeger->GetInfo( pMDICh->GetScId() );
   if( !pFInfo )
    {
      TAutoStringRc str_( hInstRC, ID_MSG_1 );
      TAutoStringRc ttl_( hInstRC, ID_TITLE_ERROR, 10 );
      BWCCMessageBox( HWindow, str_(), ttl_(), MB_ICONSTOP | MB_OK | MB_APPLMODAL );
      //"Internal application error: MDIFrame cann't get INFO for active child"
      return;
    }
   LPSTR lpName = fmManeger->GetHoldName( pMDICh->GetScId() );


   if( !lpName )
    {
      MakeMsgErr( hInstRC, HWindow, IO_ERR_CREATEHOLD,
	         ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
		 pFInfo->LpFileNameFull );
      return;
    }


   OFSTRUCT of;
   TAutoFileRc<HFILE> hf( OpenFile( lpName, &of, OF_WRITE | OF_CREATE ) );
   //HFILE hf = OpenFile( lpName, &of, OF_WRITE | OF_CREATE );
   if( hf() == HFILE_ERROR )
    {
      MakeMsgErr( hInstRC, HWindow, IO_ERR_CREATEHOLD,
	         ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
		 pFInfo->LpFileNameFull );
      return;
    }

    int res1;
 

    TMDIChFlagHolder  hold( pMDICh, &TMyMDIChild::SetCriticalLock, TRUE );

    pMDICh->LockForReenter( TRUE );

    TAutoStringRc s1_( hInstRC, ID_MSG_57, 20 );
    PTIndicator  pInd = new TIndicator( this, /*"Hold file"*/s1_(), pMDICh->GetScena()->GetBytesSize(FALSE, FALSE), pMRC );
    pInd->AddStr( pFInfo->LpFileNameFull );
    pInd->Show();


    res1 = pMDICh->GetScena()->Save( hf(), pInd );
    pInd->WorkIs100();
    pInd->Close();

    //_lclose( hf );
    pMDICh->GetScena()->SetHold( TRUE );

    pMDICh->LockForReenter( FALSE );

    if( res1 < 0 )
     { 
       switch( res1 )
        {
	  case -1:
	    TAutoStringRc str_( hInstRC, ID_MSG_4 );
	    TAutoStringRc ttl_( hInstRC, ID_TITLE_ERROR, 10 );
	    BWCCMessageBox( HWindow, str_(),
	      ttl_(), MB_ICONSTOP | MB_OK | MB_APPLMODAL );
	    //"Internal application error: From 'CMHold'"
	    break;

	  case -2:
	    GetApplication()->Error( EM_OUTOFMEMORY );
	    break;

	  case -3:
	    MakeMsgErr( hInstRC, HWindow, IO_ERR_WRITE,
              ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
	      lpName );
	    break;
	}

       pMDICh->GetScena()->SetHold( FALSE );
       fmManeger->UnHold( pFInfo );
       return;
     }

 }

void TMyMDIFrame::CMFetch( RTMessage Msg )
 {
   CheckToolbar( TMyMDIChild::GetMode_() );

   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( !pMDICh || PTWindow(pMDICh)->isA() != classtypeMDIChWindow ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   PTFileInfo pFInfo = fmManeger->GetInfo( pMDICh->GetScId() );
   if( !pFInfo )
    {
      TAutoStringRc str_( hInstRC, ID_MSG_1 );
      TAutoStringRc ttl_( hInstRC, ID_TITLE_ERROR, 10 );
      BWCCMessageBox( HWindow, str_(),
	   ttl_(), MB_ICONSTOP | MB_OK | MB_APPLMODAL );
      //"Internal application error: MDIFrame cann't get INFO for active child"
      return;
    }


   if( pMDICh->GetScena()->GetHold() == FALSE )
    {
      MakeMsg( hInstRC, HWindow, IO_ERR_NOTHOLDED,
		 ID_TITLE_MESSAGE, MB_ICONASTERISK | MB_OK | MB_APPLMODAL,
		 pFInfo->LpFileNameFull );
      return;
    }

   LPSTR lpName = fmManeger->GetHoldName( pMDICh->GetScId() );

   if( !lpName )
    {
      char cBuf[180];

      TAutoStringRc s1_( hInstRC, ID_MSG_58, 256 );
      wsprintf( cBuf, /*"For file: [%s] error get hold name"*/s1_(), pFInfo->LpFileNameFull );
      TAutoStringRc s2_( hInstRC, ID_TITLE_ERROR, 10 );
      BWCCMessageBox( HWindow, cBuf,
	   s2_(), MB_ICONSTOP | MB_OK | MB_APPLMODAL );
      return;
    }


   OFSTRUCT of;
   TAutoFileRc<HFILE> hf( OpenFile( lpName, &of, OF_READ ) );
   //HFILE hf = OpenFile( lpName, &of, OF_READ );
   if( hf() == HFILE_ERROR )
    {
      //delete pNewScena;

      MakeMsgErr( hInstRC, HWindow, IO_ERR_READ,
	ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
	lpName );

      return;	    
    }

   int res1;

   TMDIChFlagHolder  hold( pMDICh, &TMyMDIChild::SetCriticalLock, TRUE );

   pMDICh->LockForReenter( TRUE );

   lseek( hf(), 0, SEEK_END );
   long sizeFile = tell( hf() );
   lseek( hf(), 0, SEEK_SET );
   TAutoStringRc s1_( hInstRC, ID_MSG_59, 20 );
   PTIndicator  pInd = new TIndicator( this, /*"Fetch file"*/s1_(), sizeFile, pMRC );
   pInd->AddStr( pFInfo->LpFileNameFull );
   pInd->Show();

   pMDICh->GetScena()->Flush( FALSE );

   res1 = pMDICh->GetScena()->Load( hf(), pInd );
   pInd->WorkIs100();

   pInd->Close();

   if( res1 < 0 )
    {
      pMDICh->GetScena()->Flush( FALSE );
      pMDICh->RedrawVports();

      switch( res1 )
       {
	 case -1:
	   TAutoStringRc str_( hInstRC, ID_MSG_5 );
	   TAutoStringRc ttl_( hInstRC, ID_TITLE_ERROR, 10 );
	   BWCCMessageBox( HWindow, str_(),
	     ttl_(), MB_ICONSTOP | MB_OK | MB_APPLMODAL );
	   //"Internal application error: From 'CMFetch'"
	   break;

	 case -2:
	   GetApplication()->Error( EM_OUTOFMEMORY );
	   break;

	 case -3:
	   MakeMsgErr( hInstRC, HWindow, IO_ERR_READ,
             ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
	     pFInfo->LpFileNameFull );
	   break;

	 case -4:
	   MakeMsg( hInstRC, HWindow, IO_ERR_INVALIDFORMAT,
	      ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
	      pFInfo->LpFileNameFull );
	   break;

       }

      //_lclose( hf );
      pMDICh->LockForReenter( FALSE );
      return;
    }

   //_lclose( hf );

   pMDICh->RedrawVports();
   pMDICh->LockForReenter( FALSE );
 }


struct TCloseDta {
  int id;
  int calc;

  TCloseDta( int iId ) { id = iId; calc = 0; }
 };
typedef TCloseDta _FAR *PTCloseDta;

BOOL CloseInstance( Pvoid pCh, Pvoid pParam )
 {
   PTMyMDIChild  pChild = (PTMyMDIChild)pCh;

   if( PTWindow(pChild)->isA() != classtypeMDIChWindow ||
       pChild->GetScId() != PTCloseDta(pParam)->id
     ) return 0;

   if( pChild->CanClose( 1 ) == FALSE ) return 1;

   //SendMessage( pChild->HWindow, WM_CLOSE, 0L, 0L );
   //pChild->CloseWindow();
   pChild->Destroy();
   delete pChild;

   PTCloseDta(pParam)->calc++;

   return 0;
 }

void TMyMDIFrame::CMClose( RTMessage msg )//TMDIFrame TWindow
 {
   msg.Result = 0;
   if( !ActiveChild_ || ActiveChild_->isA() != classtypeMDIChWindow ) return;
   if( LOWORD(msg.LParam) == 7 && HIWORD(msg.LParam) == 7 &&
       msg.WParam != ((PTMyMDIChild)ActiveChild_)->GetScId()
     ) return;

   TCloseDta  cdDta( ((PTMyMDIChild)ActiveChild_)->GetScId() );

   msg.Result = FirstThat( CloseInstance, Pvoid(&cdDta) ) ? -1L:cdDta.calc;
   //msg.Result = 0;
 }

BOOL TMyMDIFrame::CloseChildren()
 {
   TMessage msg; _fsetmem( &msg, sizeof(TMessage), 0 );

   while( ActiveChild_ && ActiveChild_->isA() == classtypeMDIChWindow )
    {      
      CMClose( msg );
      if( msg.Result == -1L ) return FALSE;
    }

   return TRUE;
 }



PTScena  TMyMDIFrame::GetScena( UINT id )
 {
   ListIterator liIter = ListIterator( listOfScene );
   PTScena pScena = &(RTScena)liIter.current();

   for( ; int(liIter); pScena = &(RTScena)++liIter )
     if( pScena->GetId() == id ) return pScena;

   return NULL;
 }


void TMyMDIFrame::WMCommand( RTMessage Msg ) //WM_Command
 {

   switch( HIWORD(Msg.LParam) )
    {
      case CM_FIRST + UCM_CLOSE_SCENE:
	CloseScene( int(Msg.WParam) );
        Msg.Result = 0;
        return;

      case CM_FIRST + UCM_SET_CHECKED_IN_MENU:
	SetCheckedInMenu( FALSE );
	Msg.Result = 0;
	return;

      case CM_FIRST + UCM_SAVE:
	CMSave( Msg );
	return;
    };

  
   if( LOWORD(Msg.WParam) >= UCM_FILEACESS_FIRST && !Msg.LParam )
    {
      LPSTR lpName;

      if( (lpName = fmManeger->GetFileNameMenu(LOWORD(Msg.WParam))) )
       {
	 Open( lpName );
         Msg.Result = 0;
	 return;
       }
    }

   switch( Msg.WParam )
    {
      case CM_ADDITEM:
      case CM_DELETEITEM:
      case CM_SENDTEXT:
      case CM_SETITEM_COLOR:
	if( pStatusWnd )
	  SendMessage( pStatusWnd->HWindow, WM_COMMAND, Msg.WParam, Msg.LParam );
	return;
    }

   TMDIFrame::WMCommand( Msg );
 }


static int countCh = 0;

void CountInstance( Pvoid pCh, Pvoid pParam )
 {
   PTMyMDIChild pChild = PTMyMDIChild( pCh );

   if( PTWindow(pChild)->isA() != classtypeMDIChWindow ) return;

   /*if( pChild->GetPhaze() == TRUE )*/ countCh++;
 }

int TMyMDIFrame::CountChildren()
 {
   countCh = 0;
   ForEach( &CountInstance, NULL );
   return countCh;
 }

	//TMDIFrame CLIENTCREATESTRUCT  
void TMyMDIFrame::CloseScene( int idClose ) //resiv by child
 {
           ////////
   PTScena pScena = GetScena( idClose );

   if( !pScena || pScena->CanClose() == FALSE ) return;

   fmManeger->CloseFile( idClose );
   if( CountChildren() < 1) EnableWinConnectMenu( MF_DISABLED | MF_GRAYED );
   
   listOfScene.detach( *pScena, TShouldDelete::Delete );

   //MessageBeep( -1 ); MessageBeep( -1 ); MessageBeep( -1 );
 }


void TMyMDIFrame::SetCheckedInMenu( BOOL bFlDefault )
 {
  PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
  if( (!pMDICh || PTWindow(pMDICh)->isA() != classtypeMDIChWindow) && bFlDefault != TRUE ) return;
  HWND hwndCh = pMDICh ? pMDICh->HWindow:NULL;
  int incr = MenuPosIncr(hwndCh);


   HMENU hmMain = GetMenu( HWindow );
   HMENU hm = GetSubMenu( hmMain, 2 + incr );
   HMENU hm1 = GetSubMenu( hmMain, 1 + incr  );
   HMENU hmBP = GetSubMenu( hm, 13 );


   CheckMenuItem( hmBP, UCM_USE_GLOBAL_BP, MF_BYCOMMAND |
     ((TMyMDIChild::UsedGlobalPoint() == TRUE) ? MF_CHECKED:MF_UNCHECKED)
		);

   ModifyMenu( hmBP, UCM_SET_BP_VISIBLE, MF_BYCOMMAND, UCM_SET_BP_VISIBLE, 
     (bFlDefault == TRUE || pMDICh->IsVisibleBasePoint() == TRUE) ? "Unvisible global base":
								    "Visible global base"
	     );

   ModifyMenu( hm1, UCM_USE_CROSSING_FRAME, MF_BYCOMMAND, UCM_USE_CROSSING_FRAME,
     (TMyMDIChild::UsedCrossing() == TRUE) ? "Use window frame":
					     "Use crossing frame"
	     );


   CheckMenuItem( hm1, UCM_INVERTSELMODE, MF_BYCOMMAND |
     ((bFlInvertSelectMode == TRUE ) ? MF_CHECKED:MF_UNCHECKED)
		);

   /*int c = MF_CHECKED, unc = MF_UNCHECKED;
   int resc = bFlDefault == TRUE || !PTMyMDIChild(ActiveChild_)->GetScena() || PTMyMDIChild(ActiveChild_)->GetScena()->IsUsedBaseUSC() == TRUE;
   int resc2;
   if(ActiveChild_)
     resc2 = PTMyMDIChild(ActiveChild_)->GetScena()->IsUsedBaseUSC();*/
   CheckMenuItem( hm1, UCM_USE_BASEUCS, MF_BYCOMMAND |
     ((bFlDefault == TRUE || !PTMyMDIChild(ActiveChild_)->GetScena() || PTMyMDIChild(ActiveChild_)->GetScena()->IsUsedBaseUSC() == TRUE) ? MF_CHECKED:MF_UNCHECKED)
		);

   CheckToolbarIndepend();
    /*
   if( pSysTool )
    {
      pSysTool->SpecialCheck( UCM_TB_INVERTSELMODE, bFlInvertSelectMode );
      pSysTool->SpecialCheck( UCM_TB_CONECT, TMyMDIChild::IsTransformAsTransforming() );
      pSysTool->SpecialCheck( UCM_TB_GROUP, TMyMDIChild::IsTransformSelected()  );
    } */


   CheckMenuItem( hm, UCM_USE_GRID, MF_BYCOMMAND |
     (( bFlDefault == FALSE && pMDICh->IsGreedVis() == TRUE ) ? MF_CHECKED:MF_UNCHECKED)
		);

   CheckMenuItem( hm, UCM_USE_SNAP, MF_BYCOMMAND |
     (( bFlDefault == FALSE && pMDICh->IsSnapUsed() == TRUE ) ? MF_CHECKED:MF_UNCHECKED)
		);

   CheckMenuItem( hm, UCM_AXIS_ON_OFF, MF_BYCOMMAND |
     (( bFlDefault == FALSE && pMDICh->IsUCSVis() == TRUE ) ? MF_CHECKED:MF_UNCHECKED)
		);
  
   CheckMenuItem( hm, UCM_HIDEGEOMETRY, MF_BYCOMMAND |
     (( bFlDefault == FALSE && pMDICh->IsHideGeometry() == TRUE ) ? MF_CHECKED:MF_UNCHECKED)
		);

   CheckMenuItem( hm, UCM_FULLDETALL, MF_BYCOMMAND |
     (( bFlDefault == FALSE && pMDICh->IsFullDetal() == TRUE ) ? MF_CHECKED:MF_UNCHECKED)
		);

   UINT  flPrev1 = GetMenuState( hmMain, 1 + incr, MF_BYPOSITION ),
	 flPrev2 = GetMenuState( hmMain, 6 + incr, MF_BYPOSITION );

   UINT  flState, key;
   if( bFlDefault == FALSE && (pMDICh->GetVpoint_() == TUCS::VP_Camera ||
       pMDICh->GetVpoint_() == TUCS::VP_None ||
       pMDICh->AskBitmap() == TRUE) )
     flState = MF_DISABLED | MF_GRAYED;
   else flState = MF_ENABLED;

   key = flState;
   EnableMenuItem( hmMain, 1 + incr, MF_BYPOSITION | flState );
   EnableMenuItem( hmMain, 3 + incr, MF_BYPOSITION | flState );
   EnableMenuItem( hmMain, 4 + incr, MF_BYPOSITION | flState );
   EnableMenuItem( hmMain, 5 + incr, MF_BYPOSITION | flState );

   EnableMenuItem( hm, UCM_GRID_EXTENTS, MF_BYCOMMAND | flState );
   EnableMenuItem( hm, UCM_USE_SNAP, MF_BYCOMMAND | flState );
   EnableMenuItem( hm, UCM_USE_GRID, MF_BYCOMMAND | flState );
   EnableMenuItem( hm, UCM_AXIS_ON_OFF, MF_BYCOMMAND | flState );
   EnableMenuItem( hm, UCM_USERUCS_SETUP, MF_BYCOMMAND | flState );

   EnableMenuItem( hm, 13, MF_BYPOSITION | flState );
   EnableMenuItem( hm, 16, MF_BYPOSITION | flState );
   EnableMenuItem( hm, 17, MF_BYPOSITION | flState );
   EnableMenuItem( hm, UCM_F_FIT_TO_ZOOM, MF_BYCOMMAND | flState );

   if( bFlDefault == FALSE && (pMDICh->GetVpoint_() == TUCS::VP_None ||
       pMDICh->AskBitmap() == TRUE) )
     flState = MF_DISABLED | MF_GRAYED;
   else flState = MF_ENABLED;

   EnableMenuItem( hmMain, 6 + incr, MF_BYPOSITION | flState );

   if( ((flPrev1 & MF_GRAYED) != (key & MF_GRAYED) ||
	(flPrev2 & MF_GRAYED) != (flState & MF_GRAYED)) &&
	(bFlDefault == FALSE)
     )
     DrawMenuBar( HWindow );

 /*  CheckMenuItem( hm, UCM_USE_SNAP, MF_BYCOMMAND |
     ((pMDICh->ToggleGreed() == TRUE) ? MF_CHECKED:MF_UNCHECKED)
		);*/
////

 }

void TMyMDIFrame::CMUseGrid( RTMessage )//TMDIFrame
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   //if( !pMDICh || pMDICh->isA() != classtypeMDIChWindow ) return;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   pMDICh->ToggleGreed();
   SetCheckedInMenu();
 }


void TMyMDIFrame::CMGridExtents( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   //if( !pMDICh || PTWindow(pMDICh)->isA() != classtypeMDIChWindow ) return;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   pMDICh->SetGridExtents();
 }

void TMyMDIFrame::CMUseSnap( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   //if( !pMDICh || PTWindow(pMDICh)->isA() != classtypeMDIChWindow ) return;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   pMDICh->ToggleSnap();
   SetCheckedInMenu();
 }

void TMyMDIFrame::CMAxisOnOff( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   //if( !pMDICh || PTWindow(pMDICh)->isA() != windowClass ) return;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   pMDICh->ToggleUCS();
   SetCheckedInMenu();
 }

void TMyMDIFrame::CMSMoth( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   pMDICh->GetFacetAndOp( MC_Smoth );
 }

void TMyMDIFrame::CMVPortAlign( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   pMDICh->GetFacetAndOp( MC_VPortAligment );
 }

void TMyMDIFrame::CMUserUcsSetup( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   //if( !pMDICh || PTWindow(pMDICh)->isA() != windowClass ) return;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }
   
   pMDICh->UserUCSSetup();
 }

void TMyMDIFrame::CMUserUcsShift( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   //if( !pMDICh || PTWindow(pMDICh)->isA() != windowClass ) return;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }
   
   pMDICh->UserUCSShift();
 }

BOOL TMyMDIFrame::CondVisyalMDI( PTMyMDIChild pMDICh ) const
 {
   if( !pMDICh || PTWindow(pMDICh)->isA() != classtypeMDIChWindow ||
     IsIconic(pMDICh->HWindow) ) return FALSE;

   return TRUE;
 }

void TMyMDIFrame::CMUserUcsScale( RTMessage msg )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   //if( !pMDICh || PTWindow(pMDICh)->isA() != windowClass ) return;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;   

   if( pMDICh->ValidWindowOper(MC_SetupUCS) == FALSE ) return;

   double scale = pMDICh->GetScale();
   if( msg.Result == 1 ) scale *= 1.5;
   else scale /= 1.5;

   if( scale < 0.0001 ) scale = 0.0001;

   pMDICh->ChangeScale( scale );
   InvalidateRect( pMDICh->HWindow, NULL, TRUE );
   pMDICh->GenTitle( fmManeger->GetInfo(pMDICh->GetScId()) );
   PostMessage( HWindow, WM_COMMAND, (WPARAM)TMyMDIChild::GetMode_(),
      MAKELONG(HWindow, UCM_CHANGED_CHILD_MODE) );
 }

void TMyMDIFrame::CMFitToZoom( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   pMDICh->FitToZoom();
   InvalidateRect( pMDICh->HWindow, NULL, TRUE );
   pMDICh->GenTitle( fmManeger->GetInfo(pMDICh->GetScId()) );
 }

void TMyMDIFrame::CMFitToWindow( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   pMDICh->FitToWindow();
   //InvalidateRect( pMDICh->HWindow, NULL, TRUE );
   //pMDICh->GenTitle( fmManeger->GetInfo(pMDICh->GetScId()) );
 }

static int ItemFind( const Object& rObj, Pvoid pPar )
 {
   return (((RTFaceItem)rObj).pFData->isA() == (UINT)pPar );
 }


void TMyMDIFrame::CMCreateSimpleObj( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   if( pMDICh->ValidWindowOper(MC_CreCylinder) == FALSE ) return;

   static FaceTypes ftSelFace = 1;
   PTFaceDialog pFaceDlg = NULL;
     //TFaceDialog( PTWindow AParent, DoubleList *pList,
    //FaceTypes *pSelect, PTModule AModule = NULL );

   int res;

   if( (pFaceDlg = new TFaceDialog( this, &listBodysBmp, &ftSelFace, pMRC )) && !pFaceDlg->Status )
     res = GetApplication()->ExecDialog( pFaceDlg );
   else
    {
      GetApplication()->Error( pFaceDlg ? pFaceDlg->Status:ER_CREATEOBJECT );
      if( pFaceDlg ) pFaceDlg->CloseWindow();//delete pFaceDlg;
      CheckToolbar( TMyMDIChild::GetMode_() );
      return;
    }

   // FT_Cylinder, FT_Cone, FT_Sphere, FT_Torus,
   //FT_Tube
   if( res != IDOK )
    {
      CheckToolbar( TMyMDIChild::GetMode_() );
      return;
    }

   PTFaceData pFData = ((RTFaceItem)(listBodysBmp.firstThat(ItemFind,
     (Pvoid)ftSelFace))).pFData;
   pFData->dAngleSmothGrad = pMDICh->GetScena()->AngleSmoth();

   switch( ftSelFace )
    {
      case FT_Cylinder:  case FT_Cone:
	pMDICh->CreateCylinder( pFData );
	break;

      case FT_Sphere:
	pMDICh->CreateSphere( pFData );
	break;

      case FT_Torus:
	pMDICh->CreateTorus( pFData );
	break;

      case FT_Tube:
	pMDICh->CreateTube( pFData );
	break;
    }
 }

void TMyMDIFrame::CMHideGeometry( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   pMDICh->ToggleGeometry();
   SetCheckedInMenu();
 }

void TMyMDIFrame::CMFullDetall( RTMessage  )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   pMDICh->ToggleFullDetal();
   SetCheckedInMenu();
 }

void TMyMDIFrame::CMSceneInfo( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( !pMDICh || PTWindow(pMDICh)->isA() != classtypeMDIChWindow ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   PTInfDialog  pInfoDlg = NULL;
   List liStr;
   pMDICh->FillInfoDlg( liStr );

   if( (pInfoDlg = new TInfDialog( this, (LPSTR)MAKEINTRESOURCE(DlgSceneInfo), liStr, pMRC )) && !pInfoDlg->Status )
     GetApplication()->ExecDialog( pInfoDlg );
   else
    {  
      GetApplication()->Error( pInfoDlg ? pInfoDlg->Status:ER_CREATEOBJECT );
      if( pInfoDlg ) pInfoDlg->CloseWindow(); //delete pInfoDlg;
    }
 }


void TMyMDIFrame::CMMoveBody( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   pMDICh->SelectAndOp( MC_MoveBody );
 }

void TMyMDIFrame::CMDeleteBody( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   if( pMDICh->GetScena()->SelectedCount(FilterAll) )
    {
      int res = MakeMsg( hInstRC, HWindow, IO_MSG_DELETEALL,
	      ID_TITLE_QUERY, MB_ICONQUESTION | MB_YESNO | MB_APPLMODAL,
	      NULL );

      if( res == IDYES )
       {
	 pMDICh->GetScena()->DeleteAllSelected();
	 pMDICh->RedrawVports();
       }
    }
   //&&&

   pMDICh->SelectAndOp( MC_DeleteBody );
 }

void TMyMDIFrame::CM3DScaleBody( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   pMDICh->SelectAndOp( MC_3DScaleBody );
 }

void TMyMDIFrame::CM2DScaleBody( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   pMDICh->SelectAndOp( MC_2DScaleBody );
 }


void TMyMDIFrame::CMRotateBody( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   pMDICh->SelectAndOp( MC_RotateBody );
 }

void TMyMDIFrame::CheckToolbarIndepend()
 {
   if( pSysTool )
    {
      pSysTool->SpecialCheck( UCM_TB_INVERTSELMODE, bFlInvertSelectMode );
      pSysTool->SpecialCheck( UCM_TB_CONECT, TMyMDIChild::IsTransformAsTransforming() );
      pSysTool->SpecialCheck( UCM_TB_GROUP, TMyMDIChild::IsTransformSelected()  );
      pSysTool->SpecialCheck( UCM_TB_USE_BASEUCS,
	(ActiveChild_ && ActiveChild_->isA() == classtypeMDIChWindow) ?
	  PTMyMDIChild(ActiveChild_)->GetScena()->IsUsedBaseUSC():
          TRUE );
    } 
 }

void TMyMDIFrame::CheckToolbar( ModeChild mcMode )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE || !pSysTool ) return;
   int btn = 0;
  

   int *pTypeInfo, *pKeyTypeInfo;
   if( !(pKeyTypeInfo = pTypeInfo = TMyMDIFrame::FindTypeMode(mcMode)) )
    {pSysTool->Reset(); return; }

   if( pTypeInfo[2] > 0 ) btn = pTypeInfo[2];
   else if( pTypeInfo[2] == -1 ) {pSysTool->Reset(); return;}
   else if( pTypeInfo[2] == -2 )
    {      
      if( (pTypeInfo = TMyMDIFrame::FindTypeMode(pMDICh->GetNextMode())) )
       {
	 if( pTypeInfo[2] > 0 ) btn = pTypeInfo[2];
	 else
	  {
	    if( !(pTypeInfo = TMyMDIFrame::FindBtn(mcMode)) )
             {
	       pSysTool->Reset();
	       return;
             }
            else btn = pTypeInfo[0];
          }
	 //FindBtn
       }
      else {pSysTool->Reset(); return; }
    }
   else if( pTypeInfo[2] == -3 )
    {
      if( !(pTypeInfo = TMyMDIFrame::FindTypeMode2(pMDICh->GetButtonId())) )
       {
	 pSysTool->Reset();
	 return;
       }
      else btn = pTypeInfo[0];
    }

   if( pTypeInfo[1] == TS_Selectable )  pSysTool->Check( btn );
   else pSysTool->Reset();
 }

void TMyMDIFrame::CMToggleVTool( RTMessage )
 {
   HWND   hwndCh = ActiveChild_ ? ActiveChild_->HWindow:NULL;

   if( !pSysTool ) return;
   pSysTool->SetVisible( (pSysTool->IsVisible() == TRUE) ? FALSE:TRUE );

   HMENU hMain;
   HMENU  hOpt = GetSubMenu( (hMain = GetMenu(GetApplication()->MainWindow->HWindow)), 7 + MenuPosIncr(hwndCh) ),
	  hLs  = GetSubMenu( hOpt, 3 + MenuPosIncr(hwndCh) );


   CheckMenuItem( hLs, UCM_TOGGLE_VPORTTOOL, MF_BYCOMMAND |
     ((pSysTool->IsVisible() == TRUE) ? MF_CHECKED:MF_UNCHECKED)
		);
 }


void TMyMDIFrame::CMToggleStatus( RTMessage )
 {

 }


void TMyMDIFrame::CMRender( RTMessage )//Polyhonal
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE || !pMDICh->GetScena() ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }


   if( !pMDICh->GetScena() ) return;

   PTPRenderDialog pPrDlg;
   int res;
   TPRenderDialogTransfer prdtTr;
   prdtTr.rgbAdges =          *pMDICh->GetScena()->Adges();
   prdtTr.rgbIntersect =      *pMDICh->GetScena()->Intersect();
   prdtTr.polyMethod =        pMDICh->GetScena()->Method();
   prdtTr.bFlMarkIntersect =  pMDICh->GetScena()->MarkInt();


   if( (pPrDlg = new TPRenderDialog( this, (LPSTR)MAKEINTRESOURCE(DlgPolyhonalRender), prdtTr, pMRC )) && !pPrDlg->Status )
     res = GetApplication()->ExecDialog( pPrDlg );
   else
    {
      GetApplication()->Error( pPrDlg ? pPrDlg->Status:ER_CREATEOBJECT );
      if( pPrDlg ) pPrDlg->CloseWindow();
      return;
    }

   if( res != IDOK ) return;

   *pMDICh->GetScena()->Adges() = prdtTr.rgbAdges;
   *pMDICh->GetScena()->Intersect() = prdtTr.rgbIntersect;
   pMDICh->GetScena()->Method( (TPolyhonalMethod)prdtTr.polyMethod );
   pMDICh->GetScena()->MarkInt( prdtTr.bFlMarkIntersect );

   pMDICh->RenderView();
 }

void TMyMDIFrame::CMRenderFull( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE || !pMDICh->GetScena() ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }


   if( !pMDICh->GetScena() ) return;

   RTScena rSc = *pMDICh->GetScena();
   PTPhotoDlg pDlg;
   TPhotoSetupTransfer pstTransf;

   rSc.GetPhotoVariables( pstTransf );

   int res;
   if( (pDlg = new TPhotoDlg( this, (LPSTR)MAKEINTRESOURCE(DlgPhoto), pstTransf, pMRC )) && !pDlg->Status )
     res = GetApplication()->ExecDialog( pDlg );
   else
    {
      GetApplication()->Error( pDlg ? pDlg->Status:ER_CREATEOBJECT );
      if( pDlg ) pDlg->CloseWindow();
      return;
    }

   if( res != IDOK ) return;

   rSc.SetPhotoVariables( pstTransf );

   pMDICh->RenderPhoto();
 }

void TMyMDIFrame::CMImageOptions( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE || !pMDICh->GetScena() ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }


   if( !pMDICh->GetScena() ) return;

   PTImageOptDialog pDlg;
   int iPalSize = pMDICh->GetScena()->PaletteSize();
   TImageFormat ifFormat = pMDICh->GetScena()->ImageFormat();
   TConvertAlho alho = pMDICh->GetScena()->Alho();

   TNoisy nNoisy = pMDICh->GetScena()->Noisy();
   int iMarginalAmplitude = pMDICh->GetScena()->MarginalAmplitude();
   int iUniformAmplitude =  pMDICh->GetScena()->UniformAmplitude();
   int iUniformThreshold =  pMDICh->GetScena()->UniformThreshold();

   int res; 


   if( (pDlg = new TImageOptDialog( this, (LPSTR)MAKEINTRESOURCE(DlgImageOpt),
     iPalSize, ifFormat, alho,
     nNoisy, iMarginalAmplitude, iUniformAmplitude, iUniformThreshold,
     pMRC )) && !pDlg->Status )
     res = GetApplication()->ExecDialog( pDlg );
   else
    {
      GetApplication()->Error( pDlg ? pDlg->Status:ER_CREATEOBJECT );
      if( pDlg ) pDlg->CloseWindow();
      return;
    }

   if( res != IDOK ) return;

   pMDICh->GetScena()->PaletteSize( iPalSize );
   pMDICh->GetScena()->ImageFormat( ifFormat );
   pMDICh->GetScena()->Alho( alho );

   pMDICh->GetScena()->Noisy( nNoisy );
   pMDICh->GetScena()->MarginalAmplitude( iMarginalAmplitude );
   pMDICh->GetScena()->UniformAmplitude( iUniformAmplitude );
   pMDICh->GetScena()->UniformThreshold( iUniformThreshold );

 }

void TMyMDIFrame::CMBkgndSetup( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE || !pMDICh->GetScena() ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }


   if( !pMDICh->GetScena() ) return;
                    
   PTBkGndDialog pDlg;
   TAutoMemoryRc<PTBackGraund> mrcBackGraund;
   if( pMDICh->GetScena()->BkGnd() )
     mrcBackGraund = CreateCopy( *pMDICh->GetScena()->BkGnd() );

   int res;

   if( (pDlg = new TBkGndDialog( this, (LPSTR)MAKEINTRESOURCE(DlgBkGnd), mrcBackGraund, pMRC )) && !pDlg->Status )
     res = GetApplication()->ExecDialog( pDlg );
   else
    {
      GetApplication()->Error( pDlg ? pDlg->Status:ER_CREATEOBJECT );
      if( pDlg ) pDlg->CloseWindow();
      return;
    }

   if( res == IDOK )
    { 
      pMDICh->GetScena()->BkGnd( mrcBackGraund() );
      mrcBackGraund.ClearRc();
    }
 }


void TMyMDIFrame::CMPLaceHilite( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE || !pMDICh->GetScena() ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   if( !pMDICh->GetScena() ) return;

   pMDICh->GetPointAndOp( MC_PlaceHilite );
 }

void TMyMDIFrame::CMOmniAdjust( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE || !pMDICh->GetScena() ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   if( !pMDICh->GetScena() ) return;

   pMDICh->SelectAndOp( MC_OmniAdjust );
 }

void TMyMDIFrame::CMCreateOmni( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE || !pMDICh->GetScena() ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   if( !pMDICh->GetScena() ) return;

   pMDICh->GetPointAndOp( MC_CreateOmni );
 }

void TMyMDIFrame::CMAmbient( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE || !pMDICh->GetScena() ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

//******************
   if( !pMDICh->GetScena() ) return;

   PTAmbientDialog pAbbDlg;
   int res;
   RGBQUAD rgbModify = *pMDICh->GetScena()->Ambient();
   double dAmbMult = pMDICh->GetScena()->AmbientMult();

   if( (pAbbDlg = new TAmbientDialog( this, (LPSTR)MAKEINTRESOURCE(DlgAmbient), rgbModify, dAmbMult, pMRC )) && !pAbbDlg->Status )
     res = GetApplication()->ExecDialog( pAbbDlg );
   else
    {
      GetApplication()->Error( pAbbDlg ? pAbbDlg->Status:ER_CREATEOBJECT );
      if( pAbbDlg ) pAbbDlg->CloseWindow();
      return;
    }

   if( res != IDOK ) return;

   *pMDICh->GetScena()->Ambient() = rgbModify;
   pMDICh->GetScena()->AmbientMult( dAmbMult );
 }

void TMyMDIFrame::CMSelect( RTMessage Msg )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   pMDICh->SelectAndOp( MC_SelectBody );
 }

void TMyMDIFrame::CMSelectWindow( RTMessage Msg )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   pMDICh->SelectWithWindowFrame();
   //pMDICh->RedrawVports();
 }
  
void TMyMDIFrame::CMSelectAll( RTMessage Msg )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   if( pMDICh->ValidWindowOper( MC_GetWindow ) == FALSE ) return;

   if( pMDICh->GetScena() )
     if( pMDICh->GetScena()->SelectAll( bFlInvertSelectMode ) == TRUE )
       pMDICh->RedrawVports();
 }
  

void TMyMDIFrame::CMInvertSelMode( RTMessage Msg )
 {
   HWND   hwndCh = ActiveChild_ ? ActiveChild_->HWindow:NULL;
 
   CheckToolbar( TMyMDIChild::GetMode_() );

   bFlInvertSelectMode = (bFlInvertSelectMode == FALSE) ? TRUE:FALSE;

   HMENU hmMain = GetMenu( HWindow );   
   HMENU hm1 = GetSubMenu( hmMain, 1 + MenuPosIncr(hwndCh) );

   CheckMenuItem( hm1, UCM_INVERTSELMODE, MF_BYCOMMAND |
     ((bFlInvertSelectMode == TRUE ) ? MF_CHECKED:MF_UNCHECKED)
		);
   //if( pSysTool ) pSysTool->SpecialCheck( UCM_TB_INVERTSELMODE, bFlInvertSelectMode );
   CheckToolbarIndepend();
 }

void TMyMDIFrame::CMInvertSelecting( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   if( pMDICh->ValidWindowOper( MC_GetWindow ) == FALSE ) return;

   if( pMDICh->GetScena() )
     if( pMDICh->GetScena()->SelectInvert() == TRUE )
       pMDICh->RedrawVports();
 }

void TMyMDIFrame::CMCut( RTMessage Msg )
 {
   CheckToolbar( TMyMDIChild::GetMode_() );

   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   if( pMDICh->ValidWindowOper( MC_GetWindow ) == FALSE ) return;

   listClipboard.flush( TShouldDelete::Delete );

   Msg.Result = 256;
   CMCopy( Msg );

   if( !Msg.Result )
    {
      pMDICh->GetScena()->DeleteAllSelected();
      pMDICh->RedrawVports();
    }
 }

void TMyMDIFrame::CMCopy( RTMessage Msg )
 {
   CheckToolbar( TMyMDIChild::GetMode_() );
    
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE ) {Msg.Result = 0; return;}
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   //MessageBeep( -1 );

   if( pMDICh->ValidWindowOper( MC_GetWindow ) == FALSE )
    {
      if( pMDICh->GetDIB() )
       {
	 HGLOBAL hBmp = pMDICh->GetDIB()->GetDIBCopy();

	 if( !hBmp )
	  {
	    BWCCMessageBox( HWindow, "Not enought memory for create bitmap", "Error", MB_ICONSTOP | MB_OK | MB_APPLMODAL );
	    return;
	  }

	 OpenClipboard( HWindow );
	 EmptyClipboard();
	 SetClipboardData( CF_DIB, hBmp );
	 CloseClipboard();
       }

      return;
    }

   listClipboard.flush( TShouldDelete::Delete );
   if( !pMDICh->GetScena()->SelectedCount(FilterAll) )
    {
      MakeMsg( hInstRC, HWindow, IO_MSG_SELECTEDBODYSNOTFOUND,
	      ID_TITLE_MESSAGE, MB_ICONASTERISK | MB_OK | MB_APPLMODAL,
	      NULL );

      return;
    }

   TAutoStringRc s1_( hInstRC, ID_MSG_61, 50 ),
		 s2_( hInstRC, ID_MSG_62, 50 );
   PTIndicator  pInd = new TIndicator( this,
     (Msg.Result == 256) ? /*"Copy to clipboard & cut"*/s1_():s2_()/*"Copy to clipboard"*/,
     pMDICh->GetScena()->GetBytesSizeWithObol( TRUE, TRUE ), pMRC
				     );

   Msg.Result = 0;

   pInd->Show();
   PTScena pClipScena = new TScena( *pMDICh->GetScena(), pInd, TRUE, TRUE );
   pInd->WorkIs100();
   pInd->Close();

   if( pClipScena->Status == -1 )
     {
       delete pClipScena;

       MakeMsg( hInstRC, HWindow, IO_MSG_NOMEMFORCOPY,
	      ID_TITLE_ERROR, MB_ICONASTERISK | MB_OK | MB_APPLMODAL,
	      NULL );

       Msg.Result = -1;
       return;
     }
   if( pClipScena->Status == 1 ) { delete pClipScena; Msg.Result = 1; return; }


   listClipboard.add( *pClipScena ); 
 }

void TMyMDIFrame::CMPaste( RTMessage )
 {
   CheckToolbar( TMyMDIChild::GetMode_() );

   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   if( pMDICh->ValidWindowOper( MC_GetWindow ) == FALSE ) return;

   if( !listClipboard.getItemsInContainer() )
    {     

      MakeMsg( hInstRC, HWindow, IO_MSG_EMPTYCLIPBOARD,
	      ID_TITLE_MESSAGE, MB_ICONASTERISK | MB_OK | MB_APPLMODAL,
	      NULL );

      return;
    }

   DoubleListIterator dbIt = DoubleListIterator( listClipboard );
   PTScena pSc = (PTScena)&dbIt.current();

   pMDICh->GetScena()->Merge( *pSc );
   listClipboard.flush( TShouldDelete::Delete );

   pMDICh->RedrawVports();
 }


void TMyMDIFrame::CMZoomToFit( RTMessage Msg )
 {
   CMFitToZoom( Msg );
 }

void TMyMDIFrame::CMConnect( RTMessage )
 { 
   CheckToolbar( TMyMDIChild::GetMode_() );

   TMyMDIChild::SetTransformAsTransforming( (TMyMDIChild::IsTransformAsTransforming() == FALSE) ? TRUE:FALSE );

   //if( pSysTool ) pSysTool->SpecialCheck( UCM_TB_CONECT, TMyMDIChild::IsTransformAsTransforming() );
   CheckToolbarIndepend();
 }

void TMyMDIFrame::CMGroup( RTMessage )
 {
   CheckToolbar( TMyMDIChild::GetMode_() );

   TMyMDIChild::SetTransformSelected( (TMyMDIChild::IsTransformSelected() == FALSE) ? TRUE:FALSE );

   //if( pSysTool ) pSysTool->SpecialCheck( UCM_TB_GROUP, TMyMDIChild::IsTransformSelected() );
   CheckToolbarIndepend();
 }

typedef PTMyMDIChild _FAR *PPTMyMDIChild;

void MakeChildTopInstance( Pvoid pChild, Pvoid pParam )
 {
   if( PTWindow(pChild)->isA() != classtypeMDIChWindow ) return;
   if( PTMyMDIChild(pChild)->GetScId() != (*PPTMyMDIChild(pParam))->GetScId() ||
       PTMyMDIChild(pChild)->HWindow == (*PPTMyMDIChild(pParam))->HWindow ||
       PTMyMDIChild(pChild)->HWindow == GetFocus()
     ) return;

   SetWindowPos( PTWindow(pChild)->HWindow,
		 (*PPTMyMDIChild(pParam))->HWindow,
                 0, 0, 0, 0, 
		 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
		);
   *PPTMyMDIChild(pParam) = PTMyMDIChild(pChild);
 }

void TMyMDIFrame::MakeChildTop( RTMyMDIChild rChild )
 {
   PTMyMDIChild pPrevTop = &rChild;
   ForEach( MakeChildTopInstance, Pvoid(&pPrevTop) );
 }

void TMyMDIFrame::CMSetBPVisible( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( !pMDICh || PTWindow(pMDICh)->isA() != classtypeMDIChWindow ) return;

   pMDICh->SetVisibleBasePoint( (pMDICh->IsVisibleBasePoint() == TRUE) ? FALSE:TRUE );

   InvalidateRect( pMDICh->HWindow, NULL, TRUE );
   SetCheckedInMenu();
 }

void TMyMDIFrame::CMUseGlobalBP( RTMessage )
 {
   //PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   //if( !pMDICh || PTWindow(pMDICh)->isA() != classtypeMDIChWindow ) return;

   TMyMDIChild::SetUsedPoint( (TMyMDIChild::UsedGlobalPoint() == FALSE) ? TRUE:FALSE );

   SetCheckedInMenu();
 }

void TMyMDIFrame::CMSetupGlobalBP( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }


   pMDICh->GetPointAndOp( MC_SetupGlobalBP );
 }

void TMyMDIFrame::CMUseBaseUCS( RTMessage )//&&&&&&&&&&
 {
   CheckToolbar( TMyMDIChild::GetMode_() );

   if( !ActiveChild_ || PTWindow(ActiveChild_)->isA() != classtypeMDIChWindow ) return;

   PTMyMDIChild(ActiveChild_)->GetScena()->SetUsedBaseUCS(
     (PTMyMDIChild(ActiveChild_)->GetScena()->IsUsedBaseUSC() == TRUE ) ? FALSE:TRUE );

   HMENU hmMain = GetMenu( HWindow );
   HMENU hm1 = GetSubMenu( hmMain, 1 + MenuPosIncr(ActiveChild_->HWindow) );

   //int c = PTMyMDIChild(ActiveChild_)->GetScena()->IsUsedBaseUSC();
   CheckMenuItem( hm1, UCM_USE_BASEUCS , MF_BYCOMMAND |
     ((PTMyMDIChild(ActiveChild_)->GetScena()->IsUsedBaseUSC() == TRUE ) ? MF_CHECKED:MF_UNCHECKED)
		);

   CheckToolbarIndepend();
   //if( pSysTool ) pSysTool->SpecialCheck( UCM_TB_USE_BASEUCS,
     //PTMyMDIChild(ActiveChild_)->GetScena()->IsUsedBaseUSC() );
 }

void TMyMDIFrame::CMAligmentVertex( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }


   pMDICh->GetPointAndOp( MC_AligmentVertex );
 }

void TMyMDIFrame::CMAligmentFacet( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   pMDICh->GetFacetAndOp( MC_AligmentFacet );
 }

void TMyMDIFrame::CMUseCrossingFrame( RTMessage )
 {
   TMyMDIChild::SetUsedCrossing( (TMyMDIChild::UsedCrossing() == FALSE) ? TRUE:FALSE );

   SetCheckedInMenu();
 }

void TMyMDIFrame::CMLogging( RTMessage )
 {
   bFlLogging = (bFlLogging == FALSE) ? TRUE:FALSE;

   HWND   hwndCh;
   if( ActiveChild_ && ActiveChild_->isA() == classtypeMDIChWindow )
     hwndCh = ActiveChild_->HWindow;
   else hwndCh = NULL;

   int incr = MenuPosIncr(hwndCh);

   HMENU hF  = GetSubMenu( GetMenu(GetApplication()->MainWindow->HWindow),
     0 + incr );

   CheckMenuItem( hF, UCM_LOGGING, MF_BYCOMMAND |
     ((bFlLogging == TRUE) ? MF_CHECKED:MF_UNCHECKED)
		);
 }


void TMyMDIFrame::CMCopyBmpClip( RTMessage )
 {
/*   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   if( pMDICh->GetDIB() )
    {
      HGLOBAL hBmp = pMDICh->GetDIB()->GetDIBCopy();

      if( !hBmp )
       {
	 BWCCMessageBox( HWindow, "Not enought memory for create bitmap", "Error", MB_ICONSTOP | MB_OK | MB_APPLMODAL );
         return;
       }

      OpenClipboard( HWindow );
      EmptyClipboard();
      SetClipboardData( CF_DIB, hBmp );
      CloseClipboard();
    }*/
 }
