//#pragma option -v-

/*#include "toolbar.hpp"
#include "indicat.hpp"
#include "status.hpp"
#include "bmp.hpp"*/

#include <owl.h>

PTModule pModule = NULL;
HINSTANCE hInstRC = NULL;

int FAR PASCAL LibMain(HINSTANCE hInstance, WORD /*wDataSeg*/,
  WORD /* cbHeapSize */, LPSTR lpCmdLine)
{
    int TheStatus;

    hInstRC = LoadLibrary( "_3dw_rc.dll" );
    if( !hInstRC )
    {
      MessageBox( 0, "Cann't load resource, need '_3dw_rc.dll'", "3DWorld 1.0a Control DLL",
		  MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);
      return 1;
    }


    pModule = new TModule("_3DW_CTL", hInstance, lpCmdLine);
    TheStatus = pModule->Status;
    if ( TheStatus != 0 )
    {
      delete pModule;
      pModule = NULL;
    }
    return (TheStatus == 0);
}

int FAR PASCAL WEP ( int /*bSystemExit*/ )
{
  FreeLibrary( hInstRC );
  return 1;
}
