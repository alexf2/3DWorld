//#pragma option -v-

#include "status.hpp"
#include "ucs.hpp"
#include <string.h>

#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;

int TItem::isEqual( const Object& rObj) const
 {
   if( (TItem huge *)this != (TItem huge *)(&rObj) ) return FALSE;

   return TRUE;
 }

TItem::~TItem()
 {
   if( lpTxt ) delete []lpTxt; lpTxt = NULL;
 }

LPCSTR TItem::GetText() const
 {
   static char *cNull = "";

   if( lpTxt )
     return (LPCSTR)lpTxt;
   else return cNull;
 }

void TItem::NewText( LPCSTR lpT )
 {
   if( lpTxt ) delete []lpTxt;
   lpTxt = new char[ _fstrlen(lpT) + 1 ];
   if( lpTxt ) _fstrcpy( lpTxt, lpT );
 }

TStatusWindow::TStatusWindow( PTWindowsObject AParent, PTModule AModule ):
  TBWindow( AParent, NULL, AModule )
 {

   hfStatFont = NULL;

   //Attr.Style = WS_VISIBLE | WS_CHILD | WS_BORDER;
   
   Attr.Style = WS_VISIBLE | WS_BORDER | WS_POPUP | WS_CLIPCHILDREN;
   
   Attr.X = Attr.Y = Attr.W = Attr.H = 0;
   iHighWin = 0;
   SetFlags( WB_MDICHILD, FALSE );
   defaultHandl = 1;
 }
      //isA classType
void TStatusWindow::WMSize( RTMessage msg )//WM_SIZE
 {
   int widthNew = LOWORD( msg.LParam );
   RECT r;
   GetWindowRect( HWindow, &r );
   int widthOld = r.right - r.left;
   int delta = widthNew - widthOld;
   PTItem pLastItem = &(RTItem)listItems.peekAtTail();//DoubleList

   if( listItems.getItemsInContainer() )
    {

   if( delta > 0 )
     pLastItem->ChangeSize( pLastItem->GetSize() + delta + 1);
   else if( delta < 0 )
    {
      delta = -(--delta);
      if( pLastItem->GetSize() >= delta )
	pLastItem->ChangeSize( pLastItem->GetSize() - delta );
      else if( delta < widthOld - 1 )
       {
	 BOOL bFlChanged = FALSE;
         int cDelta = delta;

	 while( !bFlChanged )
	  {
	    DoubleListIterator liIter = DoubleListIterator( listItems, FALSE );//DoubleList


            for( PTItem pItem =  &(RTItem)liIter.current();
		 int(liIter); pItem = &(RTItem)--liIter )
	      {
		int chSize = pItem->GetSize();
                chSize = (chSize > 0) ? chSize-1:0;
		pItem->ChangeSize( chSize );
		if( chSize ) cDelta--;
		if( cDelta < 1 ) { bFlChanged = TRUE; break; }
              }
          }
       }
    }

    }
   //GetWndWidth()
   TBWindow::WMSize( msg );
 }

int TStatusWindow::CountWidth() const
 {
   int widt = 0;
   DoubleListIterator liIter = DoubleListIterator( listItems );//List

   for( PTItem pItem =  &(RTItem)liIter.current();
	   int(liIter); pItem = &(RTItem)++liIter )
     widt += pItem->GetSize() + 2;

   if( widt > 2 ) widt -= 2;

   return widt;
 }

void TStatusWindow::Update()
 {
   //TMessage msg;
    InvalidateRect( HWindow, NULL, FALSE );
   //_fsetmem( &msg, sizeof(TMessage), 0 );
   //RedrawWindow( HWindow, NULL, NULL, RDW_NOERASE );
   //WMPaint( msg );
   //hwnd, lprcUpdate, hrgnUpdate, fuRedraw)
 }

void TStatusWindow::WMPaint( RTMessage )//WM_Paint
 {
   //TBWindow::WMPaint( Msg );

   int curItemX = 0;

   PAINTSTRUCT  pStruct;
   BeginPaint( HWindow, &pStruct );
   HDC hdc = pStruct.hdc;
   //HDC hdc = GetWindowDC( HWindow );

   HFONT hfOld = SelectFont( hdc, hfStatFont );

   UINT alignOld = SetTextAlign( hdc, TA_LEFT | TA_TOP );
   COLORREF colorOld = GetTextColor( hdc );
   RECT r;
   GetClientRect( HWindow, &r );
   int highCl = r.bottom;

   TEXTMETRIC tm;
   GetTextMetrics( hdc, &tm );
   int sta = float(highCl - tm.tmHeight) / 2.0;

   HPEN hpWhite, hpBlack;
   HPEN hpOld = SelectPen( hdc, (hpWhite = (HPEN)GetStockObject(WHITE_PEN)) );
   COLORREF colorBkOld = SetBkColor( hdc, PALETTEINDEX(7) );
   hpBlack = (HPEN)GetStockObject(BLACK_PEN);

   DoubleListIterator liIter = DoubleListIterator( listItems );

   for( PTItem pItem =  &(RTItem)liIter.current();
	   int(liIter); pItem = &(RTItem)++liIter )
    {
      static char cBuf[1024];
      TRECT rect = TRECT( curItemX+4, sta, curItemX + pItem->GetSize(), highCl- 2 );

      SetTextColor( hdc, pItem->GetColor() );
      //MoveTo( hdc, rect.left, rect.top );

      
      _fstrcpy( cBuf, pItem->GetText() );
      int iSpcCount = (int(pItem->GetSize())-int(_fstrlen(pItem->GetText()))*int(tm.tmAveCharWidth)) /
	int(tm.tmMaxCharWidth) + 1;
      if( iSpcCount > 0 )
       {
	 iSpcCount = (iSpcCount+_fstrlen(cBuf) < 1023) ? iSpcCount:
	   (1023-_fstrlen(cBuf));
         LPSTR pKey;
         _fsetmem( (pKey=_fstrchr(cBuf, 0)), iSpcCount, 32 );
	 *(pKey + iSpcCount) = 0;
       }     
       
      ExtTextOut( hdc, rect.left, rect.top, ETO_CLIPPED | ETO_OPAQUE, &rect, cBuf,
	_fstrlen( cBuf ), NULL );
      curItemX += pItem->GetSize() + 1;

      if( curItemX < r.right )
       {
         SelectPen( hdc, hpWhite );
         MoveTo( hdc, curItemX, 0 );
         LineTo( hdc, curItemX, highCl );

         SelectPen( hdc, hpBlack );
         MoveTo( hdc, curItemX+1, 0 );
	 LineTo( hdc, curItemX+1, highCl );
       }
     // curItemX += 4;
    }

   //WMEraseBkGnd( Msg );
   //Msg.Result = 1;
   SetTextAlign( hdc, alignOld );
   SelectFont( hdc, hfOld );
   SetTextColor( hdc, colorOld );
   SelectPen( hdc, hpOld );
   SetBkColor( hdc, colorBkOld );
   //ReleaseDC( HWindow, hdc );
   EndPaint( HWindow, &pStruct );
 }

void TStatusWindow::SetupWindow()
 {
   TBWindow::SetupWindow();

   TEXTMETRIC tm;
   HDC hdc = GetDC( HWindow );
   int iHighBord = GetSystemMetrics( SM_CYBORDER );
   int iWidtBord = GetSystemMetrics( SM_CXBORDER );


   GetTextMetrics( hdc, &tm );

   iHighWin = tm.tmHeight + iHighBord*2;

   hfStatFont = CreateFont( -(iHighWin-6), 0, 0, 0, FW_NORMAL,
     0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_STROKE_PRECIS,
     DEFAULT_QUALITY, DEFAULT_PITCH, "Helv" );

   ReleaseDC( HWindow, hdc );
   EnableWindow( HWindow, FALSE );
 }

void TStatusWindow::WMEraseBkGnd( RTMessage Msg )//WM_ERASEBKGND
 {
   //HDC hdc = GetWindowDC( HWindow );
   HDC hdc = (HDC)Msg.WParam;
   RECT rWnd;
   HBRUSH hbrOld;

   GetWindowRect( HWindow, &rWnd );
   hbrOld = SelectBrush( hdc, GetStockObject(LTGRAY_BRUSH) );
   Rectangle( hdc, 0, 0, rWnd.right - rWnd.left + 1,
	      rWnd.bottom - rWnd.top + 1 );
   SelectBrush( hdc, hbrOld );
   ReleaseDC( HWindow, hdc );

   hdc = GetDC( HWindow );
   GetClientRect( HWindow, &rWnd );
   HPEN hpOld = SelectPen( hdc, GetStockObject(WHITE_PEN) );

   MoveTo( hdc, 0, 0 );
   LineTo( hdc, rWnd.right - rWnd.left + 1, 0 );
   MoveTo( hdc, 0, 0 );
   LineTo( hdc, 0, rWnd.bottom - rWnd.top );

   SelectPen( hdc, hpOld );
   //ReleaseDC( HWindow, hdc );

   Msg.Result = 0;
 }


void TStatusWindow::GetWindowClass( WNDCLASS _FAR &AWndClass )
 {
   TBWindow::GetWindowClass( AWndClass );
 }

LPSTR TStatusWindow::GetClassName()
 {
   return "3DW:StatusLine";
 } 


TStatusWindow::~TStatusWindow()
 {
   if( hfStatFont ) DeleteObject( hfStatFont );
 }

void TStatusWindow::WMCommand( RTMessage msg )
 {
   BOOL bFlPrep = FALSE;

   switch( msg.WParam )
    {
      case CM_ADDITEM:
	msg.Result = NewItem( LOWORD(msg.LParam) );
	bFlPrep = TRUE;
	break;  

      case CM_DELETEITEM:
	msg.Result = DeleteItem( CheckHandl(LOWORD(msg.LParam)) );
	bFlPrep = TRUE;
	break;

      case CM_SENDTEXT:
	msg.Result = SetText( (LPTSendTxt)msg.LParam );
	bFlPrep = TRUE;
	break;

      case CM_SETITEM_COLOR:
	msg.Result = SetColor( (LPTSetColor)msg.LParam );
	bFlPrep = TRUE;
	break;

      case CM_DEF_HANDLE:
	defaultHandl = msg.LParam;
	bFlPrep = TRUE;
        break;

    }

  if( bFlPrep == TRUE ) return;
  else TBWindow::WMCommand( msg );

 }

int TStatusWindow::GetWndWidth() const
 {
   RECT rect;
   GetClientRect( HWindow, &rect );

   return rect.right;
 }

UINT TStatusWindow::NewItem( UINT size )
 {
   int curWidt = CountWidth(), wndWidt, dif = 0;

   if( (wndWidt = GetWndWidth()) < curWidt + size  )
     dif = int(curWidt + size) - wndWidt;

   int sizeNew = int(size) - dif; sizeNew = (sizeNew < 0) ? 0:sizeNew;

   int handl;
   PTItem pNewItem = new TItem( sizeNew, (handl = GetFreeHandle()) );
   listItems.add( *pNewItem );
   Update();

   return handl;
 }

static int IdCmp( const class Object _FAR &tstObj, Pvoid pArg )
 {
   PTItem pItem = &(RTItem)tstObj;

   return pItem->GetHandl() == UINT(pArg);
 }

int TStatusWindow::DeleteItem( UINT handle )
 {
   PTItem pFind = &(RTItem)listItems.firstThat( IdCmp, (Pvoid)handle );
   //virtual Object& firstThat(condFuncType testFuncPtr, void *paramListPtr)
   //typedef int ( _FAR *condFuncType )( const class Object _FAR &, void _FAR * );

   if( *pFind == NOOBJECT ) return -1;

   listItems.detach( *pFind, TShouldDelete::Delete );
   Update();

   return 0;
 }

int TStatusWindow::SetText( LPTSendTxt lpTransfTxt )
 {
   PTItem pFind = &(RTItem)listItems.firstThat( IdCmp,
     (Pvoid)CheckHandl(lpTransfTxt->handle) );

   if( *pFind == NOOBJECT ) return -1;

   pFind->NewText( lpTransfTxt->lpTxt );
   Update();

   return 0;
 }


int TStatusWindow::SetColor( LPTSetColor lpTransgColor )
 {
   PTItem pFind = &(RTItem)listItems.firstThat( IdCmp,
     (Pvoid)CheckHandl(lpTransgColor->handle) );

   if( *pFind == NOOBJECT ) return -1;

   pFind->SetColor( lpTransgColor->color );
   return 0;
 }

UINT TStatusWindow::GetFreeHandle() const
 {
   UINT handle = 1;
   BOOL bFlFound = FALSE;

   while( !bFlFound )
    {
      DoubleListIterator liIter = DoubleListIterator( listItems );

      bFlFound = TRUE;
      for( PTItem pItem =  &(RTItem)liIter.current();
	   int(liIter); pItem = &(RTItem)++liIter )
	if( pItem->GetHandl() == handle )
	 {
	   handle++; bFlFound = FALSE; break;
         }
    }

   return handle;
 }

BOOL TStatusWindow::CanClose()
 {
   return ((PTMDIFrame)Parent)->ClientWnd->CanClose();
 }

