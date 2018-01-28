
#include "prdlg.hpp"
#include "mpalette.hpp"
#include "status.hpp"
#include "autoctl.hpp"


#include "autorc.hpp"
#include "INC\io_em.inc"

#include "INC\prdlg.inc"
#include "INC\errors.inc"

#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;


TPRenderDialog::TPRenderDialog( PTWindowsObject AParent, LPSTR AName,
  RTPRenderDialogTransfer rTransfBuf, PTModule AModule ):
  TCenterDialog( AParent, AName, AModule ),
  rD( rTransfBuf ),
  hpalPalette( NULL ),
  hwndMixAd( NULL ),
  hwndMixIn( NULL )
 {
   new TBButtonAutoValidate( this, IDOK, AModule );
   new TBButtonAutoValidate( this, IDCANCEL, AModule );

   PTBGroupBox pG1 = new TBGroupBox( this, ID_PRDLG_GRP_METHOD, AModule );
   PTBGroupBox pG2 = new TBGroupBox( this, ID_PRDLG_GRP_INTERSECT, AModule );

   new TBRadioButtonAutoValidate( this, ID_PRDLG_RADIO_RAYTRACE, pG1, AModule );
   new TBRadioButtonAutoValidate( this, ID_PRDLG_RADIO_STRSCANZ, pG1, AModule );

   new TBCheckBoxAutoValidate( this, ID_PRDLG_CHECK_MARKINTERSECT, pG2, AModule );


   pMixAd = new TMixerConnected( this, ID_PRDLG_AD_RGB_MIX,
       rD.rgbAdges, NULL, 235, AModule );
   pMixIn = new TMixerConnected( this, ID_PRDLG_IN_RGB_MIX,
       rD.rgbIntersect, NULL, 234, AModule );


   TAVEditConnected<BYTE>* pEd2;
   PTAVScrollConnectedMixer pSc2;

   pEd2 = new TAVEditConnected<BYTE>( this, ID_PRDLG_AD_RGB_R_INPUT, 10,
     (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rD.rgbAdges.rgbRed, AModule );
   pSc2 = psbAdRGBr = new TAVScrollConnectedMixer( this, ID_PRDLG_AD_RGB_R_SCROLL,
     (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rD.rgbAdges.rgbRed, 1.0, pMixAd, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
       (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

   pEd2 = new TAVEditConnected<BYTE>( this, ID_PRDLG_AD_RGB_G_INPUT, 10,
     (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rD.rgbAdges.rgbGreen, AModule );
   pSc2 = psbAdRGBg = new TAVScrollConnectedMixer( this, ID_PRDLG_AD_RGB_G_SCROLL,
     (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rD.rgbAdges.rgbGreen, 1.0, pMixAd, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
     (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

   pEd2 = new TAVEditConnected<BYTE>( this, ID_PRDLG_AD_RGB_B_INPUT, 10,
     (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rD.rgbAdges.rgbBlue, AModule );
   pSc2 = psbAdRGBb = new TAVScrollConnectedMixer( this, ID_PRDLG_AD_RGB_B_SCROLL,
     (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rD.rgbAdges.rgbBlue, 1.0, pMixAd, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
     (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );


   pEd2 = new TAVEditConnected<BYTE>( this, ID_PRDLG_IN_RGB_R_INPUT, 10,
     (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rD.rgbIntersect.rgbRed, AModule );
   pSc2 = psbInRGBr = new TAVScrollConnectedMixer( this, ID_PRDLG_IN_RGB_R_SCROLL,
     (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rD.rgbIntersect.rgbRed, 1.0, pMixIn, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
       (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

   pEd2 = new TAVEditConnected<BYTE>( this, ID_PRDLG_IN_RGB_G_INPUT, 10,
     (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rD.rgbIntersect.rgbGreen, AModule );
   pSc2 = psbInRGBg = new TAVScrollConnectedMixer( this, ID_PRDLG_IN_RGB_G_SCROLL,
     (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rD.rgbIntersect.rgbGreen, 1.0, pMixIn, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
     (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

   pEd2 = new TAVEditConnected<BYTE>( this, ID_PRDLG_IN_RGB_B_INPUT, 10,
     (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rD.rgbIntersect.rgbBlue, AModule );
   pSc2 = psbInRGBb = new TAVScrollConnectedMixer( this, ID_PRDLG_IN_RGB_B_SCROLL,
     (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rD.rgbIntersect.rgbBlue, 1.0, pMixIn, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
     (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );


   *this,
     *new TByteValidator( ID_PRDLG_AD_RGB_R_INPUT, "R", 0, 255, rD.rgbAdges.rgbRed ),
     *new TByteValidator( ID_PRDLG_AD_RGB_G_INPUT, "G", 0, 255, rD.rgbAdges.rgbGreen ),
     *new TByteValidator( ID_PRDLG_AD_RGB_B_INPUT, "B", 0, 255, rD.rgbAdges.rgbBlue ),

     *new TByteValidator( ID_PRDLG_IN_RGB_R_INPUT, "R", 0, 255, rD.rgbIntersect.rgbRed ),
     *new TByteValidator( ID_PRDLG_IN_RGB_G_INPUT, "G", 0, 255, rD.rgbIntersect.rgbGreen ),
     *new TByteValidator( ID_PRDLG_IN_RGB_B_INPUT, "B", 0, 255, rD.rgbIntersect.rgbBlue );

   *this,
     *new T3DFrame( this, ID_PRDLG_AD_RGB_R_INPUT, AModule ),
     *new T3DFrame( this, ID_PRDLG_AD_RGB_G_INPUT, AModule ),
     *new T3DFrame( this, ID_PRDLG_AD_RGB_B_INPUT, AModule ),

     *new T3DFrame( this, ID_PRDLG_IN_RGB_R_INPUT, AModule ),
     *new T3DFrame( this, ID_PRDLG_IN_RGB_G_INPUT, AModule ),
     *new T3DFrame( this, ID_PRDLG_IN_RGB_B_INPUT, AModule );

   *this,
       *new T3DThickFrame( this, ID_PRDLG_AD_RGB_MIX, 3, AModule ),
       *new T3DThickFrame( this, ID_PRDLG_IN_RGB_MIX, 3, AModule );
 }

TPRenderDialog::~TPRenderDialog()
 {
   if( hpalPalette ) { DeleteObject(hpalPalette); hpalPalette = NULL; }
 }


void TPRenderDialog::SetupWindow()
 {
   TSendTxt stSend;
   stSend.handle = 0;  
   stSend.lpTxt = "Setup render";


   TCenterDialog::SetupWindow();

   evErr.SetOwnerDialog( HWindow );

   SendMessage( Parent->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );

   hpalPalette = CreateMeanPalette();
   pMixAd->SetPal( hpalPalette );
   pMixIn->SetPal( hpalPalette );
   hwndMixAd = GetDlgItem( HWindow, ID_PRDLG_AD_RGB_MIX );
   hwndMixIn = GetDlgItem( HWindow, ID_PRDLG_IN_RGB_MIX );

   psbAdRGBr->SetRange( 0, 255 );
   psbAdRGBg->SetRange( 0, 255 );
   psbAdRGBb->SetRange( 0, 255 );
   psbInRGBr->SetRange( 0, 255 );
   psbInRGBg->SetRange( 0, 255 );
   psbInRGBb->SetRange( 0, 255 );

   InitEditors();
   SendDlgItemMessage( HWindow, ID_PRDLG_CHECK_MARKINTERSECT, BM_SETCHECK, rD.bFlMarkIntersect, 0 ); 
   SetIntersect();
   SendDlgItemMessage( HWindow,
     (rD.polyMethod == 0) ? ID_PRDLG_RADIO_RAYTRACE:ID_PRDLG_RADIO_STRSCANZ,
     BM_SETCHECK, 1, 0 );
 }

void TPRenderDialog::WMHScroll( RTMessage Msg )
 {
   if( CallConnector( Msg, GetDlgCtrlID((HWND)HIWORD(Msg.LParam)) ) == TRUE ) Msg.Result = 0;
   else TCenterDialog::DefWndProc( Msg );
 }

void TPRenderDialog::WMUserValueValidated( RTMessage msg )
 {
   CallConnector( msg, msg.WParam );
   msg.Result = 0;
 }

void TPRenderDialog::WMCommand( RTMessage Msg )
 {
   if( HIWORD(Msg.LParam) == BN_CLICKED )
    {
      switch( Msg.WParam )
       {
	 case ID_PRDLG_RADIO_RAYTRACE:
	   rD.polyMethod = 0;
	   return;

	 case ID_PRDLG_RADIO_STRSCANZ:
	   rD.polyMethod = 1;
	   return;

	 case ID_PRDLG_CHECK_MARKINTERSECT:
	   rD.bFlMarkIntersect = SendDlgItemMessage( HWindow, ID_PRDLG_CHECK_MARKINTERSECT,
	     BM_GETCHECK, 0, 0 );
	   SetIntersect();
	   return;
       }
    }

   TCenterDialog::WMCommand( Msg );
 }

void TPRenderDialog::Ok( RTMessage Msg )
 {
   rD.bFlMarkIntersect = SendDlgItemMessage( HWindow, ID_PRDLG_CHECK_MARKINTERSECT,
     BM_GETCHECK, 0, 0 );
   rD.polyMethod = (SendDlgItemMessage( HWindow, ID_PRDLG_RADIO_RAYTRACE,
     BM_GETCHECK, 0, 0 ) == FALSE) ? 1:0;

   TCenterDialog::Ok( Msg );
 }

void TPRenderDialog::SetIntersect()
 {
   const int icNumbSwt = 9;
   static int iSwt[icNumbSwt] =
    {
      ID_PRDLG_IN_RGB_R_INPUT, ID_PRDLG_IN_RGB_G_INPUT, ID_PRDLG_IN_RGB_B_INPUT,
      ID_PRDLG_IN_RGB_R_SCROLL, ID_PRDLG_IN_RGB_G_SCROLL, ID_PRDLG_IN_RGB_B_SCROLL,
      ID_PRDLG_IN_RGB_R_TXT, ID_PRDLG_IN_RGB_G_TXT, ID_PRDLG_IN_RGB_B_TXT
    };


   //SendMessage( GetDlgItem(HWindow, ID_PRDLG_CHECK_MARKINTERSECT), BM_SETCHECK, (WPARAM)bFl, 0L );
   

   for( int i = 0; i < icNumbSwt; i++ )
     EnableWindow( GetDlgItem(HWindow, iSwt[i]),  rD.bFlMarkIntersect );
 }

