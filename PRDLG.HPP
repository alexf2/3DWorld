#if !defined(_PRENDER_DLG_)
#define _PRENDER_DLG_

#include <owl.h>
#include <dialog.h>
#include <button.h>
#include <bgrpbox.h>

#include "centerdl.hpp"
#include "INC\prdlg.inc"

#undef _EXPORT2
#if defined(_PRDLGDLL) || defined(_ALLDLGDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif


struct TPRenderDialogTransfer
 {
   RGBQUAD rgbAdges;
   RGBQUAD rgbIntersect;
   int     polyMethod;
   BOOL    bFlMarkIntersect;
 };
typedef TPRenderDialogTransfer* PTPRenderDialogTransfer;
typedef TPRenderDialogTransfer& RTPRenderDialogTransfer;
//_PTRDEF(TPRenderDialogTransfer)
//_REFDEF(TPRenderDialogTransfer)

_CLASSDEF(TPRenderDialog)

class _EXPORT2 TPRenderDialog: public TCenterDialog
 {
public:
  TPRenderDialog( PTWindowsObject AParent, LPSTR AName,
		  RTPRenderDialogTransfer rTransfBuf, PTModule AModule = NULL );

  virtual ~TPRenderDialog();

  virtual HPALETTE Palette() const { return hpalPalette; }
  virtual HWND PaletteUser() const { return hwndMixAd; }

protected:
  virtual void SetupWindow();
  virtual void Ok(RTMessage Msg) = [ID_FIRST + IDOK];

  virtual void WMHScroll( RTMessage Msg ) = [WM_FIRST + WM_HSCROLL];
  void WMUserValueValidated( RTMessage ) = [WM_FIRST + WM_USER_VALUE_VALIDATED];
  virtual void WMCommand( RTMessage Msg ) = [WM_FIRST + WM_COMMAND];


private:
  RTPRenderDialogTransfer rD;

  HPALETTE hpalPalette;
  HWND hwndMixAd;
  PTMixerConnected pMixAd;
  HWND hwndMixIn;
  PTMixerConnected pMixIn;
  PTAVScrollConnectedMixer psbAdRGBr, psbAdRGBg, psbAdRGBb,
			   psbInRGBr, psbInRGBg, psbInRGBb;

  void SetIntersect();
 };


#endif
