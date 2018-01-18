#include <stdio.h>
#include <string.h>
#include <alloc.h>
#include <bgrpbox.h>
#include <bradio.h>

#include <lzexpand.h>
#include <shellapi.h>


#include "selectdl.hpp"
#include "io_msg.hpp"
#include "INC\dialog.inc"
#include "INC\bitmap.inc"

#include <noisydlg.hpp>
#include <rgb.hpp>


#include "INC\io_em.inc"


extern HINSTANCE hInstRC;
extern PTModule pMRC;
extern PTModule pModuleRc;

extern HTASK htaskMain;

void CenterWindow( HWND hwnd )
 {
   RECT WinRect;
   POINT PtLeftUp;
   int  W, H;  


   GetWindowRect( hwnd, &WinRect );
   W = WinRect.right - WinRect.left; H = WinRect.bottom - WinRect.top;

   WinRect.left = (GetSystemMetrics(SM_CXSCREEN) - W) / 2; 
   WinRect.top = (GetSystemMetrics(SM_CYSCREEN) - H) / 2;

   MoveWindow( hwnd, WinRect.left, WinRect.top, W, H, FALSE );
 }


void TMySelectDlg::WMDropFile( RTMessage Msg )
 {
   char cBuf[ 256 ];

   DragQueryFile( (HDROP)Msg.WParam, 0, cBuf, 256 );
   DragFinish( (HDROP)Msg.WParam );

   //PostAppMessage( htaskMain, WM_ACTIVATEAPP, TRUE, MAKELONG(htaskMain, 0) );
   SendMessage( HWindow, WM_ACTIVATE, TRUE,
     MAKELONG(HWindow, IsIconic(HWindow)) );

   SetDlgItemText( HWindow, ID_FNAME, cBuf );
   TMessage msg_; _fsetmem( &msg_, sizeof(TMessage), 0 );
   msg_.Message = EN_CHANGE;
   HandleFName( msg_ );
   CanClose();

   char cDrive[MAXDRIVE], cDir[MAXDIR], cName[MAXFILE], cExt[MAXEXT];
   _splitpath( cBuf, cDrive, cDir, cName, cExt );   

   _fstrcpy( cName, "*" );
   if( !_fstrlen(cExt) ) _fstrcpy( cExt, "." );
   _makepath( cBuf, cDrive, cDir, cName, cExt );

   SetDlgItemText( HWindow, ID_FNAME, cBuf );
   HandleFName( msg_ );
   CanClose();   

   Msg.Result = 0;
 }


void TMySelectDlg::Destroy()
 {
   DragAcceptFiles( HWindow, FALSE );
   DragAcceptFiles( hwndBmpV, FALSE );

   TFileDialog2::Destroy();
 }



TMySelectDlg::TMySelectDlg( PTWindowsObject AParent, int ResourceId, LPSTR AFilePath, PTModule AModule ):
  TFileDialog2( AParent, ResourceId, AFilePath, AModule )
 {
   dNoisy.nNoisy = N_None;
   dNoisy.iMarginalAmplitude = 1;
   dNoisy.iUniformAmplitude = 2;
   dNoisy.iUniformThreshold = 20;

   char cBuf[ MAXPATH + 1 ];
   GetModuleFileName( GetApplication()->hInstance, cBuf, MAXPATH );
   char drive[MAXDRIVE], dir[MAXDIR], name[MAXFILE], ext[MAXEXT];
   _splitpath( cBuf, drive, dir, name, ext );
   _makepath( cBuf, drive, dir, "", "" );

   srcMainPath = TAutoStringRc( cBuf );

   bFlNoLoad = FALSE;
   dPercent = 1;

   bFlSysPal = FALSE;
   iPalConvertSize = 256;

   hwndBmpV = NULL;
   iBmpWidth = iBmpHigh = 0;
   
   pFr = new T3DThickFrame( this, ID_BMP_VIEW, 3, AModule );
   pFrDir = new T3DFrameSt( this, ID_GRP_DIRECTORY, AModule );

   PTBGroupBox pGrpMethods = new TBGroupBox( this, ID_GRP_METHODS, AModule ),
	       pGrpDir = new TBGroupBox( this, ID_GRP_DIRECTORY, AModule );

   new TBRadioButton( this, ID_RADIO_QUANT, pGrpMethods, AModule );
   new TBRadioButton( this, ID_RADIO_MEDIUM, pGrpMethods, AModule );
   new TBRadioButton( this, ID_RADIO_LIGHT, pGrpDir, AModule );
   new TBRadioButton( this, ID_RADIO_HAVY, pGrpDir, AModule );

 }

TMySelectDlg::~TMySelectDlg()
 {
   //if( pTrueColor ) { delete pTrueColor; pTrueColor = NULL; }
   //if( p256 ) { delete p256; p256 = NULL; }
   //if( hbrHatch ) { DeleteObject(hbrHatch); hbrHatch = NULL; }
   //if( hbmAlexBmp ) { DeleteObject(hbmAlexBmp); hbmAlexBmp = NULL; }
 }


LPSTR TMySelectDlg::GetClassName()
 {
   return "TV:MAINWINDOW";
 }

void TMySelectDlg::GetWindowClass( WNDCLASS & AWndClass )
 {
   TFileDialog2::GetWindowClass( AWndClass );
   AWndClass.lpfnWndProc = BWCCDefDlgProc;
   AWndClass.hIcon = LoadIcon( GetModule()->hInstance, MAKEINTRESOURCE(ID_TRUEVIEW_ICON) );
 }


void TMySelectDlg::SetupWindow()
 {
   static int iTblBmpBtn[ NUMB_BTNBMP ][ 4 ] =
    {
     {ID_BTNBMP_LOAD_N, ID_BTNBMP_LOAD_P, ID_BTNBMP_LOAD_S, IDOK},
     {ID_BTNBMP_SAVE_N, ID_BTNBMP_SAVE_P, ID_BTNBMP_SAVE_S, ID_BTN_SAVE},
     {ID_BTNBMP_VIEW_N, ID_BTNBMP_VIEW_P, ID_BTNBMP_VIEW_S, ID_BTN_VIEW},
     {ID_BTNBMP_EXIT_N, ID_BTNBMP_EXIT_P, ID_BTNBMP_EXIT_S, IDCANCEL},
     {ID_BTNBMP_PALETTE_N, ID_BTNBMP_PALETTE_P, ID_BTNBMP_PALETTE_S, ID_BTN_PALETTE}
    };

   TFileDialog2::SetupWindow();

   CenterWindow( HWindow );

   if( pFr ) pFr->InitSize();
   if( pFrDir ) pFrDir->InitSize();

 
   //hbrHatch = CreateHatchBrush( HS_BDIAGONAL, RGB(0, 0, 0) );
   //hbmAlexBmp = LoadBitmap( GetModule()->hInstance, MAKEINTRESOURCE(BMP_TrdeMark) );
   arcHatchBrush = CreateHatchBrush( HS_BDIAGONAL, RGB(0, 0, 0) );
   arcAlexBmp = LoadBitmap( GetModule()->hInstance, MAKEINTRESOURCE(BMP_TrdeMark) );

   HWND hwnd = GetDlgItem( HWindow, ID_BMP_TRADEMARK );
   SetWindowLong( hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | WS_CLIPSIBLINGS );


   HCURSOR hcOld = SetCursor( LoadCursor(NULL, IDC_WAIT) );
   HWND hwndOldCap = SetCapture( HWindow );


   for( int i = 0; i < NUMB_BTNBMP; i++ )
    {
      a3dbBmp[ i ].N = LoadBitmap( GetModule()->hInstance, MAKEINTRESOURCE(iTblBmpBtn[i][0]) );
      a3dbBmp[ i ].P = LoadBitmap( GetModule()->hInstance, MAKEINTRESOURCE(iTblBmpBtn[i][1]) );
      a3dbBmp[ i ].S = LoadBitmap( GetModule()->hInstance, MAKEINTRESOURCE(iTblBmpBtn[i][2]) );

      if( !a3dbBmp[ i ].N || !a3dbBmp[ i ].P || !a3dbBmp[ i ].S  )
       {
	 ReleaseCapture();
	 SetCapture( hwndOldCap );
	 SetCursor( hcOld );
	 Status = ER_BITMAPLOAD;
	 GetApplication()->Error( ER_BITMAPLOAD );
         TMessage msg; _fsetmem( &msg, sizeof(TMessage), 0 );
	 Cancel( msg );
	 return;
       }

      HBITMAP hbm[3];
      hbm[0] = a3dbBmp[ i ].N();
      hbm[1] = a3dbBmp[ i ].P();
      hbm[2] = a3dbBmp[ i ].S();
      SendDlgItemMessage( HWindow, iTblBmpBtn[i][3], BBM_SETBITS, 0, (LONG)(LPSTR)hbm );
    }

   BITMAP bm;
   GetObject( a3dbBmp[ NUMB_BTNBMP - 1 ].N(), sizeof(BITMAP), &bm );

   SetWindowPos( GetDlgItem(HWindow, ID_BTN_PALETTE), NULL, 0, 0, bm.bmWidth,
     bm.bmHeight, SWP_NOMOVE | SWP_NOZORDER );


   ReleaseCapture();
   SetCapture( hwndOldCap );
   SetCursor( hcOld );

   CheckBtnState();
   SetupBmpInfo();

   hwndBmpV = GetDlgItem( HWindow, ID_BMP_VIEW );

   SendDlgItemMessage( HWindow, ID_RADIO_MEDIUM, BM_SETCHECK, 1, 0 );
   SendDlgItemMessage( HWindow, ID_RADIO_LIGHT, BM_SETCHECK, 1, 0 );
   SendDlgItemMessage( HWindow, ID_CHECK_DIFFUSION, BM_SETCHECK, 1, 0 );
   DetectRadioState();

   HMENU hm = GetSystemMenu( HWindow, FALSE );
   AppendMenu( hm, MF_SEPARATOR , 0, NULL );
   AppendMenu( hm, MF_BYCOMMAND | MF_ENABLED, UCM_SYSABOUT, "&About" );

   SendDlgItemMessage( HWindow, ID_CHECK_NOISY, BM_SETCHECK, 0, 0 );


   DragAcceptFiles( HWindow, TRUE );
   DragAcceptFiles( hwndBmpV, TRUE );
 }


void TMySelectDlg::DetectRadioState()
 {
   int iFl = SendDlgItemMessage( HWindow, ID_RADIO_MEDIUM, BM_GETCHECK, 0, 0 );

   EnableWindow( GetDlgItem(HWindow, ID_RADIO_LIGHT), iFl == 1 );
   EnableWindow( GetDlgItem(HWindow, ID_RADIO_HAVY), iFl == 1 );
 }


void TMySelectDlg::DrawItem( DRAWITEMSTRUCT _FAR & DrawInfo )
 {  

   if( DrawInfo.CtlID == ID_BTN_PALETTE )
    {
      HBITMAP hbm;

      if( (DrawInfo.itemState & ODS_SELECTED) == ODS_SELECTED )
        hbm = a3dbBmp[ NUMB_BTNBMP - 1 ].P();
      else if( (DrawInfo.itemState & ODS_FOCUS) == ODS_FOCUS )
	hbm = a3dbBmp[ NUMB_BTNBMP - 1 ].S();
      else
	hbm = a3dbBmp[ NUMB_BTNBMP - 1 ].N();

      DrawBmp( DrawInfo.hDC, hbm );
    }
   else
   if( DrawInfo.CtlID == ID_BMP_VIEW )
    {      //mrcTrueColor, mrc256;
      if( !arcBmpStretch )
       {                      
	 UnrealizeObject( arcHatchBrush() );
	 FillRect( DrawInfo.hDC, &DrawInfo.rcItem, arcHatchBrush() );
       }
      else
       {
	 int wid = DrawInfo.rcItem.right - DrawInfo.rcItem.left + 1,
	     high = DrawInfo.rcItem.bottom - DrawInfo.rcItem.top + 1; 

         HPALETTE hpOld;
	 if( !!arcPalBmpStretch )
	   hpOld = SelectPalette( DrawInfo.hDC, arcPalBmpStretch(), FALSE );

	 DrawBmp( DrawInfo.hDC, arcBmpStretch() );

	 if( !!arcPalBmpStretch )
	   SelectPalette( DrawInfo.hDC, hpOld, FALSE );

	 /*PTDIB pDIB = !!mrc256 ? mrc256():(!!mrcTrueColor ? mrcTrueColor():NULL);

         if( !pDIB ) return;

	 HPALETTE hpalOld;
	 if( pDIB->GetPal() )
	   hpalOld = SelectPalette( DrawInfo.hDC, pDIB->GetPal(), FALSE );	 

	 HDC hdcMem = CreateCompatibleDC( DrawInfo.hDC );

         RECT r; GetClientRect( hwndBmpV, &r );
	 arcBmpStretch = CreateCompatibleBitmap( DrawInfo.hDC, r.right, r.bottom );
	 HBITMAP hbmOld = (HBITMAP)SelectObject( hdcMem, arcBmpStretch() );
	 HPALETTE hpO = SelectPalette( hdcMem, pDIB->GetPal(), FALSE );

         UnrealizeObject( arcHatchBrush() );
         FillRect( hdcMem, &r, arcHatchBrush() );     

         pDIB->DrawStretchBitmapDIB( hdcMem, x_, y_, wid_, high_ );     

	 SelectPalette( hdcMem, hpO, FALSE );
         SelectObject( hdcMem, hbmOld );
	 DeleteDC( hdcMem );

	 DrawBmp( DrawInfo.hDC, arcBmpStretch() );
	 arcBmpStretch.FreeRc();
	 //pDIB->DrawStretchBitmapDIB( DrawInfo.hDC, x_, y_, wid_, high_ );

	 if( pDIB->GetPal() )
	   SelectPalette( DrawInfo.hDC, hpalOld, FALSE );
         */
       }
    }
   else if( DrawInfo.CtlID == ID_BMP_TRADEMARK )
     DrawBmp( DrawInfo.hDC, arcAlexBmp() );
   else TFileDialog2::DrawItem( DrawInfo );
 }

void TMySelectDlg::WMPaletteChanged( RTMessage Msg )
 {
   if( (HWND)(Msg.WParam) == HWindow )
    {
      Msg.Result = 0;
      return;
    }
   else WMQueryNewPalette( Msg );
 }


void TMySelectDlg::WMQueryNewPalette( RTMessage Msg )
 {
   //WM_QUERYNEWPALETTE
   //HDC  Hdc;
   HPALETTE HpalOld;
   int i;
   HPALETTE hpPal = !!arcBmpStretch ? arcPalBmpStretch():NULL;
   //!!mrc256 ? mrc256()->GetPal():
   //(!!mrcTrueColor ? mrcTrueColor()->GetPal():NULL);

   if( !hpPal ) { Msg.Result = 0; return; }

   HDC hdc = GetDC( hwndBmpV );
   HpalOld = SelectPalette( hdc, hpPal, FALSE );

   i = RealizePalette( hdc );

   SelectPalette( hdc, HpalOld, FALSE);
   ReleaseDC( hwndBmpV, hdc );

   if (i > 0)
     InvalidateRect( hwndBmpV, NULL, TRUE);


   Msg.Result = i;
 }

void TMySelectDlg::SaveImage()
 {
   if( !mrc256 ) return;
   mrc256()->Save( FilePath );
 }

void TMySelectDlg::ViewImage()
 {
   PTDIB pDIB = !!mrc256 ? mrc256():(!!mrcTrueColor ? mrcTrueColor():NULL);

   if( !pDIB ) return;

   pDIB->Reset();
   UnrealizeObject( pDIB->GetPal() );

   PTFullScrWindow pfswWin = new TFullScrWindow( this, NULL, pDIB, NULL, GetModule() );
   if( pfswWin && pfswWin->Status )
    {
      GetApplication()->Error( pfswWin->Status );
      delete pfswWin;
    }
   else GetApplication()->MakeWindow( pfswWin );
 }

void TMySelectDlg::ShowAbout() const
 {
   mrcImg = new TDIB( ID_BTN256_ABOUT, TRUE, GetApplication() );
   if( !mrcImg || mrcImg()->ErStatus )
    {
      mrcImg.FreeRc();
      GetApplication()->Error( !!mrcImg ? mrcImg()->ErStatus:ER_CREATEOBJECT );

      return;
    }

   //mrcImg()->Save( "e:\\tz.bmp" );

   PTFullScrWindow pfswWin = new TFullScrWindow( (PTMySelectDlg)this, NULL, mrcImg(), NULL, GetModule() );
   if( pfswWin && pfswWin->Status )
    {
      mrcImg.FreeRc();
      GetApplication()->Error( pfswWin->Status );
      pfswWin->CloseWindow();
    }
   else GetApplication()->MakeWindow( pfswWin );
 }

void TMySelectDlg::DefWndProc( RTMessage Msg )
 {
   if( Msg.Message == WM_USER_FULLSCRDONE )
    {
      mrcImg.FreeRc();
      Msg.Result = 0;
      return;
    }
   else
   if( Msg.Message == WM_SYSCOMMAND )
     switch( Msg.WParam & 0xFFF0 )
      {
	case UCM_SYSABOUT:
	  ShowAbout();
	  Msg.Result = 0;
	  return;
      }

   TFileDialog2::DefWndProc( Msg );
 }

void TMySelectDlg::WMCommand( RTMessage Msg )//WM_COMMAND
 {
   if( HIWORD(Msg.LParam) == BN_CLICKED )
    {
      switch( Msg.WParam )
       {
	 case ID_BTN_VIEW:
	   ViewImage();
	   break;
	           
	 case ID_BTN_SAVE:
	   bFlNoLoad = TRUE;
	   CanClose();
	   if( bFlNoLoad == TRUE ) { bFlNoLoad = FALSE; break; }
	   
	   SaveImage();
           break;

	 case ID_RADIO_MEDIUM:
	 case ID_RADIO_QUANT:
	   TFileDialog2::WMCommand( Msg );
	   DetectRadioState();
	   break;


	 case ID_RADIO_LIGHT:
	 case ID_RADIO_HAVY:
	   int iCh1 = SendDlgItemMessage( HWindow, ID_RADIO_LIGHT, BM_GETCHECK, 0, 0 ),
	       iCh2 = SendDlgItemMessage( HWindow, ID_RADIO_HAVY, BM_GETCHECK, 0, 0 );
	   int iTmp = iCh1; iCh1 = iCh2; iCh2 = iTmp;

	   TFileDialog2::WMCommand( Msg );

	   SendDlgItemMessage( HWindow, ID_RADIO_LIGHT, BM_SETCHECK, iCh1, 0 );
	   SendDlgItemMessage( HWindow, ID_RADIO_HAVY, BM_SETCHECK, iCh2, 0 );
	   break;

	 case ID_BTN_PALETTE:
	   PTPaletteDialog pDlg;               

	   if( (pDlg = new TPaletteDialog( this, (LPSTR)MAKEINTRESOURCE(DialogPalette),  bFlSysPal, iPalConvertSize, GetModule() )) && !pDlg->Status )
             GetApplication()->ExecDialog( pDlg );
           else
            {
              GetApplication()->Error( pDlg ? pDlg->Status:ER_CREATEOBJECT );
              if( pDlg ) pDlg->CloseWindow();
	    }
	   break;

	 case ID_CHECK_NOISY:
	   SetupNoisy();
	   SendDlgItemMessage( HWindow, ID_CHECK_NOISY, BM_SETCHECK,
	     (dNoisy.nNoisy == N_None) ? 0:1, 0 );
	   Msg.Result = 0;
           break;

	 default:
	   TFileDialog2::WMCommand( Msg );
	   break;
       };
    }
   else TFileDialog2::WMCommand( Msg );
 }

void TMySelectDlg::SetupNoisy()
 {
   TNoisyDta ndDta = dNoisy;

   PTNoisyDialog pDlg;
   int res;
											 
   if( (pDlg = new TNoisyDialog( this, (LPSTR)MAKEINTRESOURCE(DlgNoisy), &ndDta, pModuleRc )) && !pDlg->Status )
     res = GetApplication()->ExecDialog( pDlg );
   else
    {
      GetApplication()->Error( pDlg ? pDlg->Status:ER_CREATEOBJECT );
      if( pDlg ) pDlg->CloseWindow();
      return;
    }

   if( res == IDOK ) dNoisy = ndDta;
 }


inline double min( double d1, double d2 )
 {
   return (d1 < d2) ? d1:d2;
 }

BOOL TMySelectDlg::MakeLoad( BOOL bRes, LPSTR lpF )
 {
   if( bRes == TRUE )
    {
      if( bFlNoLoad == TRUE ) { bFlNoLoad = FALSE; return FALSE; }

      HCURSOR  hcursOld = SetCursor( LoadCursor(NULL, IDC_WAIT) );
      HWND hwndCap = SetCapture( HWindow );  


      mrc256.FreeRc();
      mrcTrueColor.FreeRc();
      arcBmpStretch.FreeRc();
      arcPalBmpStretch.FreeRc();
      mrcTrueColor = new TDIB( lpF, GetApplication() );

      SetCursor( hcursOld );
      ReleaseCapture();
      SetCapture( hwndCap );

      if( !mrcTrueColor || mrcTrueColor()->ErStatus )
       {
	 int cod  = !!mrcTrueColor ? mrcTrueColor()->ErStatus:ER_CREATEOBJECT;
	 if( !!mrcTrueColor ) mrcTrueColor.FreeRc();
	 CheckBtnState();
	 SetupBmpInfo();

	 arcBmpStretch.FreeRc();
	 arcPalBmpStretch.FreeRc();
	 mrc256.FreeRc();
	 mrcTrueColor.FreeRc();

	 InvalidateRect( hwndBmpV, NULL, TRUE );

	 GetApplication()->Error( cod );

         return FALSE;
       }     


      WORD wBit = mrcTrueColor()->BitsPerPixel();
      if(
	  (mrcTrueColor()->TestType() == FALSE) ||
	  (mrcTrueColor()->IsWindows() == FALSE) ||
          (wBit != 24 && wBit != 8 && wBit != 4 && wBit != 2) ||
	  (mrcTrueColor()->Compression() != BI_RGB && wBit == 24)
	)
       {
	 char cBuf[128];

	 if( mrcTrueColor()->TestType() == FALSE )
	   sprintf( cBuf, "It is not Windows BMP file" );
	 else if( mrcTrueColor()->IsWindows() == FALSE )
	   sprintf( cBuf, "OS/2 BMP file not support" );
         else
	 if( mrcTrueColor()->Compression() != BI_RGB && wBit == 24 )
	   sprintf( cBuf, "True colour RLE compressed bitmap not support" );
         else
	   sprintf( cBuf, "Bitmaps with %u bit per pixel not support", wBit );

	 mrcTrueColor.FreeRc();
	 mrc256.FreeRc();
	 InvalidateRect( hwndBmpV, NULL, TRUE );
         CheckBtnState();
	 SetupBmpInfo();

	 arcBmpStretch.FreeRc();
	 arcPalBmpStretch.FreeRc();	 

	 TAutoStringRc ttl_( GetModule()->hInstance, ID_TITLE_WARNING );
	 BWCCMessageBox( HWindow, cBuf,
	   ttl_(), MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL );
	 
	 return FALSE;
       }

      iBmpWidth = mrcTrueColor()->GetWidth();
      iBmpHigh = mrcTrueColor()->GetHigh();

      RECT r; GetClientRect( hwndBmpV, &r );
      double dX = double(r.right) / double(iBmpWidth),
	     dY = double(r.bottom) / double(iBmpHigh);
      double dS;
      if( r.right >= iBmpWidth && r.bottom >= iBmpHigh ) dS = 1;
      else dS = min( dX, dY );
      dPercent = dS;
      int wid = double(iBmpWidth) * dS,
	  high = double(iBmpHigh) * dS;
      int x = (r.right - wid) / 2,
	  y = (r.bottom - high) / 2;
      if( x < 0 ) x = 0; if( y < 0 ) y = 0;

      CheckBtnState();
      SetupBmpInfo();


      PTDIB pImg;
      if( wBit == 24 )
       {
	 PTIndicator  pInd = new TIndicator( this, "Create palette", 100, pMRC );
	 if( pInd )
	  {
	    //pInd->Show();
	    LPSTR lpName = ShortName( lpF, GetMaxName() );
	    if( lpName )             
	      pInd->AddStr( lpName );
	    else
	      pInd->Close();

	    if( lpName ) delete lpName;
          }
	 if( ConvertTrueColorTo256(pInd) ) pImg = mrc256();
	 else
	  {
	    CheckBtnState();
	    SetupBmpInfo();
	    if( pInd ) pInd->Close();
	    InvalidateRect( hwndBmpV, NULL, TRUE );
	    return FALSE;
          }
	 if( pInd ) pInd->Close();
       }
      else pImg = mrcTrueColor();


      CheckBtnState();


      arcPalBmpStretch = pImg->GetPal();
      pImg->CreatePaletteAssign();
 
      HDC hdc = GetDC( hwndBmpV );
      HPALETTE hpOld = SelectPalette( hdc, pImg->GetPal(), FALSE );
      RealizePalette( hdc );
      SelectPalette( hdc, hpOld, FALSE );
      HDC hdcMem = CreateCompatibleDC( hdc );
      hpOld = SelectPalette( hdcMem, pImg->GetPal(), FALSE );      
      
      arcBmpStretch = CreateCompatibleBitmap( hdc, r.right, r.bottom );      
      HBITMAP hbmOld = (HBITMAP)SelectObject( hdcMem, arcBmpStretch() );      

      UnrealizeObject( arcHatchBrush() );
      FillRect( hdcMem, &r, arcHatchBrush() );     
      


      HCURSOR hcOld = SetCursor( LoadCursor(NULL, IDC_WAIT) );
      HWND hwndOldCap = SetCapture( HWindow );

      pImg->DrawStretchBitmapDIB( hdcMem, x, y, wid, high );

      ReleaseCapture();
      SetCursor( hcOld );

      ReleaseDC( hwndBmpV, hdc );
      SelectPalette( hdcMem, hpOld, FALSE );
      SelectObject( hdcMem, hbmOld );
      DeleteDC( hdcMem );

      InvalidateRect( hwndBmpV, NULL, TRUE );
    }

   return FALSE;
 }

BOOL TMySelectDlg::CanClose()
 { 
   SetErrorMode( SEM_FAILCRITICALERRORS );
   BOOL bRes = TFileDialog2::CanClose();
   if( bRes == FALSE )
    {
    }

   return MakeLoad( bRes, FilePath );
 }

int TMySelectDlg::GetMaxName() const
 {
   const double dMaxSize = 0.7;

   TEXTMETRIC tm;
   HDC hdc = GetDC( HWindow );
   GetTextMetrics( hdc, &tm );
   ReleaseDC( HWindow, hdc );

   return double(GetSystemMetrics( SM_CXSCREEN )) * dMaxSize / double(tm.tmAveCharWidth);
 }

LPSTR ShortName( LPSTR LpFullName, int iMaxLen )
 {
   char Path[MAXPATH];
   int Len;
                
   _fstrcpy( Path, LpFullName );
   if( (Len = _fstrlen(LpFullName)) > iMaxLen )
    {
      LPSTR LpName;
      int   LenName;      
      const char *StrIns = "\\...\\";

      for( LpName = LpFullName + Len - 1, LenName = 0;
	   LenName < Len-1 && *(LpName-1) != '\\';
	   LenName++, LpName-- );

      int SizeNeed = iMaxLen - LenName - _fstrlen(StrIns);
      LPSTR LpPrev, LpCur;

      for( LpPrev = LpCur = Path, LenName = 0;
	   LenName <= SizeNeed && *LpCur;
	   LenName++, LpCur++ )
	if( *LpCur == '\\' ) LpPrev = LpCur;
      *LpPrev = 0;
      _fstrcat( Path, StrIns ); _fstrcat( Path, LpName );
    }

   LPSTR lpTmp;
   if( (lpTmp = new char[ _fstrlen(Path) + 1 ]) )
     _fstrcpy( lpTmp, Path );
   else return NULL;

   return lpTmp;
 }

void TMySelectDlg::HandleFName( RTMessage Msg )
 {
   TFileDialog2::HandleFName( Msg );
 }

void TMySelectDlg::CheckBtnState()
 {
   EnableWindow( GetDlgItem(HWindow, ID_BTN_VIEW), (!!mrcTrueColor || !!mrc256) ? TRUE:FALSE );
   EnableWindow( GetDlgItem(HWindow, ID_BTN_SAVE), (!!mrc256) ? TRUE:FALSE );
 }

void TMySelectDlg::SetupBmpInfo()
 {
   char cBuf[256];
   char cBufTtl[256];

   if( !mrcTrueColor && !mrc256 )
     sprintf( cBufTtl, "True colour viewer 1.0" );
   else
    {
      TEXTMETRIC tm;
      HDC hdc = GetDC( HWindow );
      GetTextMetrics( hdc, &tm );
      ReleaseDC( HWindow, hdc );

      RECT r; GetWindowRect( HWindow, &r );
      LPSTR lpName = ShortName( FilePath, double(r.right - r.left) * 0.7 / double(tm.tmAveCharWidth) );

      if( lpName )
       {      
	 sprintf( cBufTtl, "%s  %3d%%", lpName, int(dPercent * 100) );
         delete lpName;
       }
      else
	sprintf( cBufTtl, "True colour viewer" );
    }
   

   if( !mrcTrueColor )
     sprintf( cBuf, "No image" );
   else
    {
      if( mrcTrueColor()->BitsPerPixel() == 24 )
	sprintf( cBuf, "Image: %ux%u   True colour", (int)mrcTrueColor()->GetWidth(),
		 (int)mrcTrueColor()->GetHigh() );
      else
	sprintf( cBuf, "Image: %ux%u   %u colours, %u bits",
		 (int)mrcTrueColor()->GetWidth(),
		 (int)mrcTrueColor()->GetHigh(),
		 (int)mrcTrueColor()->NumbColors(),
		 (int)mrcTrueColor()->BitsPerPixel()
	       );      
    }

   SetDlgItemText( HWindow, ID_TXT_INFO, cBuf );
   SetCaption( cBufTtl );
 }



BOOL TMySelectDlg::ConvertTrueColorTo256( PTIndicator pI )
 {
   int wid = mrcTrueColor()->GetWidth(),
       high = mrcTrueColor()->GetHigh();

   HCURSOR hcOld = SetCursor( LoadCursor(NULL, IDC_WAIT) );
   HWND hwndOldCap = SetCapture( HWindow );
   TRGB  huge* pB = mrcTrueColor()->GetTRGBBits();
   ReleaseCapture();
   SetCursor( hcOld );

   if( !pB )
    {
      GetApplication()->Error( EM_OUTOFMEMORY );
      return FALSE;
    }
   mrcTrueColor.FreeRc();

   if( pI ) pI->AddStr( "" );


    //PTColorTable pctTbl = NULL;
    //enum TConvertAlho { CA_MedianCutGeom = 1, CA_MedianCutDiscr = 2, CA_Quantize = 4,
		    //CA_Diffuzion = 8
   PTColorTable pctTbl = NULL;
   DWORD  dwLenPal = (bFlSysPal == FALSE) ? iPalConvertSize:16;

   if( bFlSysPal == FALSE )
    {
      TConvertAlho alho =
        SendDlgItemMessage( HWindow, ID_RADIO_QUANT, BM_GETCHECK, 0, 0 ) ? CA_Quantize:
         (SendDlgItemMessage( HWindow, ID_RADIO_LIGHT, BM_GETCHECK, 0, 0 ) ? CA_MedianCutGeom:CA_MedianCutDiscr);

      alho |= (SendDlgItemMessage( HWindow, ID_CHECK_DIFFUSION, BM_GETCHECK, 0, 0 ) ? CA_Diffuzion:0);

      EnableWindow( HWindow, FALSE );
      pctTbl = TrueColorTo256( pB, (DWORD)wid * (DWORD)high, iPalConvertSize, FALSE,
			       alho, wid, high, &dNoisy, pI );
      EnableWindow( HWindow, TRUE );
    }
   else
     pctTbl = TrueColorTo256( pB, (DWORD)wid * (DWORD)high, 16, FALSE,
			      CA_System, wid, high, &dNoisy, pI );

   if( !pctTbl )
    {
      delete pB;
      return FALSE;
    }


   mrc256 = new TDIB( pB, wid, high, pctTbl, dwLenPal, FALSE, GetApplication() );
   delete pB; if( pctTbl ) delete pctTbl;

   if( !mrc256 ) return FALSE;
   if( mrc256()->ErStatus ) { mrc256.FreeRc(); return FALSE; }   

   return TRUE;
 }

TPaletteDialog::TPaletteDialog( PTWindowsObject AParent, LPSTR AName, BOOL& rTyp, int& rSize,
				PTModule AModule ):
  TCenterDialog( AParent, AName, AModule ),
  rType( rTyp ),
  rPalSize( rSize )
 {
   bType = rTyp;
   iPalSize = rSize;

   /*new TBButtonAutoValidate( this, IDOK, AModule );
   new TBButtonAutoValidate( this, IDCANCEL, AModule );

   PTBGroupBox pGrp = new TBGroupBox( this, ID_GRP_PALSIZE, AModule );

   new TBRadioButtonAutoValidate( this, ID_RADIO_SYSPAL, pGrp, AModule );
   new TBRadioButtonAutoValidate( this, ID_RADIO_ADAPTPAL, pGrp, AModule );

   new TEditAutoValidate( this, ID_EDIT_PALSIZE, 10, AModule );   

   *this,
     *new TUIntValidator( ID_EDIT_PALSIZE, "Size adaptive palette", 2, 256, iPalSize );*/
 }

void TPaletteDialog::SetCtl()
 {
   EnableWindow( GetDlgItem(HWindow, ID_EDIT_PALSIZE),
     SendDlgItemMessage( HWindow, ID_RADIO_ADAPTPAL, BM_GETCHECK, 0, 0 )
	       );

   iPalSize = GetCbn();
   if( !FindCbn(iPalSize) ) AddCbn( iPalSize );
 }

BOOL TPaletteDialog::FindCbn( int iVal ) const
 {
   char cBufGet[128];   

   int ind = 0;
   while( 1 )
    {
      if( SendDlgItemMessage( HWindow, ID_EDIT_PALSIZE, CB_GETLBTEXT, ind, (LPARAM)(LPSTR)cBufGet ) == CB_ERR )
        break;
      int iGet = 0;
      sscanf( cBufGet, "%u", &iGet );

      if( iGet == iVal ) return TRUE;

      ind++;
    }

   return FALSE;
 }

int TPaletteDialog::FindCbn( int iVal, int ) const
 {
   char cBufGet[128];

   int ind = 0;
   while( 1 )
    {
      if( SendDlgItemMessage( HWindow, ID_EDIT_PALSIZE, CB_GETLBTEXT, ind, (LPARAM)(LPSTR)cBufGet ) == CB_ERR )
        break;
      int iGet = 0;
      sscanf( cBufGet, "%u", &iGet );

      if( iGet == iVal ) return ind;

      ind++;
    }

   return 0;
 }


void TPaletteDialog::AddCbn( int iVal )
 {
   char cBuf[128];
   sprintf( cBuf, "%u", iVal );

   SendDlgItemMessage( HWindow, ID_EDIT_PALSIZE, CB_ADDSTRING, 0,
     (LPARAM)(LPSTR)cBuf );
 }

void TPaletteDialog::SelectCbn( int iVal )
 {
   SendDlgItemMessage( HWindow, ID_EDIT_PALSIZE, CB_SETCURSEL,
     FindCbn( iVal, 0 ), 0 );
 }

int TPaletteDialog::GetCbn() const
 {
   char cBuf[25];

   if( !GetDlgItemText( HWindow, ID_EDIT_PALSIZE, cBuf, 10 ) ) return 256;
   int iRet;
   sscanf( cBuf, "%u", &iRet );
   if( iRet < 16 || iRet > 256 ) iRet = 256;

   return iRet;
 }


void TPaletteDialog::Ok( RTMessage Msg )
 {
   char cBuf[128];

   GetDlgItemText( HWindow, ID_EDIT_PALSIZE, cBuf, 127 );
   int iTmp;
   int res = sscanf( cBuf, "%u", &iTmp );

   if( !res || iTmp < 16 || iTmp > 256 )
    {
      BWCCMessageBox( HWindow, "Size palette must be  16 <= size <= 256",
	   "Error", MB_ICONSTOP | MB_OK | MB_APPLMODAL );

      Msg.Result = 0;
      SetFocus( GetDlgItem(HWindow, ID_EDIT_PALSIZE) );
      return;
    } 

   rType = SendDlgItemMessage( HWindow, ID_RADIO_SYSPAL, BM_GETCHECK, 0, 0 ) ? TRUE:FALSE;
   rPalSize = iTmp;

   TCenterDialog::Ok( Msg );
 }

TPaletteDialog::~TPaletteDialog()
 { 
 }

void TPaletteDialog::SetupWindow()
 {
   TCenterDialog::SetupWindow();

   evErr.SetOwnerDialog( HWindow );
   //InitEditors();

   SendDlgItemMessage( HWindow, ID_EDIT_PALSIZE, CB_ADDSTRING, 0,
     (LPARAM)(LPSTR)"256" );
   SendDlgItemMessage( HWindow, ID_EDIT_PALSIZE, CB_ADDSTRING, 0,
     (LPARAM)(LPSTR)"236" );
   SendDlgItemMessage( HWindow, ID_EDIT_PALSIZE, CB_ADDSTRING, 0,
     (LPARAM)(LPSTR)"16" );

   SendDlgItemMessage( HWindow, bType ? ID_RADIO_SYSPAL:ID_RADIO_ADAPTPAL, BM_SETCHECK, 1, 0 );
   SendDlgItemMessage( HWindow, bType ? ID_RADIO_ADAPTPAL:ID_RADIO_SYSPAL, BM_SETCHECK, 0, 0 );


   if( !FindCbn(iPalSize) ) AddCbn( iPalSize );
   SelectCbn( iPalSize );

   SetCtl();
 }

void TPaletteDialog::WMCommand( RTMessage Msg )
 {
   if( HIWORD(Msg.LParam) == BN_CLICKED )
    {
      switch( Msg.WParam )
       {
	 case ID_RADIO_SYSPAL:
	 case ID_RADIO_ADAPTPAL:
	   SetCtl();
	   break;
       }
    }


   TCenterDialog::WMCommand( Msg );
 }

/*void TPaletteDialog::WMUserValueValidated( RTMessage msg )
 {
   SetCtl();
   msg.Result = 0;
 }*/

