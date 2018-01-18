#if !defined(_S_NO_CANCEL_DLG_)
#define _S_NO_CANCEL_DLG_

#include <owl.h>
#include <dialog.h>
#include <bbutton.h>
#include <windowsx.h>
#include <bwcc.h>

#include "INC\dlg.inc"
#include "centerdl.hpp"
#include "bmp.hpp"

#include "measur.mac"

#undef _EXPORT2
#if defined(_YNCDLL) || defined(_ALLDLGDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif


_CLASSDEF(TYNCDialog)
_CLASSDEF(T256Button)


class _EXPORT2 T256Button: public TButton
 {
public:
  T256Button( PTDIB *pDIB, HPALETTE hp, PTWindowsObject AParent,
    int AnId, LPSTR AText, int X, int Y, int W, int H,
    BOOL IsDefault, PTModule AModule = NULL ):
    TButton( AParent, AnId, AText, X, Y, W, H, IsDefault, AModule )
   {
     pButBmp = pDIB;
     hpalDIBBtn = hp;
     bFlFocus = FALSE;
     bFlMouse = FALSE;
     bFlInRect = FALSE;
   }
  virtual void WMDrawItem(RTMessage Msg) = [WM_FIRST + WM_DRAWITEM];

protected:
  virtual void ODADrawEntire( DRAWITEMSTRUCT _FAR & DrawInfo );
  virtual void Paint( HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo );
  virtual void SetupWindow();
  /*virtual void ODAFocus( DRAWITEMSTRUCT _FAR & DrawInfo );
  virtual void ODASelect( DRAWITEMSTRUCT _FAR & DrawInfo );*/
  void DrawBmp( HDC hdc );
  virtual void WMEraseBkGnd( RTMessage ) = [WM_FIRST + WM_ERASEBKGND];

  void WMLButtonDown(RTMessage Msg) = [WM_FIRST + WM_LBUTTONDOWN];
  void WMLButtonUp(RTMessage Msg) = [WM_FIRST + WM_LBUTTONUP];
  void WMMouseMove(RTMessage Msg) = [WM_FIRST + WM_MOUSEMOVE];

private:
  PTDIB *pButBmp;
  HPALETTE  hpalDIBBtn;
  PTDIB pDrawBmp;
  BOOL bFlFocus;

  BOOL bFlMouse;
  BOOL bFlInRect;

 };

class _EXPORT2 TYNCDialog: public TCenterDialog//TDialog
 {
public:

   TYNCDialog( PTWindowsObject AParent, int Resource, int* res,
	       LPSTR lpMsg, int id256_1, BOOL bFlCompress, PTModule AModule = NULL );

   virtual ~TYNCDialog();

   virtual void SetupWindow();

protected:
  //virtual void Cancel(RTMessage Msg) = [ID_FIRST + IDCANCEL];
  virtual void WMCommand(RTMessage Msg) = [WM_FIRST + WM_COMMAND];
  virtual void WMCreate(RTMessage Msg) = [WM_FIRST + WM_CREATE];


private:
  int*  pRes;
  HWND  hwndNoBut;
  HFONT hf;
  PTDIB     pDIBNorm, pDIBPress, pDIBSel;
  PTDIB  pArr[3];
  HPALETTE  hpalDIBBtn;
  PT256Button   pButBmp;
  //PTButton   pButBmp;


  PTWindowsObject pOut, pTxt; 
 };


#endif

