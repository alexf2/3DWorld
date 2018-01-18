#pragma option -v-

#include "mymdi.hpp"
#include "indicat.hpp"
//#include "ucs.hpp"
#include <filedial.h>
//#include "facedial.hpp"
#include <dir.h>
#include <io.h>


#include "INC\systbbmp.inc"
#include "INC\str.inc"
#include "INC\bodybmp.inc"
#include "INC\classtyp.inc"
#include "infdlg.hpp"
#include "ambdlg.hpp"
#include "2darray.hpp"

#include "medium.hpp"
#include "fheader.hpp"
#include "io_msg.hpp"
#include "autorc.hpp"

extern HINSTANCE hInstRC;
extern PTModule pMRC;

void TMyMDIFrame::CMOpen( RTMessage )
 {
   PTCenterFileDialog  pFileDial = NULL;
   int res;
		    //TDialog TMDIFrame
   openDlgTransfer.MakeInit();

   if( (pFileDial = new TCenterFileDialog(this, DlgFileOpen, &openDlgTransfer,
      TCenterFileDialog::Init, pMRC)) &&
     !pFileDial->Status )
    {
      if( (res = GetApplication()->ExecDialog( pFileDial )) == IDOK )
	Open( openDlgTransfer.GetSelect() );
    }
   else
    {
      GetApplication()->Error( pFileDial ? pFileDial->Status:ER_CREATEOBJECT );
      if( pFileDial ) pFileDial->CloseWindow(); //delete pFileDial;
    }
 }


void TMyMDIFrame::Open( LPSTR lpName )
 {
   SetErrorMode( SEM_FAILCRITICALERRORS );
  
   OFSTRUCT of;
   if( OpenFile(lpName, &of, OF_EXIST) == HFILE_ERROR )
    {
      MakeMsgErr( hInstRC, HWindow, IO_ERR_READ,
	      ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
	      lpName );
      return;	    
    }

   TAutoFileRc<HFILE> hf( OpenFile( lpName, &of, OF_READ ) );
   //HFILE hf = OpenFile( lpName, &of, OF_READ );

   pfiCurrentInit = fmManeger->RegisterFile( lpName,
     DsDefaultScreenLayout );
   PTScena pNewScena = new TScena( pfiCurrentInit->IdFile, GetApplication() );

   int res1;

   long sizeFile;
   lseek( hf(), 0, SEEK_END );
   sizeFile = tell( hf() );
   lseek( hf(), 0, SEEK_SET );
   TAutoStringRc s1_( hInstRC, ID_MSG_56, 20 );
   PTIndicator  pInd = new TIndicator( this, /*"Load a file"*/s1_(), sizeFile, pMRC );
   pInd->AddStr( lpName );
   pInd->Show();

   res1 = pNewScena->Load( hf(), pInd );
   pInd->WorkIs100();

   pInd->Close();

   if( res1 < 0 )
    {//&&&&&
      switch( res1 )
       {
	 case -1:
	   TAutoStringRc str_( hInstRC, ID_MSG_2 );
	   TAutoStringRc ttl_( hInstRC, ID_TITLE_ERROR, 10 );
	   BWCCMessageBox( HWindow, str_(),
	     ttl_(), MB_ICONSTOP | MB_OK | MB_APPLMODAL );
	   //"Internal application error: From 'Open'"
	   break;

	 case -2:
	   GetApplication()->Error( EM_OUTOFMEMORY );
	   break;

	 case -3:
	   MakeMsgErr( hInstRC, HWindow, IO_ERR_READ,
             ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
	     lpName );
	   break;

	 case -4:
	   MakeMsg( hInstRC, HWindow, IO_ERR_INVALIDFORMAT,
	      ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
	      lpName );
	   break;

       }

      //_lclose( hf );
      fmManeger->CloseFile( pfiCurrentInit->IdFile );
      delete pNewScena;

      return;
    }


   listOfScene.add( *pNewScena );

   //SetCapture( HWindow );
   //HCURSOR hcOld = SetCursor( LoadCursor(NULL, IDC_WAIT) );
   res1 = LoadMDI( hf(), pNewScena );
   //SetCursor( hcOld );
   //ReleaseCapture();


   if( res1 < 0 )
    {
      TMessage msg; _fsetmem( &msg, sizeof(TMessage), 0 );
      msg.LParam = MAKELONG( 7, 7 );
      msg.WParam = pfiCurrentInit->IdFile;       
      CMClose( msg );   //&&&&&
      if( !msg.Result )
       {
         fmManeger->CloseFile( pfiCurrentInit->IdFile );
	 listOfScene.detach( *pNewScena, TShouldDelete::Delete );
       }

      switch( res1 )
       {
	 case -1:
	   TAutoStringRc str_( hInstRC, ID_MSG_3 );
	   TAutoStringRc ttl_( hInstRC, ID_TITLE_ERROR, 10 );
	   BWCCMessageBox( HWindow, str_(),
	     ttl_(), MB_ICONSTOP | MB_OK | MB_APPLMODAL );
	   //"Internal application error: File maneger cann't find handle"
	   break;

	 case -2:
	   GetApplication()->Error( EM_OUTOFMEMORY );
	   break;

	 case -3:
	   MakeMsgErr( hInstRC, HWindow, IO_ERR_READ,
             ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
	     lpName );
	   break;

	 case -4:
	   MakeMsg( hInstRC, HWindow, IO_ERR_INVALIDFORMAT,
	      ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
	      lpName );
	   break;
       };       

       //_lclose( hf );       
       return;
    }

   //_lclose( hf );


   EnableWinConnectMenu( MF_ENABLED );  //ForEach
 }



void LoadInst( Pvoid pCh, Pvoid tr )
 {
   PTMyMDIChild  pChild = (PTMyMDIChild)pCh;
   TTr *pTr = (TTr *)tr;

   if( PTWindow(pChild)->isA() != classtypeMDIChWindow ) return;
   if( pChild->GetScId() != pTr->id ) return;

   pChild->Load( pTr->hf );
 }

  //-1 - internal
  //-2 - memory
  //-3 - hard
  //-4 - CRC         /***********************************

int TMyMDIFrame::LoadMDI( HFILE hf, PTScena pscScena )
 {
   SetErrorMode( SEM_FAILCRITICALERRORS );

   TMDISave ms;
   PTFileInfo pFInfo = fmManeger->GetInfo( pscScena->GetId() );
   if( !pFInfo ) return -1; 

   HWND hwndOldCap = GetCapture();
   if( !hwndOldCap ) SetCapture( HWindow );
   HCURSOR hcOld = SetCursor( LoadCursor(NULL, IDC_WAIT) );

   if( ms.Read(hf) == HFILE_ERROR )
    {
      SetCursor( hcOld );
      ReleaseCapture();
      SetCapture( hwndOldCap );
      return -3;
    }
   if( ms.CheckCRC() == FALSE )
    {
      SetCursor( hcOld );
      ReleaseCapture();
      SetCapture( hwndOldCap );
      return -4;
    }
   //pFInfo->CCopy = ms.CCopy;
   pFInfo->DsDivScreen = ms.d.DsDivScreen;
   pFInfo->bFlWndGrowAsParent = ms.d.bFlWndGrowAsParent;

   HWND hwndCap;
   BOOL bFlIn = FALSE;
   while( (hwndCap = GetCapture()) && hwndCap == hwndOldCap && IsChild(HWindow, hwndCap) == TRUE )
    {
      bFlIn = TRUE;

      BOOL res;
      MSG msg;
      if( (res = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) == TRUE )  
	DispatchMessage( &msg );
    }

   if( bFlIn == TRUE ) hwndOldCap = GetCapture();

   RECT r; GetClientRect( HWindow, &r );
   TWindowDivision  wdDiv = TWindowDivision( r, ms.d.DsDivScreen );
   vpCurrentInit = TUCS::VP_Front;
   bFlCurrentVisibleFlag = TRUE;
   PTMyMDIChild pmcNextWindow = NULL;
   for( int i = 0; i < ms.d.iNumWnds; i++ )//////////////
    {
      pRectCur = wdDiv[i];

      iDivIndexCurrent = i;
      pmcNextWindow = (PTMyMDIChild)CreateChild();
      if( !pmcNextWindow )
       {
	 SetCursor( hcOld );
         ReleaseCapture();
	 SetCapture( hwndOldCap );

	 return -2;
       }
      if( pmcNextWindow->Status )
       {
	 pmcNextWindow->CloseWindow();

	 SetCursor( hcOld );
         ReleaseCapture();
	 SetCapture( hwndOldCap );

	 return -2;
       }
      int res;
      if( !(res = pmcNextWindow->Load( hf )) )
       {
	 pmcNextWindow->CloseWindow();

	 SetCursor( hcOld );
         ReleaseCapture();
	 SetCapture( hwndOldCap );

	 return res;
       }

      pmcNextWindow->GenTitle( pFInfo );
    }

   /*if( pmcNextWindow )
     ForEach( (TActionMemFunc)&TMyMDIChild::EnableShowInstance, Pvoid(pmcNextWindow->GetScId()) );
     */
   SetCursor( hcOld );
   ReleaseCapture();
   SetCapture( hwndOldCap );

   return 0;
 }
