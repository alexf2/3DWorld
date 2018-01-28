#pragma option -v-

#include <bbutton.h>
#include <bradio.h>
#include <bgrpbox.h>
#include <BCHKBOX.H>
//#include <owl.H>

#include "viewsdlg.hpp"
#include "status.hpp"
#include "measur.mac"
#include "bmp.hpp"

/*#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif
  */

extern HINSTANCE hInstRC;


TXlatElem TViewsDlg::iXlatTbl[cViews] =
 {
   { ID_TOP, TUCS::VP_Top },
   { ID_BOTTOM, TUCS::VP_Bottom },
   { ID_USER, TUCS::VP_User },
   { ID_LEFT, TUCS::VP_Left },
   { ID_RIGHT, TUCS::VP_Right },
   { ID_CAMERA, TUCS::VP_Camera },
   { ID_FRONT, TUCS::VP_Front },
   { ID_BACK, TUCS::VP_Back },
   { ID_NONE, TUCS::VP_None }
 };

int TViewsDlg::iBmpDivBtn[ cDivBtn ][ 2 ] =
 {
  { VBMP_N_DS_FullWin, VBMP_H_DS_FullWin },
  { VBMP_N_DS_HalfVert, VBMP_H_DS_HalfVert },
  { VBMP_N_DS_HalfHor, VBMP_H_DS_HalfHor },
  { VBMP_N_DS_4, VBMP_H_DS_4 },
  { VBMP_N_DS_HalfVertAndLeftHor, VBMP_H_DS_HalfVertAndLeftHor },
  { VBMP_N_DS_HalfHorAndDownVert, VBMP_H_DS_HalfHorAndDownVert },
  { VBMP_N_DS_HalfVertAndRightHor, VBMP_H_DS_HalfVertAndRightHor },
  { VBMP_N_DS_HalfHorAndUpVert, VBMP_H_DS_HalfHorAndUpVert },
  { VBMP_N_DS_3Vert, VBMP_H_DS_3Vert },
  { VBMP_N_DS_3Hor, VBMP_H_DS_3Hor },
  { VBMP_N_DS_FullAndLeft3, VBMP_H_DS_FullAndLeft3 },
  { VBMP_N_DS_FullAndRight3, VBMP_H_DS_FullAndRight3 }
 };

int TViewsDlg::iIds[ cViewBtn + 2 ] =
 {
   ID_GRP_DIV,
   ID_GRP_VIEWS,

   ID_TOP,
   ID_BOTTOM,
   ID_USER,
   ID_LEFT,
   ID_RIGHT,
   ID_CAMERA,
   ID_FRONT,
   ID_BACK,
   ID_NONE
 };

TViewBtnDta TViewsDlg::vbDta[ cViewBtn ] =
 {
   {ID_TOP, "&Top"},
   {ID_BOTTOM, "&Bottom"},
   {ID_USER, "&User"},
   {ID_LEFT, "&Left"},
   {ID_RIGHT, "&Right"},
   {ID_CAMERA, "Ca&mera"},
   {ID_FRONT, "&Front"},
   {ID_BACK, "&Back"},
   {ID_NONE, "&None"}
 };

TViewsDlg::TViewsDlg( PTWindowsObject AParent, LPCSTR AName, PTViewDlgTransfer pTr,
    PTModule AModule ):
  TCenterDialog( AParent, (LPSTR)AName, AModule )
 {
   this->pTr = pTr;

   int iHSpc = 20, iVSpc = 27,
       iWidt = 45, iHigh = 36,
       iStaX = 45, iStaY = 24;
   RECT rect;
   rect.left = iStaX; rect.right = rect.left + iWidt;
   rect.top = iStaY; rect.bottom = rect.top + iHigh;
   hfHelv = 0;

   RECT r;
   r.left = 20; r.right = r.left + 180;
   r.top = 150; r.bottom = r.top + 100;
   pVinDivisor = new TViewDivisor( this, r, pTr->pWD );
                                 //////////
   pGrpDiv = new TMyGroupBox( this, ID_GRP_DIV, NULL, RX(19), RY(8), RX(226), RY(76), AModule );
   pGrpDiv->Attr.Style |=  1 | WS_CHILD | WS_VISIBLE | BBS_PARENTNOTIFY;
   pGrpDiv->NotifyParent = TRUE;

   int idBtn = ID_DIVBTN_FIRST;
   int count = 0;
   for( int i = 0; i < 2; i++ )
    {
      rect.left = iStaX; rect.right = rect.left + iWidt;
      for( int j = 0; j < 6; j++ )
       {
	 //PTDivButton pBtn;
	 PTMyRadioButton pBtn;

	 //pBtn = new TDivButton( pGrpDiv, idBtn++, NULL, rect.left, rect.top,
	   //iWidt, iHigh, FALSE );
	 //pBtn->Attr.Style = BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP;
	 pBtn = new TMyRadioButton( this, idBtn++, NULL, rect.left, rect.top,
	   iWidt, iHigh, pGrpDiv, AModule );         //BBS_PARENTNOTIFY BBS_OWNERDRAW
	 pBtn->Attr.Style |= BS_RADIOBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP | BBS_PARENTNOTIFY | BBS_OWNERDRAW;
	 rect.left += iWidt + iHSpc; rect.right = rect.left + iWidt;

	 keyBtnDiv[count++] = pBtn;
       }
      rect.top += iHigh + iVSpc; rect.bottom =  rect.top + iHigh;
    } 

   iHSpc = 15, iVSpc = 12,
   iWidt = 55, iHigh = 22,
   iStaX = RX(129) + 11, iStaY = RY(93) + 10;

   rect.left = iStaX; rect.right = rect.left + iWidt;
   rect.top = iStaY; rect.bottom = rect.top + iHigh;

   pGrpView = new TMyGroupBox( this, ID_GRP_VIEWS, NULL, RX(129), RY(93), RX(124), RY(68), AModule );
   pGrpView->Attr.Style = 1 | WS_CHILD | WS_VISIBLE | BBS_PARENTNOTIFY;
   pGrpView->NotifyParent = TRUE;
   //CONTROL "", 101, "BorShade", , 19, 8, 226, 76
   //CONTROL "", 102, "BorShade", 1 | WS_CHILD | WS_VISIBLE, 129, 93, 124, 68

   count = 0;
   for( i = 0; i < 3; i++ )
    {
      rect.left = iStaX; rect.right = rect.left + iWidt;
      for( int j = 0; j < 3; j++ )
       {
	 PTButton pBtn;

	 pBtn = new TButton( this, vbDta[count].id, vbDta[count].chTtl,
	    rect.left, rect.top, iWidt, iHigh, FALSE, AModule );
	 pBtn->Attr.Style = BS_PUSHBUTTON | BS_LEFTTEXT | WS_CHILD | WS_VISIBLE | WS_TABSTOP;
	 rect.left += iWidt + iHSpc; rect.right = rect.left + iWidt;
	 count++;
       }
      rect.top += iHigh + iVSpc; rect.bottom =  rect.top + iHigh;
    }

   curView = TUCS::VP_Front;
   //this->pTr = pTr;
   _fsetmem( keyBmp, sizeof(keyBmp), 0 );

   //24, 171, 36, 24
   pBOk = new TBButton( this, IDOK, "&Ok", RX(24), RY(171), RX(36), RY(24), TRUE, AModule );
   ((PTBButton)pBOk)->Attr.Style = BS_DEFPUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP;

   //103, 171, 36, 24
   pBCancel = new TBButton( this, IDCANCEL, "&Cancel", RX(103), RY(171), RX(36), RY(24), FALSE, AModule );
   ((PTBButton)pBCancel)->Attr.Style = BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP;

   //6, 162, 112, 3
   pGr1 = new TBGroupBox( this, 103, NULL, RX(6), RY(162), RX(112), RY(3), AModule );
   ((PTBGroupBox)pGr1)->Attr.Style = BSS_HDIP | WS_CHILD | WS_VISIBLE;

   //167, 167, 74, 3
   pGr2 = new TBGroupBox( this, ID_CHECK_GRP, NULL, RX(167), RY(167), RX(74), RY(30), AModule );
   ((PTBGroupBox)pGr2)->Attr.Style = 1 | WS_CHILD | WS_VISIBLE;
   

   //174, 170, 63, 11
   pChb1 = new TBCheckBox( this, ID_CHECK_DEF, (LPSTR)"&Use as default", RX(174), RY(170), RX(63), RY(11), (PTBGroupBox)pGr2, AModule );
   ((PTBCheckBox)pChb1)->Attr.Style = BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE | WS_TABSTOP;

   //174, 185, 63, 10  TRadioButton
   pChb2 = new TBCheckBox( this, ID_CHECK_INIT, (LPSTR)"&Init always", RX(174), RY(185), RX(63), RY(10), (PTBGroupBox)pGr2, AModule );
   ((PTBCheckBox)pChb2)->Attr.Style = BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE | WS_TABSTOP;

   bFlLockErase = 0;
 }

classType TViewsDlg::isA() const
 {
   return classtypeViewsDialog;
 }


BOOL CALLBACK _export DefDlgProcBWCC( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )//TDialog
 {
   if( msg == WM_ERASEBKGND )
    {
      //PTWindowsObject pDlg = GetObjectPtr( hwnd );
      //PTWindowsObject pDlg = (PTWindowsObject)MK_FP(_ES, _BX);
      PTWindowsObject pDlg = (PTWindowsObject)GetWindowLong( hwnd, DWL_USER );
      if( pDlg && pDlg->isA() == classtypeViewsDialog && PTViewsDlg(pDlg)->GetEraseFlag() == TRUE )
        return 0;
    }

   return CallWindowProc( BWCCDefDlgProc, hwnd, msg, wParam, lParam );
 }


PTXlatElem TViewsDlg::FindXlat( int code ) const
 {
   for( int i = 0; i < cViews; i++ )
     if( iXlatTbl[i].idButton == code || iXlatTbl[i].idView == code )
       return iXlatTbl + i;

   return NULL;
 }


int TViewsDlg::GetIdXlat( int view ) const
 {
   PTXlatElem pElem;

   if( (pElem = FindXlat(view)) ) return pElem->idButton;
   else return -1;
 }

int TViewsDlg::GetViewXlat( int button ) const
 {
   PTXlatElem pElem;

   if( (pElem = FindXlat(button)) ) return pElem->idView;
   else return -1;
 }

void TViewsDlg::Ok( RTMessage msg )
 {
   pTr->iUseAsDefault = SendMessage( GetDlgItem(HWindow, ID_CHECK_DEF), BM_GETCHECK, 0, 0L );
   pTr->iInitAlways = SendMessage( GetDlgItem(HWindow, ID_CHECK_INIT), BM_GETCHECK, 0, 0L );
   //////
   TCenterDialog::Ok( msg );
 }

TViewsDlg::~TViewsDlg()
 {
   for( int i = 0; i < cDivBtn; i++ )
    {
     if( keyBmp[i][0] ) DeleteObject( keyBmp[i][0] );
     if( keyBmp[i][1] ) DeleteObject( keyBmp[i][1] );
    }
   _fsetmem( keyBmp, cDivBtn * sizeof(HBITMAP) * 2, 0 );

   if( hfHelv ) { DeleteObject( hfHelv );  hfHelv = NULL; }

   pVinDivisor->GetViews( pTr->pWD );
   delete pVinDivisor;
 }

void TViewsDlg::SetupWindow()//TWindow
 {
   SetWindowLong( HWindow, DWL_USER, (LONG)PTWindowsObject(this) );

   static char *cInf = "Select new screen division for file and vpoints. Press OK for accept, Cancel for close dialog";
   TSendTxt stSend;
   stSend.handle = 0;
   stSend.lpTxt = cInf;

   MoveWindow( HWindow, 0, 0, RX(264) + 10, RY(203) + 35, FALSE );
   //27, 25, 264, 203
   TCenterDialog::SetupWindow();


   SendMessage( Parent->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );


   HCURSOR hcOld = SetCursor( LoadCursor(NULL, IDC_WAIT) );
   HWND hwndOldCap = SetCapture( HWindow );


   int idBtn = ID_DIVBTN_FIRST;   
   for( int i = 0; i < cDivBtn; i++, idBtn++ )
    { 
      HBITMAP hBmp[3];

      keyBmp[i][0] = hBmp[0] = LoadBitmap( hInstRC,
	MAKEINTRESOURCE(iBmpDivBtn[i][0]) ); //norm
      keyBmp[i][1] = hBmp[1] = LoadBitmap( hInstRC,
	MAKEINTRESOURCE(iBmpDivBtn[i][1]) ); //hightlight
      hBmp[2] = hBmp[1];//focus

      if( !keyBmp[i][0] || !keyBmp[i][1]  )
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

    }   

   ReleaseCapture();
   SetCapture( hwndOldCap );
   SetCursor( hcOld );

   hfHelv = CreateFont( -8, 0, 0, 0, FW_BOLD,
     0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_STROKE_PRECIS,
     DEFAULT_QUALITY, DEFAULT_PITCH, "Helv" );

   SetFontGlobal( hfHelv );

   for( i = 2; i < cViewBtn + 2; i++ )
     SendDlgItemMessage( pGrpView->HWindow, iIds[i], WM_SETFONT, (WPARAM)hfHelv, FALSE );

   SetFocus( GetDlgItem(pGrpDiv->HWindow, GetIdXlat(curView)) );

   SendMessage( GetDlgItem(HWindow, ID_CHECK_DEF), BM_SETCHECK, pTr->iUseAsDefault, 0L );
   SendMessage( GetDlgItem(HWindow, ID_CHECK_INIT), BM_SETCHECK, pTr->iInitAlways, 0L );
 }

void TViewsDlg::WMCommand( RTMessage Msg )//BN_CLICKED
 {
   BOOL bFlPrep = FALSE;

   if( Msg.WParam == ID_GRP_VIEWS )////
    {      
      curView = GetViewXlat( HIWORD(Msg.LParam) );
      if( curView > -1 ) bFlPrep = TRUE;
    }
   else if( (HIWORD(Msg.LParam) == BN_CLICKED || HIWORD(Msg.LParam) == BN_DOUBLECLICKED) &&
     Msg.WParam >= ID_DIVBTN_FIRST &&
     Msg.WParam < ID_DIVBTN_FIRST + cDivBtn )
    {
      pTr->pWD->Style = Msg.WParam - ID_DIVBTN_FIRST;
      if( pVinDivisor->GetStyle() != pTr->pWD->Style )
	pVinDivisor->ResetDivision( pTr->pWD->Style );
      bFlPrep = TRUE;

      if( HIWORD(Msg.LParam) == BN_DOUBLECLICKED )
       {
         TMessage msg;
         _fsetmem( &msg, sizeof(TMessage), 0 );
         msg.Receiver = pBOk->HWindow;
         msg.Message = WM_COMMAND;
         msg.WParam = pBOk->GetId();
         msg.LParam = MAKELONG(pBOk->HWindow, BN_CLICKED);
	 Ok( msg );
       }
      /*
      if( HIWORD(Msg.LParam) == BN_DOUBLECLICKED )
	PostMessage( pBOk->HWindow, WM_COMMAND, pBOk->GetId(), MAKELONG(pBOk->HWindow, BN_CLICKED) );
        */
    } 
   else if( HIWORD(Msg.LParam) == BN_CLICKED && FindId(Msg.WParam) )
    {
      pVinDivisor->SetView( GetViewXlat(Msg.WParam) );
      bFlPrep = TRUE;
    }


   if( bFlPrep == TRUE ) return;
   else TCenterDialog::WMCommand( Msg );
 }

BOOL TViewsDlg::FindId( int id )
 {
   for( int i = 0; i < cViewBtn; i++ )
     if( vbDta[i].id == id ) return TRUE;

   return FALSE;
 }


void TMyRadioButton::WMLBDoubleClick( RTMessage msg )//Style WNDCLASS
 {
   //TBRadioButton::DefWndProc( msg );
   if( Parent ) PostMessage( Parent->HWindow, WM_COMMAND,
     (WPARAM)GetId(), (LPARAM)MAKELONG(HWindow, BN_DOUBLECLICKED) );
 }

void TMyRadioButton::Paint( HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo )
 {
 }


void TMyRadioButton::WMEraseBkGnd( RTMessage Msg )
 {
   Msg.Result = 0;
 }

              //GetInstance  TDialog
LPSTR TViewsDlg::GetClassName()
 {
   return "3DW:VIEWDLG_WINDOW";
 }


void TViewsDlg::GetWindowClass( WNDCLASS FAR & AWndClass )
 {
   TCenterDialog::GetWindowClass( AWndClass );

   AWndClass.style |= CS_SAVEBITS;
   //AWndClass.lpfnWndProc = (WNDPROC)MakeProcInstance( (FARPROC)DefDlgProcBWCC, GetApplication()->hInstance );
   AWndClass.lpfnWndProc = (WNDPROC)DefDlgProcBWCC;
 }



void TViewsDlg::DrawItem( DRAWITEMSTRUCT _FAR & DrawInfo )
 {
   int index = DrawInfo.CtlID - ID_DIVBTN_FIRST;
   if( index > -1 && index < cDivBtn )
    {
      if( (DrawInfo.itemAction & ODA_SELECT) == ODA_SELECT )
	(keyBtnDiv[index])->isSelected =
	  ((keyBtnDiv[index])->isSelected == TRUE) ? FALSE:TRUE;

      if( (DrawInfo.itemState & ODS_FOCUS) == ODS_FOCUS )	  
	DrawBmp( DrawInfo.hDC, keyBmp[index][1] );
      else if( (DrawInfo.itemState & ODS_SELECTED) == ODS_SELECTED ||
	(keyBtnDiv[index])->isSelected == TRUE )
	DrawBmp( DrawInfo.hDC, keyBmp[index][2] );
      else
	DrawBmp( DrawInfo.hDC, keyBmp[index][0] );
    }
   else if( DrawInfo.CtlID >= idDivisorButtonsFirst &&
	    DrawInfo.CtlID < idDivisorButtonsFirst + MAXRECT )
    {
     /////
      COLORREF colorFon, colorFonOld,
	       colorFrame, colorFrameOld;

      if( pVinDivisor->GetCheckInd() == DrawInfo.CtlID - idDivisorButtonsFirst )
       {
	 colorFon   = RGB(0, 0, 0xFF);
	 colorFrame = PALETTEINDEX(13);
       }
      else
       {
	 colorFon   = PALETTEINDEX(4);
	 colorFrame = RGB(0xFF, 0xFF, 0xFF);
       }

      HPEN hpNew, hpOld;
      hpNew = CreatePen( PS_SOLID, 0, colorFrame );
      hpOld = SelectPen( DrawInfo.hDC, hpNew );

      HBRUSH hbrNew, hbrOld;
      hbrNew = CreateSolidBrush( colorFon );
      hbrOld = SelectBrush( DrawInfo.hDC, hbrNew );

      HFONT hfOld = SelectFont( DrawInfo.hDC, hfHelv );
      COLORREF colorTxtFonOld, colorTxtOld;
      colorTxtFonOld = SetBkColor( DrawInfo.hDC, colorFon );
      colorTxtOld = SetTextColor( DrawInfo.hDC, colorFrame );

      RECT r;
      GetClientRect( GetDlgItem(HWindow, DrawInfo.CtlID), &r );
      Rectangle( DrawInfo.hDC, 0, 0, r.right, r.bottom );

      POINT pt[4]; _fsetmem( pt, sizeof(pt), 0 );
      pt[1].x = r.right; pt[2].x = r.right; pt[2].y = r.bottom;
      pt[3].y = r.bottom;
      Polyline( DrawInfo.hDC, pt, 4 );

      char cBuf[25];
      GetDlgItemText( HWindow, DrawInfo.CtlID, cBuf, 24 );
      DrawText( DrawInfo.hDC, cBuf,
	_fstrlen( cBuf ),
	&r,
	DT_CENTER | DT_VCENTER | DT_SINGLELINE );


      SetTextColor( DrawInfo.hDC, colorTxtOld );
      SetBkColor( DrawInfo.hDC, colorTxtFonOld );
      SelectFont( DrawInfo.hDC, hfOld );
      SelectBrush( DrawInfo.hDC, hbrOld );
      SelectPen( DrawInfo.hDC, hpOld );

      DeleteObject( hbrNew );
      DeleteObject( hpNew );
      //hdc, lpsz, cb, lprc, fuFormat)
    }

   //TCenterDialog::DrawItem( DrawInfo );
 }

/*
       //TControl  WM_MOUSEMOVE TBRadioButton
void TMyRadioButton::ODADrawEntire( DRAWITEMSTRUCT _FAR & DrawInfo )
 {
   //if( (DrawInfo.itemState & (ODS_FOCUS | ODS_SELECTED)) == (ODS_FOCUS | ODS_SELECTED) )
     //pDrawBmp = pButBmp[1];

   TBRadioButton::ODADrawEntire( DrawInfo );

   if( bFlInRect == TRUE && bFlMouse == TRUE )
     DrawBmp( DrawInfo.hDC, hbmFace[1] );
     
   else
   if( (DrawInfo.itemState & ODS_FOCUS) == ODS_FOCUS ||
       (DrawInfo.itemState & ODS_SELECTED) == ODS_SELECTED )
     {
       DrawBmp( DrawInfo.hDC, hbmFace[1] );
       bFlFocus = TRUE;
       //SetCheck( BF_CHECKED );
     }
   else
    if( GetCheck() == BF_CHECKED )
     {
       DrawBmp( DrawInfo.hDC, hbmFace[2] );
       bFlFocus = TRUE;
     }
   else
    {
      DrawBmp( DrawInfo.hDC, hbmFace[0] );
      bFlFocus = FALSE;
    }

 }*/
//TDialog TControl TWindowsObject
TViewDivisor::TViewDivisor( PTDialog pOwner, const RECT& RtWin, 
  PTWindowDivision pWD ):
  TWindowDivision( RtWin, *pWD )
 {
   _fsetmem( viewButtons, sizeof(viewButtons), 0 );
   this->pOwner = pOwner;
   bIndexChecked = 0;
   dsStyle = pWD->Style;

   CreateButtons();
 }

void TViewDivisor::DeleteButtons()
 {

   PTViewsDlg(pOwner)->LockErase( TRUE );

   for( int i = 0; i < MAXRECT; i++ )
     if( !viewButtons[i] ) break;
     else
       (viewButtons[i])->CloseWindow();
   

    PTViewsDlg(pOwner)->LockErase( FALSE );

   _fsetmem( viewButtons, sizeof(viewButtons), 0 );
 }

TViewDivisor::~TViewDivisor()
 {
   DeleteButtons();
 }

void TViewDivisor::CreateButtons()
 {
   int curId = idDivisorButtonsFirst;//////
   DeleteButtons();

   PTMyRect pRectCur;
   for( int i = 0; (pRectCur = (*((PTWindowDivision)this))[i]); i++, curId++ )
    {
      viewButtons[i] = new TViewButton( pOwner, pRectCur, this, i, curId,
	pRectCur->VpVPoint, pOwner->GetModule() );
    }
     //PTWindowsObject AParent, RECT *pRect, PTViewDivisor pDivisor,
     //int index, int id
     //TViewButton( PTWindowsObject AParent, RECT *pRect, PTViewDivisor pDivisor, int index ):
   if( bIndexChecked < i ) Select( bIndexChecked );
 }

void TViewDivisor::SetView( int iView )
 {
   SetDlgItemText( pOwner->HWindow, (viewButtons[ bIndexChecked ])->Attr.Id,
     cVNames[ iView ] );
   ((*((PTWindowDivision)this))[bIndexChecked])->VpVPoint = iView;
 }

void TViewDivisor::ResetDivision( TDivStyle Style )
 {
   DeleteButtons();

   dsStyle = Style;

   TRECT rect;
   rect.left = keyRect.left; rect.right = keyRect.right;
   rect.top = keyRect.top; rect.bottom = keyRect.bottom;
   *((PTWindowDivision)this) = TWindowDivision( rect, Style );

   CreateButtons();

   PTViewsDlg(pOwner)->LockErase( TRUE );

   for( int i = 0; i < MAXRECT; i++ )//TWindowsObject TWindow
     if( viewButtons[i] ) pOwner->GetApplication()->MakeWindow( viewButtons[i] );
     else break;

   PTViewsDlg(pOwner)->LockErase( FALSE );

   //Select( bIndexChecked );
 }

void TViewDivisor::GetViews( PTWindowDivision pVDiv ) const
 {

   *pVDiv = *((PTWindowDivision)this);
/*   PTMyRect pRectCur;
   for( int i = 0; (pRectCur = (*((PTWindowDivision)this))[i]); i++ )
     ((*pVDiv)[i])->VpVPoint = pRectCur->VpVPoint;*/
 }

LPSTR TViewDivisor::cVNames[cViewBtn] =
 {
   {"Top"},
   {"Left"},
   {"Front"},
   {"Bottom"},
   {"Right"},
   {"Back"},
   {"User"},
   {"Camera"},
   {"None"},
 };

void TViewDivisor::Select( index )
 {
   bIndexChecked = index;

   PTMyRect pRectCur;
   for( int i = 0; (pRectCur = (*((PTWindowDivision)this))[i]); i++ )
     (viewButtons[i])->UnCheck();

   (viewButtons[index])->Check();
 }

void TViewButton::DefWndProc( RTMessage msg )//WM_COMMAND
 {
   if( msg.Message == WM_SETFOCUS )
    {
      pDivisorOwn->Select( index );
      bFlChecked = TRUE;
    }

   TButton::DefWndProc( msg );
 }

 //VP_Top = 0, VP_Left, VP_Front, VP_Bottom, VP_Right, VP_Back,
 //VP_User, VP_Camera, VP_None

void TViewButton::UnCheck()
 {
   if( bFlChecked == TRUE ) InvalidateRect( HWindow, NULL, FALSE );
   bFlChecked = FALSE;
 }

void TViewButton::Check()
 {
   //if( bFlChecked == FALSE ) InvalidateRect( HWindow, NULL, FALSE );
   bFlChecked = TRUE;
 }

void TViewButton::WMEraseBkGnd( RTMessage Msg )
 {
   Msg.Result = 0;
 }
