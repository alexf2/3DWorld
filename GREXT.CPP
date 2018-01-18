#pragma option -v-

#include "grext.hpp"
#include "status.hpp"


#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;


void TGrExtentsDlg::SetupWindow()
 {
   static char *cInf = "Input grid extents and spacing. Press OK for accept, Cancel for close dialog";
   TSendTxt stSend;
   stSend.handle = 0;
   stSend.lpTxt = cInf;

   TCenterDialog::SetupWindow();

   SendMessage( Parent->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );

   SetDlgItemInt( HWindow, ID_STA_X, pGrid->bounds.left, TRUE );
   SetDlgItemInt( HWindow, ID_END_X, pGrid->bounds.right, TRUE );

   SetDlgItemInt( HWindow, ID_STA_Y, pGrid->bounds.top, TRUE );
   SetDlgItemInt( HWindow, ID_END_Y, pGrid->bounds.bottom, TRUE );

   SetDlgItemInt( HWindow, ID_SPACE_X, pGrid->spcX, FALSE );
   SetDlgItemInt( HWindow, ID_SPACE_Y, pGrid->spcY, FALSE );
  

   //RECT bounds;
   //int spcX, spcY;
 }

void TGrExtentsDlg::Ok( RTMessage msg )
 {
   BOOL bFlError;
   int left, right, top, bottom, spcX, spcY;

   left = GetDlgItemInt( HWindow, ID_STA_X, &bFlError, TRUE );
   right = GetDlgItemInt( HWindow, ID_END_X, &bFlError, TRUE );

   top = GetDlgItemInt( HWindow, ID_STA_Y, &bFlError, TRUE );
   bottom = GetDlgItemInt( HWindow, ID_END_Y, &bFlError, TRUE );

   spcX = GetDlgItemInt( HWindow, ID_SPACE_X, &bFlError, FALSE );
   spcY = GetDlgItemInt( HWindow, ID_SPACE_Y, &bFlError, FALSE );

   if( spcX < 2 || spcY < 2 || right < left || bottom < top )
     bFlError = FALSE;

   if( bFlError == FALSE )
    {
      BWCCMessageBox( HWindow, "Invalid input data",
	   "Error", MB_ICONSTOP | MB_OK | MB_APPLMODAL );
      msg.Result = 0;
      return;
    }

   pGrid->bounds.left = left;
   pGrid->bounds.right = right;
   pGrid->bounds.top = top;
   pGrid->bounds.bottom = bottom;
   pGrid->spcX = spcX;
   pGrid->spcY = spcY;

   TCenterDialog::Ok( msg );
 }


