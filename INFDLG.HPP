#if !defined(_INFO_DIALOG_)
#define _INFO_DIALOG_

#include <list.h>

#include "centerdl.hpp"
//#include "mymdich.hpp"

_CLASSDEF(TInfDialog)

#undef _EXPORT2
#if defined(_INFDLGDLL) || defined(_ALLDLGDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif


class _EXPORT2 TInfDialog: public TCenterDialog
 {
public:
  TInfDialog( PTWindowsObject AParent, LPSTR AName,
    List& rList_, PTModule AModule = NULL );

  virtual ~TInfDialog();

protected:
  virtual void SetupWindow();
  virtual void DefCommandProc( RTMessage );

private:
  //PTMyMDIChild pCh;
  HFONT        hf;

  List& rList;
 };


#endif

