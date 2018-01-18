#include "mymdi.hpp"

#include <shellapi.h>
#include <bwcc.h>
#include <mem.h>

extern HINSTANCE hInstRC;
extern PTModule pMRC;


void TMyMDIFrame::WMDropFile( RTMessage Msg )
 {
   int const iLenRes = 256;
   LPSTR lpName = new char[ iLenRes ];
   if( !lpName )
    {
      DragFinish( (HDROP)Msg.WParam );
      BWCCMessageBox( HWindow, "Not enought memory", "Error", MB_ICONSTOP | MB_OK | MB_APPLMODAL );

      return;
    }

   _fsetmem( lpName, iLenRes, 0 );
   HDROP hDrop = (HDROP)Msg.WParam;
   int iNumb = DragQueryFile( hDrop , 0xFFFF, NULL, 0 );
   
   for( int i = 0; i < iNumb; i++ )
    {
      int iLengthName = DragQueryFile( hDrop, i, NULL, 0 );
      if( iLenRes < iLengthName )
       {
	 delete[] lpName;
	 lpName = new char[ iLengthName + 1 ];
	 if( !lpName )
	  {
	    DragFinish( (HDROP)Msg.WParam );
            BWCCMessageBox( HWindow, "Not enought memory", "Error", MB_ICONSTOP | MB_OK | MB_APPLMODAL );

            return;
          }
       }
      
      DragQueryFile( hDrop, i, lpName, iLengthName + 1 );

      Open( lpName );
    }

   delete[] lpName;
   DragFinish( hDrop );
 }

