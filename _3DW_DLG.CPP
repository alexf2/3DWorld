/*#include "ync.hpp"
#include "centerdl.hpp"
#include "filedial.hpp"
#include "abdialog.hpp"
#include "aligndlg.hpp"
#include "ambdlg.hpp"
#include "bpdlg.hpp"
#include "infdlg.hpp"
#include "facedial.hpp"
#include "medium.hpp"
#include "autoctl.hpp"
#include "grext.hpp"*/
#include <owl.h>

PTModule pModule = NULL;
HINSTANCE hInstRC = NULL;

PTModule pModuleRc = NULL;

int FAR PASCAL LibMain(HINSTANCE hInstance, WORD /*wDataSeg*/,
  WORD /* cbHeapSize */, LPSTR lpCmdLine)
{
    int TheStatus;

    hInstRC = LoadLibrary( "_3dw_rc.dll" );
    if( !hInstRC )
    {
      MessageBox( 0, "Cann't load resource, need '_3dw_rc.dll'", "3DWorld 1.0a Dialog DLL",
		  MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);
      return 1;
    }

    pModuleRc = new TModule( "_3DW_RC", hInstRC, NULL );

    pModule = new TModule("_3DW_DLG", hInstance, lpCmdLine);
    TheStatus = pModule->Status;
    if( TheStatus != 0 || !pModuleRc || pModuleRc->Status != 0 )
    {
      delete pModule;
      if( pModuleRc ) { delete pModuleRc; pModuleRc = NULL; }
      pModule = NULL;
    }
    return (TheStatus == 0);
}

int FAR PASCAL WEP ( int /*bSystemExit*/ )
{
  FreeLibrary( hInstRC );
  if( pModuleRc ) delete pModuleRc;
  return 1;
}
