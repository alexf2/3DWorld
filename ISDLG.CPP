#include "isdlg.hpp"
#include "noisydlg.hpp"
#include "INC\isdlg.inc"

#include "INC\io_em.inc"
#include "INC\errors.inc"


#ifdef __DLL__
  extern PTModule pModule;
#else                    
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;
extern PTModule pModuleRc;

TBtnValueTbl TImageOptDialog::bvtTbl[ iNumbBtn ] =
 {
  { ID_ISDLG_256_RADIO, 256 },
  { ID_ISDLG_236_RADIO, 236 },
  { ID_ISDLG_16SYS_RADIO, -16 },
  { ID_ISDLG_16ADAPT_RADIO, 16 },
  { ID_ISDLG_TRUECOLOR_RADIO, 0 },
  { ID_ISDLG_WINDOWED_RADIO,  IF_Windowed },
  { ID_ISDLG_FULLSCREEN_RADIO, IF_FullScreen },
  { ID_ISDLG_640x480_RADIO, IF_640x480 },
  { ID_ISDLG_800x600_RADIO, IF_800x600 },
  { ID_ISDLG_1024x768_RADIO, IF_1024x768 }
 };

int TImageOptDialog::iAlhoBtnTbl[ iNumbBtnAlho ] =
 {
   ID_RADIO_QUANT,
   ID_RADIO_MEDIUMSECT,
   ID_RADIO_GEOMETRIC,
   ID_RADIO_DISCRET,
   ID_CHECK_DIFFUSION,
   ID_CHECK_NOISY
 };

int TImageOptDialog::Btn( int iValue ) const
 {
   for( int i = 0; i < iNumbBtn; i++ )
     if( TImageOptDialog::bvtTbl[ i ].iValueVariable == iValue )
       return TImageOptDialog::bvtTbl[ i ].iIdBtn;

   return -1;
 }

int TImageOptDialog::Value( int iBtn ) const
 {
   for( int i = 0; i < iNumbBtn; i++ )
     if( TImageOptDialog::bvtTbl[ i ].iIdBtn == iBtn )
       return TImageOptDialog::bvtTbl[ i ].iValueVariable;

   return -1;

 }

static char* cNoisyNames[ 3 ] =
 {
   "Marginal noisy",
   "Uniform noisy",
   "No noisy"
 };


void TImageOptDialog::CheckNoisy()
 {
   if( nNoisy > - 1 && nNoisy < 4 )
     SetDlgItemText( HWindow, ID_CHECK_NOISY, cNoisyNames[ nNoisy ] );
   SendDlgItemMessage( HWindow, ID_CHECK_NOISY, BM_SETCHECK, (WPARAM)TRUE, 0 );
 }

void TImageOptDialog::SetupNoisy()
 {
   TNoisyDta ndDta;

   ndDta.nNoisy             = nNoisy;
   ndDta.iMarginalAmplitude = iMarginalAmplitude;
   ndDta.iUniformAmplitude  = iUniformAmplitude;
   ndDta.iUniformThreshold  = iUniformThreshold;


   PTNoisyDialog pDlg;
   int res;
											 
   if( (pDlg = new TNoisyDialog( this, (LPSTR)MAKEINTRESOURCE(DlgNoisy), &ndDta, pModuleRc )) && !pDlg->Status )
     res = GetApplication()->ExecDialog( pDlg );
   else
    {
      GetApplication()->Error( pDlg ? pDlg->Status:ER_CREATEOBJECT );
      if( pDlg ) pDlg->CloseWindow();
      return;
    }

   if( res == IDOK )
    {
      nNoisy             = ndDta.nNoisy;
      iMarginalAmplitude = ndDta.iMarginalAmplitude;
      iUniformAmplitude  = ndDta.iUniformAmplitude;
      iUniformThreshold  = ndDta.iUniformThreshold;

      CheckNoisy();
    }
 }

TImageOptDialog::TImageOptDialog( PTWindowsObject AParent, LPSTR AName, int& iPalSize_,
		 TImageFormat& ifFormat_, TConvertAlho&  caAlhoPalette_,
		 TNoisy& nNoisy_, int& iMarginalAmplitude_,
		 int& iUniformAmplitude_, int& iUniformThreshold_,
		 PTModule AModule ):
   TCenterDialog( AParent, AName, AModule ),
   iPalSize( iPalSize_ ),
   ifFormat( ifFormat_ ),
   caAlhoPalette( caAlhoPalette_ ),

   nNoisy( nNoisy_ ),
   iMarginalAmplitude( iMarginalAmplitude_ ),
   iUniformAmplitude( iUniformAmplitude_ ),
   iUniformThreshold( iUniformThreshold_ )
 {
 }


void TImageOptDialog::SetupWindow()
 {
   TCenterDialog::SetupWindow();

   SendDlgItemMessage( HWindow, Btn(iPalSize), BM_SETCHECK, (WPARAM)TRUE, 0 );
   SendDlgItemMessage( HWindow, Btn( int(ifFormat) ), BM_SETCHECK, (WPARAM)TRUE, 0 );

   SendDlgItemMessage( HWindow, (caAlhoPalette & CA_Quantize) ? ID_RADIO_QUANT:ID_RADIO_MEDIUMSECT,
     BM_SETCHECK, 1, 0 );
   //if( (caAlhoPalette & (CA_MedianCutGeom | CA_MedianCutDiscr)) )
   SendDlgItemMessage( HWindow, (caAlhoPalette & CA_MedianCutGeom) ? ID_RADIO_GEOMETRIC:ID_RADIO_DISCRET,
       BM_SETCHECK, 1, 0 );
   SendDlgItemMessage( HWindow, ID_CHECK_DIFFUSION, BM_SETCHECK,
     (caAlhoPalette & CA_Diffuzion) ? 1:0, 0 );   
   

   LRESULT lCheckedTrue = SendDlgItemMessage( HWindow, ID_ISDLG_TRUECOLOR_RADIO,
     BM_GETCHECK, 0, 0 );
   LRESULT lCheckedSys16 = SendDlgItemMessage( HWindow, ID_ISDLG_16SYS_RADIO,
     BM_GETCHECK, 0, 0 );

   BOOL bFlEnable = (lCheckedTrue || lCheckedSys16) ? FALSE:TRUE;
   for( int i = 0; i < iNumbBtnAlho; i++ )
     EnableWindow( GetDlgItem(HWindow, TImageOptDialog::iAlhoBtnTbl[i]), bFlEnable );

   DetectRadioState();
   CheckNoisy();

   SetFocus( GetDlgItem(HWindow, IDOK) );
 }

void TImageOptDialog::DetectRadioState()
 {
   if( !IsWindowEnabled(GetDlgItem( HWindow, ID_RADIO_QUANT )) )
     return;

   int iFl = SendDlgItemMessage( HWindow, ID_RADIO_MEDIUMSECT, BM_GETCHECK, 0, 0 );

   EnableWindow( GetDlgItem(HWindow, ID_RADIO_GEOMETRIC), iFl == 1 );
   EnableWindow( GetDlgItem(HWindow, ID_RADIO_DISCRET), iFl == 1 );
 }

void TImageOptDialog::Ok( RTMessage msg )
 {
   TConvertAlho alho = 0;

   if( !SendDlgItemMessage( HWindow, ID_RADIO_MEDIUMSECT, BM_GETCHECK, 0, 0 ) )
     alho = CA_Quantize;
   else
     alho = SendDlgItemMessage( HWindow, ID_RADIO_GEOMETRIC, BM_GETCHECK, 0, 0 ) ?
	    CA_MedianCutGeom:CA_MedianCutDiscr;

   alho |= (SendDlgItemMessage( HWindow, ID_CHECK_DIFFUSION, BM_GETCHECK, 0, 0 ) ?
	    CA_Diffuzion:0);

   caAlhoPalette = alho;

   TCenterDialog::Ok( msg );
 }

void TImageOptDialog::WMCommand( RTMessage Msg )
 {
   if( HIWORD(Msg.LParam) == BN_CLICKED )
    {
      if( Msg.WParam >= ID_IMG_STA && Msg.WParam <= ID_IMG_END )
       {
         int iValue = Value( Msg.WParam );
         if( iValue != -1 )
          {
	    if( iValue == 0 || iValue == 236 || iValue == 256 ||
	        iValue == 16 || iValue == -16
              )
	      iPalSize = iValue;
	    else ifFormat = (TImageFormat)iValue;

	    if( Msg.WParam >= ID_ISDLG_256_RADIO && Msg.WParam <= ID_ISDLG_TRUECOLOR_RADIO )
             {
	       BOOL bFlEnable = TRUE;

	       if( Msg.WParam == ID_ISDLG_TRUECOLOR_RADIO ||
	           Msg.WParam == ID_ISDLG_16SYS_RADIO
	         )
	         bFlEnable = FALSE;

	       for( int i = 0; i < iNumbBtnAlho; i++ )
		 EnableWindow( GetDlgItem(HWindow, TImageOptDialog::iAlhoBtnTbl[i]), bFlEnable );

	       DetectRadioState();
             }

	    Msg.Result = 0;
	    return;
	  } 
       }
      else if( Msg.WParam >= ID_METHOD_STA && Msg.WParam <= ID_METHOD_END )
       {
	 switch( Msg.WParam )
	  {
	    case ID_RADIO_QUANT:
	    case ID_RADIO_MEDIUMSECT:
	      TCenterDialog::WMCommand( Msg );
	      DetectRadioState();
	      break;

	    case ID_RADIO_GEOMETRIC:
	    case ID_RADIO_DISCRET:
	      int iCh1 = SendDlgItemMessage( HWindow, ID_RADIO_GEOMETRIC, BM_GETCHECK, 0, 0 ),
		  iCh2 = SendDlgItemMessage( HWindow, ID_RADIO_DISCRET, BM_GETCHECK, 0, 0 );
	      int iTmp = iCh1; iCh1 = iCh2; iCh2 = iTmp;

	      TCenterDialog::WMCommand( Msg );

	      SendDlgItemMessage( HWindow, ID_RADIO_GEOMETRIC, BM_SETCHECK, iCh1, 0 );
	      SendDlgItemMessage( HWindow, ID_RADIO_DISCRET, BM_SETCHECK, iCh2, 0 );
	      break;

	    case ID_CHECK_NOISY:
	      SetupNoisy();	      
	      break;
	  }         

	 Msg.Result = 0;
	 return;
       }    
      
    }

   TCenterDialog::WMCommand( Msg );
 }

