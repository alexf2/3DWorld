//#pragma option -v-

#include "toolbar.hpp"
#include "INC\errors.inc"
#include "INC\menu.inc"
#include "bmp.hpp"

#include <list.h>
#include <windowsx.h>
#include <string.h>


#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;


void THintWindow::GetWindowClass( WNDCLASS _FAR & AWndClass )
 {
   TWindow::GetWindowClass( AWndClass );

   AWndClass.style = CS_SAVEBITS;
   AWndClass.hbrBackground = CreateSolidBrush( RGB(0xFF, 0xFF, 100) );
 }

LPSTR THintWindow::GetClassName()
 {
   return "3DW:HINT_WINDOW";
 }

void THintWindow::DefWndProc( RTMessage Msg )
 {
   PTApplication pApp;

   switch( Msg.Message )
    {
      case WM_KEYDOWN:
      case WM_KEYUP:
      case WM_CHAR:
      case WM_SYSKEYDOWN:
      case WM_SYSKEYUP:
      case WM_SYSCHAR:
      case WM_DEADCHAR:
      case WM_SYSDEADCHAR:

	if( Parent && Parent->isA() == classtypeToolbarButton &&
	    Parent->Parent && Parent->Parent->isA() == classtypeToolbarWindow
	  )
	  PTToolBarWindow(Parent->Parent)->SetLock( TRUE );
	if( (pApp = GetApplication()) )
	  PostMessage( pApp->MainWindow->HWindow, Msg.Message, Msg.WParam, Msg.LParam );
        Msg.Result = 0;
	break;

      default:
        TWindow::DefWndProc( Msg );
    }
 }

void THintWindow::SetupWindow()
 {
   TWindow::SetupWindow();
   if( Parent && Parent->Parent && Parent->Parent->isA() == classtypeToolbarWindow )
     PTToolBarWindow(Parent->Parent)->SetHandle( HWindow );

   //SetWindowPos( HWindow, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW );
   //hwnd, hwndInsertAfter, x, y, cx, cy, fuFlags)
 }

const int THintWindow::iLenBuf = 128;

THintWindow::THintWindow( PTWindowsObject AParent, UINT idHint, HFONT hf, PTModule AModule ):
  TWindow( AParent, NULL, AModule )
 {

   this->hf = hf;
   Attr.Style = WS_DISABLED | WS_POPUP;

   if( !(lpHint = new char[iLenBuf + 1]) ) return;

   LoadString( hInstRC, idHint, lpHint, iLenBuf );

   RECT r;
   GetWindowRect( AParent->HWindow, &r );
   int wid = r.right - r.left,
       high = r.bottom - r.top;

   HDC hdc = GetDC( Parent->HWindow );
   HFONT hfOld = SelectFont( hdc, hf );//SelectObject
   DWORD dwExt = GetTextExtent( hdc, lpHint, _fstrlen(lpHint) );
   SelectFont( hdc, hfOld );
   ReleaseDC( Parent->HWindow, hdc );

   Attr.W = GetSystemMetrics( SM_CXBORDER ) * 4 +
	    LOWORD(dwExt);
   Attr.H = GetSystemMetrics( SM_CYBORDER ) * 4 +
	    HIWORD(dwExt);

   POINT ptCurs; GetCursorPos( &ptCurs );
   if( wid < Attr.W ) Attr.X = ptCurs.x;
   else Attr.X = r.left + (wid - Attr.W) / 2;
   /*if( high < Attr.H ) Attr.Y = ptCurs.y + GetSystemMetrics( SM_CYCURSOR );
   else Attr.Y = r.top + (high - Attr.H) / 2;*/
   Attr.Y = ptCurs.y + GetSystemMetrics( SM_CYCURSOR ) / 2;
   //Attr.Y = GetSystemMetrics( SM_CXCURSOR );
   //Attr.Y = GetSystemMetrics( SM_CYCURSOR );

   int ws = GetSystemMetrics( SM_CXSCREEN ),
       hs = GetSystemMetrics( SM_CYSCREEN );
   if( Attr.X + Attr.W > ws ) Attr.X -= (Attr.X + Attr.W - ws);
   if( Attr.Y + Attr.H > hs ) Attr.Y -= (Attr.Y + Attr.H - hs);
   if( Attr.X <= 0 ) Attr.X = 1;
   if( Attr.Y <= 0 ) Attr.Y = 1;

   /*POINT ptTmp; ptTmp.x = Attr.X; ptTmp.y = Attr.Y;
   ScreenToClient( AParent->HWindow, &ptTmp );
   Attr.X = ptTmp.x; Attr.Y = ptTmp.y;*/
 }


void THintWindow::Paint( HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo )
 {
   TWindow::Paint( PaintDC, PaintInfo );

   HFONT hfOld = SelectFont( PaintDC, hf );
   int len;
   DWORD dwExt = GetTextExtent( PaintDC, lpHint, (len = _fstrlen(lpHint)) );
   RECT r;
   GetWindowRect( HWindow, &r );
   r.right -= r.left; r.right++;  r.bottom -= r.top; r.bottom++;
   r.left = r.top = 0;

   COLORREF cOld = SetTextColor( PaintDC, RGB(0, 0, 0) );
   int mOld = SetBkMode( PaintDC, TRANSPARENT );
   HPEN hpOld = SelectPen( PaintDC, (HPEN)GetStockObject(BLACK_PEN) );
   HBRUSH hbrOld = SelectBrush( PaintDC, (HBRUSH)GetStockObject(NULL_BRUSH) );

   DrawText( PaintDC, lpHint, len, &r, DT_CENTER | DT_VCENTER | DT_NOCLIP | DT_SINGLELINE);
   Rectangle( PaintDC, 0, 0, r.right - 1, r.bottom - 1 );

   SelectBrush( PaintDC, hbrOld );
   SelectPen( PaintDC, hpOld );
   SetBkMode( PaintDC, mOld );
   SetTextColor( PaintDC, cOld );
   SelectFont( PaintDC, hfOld );
 }

THintWindow::~THintWindow()
 {
   if( lpHint ) { delete []lpHint; lpHint = NULL; }
 }


MemBlocks* TButtonInit::mgr = NULL;
long TButtonInit::countMemMgr = 0;

void _FAR* TButtonInit::operator new( size_t size )
 {
   if( countMemMgr++ == 0 )
     TButtonInit::mgr = new MemBlocks( sizeof(TButtonInit), 50 );

   return TButtonInit::mgr->allocate( size );
 }

void TButtonInit::operator delete( void _FAR* pToDel )
 {   
   TButtonInit::mgr->free( pToDel );

   if( --countMemMgr == 0 )
    {
      delete TButtonInit::mgr;
      TButtonInit::mgr = NULL;
    }
 }



int TButtonInit::isEqual( const Object& rObj ) const
 {
   if( (TButtonInit huge *)this != (TButtonInit huge *)(&rObj) ) return FALSE;
   return TRUE;
 }

TButtonInit::TButtonInit( int resIdNorm, int resIdPress, int iCommand, HINSTANCE hInst,
  TStrategy tsStrat, UINT idOfHint )
 {
   this->hInst = hInst;
   this->iCommand = iCommand;
   hbmN = hbmH = NULL;
		      
   hbmN = LoadBitmap( hInst, (LPCSTR)MAKEINTRESOURCE(resIdNorm) );
   hbmH = LoadBitmap( hInst, (LPCSTR)MAKEINTRESOURCE(resIdPress) );

   if( !hbmN || !hbmH )    
     Status = ER_BITMAPLOAD;
   else Status = 0;

   //isSelectable = bFlCheck;
   tsStrategy = tsStrat;
   isChecked = FALSE;
   bFlRedraw = FALSE;

   this->idOfHint = idOfHint;

 }

void TButtonInit::Check( BOOL fl )
 {
   if( tsStrategy != TS_Default )
   // if( tsStrategy == TS_Selectable )
     isChecked = fl;
 }

TButtonInit::~TButtonInit()
 {
   if( hbmN ) DeleteObject( hbmN ); hbmN = NULL;
   if( hbmH ) DeleteObject( hbmH ); hbmH = NULL;
 }

void TToolBarWindow::UpdateState()
 {
   ShowWindow( HWindow, (isVisible == TRUE) ? SW_SHOW:SW_HIDE );
 }

const unsigned long TToolBarWindow::clockForHintDelay = 20;

TToolBarWindow::TToolBarWindow( PTWindowsObject AParent, LPSTR ATitle, DoubleList *plistBmp,
  int str, int clm, HFONT hf,
  PTModule AModule ):
  TBWindow( AParent, ATitle, AModule )//TDialog
  {

    hHandleCurHint = NULL;
    //ulClockForHint = 0;
    bIsLocked = FALSE;
    this->hf = hf;
    idTimer = 0;
    StopAndClearWaitCount();
    isVisible = TRUE;
    this->plistBmp = plistBmp;
                                 //DoubleList
    GetObject( ((RTButtonInit)plistBmp->peekAtHead()).GetHandleN(),
      sizeof(BITMAP), (LPSTR)&bmpInf );
    iCol = clm; iStr = str;

    int iHSpc = 2, iVSpc = 2,
       iWidt = bmpInf.bmWidth, iHigh = bmpInf.bmHeight,
       iStaX = 1, iStaY = 1;

    Attr.Style = WS_POPUP | WS_CAPTION | WS_SYSMENU;
    
    Attr.W = iCol * bmpInf.bmWidth +
      GetSystemMetrics(SM_CXBORDER) * 2 + (iCol-1) * iHSpc + iStaX + 1;
    Attr.H = iStr * bmpInf.bmHeight + iStr * 0 +
      GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYBORDER) * 2 +
      (iStr-1) * iVSpc + iStaY + 1;
    Attr.X = Attr.Y = 0;

    SetFlags( WB_MDICHILD, FALSE );

   DoubleListIterator liIter = DoubleListIterator( *plistBmp, FALSE );
   PTButtonInit pbiInit = (PTButtonInit)&liIter.current();

   if( *pbiInit == NOOBJECT ) { Status = ER_CREATEOBJECT; return; }


   RECT rect;
   rect.left = iStaX; rect.right = rect.left + iWidt;
   rect.top = iStaY; rect.bottom = rect.top + iHigh;

   int idBtn = SYSTOOL_FIRST_COMMAND + 1;
   int count = 0;
   for( int i = 0; i < iStr && int(liIter); i++ )
    {
      rect.left = iStaX; rect.right = rect.left + iWidt;
      for( int j = 0; j < iCol && int(liIter); j++, count++,
	pbiInit = (PTButtonInit)&--liIter, idBtn++ )
       { 
	 PTToolButton pBtn;

	 if( *pbiInit == NOOBJECT ) return;

	 pBtn = new TToolButton( this, idBtn, NULL, rect.left, rect.top,
	 rect.right - rect.left, rect.bottom - rect.top, FALSE, pbiInit->GetHintId(),
	   GetModule() );
								 //BBS_PARENTNOTIFY | WS_GROUP;
	 pBtn->Attr.Style = BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_GROUP |
	   BBS_PARENTNOTIFY | BBS_OWNERDRAW;
	 //BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
	 rect.left += iWidt + iHSpc; rect.right = rect.left + iWidt;

       }
      rect.top += iHigh + iVSpc; rect.bottom =  rect.top + iHigh;
    }

  }

void TToolBarWindow::WMCreate( RTMessage msg )
 {
   TBWindow::WMCreate( msg );
   
 }

void TToolBarWindow::WMKillFocus( RTMessage msg )//WM_KILLFOCUS
 {
   if( HWND(msg.WParam) == hHandleCurHint )
    {
      msg.Result = 0;
      //SetActiveWindow( HWindow );
      //SetFocus( HWindow );
    }
   else
     TBWindow::DefWndProc( msg );
 }

//Bm.bmWidth;
//PtSize.y = Bm.bmHeight;

/*LPSTR TToolBarWindow::GetClassName()
 {
   return "3DW:TOOLBAR";
 }*/

void TToolBarWindow::GetWindowClass(WNDCLASS _FAR & AWndClass)
 {
   TBWindow::GetWindowClass( AWndClass );

   AWndClass.style = CS_SAVEBITS | CS_NOCLOSE;
   AWndClass.hbrBackground = (HBRUSH)GetStockObject( DKGRAY_BRUSH );
 }

void TToolBarWindow::SetupWindow()
 {
   TBWindow::SetupWindow();


   HMENU hmenuSys;
   if( (hmenuSys = GetSystemMenu(HWindow, FALSE)) )
    {      
      DeleteMenu( hmenuSys, 0, MF_BYPOSITION );
      for( int i = 0; i < 7; i++ )
	DeleteMenu( hmenuSys, 1, MF_BYPOSITION );

      AppendMenu( hmenuSys, MF_BYCOMMAND | MF_ENABLED,
	UCM_SYSTOOLBAR_HIDE, "&Hide" );
    }
 }


static int ButtonInitItemsFind( const Object& rObj, Pvoid pPar )
 {
   return (((RTButtonInit)rObj).GetCommand() == (UINT)pPar );
 }


void TToolBarWindow::SpecialCheck( int iCom, BOOL bFlCh )
 {
   RTButtonInit rItem = (RTButtonInit)plistBmp->firstThat( ButtonInitItemsFind, Pvoid(iCom) );               
   if( rItem == NOOBJECT || rItem.GetStrategy() != TS_IndependSwt ) return;

   BOOL keyCheck = rItem.GetCheck();
   rItem.Check( bFlCh );

   if( rItem.GetCheck() != keyCheck )
    {
      rItem.bFlRedraw = TRUE;
      PTWindowsObject pBtn = ChildWithId( iCom );
      InvalidateRect( pBtn->HWindow, NULL, FALSE );
    }
 }

void TToolBarWindow::Check( int iCommand )
 {
   DoubleListIterator  dbIt = DoubleListIterator( *plistBmp );
   PTButtonInit pIni = (PTButtonInit)&dbIt.current();

   RTButtonInit rActionBtnIni = (RTButtonInit)plistBmp->firstThat( ButtonInitItemsFind, Pvoid(iCommand) );
   if( rActionBtnIni == NOOBJECT ) return;

   if( rActionBtnIni.GetStrategy() == TS_IndependSwt )
    {
      rActionBtnIni.Check( (rActionBtnIni.GetCheck() == FALSE) ? TRUE:FALSE );
      PTWindowsObject pBtn = ChildWithId( iCommand );
      InvalidateRect( pBtn->HWindow, NULL, FALSE );

      return;
    }

   for( ; int(dbIt); pIni = (PTButtonInit)&++dbIt )
    {
      BOOL keyCheck = pIni->GetCheck();

      if( pIni->GetStrategy() != TS_IndependSwt )
       {
	 if( pIni->GetCommand() == iCommand )
	   pIni->Check( TRUE );
	 else
	   pIni->Check( FALSE );
       }

      if( pIni->GetCheck() != keyCheck )
       {
	 pIni->bFlRedraw = TRUE;
	 PTWindowsObject pBtn = ChildWithId( pIni->GetCommand() );
	 InvalidateRect( pBtn->HWindow, NULL, FALSE );
       }
    }
 }

void TToolBarWindow::Reset()
 {
   DoubleListIterator  dbIt = DoubleListIterator( *plistBmp );
   PTButtonInit pIni = (PTButtonInit)&dbIt.current();

   for( ; int(dbIt); pIni = (PTButtonInit)&++dbIt )
    {
      BOOL keyCheck = pIni->GetCheck();
      if( pIni->GetStrategy() != TS_IndependSwt ) pIni->Check( FALSE );
      if( keyCheck != FALSE )
       {
	 pIni->bFlRedraw = TRUE;
	 PTWindowsObject pBtn = ChildWithId( pIni->GetCommand() );
	 InvalidateRect( pBtn->HWindow, NULL, FALSE );
       }
    }
 }

TStrategy TToolBarWindow::GetStrategyItem( int iCom )
//BOOL TToolBarWindow::IsGroupedItem( int iCom )
 {
   DoubleListIterator  dbIt = DoubleListIterator( *plistBmp );
   PTButtonInit pIni = (PTButtonInit)&dbIt.current();

   for( ; int(dbIt); pIni = (PTButtonInit)&++dbIt )
     if( pIni->GetCommand() == iCom ) return pIni->GetStrategy();

   return TS_Default;
 }


void TToolBarWindow::DefWndProc( RTMessage Msg )
 {
   if( Msg.Message == WM_SYSCOMMAND )
     switch( Msg.WParam & 0xFFF0 )
      {
	case UCM_SYSTOOLBAR_HIDE:
	  SendMessage( Parent->HWindow, WM_COMMAND, 0,
	    MAKELONG(0, UCM_TOGGLE_VPORTTOOL) );
	  Msg.Result = 0;
	  return;
      }

   TBWindow::DefWndProc( Msg );
 }


void TToolBarWindow::WMCommand( RTMessage msg )//WM_COMMAND TWindowsObject
 {
   HWND hwndCtl = (HWND)LOWORD(msg.LParam);
   PTToolButton pBtn;

   if( HIWORD(msg.LParam) == BN_CLICKED &&
     (pBtn = (PTToolButton)ChildWithId(msg.WParam)) )
    {      
      SetFocus( Parent->HWindow );

      PostMessage( Parent->HWindow, WM_COMMAND, msg.WParam,
	MAKELONG(HWindow, BN_CLICKED) );

      /*if( IsGroupedItem(msg.WParam) == TRUE )
	Check( msg.WParam );*/
      Check( msg.WParam );

      msg.Result = 0;
      return;
    }

   TBWindow::WMCommand( msg );
 }

void TToolBarWindow::WMDestroy( RTMessage msg )
 {
   if( Parent )
     SendMessage( Parent->HWindow, WM_COMMAND, Attr.Id,
       MAKELONG(0, UCM_SYSTOOLBAR_DESTROYED) );

   TBWindow::WMDestroy( msg );
 }


/*LPSTR TToolButton::GetClassName()
 {
      return "3DW:TOOLBAR_BUTTON";
 }*/

void TToolButton::GetWindowClass( WNDCLASS _FAR & AWndClass )
 {
   TBButton::GetWindowClass( AWndClass );

   //AWndClass.hbrBackground = CreateSolidBrush( RGB(0, 0, 0) );
   AWndClass.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
 }

void TToolButton::WMEraseBkGnd( RTMessage msg )
 {
   //TBButton::DefWndProc( msg );
   msg.Result = 0;
 }



void TToolBarWindow::DrawItem( DRAWITEMSTRUCT _FAR & DrawInfo )
 {
   //ListIterator dlIter = ListIterator( *plistBmp );TWindowsObject
   RTButtonInit rItem = (RTButtonInit)plistBmp->firstThat( ButtonInitItemsFind, (Pvoid)DrawInfo.CtlID );
               
   if( rItem == NOOBJECT ) { TBWindow::DrawItem( DrawInfo ); return; }

  PTToolButton pTB = (PTToolButton)ChildWithId( DrawInfo.CtlID );

  /* if( (DrawInfo.itemAction & ODA_FOCUS) == ODA_FOCUS )
     pTB->flFocus = (pTB->flFocus == TRUE) ? FALSE:TRUE;
      */
   if( (DrawInfo.itemState & (ODS_FOCUS | ODS_SELECTED)) ==
     (ODS_FOCUS | ODS_SELECTED) || rItem.GetCheck() == TRUE )
     DrawBmp( DrawInfo.hDC, rItem.GetHandleH() );
   else
     DrawBmp( DrawInfo.hDC, rItem.GetHandleN() );
 }

void TToolButton::WMMouseMove( RTMessage Msg )
//void TToolButton::WMNCHittest( RTMessage Msg )
 {
   DefWndProc( Msg );

   if( Parent )
    {
      PTToolBarWindow(Parent)->WMNCHittest();
      PTToolBarWindow(Parent)->StartWaitCount();
    }
 }

void TToolButton::WMLButtonDown( RTMessage Msg )
 {
   TBButton::WMLButtonDown( Msg );

   if( Parent && Msg.Message == WM_LBUTTONDOWN )
     PTToolBarWindow(Parent)->SetLock( TRUE );
 }


void TToolButton::WMLButtonUp( RTMessage Msg )
 {
   TBButton::DefWndProc( Msg );


   if( Parent )
    {
      PTToolBarWindow(Parent)->StopAndClearWaitCount();
      PTToolBarWindow(Parent)->StartWaitCount();
      PTToolBarWindow(Parent)->SetLock( FALSE );
    }

   /*if( Parent && Parent->isA() == classtypeToolbarWindow )
     PTToolBarWindow(Parent)->Check( GetId() );*/
 }


void TToolButton::WMTimer()
 {
   POINT poCurs;
   GetCursorPos( &poCurs );
   BOOL tst = CursorInWindow(HWindow, poCurs);

   if( tst == FALSE && pHintWnd ) { KillHint(); return; }
   //ScreenToClient( Parent->HWindow, &poCurs );

   if( Parent && PTToolBarWindow(Parent)->QueryHint() == TRUE )
    {
      if( tst == TRUE )
       {
	 if( !pHintWnd ) MakeHint();
       }
      else if( pHintWnd ) KillHint();
    }
 }

void TToolButton::KillHint()
 {
   if( pHintWnd )
   {
     pHintWnd->CloseWindow(); pHintWnd = NULL;
   }
 }

static void KillHintInstance( Pvoid pCh, Pvoid pParam )
 {
   if( PTWindowsObject(pCh)->isA() != classtypeToolbarButton ) return;

   PTToolButton(pCh)->KillHint();
 }


void TToolButton::MakeHint()
 {
    if( Parent )
     {
       Parent->ForEach( KillHintInstance, NULL );

       POINT p;
       GetCursorPos( &p );
       LRESULT res = SendMessage( HWindow, WM_NCHITTEST, 0, MAKELONG(p.x, p.y) );

       if( res != HTCLIENT && res != HTBORDER ) return;
        
       pHintWnd = new THintWindow( this, idOfHint, PTToolBarWindow(Parent)->GetFont(),
	 GetModule() );
     }

    if( pHintWnd )
     {
       GetApplication()->MakeWindow( pHintWnd );
       ShowWindow( pHintWnd->HWindow, SW_SHOWNOACTIVATE );
       //UpdateWindow( pHintWnd->HWindow );
     }
 }



void  TToolBarWindow::SetLock( BOOL bL )
 {
   bIsLocked = bL;
   if( bL == TRUE ) ForEach( KillHintInstance, NULL );
 }

void TToolBarWindow::WMNCHittest()
 {
   //DefWndProc( Msg );

   if( !idTimer )
    {
      //SetLock( FALSE );
      bIsLocked = FALSE;
      StopAndClearWaitCount();
      if( SetTimer( HWindow, 1, 55, NULL ) ) idTimer = 1;
    }

 }

void TToolBarWindow::StartWaitCount()
 {
   if( ulClockForHint <= clockForHintDelay ) ulClockForHint = 1;
 }

BOOL TToolBarWindow::QueryHint() const
 {
   BOOL bFlCapt = TRUE;
   HWND hwnd;

   if( (hwnd = GetCapture()) && hwnd != HWindow && IsChild(HWindow, hwnd) == FALSE )
     bFlCapt = FALSE;

   return BOOL(ulClockForHint > clockForHintDelay && GetLock() == FALSE &&
     IsWindowEnabled(HWindow) == TRUE && bFlCapt );
 }

BOOL CursorInWindow( HWND hwnd, POINT& poCurs )
 {
   RECT r;
   GetWindowRect( hwnd, &r );

   return BOOL(poCurs.x > r.left && poCurs.x < r.right &&
	    poCurs.y > r.top  && poCurs.y < r.bottom);
 }

//typedef void (TActionFunc)(Pvoid Child, PVoid ParamList);

static void CallWMTInstance( Pvoid pCh, Pvoid pParam )
 {
   if( PTWindowsObject(pCh)->isA() != classtypeToolbarButton ) return;

   PTToolButton(pCh)->WMTimer();
 }

void TToolBarWindow::WMTimer( RTMessage Msg )
 {
   ForEach( CallWMTInstance, NULL );
   //void ForEach(TActionFunc Action, Pvoid PParamList)

   Msg.Result = 0;

   POINT poCurs;
   GetCursorPos( &poCurs );

   if( CursorInWindow(HWindow, poCurs) == FALSE )
    {
      if( idTimer )
       {
	 idTimer = 0; KillTimer( HWindow, 1 );
	 ForEach( CallWMTInstance, NULL );
       }
      return;
    }

   if( QueryHint() == FALSE && ulClockForHint ) ulClockForHint++;
 }

void TToolButton::WMSetFocus( RTMessage msg )
 {
   flFocus = TRUE;
   TBButton::DefWndProc( msg );
 }

void TToolButton::WMKillFocus( RTMessage msg )
 {
   flFocus = FALSE;
   TBButton::DefWndProc( msg );
 }


#pragma option -v.
