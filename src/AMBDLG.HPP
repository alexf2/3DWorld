#if !defined(_AMBIENT_DLG_)
#define _AMBIENT_DLG_

#include <owl.h>
#include <dialog.h>
#include <button.h>
#include <bgrpbox.h>

#include "centerdl.hpp"
#include "INC\ambdlg.inc"

#undef _EXPORT2
#if defined(_AMBDLGDLL) || defined(_ALLDLGDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif


_CLASSDEF(TAmbientDialog)

class _EXPORT2 TAmbientDialog: public TCenterDialog
 {
public:
  TAmbientDialog( PTWindowsObject AParent, LPSTR AName,
		   RGBQUAD& rRGBAmbient, double& rdAmbMult_,
		   PTModule AModule = NULL );

  virtual ~TAmbientDialog();

  virtual HPALETTE Palette() const { return hpalPalette; }
  virtual HWND PaletteUser() const { return hwndMix; }

protected:
  virtual void SetupWindow();

  virtual void WMCommand( RTMessage Msg ) = [WM_FIRST + WM_COMMAND];

  virtual void WMHScroll( RTMessage Msg ) = [WM_FIRST + WM_HSCROLL];
  void WMUserValueValidated( RTMessage ) = [WM_FIRST + WM_USER_VALUE_VALIDATED];


private:
  double& rdAmbMult;
  RGBQUAD& rRGBAmb;
  HPALETTE hpalPalette;
  PTAVScrollConnectedMixer psbRGBr, psbRGBg, psbRGBb;
  HWND hwndMix;

  PTMixerConnected pMix; 

  //void SetMix();
  //void UpdateTiledScrollbar( int id );
 };


#endif
