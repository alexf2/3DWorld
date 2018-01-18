#pragma option -v-

#include "measur.mac"

#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif


int TMeasur::Rx( const int  xInDialogUnits ) const
 {
   return (xInDialogUnits * widDlg) / 4;
 }

int TMeasur::Ry( const int  yInDialogUnits ) const
 {
   return (yInDialogUnits * highDlg) / 8;
 }
