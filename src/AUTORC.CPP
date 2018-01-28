#pragma option -v-

#include "autorc.hpp"
#include <bwcc.h>
#include <applicat.h>

#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

//extern HINSTANCE hInstRC;


TAutoStringRc::TAutoStringRc( HINSTANCE hInst, int id, int iSize )
 {
   int len = (iSize < 1) ? 2:iSize;
   tRc = new char[ len ];
   if( !tRc ) return;

   tRc[0] = 0;
   if( !LoadString( hInst, id, tRc, len - 1 ) )
    {
      HWND hwnd;
      PTApplication pApp = GetApplicationObject();
      if( pApp && pApp->isA() == applicationClass )
	hwnd = pApp->MainWindow->HWindow;
      else hwnd = NULL;

      BWCCMessageBox( hwnd, "Error loading string resource", "Error", MB_ICONSTOP | MB_OK | MB_APPLMODAL );
    }

 }

TAutoStringRc::~TAutoStringRc()
 {
   if( tRc ) { delete tRc; tRc = NULL; }
 }

char* TAutoStringRc::CopyRc( char*& rT ) const
 {
   char *pTmp = new char[ lstrlen(rT) + 1 ];
   if( !pTmp ) return NULL;

   lstrcpy( pTmp, rT );
   return pTmp; 
 }

char* TAutoStringRc::operator()() const
 {
   return tRc ? tRc:"Error loading string";
 }

TAutoStringRc::TAutoStringRc( char* lpInitStr )
 {
   tRc = new char[ lstrlen(lpInitStr) + 1 ];
   if( tRc ) lstrcpy( tRc, lpInitStr );
 }

void TAutoCursorRc::FreeRc()
 {
   if( tRc ) { SetCursor( tRc ); tRc = NULL; }
 }
