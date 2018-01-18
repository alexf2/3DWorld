#if !defined(_HILITE_DLG_)
#define _HILITE_DLG_

#include <owl.h>
#include <dialog.h>
#include <button.h>
#include <bgrpbox.h>
#include <string.h>

#include "centerdl.hpp"
#include "INC\omnidlg.inc"
#include "autorc.hpp"

#undef _EXPORT2
#if defined(_HILITEDLL) || defined(_ALLDLGDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif


_CLASSDEF(TPlaceHiliteDlg)

class _EXPORT2 TPlaceHiliteDlg: public TCenterDialog
 {
public:
  TPlaceHiliteDlg( PTWindowsObject AParent, LPSTR AName,
		   int& riDst, LPSTR lpTtl,
		   PTModule AModule = NULL );
  virtual ~TPlaceHiliteDlg();

  virtual HPALETTE Palette() const { return NULL; }
  virtual HWND PaletteUser() const { return NULL; }

  virtual BOOL CanClose();
  virtual void DrawItem( DRAWITEMSTRUCT _FAR & DrawInfo );

protected:
  virtual void SetupWindow();

  virtual void WMCommand( RTMessage Msg ) = [WM_FIRST + WM_COMMAND];  
  virtual void Ok( RTMessage ) = [ID_FIRST + IDOK];  
  
private:
  int& riDistance;
  String  strTtl;

  TAutoGDIRc<HBITMAP> hbmHint;
 };


#endif
