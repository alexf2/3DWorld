//#pragma option -v-

#include "ync.hpp"
#include "status.hpp"
#include <button.h>
#include <bgrpbox.h>

#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;

TYNCDialog::TYNCDialog( PTWindowsObject AParent, int Resource, int* res,
			LPSTR lpMsg, int id256_1, BOOL bFlCompress, PTModule AModule ):
  TCenterDialog( AParent, Resource, AModule )
 {
   TMeasur tmRc = TMeasur();
  

						//11 7 201 35
   pTxt = new TBGroupBox( this, 102, NULL, tmRc.Rx(11), tmRc.Ry(7), tmRc.Rx(201), tmRc.Ry(35), AModule );
   ((PTBGroupBox)pTxt)->Attr.Style = 1 | WS_CHILD | WS_VISIBLE;

   pOut = new TBStatic( pTxt, -1, lpMsg, 1, 1, 2, 2, 256, AModule );
   ((PTBStatic)pOut)->Attr.Style = WS_CHILD | WS_VISIBLE | SS_CENTER;



   pRes = res;  *pRes = IDCANCEL;
   hf = CreateFont( -8, 0, 0, 0, FW_SEMIBOLD,
     0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_STROKE_PRECIS,
     DEFAULT_QUALITY, DEFAULT_PITCH, "Helv" );

 
   pDIBNorm =  new TDIB( id256_1, bFlCompress, GetApplication() );
   pDIBPress = new TDIB( id256_1 + 1, bFlCompress, GetApplication() );
   pDIBSel =   new TDIB( id256_1 + 2, bFlCompress, GetApplication() );
   

   if( !pDIBNorm || !pDIBPress || !pDIBSel || pDIBNorm->ErStatus ||
	pDIBPress->ErStatus || pDIBSel->ErStatus )
    {
      Status =  pDIBPress ? pDIBPress->ErStatus:ER_CREATEOBJECT;
      return;
    }
   pArr[0] = pDIBNorm;
   pArr[1] = pDIBPress;
   pArr[2] = pDIBSel;         //TWindow
   hpalDIBBtn = pDIBNorm->CreatePalette();
  
                                                                  //76              //108
   pButBmp = new T256Button( pArr, hpalDIBBtn, this, IDNO, "&No", tmRc.Rx(76), tmRc.Ry(62), tmRc.Rx(108), tmRc.Ry(79), FALSE, AModule );
   pButBmp->Attr.Style = BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW;
   //CONTROL "&No", IDNO, "BorBtn", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP, 66, 51, 91, 63
 }

TYNCDialog::~TYNCDialog()
 {
   if( hf ) DeleteFont( hf );
   if( pDIBNorm ) delete pDIBNorm;
   if( pDIBPress ) delete pDIBPress;
   if( pDIBSel ) delete pDIBSel;
  /* for( int i = 0; i < 3; i++ )
     if( hbm3State[i] ) DeleteObject( hbm3State[i] );*/
   if( hpalDIBBtn ) DeleteObject( hpalDIBBtn );
 }

void T256Button::WMLButtonDown( RTMessage Msg )
 {
   SetCapture( HWindow );
   bFlMouse = TRUE;
   bFlInRect = TRUE;
   InvalidateRect( HWindow, NULL, FALSE );
   SetFocus( HWindow );

   TButton::WMLButtonDown( Msg );
 }

void T256Button::WMLButtonUp( RTMessage Msg )
 {
   ReleaseCapture();
   bFlMouse = FALSE;
   bFlInRect = FALSE;
   InvalidateRect( HWindow, NULL, FALSE );

   TButton::WMLButtonDown( Msg );
 }

void T256Button::WMMouseMove( RTMessage Msg )//WM_MOUSEMOVE
 {
   static BOOL bPrevState = FALSE;
   RECT rect;

   Msg.Result = 0;
   if( (Msg.WParam & MK_LBUTTON) != MK_LBUTTON )
     bFlInRect = FALSE;
   else
    {
      int x = LOWORD( Msg.LParam );
      int y = HIWORD( Msg.LParam );
   
      GetWindowRect( HWindow, &rect );//TWindow
      if( x < 0 || x > rect.right - rect.left + 1 ||
	  y < 0 || y > rect.bottom - rect.top + 1 )
        bFlInRect = FALSE;
      else bFlInRect = TRUE;
    }

   if( bPrevState != bFlInRect )
     InvalidateRect( HWindow, NULL, FALSE );
  

   bPrevState = bFlInRect;
   TButton::WMLButtonDown( Msg );
 }


void TYNCDialog::WMCreate( RTMessage Msg )
 {
   TCenterDialog::DefWndProc( Msg );


 }

void TYNCDialog::SetupWindow()
 {
   TCenterDialog::SetupWindow(); 


   SetFontGlobal( hf );
   
   //SendDlgItemMessage( HWindow, 102, WM_SETFONT, (WPARAM)hf, FALSE );
   SendDlgItemMessage( pTxt->HWindow, -1, WM_SETFONT, (WPARAM)hf, FALSE );
   

   RECT r;
   TMeasur tmRc = TMeasur();
   GetClientRect( pTxt->HWindow, &r );
   MoveWindow( pOut->HWindow, tmRc.Rx(5), tmRc.Ry(5), r.right - tmRc.Rx(10), r.bottom - tmRc.Ry(10), FALSE );
   //MoveWindow( pOut->HWindow, 10, 10, 10, 10, FALSE );

   GetClientRect( HWindow, &r );
   int w;
   int sta = (r.right - (w = pArr[0]->GetWidth())) / 2;   //79
   MoveWindow( pButBmp->HWindow, sta, tmRc.Ry(62), w + 1, pArr[0]->GetHigh() + 1, FALSE );


   static char *cYNCDlg = "If you want to keep work select YES. If You select DEAD data must lost, if you select Cancel closing will abort.";

   TSendTxt stSend;
   stSend.handle = 0;
   stSend.lpTxt = cYNCDlg;
   SendMessage( Parent->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );

   hwndNoBut = GetDlgItem( HWindow, IDNO );
   //SendDlgItemMessage( HWindow, -1, WM_SETFONT, (WPARAM)hf, FALSE );


   if( pDIBNorm && pDIBPress && pDIBSel )

      SetWindowPos( pButBmp->HWindow, GetDlgItem(HWindow, IDYES),
	pButBmp->Attr.X, pButBmp->Attr.Y, pButBmp->Attr.W, pButBmp->Attr.H,
	SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOACTIVATE );

 }

void TYNCDialog::WMCommand( RTMessage msg )
 {
   if( HIWORD(msg.LParam) == BN_CLICKED )
    {
      *pRes = msg.WParam;
      msg.Result = 0;
      ShutDownWindow();
      return;
    }
 
   TCenterDialog::WMCommand( msg );
 }

void T256Button::SetupWindow()
 {
   TButton::SetupWindow();
   //InvalidateRect( HWindow, NULL, TRUE );
 }

void T256Button::DrawBmp( HDC hdc )
 {
   HPALETTE HpalOld;

   HpalOld = SelectPalette( hdc, hpalDIBBtn, FALSE );
   RealizePalette( hdc );

   pDrawBmp->DrawBitmapDIB( hdc, 0, 0 );

   SelectPalette( hdc, HpalOld, FALSE );
 }

void T256Button::Paint( HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo )
 {
   TButton::Paint( PaintDC, PaintInfo );
   //pDrawBmp = (bFlFocus == TRUE) ? pButBmp[2]:pButBmp[0];
   //DrawBmp( PaintDC );
 }

void T256Button::WMEraseBkGnd( RTMessage Msg )
 {
   Msg.Result = 0;
 }

void T256Button::WMDrawItem( RTMessage Msg )
 {
   TButton::WMDrawItem( Msg );
 } 

       //TControl  WM_MOUSEMOVE
void T256Button::ODADrawEntire( DRAWITEMSTRUCT _FAR & DrawInfo )
 {
   //if( (DrawInfo.itemState & (ODS_FOCUS | ODS_SELECTED)) == (ODS_FOCUS | ODS_SELECTED) )
     //pDrawBmp = pButBmp[1];
   if( bFlInRect == TRUE && bFlMouse == TRUE )
     pDrawBmp = pButBmp[1];
   else
   if( (DrawInfo.itemState & ODS_FOCUS) == ODS_FOCUS )
     { pDrawBmp = pButBmp[2]; bFlFocus = TRUE; }
   else { pDrawBmp = pButBmp[0]; bFlFocus = FALSE; }
   

   DrawBmp( DrawInfo.hDC );
 }
