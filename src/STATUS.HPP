#if !defined(_STATUS_WND_)
#define _STATUS_WND_

#include <owl.h>
#include <bwindow.h>
#include <dbllist.h>
#include <windowsx.h>
#include "INC\classtyp.inc"

#undef _EXPORT2
#if defined(_STATUSDLL) || defined(_ALLCTLDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif


#define TYP_STATUS_ITEM 103

 //Use HIWORD(LParam) for any code of command 
#define CM_ADDITEM     (WM_USER + 40) //WParam = CM_ADDITEM, LParam = size 
                                      //Return = Handle
#define CM_DELETEITEM  (WM_USER + 41) //WParam = CM_DELETEITEM, LParam = handle
#define CM_SENDTEXT    (WM_USER + 42) //WParam = CM_SENDTEXT, LParam = TSendText 

#define CM_SETITEM_COLOR (WM_USER + 43)//WParam = CM_SETITEM_COLOR
				       //LParam = TSetColor
#define CM_DEF_HANDLE    (WM_USER + 44)

_CLASSDEF(TItem)
_CLASSDEF(TStatusWindow)

struct _EXPORT2 TSendTxt       
 {
   LPCSTR lpTxt;
   UINT handle;
 };                
typedef TSendTxt FAR *LPTSendTxt;

struct _EXPORT2 TSetColor
 {
   COLORREF color;
   UINT handle;
 };
typedef TSetColor FAR *LPTSetColor;


class _EXPORT2 TItem: public Object
 {

public:
  TItem( int iSizeWnd, int hand )
   {
     iSizeItemWnd = iSizeWnd;
     lpTxt = NULL;
     handl = hand;
     color = PALETTEINDEX(0);
   }

  ~TItem();

  virtual classType isA() const { return TYP_STATUS_ITEM; }
  virtual hashValueType hashValue() const { return 0; }
  virtual int isEqual( const Object& ) const;
  virtual char *nameOf() const { return "StatusItem"; }
  virtual void printOn( ostream& ) const {};

  void ChangeSize( int iNew ) { iSizeItemWnd = iNew; }
  int GetSize() const { return iSizeItemWnd; }
  UINT GetHandl() const { return handl; }
  void NewText( LPCSTR lpT );
  LPCSTR GetText() const;

  void SetColor( COLORREF col ) { color = col; }
  COLORREF GetColor() const { return color; }

private:
  LPSTR lpTxt;
  int iSizeItemWnd;
  UINT handl;
  COLORREF color;

 };

_CLASSDEF(TStatusWindow)


class _EXPORT2 TStatusWindow: public TBWindow
 {
public:
  TStatusWindow( PTWindowsObject AParent, PTModule AModule = NULL );
 
  virtual ~TStatusWindow();
  int GetHigh() const { return iHighWin; }
  virtual BOOL CanClose();
  virtual classType isA() const
   {
     return classtypeStatusWindow;
   }

protected:
  virtual void GetWindowClass( WNDCLASS _FAR &AWndClass );
  virtual LPSTR GetClassName();
  virtual void SetupWindow();
  virtual void WMEraseBkGnd( RTMessage ) = [WM_FIRST + WM_ERASEBKGND];
  virtual void WMPaint( RTMessage Msg ) = [WM_FIRST + WM_PAINT];
  virtual void WMCommand(RTMessage Msg) = [WM_FIRST + WM_COMMAND];
  virtual void WMSize(RTMessage Msg) = [WM_FIRST + WM_SIZE];

private:
  DoubleList listItems;
  int  iHighWin;

  int defaultHandl;

  HFONT   hfStatFont;

  UINT NewItem( UINT size ); //ret handle
  int DeleteItem( UINT handle );
  int SetText( LPTSendTxt lpTransfTxt  );
  int SetColor( LPTSetColor lpTransgColor );
  UINT GetFreeHandle() const;
  int CheckHandl( UINT hnd ) { return hnd ? hnd:defaultHandl; }

  int CountWidth() const;
  int GetWndWidth() const;
  void Update();

 };

#endif
