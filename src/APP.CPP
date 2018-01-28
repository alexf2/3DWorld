//#pragma option -v-

#include "mymdi.hpp"
#include "app.hpp"
#include "INC\menu.inc"

#include <dos.h>


extern HINSTANCE hInstRC;

//TApplication
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


/*EM_INVALIDCHILD	-1	One or more of the window's children is not valid.
EM_INVALIDCLIENT	-2	MDI client window could not be created.
EM_INVALIDMAINWINDOW	-3	Main window could not be created.
EM_INVALIDMODULE	-4	Window is invalid because Module object was not set.
EM_INVALIDWINDOW	-5	Window is invalid because Create did not succeed.
EM_OUTOFMEMORY	-6	A memory allocation ate into the safety pool.
*/


T3dWorldApp::T3dWorldApp( LPSTR AppName, HINSTANCE hInstance, HINSTANCE hPrevInstance,
	      	          LPSTR lpCmdLine, int nCmdShow ):
  TApplication( AppName, hInstance, hPrevInstance, lpCmdLine, nCmdShow )
 {
   HAccTable = LoadAccelerators( hInstRC, "AccelMainWindow" );
 };

void T3dWorldApp::InitMainWindow()
 {
   (void)BWCCGetVersion();
   MainWindow = new TMyMDIFrame(Name, NULL, this );//TMDIFrame
 }

void T3dWorldApp::InitInstance()
 {
   SetErrorMode( SEM_FAILCRITICALERRORS );

   TApplication::InitInstance();

   char cName[256];

   if( !MainWindow ) return;

   wsprintf( cName, "%s%s", PTMyMDIFrame(MainWindow)->MainPath(), "default.mli" );

   TMessage msg; _fsetmem( &msg, sizeof(TMessage), 0 );
   msg.Result = -1; msg.LParam = (LPARAM)(LPSTR)cName;
   PTMyMDIFrame(MainWindow)->CMLoadMedium( msg );
 }

void T3dWorldApp::Error(int ErrorCode)
 {
   const int cbBuff = 255;
   char  lpszBuff[cbBuff+1];

   for( int i = 0; i < cErrors; i++ )
     if( AppErrors[i].ErrCode == ErrorCode )
      {                          //TApplication
	if( LoadString(hInstRC, AppErrors[i].idStringMessage,
	    lpszBuff, cbBuff) )
	 {	 
	   if( MessageBox( MainWindow->HWindow, lpszBuff, (AppErrors[i].TypeError==msError)
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
	   MessageBox( 0, "Internal application error: cann't load my string resource", "Fatal Error",
		       MB_OK | MB_ICONSTOP | MB_TASKMODAL );
	   //PostQuitMessage( 0 );
	   SendMessage( MainWindow->HWindow, WM_SYSCOMMAND, SC_CLOSE, 0L );
         }

        return;
      }
   TApplication::Error( ErrorCode );
 }


BOOL T3dWorldApp::CanClose()
 {
   return MainWindow->CanClose();
 }

#pragma option -v.
	