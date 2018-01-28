#pragma option -v-

#include "io_msg.hpp"

#include <dos.h>
#include <stdio.h>
#include <bwcc.h>
#include <windows.h>

#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

//extern HINSTANCE hInstRC;

static void ErrMsg( HWND hwnd )
 {
   BWCCMessageBox( hwnd, "Internal application error: string load from resource failed",
     "Error", MB_ICONSTOP | MB_OK | MB_APPLMODAL );
 }

static LPSTR GetStr( HINSTANCE hInst, HWND hwnd, UINT idStrTemplate, UINT idTitle, UINT fuStyle, LPSTR lpName )
 {
   static char cNullStr[] = "";
   const int iLenBuf = 128;
   char cBuf[ iLenBuf ];



   if( !LoadString(hInst, idStrTemplate, cBuf, iLenBuf) )
    {
      ErrMsg( hwnd );
      return NULL;
    }

   LPSTR lpN = lpName ? lpName:cNullStr;
   int iLenMsg = lstrlen( cBuf ) + lstrlen( lpN ) + 2;
   LPSTR lpMsg = new char[ iLenMsg ];

   if( !lpMsg )
    {
      PTApplication pApp = GetApplicationObject();
      if( pApp && pApp->isA() == applicationClass )
	pApp->Error( EM_OUTOFMEMORY );

      return NULL;
    }

   wsprintf( lpMsg, cBuf, lpN );


   return lpMsg;
 }

int _EXPFUNC2 MakeMsg( HINSTANCE hInst, HWND hwnd, UINT idStrTemplate, UINT idTitle, UINT fuStyle, LPSTR lpName )
 {
   LPSTR lpMsg = GetStr( hInst, hwnd, idStrTemplate, idTitle, fuStyle, lpName );
   if( !lpMsg ) return IDCANCEL;

   const int iLenBufH = 128;
   char cBufH[ iLenBufH ];

   if( !LoadString(hInst, idTitle, cBufH, iLenBufH) )
    {
      delete []lpMsg;
      ErrMsg( hwnd );
      return IDCANCEL;
    }


   int res = BWCCMessageBox( hwnd, lpMsg, cBufH, fuStyle );
   delete []lpMsg;

   return res;
 }


int _EXPFUNC2 MakeMsgErr( HINSTANCE hInst, HWND hwnd, UINT idStrTemplate, UINT idTitle, UINT fuStyle, LPSTR lpName )
 {   
   DOSERROR doserr;
   dosexterr( &doserr );

   LPSTR lpMsg = GetStr( hInst, hwnd, idStrTemplate, idTitle, fuStyle, lpName );
   if( !lpMsg ) return IDCANCEL;

   const int iLenBufH = 128;
   char cBufH[ iLenBufH ];

   if( !LoadString(hInst, idTitle, cBufH, iLenBufH) )
    {
      delete []lpMsg;
      ErrMsg( hwnd );
      return IDCANCEL;
    }


   char cBuf[128];
   //sprintf( cBuf, " \0x22" );
   cBuf[0] = ' '; cBuf[1] = '""'; cBuf[2] = 0;
   if( !LoadString(hInst, doserr.de_exterror + IO_ERR_FIRST_ID_ERRORS, cBuf + 2, 127) )
    {
      delete []lpMsg;
      ErrMsg( hwnd );
      return IDCANCEL;
    }
   //sprintf( cBuf + lstrlen(cBuf), " """ );
   int iBLen = lstrlen(cBuf);
   cBuf[iBLen++] = ' '; cBuf[iBLen++] = '""'; cBuf[iBLen] = 0;

   LPSTR lpMsg2 = new char[ lstrlen(lpMsg) + lstrlen(cBuf) + 2 ];
   if( !lpMsg2 )
    {
      delete []lpMsg;

      PTApplication pApp = GetApplicationObject();
      if( pApp && pApp->isA() == applicationClass )
	pApp->Error( EM_OUTOFMEMORY );

      return IDCANCEL;
    }

   wsprintf( lpMsg2, "%s%s", lpMsg, cBuf );

   int res = BWCCMessageBox( hwnd, lpMsg2, cBufH, fuStyle );
   delete []lpMsg;
   delete []lpMsg2;

   return res;
 }

