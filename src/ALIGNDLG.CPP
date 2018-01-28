#pragma option -v-

#include <bgrpbox.h>
#include <bradio.h>
#include <bwcc.h>

#include "aligndlg.hpp"
#include "measur.mac"

#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;

TAligmentDialog::TAligmentDialog( PTWindowsObject AParent, LPSTR AName,
				  BOOL _FAR &rbForwardFlag, PTModule AModule ):
    TCenterDialog( AParent, AName, AModule),
    rbForwardFl( rbForwardFlag )
   {
     TMeasur m;

     /*PTBGroupBox pGr = new TBGroupBox( this, ALIGNDLG_IDNORMALGROUP, NULL,
       m.Rx(10), m.Ry(17), m.Rx(61), m.Ry(40) );*/
       //12, 21, 68, 45
     //pGr->Attr.Style |= 1 | WS_CHILD | WS_VISIBLE | WS_GROUP;
     PTBGroupBox pGr = new TBGroupBox( this, ALIGNDLG_IDNORMALGROUP, AModule );


     /*PTBRadioButton pR = new TBRadioButton( this, ALIGNDLG_IDCHECK_FORWARD,
       "&Forward", m.Rx(17), m.Ry(22), m.Rx(50), m.Ry(10), pGr );*/
       //21, 27, 50, 10
     //pR->Attr.Style |= WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP;
     new TBRadioButton( this, ALIGNDLG_IDCHECK_FORWARD, pGr, AModule );

     /*pR = new TBRadioButton( this, ALIGNDLG_IDCHECK_BACKWARD,
       "&Backward", m.Rx(17), m.Ry(40), m.Rx(50), m.Ry(10), pGr );*/
       //21, 47, 50, 10
     //pR->Attr.Style |= WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP;
     new TBRadioButton( this, ALIGNDLG_IDCHECK_BACKWARD, pGr, AModule );
   }


void TAligmentDialog::ShutDownWindow( int ARetValue )
 {
   rbForwardFl = (BOOL)SendDlgItemMessage( HWindow, ALIGNDLG_IDCHECK_FORWARD, BM_GETCHECK, 0, 0 );

   TCenterDialog::ShutDownWindow( ARetValue );
 }

void TAligmentDialog::SetupWindow()
 {
   TCenterDialog::SetupWindow();

   HFONT hf = (HFONT)SendMessage( HWindow, WM_GETFONT, 0, 0 );
   if( hf ) SetFontGlobal( hf );

   CenterCtlH( IDCANCEL );

   SendDlgItemMessage( HWindow, ALIGNDLG_IDCHECK_FORWARD, BM_SETCHECK, (WPARAM)rbForwardFl, 0 );
   SetFocus( GetDlgItem(HWindow, ALIGNDLG_IDALIGMENT_AND_CONNECT) );
 }

void TAligmentDialog::AligmentOnly( RTMessage )
 {
   CloseWindow( ALIGNDLG_IDALIGMENT_ONLY );
 }

void TAligmentDialog::AligmentAndConnect( RTMessage )
 {
   CloseWindow( ALIGNDLG_IDALIGMENT_AND_CONNECT );
 }


