#if !defined(_PHOTO_DLG_)
#define _PHOTO_DLG_

#include <owl.h>

#include "phtransf.hpp"
#include "centerdl.hpp"
#include "INC\phdlg.inc"

#undef _EXPORT2
#if defined(_PHDLGDLL) || defined(_ALLDLGDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif


_CLASSDEF(TPhotoDlg)

const int iNumBtn =  2;
const int iNumbCheck = 3;
const int iNumbRadio = 6;

class _EXPORT2 TPhotoDlg: public TCenterDialog
 {
public:
  TPhotoDlg( PTWindowsObject AParent, LPSTR AName,
	     RTPhotoSetupTransfer rTransf,
             PTModule AModule = NULL );

  virtual ~TPhotoDlg();

  virtual HPALETTE Palette() const { return NULL; }
  virtual HWND PaletteUser() const { return NULL; }

protected:
  virtual void SetupWindow();
  virtual void WMCommand( RTMessage Msg ) = [WM_FIRST + WM_COMMAND];
  void WMUserValueValidated( RTMessage ) = [WM_FIRST + WM_USER_VALUE_VALIDATED];
  virtual void Ok( RTMessage ) = [ID_FIRST + IDOK];

private: 
  RTPhotoSetupTransfer rDta;

  static int aiBtnAlias[ iNumBtn ];
  static int aiCheckAlias[ iNumbCheck ];
  static int aiRadioAlias[ iNumbRadio ];

 };



#endif
