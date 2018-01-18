//#pragma option -v-

#include "indicat.hpp"
#include "bmp.hpp"
#include <string.h>
#include <mem.h>
#include <alloc.h>
#include <windowsx.h>
#include "INC\dlg.inc"

#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;

static BOOL bFlNoMove = FALSE;

TDrawTitleBuf TProgress::dtbBuf;

const double highButtonCancel = 3.0,
	     widthButtonCancel = highButtonCancel * 3.0,

	     highProgress     = 1.5,
	     widthProgress     = 0.85,

	     upskip           = 0.7,
	     downskip         = 0.7,
	     interval         = 0.2,

	     minWidthWnd      = 40,

	     stepRefresh      = 5.0;
	        
#define ID_PROGRESS 101

static void near pascal Quant()
 {
   int res;
   do {
     MSG msg;
     if( (res = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) == TRUE )      
       DispatchMessage( &msg );
    } while( res == TRUE );
 }

TStr::TStr( int indAssign, LPCSTR lpStr ):
  TIndexedItem( indAssign )
 {
   if( !(lpTxt = new char[_fstrlen(lpStr) + 1]) ) return;
   lstrcpy( lpTxt, lpStr );
 }

TStr::~TStr()
 {
   if( lpTxt ) { delete []lpTxt; lpTxt = NULL; }
 }

/*int TStr::isEqual( const Object& rObj ) const
 {
   if( (TStr huge *)this != (TStr huge *)(&rObj) ) return FALSE;
   return TRUE;
 }*/

void TStr::Update( LPCSTR lpStr )
 {
   if( lpTxt ) delete []lpTxt;

   if( !(lpTxt = new char[_fstrlen(lpStr) + 1]) ) return;
   lstrcpy( lpTxt, lpStr );
 }


void TProgress::WMEraseBkGnd( RTMessage Msg )
 {
   Msg.Result = 0;
 }


void TIndicator::WMEraseBkGnd( RTMessage Msg )//WM_ERASEBKGND
 {
   HBRUSH hbrOld = SelectBrush( (HDC)(Msg.WParam), (HBRUSH)GetStockObject(LTGRAY_BRUSH) );

   RECT r;
   GetClientRect( HWindow, &r );
   Rectangle( (HDC)(Msg.WParam), -1, -1, r.right + 1, r.bottom + 1 );

   SelectBrush( (HDC)(Msg.WParam), hbrOld );

   Msg.Result = TRUE;
 }


TIndicator::TIndicator( PTWindowsObject AParent,
     LPCSTR lpTitle,
     LONG  ulWorkSize,
     PTModule AModule  ):

     strTtl( 1, lpTitle ),
     TDialog( AParent, (LPSTR)MAKEINTRESOURCE(DlgProgressInd), AModule )
   {
     bFlEnableClose = FALSE;
     bFlRet = FALSE;

     ulWorkMaking = 0;
     ulWorkFull = ulWorkSize;
     ulStepRefresh = (LONG)(double(ulWorkSize) / 100.0 * stepRefresh);
     if( !ulStepRefresh ) ulStepRefresh = 1;

     pbtnCancel = new TBButton( this, IDCANCEL, "&Cancel", 0, 0, 10, 10,
       TRUE, AModule );
     pstProgress = new TProgress( this, ID_PROGRESS, NULL, 0, 0, 10, 10,
       AModule );
     pstProgress->Attr.Style |= WS_CHILD | WS_VISIBLE | BS_OWNERDRAW;

     lDelta = 0;

     
     pFrame = new T3DThickFrame( this, ID_PROGRESS, 2, AModule );
   };

void TIndicator::Init( LONG size )
 {
   bFlEnableClose = FALSE;
   bFlRet = FALSE;

   ulWorkMaking = 0;
   ulWorkFull = size;
   ulStepRefresh = (LONG)(double(size) / 100.0 * 2.0);
   if( !ulStepRefresh ) ulStepRefresh = 1;
   lDelta = 0;

   InvalidateRect( HWindow, NULL, FALSE );
 }

TIndicator::~TIndicator()
 {
   //if( lpTtl ) { delete []lpTtl; lpTtl = NULL; }
 }
   //

void TIndicator::ShutDownWindow()
 {
   bFlRet = IsWindowEnabled(pbtnCancel->HWindow);
                                  
   if( bFlEnableClose == FALSE || bFlRet == FALSE ) return;
   else TDialog::ShutDownWindow();
 }

BOOL TIndicator::CanClose()
 {
   return (IsWindowEnabled(pbtnCancel->HWindow) == FALSE) ? FALSE:bFlEnableClose;
 }

void TIndicator::Show()
 {
	       //TDialog
   if( HWindow && IsWindowVisible(HWindow) ) return;

   GetApplication()->MakeWindow( this );
   SetBounds();
   if( pFrame ) pFrame->InitSize();
   ShowWindow( HWindow, SW_SHOW );

   Quant();

   //EnableKBHandler();
 }

void TIndicator::Close()
 {
   bFlEnableClose = TRUE;
   EnableWindow( pbtnCancel->HWindow, TRUE );
   CloseWindow();
 }

LPSTR TIndicator::GetClassName()
 {
   return "3DW:INDICATOR";
 } 

void TIndicator::SetupWindow()
 {
   TDialog::SetupWindow();
   SetCaption( strTtl.lpTxt );
   if( pFrame ) pFrame->InitSize();
 }

void TIndicator::GetWindowClass( WNDCLASS& AWndClass )
 {
   TDialog::GetWindowClass( AWndClass );

   AWndClass.hbrBackground = (HBRUSH)GetStockObject( GRAY_BRUSH );
 }

void  TIndicator::AddStr( LPCSTR lpStrForAdd )
 {
   PTStr pStrTmp = new TStr( dblTxt.GetFreeIndex(), lpStrForAdd );

   if( !pStrTmp ) return;

   dblTxt.addAtTail( *pStrTmp ); //DoubleList
 }

BOOL  TIndicator::DeleteStr( int ind )
 {
   PTStr  pStr = (PTStr)(dblTxt[ ind ]);

   if( !pStr ) return FALSE;

   dblTxt.detach( *pStr, TShouldDelete::Delete );
   return TRUE;
 }

BOOL  TIndicator::UpdateStr( int ind, LPCSTR lpNewStr )
 {
   PTStr  pStr = (PTStr)(dblTxt[ ind ]);

   if( !pStr ) return FALSE;

   pStr->Update( lpNewStr );
   return TRUE;
 }

void  TIndicator::SetTitle( LPCSTR lpStr )
 {
   strTtl.Update( lpStr );
   SetCaption( strTtl.lpTxt );
 }


void  TIndicator::WorkIs100()
 {
   ulWorkMaking = ulWorkFull;
   InvalidateRect( pstProgress->HWindow, NULL, FALSE );
 }

BOOL  TIndicator::WorkMessage( LONG ulCurSize )
 {
   if( ulWorkFull < ulCurSize )
    {
      Quant();
      return bFlRet;
    }

   LONG lKey = ulWorkMaking;
   ulWorkMaking = ulCurSize;  
   
   Quant();

   if( ulCurSize - lDelta >= ulStepRefresh )
     lDelta = ulCurSize;
   else return bFlRet;

   InvalidateRect( pstProgress->HWindow, NULL, FALSE );

   return bFlRet;
 }

inline double min( double a1, double a2 )
 {
   return  (a1 < a2 ) ?  a1:a2; 
 }

inline double max( double a1, double a2 )
 {
   return  (a1 > a2 ) ?  a1:a2; 
 }


void TIndicator::SetBounds()//TWindow
 {
   int maxLen = 0;
   //HWND HWindow = Parent->HWindow;

   DoubleListIterator dblIter = DoubleListIterator( dblTxt );
   PTStr  pStr = (PTStr)&dblIter.current();

   int len;
   for( ; int(dblIter);  pStr = (PTStr)&++dblIter )
     if( (len = _fstrlen(pStr->lpTxt)) > maxLen )  maxLen = len;

   TEXTMETRIC tm;
   HDC  hdc = GetDC( HWindow );
   HFONT hfOld = (HFONT)SelectObject( hdc, GetStockObject(SYSTEM_FONT) );  
   GetTextMetrics( hdc, &tm );
   SelectObject( hdc, hfOld );
   ReleaseDC( HWindow, hdc );

   RECT r;
   int indY;
   int wid = min( max( 1.5 * double(maxLen) *  double(tm.tmAveCharWidth), double(minWidthWnd *  tm.tmAveCharWidth) ),
		  double(GetSystemMetrics(SM_CXSCREEN) - 4)
	        ),
       high = (indY = upskip * double(tm.tmHeight) +
              dblTxt.getItemsInContainer() * tm.tmHeight +
	      double(dblTxt.getItemsInContainer()) * double(tm.tmHeight) * interval +
	      double(tm.tmHeight) * interval
	      ) +
	      highButtonCancel * double(tm.tmHeight) +
	      highProgress     * double(tm.tmHeight) +
	      2.0 * double(tm.tmHeight) * interval +
	      downskip * 3.0 * double(tm.tmHeight) +
	      GetSystemMetrics( SM_CYCAPTION ) +
	      2 * GetSystemMetrics( SM_CYBORDER );
 

   r.left = (GetSystemMetrics(SM_CXSCREEN) - wid) / 2; 
   r.top = (GetSystemMetrics(SM_CYSCREEN) - high) / 2;
   r.right = r.left + wid;
   r.bottom = r.top + high;

  
   SetWindowPos( HWindow, NULL, r.left, r.top, wid, high,
     SWP_NOZORDER | ((bFlNoMove == TRUE) ? SWP_NOMOVE | SWP_NOACTIVATE:0) );

   GetClientRect( HWindow, &r );
   wid = r.right; high = r.bottom;

   r.left = (double(wid) - widthProgress * double(wid)) / 2.0;
   MoveWindow( pstProgress->HWindow,
               r.left,
	       indY + double(downskip) * double(tm.tmHeight),
	       widthProgress * double(wid),
	       highProgress * double(tm.tmHeight), TRUE
	     );


   r.left = (double(wid) - 65) / 2.0;
   MoveWindow( pbtnCancel->HWindow,
	       r.left,
	       /*high - double(downskip) * double(tm.tmHeight) -
	       highButtonCancel * double(tm.tmHeight),*/
	       high - double(downskip) * double(tm.tmHeight) - 41,
               65,
	       //widthButtonCancel * double(tm.tmHeight),
	       //highButtonCancel * double(tm.tmHeight),
	       41,
	       TRUE
	     );

   if( pFrame ) pFrame->InitSize();

   Quant();
 }

void TIndicator::DefWndProc( RTMessage Msg )
 {
   if( Msg.Message = WM_KEYUP && Msg.WParam == VK_ESCAPE )
     bFlRet = TRUE;

   TDialog::DefWndProc( Msg );
 }

void TIndicator::WMSysCommand( RTMessage Msg )
 {
   /*if( ((Msg.WParam & 0xFFF0) == SC_CLOSE) )
    {
      Msg.Result = 0;
      return;
    }*/

   DefWndProc( Msg );
 }


void TIndicator::WMCommand( RTMessage Msg )//WM_Command
 {

   if( Msg.WParam == IDCANCEL )
     bFlRet = TRUE;

   TDialog::WMCommand( Msg );
 }

void TIndicator::WMPaint( RTMessage Msg )//WM_PAINT
 {
   Msg.Result = 0;

   PAINTSTRUCT  PaintInfo;
   HDC PaintDC = BeginPaint( HWindow, &PaintInfo );

   TEXTMETRIC tm;  
   HFONT hfOld = (HFONT)SelectObject( PaintDC, GetStockObject(SYSTEM_FONT) );
   GetTextMetrics( PaintDC, &tm );


   RECT r;
   GetClientRect( HWindow, &r );
   r.left = 0;
   r.top = double(tm.tmHeight) * upskip;
   r.bottom = r.top + tm.tmHeight + 2;
   //r.right *= 2;
   COLORREF cOld = SetBkColor( PaintDC, PALETTEINDEX(7) );
   COLORREF cTxt = SetTextColor( PaintDC, RGB(0, 0, 0) );
   UINT uiOldAln = SetTextAlign( PaintDC, TA_CENTER | TA_TOP );

   int curY = double(tm.tmHeight) * interval + tm.tmHeight,
       x = r.right / 2,
       y = tm.tmHeight - 4;

   DoubleListIterator dblIter = DoubleListIterator( dblTxt );
   PTStr  pStr = (PTStr)&dblIter.current();

   for( ; int(dblIter);  pStr = (PTStr)&++dblIter,
       r.top += curY, r.bottom += curY, y += curY  )
     ExtTextOut( PaintDC, x, y, ETO_OPAQUE | ETO_CLIPPED,
       &r,
       pStr->lpTxt,
       _fstrlen( pStr->lpTxt ),
       NULL );

   SetTextAlign( PaintDC, uiOldAln );
   SetTextColor( PaintDC, cTxt );
   SetBkColor( PaintDC, cOld );
   SelectObject( PaintDC, hfOld );

   EndPaint( HWindow, &PaintInfo );
 }

void TIndicator::Update()
 {
   RECT r;
   GetClientRect( HWindow, &r );
   TEXTMETRIC tm;
   HDC hdc = GetDC( HWindow );
   HFONT hfOld = (HFONT)SelectObject( hdc, GetStockObject(SYSTEM_FONT) );
   GetTextMetrics( hdc, &tm );

   r.bottom = upskip * double(tm.tmHeight) +
              dblTxt.getItemsInContainer() * tm.tmHeight +
	      double(dblTxt.getItemsInContainer()) * double(tm.tmHeight) * interval;

   InvalidateRect( HWindow, NULL, FALSE );

   SelectObject( hdc, hfOld );
   ReleaseDC( HWindow, hdc );

   Quant();
 }

/*void TIndicator::DrawItem( DRAWITEMSTRUCT _FAR& DrawInfo )
 {
   if( DrawInfo.CtlID == ID_PROGRESS )
     pstProgress->DrawItem( DrawInfo );
   else TDialog::DrawItem( DrawInfo );
 }*/

void TIndicator::SetStatusButton( BOOL bFlStat )//SetFlags
 {
   EnableWindow( pbtnCancel->HWindow, bFlStat );
   HMENU hmSys = GetSystemMenu( HWindow, FALSE );

   EnableMenuItem( hmSys, 1, MF_BYPOSITION | ((bFlStat == TRUE) ? MF_ENABLED:MF_DISABLED | MF_GRAYED) );
 }

void TProgress::Paint( HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo )
 {
   PTIndicator pInd = (PTIndicator)Parent;
   RECT r;
   GetClientRect( HWindow, &r );

   if( !TProgress::dtbBuf.hbm )
     TProgress::dtbBuf.InitBuff( PaintDC, GetSystemMetrics( SM_CXSCREEN ) / 2,
       r.bottom );

   TProgress::dtbBuf.RequiestSize( PaintDC, r.right, r.bottom );

   HDC hdcBmp = CreateCompatibleDC( PaintDC );
   //HBITMAP hbmDraw = CreateCompatibleBitmap( PaintDC, r.right, r.bottom );
   HBITMAP hbmOld = (HBITMAP)SelectObject( hdcBmp, TProgress::dtbBuf.hbm );

   //HBRUSH hbrOld = SelectBrush( hdcBmp, GetStockObject(WHITE_BRUSH) );
   //Rectangle( hdcBmp, 0, 0, r.right, r.bottom );
   FillRect( hdcBmp, &r, (HBRUSH)GetStockObject(WHITE_BRUSH) );
              /*
   HPEN hpOld = SelectPen( hdcBmp, GetStockObject(BLACK_PEN) );
   MoveTo( hdcBmp, 0, 0 );
   LineTo( hdcBmp, 0, r.bottom );

   MoveTo( hdcBmp, 0, 0 );
   LineTo( hdcBmp, r.right + 1, 0 );

   HPEN hp = CreatePen( PS_SOLID, 0, RGB(0xA0, 0xA0, 0xA4) );
   SelectPen( hdcBmp, hp );
   MoveTo( hdcBmp, r.right - 2, 0 );
   LineTo( hdcBmp, r.right - 2, r.bottom );

   MoveTo( hdcBmp, r.right + 1, r.bottom - 3 );
   LineTo( hdcBmp, 0, r.bottom - 3 );
     */

   char cBuf[60];
   int len = wsprintf( cBuf, "%d%%", pInd->GetPercent() );
   HFONT hf = SelectFont( hdcBmp, GetStockObject(SYSTEM_FONT) );  


   int  modeOld = SetBkMode( hdcBmp, TRANSPARENT );
   COLORREF cTxt = SetTextColor( hdcBmp, RGB(0, 0, 0) );
   DrawText( hdcBmp, cBuf, len, &r,
	     DT_CENTER | DT_VCENTER | DT_SINGLELINE
	   );

   SetTextColor( hdcBmp, cTxt );
   SetBkMode( hdcBmp, modeOld );



   int ropOld = SetROP2( hdcBmp, R2_MERGEPENNOT ); //R2_NOT
   HBRUSH hbrBl = CreateSolidBrush( PALETTEINDEX(16) );
   HBRUSH hbrOld = SelectBrush( hdcBmp, hbrBl );
   HPEN hpBl = CreatePen( PS_SOLID, 1, PALETTEINDEX(16) );
   HPEN hpOld = SelectPen( hdcBmp, hpBl );

   Rectangle( hdcBmp, 0, 0,
	      min(r.right, int(double(r.right) * double(pInd->GetPercent()) / 100.0)),
	      r.bottom 
	    );

   SetROP2( hdcBmp, ropOld );
   SelectPen( hdcBmp, hpOld );
   SelectBrush( hdcBmp, hbrOld );
   SelectObject( hdcBmp, hbmOld );
   SelectFont( hdcBmp, hf );
   DeleteObject( hbrBl );
   DeleteObject( hpBl );
   DeleteDC( hdcBmp );


   DrawBmp( PaintDC, TProgress::dtbBuf.hbm );
   //DeleteObject( hbmDraw );
 }

void TIndicator::AdaptBounds()
 {
   bFlNoMove = TRUE;
   SetBounds();
   bFlNoMove = FALSE;
 }

int  TIndicator::GetPercent() const
 {
   if( !ulWorkFull ) return 100;
   else
     return double(ulWorkMaking) / double(ulWorkFull) * 100.0;
 }