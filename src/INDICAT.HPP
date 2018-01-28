#if !defined(_TINDICATOR_)
#define _TINDICATOR_

#include <owl.h>
#include <bwindow.h>
#include <dialog.h>
#include <bbutton.h>
#include <control.h>
#include <dbllist.h>

//#include "centerdl.hpp"
#include "ind_dbl.hpp"
#include "drawbuf.hpp"
#include "frame3d.hpp"

#define TYP_STRING 110

#undef _EXPORT2
#if defined(_INDICATDLL) || defined(_ALLCTLDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif


//_CLASSDEF(TIndicator)  in "ind_dbl.hpp"
_CLASSDEF(TStr)
_CLASSDEF(TProgress)



class _EXPORT2 TStr: public TIndexedItem
 {

public:
  LPSTR lpTxt;

  TStr():
    TIndexedItem( -1 )
    { lpTxt = NULL; }

  TStr( int indAssign , LPCSTR lpStr );

  ~TStr();

  virtual classType isA() const { return TYP_STRING; }
  virtual hashValueType hashValue() const { return 0; }
  //virtual int isEqual( const Object& ) const;
  virtual char *nameOf() const { return "TString"; }
  virtual void printOn( ostream& ) const {};


  void Update( LPCSTR );
 };


class _EXPORT2 TIndicator: public TDialog//TWindow
 {
public:

   TIndicator( PTWindowsObject AParent,
     LPCSTR lpTitle,
     LONG  ulWorkSize,
     PTModule AModule = NULL );

  ~TIndicator();

  void  AddStr( LPCSTR );
  BOOL  DeleteStr( int ind );
  void  AdaptBounds();
  BOOL  UpdateStr( int ind, LPCSTR lpNewStr );
  void  Redraw() { Update(); }

  void  SetTitle( LPCSTR );

  BOOL  WorkMessage( LONG ulCurSize );
  BOOL  WorkMessageAdd( LONG ulWorkingSize );
  void  WorkIs100();

  void Init( LONG size );

  int  GetPercent() const; 

  void Show();
  void Close();

  void SetStatusButton( BOOL );

  virtual void ShutDownWindow();
  virtual BOOL CanClose();

  BOOL GetProcessCode() const { return bFlRet; }

  virtual void DefWndProc( RTMessage Msg );
  //virtual void DrawItem( DRAWITEMSTRUCT _FAR& DrawInfo );

protected:
  virtual void WMPaint( RTMessage Msg) = [WM_FIRST + WM_PAINT];
  virtual void WMCommand( RTMessage Msg ) = [WM_FIRST + WM_COMMAND];
  virtual void WMSysCommand( RTMessage Msg ) = [WM_FIRST + WM_SYSCOMMAND];
  virtual void GetWindowClass(WNDCLASS & AWndClass);
  virtual LPSTR GetClassName();
  virtual void WMEraseBkGnd( RTMessage ) = [WM_FIRST + WM_ERASEBKGND];

  virtual void SetupWindow();
  //virtual void ODADrawEntire( DRAWITEMSTRUCT _FAR& DrawInfo );

private:
  TIndexedDoubleList  dblTxt;
  TStr        strTtl;   
  LONG       ulWorkFull, ulWorkMaking;
  LONG       ulStepRefresh;
  BOOL        bFlRet;

  BOOL        bFlEnableClose;

  PTBButton   pbtnCancel;
  PTProgress  pstProgress;

  LONG  lDelta;

  PT3DFrame pFrame;

  void Update();
  void SetBounds();
 };

class _EXPORT2 TProgress: public TControl
 {
public:
  TProgress( PTWindowsObject AParent, int AnId, LPSTR ATitle, int X, int Y, int W, int H, PTModule
             ATModule = NULL ):
    TControl( AParent, AnId, ATitle, X, Y, W, H, ATModule )
   {
   }


protected:
  virtual void WMEraseBkGnd( RTMessage ) = [WM_FIRST + WM_ERASEBKGND];

  virtual void Paint(HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo);

private:
  static TDrawTitleBuf dtbBuf;

 };


inline BOOL  TIndicator::WorkMessageAdd( LONG ulWorkingSize )
 {
   return WorkMessage( ulWorkMaking + ulWorkingSize );
 }


#endif
