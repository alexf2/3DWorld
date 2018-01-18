#include "frame3d.hpp"
#include <windowsx.h>

T3DFrame::T3DFrame( PTWindowsObject AParent, int iIdFramed_, PTModule AModule ):
    TWindow( AParent, NULL, AModule )
 {
   iIdFramed = iIdFramed_;
   SetFlags( WB_MDICHILD, FALSE );
   Attr.Style = WS_CHILD | WS_VISIBLE;
 }

LPSTR T3DFrame::GetClassName()
 {
   return "3DW:3DFrame";
 }

void T3DFrame::GetWindowClass( WNDCLASS & AWndClass )
 {
   TWindow::GetWindowClass( AWndClass );

   AWndClass.hbrBackground = (HBRUSH)GetStockObject( NULL_BRUSH );
 }

void T3DFrame::InitSize()
 {
   HWND hwndFramed;
   if( !Parent || !Parent->HWindow || !(hwndFramed = GetDlgItem(Parent->HWindow, iIdFramed)) )
    {
      HWND hwndOwn = GetApplication() ? GetApplication()->MainWindow->HWindow:NULL;
      MessageBox( hwndOwn, "T3DFrame: initialize need after ctl create", "Error", MB_OK | MB_ICONSTOP );
      return;
    }

   RECT r; GetWindowRect( hwndFramed, &r );
   POINT p1, p2; 
   p1.x = r.left; p1.y = r.top; p2.x = r.right; p2.y = r.bottom;
   ScreenToClient( Parent->HWindow, &p1 );
   ScreenToClient( Parent->HWindow, &p2 );
   rWin.left = p1.x - 3 - Dx(); rWin.right = p2.x - p1.x + 6 + 2*Dx();
   rWin.top = p1.y - 2 - Dy(); rWin.bottom = p2.y - p1.y + 4 + 2*Dy();

   MoveWindow( HWindow, rWin.left, rWin.top,
	       rWin.right, rWin.bottom, FALSE );


   w = rWin.right;
   h = rWin.bottom;

   rWin.right = rWin.left + w;
   rWin.bottom = rWin.top + h;

   return;
 }

void T3DThickFrame::InitSize()
 {
   T3DFrame::InitSize();

   rInt.left = iThick + 4; rInt.right = w - iThick - 5;
   rInt.top = iThick + 3; rInt.bottom = h - iThick - 4;

   RECT r = rInt;
   r.left -= 1; r.right += 2; r.top -= 1; r.bottom += 2;
   hrgEx = CreateRectRgn( 2, 2, w - 2, h - 2 );
   HRGN hrgInt = CreateRectRgnIndirect( &r );
   CombineRgn( hrgEx, hrgEx, hrgInt, RGN_DIFF );
   
   DeleteRgn( hrgInt );
 }


void T3DFrame::Paint( HDC PaintDC_, PAINTSTRUCT _FAR & PaintInfo )
 {
   HDC PaintDC = GetWindowDC( HWindow );
   /*RECT r; GetWindowRect( HWindow, &r );
   int w = r.right - r.left,
       h = r.bottom - r.top;*/

   HPEN hpWhite = (HPEN)GetStockObject( WHITE_PEN ),
	hpBlack = (HPEN)GetStockObject( BLACK_PEN ),
	hpLightGr = CreatePen( PS_SOLID, 1, RGB(195, 195, 195) ),
	hpDarkGr = CreatePen( PS_SOLID, 1, RGB(128, 128, 128) );

   HPEN hpOld = (HPEN)SelectObject( PaintDC, hpDarkGr );
   MoveTo( PaintDC, 0, 0 );
   LineTo( PaintDC, w, 0 );
   MoveTo( PaintDC, 0, 0 );
   LineTo( PaintDC, 0, h - 1 );

   SelectObject( PaintDC, hpWhite );
   MoveTo( PaintDC, 0, h - 1 );
   LineTo( PaintDC, w - 1, h - 1 );
   MoveTo( PaintDC, w - 1, h - 1 );
   LineTo( PaintDC, w - 1, 1 );

   SelectObject( PaintDC, hpBlack );
   MoveTo( PaintDC, 1, 1 );
   LineTo( PaintDC, w - 1, 1 );
   MoveTo( PaintDC, 1, 1 );
   LineTo( PaintDC, 1, h - 2 );

   SelectObject( PaintDC, hpLightGr );
   MoveTo( PaintDC, 1, h - 2 );
   LineTo( PaintDC, w - 2, h - 2 );
   MoveTo( PaintDC, w - 2, h - 2 );
   LineTo( PaintDC, w - 2, 1 );

   SelectObject( PaintDC, hpOld );
   DeleteObject( hpLightGr );
   DeleteObject( hpDarkGr );

   ReleaseDC( HWindow, PaintDC );
 }

T3DThickFrame::T3DThickFrame( PTWindowsObject AParent, int iIdFramed_, int iThick_,
	       PTModule AModule ):
  T3DFrame( AParent, iIdFramed_, AModule )
 {
   iThick = iThick_;
   hrgEx = NULL;
 }

T3DThickFrame::~T3DThickFrame()
 {
   if( hrgEx ) { DeleteRgn( hrgEx ); hrgEx = NULL; }
 }

void T3DThickFrame::Paint( HDC PaintDC_, PAINTSTRUCT _FAR & PaintInfo )
 {
   HDC PaintDC = GetWindowDC( HWindow );
   /*RECT r; GetWindowRect( HWindow, &r );
   int w = r.right - r.left,
       h = r.bottom - r.top;*/

   HPEN hpWhite = (HPEN)GetStockObject( WHITE_PEN ),
	hpBlack = (HPEN)GetStockObject( BLACK_PEN ),
	hpLightGr = CreatePen( PS_SOLID, 1, RGB(195, 195, 195) ),
	hpDarkGr = CreatePen( PS_SOLID, 1, RGB(128, 128, 128) );  
   

   HPEN hpOld = (HPEN)SelectObject( PaintDC, hpBlack );
   MoveTo( PaintDC, 1, 0 );
   LineTo( PaintDC, w - 1, 0 );
   MoveTo( PaintDC, 0, 1 );
   LineTo( PaintDC, 0, h - 1 );

   MoveTo( PaintDC, 1, h - 1 );
   LineTo( PaintDC, w - 1, h - 1 );
   MoveTo( PaintDC, w - 1, h - 2 );
   LineTo( PaintDC, w - 1, 0 );

   SelectObject( PaintDC, hpWhite );
   MoveTo( PaintDC, 1, 1 );
   LineTo( PaintDC, w - 1, 1 );
   MoveTo( PaintDC, 1, 1 );
   LineTo( PaintDC, 1, h - 2 );

   SelectObject( PaintDC, hpDarkGr );
   MoveTo( PaintDC, 1, h - 2 );
   LineTo( PaintDC, w - 1, h - 2 );
   MoveTo( PaintDC, w - 2, h - 2 );
   LineTo( PaintDC, w - 2, 1 );

   SetPixel( PaintDC, 0, 0, RGB(195, 195, 195) );
   SetPixel( PaintDC, w - 1, 0, RGB(195, 195, 195) );
   SetPixel( PaintDC, 0, h - 1, RGB(195, 195, 195) );
   SetPixel( PaintDC, w - 1, h - 1, RGB(195, 195, 195) );


   SelectObject( PaintDC, hpBlack );
   MoveTo( PaintDC, rInt.left + 1, rInt.top );
   LineTo( PaintDC, rInt.right, rInt.top );
   MoveTo( PaintDC, rInt.left, rInt.top + 1 );
   LineTo( PaintDC, rInt.left, rInt.bottom );

   MoveTo( PaintDC, rInt.left + 1, rInt.bottom );
   LineTo( PaintDC, rInt.right, rInt.bottom );
   MoveTo( PaintDC, rInt.right, rInt.bottom - 1 );
   LineTo( PaintDC, rInt.right, rInt.top );

   SelectObject( PaintDC, hpDarkGr );
   MoveTo( PaintDC, rInt.left - 1, rInt.top - 1 );
   LineTo( PaintDC, rInt.right + 2, rInt.top - 1 );
   MoveTo( PaintDC, rInt.left - 1, rInt.top - 1 );
   LineTo( PaintDC, rInt.left - 1, rInt.bottom + 1 );

   SelectObject( PaintDC, hpWhite );
   MoveTo( PaintDC, rInt.left - 1, rInt.bottom + 1 );
   LineTo( PaintDC, rInt.right + 1, rInt.bottom + 1 );
   MoveTo( PaintDC, rInt.right + 1, rInt.bottom + 1 );
   LineTo( PaintDC, rInt.right + 1, rInt.top  - 1 );

   SetPixel( PaintDC, rInt.left, rInt.top, RGB(195, 195, 195) );
   SetPixel( PaintDC, rInt.left, rInt.bottom, RGB(195, 195, 195) );
   SetPixel( PaintDC, rInt.right, rInt.bottom, RGB(195, 195, 195) );
   SetPixel( PaintDC, rInt.right, rInt.top, RGB(195, 195, 195) );
 
  
   FillRgn( PaintDC, hrgEx, (HBRUSH)GetStockObject(LTGRAY_BRUSH) );


   SelectObject( PaintDC, hpOld );
   DeleteObject( hpLightGr );
   DeleteObject( hpDarkGr );

   ReleaseDC( HWindow, PaintDC );
 }

