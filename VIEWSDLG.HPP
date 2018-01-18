#if !defined(_VIEWS_DIALOG_)
#define _VIEWS_DIALOG_

#include <owl.h>
#include <bwcc.h>
#include <bgrpbox.h>
#include <bbutton.h>//TBButton
#include <bradio.h>


#include "INC\classtyp.inc"
#include "INC\dlg.inc"
#include "INC\viewsbmp.inc"
#include "centerdl.hpp"
#include "windiv.hpp"
#include "ucs.hpp"

#define ID_GRP_DIV   101
#define ID_GRP_VIEWS 102

/*#undef _EXPORT2
#if defined(_VIEWSDLGDLL) || defined(_ALLDLGDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif
  */

//extern PTWindowsObject GetObjectPtr( HWND );

BOOL CALLBACK _export DefDlgProcBWCC( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );
                                    
const int cViews = 9, cDivBtn = 12, cViewBtn = 9,
	  idDivisorButtonsFirst = 210;

_CLASSDEF(TMyRadioButton)
_CLASSDEF(TMyGroupBox)
_CLASSDEF(TViewDivisor)
_CLASSDEF(TDivButton)
_CLASSDEF(TViewButton)
_CLASSDEF(TViewsDlg)


_CLASSDEF(TViewDlgTransfer)
class  TViewDlgTransfer
 {
public:
  PTWindowDivision pWD;
  int iUseAsDefault;
  int iInitAlways;
 };

class  TViewDivisor: public TWindowDivision
 {
public:
  static LPSTR cVNames[cViewBtn];

  TViewDivisor( PTDialog pOwner, const RECT& RtWin,
    PTWindowDivision pWD );
  ~TViewDivisor();

  void SetView( int iView );
  void ResetDivision( TDivStyle Style );
  void GetViews( PTWindowDivision ) const;
  void Select( int index );
  TDivStyle GetStyle() const { return dsStyle; }
  int GetCheckInd() { return bIndexChecked; }

private:
  TDivStyle dsStyle;
  PTViewButton viewButtons[ MAXRECT ];
  PTDialog pOwner;
  int bIndexChecked;

  void CreateButtons();
  void DeleteButtons();
 };

class  TViewButton: public TButton
 {
public:
  TViewButton( PTWindowsObject AParent, RECT *pRect, PTViewDivisor pDivisor,
    int index, int id, int iVPoint, PTModule AModule = NULL ):
    TButton( AParent, id, TViewDivisor::cVNames[iVPoint],
      pRect->left, pRect->top, pRect->right-pRect->left,
      pRect->bottom-pRect->top, FALSE, AModule )
   {
     pDivisorOwn = pDivisor;
     this->index = index;
     bFlChecked = FALSE;
     Attr.Style = BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE |
       WS_TABSTOP | BS_OWNERDRAW;
   }

  void UnCheck();
  void Check();

protected:
  virtual void DefWndProc(RTMessage Msg);//TWindowsObject
  virtual void WMEraseBkGnd( RTMessage ) = [WM_FIRST + WM_ERASEBKGND];

private:
  PTViewDivisor pDivisorOwn;
  int index;
  BOOL bFlChecked;

 };


class  TMyGroupBox: public TBGroupBox
 {
public:
  TMyGroupBox( PTWindowsObject AParent, int AnId, LPSTR AText, int X, int Y,
    int W, int H, PTModule AModule = NULL ):
    TBGroupBox( AParent, AnId, AText, X, Y, W, H, AModule )
   {
   }
protected:
  /*virtual void DrawItem(DRAWITEMSTRUCT _FAR & DrawInfo);
  virtual void WMEraseBkGnd( RTMessage ) = [WM_FIRST + WM_ERASEBKGND];
  virtual void Paint(HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo);
    */
 };

class  TMyRadioButton: public TBRadioButton//TWindowsObject TWindow
 {
public:
  TMyRadioButton( PTWindowsObject AParent, int AnId, LPSTR ATitle, int X, int Y, int W, int H,
    PTGroupBox AGroup, PTModule AModule = NULL):
    TBRadioButton( AParent, AnId, ATitle, X, Y, W, H, AGroup, AModule )
   {
     //Attr.Style |= BS_OWNERDRAW;
/*     _fsetmem( hbmFace, sizeof(hbmFace), 0 );
     bFlFocus = FALSE;
     bFlMouse = FALSE;
     bFlInRect = FALSE;*/
     isSelected = FALSE;
   };

   BOOL isSelected;



protected:
  /*virtual void WMCommand( RTMessage Msg ) = [WM_FIRST + WM_COMMAND];
  virtual void ODADrawEntire( DRAWITEMSTRUCT _FAR & DrawInfo );
    */

  virtual void Paint( HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo );
  virtual void WMEraseBkGnd( RTMessage ) = [WM_FIRST + WM_ERASEBKGND];

  void WMLBDoubleClick( RTMessage ) = [WM_FIRST + WM_LBUTTONDBLCLK];
  

/*  void WMLButtonDown(RTMessage Msg) = [WM_FIRST + WM_LBUTTONDOWN];
  void WMLButtonUp(RTMessage Msg) = [WM_FIRST + WM_LBUTTONUP];
  void WMMouseMove(RTMessage Msg) = [WM_FIRST + WM_MOUSEMOVE];
  */


 };


class  TDivButton: public TBButton
 {
public:
  TDivButton( PTWindowsObject AParent, int AnId, LPSTR AText, int X, int Y,
	      int W, int H, BOOL IsDefault, PTModule AModule = NULL ):
    TBButton( AParent, AnId, AText, X, Y, W, H, IsDefault, AModule )
   {

   };

 };






struct TXlatElem
 {
   int idButton;
   int idView;
 };
typedef TXlatElem FAR *PTXlatElem;

struct TViewBtnDta
 {
   int id;
   char *chTtl;
 };

class  TViewsDlg: public TCenterDialog
 {
public:
  TViewsDlg( PTWindowsObject AParent, LPCSTR AName, PTViewDlgTransfer pTr,
    PTModule AModule = NULL );
  virtual ~TViewsDlg();
    
  void LockErase( BOOL bFlLock ) { bFlLockErase = bFlLock; }
  int GetEraseFlag() const { return bFlLockErase; }


  virtual classType isA() const;

protected:
  virtual void WMCommand(RTMessage Msg) = [WM_FIRST + WM_COMMAND];
  virtual void Ok( RTMessage msg ) = [ID_FIRST + IDOK];
  virtual void SetupWindow();
  virtual void GetWindowClass( WNDCLASS FAR & AWndClass );
  virtual LPSTR GetClassName();

  virtual void DrawItem(DRAWITEMSTRUCT _FAR & DrawInfo);

  //virtual void WMEraseBkGnd( RTMessage ) = [WM_FIRST + WM_ERASEBKGND];
  //virtual void DefWndProc(RTMessage Msg);//TWindowsObject

private:
  static TXlatElem iXlatTbl[ cViews ];
  static int iBmpDivBtn[ cDivBtn ][ 2 ];
  static int iIds[ cViewBtn + 2 ];
  static TViewBtnDta vbDta[ cViewBtn ];
  HBITMAP keyBmp[ cDivBtn ][ 2 ];
  PTMyRadioButton  keyBtnDiv[ cDivBtn ];

  PTViewDivisor pVinDivisor;

  int curView;
  //PTWindowDivision pWD;
  PTViewDlgTransfer pTr;

  HFONT hfHelv;
  //PTBGroupBox pGrpView;
  PTMyGroupBox pGrpView;
  PTMyGroupBox pGrpDiv;

  int bFlLockErase;

  PTXlatElem FindXlat( int code ) const;
  int GetViewXlat( int button ) const;
  int GetIdXlat( int view ) const;

  int GetTypeDiv( int idBmp ) const { return (idBmp >= VBMP_FIRST_H) ?
    (idBmp-VBMP_FIRST_H):(idBmp-VBMP_FIRST_N); }
  //void DrawBmp( HDC hdc, HBITMAP hbmp );

  BOOL FindId( int id );

  PTWindowsObject pBOk, pBCancel;
  PTWindowsObject pGr1, pGr2, pChb1, pChb2;
 };

#endif
