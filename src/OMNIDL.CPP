#include <stdio.h>

#include "omnidl.hpp"
#include "mpalette.hpp"
#include "status.hpp"
#include "autoctl.hpp"


#include "autorc.hpp"
#include "INC\io_em.inc"

#include "INC\omnidlg.inc"
#include "INC\errors.inc"

#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;

TOmniDialog::TOmniDialog( PTWindowsObject AParent, LPSTR AName,
			  RGBQUAD& rRGBOmni_, double& rdOmniMult_,
			  BOOL& bFlOn_,
			  BOOL& bFlCastShadows_,
			  LPSTR lpNameOmni,
			  PTModule AModule ):
  TCenterDialog( AParent, AName, AModule ),
  rRGBOmni( rRGBOmni_ ),
  rdOmniMult( rdOmniMult_ ),
  hpalPalette( NULL ),
  hwndMix( NULL ),
  bFlOn( bFlOn_ ),
  bFlCastShadows( bFlCastShadows_ ),
  strName( lpNameOmni )
 {
   new TBButtonAutoValidate( this, IDOK, AModule );
   new TBButtonAutoValidate( this, IDCANCEL, AModule );

   pMix = new TMixerConnected( this, ID_OMNIDLG_RGB_MIX,
       rRGBOmni, NULL, 235, AModule );

   PTBGroupBox pOptGrp = new TBGroupBox( this, ID_OMNIDLG_OPT_GRP, AModule );

   new TBRadioButtonAutoValidate( this, ID_OMNIDLG_ON_RADIO, pOptGrp, AModule );
   new TBRadioButtonAutoValidate( this, ID_OMNIDLG_OFF_RADIO, pOptGrp, AModule );
   new TBCheckBoxAutoValidate( this, ID_OMNIDLG_SHADOWS_CHECK, pOptGrp, AModule );

   TAVEditConnected<BYTE>* pEd2;
   PTAVScrollConnectedMixer pSc2;

   pEd2 = new TAVEditConnected<BYTE>( this, ID_OMNIDLG_RGB_R_INPUT, 10,
     (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rRGBOmni.rgbRed, AModule );
   pSc2 = psbRGBr = new TAVScrollConnectedMixer( this, ID_OMNIDLG_RGB_R_SCROLL,
     (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rRGBOmni.rgbRed, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
       (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

   pEd2 = new TAVEditConnected<BYTE>( this, ID_OMNIDLG_RGB_G_INPUT, 10,
     (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rRGBOmni.rgbGreen, AModule );
   pSc2 = psbRGBg = new TAVScrollConnectedMixer( this, ID_OMNIDLG_RGB_G_SCROLL,
     (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rRGBOmni.rgbGreen, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
     (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

   pEd2 = new TAVEditConnected<BYTE>( this, ID_OMNIDLG_RGB_B_INPUT, 10,
     (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rRGBOmni.rgbBlue, AModule );
   pSc2 = psbRGBb = new TAVScrollConnectedMixer( this, ID_OMNIDLG_RGB_B_SCROLL,
     (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rRGBOmni.rgbBlue, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
     (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

   new TEditAutoValidate( this, ID_OMNIDLG_MULT_INPUT, 15, AModule );


   *this,
     *new TByteValidator( ID_OMNIDLG_RGB_R_INPUT, "R", 0, 255, rRGBOmni.rgbRed ),
     *new TByteValidator( ID_OMNIDLG_RGB_G_INPUT, "G", 0, 255, rRGBOmni.rgbGreen ),
     *new TByteValidator( ID_OMNIDLG_RGB_B_INPUT, "B", 0, 255, rRGBOmni.rgbBlue ),
     *new TDValidator( ID_OMNIDLG_MULT_INPUT, "Multiplier", 0.001, 10000.0, rdOmniMult );

   *this,
     *new T3DFrame( this, ID_OMNIDLG_RGB_R_INPUT, AModule ),
     *new T3DFrame( this, ID_OMNIDLG_RGB_G_INPUT, AModule ),
     *new T3DFrame( this, ID_OMNIDLG_RGB_B_INPUT, AModule ),
     *new T3DFrame( this, ID_OMNIDLG_MULT_INPUT, AModule );

   *this,
       *new T3DThickFrame( this, ID_OMNIDLG_RGB_MIX, 3, AModule );

 }

TOmniDialog::~TOmniDialog()
 {
   if( hpalPalette ) { DeleteObject(hpalPalette); hpalPalette = NULL; }
 }

void TOmniDialog::Ok( RTMessage msg )
 {
   bFlOn = SendDlgItemMessage( HWindow, ID_OMNIDLG_ON_RADIO, BM_GETCHECK, 0, 0 );
   bFlCastShadows = SendDlgItemMessage( HWindow, ID_OMNIDLG_SHADOWS_CHECK, BM_GETCHECK, 0, 0 );

   TCenterDialog::Ok( msg );
 }

void TOmniDialog::SetupWindow()
 {
   
   /*TSendTxt stSend;
   stSend.handle = 0;
   TAutoStringRc str_( hInstRC, ID_MSG_89 );   
   stSend.lpTxt = str_();*/

   TCenterDialog::SetupWindow();

   char cBuf[256];
   sprintf( cBuf, "Setup omni '%s'", PCchar(strName) );
   SetCaption( cBuf );

   evErr.SetOwnerDialog( HWindow );

   //SendMessage( Parent->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );

   hpalPalette = CreateMeanPalette();
   pMix->SetPal( hpalPalette );
   hwndMix = GetDlgItem( HWindow, ID_OMNIDLG_RGB_MIX );

   psbRGBr->SetRange( 0, 255 );
   psbRGBg->SetRange( 0, 255 );
   psbRGBb->SetRange( 0, 255 );

   InitEditors();

   SendDlgItemMessage( HWindow, bFlOn ? ID_OMNIDLG_ON_RADIO:ID_OMNIDLG_OFF_RADIO,
     BM_SETCHECK, 1, 0 );
   SendDlgItemMessage( HWindow, ID_OMNIDLG_SHADOWS_CHECK,
     BM_SETCHECK, bFlCastShadows ? 1:0, 0 );
 }

void TOmniDialog::WMHScroll( RTMessage Msg )
 {
   if( CallConnector( Msg, GetDlgCtrlID((HWND)HIWORD(Msg.LParam)) ) == TRUE ) Msg.Result = 0;
   else TCenterDialog::DefWndProc( Msg );
 }

void TOmniDialog::WMUserValueValidated( RTMessage msg )
 {
   CallConnector( msg, msg.WParam );
   msg.Result = 0;
 }



void TOmniDialog::WMCommand( RTMessage Msg )
 {  /*
   switch( HIWORD(Msg.LParam) )
    {
      case EN_USER_VALUE_VALIDATED:
	UpdateTiledScrollbar( Msg.WParam );
	break;
    } */

   TCenterDialog::WMCommand( Msg );
 }

