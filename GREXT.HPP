#if !defined(_GREED_EXTENTS_)
#define _GREED_EXTENTS_

#include <bwcc.h>

#include "INC\dlg.inc"
#include "centerdl.hpp"
#include "ucs.hpp"

#undef _EXPORT2
#if defined(_GREXTDLL) || defined(_ALLDLGDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif


_CLASSDEF(TGrExtentsDlg)


class  _EXPORT2 TGrExtentsDlg: public TCenterDialog
 {
public:
  TGrExtentsDlg( PTWindowsObject AParent, LPCSTR AName,
    PTGrid pGrid, PTModule AModule = NULL ):
    TCenterDialog( AParent, (LPSTR)AName, AModule )
   {
     this->pGrid = pGrid;
   }

protected:
  virtual void SetupWindow();
  virtual void Ok(RTMessage Msg) = [ID_FIRST + IDOK];
     
private:
  PTGrid pGrid;

 };

#endif
