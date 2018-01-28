#include <windowsx.h>
#include "fullscr.hpp"
#include "INC\mdicurs.inc"
//#include <stdio.h>

extern HINSTANCE hInstRC;
extern PTModule pMRC;


int TSysPaletteSave::iIndexPal[ iNumbEnt ] =
 {
   COLOR_ACTIVEBORDER,
   COLOR_ACTIVECAPTION,
   COLOR_APPWORKSPACE,
   COLOR_BACKGROUND,
   COLOR_BTNFACE,
   COLOR_BTNHIGHLIGHT,
   COLOR_BTNSHADOW,
   COLOR_BTNTEXT,
   COLOR_CAPTIONTEXT,
   COLOR_GRAYTEXT,
   COLOR_HIGHLIGHT,
   COLOR_HIGHLIGHTTEXT,
   COLOR_INACTIVEBORDER,
   COLOR_INACTIVECAPTION,
   COLOR_INACTIVECAPTIONTEXT,
   COLOR_MENU,
   COLOR_MENUTEXT,
   COLOR_SCROLLBAR,
   COLOR_WINDOW,
   COLOR_WINDOWFRAME,
   COLOR_WINDOWTEXT
 };

COLORREF TSysPaletteSave::rgb[ iNumbEnt ];


TSysPaletteSave::~TSysPaletteSave()
 {
   ~*this;
 }

void TSysPaletteSave::operator()( HDC hdc_ )
 {
   Reset();   

   iNumColors = GetDeviceCaps( hdc, NUMCOLORS );
   if( !hdc_ || !iNumColors || iNumColors > 20 ) return;
   hdc = hdc_;

   for( int i = 0; i < iNumbEnt; i++ )
     TSysPaletteSave::rgb[ i ] = GetSysColor( TSysPaletteSave::iIndexPal[i] );
 }

void TSysPaletteSave::operator~()
 {
   if( hdc )
     SetSysColors( iNumbEnt , TSysPaletteSave::iIndexPal,
		   TSysPaletteSave::rgb );
   else return;

   Reset();
 }

void TSysPaletteSave::Reset()
 {
   hdc = NULL;
 }

int TFullScrWindow::iMouseX, TFullScrWindow::iMouseY;

TFullScrWindow::TFullScrWindow( PTWindowsObject AParent, LPSTR ATitle,
  PTDIB  pbmpDIB_, HPALETTE hpalAlias_, PTModule AModule ):
  TWindow( AParent, ATitle, AModule )
 {
   bFlOldDisable = FALSE;
   hpalAlias = hpalAlias_;
   Attr.Style = WS_BORDER | WS_MAXIMIZE | WS_VISIBLE | WS_POPUP;
   pbmpDIB = pbmpDIB_;
   iBmpWidth = pbmpDIB->GetWidth();
   iBmpHigh = pbmpDIB->GetHigh();
   hcHand = NULL;
   hdc = NULL;

   iStaOrgX = iStaOrgY = 0;
   bFlTrack = FALSE;

   _fsetmem( &r, sizeof(r), 0 );
 }

void TFullScrWindow::Destroy()
 {
   PTApplication pApp = GetApplication();
   if( pApp && pApp->MainWindow )
     EnableWindow( pApp->MainWindow->HWindow, bFlOldDisable ? FALSE:TRUE );

   SetSystemPaletteUse( hdc, SYSPAL_STATIC );
   ~spsSysPalette;

   TWindow::Destroy();

   if( Parent )
     PostMessage( Parent->HWindow, WM_USER_FULLSCRDONE, 0, 0 );
 }

TFullScrWindow::~TFullScrWindow()
 {
   if( hcHand ) {DestroyCursor( hcHand ); hcHand = NULL; }   
 }

LPSTR TFullScrWindow::GetClassName()
 {
   return "3DW:FULLSCRWINDOW";
 }

void TFullScrWindow::GetWindowClass( WNDCLASS & AWndClass )
 {
   TWindow::GetWindowClass( AWndClass );

   AWndClass.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
   AWndClass.style |= CS_SAVEBITS | CS_OWNDC;
 }

BOOL TFullScrWindow::Create()
 {
   if( !TWindow::Create() ) return FALSE;

      PTApplication pApp = GetApplication();
   if( pApp && pApp->MainWindow )
     bFlOldDisable = EnableWindow( pApp->MainWindow->HWindow, FALSE );

   hdc = GetDC( HWindow );
   spsSysPalette( hdc );
   SetSystemPaletteUse( hdc, SYSPAL_NOSTATIC );

   GetClientRect( HWindow, &r );
   iBmpViewWidth = r.right; iBmpViewHigh = r.bottom;
   hcHand = LoadCursor( hInstRC, MAKEINTRESOURCE(CSR_OT_Hand) );

   if( iBmpWidth < r.right ) iStaOrgX = (r.right - iBmpWidth) / 2;
   if( iBmpHigh < r.bottom ) iStaOrgY = (r.bottom - iBmpHigh) / 2;
   SetViewportOrg( hdc, iStaOrgX, iStaOrgY );

   InvalidateRect( HWindow, NULL, TRUE );

   return TRUE;
 }
   /*
void TFullScrWindow::SetupWindow()
 {
   TWindow::SetupWindow();

 }   */

void TFullScrWindow::WMEraseBkGnd( RTMessage msg )
 {
   //if( r.right == r.bottom == 0 ) GetClientRect( HWindow, &r );

   if( iBmpWidth >= r.right && iBmpHigh >= r.bottom )
    {
      msg.Result = 0;
      return;
    }

   TWindow::DefWndProc( msg );
 }

void TFullScrWindow::Paint( HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo )
 {
   if( pbmpDIB )
    {
      pbmpDIB->DrawBitmapDIBWithPalette( PaintDC, hpalAlias, 0, 0 );
      return;
    }
 }

void TFullScrWindow::PrepareKey( RTMessage msg )
 {
   if( pbmpDIB )
    {  
      int iPoW = iStaOrgX,
	  iPoH = iStaOrgY;
      int iDx = iBmpWidth - iBmpViewWidth,
	  iDy = iBmpHigh - iBmpViewHigh;
      int iVStep = double(iBmpViewHigh) * 0.2,
	  iHStep = double(iBmpViewWidth) * 0.2;

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
           int cod = msg.WParam;
	   TWindow::DefWndProc( msg );
	   if( cod != VK_SHIFT )
	     //CloseWindow();
             PostMessage( HWindow, WM_CLOSE, 0, 0 );
           return;
       }


      if( iDx > 0 || iDy > 0 )
       {
         if( iPoW > 0 ) iPoW = 0;
         else if( iPoW < -iDx ) iPoW = -iDx;
         if( iPoH > 0 ) iPoH = 0;
	 else if( iPoH < -iDy ) iPoH = -iDy;

	 if( iPoW != iStaOrgX || iPoH != iStaOrgY )
	  {
	    iStaOrgX = (iDx> 0) ? iPoW:0;
	    iStaOrgY = (iDy > 0) ? iPoH:0;
	    SetViewportOrg( hdc, iStaOrgX, iStaOrgY );

	    InvalidateRect( HWindow, NULL, TRUE );
	  }
       }   
    }
   else PostMessage( HWindow, WM_CLOSE, 0, 0 );//CloseWindow();

   TWindow::DefWndProc( msg );
 }

void TFullScrWindow::WMKeyDown( RTMessage msg )
 {
   PrepareKey( msg );
 }

void TFullScrWindow::WMSysKeyDown( RTMessage msg )
 {
   PrepareKey( msg );
 }


void TFullScrWindow::WMLButtonDown( RTMessage msg )
 {
   TWindow::WMLButtonDown( msg );
   SetCursor( hcHand );
   bFlTrack = TRUE;
 }

void TFullScrWindow::WMSetCursor( RTMessage msg )
 {
   if( bFlTrack == TRUE )
    {
      SetCursor( hcHand );
      msg.Result = TRUE;
    }
   else TWindow::DefWndProc( msg );
 }

void TFullScrWindow::WMLButtonUp( RTMessage msg )
 {
   TWindow::DefWndProc( msg );
   SetCursor( LoadCursor(NULL, IDC_ARROW) );
   bFlTrack = FALSE;
 }

void TFullScrWindow::WMRButtonUp( RTMessage msg )
 {
   TWindow::DefWndProc( msg );
   //CloseWindow();
   PostMessage( HWindow, WM_CLOSE, 0, 0 );
 }


void TFullScrWindow::WMMouseMove( RTMessage msg )//WM_MOUSEMOVE
 {
   if( msg.WParam & MK_LBUTTON )
    {
      if( pbmpDIB )
       {

	 int iPoW = iStaOrgX,
	     iPoH = iStaOrgY;
         int iDx = iBmpWidth - iBmpViewWidth,
	     iDy = iBmpHigh - iBmpViewHigh;      

	 iPoW -= (TFullScrWindow::iMouseX - LOWORD(msg.LParam));
	 iPoH -= (TFullScrWindow::iMouseY - HIWORD(msg.LParam));

         if( iDx > 0 || iDy > 0 )
          {
            if( iPoW > 0 ) iPoW = 0;
            else if( iPoW < -iDx ) iPoW = -iDx;
            if( iPoH > 0 ) iPoH = 0;
	    else if( iPoH < -iDy ) iPoH = -iDy;

	    if( iPoW != iStaOrgX || iPoH != iStaOrgY )
	     {
	       iStaOrgX = (iDx> 0) ? iPoW:0;
	       iStaOrgY = (iDy > 0) ? iPoH:0;
	       SetViewportOrg( hdc, iStaOrgX, iStaOrgY );

	       InvalidateRect( HWindow, NULL, TRUE );
	     }
          }   
       }
      msg.Result = 0;
    }
   else TWindow::DefWndProc( msg );

   TFullScrWindow::iMouseX = LOWORD(msg.LParam);
   TFullScrWindow::iMouseY = HIWORD(msg.LParam);
 }

void TFullScrWindow::WMPaletteChanged( RTMessage Msg )
 {
   if( (HWND)(Msg.WParam) == HWindow )
    {
      Msg.Result = 0;
      return;
    }
   else WMQueryNewPalette( Msg );
 }


void TFullScrWindow::WMQueryNewPalette( RTMessage Msg )
 {
   HPALETTE HpalOld;
   int i;
   HPALETTE hp = hpalAlias ? hpalAlias:(pbmpDIB ? pbmpDIB->GetPal():NULL);

   if( !hp ) { Msg.Result = 0; return; }

   if( !hdc ) hdc = GetDC( HWindow );
   HpalOld = SelectPalette( hdc, hp, FALSE );

   i = RealizePalette( hdc );

   SelectPalette( hdc, HpalOld, FALSE);

   if (i > 0)
     InvalidateRect( HWindow, NULL, TRUE);


   Msg.Result = i;
 }


