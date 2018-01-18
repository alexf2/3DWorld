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

void TMyMDIFrame::CMSave( RTMessage Msg )
 {
   SetErrorMode( SEM_FAILCRITICALERRORS );

   Msg.Result = 0;
   PTMyMDIChild pCh = ((PTMyMDIChild)ActiveChild_);
   if( !pCh || ActiveChild_->isA() != classtypeMDIChWindow ) return;
   if( pCh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }


   if( pCh->AskBitmap() == TRUE )
    {
      pCh->SaveBmp();
      return;
    }

   PTFileInfo pFInfo = fmManeger->GetInfo( pCh->GetScId() );
   OFSTRUCT of;
   if( pFInfo )
    {
      if( Msg.WParam == 7 && Msg.LParam == 7 )
       {  
	 if( OpenFile(pFInfo->LpFileNameFull, &of, OF_EXIST) != HFILE_ERROR )
	  {   
	    int res = MakeMsg( hInstRC, HWindow, IO_MSG_ALREDYEXIST,
	      ID_TITLE_WARNING, MB_ICONEXCLAMATION | MB_OKCANCEL | MB_APPLMODAL,
	      pFInfo->LpFileNameFull );

            if( res == IDCANCEL ) { Msg.Result = -1; return; }
	  }
       }

      TAutoFileRc<HFILE> hf( OpenFile( pFInfo->LpFileNameFull, &of, OF_CREATE ) );
      //HFILE hf = OpenFile( pFInfo->LpFileNameFull, &of, OF_CREATE );

      if( hf() == HFILE_ERROR )
       {
	 MakeMsgErr( hInstRC, HWindow, IO_ERR_CREATE,
	      ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
	      pFInfo->LpFileNameFull );

	 Msg.Result = -1;
         return;	    
       }


      int res1, res2;

       
      TMDIChFlagHolder  hold( pCh, &TMyMDIChild::SetCriticalLock, TRUE );
      pCh->LockForReenter( TRUE );

      TAutoStringRc s1_( hInstRC, ID_MSG_60, 20 );
      PTIndicator  pInd = new TIndicator( this, /*"Save a file"*/s1_(), pCh->GetScena()->GetBytesSize(FALSE, FALSE), pMRC );
      pInd->AddStr( pFInfo->LpFileNameFull );
      pInd->Show();


      res1 = pCh->GetScena()->Save( hf(), pInd );
      pInd->WorkIs100();
      pInd->Close();

      pCh->LockForReenter( FALSE );

      if( res1 < 0 )
       {
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
	      MakeMsgErr( hInstRC, HWindow, IO_ERR_WRITE,
                ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
		 pFInfo->LpFileNameFull );
	      break;

          }

	 //_lclose( hf );
	 Msg.Result = -1;
         return;
       }


      SetCapture( HWindow );
      HCURSOR hcOld = SetCursor( LoadCursor(NULL, IDC_WAIT) );
      res2 = SaveMDI( hf() );
      SetCursor( hcOld );
      ReleaseCapture();


      if( res2 < 0 )
       {
	 switch( res2 )
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
	      MakeMsgErr( hInstRC, HWindow, IO_ERR_WRITE,
                ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
		pFInfo->LpFileNameFull );
	      break;
	   };

	 //_lclose( hf );
	 Msg.Result = -1;
	 return;
       }     

      //_lclose( hf );
    }
 }




void TMyMDIFrame::CMSaveAs( RTMessage Msg )
 {
   PTCenterFileDialog  pFileDial = NULL;
   int res;
   PTMyMDIChild pCh = ((PTMyMDIChild)ActiveChild_);
   if( !pCh || ActiveChild_->isA() != classtypeMDIChWindow )  return;
		    //TDialog
   if( pCh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   if( pCh->AskBitmap() == TRUE )
    {
      pCh->SaveBmp();
      return;
    }


   saveDlgTransfer.MakeInit();
   LPSTR lpKeepName = _fstrdup( fmManeger->GetInfo( pCh->GetScId() )->LpFileNameFull );


   if( (pFileDial = new TCenterFileDialog(this, DlgFileSave, &saveDlgTransfer,
      TCenterFileDialog::Init, pMRC)) &&
     !pFileDial->Status )
    {
      if( (res = GetApplication()->ExecDialog( pFileDial )) == IDOK )
       {
	 fmManeger->ChangeName( ((PTMyMDIChild)ActiveChild_)->GetScId(),
	   saveDlgTransfer.chFilePath );

	 Msg.WParam = 7; Msg.LParam = 7;
	 CMSave( Msg );

	 if( Msg.Result == -1 )
	   fmManeger->ChangeName( ((PTMyMDIChild)ActiveChild_)->GetScId(),
	      lpKeepName );
       }
    }
   else
    {
      GetApplication()->Error( pFileDial ? pFileDial->Status:ER_CREATEOBJECT );
      if( pFileDial ) pFileDial->CloseWindow(); //delete pFileDial;
    }

   delete lpKeepName;
 }

static BOOL SaveInst( Pvoid pCh, Pvoid tr )
 {
   PTMyMDIChild  pChild = (PTMyMDIChild)pCh;
   TTr *pTr = (TTr *)tr;

   if( PTWindow(pChild)->isA() != classtypeMDIChWindow ) return 0;
   if( pChild->GetScId() != pTr->id ) return 0;

   int res;
   if( (res = pChild->Save( pTr->hf )) < 0 )
    {
      pChild->Status = res; return 1;
    }

   return 0;
 }

int TMyMDIFrame::SaveMDI( HFILE hf )
 {
   SetErrorMode( SEM_FAILCRITICALERRORS );

   PTMyMDIChild pCh = ((PTMyMDIChild)ActiveChild_);
   if( !pCh || PTWindow(pCh)->isA() != classtypeMDIChWindow ) return -1;
   PTFileInfo pFInfo = fmManeger->GetInfo( pCh->GetScId() );
   if( !pFInfo ) return -1;

   TMDISave ms( pFInfo->CCopy, pFInfo->DsDivScreen, pFInfo->bFlWndGrowAsParent,
     pCh->CalcSelf() );
   ms.InitCRC();

   //if( _lwrite(hf, &ms, sizeof(TMDISave)) == HFILE_ERROR ) return -1;
   if( ms.Save(hf) == HFILE_ERROR ) return -3;

   TTr tr; tr.id = pCh->GetScId(); tr.hf = hf;
   PTMyMDIChild pCErr = (PTMyMDIChild)FirstThat( SaveInst, (Pvoid)&tr );

   int ret = 0;
   if( pCErr ) { ret = pCErr->Status; pCErr->Status = 0; }

   return ret;
 }
