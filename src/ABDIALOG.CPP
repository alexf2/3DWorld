//#pragma option -v-

#include <bbutton.h>
#include <bgrpbox.h>
#include <bwcc.h>

#include "fullscr.hpp"
#include "abdialog.hpp"
#include "autorc.hpp"
#include "measur.mac"
#include "INC\aboutbmp.inc"
#include "INC\img256.inc"

#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;

//#include <owl.h>

TAboutDialog::TAboutDialog( PTWindowsObject AParent, LPSTR AName, int id256,
			    BOOL bCompress, PTModule AModule ):
  TCenterDialog( AParent, AName, AModule )
 {
  //TDialog Attr          TApplication   
   TAutoCursorRc crcCursor( LoadCursor(NULL, IDC_WAIT) );
   HWND hwndCap = AParent ? SetCapture( AParent->HWindow ):NULL;  


   if( !(AboutBmp = new TBitMap( GetApplication(), GetApplication()->hInstance,
				 id256, bCompress, this, 105, "", 0, 0 )) ||
       AboutBmp->Status )
     Status = AboutBmp->Status ? AboutBmp->Status:ER_CREATEOBJECT;
         
   
   ReleaseCapture();
   SetCapture( hwndCap );


   /*pBtn = new TBButton( this, IDOK, "&Ok", 1, 1, 64, 40,
     TRUE, AModule );
   ((PTBButton)pBtn)->Attr.Style = BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP;

   pGr =  new TBGroupBox( this, 103, NULL, RX(6), RY(182), RX(284), RY(2),
     AModule );
   ((PTBGroupBox)pGr)->Attr.Style = 4 | WS_CHILD | WS_VISIBLE;*/

 };


void TAboutDialog::WMQueryNewPalette( RTMessage Msg )
 {
   if( AboutBmp ) AboutBmp->WMQueryNewPalette( Msg );
 }

void TAboutDialog::WMPaletteChanged( RTMessage Msg )
 {
   if( AboutBmp ) AboutBmp->WMPaletteChanged( Msg );
 }


void TAboutDialog::SetupWindow()
 {
   static char *cAb = "(c) AlexCorp.  1995  3D World 1.0";
   RECT WinRect;
   POINT PtLeftUp;
   int  W, H;   

   //MoveWindow( HWindow, 1, 1, RX(345), RY(285), FALSE );
   TCenterDialog::SetupWindow();

   TSendTxt stSend;
   stSend.handle = 0;
   stSend.lpTxt = cAb;
   SendMessage( Parent->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
  

   //RECT r;
   //GetWindowRect( pBtn->HWindow, &r );
   GetClientRect( HWindow, &WinRect );
   //int wid = r.right - r.left,
     //  high = r.bottom - r.top;
   //MoveWindow( pBtn->HWindow, (WinRect.right - wid) / 2, RY(232), wid, high, FALSE );

   int wid = double(WinRect.right) * 0.95;
   //MoveWindow( pGr->HWindow, (WinRect.right - wid) / 2, RY(225), wid, 2, FALSE );

   if( AboutBmp )
    {
      GetClientRect( HWindow, &WinRect );

      PtLeftUp.x = (WinRect.right - WinRect.left - AboutBmp->Attr.W) / 2;
      PtLeftUp.y = 10;

      MoveWindow( AboutBmp->HWindow, PtLeftUp.x, PtLeftUp.y,
		  AboutBmp->Attr.W, AboutBmp->Attr.H, FALSE );
    }

    grcBtnBmp[0].grcN = LoadBitmap( hInstRC, MAKEINTRESOURCE(ABOUTDLG_BMP_TM_N) );
    grcBtnBmp[0].grcS = LoadBitmap( hInstRC, MAKEINTRESOURCE(ABOUTDLG_BMP_TM_S) );
    grcBtnBmp[0].grcH = LoadBitmap( hInstRC, MAKEINTRESOURCE(ABOUTDLG_BMP_TM_H) );

    if( !grcBtnBmp[0].grcN || !grcBtnBmp[0].grcS || !grcBtnBmp[0].grcH )
     {
       Status = ER_BITMAPLOAD;
       GetApplication()->Error( ER_BITMAPLOAD );
       TMessage msg; _fsetmem( &msg, sizeof(TMessage), 0 );
       Cancel( msg );
       return;
     }

    HBITMAP hbm[3] = { grcBtnBmp[0].grcN(), grcBtnBmp[0].grcH(), grcBtnBmp[0].grcS() };
    SendDlgItemMessage( HWindow, BTN_TM, BBM_SETBITS, 0, (LONG)(LPSTR)(hbm) );
 }


TAboutDialog::~TAboutDialog()

 {
 }


/*LPSTR TAboutDialog::GetClassName()
 {
   return "3DW:ABOUT";
 }*/ 



/*void TAboutDialog::GetWindowClass(WNDCLASS _FAR & AWndClass)
 {
   TCenterDialog::GetWindowClass( AWndClass );

   //AWndClass.hbrBackground = NULL;
 }*/


void TAboutDialog::DefCommandProc( RTMessage Msg )
 {
   if( Msg.WParam == IDCANCEL )
    {
      Cancel( Msg );
      Msg.Result = 0;
    }
   else
     TCenterDialog::DefCommandProc( Msg );
 }

void TAboutDialog::DefWndProc( RTMessage Msg )
 {
   if( Msg.Message == WM_USER_FULLSCRDONE )
    {
      mrcImg.FreeRc();
      Msg.Result = 0;
      return;
    }
   else if( Msg.Message == WM_COMMAND && Msg.WParam == BTN_TM &&
	    HIWORD(Msg.LParam) == BN_CLICKED
	  )
    {
      mrcImg = new TDIB( DDB256_About2, TRUE, GetApplication() );
      if( !mrcImg || mrcImg()->ErStatus )
       {
         mrcImg.FreeRc();
         GetApplication()->Error( !!mrcImg ? mrcImg()->ErStatus:ER_CREATEOBJECT );

         return;
       }

      PTFullScrWindow pfswWin = new TFullScrWindow( (PTAboutDialog)this, NULL, mrcImg(), NULL, GetModule() );
      if( pfswWin && pfswWin->Status )
       {
         mrcImg.FreeRc();
         GetApplication()->Error( pfswWin->Status );
         pfswWin->CloseWindow();
       }
      else GetApplication()->MakeWindow( pfswWin );

      Msg.Result = 0;
      return;
    }


   TCenterDialog::DefWndProc( Msg );
 }

#pragma option -v.
