#if !defined(_AB_DIALOG_)
#define _AB_DIALOG_

#include <owl.h>
#include "bmp.hpp"
#include "centerdl.hpp"
#include "status.hpp"
#include "autorc.hpp"

#include "INC\errors.inc"

#undef _EXPORT2
#if defined(_ABDIALOGDLL) || defined(_ALLDLGDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif



_CLASSDEF(TAboutDialog)

class _EXPORT2 TAboutDialog: public TCenterDialog

 {

public:                        
  TAboutDialog( PTWindowsObject AParent, LPSTR AName, int id256,
		BOOL bCompress, PTModule AModule = NULL );

  virtual ~TAboutDialog();

  virtual void DefWndProc( RTMessage );
     
protected:
  //virtual void GetWindowClass(WNDCLASS _FAR & AWndClass);
  //virtual LPSTR GetClassName();

  virtual void SetupWindow();
  virtual void DefCommandProc( RTMessage );

  virtual void WMQueryNewPalette( RTMessage ) = [WM_FIRST + WM_QUERYNEWPALETTE];
  virtual void WMPaletteChanged( RTMessage ) = [WM_FIRST + WM_PALETTECHANGED];
                                 
                               
private:
  struct TBitmapContain
   {
     TAutoGDIRc<HBITMAP> grcN, grcS, grcH;
   };

  TBitMap  *AboutBmp;

  TBitmapContain grcBtnBmp[ 1 ];
  TAutoMemoryRc<PTDIB> mrcImg;
 };


#endif
