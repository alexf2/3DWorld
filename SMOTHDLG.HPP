#if !defined(_SMOTH_DLG_)
#define _SMOTH_DLG_

#include <owl.h>
#include <dialog.h>
#include <button.h>
#include <bgrpbox.h>

#include "centerdl.hpp"
#include "INC\smothdlg.inc"

#undef _EXPORT2
#if defined(_SMOTHDLGDLL) || defined(_ALLDLGDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif


_CLASSDEF(TSmothDialog)

class _EXPORT2 TSmothDialog: public TCenterDialog
 {
public:

  TSmothDialog( PTWindowsObject AParent, LPSTR AName,
		int& dAngleSmoth, int& iSmothGrp, int& iIdBtn,
	        PTModule AModule = NULL );

  //virtual ~TSmothDialog();

  /*virtual HPALETTE Palette() const { return hpalPalette; }
  virtual HWND PaletteUser() const { return hwndMix; }*/

protected:
  virtual void SetupWindow();

  virtual void WMCommand( RTMessage Msg ) = [WM_FIRST + WM_COMMAND];

  //virtual void WMHScroll( RTMessage Msg ) = [WM_FIRST + WM_HSCROLL];
  //void WMUserValueValidated( RTMessage ) = [WM_FIRST + WM_USER_VALUE_VALIDATED];


private:
  int& iAngleSmoth;
  int& iSmothGrp;
  int& iIdBtn;
 };


#endif

