//#pragma option -v-

#include "app.hpp"
#include <dialog.h>



HINSTANCE hInstRC = NULL;
PTModule pMRC = NULL;

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine,
		   int nCmdShow )
 {
   if( hPrevInst )
    {
     /* static DLGPROC lpfnDlgProc;
      BOOL  bRes;      
      
      lpfnDlgProc = (DLGPROC)MakeProcInstance( (FARPROC)DupStartDlgProc,
						hInst );
      bRes = DialogBox( hInst, MAKEINTRESOURCE(DL_DUPSTART), 0, lpfnDlgProc );
      FreeProcInstance( (FARPROC)lpfnDlgProc );*/

      MessageBox( 0, "You cann't run more when one copy  !", "3D World Power",
		  MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);
      /*				   
      PostAppMessage( hInst, WM_ACTIVATEAPP, (WPARAM)TRUE,
	(LPARAM)MAKELONG(hPrevInst, 0) );
	*/
      return FALSE;
    }

   hInstRC = LoadLibrary( "_3dw_rc.dll" );
   if( !hInstRC )
    {
      MessageBox( 0, "Cann't load resource, need '_3dw_rc.dll'", "3DWorld 1.0a",
		  MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);
      return 1;
    }

   T3dWorldApp  _3dWorldApp = T3dWorldApp( "3D World 1.0a", hInst, hPrevInst,
					   lpCmdLine, nCmdShow );

   pMRC = new TModule( "_3DW_RC", hInstRC, NULL );
   if( !pMRC || pMRC->Status )
    {
      if( pMRC ) delete pMRC; pMRC = NULL;
      MessageBox( 0, "Error create RC module", "3DWorld 1.0a",
		  MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);
      return 1;
    }

   _3dWorldApp.Run();

   delete pMRC; pMRC = NULL;
   FreeLibrary( hInstRC );

   return _3dWorldApp.Status;
 }

#pragma option -v.
