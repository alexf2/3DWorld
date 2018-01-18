#include "ambdlg.hpp"
#include "mpalette.hpp"
#include "status.hpp"
#include "autoctl.hpp"


#include "autorc.hpp"
#include "INC\io_em.inc"

#include "INC\ambdlg.inc"
#include "INC\errors.inc"

#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;

TAmbientDialog::TAmbientDialog( PTWindowsObject AParent, LPSTR AName,
				RGBQUAD& rRGBAmbient, double& rdAmbMult_,
				PTModule AModule ):
  TCenterDialog( AParent, AName, AModule ),
  rRGBAmb( rRGBAmbient ),
  rdAmbMult( rdAmbMult_ ),
  hpalPalette( NULL ),
  hwndMix( NULL )
 {
   /*psbRGBr = new TScrollBarAutoValidate( this, ID_AMBDLG_RGB_R_SCROLL, AModule );
   psbRGBg = new TScrollBarAutoValidate( this, ID_AMBDLG_RGB_G_SCROLL, AModule );
   psbRGBb = new TScrollBarAutoValidate( this, ID_AMBDLG_RGB_B_SCROLL, AModule );

   new TEditAutoValidate( this, ID_AMBDLG_RGB_R_INPUT, 4, AModule );
   new TEditAutoValidate( this, ID_AMBDLG_RGB_G_INPUT, 4, AModule );
   new TEditAutoValidate( this, ID_AMBDLG_RGB_B_INPUT, 4, AModule );*/

   new TBButtonAutoValidate( this, IDOK, AModule );
   new TBButtonAutoValidate( this, IDCANCEL, AModule );

   pMix = new TMixerConnected( this, ID_AMBDLG_RGB_MIX,
       rRGBAmb, NULL, 235, AModule );


   TAVEditConnected<BYTE>* pEd2;
   PTAVScrollConnectedMixer pSc2;

   pEd2 = new TAVEditConnected<BYTE>( this, ID_AMBDLG_RGB_R_INPUT, 10,
     (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rRGBAmb.rgbRed, AModule );
   pSc2 = psbRGBr = new TAVScrollConnectedMixer( this, ID_AMBDLG_RGB_R_SCROLL,
     (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rRGBAmb.rgbRed, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
       (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

   pEd2 = new TAVEditConnected<BYTE>( this, ID_AMBDLG_RGB_G_INPUT, 10,
     (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rRGBAmb.rgbGreen, AModule );
   pSc2 = psbRGBg = new TAVScrollConnectedMixer( this, ID_AMBDLG_RGB_G_SCROLL,
     (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rRGBAmb.rgbGreen, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
     (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

   pEd2 = new TAVEditConnected<BYTE>( this, ID_AMBDLG_RGB_B_INPUT, 10,
     (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rRGBAmb.rgbBlue, AModule );
   pSc2 = psbRGBb = new TAVScrollConnectedMixer( this, ID_AMBDLG_RGB_B_SCROLL,
     (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rRGBAmb.rgbBlue, 1.0, pMix, AModule );
   pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
   *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
     (PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

   new TEditAutoValidate( this, ID_AMBDLG_MULT_INPUT, 15, AModule );


   *this,
     *new TByteValidator( ID_AMBDLG_RGB_R_INPUT, "R", 0, 255, rRGBAmb.rgbRed ),
     *new TByteValidator( ID_AMBDLG_RGB_G_INPUT, "G", 0, 255, rRGBAmb.rgbGreen ),
     *new TByteValidator( ID_AMBDLG_RGB_B_INPUT, "B", 0, 255, rRGBAmb.rgbBlue ),
     *new TDValidator( ID_AMBDLG_MULT_INPUT, "Multiplier", 0.003, 255, rdAmbMult );

   *this,
     *new T3DFrame( this, ID_AMBDLG_RGB_R_INPUT, AModule ),
     *new T3DFrame( this, ID_AMBDLG_RGB_G_INPUT, AModule ),
     *new T3DFrame( this, ID_AMBDLG_RGB_B_INPUT, AModule ),
     *new T3DFrame( this, ID_AMBDLG_MULT_INPUT, AModule );

   *this,
       *new T3DThickFrame( this, ID_AMBDLG_RGB_MIX, 3, AModule );

 }

TAmbientDialog::~TAmbientDialog()
 {
   if( hpalPalette ) { DeleteObject(hpalPalette); hpalPalette = NULL; }
 }

void TAmbientDialog::SetupWindow()
 {
   //static char *cInf = "Setup ambient";
   TSendTxt stSend;
   stSend.handle = 0;
   TAutoStringRc str_( hInstRC, ID_MSG_89 );
   //stSend.lpTxt = cInf;
   stSend.lpTxt = str_();


   TCenterDialog::SetupWindow();

   evErr.SetOwnerDialog( HWindow );

   SendMessage( Parent->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );

   hpalPalette = CreateMeanPalette();
   pMix->SetPal( hpalPalette );
   hwndMix = GetDlgItem( HWindow, ID_AMBDLG_RGB_MIX );

   psbRGBr->SetRange( 0, 255 );
   psbRGBg->SetRange( 0, 255 );
   psbRGBb->SetRange( 0, 255 );

   InitEditors();
 }

void TAmbientDialog::WMHScroll( RTMessage Msg )
 {
   if( CallConnector( Msg, GetDlgCtrlID((HWND)HIWORD(Msg.LParam)) ) == TRUE ) Msg.Result = 0;
   else TCenterDialog::DefWndProc( Msg );
 }

void TAmbientDialog::WMUserValueValidated( RTMessage msg )
 {
   CallConnector( msg, msg.WParam );
   msg.Result = 0;
 }



void TAmbientDialog::WMCommand( RTMessage Msg )
 {  /*
   switch( HIWORD(Msg.LParam) )
    {
      case EN_USER_VALUE_VALIDATED:
	UpdateTiledScrollbar( Msg.WParam );
	break;
    } */

   TCenterDialog::WMCommand( Msg );
 }

