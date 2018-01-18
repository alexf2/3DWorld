#include "smothdlg.hpp"

#include "INC\smothdlg.inc"

#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;

TSmothDialog::TSmothDialog( PTWindowsObject AParent, LPSTR AName,
			   int& iAngleSmoth_, int& iSmothGrp_, int& iIdBtn_,
			   PTModule AModule ):
  TCenterDialog( AParent, AName, AModule ),
  iAngleSmoth( iAngleSmoth_ ),
  iSmothGrp( iSmothGrp_ ),
  iIdBtn( iIdBtn_ )
 {
   iIdBtn = 0;

   new TBButtonAutoValidate( this, IDCANCEL, AModule );

   new TBButtonAutoValidate( this, SMOTHDLG_BTN_AUTOSMOTH, AModule );
   new TBButtonAutoValidate( this, SMOTHDLG_BTN_SMOTHALL, AModule );
   new TBButtonAutoValidate( this, SMOTHDLG_BTN_CLEARALL, AModule );
   new TBButtonAutoValidate( this, SMOTHDLG_BTN_ASSIGNSMOTH, AModule );

   new TEditAutoValidate( this, SMOTHDLG_INPUT_ANGLE, 15, AModule );
   new TEditAutoValidate( this, SMOTHDLG_INPUT_GROUP, 15, AModule );
 		
   *this,
     *new TUIntValidator( SMOTHDLG_INPUT_ANGLE, "Maximum smoth angle", 0, 180, (unsigned int)iAngleSmoth ),
     *new TUIntValidator( SMOTHDLG_INPUT_GROUP, "Smothing group", 0, 3768, (unsigned int)iSmothGrp );
 }


void TSmothDialog::SetupWindow()
 {
   TCenterDialog::SetupWindow();

   evErr.SetOwnerDialog( HWindow ); 
   InitEditors();

   SetFocus( GetDlgItem(HWindow, SMOTHDLG_INPUT_GROUP) );
 }

void TSmothDialog::WMCommand( RTMessage Msg )
 {
   if( HIWORD(Msg.LParam) == BN_CLICKED )    
     switch( Msg.WParam )
      {
	case SMOTHDLG_BTN_AUTOSMOTH:
	case SMOTHDLG_BTN_SMOTHALL:
	case SMOTHDLG_BTN_CLEARALL:
	case SMOTHDLG_BTN_ASSIGNSMOTH:
	  iIdBtn = Msg.WParam;
          Ok( Msg );
	  //CloseWindow( Msg.WParam );
	  Msg.Result = 0;
          return;
      }

   TCenterDialog::WMCommand( Msg );
 }
