#if !defined(_GLOBAL_BASEPOINT_DLG_)
#define _GLOBAL_BASEPOINT_DLG_

#include <owl.h>
#include <dialog.h>
#include <button.h>//TButton
#include <windowsx.h>
#include <bgrpbox.h>

#include "centerdl.hpp"
#include "INC\bpdlgbmp.inc"

#define ID_FIRST_BPDLG 10

#define ID_POINT  (ID_FIRST_BPDLG + 1)
#define ID_VERTEX (ID_FIRST_BPDLG + 2)
#define ID_BODY   (ID_FIRST_BPDLG + 3)
#define ID_CANCEL (ID_FIRST_BPDLG + 4)

#undef _EXPORT2
#if defined(_BPDLGDLL) || defined(_ALLDLGDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif


enum TPlaicement { PL_TopLeft = (ID_FIRST_BPDLG + 5), PL_Top, PL_TopRight,
		   PL_Left, PL_Center, PL_Right, PL_BottomLeft, PL_Bottom,
		   PL_BottomRight
                 };

typedef TPlaicement _FAR &RTPlaicement;
typedef TPlaicement _FAR *PTPlaicement;


_CLASSDEF(TBPDialog)
_CLASSDEF(TMainGroup)


class _EXPORT2 TMainGroup: public TBGroupBox
 {
public:
  TMainGroup( PTWindowsObject AParent, int AnId, LPSTR ATitle, int X, int Y, int
    W, int H, RTPlaicement rPlace, PTModule AModule = NULL ):

      TBGroupBox( AParent, AnId, NULL, X, Y, W, H, AModule ),
      pPlace( &rPlace )
   {
   }

  virtual void DrawItem( DRAWITEMSTRUCT _FAR & DrawInfo );

protected:
  virtual void WMCommand( RTMessage msg ) = [WM_FIRST + WM_COMMAND];


private:
  PTPlaicement pPlace;
 };

class _EXPORT2 TBPDialog: public TCenterDialog
 {
public:

  TBPDialog( PTWindowsObject AParent, LPSTR AName, RTPlaicement rPlace, PTModule AModule = NULL );
  virtual ~TBPDialog();

  virtual void DrawItem( DRAWITEMSTRUCT _FAR & DrawInfo );

protected:
  virtual void SetupWindow();
  virtual void WMCommand( RTMessage msg ) = [WM_FIRST + WM_COMMAND];

private:
  typedef struct {
    HBITMAP hbmN, hbmH, hbmS;
   } TBtnBmp;

  TBtnBmp bbControlBmp[ BPDLG_NUMB_CONTROL_BMP ];
  TBtnBmp bbCheckBmp[ BPDLG_NUMB_CHECK_BMP ];

  HFONT hf;

  PTMainGroup pMGr;

  PTPlaicement pPlace;

  void DrawFrame( HWND hwndOwner, RECT _FAR &rRect, BOOL bFlDraw );

 };

#endif

