#include <dialog.h>
#include <button.h>
#include <bgrpbox.h>

#include "phdlg.hpp"
#include "INC\phdlg.inc"

#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;

static void near pascal CheckAntialiasingState( HWND hwnd )
 {
   BOOL bFlEnabled = SendDlgItemMessage( hwnd, ID_PHDLG_CHECK_ANTIALIASING, BM_GETCHECK, 0, 0 );
   BOOL res = SendDlgItemMessage( hwnd, ID_PHDLG_RADIO_STRING, BM_GETCHECK, 0, 0 );

   if( res ) { bFlEnabled = 0; res = 0; }
   else res = 1;

   EnableWindow( GetDlgItem(hwnd, ID_PHDLG_INPUT_NUMBPIXELS), bFlEnabled );
   EnableWindow( GetDlgItem(hwnd, ID_PHDLG_INPUT_STRENGTS), bFlEnabled );

   EnableWindow( GetDlgItem(hwnd, ID_PHDLG_CHECK_ANTIALIASING), res );
 }

int TPhotoDlg::aiBtnAlias[ iNumBtn ] =
 {
   IDOK,
   IDCANCEL
 };

int TPhotoDlg::aiCheckAlias[ iNumbCheck ] =
 {
   ID_PHDLG_CHECK_ANTIALIASING,
   ID_PHDLG_CHECK_SHADOWS,
   ID_CHECK_STRICTGAMMA
//   ID_PHDLG_CHECK_RENDERBACK
 };

int TPhotoDlg::aiRadioAlias[ iNumbRadio ] =
 {
   ID_PHDLG_RADIO_FLAT,           
   ID_PHDLG_RADIO_PHONG,
   ID_PHDLG_RADIO_RAY,
   ID_PHDLG_RADIO_STRING,
   ID_PHDLG_RADIO_LOCAL,
   ID_PHDLG_RADIO_GLOBALHOLLS
 };



TPhotoDlg::TPhotoDlg( PTWindowsObject AParent, LPSTR AName,
		      RTPhotoSetupTransfer rTransf,
		      PTModule AModule ):
   rDta( rTransf ),
   TCenterDialog( AParent, AName, AModule )
 {
   PTBGroupBox pOptGrp = new TBGroupBox( this, ID_PHDLG_GRP_OPTIONS, AModule ),
	       pShadinfGrp = new TBGroupBox( this, ID_PHDLG_GRP_SHADINGLIM, AModule ),
	       pRenderAlhoGrp = new TBGroupBox( this, ID_PHDLG_GRP_RENDERALHORITM, AModule ),
	       pTraceMethodGrp = new TBGroupBox( this, ID_PHDLG_GRP_TRACEMETHOD, AModule );
   
   for( int k = 0; k < iNumBtn; k++ )
     new TBButtonAutoValidate( this, TPhotoDlg::aiBtnAlias[ k ], AModule );

   for( k = 0; k < iNumbCheck; k++ )
     new TBCheckBoxAutoValidate( this, TPhotoDlg::aiCheckAlias[ k ], pOptGrp, AModule );

   new TBRadioButtonAutoValidate( this, ID_PHDLG_RADIO_FLAT, pShadinfGrp, AModule );
   new TBRadioButtonAutoValidate( this, ID_PHDLG_RADIO_PHONG, pShadinfGrp, AModule );

   new TBRadioButtonAutoValidate( this, ID_PHDLG_RADIO_RAY, pRenderAlhoGrp, AModule );
   new TBRadioButtonAutoValidate( this, ID_PHDLG_RADIO_STRING, pRenderAlhoGrp, AModule );

   new TBRadioButtonAutoValidate( this, ID_PHDLG_RADIO_LOCAL, pTraceMethodGrp, AModule );
   new TBRadioButtonAutoValidate( this, ID_PHDLG_RADIO_GLOBALHOLLS, pTraceMethodGrp, AModule );

   new TEditAutoValidate( this, ID_PHDLG_INPUT_NUMBPIXELS, 15, AModule );
   new TEditAutoValidate( this, ID_PHDLG_INPUT_STRENGTS, 15, AModule );

   *this,
     *new TDValidator( ID_PHDLG_INPUT_STRENGTS, "Strengts", 0, 1, rDta.dStrengtsAntialias ),
     *new TUIntValidator( ID_PHDLG_INPUT_NUMBPIXELS, "Division", 2, 8, (unsigned int)rDta.iNumbPointsAntiAlias );
 }

TPhotoDlg::~TPhotoDlg()
 {

 }

void TPhotoDlg::SetupWindow()
 {
   TCenterDialog::SetupWindow();

   evErr.SetOwnerDialog( HWindow );
   

   SendDlgItemMessage( HWindow, ID_PHDLG_CHECK_ANTIALIASING, BM_SETCHECK, (WPARAM)rDta.bFlAntiAliasing, 0 );
   SendDlgItemMessage( HWindow, ID_PHDLG_CHECK_SHADOWS, BM_SETCHECK, (WPARAM)rDta.bFlShadows, 0 );
   SendDlgItemMessage( HWindow, ID_CHECK_STRICTGAMMA, BM_SETCHECK, (WPARAM)rDta.bFlStrictGamma, 0 );
   
   //SendDlgItemMessage( HWindow, ID_PHDLG_CHECK_RENDERBACK, BM_SETCHECK, (WPARAM)rDta.bFlRenderBack, 0 );

   SendDlgItemMessage( HWindow, (rDta.saShadingLimit == SA_Flat) ? ID_PHDLG_RADIO_FLAT:ID_PHDLG_RADIO_PHONG,
     BM_SETCHECK, (WPARAM)TRUE, 0 );
   SendDlgItemMessage( HWindow, (rDta.polyMethod == PM_RayTracing) ? ID_PHDLG_RADIO_RAY:ID_PHDLG_RADIO_STRING,
     BM_SETCHECK, (WPARAM)TRUE, 0 );
   SendDlgItemMessage( HWindow, (rDta.tmTraceModel == TM_Local) ? ID_PHDLG_RADIO_LOCAL:ID_PHDLG_RADIO_GLOBALHOLLS,
     BM_SETCHECK, (WPARAM)TRUE, 0 );

   CheckAntialiasingState( HWindow );
   EnableWindow( GetDlgItem(HWindow, ID_PHDLG_RADIO_GLOBALHOLLS), 0 );

   InitEditors();
 }

void TPhotoDlg::WMUserValueValidated( RTMessage msg )
 {
   CallConnector( msg, msg.WParam );
   msg.Result = 0;
 }


void TPhotoDlg::WMCommand( RTMessage Msg )
 {
   if( HIWORD(Msg.LParam) == BN_CLICKED &&
       (Msg.WParam == ID_PHDLG_CHECK_ANTIALIASING ||
	Msg.WParam == ID_PHDLG_RADIO_STRING ||
	Msg.WParam == ID_PHDLG_RADIO_RAY)
     )
     CheckAntialiasingState( HWindow );

   TCenterDialog::WMCommand( Msg );
 }


void TPhotoDlg::Ok( RTMessage msg )
 {
   rDta.bFlAntiAliasing = SendDlgItemMessage( HWindow, ID_PHDLG_CHECK_ANTIALIASING, BM_GETCHECK, 0, 0 );
   rDta.bFlShadows = SendDlgItemMessage( HWindow, ID_PHDLG_CHECK_SHADOWS, BM_GETCHECK, 0, 0 );
   rDta.bFlStrictGamma = SendDlgItemMessage( HWindow, ID_CHECK_STRICTGAMMA, BM_GETCHECK, 0, 0 );

   //rDta.bFlRenderBack = SendDlgItemMessage( HWindow, ID_PHDLG_CHECK_RENDERBACK, BM_GETCHECK, 0, 0 );

   rDta.saShadingLimit = SendDlgItemMessage( HWindow, ID_PHDLG_RADIO_FLAT,
     BM_GETCHECK, 0, 0 ) ? SA_Flat:SA_Phong;
   rDta.polyMethod = SendDlgItemMessage( HWindow, ID_PHDLG_RADIO_RAY,
     BM_GETCHECK, 0, 0 ) ? PM_RayTracing:PM_StringScan;
   rDta.tmTraceModel = SendDlgItemMessage( HWindow, ID_PHDLG_RADIO_LOCAL,
     BM_GETCHECK, 0, 0 ) ? TM_Local:TM_GlobalHolls;

   TCenterDialog::Ok( msg );
 }


