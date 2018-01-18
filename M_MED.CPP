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

void TMyMDIFrame::CMAssignMaterial( RTMessage )
 {
   PTMyMDIChild pMDICh = (PTMyMDIChild)ActiveChild_;
   if( CondVisyalMDI(pMDICh) == FALSE ) return;
   if( pMDICh->GetCriticalLock() == TRUE ) { MessageBeep(-1); return; }

   if( !pMDICh->GetScena()->SelectedCount(FilterBody) )
    {      
      MakeMsg( hInstRC, HWindow, IO_MSG_SELECTEDBODYSNOTFOUND,
	      ID_TITLE_MESSAGE, MB_ICONASTERISK | MB_OK | MB_APPLMODAL,
	      NULL );

      return;     
    }

   PTAssignDialog  pAss = NULL;
   int res;
   TMedium  mdSel;

   PTSurface pSf = pMDICh->GetScena()->GetSelectedFirst(FilterBody);
   if( pSf ) mdSel = pSf->medium;


   if( (pAss = new TAssignDialog( this, "DlgAssign", mdSel, dblMedium, pMRC )) && !pAss->Status )
     res = GetApplication()->ExecDialog( pAss );
   else
    {
      GetApplication()->Error( pAss ? pAss->Status:ER_CREATEOBJECT );
      if( pAss ) pAss->CloseWindow();//delete pAss;
      return;
    }

   if( res == IDOK )
    {
      pMDICh->GetScena()->AssignMediumAllSelected( mdSel );

      char cBuf[128];

      TAutoStringRc s1_( hInstRC, ID_MSG_63, 50 );
      wsprintf( cBuf, /*"Medium [%s] is assigned"*/s1_(), mdSel.GetName() ? mdSel.GetName():"" );

      TSendTxt stSend;
      stSend.handle = 0;
      stSend.lpTxt = cBuf;

      SendMessage( HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
    }
  }

static int SaveMediumInstance( const Object _FAR &rObj, void _FAR *hf )
 {
   return (RTMedium(rObj).Save( (HFILE)hf ) == HFILE_ERROR) ? 1:0;
 }

void TMyMDIFrame::CMSaveMedium( RTMessage )
 {
   UINT uiOldErrMode = SetErrorMode( SEM_FAILCRITICALERRORS );

   saveMedDlgTransfer.MakeInit();
   PTCenterFileDialog pFileDial = NULL;

   if( (pFileDial = new TCenterFileDialog(this, DlgMedSave, &saveMedDlgTransfer,
      TCenterFileDialog::Init, pMRC)) &&
     !pFileDial->Status )
    {
      int res;
      if( (res = GetApplication()->ExecDialog( pFileDial )) == IDOK )
       {       

	 OFSTRUCT of;   
	 if( OpenFile(saveMedDlgTransfer.chFilePath, &of, OF_EXIST) != HFILE_ERROR )
	  {
	    int res = MakeMsg( hInstRC, HWindow, IO_MSG_ALREDYEXIST,
	      ID_TITLE_WARNING, MB_ICONEXCLAMATION | MB_OKCANCEL | MB_APPLMODAL,
	      saveMedDlgTransfer.chFilePath );

	    if( res == IDCANCEL ) { /*SetErrorMode(uiOldErrMode);*/ return; }
	  }     

	 TAutoFileRc<HFILE> hf( OpenFile( saveMedDlgTransfer.chFilePath, &of, OF_CREATE ) );
	 //HFILE hf = OpenFile( saveMedDlgTransfer.chFilePath, &of, OF_CREATE );

	 if( hf() == HFILE_ERROR )
          {
	    MakeMsgErr( hInstRC, HWindow, IO_ERR_CREATE,
	      ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
	      saveMedDlgTransfer.chFilePath );

	    //SetErrorMode(uiOldErrMode);
            return;	    
          }

	 TMediumHeader  mdhHeader( dblMedium.getItemsInContainer() );
	 mdhHeader.InitCRC();
	 if( mdhHeader.Save( hf() ) == HFILE_ERROR )
	  {    

	    MakeMsgErr( hInstRC, HWindow, IO_ERR_WRITE,
	       ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
	       saveMedDlgTransfer.chFilePath );

	     //_lclose( hf );
	     //SetErrorMode(uiOldErrMode);
	     return;
          }

	 RTMedium rMed = (RTMedium)dblMedium.firstThat( SaveMediumInstance, Pvoid(hf()) );


	 if( rMed != NOOBJECT )
	  {
	    MakeMsgErr( hInstRC, HWindow, IO_ERR_WRITE,
	       ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
	       saveMedDlgTransfer.chFilePath );

	     //_lclose( hf );
	     //SetErrorMode(uiOldErrMode);
	     return;
	  }

	 //_lclose( hf );
	// SetErrorMode(uiOldErrMode);
       }
    }
   else
    {
      GetApplication()->Error( pFileDial ? pFileDial->Status:ER_CREATEOBJECT );
      if( pFileDial ) pFileDial->CloseWindow();//delete pFileDial;
    }

   //SetErrorMode(uiOldErrMode);
 }

void TMyMDIFrame::CMLoadMedium( RTMessage msg )//WM_COMMAND
 {
   UINT uiOldErrMode = SetErrorMode( SEM_FAILCRITICALERRORS );

   LPSTR lpLoa = NULL;
   if( msg.Result == -1  ) lpLoa = (LPSTR)msg.LParam;


   openMedDlgTransfer.MakeInit();
   PTCenterFileDialog pFileDial = NULL;

   if( lpLoa || ((pFileDial = new TCenterFileDialog(this, DlgMedLoad, &openMedDlgTransfer,
      TCenterFileDialog::Init, pMRC)) &&
     !pFileDial->Status) )
    {
      int res;      
      if( lpLoa || (res = GetApplication()->ExecDialog( pFileDial )) == IDOK )
       {
	 if( !lpLoa ) lpLoa = openMedDlgTransfer.chFilePath;

	 OFSTRUCT of;
	 if( OpenFile(lpLoa, &of, OF_EXIST) == HFILE_ERROR )
	  {
	    MakeMsgErr( hInstRC, HWindow, IO_ERR_READ,
	      ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
	      lpLoa );

	    //SetErrorMode(uiOldErrMode);
	    return; 
	  }     

	 TAutoFileRc<HFILE> hf( OpenFile( lpLoa, &of, OF_READ ) );
	 //HFILE hf = OpenFile( lpLoa, &of, OF_READ );

	 if( hf() == HFILE_ERROR )
          {
	    MakeMsgErr( hInstRC, HWindow, IO_ERR_READ,
	      ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
	      lpLoa );

	    //SetErrorMode(uiOldErrMode);
            return;	    
          }

	 dblMedium.flush( TShouldDelete::Delete );
	 TMediumHeader  mdhHeader( 0 );
	 if( mdhHeader.Read( hf() ) == HFILE_ERROR )
	  {
	    MakeMsgErr( hInstRC, HWindow, IO_ERR_READ,
	       ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
	       lpLoa );

	     //_lclose( hf );
	     //SetErrorMode(uiOldErrMode);
	     return;
          }

	 if( mdhHeader.CheckCRC() == FALSE )
	  {
	    MakeMsg( hInstRC, HWindow, IO_ERR_INVALIDFORMAT,
	      ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
	      lpLoa );

	    //_lclose( hf );
	    //SetErrorMode(uiOldErrMode);
	    return;
          }

	 for( int i = 0; i < mdhHeader.d.iNumbMediums; i++ )
	  {
	    PTMedium pMed = new TMedium();

	    if( !pMed )
	     {
	       //_lclose( hf );
	       GetApplication()->Error( EM_OUTOFMEMORY );
	       //SetErrorMode(uiOldErrMode);
	       return;
	     }

	    if( pMed->Load( hf() ) == HFILE_ERROR )
	     {
	       MakeMsgErr( hInstRC, HWindow, IO_ERR_READ,
	         ID_TITLE_ERROR, MB_ICONSTOP | MB_OK | MB_APPLMODAL,
		 lpLoa );

	       delete pMed;
	       //_lclose( hf );
	       //SetErrorMode(uiOldErrMode);
	       return;
	     }

	    dblMedium.add( *pMed );
	  } 

	 //_lclose( hf );
	 //SetErrorMode(uiOldErrMode);
       }
    }
   else
    {
      GetApplication()->Error( pFileDial ? pFileDial->Status:ER_CREATEOBJECT );
      if( pFileDial ) pFileDial->CloseWindow();//delete pFileDial;
    }

  // SetErrorMode(uiOldErrMode);
 }

