#include "noisydlg.hpp"
#include "INC\noisydlg.inc"


#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;



TNoisyDialog::TNoisyDialog( PTWindowsObject AParent, LPSTR AName,
			    PTNoisyDta pnDta,
			    PTModule AModule ):
  TCenterDialog( AParent, AName, AModule ),
  rnDta( *pnDta )
 {   
   new TBButtonAutoValidate( this, IDOK, AModule );
   new TBButtonAutoValidate( this, IDCANCEL, AModule );

   PTBGroupBox pGrpM = new TBGroupBox( this, IDNOISY_GRP_MAIN, AModule );

   new TBRadioButtonAutoValidate( this, IDNOISY_RADIO_MARGINAL, pGrpM, AModule );
   new TBRadioButtonAutoValidate( this, IDNOISY_RADIO_UNIFORM, pGrpM, AModule );
   new TBRadioButtonAutoValidate( this, IDNOISY_RADIO_NONE, pGrpM, AModule );

   new TEditAutoValidate( this, IDNOISY_INPUT_MARGINAL_AMPL, 15, AModule );
   new TEditAutoValidate( this, IDNOISY_INPUT_UNIFORM_AMPL, 15, AModule );


   TAVEditConnected<unsigned int>* pEd;
   TAVScrollConnected<unsigned int>* pSc;

   pEd = new TAVEditConnected<unsigned int>( this, IDNOISY_INPUT_UNIFORM_THSRESOL, 15,
     (PTSetupFunc)&TAVScrollConnected<unsigned int>::Setup, (unsigned int)rnDta.iUniformThreshold, AModule );
   pSc = new TAVScrollConnected<unsigned int>( this, IDNOISY_SCROLL_UNIFORM_THSRESOL,
     (PTSetupFunc)&TAVEditConnected<unsigned int>::Setup, (unsigned int)rnDta.iUniformThreshold, 1, AModule );
   pEd->Connect( pSc ); pSc->Connect( pEd );
   *this, *new TConnector( pEd, pSc, (PTUpdateFunc)&TAVEditConnected<unsigned int>::Update,
     (PTUpdateFunc)&TAVScrollConnected<unsigned int>::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );


   *this,
     *new TUIntValidator( IDNOISY_INPUT_MARGINAL_AMPL, "Marginal amplitude", 0, 10, (unsigned int)rnDta.iMarginalAmplitude ),
     *new TUIntValidator( IDNOISY_INPUT_UNIFORM_AMPL, "Uniform amplitude", 1, 10, (unsigned int)rnDta.iUniformAmplitude ),
     *new TUIntValidator( IDNOISY_INPUT_UNIFORM_THSRESOL, "Uniform thsresol", 1, 765, (unsigned int)rnDta.iUniformThreshold );
 }

void TNoisyDialog::Ok( RTMessage msg )
 {
   TNoisy noisy;

   if( SendDlgItemMessage( HWindow, IDNOISY_RADIO_MARGINAL, BM_GETCHECK, 0, 0 ) )
     noisy = N_Marginal;
   else if( SendDlgItemMessage( HWindow, IDNOISY_RADIO_UNIFORM, BM_GETCHECK, 0, 0 ) )
     noisy = N_Uniform;
   else noisy = N_None;

   rnDta.nNoisy = noisy;

   TCenterDialog::Ok( msg );
 }


  
void TNoisyDialog::SetupWindow()
 {   
   TCenterDialog::SetupWindow();

   evErr.SetOwnerDialog( HWindow );

   SetScrollRange( GetDlgItem( HWindow,
		     IDNOISY_SCROLL_UNIFORM_THSRESOL ),
                     SB_CTL,
                     1, 765,		     
		     FALSE );

   InitEditors();

   int id = (rnDta.nNoisy == N_Marginal) ? IDNOISY_RADIO_MARGINAL:
	    ((rnDta.nNoisy == N_Uniform) ? IDNOISY_RADIO_UNIFORM:
	    IDNOISY_RADIO_NONE);
   SendDlgItemMessage( HWindow, id, BM_SETCHECK, 1, 0 );
 }

void TNoisyDialog::WMCommand( RTMessage Msg )
 {
   TCenterDialog::WMCommand( Msg );
 }
  
void TNoisyDialog::WMHScroll( RTMessage Msg )
 {
   if( CallConnector( Msg, GetDlgCtrlID((HWND)HIWORD(Msg.LParam)) ) == TRUE ) Msg.Result = 0;
   else TCenterDialog::DefWndProc( Msg );
 }

void TNoisyDialog::WMUserValueValidated( RTMessage msg )
 {
   CallConnector( msg, msg.WParam );
   msg.Result = 0;
 }

