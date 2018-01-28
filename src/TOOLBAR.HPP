#if! defined(_TOOLBAR_WINDOW_)
#define _TOOLBAR_WINDOW_

#include <owl.h>
#include <bwcc.h>
#include <bwindow.h>
#include <bbutton.h>
#include <dbllist.h> //DoubleList
#include "INC\classtyp.inc"

#undef _EXPORT2
#if defined(_TOOLBARDLL) || defined(_ALLCTLDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif


#define TYP_BUTTON_INIT 120

const int idFirstBtn = 200;

BOOL CursorInWindow( HWND hwnd, POINT& poCurs );

_CLASSDEF(TToolBarWindow)
_CLASSDEF(TButtonInit)
_CLASSDEF(TToolButton)
_CLASSDEF(THintWindow)


enum TStrategy { TS_Default = 0, TS_Selectable, TS_IndependSwt };

class _EXPORT2 THintWindow: public TWindow
 {
public:
  THintWindow( PTWindowsObject AParent, UINT idHint, HFONT hf, PTModule AModule = NULL );

  virtual ~THintWindow();

  virtual void DefWndProc( RTMessage Msg );

protected:
  virtual void GetWindowClass( WNDCLASS _FAR & AWndClass );
  virtual LPSTR GetClassName();
  virtual void Paint( HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo );
  virtual void SetupWindow();


private:
  static const int iLenBuf;
  LPSTR lpHint;
  HFONT hf;

 };

class _EXPORT2 TToolBarWindow: public TBWindow//
 {
public:  

  TToolBarWindow( PTWindowsObject AParent, LPSTR ATitle, DoubleList *plistBmp,
    int str, int clm, HFONT hf,
    PTModule AModule = NULL );
 
  //~TToolBarWindow();

  virtual classType isA() const
   {
     return classtypeToolbarWindow;
   }

  virtual void DrawItem( DRAWITEMSTRUCT _FAR & DrawInfo );

  void Check( int iCom );
  void SpecialCheck( int iCom, BOOL bFlCh );
  TStrategy GetStrategyItem( int iCom );
  void Reset();

  BOOL IsVisible() const { return isVisible; }
  void SetVisible( BOOL fl ) { isVisible = fl; UpdateState(); }
  void UpdateState();

  BOOL QueryHint() const;
  void StopAndClearWaitCount() { ulClockForHint = 0; }
  void StartWaitCount();

  HFONT GetFont() const { return hf; }
  BOOL  GetLock() const { return bIsLocked; }
  void  SetLock( BOOL bL ); 

  void WMNCHittest();

  void SetHandle( HWND hwnd ) { hHandleCurHint = hwnd; }

protected:
  //virtual LPSTR GetClassName();
  virtual void GetWindowClass( WNDCLASS _FAR & AWndClass );
  virtual void SetupWindow();
  virtual void WMCommand( RTMessage msg ) = [WM_FIRST + WM_COMMAND];
  virtual void WMDestroy( RTMessage msg ) = [WM_FIRST + WM_DESTROY];
  virtual void WMCreate( RTMessage msg ) = [WM_FIRST + WM_CREATE];
  virtual void DefWndProc( RTMessage Msg );

  void WMTimer( RTMessage ) = [WM_FIRST + WM_TIMER];

  void WMKillFocus( RTMessage ) = [WM_FIRST + WM_KILLFOCUS];

private:
  DoubleList *plistBmp; //Don't delete
  BITMAP  bmpInf;
  int iCol, iStr;

  BOOL isVisible;

  static const unsigned long clockForHintDelay;
  unsigned long ulClockForHint;
  UINT  idTimer;

  HFONT hf;

  BOOL bIsLocked;

  HWND hHandleCurHint;
 };


class _EXPORT2 TButtonInit: public Object
 {
public:
   int Status;
   BOOL  bFlRedraw;

   TButtonInit( int resIdNorm, int resIdPress, int iCommand, HINSTANCE hInst,
     TStrategy tsStrat, UINT idOfHint );
   ~TButtonInit();

   virtual classType isA() const { return TYP_BUTTON_INIT; }
   virtual hashValueType hashValue() const { return 0; }
   virtual int isEqual( const Object& ) const;
   virtual char *nameOf() const { return "ButtonInit"; }
   virtual void printOn( ostream& ) const {};

   HBITMAP GetHandleN() const { return hbmN; }
   HBITMAP GetHandleH() const { return hbmH; }
   int GetCommand() const { return iCommand; }

   void Check( BOOL fl );
   BOOL GetCheck() const { return isChecked; }
   TStrategy GetStrategy() const { return tsStrategy; }

   UINT GetHintId() const { return idOfHint; }

   void _FAR* operator new( size_t size );
   void operator delete( void _FAR* pToDel );

private:
  int iCommand;
  HBITMAP hbmN, hbmH;
  HINSTANCE hInst;

  TStrategy  tsStrategy;
  BOOL  isChecked;


  UINT idOfHint;  

  static MemBlocks *mgr;
  static long countMemMgr;

 };


class _EXPORT2 TToolButton: public TBButton//TWindowsObject
 {                                        //TControl
friend class TToolBarWindow;

public:
  TToolButton( PTWindowsObject AParent, int AnId, LPSTR AText, int X, int Y,
    int W, int H, BOOL IsDefault, UINT idOfHint, PTModule AModule = NULL ):
    TBButton( AParent, AnId, AText, X, Y, W, H, IsDefault, AModule )
   {
     flFocus = FALSE;
     pHintWnd = NULL;
     this->idOfHint = idOfHint;
   }

  void WMTimer();
  void KillHint();

  virtual classType isA() const
   {
     return classtypeToolbarButton;
   }


protected:
  //virtual LPSTR GetClassName();
  BOOL flFocus;

  virtual void GetWindowClass( WNDCLASS _FAR & AWndClass );
  virtual void WMEraseBkGnd( RTMessage ) = [WM_FIRST + WM_ERASEBKGND];

  void WMSetFocus( RTMessage ) = [WM_FIRST + WM_SETFOCUS];
  void WMKillFocus( RTMessage ) = [WM_FIRST + WM_KILLFOCUS];

  void WMMouseMove( RTMessage ) = [WM_FIRST + WM_MOUSEMOVE];
  //void WMNCHittest( RTMessage ) = [WM_FIRST + WM_NCHITTEST];
  virtual void WMLButtonDown( RTMessage Msg ) = [WM_FIRST + WM_LBUTTONDOWN];
  void WMLButtonUp( RTMessage Msg ) = [WM_FIRST + WM_LBUTTONUP];

private:
  PTHintWindow  pHintWnd;
  UINT idOfHint;

  void MakeHint();

 };


#endif
