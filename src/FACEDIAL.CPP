#pragma option -v-

#include "facedial.hpp"
#include "bmp.hpp"

#include <bwcc.h>
#include <stdio.h>
#include <bchkbox.h>
#include <bstatbmp.h>

#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;

static int sc1, sc2;
//93, 7, 149, 84
const int tb1X = RX(110), tb1W = RX(115), tb1Y = RY(25), tb1H = RY(12),
	  tb2X = RX(110), tb2W = RX(115), tb2Y = RY(62), tb2H = RY(12),

	  tx1X = RX(110), tx1W = RX(115), tx1Y = RY(12), tx1H = RY(12),
	  tx2X = RX(110), tx2W = RX(115), tx2Y = RY(52), tx2H = RY(12);

void TFaceData::InitControls( HFONT hf, PTApplication pApp, HWND hwnd )
 {
   SendMessage( GetDlgItem(hwnd, ID_CHECK_SMOTH), BM_SETCHECK, bFlSmoothed, 0L );
   SendMessage( GetDlgItem(hwnd, ID_CHECK_ALIGN), BM_SETCHECK, bFlAligmented, 0L );
 }

void TCylinderInit::InitControls( HFONT hf, PTApplication pApp, HWND hwnd )
 {
   TFaceData::InitControls( hf, pApp, hwnd );

   if( !pOwn ) return;

   sc1 = CYLINDER_SCROLLBAR_SEGM;
   sc2 = -1;

   pScSegm = new TMyScrollBar( pOwn, CYLINDER_SCROLLBAR_SEGM, tb1X, tb1Y, tb1W, tb1H, TRUE, pApp );


   PTStatic pSt;
   pSt = new TStatic( pOwn, CYLINDER_SCROLLBAR_SEGM_TEXT, "Number of sides cylinder",
       tx1X, tx1Y, tx1W, tx1H, 50, pApp );

   pApp->MakeWindow( pScSegm );
   pScSegm->SetRange( 3, 100 );
   pScSegm->SetPosition_( nSides );

   pApp->MakeWindow( pSt );
   SendDlgItemMessage( hwnd, CYLINDER_SCROLLBAR_SEGM, WM_SETFONT, (WPARAM)hf, FALSE );
   SendDlgItemMessage( hwnd, CYLINDER_SCROLLBAR_SEGM_TEXT, WM_SETFONT, (WPARAM)hf, FALSE );

   ShowWindow( GetDlgItem(hwnd, ID_VALUE_TB2), SW_HIDE );

   //SendMessage( GetDlgItem(hwnd, ID_CHECK_SMOTH), BM_SETCHECK, bFlSmoothed, 0L );
 }

void TCylinderInit::DeleteControls()
 {
   if( !pOwn || !pScSegm ) return;

   nSides = pScSegm->GetPosition();
	      //TMyScrollBar
   if( pScSegm ) pScSegm->CloseWindow();
   pScSegm = NULL;

   PTStatic pText = (PTStatic)pOwn->ChildWithId( CYLINDER_SCROLLBAR_SEGM_TEXT );
   if( pText ) pText->CloseWindow(); pText = NULL;
 }


void TConeInit::InitControls( HFONT hf, PTApplication pApp, HWND hwnd )
 {
   TFaceData::InitControls( hf, pApp, hwnd );

   if( !pOwn ) return;

   sc1 = CONE_SCROLLBAR_SEGM;
   sc2 = -1;

   pScSegm = new TMyScrollBar( pOwn, CONE_SCROLLBAR_SEGM, tb1X, tb1Y, tb1W, tb1H,
     TRUE, pApp );


   PTStatic pSt;
   pSt = new TStatic( pOwn, CONE_SCROLLBAR_SEGM_TEXT, "Number of sides cone",
     tx1X, tx1Y, tx1W, tx1H, 50, pApp );

   pApp->MakeWindow( pScSegm );
   pScSegm->SetRange( 3, 100 );
   pScSegm->SetPosition_( nSides );

   pApp->MakeWindow( pSt );
   SendDlgItemMessage( hwnd, CONE_SCROLLBAR_SEGM, WM_SETFONT, (WPARAM)hf, FALSE );
   SendDlgItemMessage( hwnd, CONE_SCROLLBAR_SEGM_TEXT, WM_SETFONT, (WPARAM)hf, FALSE );

   ShowWindow( GetDlgItem(hwnd, ID_VALUE_TB2), SW_HIDE );
   //SendMessage( GetDlgItem(hwnd, ID_CHECK_SMOTH), BM_SETCHECK, bFlSmoothed, 0L );
 }

void TConeInit::DeleteControls()
 {
   if( !pOwn || !pScSegm ) return;

   nSides = pScSegm->GetPosition();
   if( pScSegm ) pScSegm->CloseWindow();
   pScSegm = NULL;

   PTStatic pText = (PTStatic)pOwn->ChildWithId( CONE_SCROLLBAR_SEGM_TEXT );
   if( pText ) pText->CloseWindow(); pText = NULL;
 }

void TSphereInit::InitControls( HFONT hf, PTApplication pApp, HWND hwnd )
 {
   TFaceData::InitControls( hf, pApp, hwnd );

   if( !pOwn ) return;

   sc1 = SPHERE_SCROLLBAR_MERID;
   sc2 = SPHERE_SCROLLBAR_HORIZ;

   //pScMerid, pScHoriz;
   pScMerid = new TMyScrollBar( pOwn, SPHERE_SCROLLBAR_MERID,
     tb1X, tb1Y, tb1W, tb1H, TRUE, pApp );


   PTStatic pSt;
   pSt = new TStatic( pOwn, SPHERE_SCROLLBAR_MERID_TEXT, "Number meridian of sphere",
     tx1X, tx1Y, tx1W, tx1H, 50, pApp );
   pApp->MakeWindow( pSt );
   SendDlgItemMessage( hwnd, SPHERE_SCROLLBAR_MERID_TEXT, WM_SETFONT, (WPARAM)hf, FALSE );

   pScHoriz = new TMyScrollBar( pOwn, SPHERE_SCROLLBAR_HORIZ,
     tb2X, tb2Y, tb2W, tb2H, TRUE, pApp );


   pSt = new TStatic( pOwn, SPHERE_SCROLLBAR_HORIZ_TEXT, "Number parall of sphere",
     tx2X, tx2Y, tx2W, tx2H, 16, pApp );
   pApp->MakeWindow( pSt );
   SendDlgItemMessage( hwnd, SPHERE_SCROLLBAR_HORIZ_TEXT, WM_SETFONT, (WPARAM)hf, FALSE );

   pApp->MakeWindow( pScMerid );
   pScMerid->SetRange( 3, 100 );
   pScMerid->SetPosition_( nMerid );

   SendDlgItemMessage( hwnd, SPHERE_SCROLLBAR_HORIZ, WM_SETFONT, (WPARAM)hf, FALSE );
   pApp->MakeWindow( pScHoriz );
   pScHoriz->SetRange( 3, 100 );
   pScHoriz->SetPosition_( nHoriz );

   SendDlgItemMessage( hwnd, SPHERE_SCROLLBAR_HORIZ_TEXT, WM_SETFONT, (WPARAM)hf, FALSE );

   ShowWindow( GetDlgItem(hwnd, ID_VALUE_TB2), SW_SHOW );
   //SendMessage( GetDlgItem(hwnd, ID_CHECK_SMOTH), BM_SETCHECK, bFlSmoothed, 0L );
 }

void TSphereInit::DeleteControls()
 {
   if( !pOwn || !pScMerid  || !pScHoriz ) return;

   nMerid = pScMerid->GetPosition();
   nHoriz = pScHoriz->GetPosition();

   if( pScMerid ) pScMerid->CloseWindow();
   pScMerid = NULL;
   if( pScHoriz ) pScHoriz->CloseWindow();
   pScHoriz = NULL;

   PTStatic pText = (PTStatic)pOwn->ChildWithId( SPHERE_SCROLLBAR_MERID_TEXT );
   if( pText ) pText->CloseWindow(); 
   pText = (PTStatic)pOwn->ChildWithId( SPHERE_SCROLLBAR_HORIZ_TEXT );
   if( pText ) pText->CloseWindow();
 }

void TTorusInit::InitControls( HFONT hf, PTApplication pApp, HWND hwnd )
 {
   TFaceData::InitControls( hf, pApp, hwnd );

   if( !pOwn ) return;

   sc1 = TORUS_SCROLLBAR_MERID;
   sc2 = TORUS_SCROLLBAR_HORIZ;

   //pScMerid, pScHoriz;
   pScMerid = new TMyScrollBar( pOwn, TORUS_SCROLLBAR_MERID,
     tb1X, tb1Y, tb1W, tb1H, TRUE, pApp );


   PTStatic pSt;
   pSt = new TStatic( pOwn, TORUS_SCROLLBAR_MERID_TEXT, "Number meridian of sphere",
     tx1X, tx1Y, tx1W, tx1H, 50, pApp );
   pApp->MakeWindow( pSt );
   SendDlgItemMessage( hwnd, TORUS_SCROLLBAR_MERID_TEXT, WM_SETFONT, (WPARAM)hf, FALSE );

   pScHoriz = new TMyScrollBar( pOwn, TORUS_SCROLLBAR_HORIZ,
     tb2X, tb2Y, tb2W, tb2H, TRUE, pApp );


   pSt = new TStatic( pOwn, TORUS_SCROLLBAR_HORIZ_TEXT, "Number parall of sphere",
     tx2X, tx2Y, tx2W, tx2H, 16, pApp );
   SendDlgItemMessage( hwnd, TORUS_SCROLLBAR_HORIZ_TEXT, WM_SETFONT, (WPARAM)hf, FALSE );

   pApp->MakeWindow( pScMerid );
   pScMerid->SetRange( 3, 100 );
   pScMerid->SetPosition_( nMerid );

   SendDlgItemMessage( hwnd, TORUS_SCROLLBAR_HORIZ, WM_SETFONT, (WPARAM)hf, FALSE );
   pApp->MakeWindow( pScHoriz );
   pScHoriz->SetRange( 3, 100 );
   pScHoriz->SetPosition_( nHoriz );

   SendDlgItemMessage( hwnd, TORUS_SCROLLBAR_MERID, WM_SETFONT, (WPARAM)hf, FALSE );

   ShowWindow( GetDlgItem(hwnd, ID_VALUE_TB2), SW_SHOW );
   //SendMessage( GetDlgItem(hwnd, ID_CHECK_SMOTH), BM_SETCHECK, bFlSmoothed, 0L );
 }

void TTorusInit::DeleteControls()
 {
   if( !pOwn || !pScMerid  || !pScHoriz  ) return;

   nMerid = pScMerid->GetPosition();
   nHoriz = pScHoriz->GetPosition();

   if( pScMerid ) pScMerid->CloseWindow();
   pScMerid = NULL;
   if( pScHoriz ) pScHoriz->CloseWindow();
   pScHoriz = NULL;

   PTStatic pText = (PTStatic)pOwn->ChildWithId( TORUS_SCROLLBAR_MERID_TEXT );
   if( pText ) pText->CloseWindow();
   pText = (PTStatic)pOwn->ChildWithId( TORUS_SCROLLBAR_HORIZ_TEXT );
   if( pText ) pText->CloseWindow();
 }

void TTubeInit::InitControls( HFONT hf, PTApplication pApp, HWND hwnd )
 {
   TFaceData::InitControls( hf, pApp, hwnd );

   if( !pOwn ) return;

   sc1 = TUBE_SCROLLBAR_SEGM;
   sc2 = -1;

   pScSegm = new TMyScrollBar( pOwn, TUBE_SCROLLBAR_SEGM,
     tb1X, tb1Y, tb1W, tb1H, TRUE, pApp );


   PTStatic pSt;
   pSt = new TStatic( pOwn, TUBE_SCROLLBAR_SEGM_TEXT, "Number segments of tube",
     tx1X, tx1Y, tx1W, tx1H, 50, pApp );
   pApp->MakeWindow( pSt );
   SendDlgItemMessage( hwnd, TUBE_SCROLLBAR_SEGM_TEXT, WM_SETFONT, (WPARAM)hf, FALSE );

   pApp->MakeWindow( pScSegm );
   pScSegm->SetRange( 3, 100 );
   pScSegm->SetPosition_( nSides );

   SendDlgItemMessage( hwnd, TUBE_SCROLLBAR_SEGM, WM_SETFONT, (WPARAM)hf, FALSE );

   ShowWindow( GetDlgItem(hwnd, ID_VALUE_TB2), SW_HIDE );
   //SendMessage( GetDlgItem(hwnd, ID_CHECK_SMOTH), BM_SETCHECK, bFlSmoothed, 0L );
 }

void TTubeInit::DeleteControls()
 {
   if( !pOwn ||!pScSegm ) return;

   nSides = pScSegm->GetPosition();

   if( pScSegm ) pScSegm->CloseWindow();
   pScSegm = NULL;

   PTStatic pText = (PTStatic)pOwn->ChildWithId( TUBE_SCROLLBAR_SEGM_TEXT );
   if( pText ) pText->CloseWindow();
 }


TFaceItem::TFaceItem( int resIdNorm, int resIdHilite, HINSTANCE hInst, PTFaceData pFD )
 {
   this->hInst = hInst;

   hbmN = hbmH = NULL;

   hbmN = LoadBitmap( hInst, (LPCSTR)MAKEINTRESOURCE(resIdNorm) );
   hbmH = LoadBitmap( hInst, (LPCSTR)MAKEINTRESOURCE(resIdHilite) );

   pFData = pFD;

   if( !hbmN || !hbmH )    
     Status = ER_BITMAPLOAD;
   else Status = 0;
 }

TFaceItem::~TFaceItem()
 {
   if( hbmN ) DeleteObject( hbmN ); hbmN = NULL;
   if( hbmH ) DeleteObject( hbmH ); hbmH = NULL;
   if( pFData ) delete pFData;
 }

int TFaceItem::isEqual( const Object& rObj ) const
 {
   if( (TFaceItem huge *)this != (TFaceItem huge *)(&rObj) ) return FALSE;
   return TRUE;
 }


TFaceDialog::TFaceDialog( PTWindowsObject AParent, DoubleList *pList,
  FaceTypes *pSelect, PTModule AModule ):
  TCenterDialog( AParent, (LPSTR)MAKEINTRESOURCE(DlgCreateBody), AModule )
  {
    hf = CreateFont( -8, 0, 0, 0, FW_BOLD,
     0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_STROKE_PRECIS,
     DEFAULT_QUALITY, DEFAULT_PITCH, "Helv" );

    pSelIndex = pSelect;
    this->pList = pList;
    sc1 = sc2 = -1;

    // 22, 19, 51, 129
    PTListBox pLi = new TListBox( this, ID_LIST, RX(22), RY(19), RX(51), RY(129), AModule );
    pLi->Attr.Style = LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_DISABLENOSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_TABSTOP;

    // 15, 7, 65, 148
    PTBGroupBox pGr = new TBGroupBox( this, 102, NULL, RX(15), RY(7), RX(65), RY(148), AModule );
    pGr->Attr.Style = 1 | WS_CHILD | WS_VISIBLE;

    PTBStatic  pSt = new TBStatic( this, -1, "&Bodys", RX(21), RY(9), RX(35), RY(8), 255, AModule );
    pSt->Attr.Style |= WS_CHILD | WS_VISIBLE | WS_GROUP;


								     //RY(56)
    //pGr = new TBGroupBox( this, 105, NULL, RX(198), RY(100), RX(44), RY(56), AModule );
    //pGr->Attr.Style = 1 | WS_CHILD | WS_VISIBLE;

									    
    PTBCheckBox pCh = new TBCheckBox( this, ID_CHECK_SMOTH, "&Smoothed", RX(96), RY(99), RX(53), RY(10), pGr, AModule );
    pCh->Attr.Style = BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE | WS_TABSTOP;

    pCh = new TBCheckBox( this, ID_CHECK_ALIGN, "&Aligmented", RX(96), RY(113), RX(53), RY(10), pGr, AModule );
    pCh->Attr.Style = BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE | WS_TABSTOP;

    PTBStaticBmp pBm = new TBStaticBmp( this, ID_SELECT_BMP, NULL, RX(200), RY(112), RX(40), RY(43), AModule );
    pBm->Attr.Style = BBS_BITMAP | BBS_OWNERDRAW | WS_CHILD | WS_VISIBLE;

    pSt = new TBStatic( this, 103, "Selected body", RX(196), RY(95), RX(47), RY(8), 255, AModule );
    pSt->Attr.Style |= WS_CHILD | WS_VISIBLE | WS_GROUP;

    pGr = new TBGroupBox( this, 106, NULL, RX(93), RY(7), RX(149), RY(84), AModule );
    pGr->Attr.Style = 1 | WS_CHILD | WS_VISIBLE;

    pSt = new TBStatic( this, ID_VALUE_TB1, NULL, RX(152), RY(42), RX(30), RY(8), 255, AModule );
    pSt->Attr.Style |= WS_CHILD | WS_VISIBLE | WS_GROUP;

    pSt = new TBStatic( this, ID_VALUE_TB2, NULL, RX(152), RY(75), RX(30), RY(9), 255, AModule );
    pSt->Attr.Style |= WS_CHILD | WS_VISIBLE | WS_GROUP;

    PTBButton pBt = new TBButton( this, IDOK, "&Ok", RX(89), RY(132), RX(36), RY(24), TRUE, AModule );
    pBt->Attr.Style = BS_DEFPUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP;

    pBt = new TBButton( this, IDCANCEL, "&Cancel", RX(151), RY(132), RX(36), RY(24), TRUE, AModule );
    pBt->Attr.Style = BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP;
						     //93, 96, 60, 30
    pGr = new TBGroupBox( this, 104, NULL, RX(93), RY(96), RX(60), RY(30), AModule );
    pGr->Attr.Style = 1 | WS_CHILD | WS_VISIBLE;


    *this,
      *new T3DThickFrame( this, ID_SELECT_BMP, 5, AModule );
      
  }

TFaceDialog::~TFaceDialog()
 {

   if( hf ) DeleteObject( hf ); hf = NULL;
 }

/*void TFaceDialog::WMCreate( RTMessage msg )
 {
   TCenterDialog::WMCreate( msg );

 }*/

static int ItemFind( const Object& rObj, Pvoid pPar )
 {
   return (((RTFaceItem)rObj).pFData->isA() == (UINT)pPar );
 }


void TFaceDialog::ReturnSelectAndDestroy()
 {
   RTFaceItem rItem = (RTFaceItem)pList->firstThat( ItemFind, (Pvoid)(*pSelIndex) );
   if( rItem != NOOBJECT )
    {
      rItem.pFData->bFlSmoothed = SendMessage( GetDlgItem(HWindow, ID_CHECK_SMOTH), BM_GETCHECK, 0, 0L );
      rItem.pFData->bFlAligmented = SendMessage( GetDlgItem(HWindow, ID_CHECK_ALIGN), BM_GETCHECK, 0, 0L );
    }
   
   rItem.pFData->DeleteControls();
 }

void TFaceDialog::ShutDownWindow( int ARetValue )
 {
   ReturnSelectAndDestroy();

   TCenterDialog::ShutDownWindow( ARetValue );
 }

void TFaceDialog::ShutDownWindow()
 {
   ReturnSelectAndDestroy();

   TCenterDialog::ShutDownWindow();
 }

void TFaceDialog::DefWndProc( RTMessage msg )
 {
   if( msg.Message == WM_HSCROLL || (msg.Message == WM_COMMAND && !msg.WParam) )
    {
      HWND w1 = NULL;
      HWND w2 = NULL;
      if( sc1 != -1 ) w1 = GetDlgItem( HWindow, sc1 );
      if( sc2 != -1 ) w2 = GetDlgItem( HWindow, sc2 );

      int idToSend = ((HWND)HIWORD(msg.LParam) == w1) ? ID_VALUE_TB1:ID_VALUE_TB2;
       
      char cBuf[10];
      PTMyScrollBar pSc = (PTMyScrollBar)ChildWithId(( (idToSend == ID_VALUE_TB1) ? sc1:sc2 ) );
      if( pSc )
	sprintf( cBuf, "%5d", pSc->GetPosition() );

      int idT;           //TMyScrollBar
      SetDlgItemText( HWindow, idToSend, cBuf );
      InvalidateRect( GetDlgItem(HWindow, idToSend), NULL, FALSE );

      msg.Result = 0;
      return;
    }

   if( msg.Message == WM_MEASUREITEM )
    {
      MEASUREITEMSTRUCT FAR* pMeasur = (MEASUREITEMSTRUCT FAR*)msg.LParam;
                          //DoubleList
      RTFaceItem rItem = (RTFaceItem)pList->peekAtHead();
      if( rItem != NOOBJECT )
       {
	 BITMAP bmpInf;
	 GetObject( rItem.GetHandleN(),
           sizeof(BITMAP), (LPSTR)&bmpInf );

	 pMeasur->itemWidth = bmpInf.bmWidth;
	 pMeasur->itemHeight = bmpInf.bmHeight;
       }

      msg.Result = 0;
      return;
    }

   if( msg.Message == WM_COMMAND )
    {
      if( HIWORD(msg.LParam) == LBN_SELCHANGE || HIWORD(msg.LParam) == LBN_DBLCLK )
       {
	 int sel = SendDlgItemMessage( HWindow, ID_LIST, LB_GETCURSEL, 0, 0L ) + 1;

	 if( sel == *pSelIndex && HIWORD(msg.LParam) != LBN_DBLCLK )
	  {
	    msg.Result = 0;
	    return;
          }

         RTFaceItem rItem = (RTFaceItem)pList->firstThat( ItemFind, (Pvoid)sel );
         if( rItem != NOOBJECT )
          {
	    RTFaceItem rItemOld = (RTFaceItem)pList->firstThat( ItemFind, (Pvoid)(*pSelIndex) );
	    if( rItemOld != NOOBJECT ) rItemOld.pFData->DeleteControls();

	    rItem.pFData->InitControls( hf, GetApplication(), HWindow );
	    *pSelIndex = (FaceTypes)sel;

	    InvalidateRect( GetDlgItem(HWindow, ID_SELECT_BMP), NULL, FALSE );

	    if( HIWORD(msg.LParam) == LBN_DBLCLK )
	     {
	       TMessage msg;
               _fsetmem( &msg, sizeof(TMessage), 0 );
               msg.Receiver = HWindow;
               msg.Message = WM_COMMAND;
	       msg.WParam = IDOK;
	       msg.LParam = MAKELONG(HWindow, BN_CLICKED);
	       Ok( msg );
             }
	  }         

         msg.Result = 0;
         return;
       }
    }


   TCenterDialog::DefWndProc( msg );
 }

void TFaceDialog::DrawItem( DRAWITEMSTRUCT _FAR & DrawInfo )
 {
   if( DrawInfo.CtlID == ID_SELECT_BMP )
    {
      RTFaceItem rItem = (RTFaceItem)pList->firstThat( ItemFind, (Pvoid)(*pSelIndex) );
      if( rItem == NOOBJECT ) return;

      DrawBmp( DrawInfo.hDC, rItem.GetHandleH() );
      return;
    }

   RTFaceItem rItem = (RTFaceItem)pList->firstThat( ItemFind, (Pvoid)(DrawInfo.itemID + 1) );
   if( rItem == NOOBJECT ) return;
   HBITMAP hbmDR;

   if( (DrawInfo.itemState & (ODS_FOCUS | ODS_SELECTED)) == (ODS_FOCUS | ODS_SELECTED) )
     hbmDR = rItem.GetHandleH();
   else hbmDR = rItem.GetHandleN();
   DrawBmpInRect( DrawInfo.hDC, hbmDR, DrawInfo.rcItem );

 }

LPSTR TFaceDialog::GetClassName()
 {
   return "3DW:FACE";
 }

void TFaceDialog::GetWindowClass( WNDCLASS FAR & AWndClass )
 {
   TCenterDialog::GetWindowClass( AWndClass );

   AWndClass.style |= CS_SAVEBITS;
 }


void TFaceDialog::SetupWindow()
 {
   SetWindowLong( HWindow, GWL_STYLE, GetWindowLong(HWindow, GWL_STYLE) | WS_CLIPCHILDREN );

   MoveWindow( HWindow, 1, 1, RX(251) + 15, RY(162) + 37, FALSE );

   SetFontGlobal( hf );
   TCenterDialog::SetupWindow();
   SetFontGlobal( hf );

   /*
   SendDlgItemMessage( HWindow, ID_LIST, WM_SETFONT, (WPARAM)hf, FALSE );
   SendDlgItemMessage( HWindow, 102, WM_SETFONT, (WPARAM)hf, FALSE );

   SendDlgItemMessage( HWindow, -1, WM_SETFONT, (WPARAM)hf, FALSE );
   SendDlgItemMessage( HWindow, ID_CHECK_SMOTH, WM_SETFONT, (WPARAM)hf, FALSE );

   SendDlgItemMessage( HWindow, ID_SELECT_BMP, WM_SETFONT, (WPARAM)hf, FALSE );
   SendDlgItemMessage( HWindow, 105, WM_SETFONT, (WPARAM)hf, FALSE );

   SendDlgItemMessage( HWindow, 106, WM_SETFONT, (WPARAM)hf, FALSE );
   SendDlgItemMessage( HWindow, ID_VALUE_TB1, WM_SETFONT, (WPARAM)hf, FALSE );
   SendDlgItemMessage( HWindow, ID_VALUE_TB2, WM_SETFONT, (WPARAM)hf, FALSE );

   SendDlgItemMessage( HWindow, IDOK, WM_SETFONT, (WPARAM)hf, FALSE );
   SendDlgItemMessage( HWindow, IDCANCEL, WM_SETFONT, (WPARAM)hf, FALSE );

   SendDlgItemMessage( HWindow, 103, WM_SETFONT, (WPARAM)hf, FALSE );
   SendDlgItemMessage( HWindow, 104, WM_SETFONT, (WPARAM)hf, FALSE );
     */

   DoubleListIterator dbiIter = DoubleListIterator( *pList, FALSE );
   PTFaceItem pItem =  (PTFaceItem)&dbiIter.current();

   for( int ind = 0 ; int(dbiIter); pItem = (PTFaceItem)&--dbiIter, ind++ )
    {
      SendDlgItemMessage( HWindow, ID_LIST, LB_ADDSTRING, 0,  (LPARAM)" " );
      pItem->pFData->SetOwner( this );
    }

   SendDlgItemMessage( HWindow, ID_LIST, LB_SETCURSEL, *pSelIndex - 1,  0L );
   //InvalidateRect( GetDlgItem(HWindow, ID_LIST), NULL, FALSE );
   SetFocus( GetDlgItem(HWindow, ID_LIST) );

   RTFaceItem rItem = (RTFaceItem)pList->firstThat( ItemFind, (Pvoid)(*pSelIndex) );
   if( rItem == NOOBJECT ) return;

   rItem.pFData->DeleteControls();
   rItem.pFData->InitControls( hf, GetApplication(), HWindow );
 }

/*void TFaceDialog::WMCreate( RTMessage msg )
 {
   TCenterDialog::DefWndProc( msg );

   SendDlgItemMessage( HWindow, ID_LIST, LB_SETCURSEL, 0,  0L );
   InvalidateRect( GetDlgItem(HWindow, ID_SELECT_BMP), NULL, FALSE );

   RTFaceItem rItem = (RTFaceItem)pList->firstThat( ItemFind, (Pvoid)(*pSelIndex) );
   if( rItem == NOOBJECT ) return;

   rItem.pFData->DeleteControls();
   rItem.pFData->InitControls( hf, GetApplication(), HWindow );
 }*/

void TMyScrollBar::SBThumbTrack( RTMessage msg )
 {
   TScrollBar::SBThumbTrack( msg );
                                          //WM_HSCROLL
   SendMessage( Parent->HWindow, WM_COMMAND, 0, msg.LParam );
 }


void TMyScrollBar::SetPosition_( int ThumbPos )
 {
   TScrollBar::SetPosition( ThumbPos );
   if( Parent )
     SendMessage( Parent->HWindow, WM_COMMAND, 0,
       MAKELONG(ThumbPos, HWindow) );
 }


