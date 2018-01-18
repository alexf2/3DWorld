#include <owl.h>

PTModule pModule = NULL;

int FAR PASCAL LibMain(HINSTANCE hInstance, WORD /*wDataSeg*/,
  WORD /* cbHeapSize */, LPSTR lpCmdLine)
{
    int TheStatus;

    pModule = new TModule("_3DW_RC", hInstance, lpCmdLine);
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
  return 1;
}
