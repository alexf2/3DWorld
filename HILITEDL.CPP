
#include <stdio.h>

#include "bmp.hpp"
#include "hilitedl.hpp"
#include "autoctl.hpp"
#include "autorc.hpp"
#include "INC\io_em.inc"

#include "INC\hilitedl.inc"
#include "INC\errors.inc"

#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;


static BOOL bFlEnter = FALSE;

TPlaceHiliteDlg::TPlaceHiliteDlg( PTWindowsObject AParent, LPSTR AName,
		                  int& riDst, LPSTR lpTtl,
				  PTModule AModule ):
  TCenterDialog( AParent, AName, AModule ),
  riDistance( riDst ),
  strTtl( lpTtl )
 {
   riDistance = 50;
   bFlEnter = FALSE;
 }

TPlaceHiliteDlg::~TPlaceHiliteDlg()
 {
 }

void TPlaceHiliteDlg::DrawItem( DRAWITEMSTRUCT _FAR & DrawInfo )
 {   
   if( bFlEnter == FALSE && IsWindowVisible(HWindow) )
    {
      SetFocus( GetDlgItem(HWindow, ID_HILITEDLG_DISTANCE_INPUT) );
      SendDlgItemMessage( HWindow, ID_HILITEDLG_DISTANCE_INPUT,
	EM_SETSEL, 0, MAKELONG(0, -1) );
    }


   if( DrawInfo.CtlID == ID_BTNBMP )
     DrawBmp( DrawInfo.hDC, hbmHint() );

   bFlEnter = TRUE;
 }

void TPlaceHiliteDlg::SetupWindow()
 {
   TCenterDialog::SetupWindow();
   
   SetCaption( (LPSTR)PCchar(strTtl) );
   evErr.SetOwnerDialog( HWindow );
   InitEditors();

   char cBuf[25];
   sprintf( cBuf, "%d", riDistance );
   SetDlgItemText( HWindow, ID_HILITEDLG_DISTANCE_INPUT, cBuf );

   hbmHint = LoadBitmap( hInstRC, MAKEINTRESOURCE(ID_HILITEDLG_BITMAP) );

   if( !hbmHint )
    {
      Status = ER_BITMAPLOAD;
      GetApplication()->Error( ER_BITMAPLOAD );
      TMessage msg; _fsetmem( &msg, sizeof(TMessage), 0 );
      Cancel( msg );
      return;
    }

   SetFocus( GetDlgItem(HWindow, ID_HILITEDLG_DISTANCE_INPUT) );
 }

void TPlaceHiliteDlg::WMCommand( RTMessage Msg )
 {
   TCenterDialog::WMCommand( Msg );
 }

void TPlaceHiliteDlg::Ok( RTMessage msg )
 {
   char cBuf[25];
   GetDlgItemText( HWindow, ID_HILITEDLG_DISTANCE_INPUT, cBuf, 24 );
   sscanf( cBuf, "%d", &riDistance );

   TCenterDialog::Ok( msg );
 }

BOOL TPlaceHiliteDlg::CanClose()
 {
   int iTst;
   char cBuf[25];
   GetDlgItemText( HWindow, ID_HILITEDLG_DISTANCE_INPUT, cBuf, 24 );

   return sscanf( cBuf, "%d", &iTst );
 }

