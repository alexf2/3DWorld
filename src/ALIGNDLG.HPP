#if !defined(_ALIGNDLG_)
#define _ALIGNDLG_

#include <owl.h>
#include <dialog.h>
#include <button.h>
#include <bgrpbox.h>

#include "centerdl.hpp"
#include "INC\aligndlg.inc"

#undef _EXPORT2
#if defined(_ALIGNDLGDLL) || defined(_ALLDLGDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif


_CLASSDEF(TAligmentDialog)

class _EXPORT2 TAligmentDialog: public TCenterDialog
 {
public:
  TAligmentDialog( PTWindowsObject AParent, LPSTR AName,
		   BOOL _FAR &rbForwardFlag, PTModule AModule = NULL );

  virtual void ShutDownWindow( int ARetValue );

protected:
  virtual void SetupWindow();
  void AligmentOnly( RTMessage ) = [ID_FIRST + ALIGNDLG_IDALIGMENT_ONLY];
  void AligmentAndConnect( RTMessage ) = [ID_FIRST + ALIGNDLG_IDALIGMENT_AND_CONNECT];

private:
  BOOL _FAR &rbForwardFl;

 };

#endif
