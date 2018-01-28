#include "bkgnddlg.hpp"
#include "bmp.hpp"
#include "frame3d.hpp"

#include <mem.h>
#include <control.h>
#include <bgrpbox.h>
#include <bradio.h>
#include <alloc.h>
#include <windowsx.h>
#include <bwcc.h>

#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;

/*class far TGroupMousePass;
typedef TGroupMousePass* PTGroupMousePass;
typedef TGroupMousePass& RTGroupMousePass;*/
_CLASSDEF(TGroupMousePass)
class TGroupMousePass: public TBGroupBox
 {
public:
  TGroupMousePass( PTWindowsObject AParent, int ResourceId,
    PTModule AModule = NULL ):
    TBGroupBox( AParent, ResourceId, AModule )
     {
     }

  virtual ~TGroupMousePass();

  RTGroupMousePass operator+( RTWindowsObject rObj )
   {
     lPass.add( rObj );
     return *this;
   }

  virtual void DefWndProc( RTMessage );

private:
  List lPass;

 };

void TGroupMousePass::DefWndProc( RTMessage msg )
 {
   if( /*msg.Message == WM_MOUSEMOVE || msg.Message == WM_LBUTTONDOWN ||
       msg.Message == WM_LBUTTONUP*/
       msg.Message == WM_NCHITTEST
     )
    {
      int x = LOWORD( msg.LParam ), y = HIWORD( msg.LParam );

      ListIterator liIt = ListIterator( lPass );
      PTWindowsObject pWO = (PTWindowsObject)&liIt.current();

      for( ; int(liIt); pWO = (PTWindowsObject)&++liIt )
       {
	 if( IsWindowVisible(pWO->HWindow) == FALSE ) continue;
	 RECT r; GetWindowRect( pWO->HWindow, &r );
	 if( x >= r.left && x <= r.right &&
	     y >= r.top && y <= r.bottom
	   )
	  {
	    SendMessage( pWO->HWindow, msg.Message, msg.WParam, msg.LParam );
            break;
          }
       }
    }

   TBGroupBox::DefWndProc( msg );
 }

TGroupMousePass::~TGroupMousePass()
 {
   lPass.flush( TShouldDelete::NoDelete );
 }


/*class far THiliteView;
typedef THiliteView* PTHiliteView;
typedef THiliteView& RTHiliteView;*/
_CLASSDEF(THiliteView)
class THiliteView: public TBStaticBmp
 {
public:


  THiliteView( PTWindowsObject AParent, int ResourceId,
	       int& riX_, int& riY_,
	       PTModule AModule = NULL ):
    //TBStaticBmp( AParent, ResourceId, AModule ),
    TBStaticBmp( AParent, ResourceId, AModule ),
    riX( riX_ ),
    riY( riY_ )
     {
       iXKey = iYKey = 0;
       iCapture = 0;
     }

  virtual void WMDrawItem( RTMessage Msg ) = [WM_FIRST + WM_DRAWITEM];
  //virtual void Paint( HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo );
  void WMEraseBkGnd( RTMessage ) = [WM_FIRST + WM_ERASEBKGND];
  void WMMouseMove( RTMessage ) = [WM_FIRST + WM_MOUSEMOVE];
  virtual void WMLButtonDown( RTMessage ) = [WM_FIRST + WM_LBUTTONDOWN];
  void WMLButtonUp( RTMessage ) = [WM_FIRST + WM_LBUTTONUP];

  virtual void DefWndProc( RTMessage );

private:
  int& riX;
  int& riY;
  int iXKey, iYKey;

  int iCapture;
  int x, y;
 };

void THiliteView::DefWndProc( RTMessage msg )
 {
   if( msg.Message == WM_MOUSEMOVE ) MessageBeep( -1 );
   TBStaticBmp::DefWndProc( msg );
 }

void THiliteView::WMLButtonUp( RTMessage msg )
 {
   if( iCapture )
    {
      iCapture = 0;
      ReleaseCapture();
    }

   TBStaticBmp::DefWndProc( msg );
 }

void THiliteView::WMLButtonDown( RTMessage msg )//WM_LBUTTONDOWN
 {
   const int iDeltaMax = 5;

   RECT r; GetClientRect( HWindow, &r );
   int dy = abs( (double)r.bottom * ((double)riY / 100.0) - HIWORD(msg.LParam) ),
       dx = abs( (double)r.right * ((double)riX / 100.0) - LOWORD(msg.LParam) );

   if( dx < iDeltaMax || dy < iDeltaMax )
    {
      if( dx < iDeltaMax && dy < iDeltaMax ) iCapture = 3;
      else if( dx < dy ) iCapture = 1;      
      else iCapture = 2;

      SetCapture( HWindow );
    }

   TBStaticBmp::WMLButtonDown( msg );
 }

void THiliteView::WMMouseMove( RTMessage msg )//WM_MOUSEMOVE
 {
   if( iCapture == 1 ) riX -= (x - LOWORD( msg.LParam ));
   else if( iCapture == 2 ) riY -= (y - HIWORD( msg.LParam ));
   else if( iCapture == 3 )
    {
      riX -= (x - LOWORD( msg.LParam ));
      riY -= (y - HIWORD( msg.LParam ));
    }

   if( iCapture )
    {
      if( riX < 0 ) riX = 0;
      else if( riX > 100 ) riX = 100;
      if( riY < 0 ) riY = 0;
      else if( riY > 100 ) riY = 100;

      InvalidateRect( HWindow, NULL, FALSE );
    }

   x = LOWORD( msg.LParam ); y = HIWORD( msg.LParam );

   TBStaticBmp::DefWndProc( msg );
 }

//void THiliteView::Paint( HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo )
void THiliteView::WMDrawItem( RTMessage Msg )
 {
   LPDRAWITEMSTRUCT pDr = (LPDRAWITEMSTRUCT)Msg.LParam;

   HPEN hpOld = SelectPen( pDr->hDC, (HPEN)GetStockObject(BLACK_PEN) );

   RECT r = pDr->rcItem; //GetClientRect( HWindow, &r );
   int x1 = 0, y1 = (double)r.bottom * ((double)iYKey / 100.0), x2 = r.right,
       y21 = 0, x21 = (double)r.right * (double)iXKey / 100.0, y22 = r.bottom;
   MoveTo( pDr->hDC, x1, y1 );
   LineTo( pDr->hDC, x2, y1 );
   MoveTo( pDr->hDC, x21, y21 );
   LineTo( pDr->hDC, x21, y22 );

   x1 = 0; y1 = (double)r.bottom * ((double)riY / 100.0); x2 = r.right;
   y21 = 0; x21 = (double)r.right * (double)riX / 100.0; y22 = r.bottom;

   SelectPen( pDr->hDC, (HPEN)GetStockObject(WHITE_PEN) );
   MoveTo( pDr->hDC, x1, y1 );
   LineTo( pDr->hDC, x2, y1 );
   MoveTo( pDr->hDC, x21, y21 );
   LineTo( pDr->hDC, x21, y22 );  

   iXKey = riX; iYKey = riY;

   SelectPen( pDr->hDC, hpOld );
   Msg.Result = TRUE;
 }



void THiliteView::WMEraseBkGnd( RTMessage msg )
 {
   RECT r; GetClientRect( HWindow, &r );
   FillRect( (HDC)msg.WParam, &r, (HBRUSH)GetStockObject(BLACK_BRUSH) );
   msg.Result = 0;
 }

int TBkGndDialog::iTblAVButtons[ iNumbAVButtons ] =
 {
   IDOK,
   IDCANCEL,
   ID_BTN_PREVIEW,
   ID_RECT_BTN_UP, 
   ID_RECT_BTN_DOWN, 
   ID_RECT_BTN_LEFT, 
   ID_RECT_BTN_RIGHT,
   ID_RECT_BTN_CENTER,
   ID_CIRCULAR_BTN_UP,
   ID_CIRCULAR_BTN_DOWN, 
   ID_CIRCULAR_BTN_LEFT, 
   ID_CIRCULAR_BTN_RIGHT, 
   ID_CIRCULAR_BTN_CENTER
 };

int TBkGndDialog::iTblFontUsers[ iNumbFontUsers ] =
 {
   ID_RECT_BTN_UP,
   ID_RECT_BTN_DOWN,
   ID_RECT_BTN_LEFT,
   ID_RECT_BTN_RIGHT,
   ID_RECT_BTN_CENTER   
 };

TBkGndDialog::TIndexGradientTbl TBkGndDialog::igtTbl[ iNumbGradient ] =
 {
  {BK_Flat,        0},
  {BK_Linear,      1},
  {BK_Rectangular, 2},
  {BK_Circular,    3}
 };

TBkGndDialog::TIndexGradientTbl TBkGndDialog::igtTblRadio[ iNumbGradient ] =
 {
  {BK_Flat,        ID_RADIO_FLAT},
  {BK_Linear,      ID_RADIO_LINEAR},
  {BK_Rectangular, ID_RADIO_RECTANGULAR},
  {BK_Circular,    ID_RADIO_CIRCULAR}
 };

TBkGndDialog::TScrollRange TBkGndDialog::srTblRange[ iNumbScrolls ] =
 {
  {ID_FLAT_SCROLL_R, 0, 255 },
  {ID_FLAT_SCROLL_G, 0, 255 },
  {ID_FLAT_SCROLL_B, 0, 255 },

  {ID_LINEAR_SCROLL_R1, 0, 255 },
  {ID_LINEAR_SCROLL_G1, 0, 255 },
  {ID_LINEAR_SCROLL_B1, 0, 255 },

  {ID_LINEAR_SCROLL_R2, 0, 255 },
  {ID_LINEAR_SCROLL_G2, 0, 255 },
  {ID_LINEAR_SCROLL_B2, 0, 255 },

  {ID_LINEAR_SCROLL_ANGLE, 0, 180 },

  {ID_RECT_SCROLL_R1, 0, 255 },
  {ID_RECT_SCROLL_G1, 0, 255 },
  {ID_RECT_SCROLL_B1, 0, 255 },

  {ID_RECT_SCROLL_R2, 0, 255 },
  {ID_RECT_SCROLL_G2, 0, 255 },
  {ID_RECT_SCROLL_B2, 0, 255 },

  {ID_CIRCULAR_SCROLL_R1, 0, 255 },
  {ID_CIRCULAR_SCROLL_G1, 0, 255 },
  {ID_CIRCULAR_SCROLL_B1, 0, 255 },

  {ID_CIRCULAR_SCROLL_R2, 0, 255 },
  {ID_CIRCULAR_SCROLL_G2, 0, 255 },
  {ID_CIRCULAR_SCROLL_B2, 0, 255 },

 };

TBkGndDialog::TOvlItem TBkGndDialog::oiTblOvlItems[ iNumbOvlItems ] =
 {
  { ID_FLAT_INPUT_R, BK_Flat },
  { ID_FLAT_INPUT_G, BK_Flat },
  { ID_FLAT_INPUT_B, BK_Flat },
  { ID_FLAT_SCROLL_R, BK_Flat },
  { ID_FLAT_SCROLL_G, BK_Flat },
  { ID_FLAT_SCROLL_B, BK_Flat },
  { ID_FLAT_TXT_R, BK_Flat },
  { ID_FLAT_TXT_G, BK_Flat },
  { ID_FLAT_TXT_B, BK_Flat },
  { ID_FLAT_TXT_0, BK_Flat },
  { ID_FLAT_TXT_255, BK_Flat },

  { ID_LINEAR_INPUT_R1, BK_Linear },
  { ID_LINEAR_INPUT_G1, BK_Linear },
  { ID_LINEAR_INPUT_B1, BK_Linear },
  { ID_LINEAR_INPUT_R2, BK_Linear },
  { ID_LINEAR_INPUT_G2, BK_Linear },
  { ID_LINEAR_INPUT_B2, BK_Linear },

  { ID_LINEAR_SCROLL_R1, BK_Linear },
  { ID_LINEAR_SCROLL_G1, BK_Linear },
  { ID_LINEAR_SCROLL_B1, BK_Linear },

  { ID_LINEAR_SCROLL_R2, BK_Linear },
  { ID_LINEAR_SCROLL_G2, BK_Linear },
  { ID_LINEAR_SCROLL_B2, BK_Linear },

  { ID_LINEAR_INPUT_ANGLE, BK_Linear },
  { ID_LINEAR_SCROLL_ANGLE, BK_Linear },

  { ID_LINEAR_TXT_START, BK_Linear },
  { ID_LINEAR_TXT_END, BK_Linear },

  { ID_LINEAR_TXT_R1, BK_Linear },
  { ID_LINEAR_TXT_G1, BK_Linear },
  { ID_LINEAR_TXT_B1, BK_Linear },

  { ID_LINEAR_TXT_R2, BK_Linear },
  { ID_LINEAR_TXT_G2, BK_Linear },
  { ID_LINEAR_TXT_B2, BK_Linear },

  { ID_LINEAR_TXT_01, BK_Linear },
  { ID_LINEAR_TXT_02, BK_Linear },
  { ID_LINEAR_TXT_255, BK_Linear },
  { ID_LINEAR_TXT_180, BK_Linear },
  { ID_LINEAR_TXT_ANGLE, BK_Linear },

  { ID_RECT_INPUT_R1, BK_Rectangular },
  { ID_RECT_INPUT_G1, BK_Rectangular },
  { ID_RECT_INPUT_B1, BK_Rectangular },

  { ID_RECT_SCROLL_R1, BK_Rectangular },
  { ID_RECT_SCROLL_G1, BK_Rectangular },
  { ID_RECT_SCROLL_B1, BK_Rectangular },

  { ID_RECT_INPUT_R2, BK_Rectangular },
  { ID_RECT_INPUT_G2, BK_Rectangular },
  { ID_RECT_INPUT_B2, BK_Rectangular },

  { ID_RECT_SCROLL_R2, BK_Rectangular },
  { ID_RECT_SCROLL_G2, BK_Rectangular },
  { ID_RECT_SCROLL_B2, BK_Rectangular },

  { ID_RECT_BTN_UP, BK_Rectangular },
  { ID_RECT_BTN_DOWN, BK_Rectangular },
  { ID_RECT_BTN_LEFT, BK_Rectangular },
  { ID_RECT_BTN_RIGHT, BK_Rectangular },
  { ID_RECT_BTN_CENTER, BK_Rectangular },

  { ID_RECT_MIX_HILITE, BK_Rectangular },

  { ID_RECT_TXT_START, BK_Rectangular },
  { ID_RECT_TXT_END, BK_Rectangular },

  { ID_RECT_TXT_R1, BK_Rectangular },
  { ID_RECT_TXT_G1, BK_Rectangular },
  { ID_RECT_TXT_B1, BK_Rectangular },
  { ID_RECT_TXT_R2, BK_Rectangular },
  { ID_RECT_TXT_G2, BK_Rectangular },
  { ID_RECT_TXT_B2, BK_Rectangular },

  { ID_RECT_TXT_0, BK_Rectangular },
  { ID_RECT_TXT_255, BK_Rectangular },
  { ID_RECT_TXT_HILITE, BK_Rectangular },

  { ID_CIRCULAR_INPUT_R1, BK_Circular },
  { ID_CIRCULAR_INPUT_G1, BK_Circular },
  { ID_CIRCULAR_INPUT_B1, BK_Circular },

  { ID_CIRCULAR_SCROLL_R1, BK_Circular },
  { ID_CIRCULAR_SCROLL_G1, BK_Circular },
  { ID_CIRCULAR_SCROLL_B1, BK_Circular },

  { ID_CIRCULAR_INPUT_R2, BK_Circular },
  { ID_CIRCULAR_INPUT_G2, BK_Circular },
  { ID_CIRCULAR_INPUT_B2, BK_Circular },

  { ID_CIRCULAR_SCROLL_R2, BK_Circular },
  { ID_CIRCULAR_SCROLL_G2, BK_Circular },
  { ID_CIRCULAR_SCROLL_B2, BK_Circular },

  { ID_CIRCULAR_BTN_UP, BK_Circular },
  { ID_CIRCULAR_BTN_DOWN, BK_Circular },
  { ID_CIRCULAR_BTN_LEFT, BK_Circular },
  { ID_CIRCULAR_BTN_RIGHT, BK_Circular },
  { ID_CIRCULAR_BTN_CENTER, BK_Circular },

  { ID_CIRCULAR_MIX_HILITE, BK_Circular },

  { ID_CIRCULAR_TXT_START, BK_Circular },
  { ID_CIRCULAR_TXT_END, BK_Circular },

  { ID_CIRCULAR_TXT_R1, BK_Circular },
  { ID_CIRCULAR_TXT_G1, BK_Circular },
  { ID_CIRCULAR_TXT_B1, BK_Circular },
  { ID_CIRCULAR_TXT_R2, BK_Circular },
  { ID_CIRCULAR_TXT_G2, BK_Circular },
  { ID_CIRCULAR_TXT_B2, BK_Circular },

  { ID_CIRCULAR_TXT_0, BK_Circular },
  { ID_CIRCULAR_TXT_255, BK_Circular },
  { ID_CIRCULAR_TXT_HILITE, BK_Circular }
 };


int TBkGndDialog::ClassOfRadio( int id ) const
 {
   for( int i = 0; i < iNumbGradient; i++ )
     if( id == TBkGndDialog::igtTblRadio[ i ].iIndex )
       return TBkGndDialog::igtTblRadio[ i ].classGradient;

   return -1;
 }

int TBkGndDialog::IdOfClass( int cl ) const
 {
   for( int i = 0; i < iNumbGradient; i++ )
     if( cl == TBkGndDialog::igtTblRadio[ i ].classGradient )
       return TBkGndDialog::igtTblRadio[ i ].iIndex;

   return ID_RADIO_NONE;
 }

void TGradientConnected::Paint( HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo )
 {
   if( !!grcBmp )
     grcBmp()->DrawBitmapDIBWithPalette( PaintDC, NULL, 0, 0 );
   else
    {
      HBRUSH hbr = CreateHatchBrush( HS_BDIAGONAL, RGB(0, 0, 0) );
      RECT r;
      r.left = r.top = 0;
      r.right = Attr.W; r.bottom = Attr.H;
      FillRect( PaintDC, &r, hbr );
      DeleteObject( hbr );
    }
 }

int TBkGndDialog::IndexOf( RTBackGraund rBkGnd ) const
 {
   classType cl = rBkGnd.isA();
   for( int i = 0; i < iNumbGradient; i++ )
     if( cl == TBkGndDialog::igtTbl[ i ].classGradient )
       return TBkGndDialog::igtTbl[ i ].iIndex;

   return -1;
 }

int TBkGndDialog::IndexOf( classType cl ) const
 {
   for( int i = 0; i < iNumbGradient; i++ )
     if( cl == TBkGndDialog::igtTbl[ i ].classGradient )
       return TBkGndDialog::igtTbl[ i ].iIndex;

   return -1;
 }


classType TBkGndDialog::ClassOf( int iIndex ) const
 {
   for( int i = 0; i < iNumbGradient; i++ )
     if( iIndex == TBkGndDialog::igtTbl[ i ].iIndex )
       return TBkGndDialog::igtTbl[ i ].classGradient;

   return -1;
 }

TBkGndDialog::TBkGndDialog( PTWindowsObject pOwn, LPSTR lpSrcName,
  TAutoMemoryRc<PTBackGraund>& rmrcBkGnd_, PTModule AModule ):
  TCenterDialog( pOwn, lpSrcName, AModule ),
  rmrcBkGnd( rmrcBkGnd_ ),  
  hwndMix( NULL ),
  iIndexSelected( -1 )
 {
   bFlOk = FALSE;
   iWid = iHigh = 0;
   _fsetmem( bkgndItems, sizeof(PTBackGraund) * iNumbGradient, 0 );
   _fsetmem( bkgndItemsKey, sizeof(PTBackGraund) * iNumbGradient, 0 );   

   //mrcPalette = new TColorTable[ 234 ];

   for( int k = 0; k < iNumbAVButtons; k++ )
     new TBButtonAutoValidate( this, TBkGndDialog::iTblAVButtons[ k ], AModule );


     //ID_GRP_SETUP
   PTGroupMousePass pMainGrp = new TGroupMousePass( this, ID_GRP_SETUP, AModule );

   PTBGroupBox pClassGrp = new TBGroupBox( this, ID_GRP_CLASS, AModule );

   new TBRadioButtonAutoValidate( this, ID_RADIO_FLAT, pClassGrp, AModule );
   new TBRadioButtonAutoValidate( this, ID_RADIO_LINEAR, pClassGrp, AModule );
   new TBRadioButtonAutoValidate( this, ID_RADIO_RECTANGULAR, pClassGrp, AModule );
   new TBRadioButtonAutoValidate( this, ID_RADIO_CIRCULAR, pClassGrp, AModule );
   new TBRadioButtonAutoValidate( this, ID_RADIO_NONE, pClassGrp, AModule );

   new TBCheckBoxAutoValidate( this, ID_CHECK_TRACK, pClassGrp, AModule );

   pMix = new TGradientConnected( this, ID_MIX_GRADIENT,
     mrcGradientBmp, AModule );

   *this,
     *new T3DThickFrame( this, ID_MIX_GRADIENT, 3, AModule );

   iIndexSelected = !rmrcBkGnd ? -1:IndexOf( *(rmrcBkGnd()) );

   for( int iInd = 0; iInd < iNumbGradient; iInd++ )
    {
      classType clT = ClassOf( iInd );

      if( iInd == iIndexSelected )
       {
	 bkgndItems[ iInd ] = rmrcBkGnd();
	 rmrcBkGnd.ClearRc();
	 bkgndItemsKey[ iInd ] = CreateBkForClass( clT );
       }
      else
       {
	 bkgndItems[ iInd ] = CreateBkForClass( clT );
	 bkgndItemsKey[ iInd ] = CreateBkForClass( clT );
       }
    }   


   RTBackGraundFlat rBk = *PTBackGraundFlat(bkgndItems[ IndexOf(BK_Flat) ]);

   TAVEditConnected<BYTE>* pEd2;
   TAVScrollConnected<BYTE>* pSc2;

   pEd2 = new TAVEditConnected<BYTE>( this, ID_FLAT_INPUT_R, 10,
	  (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rBk.RGBSta().rgbRed, AModule );
   pSc2 = new TAVScrollConnectedMixer( this, ID_FLAT_SCROLL_R,
	  (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rBk.RGBSta().rgbRed, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	  (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

   pEd2 = new TAVEditConnected<BYTE>( this, ID_FLAT_INPUT_G, 10,
	  (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rBk.RGBSta().rgbGreen, AModule );
   pSc2 = new TAVScrollConnectedMixer( this, ID_FLAT_SCROLL_G,
	  (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rBk.RGBSta().rgbGreen, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	  (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

   pEd2 = new TAVEditConnected<BYTE>( this, ID_FLAT_INPUT_B, 10,
	  (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rBk.RGBSta().rgbBlue, AModule );
   pSc2 = new TAVScrollConnectedMixer( this, ID_FLAT_SCROLL_B,
	  (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rBk.RGBSta().rgbBlue, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	  (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );


   *this,
     *new TByteValidator( ID_FLAT_INPUT_R, "R", 0, 255,
       rBk.RGBSta().rgbRed ),
     *new TByteValidator( ID_FLAT_INPUT_G, "G", 0, 255,
       rBk.RGBSta().rgbGreen ),
     *new TByteValidator( ID_FLAT_INPUT_B, "B", 0, 255,
       rBk.RGBSta().rgbBlue );

   RTBackGraundLinear rBk2 = *PTBackGraundLinear(bkgndItems[ IndexOf(BK_Linear) ]);

   pEd2 = new TAVEditConnected<BYTE>( this, ID_LINEAR_INPUT_R1, 10,
	  (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rBk2.RGBSta().rgbRed, AModule );
   pSc2 = new TAVScrollConnectedMixer( this, ID_LINEAR_SCROLL_R1,
	  (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rBk2.RGBSta().rgbRed, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	  (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

   pEd2 = new TAVEditConnected<BYTE>( this, ID_LINEAR_INPUT_G1, 10,
	  (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rBk2.RGBSta().rgbGreen, AModule );
   pSc2 = new TAVScrollConnectedMixer( this, ID_LINEAR_SCROLL_G1,
	  (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rBk2.RGBSta().rgbGreen, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	  (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

   pEd2 = new TAVEditConnected<BYTE>( this, ID_LINEAR_INPUT_B1, 10,
	  (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rBk2.RGBSta().rgbBlue, AModule );
   pSc2 = new TAVScrollConnectedMixer( this, ID_LINEAR_SCROLL_B1,
	  (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rBk2.RGBSta().rgbBlue, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	  (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );


   pEd2 = new TAVEditConnected<BYTE>( this, ID_LINEAR_INPUT_R2, 10,
	  (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rBk2.RGBEnd().rgbRed, AModule );
   pSc2 = new TAVScrollConnectedMixer( this, ID_LINEAR_SCROLL_R2,
	  (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rBk2.RGBEnd().rgbRed, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	  (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

   pEd2 = new TAVEditConnected<BYTE>( this, ID_LINEAR_INPUT_G2, 10,
	  (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rBk2.RGBEnd().rgbGreen, AModule );
   pSc2 = new TAVScrollConnectedMixer( this, ID_LINEAR_SCROLL_G2,
	  (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rBk2.RGBEnd().rgbGreen, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	  (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

   pEd2 = new TAVEditConnected<BYTE>( this, ID_LINEAR_INPUT_B2, 10,
	  (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rBk2.RGBEnd().rgbBlue, AModule );
   pSc2 = new TAVScrollConnectedMixer( this, ID_LINEAR_SCROLL_B2,
	  (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rBk2.RGBEnd().rgbBlue, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	  (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );


   pEd2 = new TAVEditConnected<BYTE>( this, ID_LINEAR_INPUT_ANGLE, 10,
	  (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rBk2.Angle(), AModule );
   pSc2 = new TAVScrollConnectedMixer( this, ID_LINEAR_SCROLL_ANGLE,
	  (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rBk2.Angle(), 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	  (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );


   *this,
     *new TByteValidator( ID_LINEAR_INPUT_R1, "R start", 0, 255,
       rBk2.RGBSta().rgbRed ),
     *new TByteValidator( ID_LINEAR_INPUT_G1, "G start", 0, 255,
       rBk2.RGBSta().rgbGreen ),
     *new TByteValidator( ID_LINEAR_INPUT_B1, "B start", 0, 255,
       rBk2.RGBSta().rgbBlue ),

     *new TByteValidator( ID_LINEAR_INPUT_R2, "R end", 0, 255,
       rBk2.RGBEnd().rgbRed ),
     *new TByteValidator( ID_LINEAR_INPUT_G2, "G end", 0, 255,
       rBk2.RGBEnd().rgbGreen ),
     *new TByteValidator( ID_LINEAR_INPUT_B2, "B end", 0, 255,
       rBk2.RGBEnd().rgbBlue ),

     *new TByteValidator( ID_LINEAR_INPUT_ANGLE, "Angle of gradient", 0, 180,
       rBk2.Angle() );

   RTBackGraundRectangular rBk3 = *PTBackGraundRectangular(bkgndItems[ IndexOf(BK_Rectangular) ]);

   pEd2 = new TAVEditConnected<BYTE>( this, ID_RECT_INPUT_R1, 10,
	  (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rBk3.RGBSta().rgbRed, AModule );
   pSc2 = new TAVScrollConnectedMixer( this, ID_RECT_SCROLL_R1,
	  (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rBk3.RGBSta().rgbRed, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	  (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

   pEd2 = new TAVEditConnected<BYTE>( this, ID_RECT_INPUT_G1, 10,
	  (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rBk3.RGBSta().rgbGreen, AModule );
   pSc2 = new TAVScrollConnectedMixer( this, ID_RECT_SCROLL_G1,
	  (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rBk3.RGBSta().rgbGreen, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	  (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

   pEd2 = new TAVEditConnected<BYTE>( this, ID_RECT_INPUT_B1, 10,
	  (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rBk3.RGBSta().rgbBlue, AModule );
   pSc2 = new TAVScrollConnectedMixer( this, ID_RECT_SCROLL_B1,
	  (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rBk3.RGBSta().rgbBlue, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	  (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );


   pEd2 = new TAVEditConnected<BYTE>( this, ID_RECT_INPUT_R2, 10,
	  (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rBk3.RGBEnd().rgbRed, AModule );
   pSc2 = new TAVScrollConnectedMixer( this, ID_RECT_SCROLL_R2,
	  (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rBk3.RGBEnd().rgbRed, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	  (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

   pEd2 = new TAVEditConnected<BYTE>( this, ID_RECT_INPUT_G2, 10,
	  (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rBk3.RGBEnd().rgbGreen, AModule );
   pSc2 = new TAVScrollConnectedMixer( this, ID_RECT_SCROLL_G2,
	  (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rBk3.RGBEnd().rgbGreen, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	  (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

   pEd2 = new TAVEditConnected<BYTE>( this, ID_RECT_INPUT_B2, 10,
	  (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rBk3.RGBEnd().rgbBlue, AModule );
   pSc2 = new TAVScrollConnectedMixer( this, ID_RECT_SCROLL_B2,
	  (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rBk3.RGBEnd().rgbBlue, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	  (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );


   PTHiliteView pHView = new THiliteView( this, ID_RECT_MIX_HILITE,
     rBk3.XCenter(), rBk3.YCenter(),
     AModule );

   *pMainGrp + *pHView;

   *this,
     *new TByteValidator( ID_RECT_INPUT_R1, "R start", 0, 255,
       rBk3.RGBSta().rgbRed ),
     *new TByteValidator( ID_RECT_INPUT_G1, "G start", 0, 255,
       rBk3.RGBSta().rgbGreen ),
     *new TByteValidator( ID_RECT_INPUT_B1, "B start", 0, 255,
       rBk3.RGBSta().rgbBlue ),

     *new TByteValidator( ID_RECT_INPUT_R2, "R end", 0, 255,
       rBk3.RGBEnd().rgbRed ),
     *new TByteValidator( ID_RECT_INPUT_G2, "G end", 0, 255,
       rBk3.RGBEnd().rgbGreen ),
     *new TByteValidator( ID_RECT_INPUT_B2, "B end", 0, 255,
       rBk3.RGBEnd().rgbBlue );



   RTBackGraundCircular rBk4 = *PTBackGraundCircular(bkgndItems[ IndexOf(BK_Circular) ]);

   pEd2 = new TAVEditConnected<BYTE>( this, ID_CIRCULAR_INPUT_R1, 10,
	  (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rBk4.RGBSta().rgbRed, AModule );
   pSc2 = new TAVScrollConnectedMixer( this, ID_CIRCULAR_SCROLL_R1,
	  (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rBk4.RGBSta().rgbRed, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	  (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

   pEd2 = new TAVEditConnected<BYTE>( this, ID_CIRCULAR_INPUT_G1, 10,
	  (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rBk4.RGBSta().rgbGreen, AModule );
   pSc2 = new TAVScrollConnectedMixer( this, ID_CIRCULAR_SCROLL_G1,
	  (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rBk4.RGBSta().rgbGreen, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	  (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

   pEd2 = new TAVEditConnected<BYTE>( this, ID_CIRCULAR_INPUT_B1, 10,
	  (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rBk4.RGBSta().rgbBlue, AModule );
   pSc2 = new TAVScrollConnectedMixer( this, ID_CIRCULAR_SCROLL_B1,
	  (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rBk4.RGBSta().rgbBlue, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	  (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );


   pEd2 = new TAVEditConnected<BYTE>( this, ID_CIRCULAR_INPUT_R2, 10,
	  (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rBk4.RGBEnd().rgbRed, AModule );
   pSc2 = new TAVScrollConnectedMixer( this, ID_CIRCULAR_SCROLL_R2,
	  (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rBk4.RGBEnd().rgbRed, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	  (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

   pEd2 = new TAVEditConnected<BYTE>( this, ID_CIRCULAR_INPUT_G2, 10,
	  (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rBk4.RGBEnd().rgbGreen, AModule );
   pSc2 = new TAVScrollConnectedMixer( this, ID_CIRCULAR_SCROLL_G2,
	  (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rBk4.RGBEnd().rgbGreen, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	  (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

   pEd2 = new TAVEditConnected<BYTE>( this, ID_CIRCULAR_INPUT_B2, 10,
	  (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rBk4.RGBEnd().rgbBlue, AModule );
   pSc2 = new TAVScrollConnectedMixer( this, ID_CIRCULAR_SCROLL_B2,
	  (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rBk4.RGBEnd().rgbBlue, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	  (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );


   pHView = new THiliteView( this, ID_CIRCULAR_MIX_HILITE,
     rBk4.XCenter(), rBk4.YCenter(),
     AModule );

   *pMainGrp + *pHView;

   *this,
     *new TByteValidator( ID_CIRCULAR_INPUT_R1, "R start", 0, 255,
       rBk4.RGBSta().rgbRed ),
     *new TByteValidator( ID_CIRCULAR_INPUT_G1, "G start", 0, 255,
       rBk4.RGBSta().rgbGreen ),
     *new TByteValidator( ID_CIRCULAR_INPUT_B1, "B start", 0, 255,
       rBk4.RGBSta().rgbBlue ),

     *new TByteValidator( ID_CIRCULAR_INPUT_R2, "R end", 0, 255,
       rBk4.RGBEnd().rgbRed ),
     *new TByteValidator( ID_CIRCULAR_INPUT_G2, "G end", 0, 255,
       rBk4.RGBEnd().rgbGreen ),
     *new TByteValidator( ID_CIRCULAR_INPUT_B2, "B end", 0, 255,
       rBk4.RGBEnd().rgbBlue );


 }

TBkGndDialog::~TBkGndDialog()
 {
   //listOfOvlCtl.flush( TShouldDelete::NoDelete );

   if( bFlOk == TRUE && iIndexSelected != -1 )
    {
      rmrcBkGnd = bkgndItems[ iIndexSelected ];
      bkgndItems[ iIndexSelected ] = NULL;
    }
	       
   for( int i = 0; i < iNumbGradient; i++ )
     if( bkgndItems[ i ] ) { delete bkgndItems[ i ]; bkgndItems[ i ] = NULL; }

   for( i = 0; i < iNumbGradient; i++ )
     if( bkgndItemsKey[ i ] ) { delete bkgndItemsKey[ i ]; bkgndItemsKey[ i ] = NULL; }
 }

HPALETTE TBkGndDialog::Palette() const
 {
   if( !!mrcGradientBmp ) return mrcGradientBmp()->GetPal();
   else return NULL;
 }

void TBkGndDialog::Ok( RTMessage Msg )
 {
   bFlOk = TRUE;
   TCenterDialog::Ok( Msg );
 }

void TBkGndDialog::SetupWindow()
 {
   TCenterDialog::SetupWindow();

   int iIdN = ID_BKGND_BMP_FIRST_N + 1,
       iIdS = ID_BKGND_BMP_FIRST_S + 1,
       iIdH = ID_BKGND_BMP_FIRST_H + 1,
       iBtnId = ID_RECT_BTN_UP,
       iBtnIdC = ID_CIRCULAR_BTN_UP;

   for( int k = 0; k < BKGND_NUMBBMP; k++, iIdN++, iIdS++, iIdH++, iBtnId++, iBtnIdC++ )
    {
      grcBmp[ k ].grcN = LoadBitmap( hInstRC, MAKEINTRESOURCE(iIdN) );
      grcBmp[ k ].grcS = LoadBitmap( hInstRC, MAKEINTRESOURCE(iIdS) );
      grcBmp[ k ].grcH = LoadBitmap( hInstRC, MAKEINTRESOURCE(iIdH) );

      if( !grcBmp[ k ].grcN || !grcBmp[ k ].grcS || !grcBmp[ k ].grcH )
       {
	 Status = ER_BITMAPLOAD;
         GetApplication()->Error( ER_BITMAPLOAD );
         TMessage msg; _fsetmem( &msg, sizeof(TMessage), 0 );
         Cancel( msg );
	 return;
       }

      HBITMAP hbm[ 3 ] = { grcBmp[ k ].grcN(), grcBmp[ k ].grcH(), grcBmp[ k ].grcS() };
      SendDlgItemMessage( HWindow, iBtnId, BBM_SETBITS, 0, (LONG)(LPSTR)(hbm) );
      SendDlgItemMessage( HWindow, iBtnIdC, BBM_SETBITS, 0, (LONG)(LPSTR)(hbm) );
    }

  


   for( int c = 0; c < iNumbOvlItems; c++ )
     *this + *new TBkGndDialog::TCtlData(
       TBkGndDialog::oiTblOvlItems[ c ].id,
       GetDlgItem( HWindow, TBkGndDialog::oiTblOvlItems[ c ].id ),
       TBkGndDialog::oiTblOvlItems[ c ].clItem
					);

   evErr.SetOwnerDialog( HWindow );
   hwndMix = GetDlgItem( HWindow, ID_MIX_GRADIENT );

   RECT r;
   GetClientRect( GetDlgItem(HWindow, ID_MIX_GRADIENT), &r );
   iWid = r.right; iHigh = r.bottom;
   mrcDrawBuf = (TRGB huge*)farcalloc( (DWORD)iWid * (DWORD)iHigh, sizeof(TRGB) );

   for( c = 0; c < iNumbScrolls; c++ )
     SetScrollRange( GetDlgItem( HWindow, TBkGndDialog::srTblRange[c].id ),
                     SB_CTL,
		     TBkGndDialog::srTblRange[c].left ,
		     TBkGndDialog::srTblRange[c].right ,
		     FALSE );

   InitEditors();

   int iCh = (iIndexSelected == -1) ? ID_RADIO_NONE:IdOfClass( ClassOf(iIndexSelected));
   SetFocus( GetDlgItem(HWindow, iCh) );
   SendDlgItemMessage( HWindow, iCh, BM_SETCHECK, 1, 0 );
   //SendDlgItemMessage( HWindow, iCh, BM_SETCHECK, 1, 0 );

   // int id, HWND hwnd_, classType cl_ )   
     
   //CreateOvlCtl();
 }


void TBkGndDialog::WMCommand( RTMessage Msg )
 {
   if( HIWORD(Msg.LParam) == BN_CLICKED )     
    {
      if( Msg.WParam >= ID_RADIO_FIRST && Msg.WParam <= ID_RADIO_END )
       {       
         DeleteOvlCtl();
         iIndexSelected = IndexOf( ClassOfRadio( Msg.WParam ) );

         if( iIndexSelected != -1 )
          {
	    EnableWindow( GetDlgItem(HWindow, ID_CHECK_TRACK), TRUE );
	    SendDlgItemMessage( HWindow, ID_CHECK_TRACK, BM_SETCHECK,
	      bkgndItems[ iIndexSelected ]->Track(), 0 );
          }
         else
	   EnableWindow( GetDlgItem(HWindow, ID_CHECK_TRACK), FALSE );

         CreateOvlCtl();
	 RefreshBmp( TRUE );
	 InvalidateRect( hwndMix, NULL, TRUE );

         Msg.Result = 0;
	 return;
       }
      else if( Msg.WParam == ID_CHECK_TRACK )
       {
         if( iIndexSelected != -1 )
	   bkgndItems[ iIndexSelected ]->Track(
	     SendDlgItemMessage(HWindow, ID_CHECK_TRACK, BM_GETCHECK, 0, 0) ? TRUE:FALSE
					      );

	 Msg.Result = 0;
	 return;
       }
      else  if( Msg.WParam == ID_BTN_PREVIEW )
       {
	 RefreshBmp( TRUE );
	 InvalidateRect( hwndMix, NULL, TRUE );

	 Msg.Result = 0;
	 return;
       }
      else if( Msg.WParam >= ID_RECT_BTN_UP && Msg.WParam <= ID_RECT_BTN_CENTER ||
	       Msg.WParam >= ID_CIRCULAR_BTN_UP && Msg.WParam <= ID_CIRCULAR_BTN_CENTER
             )
       {
	 RTBackGraundRectangular rBk = *PTBackGraundRectangular(bkgndItems[ iIndexSelected ]);
	 int iTmp;
	 switch( Msg.WParam )
	  {
	    case ID_RECT_BTN_UP:
	    case ID_CIRCULAR_BTN_UP:
	      iTmp = rBk.YCenter();
	      if( iTmp > 0 ) rBk.YCenter( --iTmp );
	      break;

	    case ID_RECT_BTN_DOWN:
	    case ID_CIRCULAR_BTN_DOWN:
	      iTmp = rBk.YCenter();
	      if( iTmp < 100 ) rBk.YCenter( ++iTmp );
	      break;

	    case ID_RECT_BTN_LEFT:
	    case ID_CIRCULAR_BTN_LEFT:
	      iTmp = rBk.XCenter();
	      if( iTmp > 0 ) rBk.XCenter( --iTmp );
	      break;

	    case ID_RECT_BTN_RIGHT:
	    case ID_CIRCULAR_BTN_RIGHT:
	      iTmp = rBk.XCenter();
	      if( iTmp < 100 ) rBk.XCenter( ++iTmp );
	      break;

	    case ID_RECT_BTN_CENTER:
	    case ID_CIRCULAR_BTN_CENTER:
	      rBk.XCenter( 50 );
	      rBk.YCenter( 50 );
	      break;
	  };

	 InvalidateRect(
	   GetDlgItem( HWindow,
	     (rBk.isA() == BK_Rectangular) ? ID_RECT_MIX_HILITE:
					      ID_CIRCULAR_MIX_HILITE
		     ),

	   NULL, FALSE );

	 Msg.Result = 0;
	 return;
       }
    }

   TCenterDialog::WMCommand( Msg );
 }

void TBkGndDialog::WMHScroll( RTMessage Msg )
 {
   if( CallConnector( Msg, GetDlgCtrlID((HWND)HIWORD(Msg.LParam)) ) == TRUE ) Msg.Result = 0;
   else TCenterDialog::DefWndProc( Msg );
 }

void TBkGndDialog::WMUserValueValidated( RTMessage msg )
 {
   CallConnector( msg, msg.WParam );   
   RefreshBmp( FALSE );
   msg.Result = 0;
 }

void TBkGndDialog::RefreshBmp( BOOL bFlNoCheck )
 {  
   if( iIndexSelected == -1 )
    {
      mrcGradientBmp.FreeRc();
      InvalidateRect( hwndMix, NULL, TRUE );
    }
   else
    {
      if( bFlNoCheck == FALSE &&
	  (*bkgndItems[ iIndexSelected ] == *bkgndItemsKey[ iIndexSelected ] ||
	   bkgndItems[ iIndexSelected ]->Track() == FALSE)
        )
	return;

      *bkgndItemsKey[ iIndexSelected ] = *bkgndItems[ iIndexSelected ];

      //_fmemset( mrcPalette(), 0, 234 * sizeof(TColorTable) );
      bkgndItems[ iIndexSelected ]->Init( mrcDrawBuf(), NULL, 0, iWid, iHigh, NULL );

      TNoisyDta ndDta;
      ndDta.nNoisy = N_None;
      ndDta.iMarginalAmplitude = 2;
      ndDta.iUniformAmplitude = 2;
      ndDta.iUniformThreshold = 10;


      TAutoCursorRc crcCursor( LoadCursor(NULL, IDC_WAIT) );
      mrcPalette = TrueColorTo256( mrcDrawBuf(), (DWORD)iWid * (DWORD)iHigh,
	236, FALSE, CA_MedianCutGeom, iWid, iHigh, &ndDta, NULL );
      crcCursor.FreeRc();

      mrcGradientBmp = new TDIB( mrcDrawBuf(), iWid, iHigh,
	mrcPalette(), 236, TRUE, GetApplication() );

      SendMessage( HWindow, WM_QUERYNEWPALETTE, 0, 0 );
    }    
 }

void TBkGndDialog::DeleteOvlCtl()
 {
   /*RemoveAllConnectors();
   RemoveAllValidators();
   listOfOvlCtl.flush( TShouldDelete::Delete );*/

   for( int i = 0; i < iNumbGradient; i++ )
    {
      ListIterator lIt = ListIterator( laCtl[i] );
      TBkGndDialog::PTCtlData pD = (TBkGndDialog::PTCtlData)&lIt.current();

      HDWP hdwpNext = BeginDeferWindowPos( laCtl[i].getItemsInContainer() );

      for( ; int(lIt); pD = (TBkGndDialog::PTCtlData)&++lIt )
	hdwpNext = DeferWindowPos( hdwpNext, pD->hwnd, NULL, 0, 0, 0, 0,
	  SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER );
	  //ShowWindow( pD->hwnd, SW_HIDE );

      EndDeferWindowPos( hdwpNext );
    }
 }

void TBkGndDialog::CreateOvlCtl()
 {
   if( iIndexSelected == -1 ) return;

   ListIterator lIt = ListIterator( laCtl[iIndexSelected] );
   TBkGndDialog::PTCtlData pD = (TBkGndDialog::PTCtlData)&lIt.current();

   HDWP hdwpNext = BeginDeferWindowPos( laCtl[iIndexSelected].getItemsInContainer() );

   for( ; int(lIt); pD = (TBkGndDialog::PTCtlData)&++lIt )
     hdwpNext = DeferWindowPos( hdwpNext, pD->hwnd, NULL, 0, 0, 0, 0,
       SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER );
     //ShowWindow( pD->hwnd, SW_SHOW );

   EndDeferWindowPos( hdwpNext );
 }

RTBkGndDialog TBkGndDialog::operator+( RTCtlData rD )
 {
   if( rD != NOOBJECT )
     laCtl[ IndexOf(rD.cl) ].add( rD );

   return *this;
 }

TBkGndDialog::TCtlData::~TCtlData()
 {
   int a = 1;
 }

