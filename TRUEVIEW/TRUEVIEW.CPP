#include "trueview.hpp"
#include "selectdl.hpp"
#include <bwcc.h>
#include <dir.h>
#include <string.h>

#include "INC\dialog.inc"
#include "INC\errors.inc"


static TAppError  AppErrors[cErrors] =
 {
   {ER_ICON_LOAD, msWarning, ID_ICON_LOAD},

   {EM_INVALIDCHILD, msError, ID_ICON_LOAD},
   {EM_INVALIDCLIENT, msError, ID_INVALIDCLIENT},
   {EM_INVALIDMAINWINDOW, msError, ID_INVALIDMAINWINDOW},
   {EM_INVALIDMODULE, msError, ID_INVALIDMODULE},
   {EM_INVALIDWINDOW, msError, ID_INVALIDWINDOW},
   {EM_OUTOFMEMORY, msError, ID_OUTOFMEMORY}, //Error

   {ER_CREATE_BRUSH, msError, ID_CREATE_BRUSH},
   {ER_GET_DC, msError, ID_GET_DC},
   {ER_FILEIO, msError, ID_FILEIO},

   {ER_BITMAP_INVALID, msError, ID_BITMAP_INVALID},
   {ER_BITMAP_NONWINDOWS, msError, ID_BITMAP_NONWINDOWS},
   {ER_BITMAP_NOTSUPPORT_FORMAT, msError, ID_BITMAP_NOTSUPPORT_FORMAT},

   {ER_CREATEOBJECT, msError, ID_CREATEOBJECT},
   {ER_BITMAPLOAD, msError, ID_BITMAPLOAD},
   {ER_CURSORLOAD, msError, ID_CURSORLOAD},
   {ER_CREATESURF, msError, ID_CREATESURF}
 };


HINSTANCE hInstRC = NULL;
PTModule pMRC = NULL;
PTModule pModuleRc = NULL;
HTASK htaskMain = NULL;

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine,
		   int nCmdShow )
 {
   htaskMain = GetCurrentTask();

   if( hPrevInst )
    {
      return FALSE;
    }

   (void)BWCCGetVersion();
   SetErrorMode( SEM_FAILCRITICALERRORS );

   hInstRC = LoadLibrary( "_3dw_rc.dll" );
   if( !hInstRC )
    {
      MessageBox( 0, "Cann't load resource, need '_3dw_rc.dll'", "True color view",
		  MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);
      return 1;
    }


   TTrueViewApp  tvaApp( "True view", hInst, hPrevInst,
			 lpCmdLine, nCmdShow );

   pModuleRc = new TModule( "_3DW_RC", hInstRC, NULL );

   pMRC = new TModule( "_3DW_RC", hInstRC, NULL );
   if( !pMRC || pMRC->Status || !pModuleRc || pModuleRc->Status != 0 )
    {
      if( pMRC ) delete pMRC; pMRC = NULL;
      if( pModuleRc ) { delete pModuleRc; pModuleRc = NULL; }
      MessageBox( 0, "Error create RC module", "True color view",
		  MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);
      return 1;
    }

   tvaApp.Run();

   delete pMRC; pMRC = NULL;
   if( pModuleRc ) delete pModuleRc;
   FreeLibrary( hInstRC );


   return tvaApp.Status;
 }


TTrueViewApp::TTrueViewApp( LPSTR AppName, HINSTANCE hInstance, HINSTANCE hPrevInstance,
	      	          LPSTR lpCmdLine, int nCmdShow ):
  TApplication( AppName, hInstance, hPrevInstance, lpCmdLine, nCmdShow )
 {
   char chDrive[MAXDRIVE], chDir[MAXDIR], chName[MAXFILE],
        chExt[MAXEXT];

   lpInitPath = new char[MAXPATH + 100];
   if( lpInitPath )
    {
      if( lpCmdLine )
       {
	 _splitpath( lpCmdLine, chDrive, chDir, chName, chExt );
	 if( !_fstrlen(chDrive) && !_fstrlen(chDir) && !_fstrlen(chName) &&
	     !_fstrlen(chExt)
	   ) *lpInitPath = 0;
	 else _fstrncpy( lpInitPath, lpCmdLine, MAXPATH );
       }
      else *lpInitPath = 0;

      if( !_fstrlen(lpInitPath) ) getcwd( lpInitPath, MAXPATH );

      if( lpInitPath[_fstrlen(lpInitPath) - 1] != '\\' )
        _fstrcat( lpInitPath, "\\" );

      _splitpath( lpInitPath, chDrive, chDir, chName, chExt );
      if( !_fstrlen(chName) && !_fstrlen(chExt) )
	_fstrcat( lpInitPath, "*.bmp" );
    }

 };

TTrueViewApp::~TTrueViewApp()
 {
   if( lpInitPath ) { delete lpInitPath; lpInitPath = NULL; }
 }

void TTrueViewApp::InitMainWindow()
 {   
   MainWindow = new TMySelectDlg( NULL, DialogMainWindow, lpInitPath, this );
 }

void TTrueViewApp::Error( int ErrorCode )
 {
   const int cbBuff = 255;
   char  lpszBuff[cbBuff+1];

   for( int i = 0; i < cErrors; i++ )
     if( AppErrors[i].ErrCode == ErrorCode )
      {                          //TApplication
	if( LoadString(hInstance, AppErrors[i].idStringMessage,
	    lpszBuff, cbBuff) )
	 {	 
	   if( BWCCMessageBox( MainWindow->HWindow, lpszBuff, (AppErrors[i].TypeError==msError)
		       ? "Error":"Warning",
		       ((AppErrors[i].TypeError==msError)
		       ? MB_OKCANCEL:MB_OK) |
		       MB_TASKMODAL |
		       ((AppErrors[i].TypeError==msError)
		       ? MB_ICONSTOP:MB_ICONEXCLAMATION) ) == IDCANCEL )
	     SendMessage( MainWindow->HWindow, WM_SYSCOMMAND, SC_CLOSE, 0L );
	 }
	else
	 {
	   BWCCMessageBox( 0, "Internal application error: cann't load my string resource", "Fatal Error",
		       MB_OK | MB_ICONSTOP | MB_TASKMODAL );
	   //PostQuitMessage( 0 );
	   SendMessage( MainWindow->HWindow, WM_SYSCOMMAND, SC_CLOSE, 0L );
	 }

        return;
      }
   TApplication::Error( ErrorCode );
 }

