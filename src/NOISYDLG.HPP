#if !defined(_NOISY_DLG_)
#define _NOISY_DLG_

#undef _EXPORT2
#if defined(_NOISYDLGDLL) || defined(_ALLDLGDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif

#include "centerdl.hpp"
#include "rgb.hpp"

_CLASSDEF(TNoisyDialog)

class _EXPORT2 TNoisyDialog: public TCenterDialog
 {
public:
  TNoisyDialog( PTWindowsObject AParent, LPSTR AName,
		PTNoisyDta pnDta,
                PTModule AModule = NULL );

  virtual ~TNoisyDialog() {};
  
protected:
  virtual void SetupWindow();
  virtual void WMCommand( RTMessage ) = [WM_FIRST + WM_COMMAND]; 
  virtual void WMHScroll( RTMessage Msg ) = [WM_FIRST + WM_HSCROLL];
  void WMUserValueValidated( RTMessage ) = [WM_FIRST + WM_USER_VALUE_VALIDATED];
  virtual void Ok( RTMessage ) = [ID_FIRST + IDOK];


private:
  RTNoisyDta rnDta;

 };


#endif
