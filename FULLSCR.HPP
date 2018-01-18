#if !defined(_FULLSCR_WINDOW_)
#define  _FULLSCR_WINDOW_

#include <owl.h>
#include <window.h>
#include "bmp.hpp"
//#include "rgb.hpp"

#undef _EXPORT2
#if defined(_FULLSCRDLL) || defined(_ALLDLGDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif

#define WM_USER_FULLSCRDONE (WM_USER + 5)

_CLASSDEF(TFullScrWindow)
_CLASSDEF(TSysPaletteSave)

const int iNumbEnt = 21;

class _EXPORT2 TSysPaletteSave
 {
public:
  TSysPaletteSave() { hdc = NULL; }
  ~TSysPaletteSave();

  void operator()( HDC );
  void operator~();

  void Reset();

private:
  static int iIndexPal[ iNumbEnt ];
  static COLORREF rgb[ iNumbEnt ];
  HDC hdc;
  int iNumColors;
 };

class _EXPORT2 TFullScrWindow: public TWindow
 {
public:
  TFullScrWindow( PTWindowsObject AParent, LPSTR ATitle, PTDIB  pbmpDIB_, HPALETTE hpalAlias_, PTModule AModule = NULL);
  virtual ~TFullScrWindow();

  virtual void Destroy();

protected:
  virtual LPSTR GetClassName();      //TMDIFrame
  virtual void GetWindowClass( WNDCLASS & AWndClass );
  //virtual void SetupWindow();
  virtual BOOL Create();

  void WMEraseBkGnd( RTMessage ) = [WM_FIRST + WM_ERASEBKGND];
  virtual void Paint( HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo );

  //void WMChar( RTMessage ) = [WM_FIRST + WM_CHAR];
  //void WMDeadChar( RTMessage ) = [WM_FIRST + WM_DEADCHAR];
  void WMKeyDown( RTMessage ) = [WM_FIRST + WM_KEYDOWN];
  void WMSysKeyDown( RTMessage ) = [WM_FIRST + WM_SYSKEYDOWN];
  void WMMouseMove( RTMessage msg ) = [WM_FIRST + WM_MOUSEMOVE];
  void WMLButtonDown( RTMessage ) = [WM_FIRST + WM_LBUTTONDOWN];
  void WMRButtonUp( RTMessage ) = [WM_FIRST + WM_RBUTTONUP];
  void WMLButtonUp( RTMessage ) = [WM_FIRST + WM_LBUTTONUP];

  void WMSetCursor( RTMessage ) = [WM_FIRST + WM_SETCURSOR];

  void WMQueryNewPalette( RTMessage ) = [WM_FIRST + WM_QUERYNEWPALETTE];
  void WMPaletteChanged( RTMessage ) = [WM_FIRST + WM_PALETTECHANGED];

private:
  int iBmpWidth, iBmpHigh;
  int iBmpViewWidth, iBmpViewHigh;
  PTDIB  pbmpDIB;
  HPALETTE hpalAlias;
  HCURSOR hcHand;
  HDC hdc;
  BOOL bFlTrack;
  int iStaOrgX, iStaOrgY;
  RECT r;

  TSysPaletteSave spsSysPalette;

  static int iMouseX, iMouseY;

  BOOL bFlOldDisable;

  void PrepareKey( RTMessage msg );
 };

#endif
