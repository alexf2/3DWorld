#if !defined(_OMNI_DLG_)
#define _OMNI_DLG_

#include <owl.h>
#include <dialog.h>
#include <button.h>
#include <bgrpbox.h>
#include <string.h>

#include "centerdl.hpp"
#include "INC\omnidlg.inc"

#undef _EXPORT2
#if defined(_OMNIDLGDLL) || defined(_ALLDLGDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif


_CLASSDEF(TOmniDialog)

class _EXPORT2 TOmniDialog: public TCenterDialog
 {
public:
  TOmniDialog( PTWindowsObject AParent, LPSTR AName,
	       RGBQUAD& rRGBOmni, double& rdOmniMult_,
	       BOOL& bFlOn,
	       BOOL& bFlCastShadows,
	       LPSTR lpNameOmni, 
	       PTModule AModule = NULL );

  virtual ~TOmniDialog();

  virtual HPALETTE Palette() const { return hpalPalette; }
  virtual HWND PaletteUser() const { return hwndMix; }

protected:
  virtual void SetupWindow();

  virtual void WMCommand( RTMessage Msg ) = [WM_FIRST + WM_COMMAND];

  virtual void WMHScroll( RTMessage Msg ) = [WM_FIRST + WM_HSCROLL];
  void WMUserValueValidated( RTMessage ) = [WM_FIRST + WM_USER_VALUE_VALIDATED];

  virtual void Ok( RTMessage ) = [ID_FIRST + IDOK];

private:
  double& rdOmniMult;
  RGBQUAD& rRGBOmni;
  BOOL& bFlOn;
  BOOL& bFlCastShadows;
  String strName;

  HPALETTE hpalPalette;
  PTAVScrollConnectedMixer psbRGBr, psbRGBg, psbRGBb;
  HWND hwndMix;  

  PTMixerConnected pMix; 

 };


#endif
