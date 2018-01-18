#pragma option -v-

#include <bbutton.h>//TBButton
#include <bwcc.h>
#include <windowsx.h>

#include "bpdlg.hpp"
#include "bmp.hpp"
#include "status.hpp"
#include "INC\errors.inc"
#include "autorc.hpp"
#include "INC\io_em.inc"



#define BPDLG_WIDTH 620
#define BPDLG_HIGH  420

#define BPDLG_COMMANDBTN_WIDTH 128
#define BPDLG_COMMANDBTN_HIGH  80
#define BPDLG_COMMANDBTN_SPC_DOWN 15
#define BPDLG_COMMANDBTN_SPC_X 20
#define BPDLG_SHIFTY_BODY_BMP  20


#define BPDLG_CHECKBTN_WIDTH 141
#define BPDLG_CHECKBTN_HIGH  62
#define BPDLG_CHECKBTN_SPC_X 40
#define BPDLG_CHECKBTN_SPC_Y 20

#define BPDLG_GROUP_WIDTH 555
#define BPDLG_GROUP_HIGH  257
#define BPDLG_GROUP_Y 15

#define BPDLG_GROUP2_WIDTH 150
#define BPDLG_GROUP2_HIGH  100

#define BPDLG_RIBBON_WIDTH 150
#define BPDLG_RIBBON_HIGH  5


#define BPDLG_ID_GROUP  100
#define BPDLG_ID_GROUP2 101


#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;

TBPDialog::TBPDialog( PTWindowsObject AParent, LPSTR AName, RTPlaicement rPlace, PTModule AModule ):
  TCenterDialog( AParent, AName, AModule ),
  pPlace( &rPlace )
 {
   hf = NULL;
   _fsetmem( bbControlBmp, sizeof(TBPDialog::TBtnBmp) * BPDLG_NUMB_CONTROL_BMP, 0 );
   _fsetmem( bbCheckBmp, sizeof(TBPDialog::TBtnBmp) * BPDLG_NUMB_CHECK_BMP, 0 );

   int iWidthClient = BPDLG_WIDTH - 2 * GetSystemMetrics(SM_CXDLGFRAME),
       iHighClient = BPDLG_HIGH - 2 * GetSystemMetrics(SM_CYDLGFRAME) - GetSystemMetrics(SM_CYCAPTION);
   int x, y;

   int xGroup = x = (iWidthClient - BPDLG_GROUP_WIDTH) / 2;
   //y = (iHighClient - BPDLG_GROUP_HIGH) / 2;

   pMGr = new TMainGroup( this, BPDLG_ID_GROUP, NULL, x, BPDLG_GROUP_Y,
     BPDLG_GROUP_WIDTH, BPDLG_GROUP_HIGH, rPlace, AModule );
   pMGr->Attr.Style = 1 | WS_CHILD | WS_VISIBLE | WS_TABSTOP;

   int xBodyBtn = (iWidthClient - BPDLG_NUMB_CONTROL_BMP * BPDLG_COMMANDBTN_WIDTH -
	(BPDLG_NUMB_CONTROL_BMP - 1) * BPDLG_COMMANDBTN_SPC_X) / 2 +
	2 * (BPDLG_COMMANDBTN_SPC_X + BPDLG_COMMANDBTN_WIDTH),
       yBodyBtn = iHighClient - BPDLG_COMMANDBTN_SPC_DOWN - BPDLG_COMMANDBTN_HIGH -
                  BPDLG_SHIFTY_BODY_BMP;
   x = xBodyBtn + (BPDLG_COMMANDBTN_WIDTH - BPDLG_GROUP2_WIDTH) / 2;
   y = yBodyBtn + (BPDLG_COMMANDBTN_HIGH - BPDLG_GROUP2_HIGH) / 2;
   PTBGroupBox pStat = new TBGroupBox( this, BPDLG_ID_GROUP2, NULL, x, y,
     BPDLG_GROUP2_WIDTH, BPDLG_GROUP2_HIGH, AModule );
   pStat->Attr.Style = 1 | WS_CHILD | WS_VISIBLE;

   PTBStatic pStatR = new TBStatic( this, BPDLG_ID_GROUP2 + 1, NULL,
     x, y,
     BPDLG_RIBBON_WIDTH, BPDLG_RIBBON_HIGH, 0, AModule );
   pStatR->Attr.Style = WS_CHILD | WS_VISIBLE | WS_GROUP;

   x = (iWidthClient - BPDLG_NUMB_CONTROL_BMP * BPDLG_COMMANDBTN_WIDTH -
	(BPDLG_NUMB_CONTROL_BMP - 1) * BPDLG_COMMANDBTN_SPC_X) / 2;
   y = iHighClient - BPDLG_COMMANDBTN_SPC_DOWN - BPDLG_COMMANDBTN_HIGH;
   int iCurId = ID_FIRST_BPDLG + 1;
   for( int i = 0; i < BPDLG_NUMB_CONTROL_BMP; i++, iCurId++,
	x += BPDLG_COMMANDBTN_SPC_X + BPDLG_COMMANDBTN_WIDTH
      )
     new TBButton( this, iCurId, NULL, x, (i == 2) ? yBodyBtn:y, BPDLG_COMMANDBTN_WIDTH,
       BPDLG_COMMANDBTN_HIGH, i ? FALSE:TRUE, AModule );
      

   int keyX = x = 0 + (BPDLG_GROUP_WIDTH - 3 * BPDLG_CHECKBTN_WIDTH - 2 * BPDLG_CHECKBTN_SPC_X) / 2;
   int keyY = y = 0 + (BPDLG_GROUP_HIGH - 3 * BPDLG_CHECKBTN_HIGH - 2 * BPDLG_CHECKBTN_SPC_Y) / 2;

   iCurId = PL_TopLeft;
   for( i = 0; i < 3; i++, y += BPDLG_CHECKBTN_HIGH +BPDLG_CHECKBTN_SPC_Y )
    {
      x = keyX;
      for( int j = 0; j < 3; j++,
	   x += BPDLG_CHECKBTN_WIDTH +BPDLG_CHECKBTN_SPC_X,
	   iCurId++
	 )
       {
	 PTBButton pBtn = new TBButton( pMGr, iCurId, NULL, x, y, BPDLG_CHECKBTN_WIDTH,
	  BPDLG_CHECKBTN_HIGH, FALSE, AModule );
	 pBtn->Attr.Style |= BBS_OWNERDRAW;
       }
    }
 }

TBPDialog::~TBPDialog()
 {
   if( hf ) { DeleteObject( hf ); hf = NULL; }

   for( int i = 0; i < BPDLG_NUMB_CONTROL_BMP; i++ )
    {
      if( bbControlBmp[i].hbmN ) DeleteObject( bbControlBmp[i].hbmN );
      if( bbControlBmp[i].hbmH ) DeleteObject( bbControlBmp[i].hbmH );
      if( bbControlBmp[i].hbmS ) DeleteObject( bbControlBmp[i].hbmS );
    }
   _fsetmem( bbControlBmp, sizeof(TBPDialog::TBtnBmp) * BPDLG_NUMB_CONTROL_BMP, 0 );
     
   for( i = 0; i < BPDLG_NUMB_CHECK_BMP; i++ )
    {
      if( bbCheckBmp[i].hbmN ) DeleteObject( bbCheckBmp[i].hbmN );
      if( bbCheckBmp[i].hbmH ) DeleteObject( bbCheckBmp[i].hbmH );
      if( bbCheckBmp[i].hbmS ) DeleteObject( bbCheckBmp[i].hbmS );
    }
   _fsetmem( bbCheckBmp, sizeof(TBPDialog::TBtnBmp) * BPDLG_NUMB_CHECK_BMP, 0 );
 }

void TBPDialog::SetupWindow()
 {

   //BPDLG_ID_GROUP2 + 1

   //static char *cInf = "Select placement of global base point.";
   TSendTxt stSend;
   stSend.handle = 0;
   TAutoStringRc str_( hInstRC, ID_MSG_31 );
   //stSend.lpTxt = cInf;
   stSend.lpTxt = str_();

   MoveWindow( HWindow, 0, 0, BPDLG_WIDTH, BPDLG_HIGH, FALSE );

   TCenterDialog::SetupWindow();

   SendMessage( Parent->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );

   hf = CreateFont( -8, 0, 0, 0, FW_BOLD,
     0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_STROKE_PRECIS,
     DEFAULT_QUALITY, DEFAULT_PITCH, "Helv" );

   SetFontGlobal( hf );

   HCURSOR hcOld = SetCursor( LoadCursor(NULL, IDC_WAIT) );
   HWND hwndOldCap = SetCapture( HWindow );


   int iCurId = ID_FIRST_BPDLG + 1;
   int iIdBmpN = BPDLGBMP_POINT_N,
       iIdBmpH = BPDLGBMP_POINT_H,
       iIdBmpS = BPDLGBMP_POINT_S;
   for( int i = 0; i < BPDLG_NUMB_CONTROL_BMP; i++, iCurId++,
	iIdBmpN++, iIdBmpH++, iIdBmpS++
      )
    {      
      bbControlBmp[i].hbmN = LoadBitmap( hInstRC, MAKEINTRESOURCE(iIdBmpN) );
      bbControlBmp[i].hbmH = LoadBitmap( hInstRC, MAKEINTRESOURCE(iIdBmpH) );
      bbControlBmp[i].hbmS = LoadBitmap( hInstRC, MAKEINTRESOURCE(iIdBmpS) );

      if( !bbControlBmp[i].hbmN || !bbControlBmp[i].hbmH || !bbControlBmp[i].hbmS )
       {      
	 ReleaseCapture();
	 SetCapture( hwndOldCap );
	 SetCursor( hcOld );
	 Status = ER_BITMAPLOAD;
	 GetApplication()->Error( ER_BITMAPLOAD );
         TMessage msg; _fsetmem( &msg, sizeof(TMessage), 0 );
	 Cancel( msg );
	 return;
       }

      SendDlgItemMessage( HWindow, iCurId, BBM_SETBITS, 0, (LONG)(LPSTR)(&bbControlBmp[i]) );
    }

   iCurId = PL_TopLeft;
   iIdBmpN = BPDLGBMP_TOPLEFT_N;
   iIdBmpH = BPDLGBMP_TOPLEFT_H;
   iIdBmpS = BPDLGBMP_TOPLEFT_S;
   for( i = 0; i < BPDLG_NUMB_CHECK_BMP; i++, iCurId++,
	iIdBmpN++, iIdBmpH++, iIdBmpS++
      )
    {
      bbCheckBmp[i].hbmN = LoadBitmap( hInstRC, MAKEINTRESOURCE(iIdBmpN) );
      bbCheckBmp[i].hbmH = LoadBitmap( hInstRC, MAKEINTRESOURCE(iIdBmpH) );
      bbCheckBmp[i].hbmS = LoadBitmap( hInstRC, MAKEINTRESOURCE(iIdBmpS) );

      if( !bbCheckBmp[i].hbmN || !bbCheckBmp[i].hbmH || !bbCheckBmp[i].hbmS )
       {
	 ReleaseCapture();
	 SetCapture( hwndOldCap );
	 SetCursor( hcOld );
	 Status = ER_BITMAPLOAD;
	 GetApplication()->Error( ER_BITMAPLOAD );
         TMessage msg; _fsetmem( &msg, sizeof(TMessage), 0 );
	 Cancel( msg );
         return;
       }

      //SendDlgItemMessage( pMGr->HWindow, iCurId, BBM_SETBITS, 0, (LONG)(LPSTR)bbCheckBmp[i] );
    }

   ReleaseCapture();
   SetCapture( hwndOldCap );
   SetCursor( hcOld );

   HWND hwnd = GetDlgItem( HWindow, ID_POINT );
   if( hwnd ) SetWindowPos( hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER );
 }



void TMainGroup::WMCommand( RTMessage msg )
 {
   if( HIWORD(msg.LParam) == BN_CLICKED ) *pPlace = (TPlaicement)msg.WParam;
   else TBGroupBox::WMCommand( msg );
 }

void TBPDialog::WMCommand( RTMessage msg )
 {
   if( HIWORD(msg.LParam) == BN_CLICKED )
     CloseWindow( msg.WParam );
   else TDialog::WMCommand( msg );
 }

void TMainGroup::DrawItem( DRAWITEMSTRUCT _FAR & DrawInfo )
 {
   if( Parent ) PTBPDialog(Parent)->DrawItem( DrawInfo );
 }

void TBPDialog::DrawFrame( HWND hwndOwner, RECT _FAR &rRect, BOOL bFlDraw )
 {
   HDC hdc = GetDC( hwndOwner );

   HPEN hpBord = CreatePen( PS_SOLID, 2,
     (bFlDraw == TRUE) ? RGB(0, 0, 0):RGB(0xC0, 0xC0, 0xC0) );
   HPEN hpOld = SelectPen( hdc, hpBord );
   HBRUSH hbrOld = SelectBrush( hdc, (HBRUSH)GetStockObject(NULL_BRUSH) );

   Rectangle( hdc, rRect.left - 1, rRect.top - 1, rRect.right + 2, rRect.bottom + 2 );

   SelectBrush( hdc, hbrOld );
   SelectPen( hdc, hpOld );
   DeleteObject( hpBord );

   ReleaseDC( hwndOwner, hdc );
 }

void TBPDialog::DrawItem( DRAWITEMSTRUCT _FAR & DrawInfo )
 {
   if( DrawInfo.CtlID >= PL_TopLeft && DrawInfo.CtlID <= PL_BottomRight )
    {

	DrawBmp( DrawInfo.hDC, (DrawInfo.itemState & ODS_SELECTED) ?
				 bbCheckBmp[DrawInfo.CtlID - PL_TopLeft].hbmH:
				 ((DrawInfo.itemState & ODS_FOCUS || *pPlace == DrawInfo.CtlID) ?
				   bbCheckBmp[DrawInfo.CtlID - PL_TopLeft].hbmS:
				   bbCheckBmp[DrawInfo.CtlID - PL_TopLeft].hbmN)
	       );

	PTButton pPrivBtn = (PTButton)pMGr->ChildWithId( DrawInfo.CtlID );
	if( pPrivBtn )
	 {
	   RECT r;
	   r.left = pPrivBtn->Attr.X; r.top = pPrivBtn->Attr.Y;
	   r.right = r.left + pPrivBtn->Attr.W;
	   r.bottom = r.top + pPrivBtn->Attr.H;

	   DrawFrame( pMGr->HWindow, r,
	     (DrawInfo.itemState & (ODS_SELECTED | ODS_FOCUS) || *pPlace == DrawInfo.CtlID) ? TRUE:FALSE );
         }

	if( (DrawInfo.itemState & ODS_SELECTED) && *pPlace != DrawInfo.CtlID )
         {
	   PTButton pPrivBtn = (PTButton)pMGr->ChildWithId( *pPlace );
	   if( pPrivBtn )
	    {
	      HDC hdc = GetDC( pPrivBtn->HWindow );
	      DrawBmp( hdc, bbCheckBmp[ *pPlace - PL_TopLeft ].hbmN );
	      ReleaseDC( pPrivBtn->HWindow, hdc );

	      RECT r;
	      r.left = pPrivBtn->Attr.X; r.top = pPrivBtn->Attr.Y;
	      r.right = r.left + pPrivBtn->Attr.W;
	      r.bottom = r.top + pPrivBtn->Attr.H;

	      DrawFrame( pMGr->HWindow, r, FALSE );
	    }
	 }
    }
 }
