//#pragma option -v-

#include "infdlg.hpp"
#include "INC\dlg.inc"

#include <string.h>

#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;

TInfDialog::TInfDialog( PTWindowsObject AParent, LPSTR AName, List& rList_,
  PTModule AModule ):
    TCenterDialog( AParent, AName, AModule ),
    rList( rList_ )
   {
   }



static void FillInstance( Object _FAR &rObj, void _FAR *pH )
 {
   SendMessage( *(HWND*)pH, LB_INSERTSTRING, 0, (LPARAM)(PCchar)(String&)rObj );
 }

void TInfDialog::SetupWindow()
 {
   TCenterDialog::SetupWindow();
   //pCh->FillInfoDlg( this );
   HWND hwndList = GetDlgItem( HWindow, ID_LIST_TXT );
   rList.forEach( FillInstance, Pvoid(&hwndList) ); 

   //hf = (HFONT)GetStockObject( SYSTEM_FIXED_FONT );
   //if( hf ) SetFontGlobal( hf );

   CenterCtlH( IDOK );
 }


TInfDialog::~TInfDialog()
 {
   //if( hf ) DeleteObject( hf ); hf = NULL;
 }

void TInfDialog::DefCommandProc( RTMessage Msg )
 {
   if( Msg.WParam == IDCANCEL )
    {
      Cancel( Msg );
      Msg.Result = 0;
    }
   else
     TCenterDialog::DefCommandProc( Msg );
 }
