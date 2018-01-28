//#pragma option -v

#include <string.h>
#include <stdio.h>
#include <mem.h>
#include <bwcc.h>
#include <windowsx.h>
#include <bstatbmp.h>

#include "mpalette.hpp"
#include "autorc.hpp"
#include "INC\io_em.inc"

#include "medium.hpp"
#include "INC\medium.inc"
#include "INC\medbmp.inc"
#include "INC\errors.inc"

#include "status.hpp"
#include "autoctl.hpp"


#define PRECIZION 3
#define PRECIZION_MULT 1000


#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;

TAssignDialog::TScrollRange TAssignDialog::srTblRange[ iNumbScrolls_AssDlg ] =
 {
   { ID_MEDDLG_KA_SCROLL, 0, TAssignDialog::GetPos1(1) },
   { ID_MEDDLG_KD_SCROLL, 0, TAssignDialog::GetPos1(1) },
   { ID_MEDDLG_KS_SCROLL, 0, TAssignDialog::GetPos1(1) },

   { ID_MEDDLG_KT_SCROLL, 0, TAssignDialog::GetPos1(1) },
   { ID_MEDDLG_REFR_SCROLL, 0, TAssignDialog::GetPos1(1) },
   { ID_MEDDLG_BETTA_SCROLL, 0, TAssignDialog::GetPos1(1) },

   { ID_MEDDLG_RGB_R_SCROLL, 0, 255 },
   { ID_MEDDLG_RGB_G_SCROLL, 0, 255 },
   { ID_MEDDLG_RGB_B_SCROLL, 0, 255 }
 };


int TMedium::isEqual( const Object& rObj ) const
 {
   return (TMedium huge *)this == (TMedium huge *)&RTMedium(rObj);
 }

void TMedium::Copy( RTMedium rM )
 {
   dKa = rM.dKa;
   dKd = rM.dKd;
   dKs = rM.dKs;
   dKt = rM.dKt;
   rgbColor.rgbBlue = rM.rgbColor.rgbBlue;
   rgbColor.rgbGreen = rM.rgbColor.rgbGreen;
   rgbColor.rgbRed = rM.rgbColor.rgbRed;
   rgbColor.rgbReserved = rM.rgbColor.rgbReserved;

   iPhong = rM.iPhong;

   bNoLineTransp = rM.bNoLineTransp;

   dRefr = rM.dRefr ;
   dBetta = rM.dBetta;

   saShadingAlho = rM.saShadingAlho;

   lpName = NULL;
 }

TMedium::TMedium( RTMedium rMed )
 {
   //_fmemcpy( this, &rMed, sizeof(TMedium) );
   Copy( rMed );
   lpName = _fstrdup( rMed.lpName );
 }


RTMedium TMedium::operator=( RTMedium rMed )
 {
   //if( lpName ) { delete []lpName; lpName = NULL; }
   ResetName();

   //_fmemcpy( this, &rMed, sizeof(TMedium) );
   Copy( rMed );
   if( rMed.lpName )
     lpName = _fstrdup( rMed.lpName );
   else lpName = NULL;

   return *this;
 }

int WriteStr( HFILE hf, LPSTR lpStr )
 {
   int len = lpStr ? lstrlen( lpStr ):0;

   if( _lwrite( hf, &len, sizeof(int) ) == HFILE_ERROR )
     return HFILE_ERROR;

   if( !len ) return 0;

   return _lwrite( hf, lpStr, len );
 }

LPSTR ReadStr( HFILE hf )
 {
   int len;

   UINT res = _lread( hf, &len, sizeof(int) );
   if( res == HFILE_ERROR || res != sizeof(int) ) return NULL;


   LPSTR lpTmp = new char[ len + 1 ];
   if( len )
    {
      res = _lread( hf, lpTmp, len );
      if( res == HFILE_ERROR || res != len ) { delete []lpTmp; return NULL; }
    }

   lpTmp[ len ] = 0;

   return lpTmp;
 }


struct TMediumBuf
 {
  double  dKa,  
	  dKd,  
	  dKs,  
	  dKt;
  RGBQUAD rgbColor;
  unsigned int iPhong;

  double  dRefr,
	  dBetta;

  TShadingAlho saShadingAlho;

 // LPSTR   lpName;
 };

int TMedium::Save( HFILE hf )
 {
   TMediumBuf mB;

   mB.dKa = dKa;
   mB.dKd = dKd;
   mB.dKs = dKs;
   mB.dKt = dKt;

   mB.iPhong = iPhong;
   mB.dRefr = dRefr;
   mB.dBetta = dBetta;

   if( bNoLineTransp == FALSE ) mB.dRefr = mB.dBetta = mB.dKt;

   mB.rgbColor = rgbColor;

   mB.saShadingAlho = saShadingAlho;

   if( _lwrite(hf, &mB, sizeof(TMediumBuf)) == HFILE_ERROR ) return HFILE_ERROR;
   
   return WriteStr( hf, lpName );
 }

int TMedium::Load( HFILE hf )
 {
   TMediumBuf mB;

   UINT l;
   if( (l = _lread(hf, &mB, sizeof(TMediumBuf))) == HFILE_ERROR ) return HFILE_ERROR;
   if( l != sizeof(TMediumBuf) ) return HFILE_ERROR;

   dKa = mB.dKa;
   dKd = mB.dKd;
   dKs = mB.dKs;
   dKt = mB.dKt;

   iPhong = mB.iPhong;
   dRefr = mB.dRefr;
   dBetta = mB.dBetta;

   rgbColor = mB.rgbColor;

   if( dKt > 0 && dRefr == dBetta && dKt == dRefr ) bNoLineTransp = FALSE;
   else bNoLineTransp = TRUE;

   saShadingAlho = mB.saShadingAlho;

   LPSTR lpTmp;
   if( !(lpTmp = ReadStr(hf)) ) return HFILE_ERROR;

   ResetName();
   lpName = lpTmp;

   return lstrlen( lpTmp );
 }

void TMedium::ResetName()
 {
   if( lpName ) { delete []lpName; lpName = NULL; }
 }

int TMedium::operator==( RTMedium rMed )
 {
   if( !lpName || !rMed.lpName ) return 0;
   return !strcmpi( rMed.lpName, lpName );
 }


TMedium::TMedium()
 {
   static char *lpDefName = "Default";

   dKa = 0.5;
   dKd = 0.6;
   dKs = 0.8;
   dKt = 0;
   rgbColor.rgbBlue = 0x80;
   rgbColor.rgbReserved = rgbColor.rgbRed = rgbColor.rgbGreen = 0;

   iPhong = 10;

   dRefr  = 0;
   dBetta = 0;

   bNoLineTransp = FALSE;

   saShadingAlho = SA_Phong;

   lpName = new char[ _fstrlen(lpDefName) + 1 ];
   if( lpName ) _fstrcpy( lpName, lpDefName );
 }

TMedium::~TMedium()
 {
   //if( lpName ) { delete []lpName; lpName = NULL; }
   ResetName();
 }

TAssignDialog::TAssignDialog( PTWindowsObject AParent, LPSTR AName, RTMedium rmSel,
    DoubleList _FAR & rdblLib, PTModule AModule ):

    TCenterDialog( AParent, AName, AModule ),
    rdblLibrary( rdblLib ),
    rmSelected( rmSel ),
    hpalPalette( NULL ),
    hwndMix( NULL )
   {
     //ltLock = LT_NoLocked;

     _fsetmem( bbControlBmp, sizeof(TAssignDialog::TBtnBmp) * MEDDLG_NUMB_CONTROL_BMP, 0 );

     new TBButtonAutoValidate( this, ID_MEDDLG_ADD_BTN, AModule );
     new TBButtonAutoValidate( this, ID_MEDDLG_DEL_BTN, AModule );
     new TBButtonAutoValidate( this, IDOK, AModule );
     new TBButtonAutoValidate( this, IDCANCEL, AModule );

     new TComboBoxAutoValidate( this, ID_MEDDLG_MEDIUM_COMBO, 127, AModule );
		      //TAVColorScrollBar


     pMix = new TMixerConnected( this, ID_MEDDLG_RGB_MIX,
       rmSelected.rgbColor, NULL, 235, AModule );


     PTBGroupBox pShadingGrp = new TBGroupBox( this, ID_MEDDLG_SHADING_GRP, AModule );

     new TBRadioButtonAutoValidate( this, ID_MEDDLG_SHADING_FLAT_RADIO, pShadingGrp, AModule );
     new TBRadioButtonAutoValidate( this, ID_MEDDLG_SHADING_PHONG_RADIO, pShadingGrp, AModule );


     /*new TScrollBarAutoValidate( this, ID_MEDDLG_KA_SCROLL, AModule );
     new TScrollBarAutoValidate( this, ID_MEDDLG_KD_SCROLL, AModule );
     new TScrollBarAutoValidate( this, ID_MEDDLG_KS_SCROLL, AModule );
     new TScrollBarAutoValidate( this, ID_MEDDLG_KT_SCROLL, AModule );
     new TScrollBarAutoValidate( this, ID_MEDDLG_RGB_R_SCROLL, AModule );
     new TScrollBarAutoValidate( this, ID_MEDDLG_RGB_G_SCROLL, AModule );
     new TScrollBarAutoValidate( this, ID_MEDDLG_RGB_B_SCROLL, AModule );
     new TScrollBarAutoValidate( this, ID_MEDDLG_REFR_SCROLL, AModule );
     new TScrollBarAutoValidate( this, ID_MEDDLG_BETTA_SCROLL, AModule );
       */

     TAVEditConnected<double>* pEd;
     TAVScrollConnected<double>* pSc;

     pEd = new TAVEditConnected<double>( this, ID_MEDDLG_KA_INPUT, 15,
       (PTSetupFunc)&TAVScrollConnected<double>::Setup, rmSelected.dKa, AModule );
     pSc = new TAVScrollConnected<double>( this, ID_MEDDLG_KA_SCROLL,
       (PTSetupFunc)&TAVEditConnected<double>::Setup, rmSelected.dKa, 1000.0, AModule );
     pEd->Connect( pSc ); pSc->Connect( pEd );
     *this, *new TConnector( pEd, pSc, (PTUpdateFunc)&TAVEditConnected<double>::Update,
	(PTUpdateFunc)&TAVScrollConnected<double>::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

     pEd = new TAVEditConnected<double>( this, ID_MEDDLG_KD_INPUT, 15,
       (PTSetupFunc)&TAVScrollConnected<double>::Setup, rmSelected.dKd, AModule );
     pSc = new TAVScrollConnected<double>( this, ID_MEDDLG_KD_SCROLL,
       (PTSetupFunc)&TAVEditConnected<double>::Setup, rmSelected.dKd, 1000.0, AModule );
     pEd->Connect( pSc ); pSc->Connect( pEd );
     *this, *new TConnector( pEd, pSc, (PTUpdateFunc)&TAVEditConnected<double>::Update,
	(PTUpdateFunc)&TAVScrollConnected<double>::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

     pEd = new TAVEditConnected<double>( this, ID_MEDDLG_KS_INPUT, 15,
       (PTSetupFunc)&TAVScrollConnected<double>::Setup, rmSelected.dKs, AModule );
     pSc = new TAVScrollConnected<double>( this, ID_MEDDLG_KS_SCROLL,
       (PTSetupFunc)&TAVEditConnected<double>::Setup, rmSelected.dKs, 1000.0, AModule );
     pEd->Connect( pSc ); pSc->Connect( pEd );
     *this, *new TConnector( pEd, pSc, (PTUpdateFunc)&TAVEditConnected<double>::Update,
	(PTUpdateFunc)&TAVScrollConnected<double>::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

     pEd = new TAVEditConnected<double>( this, ID_MEDDLG_KT_INPUT, 15,
       (PTSetupFunc)&TAVScrollConnected<double>::Setup, rmSelected.dKt, AModule );
     pSc = new TAVScrollConnected<double>( this, ID_MEDDLG_KT_SCROLL,
       (PTSetupFunc)&TAVEditConnected<double>::Setup, rmSelected.dKt, 1000.0, AModule );
     pEd->Connect( pSc ); pSc->Connect( pEd );
     *this, *new TConnector( pEd, pSc, (PTUpdateFunc)&TAVEditConnected<double>::Update,
	(PTUpdateFunc)&TAVScrollConnected<double>::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

     pEd = new TAVEditConnected<double>( this, ID_MEDDLG_REFR_INPUT, 15,
       (PTSetupFunc)&TAVScrollConnected<double>::Setup, rmSelected.dRefr, AModule );
     pSc = new TAVScrollConnected<double>( this, ID_MEDDLG_REFR_SCROLL,
       (PTSetupFunc)&TAVEditConnected<double>::Setup, rmSelected.dRefr, 1000.0, AModule );
     pEd->Connect( pSc ); pSc->Connect( pEd );
     *this, *new TConnector( pEd, pSc, (PTUpdateFunc)&TAVEditConnected<double>::Update,
	(PTUpdateFunc)&TAVScrollConnected<double>::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

     pEd = new TAVEditConnected<double>( this, ID_MEDDLG_BETTA_INPUT, 15,
       (PTSetupFunc)&TAVScrollConnected<double>::Setup, rmSelected.dBetta, AModule );
     pSc = new TAVScrollConnected<double>( this, ID_MEDDLG_BETTA_SCROLL,
       (PTSetupFunc)&TAVEditConnected<double>::Setup, rmSelected.dBetta, 1000.0, AModule );
     pEd->Connect( pSc ); pSc->Connect( pEd );
     *this, *new TConnector( pEd, pSc, (PTUpdateFunc)&TAVEditConnected<double>::Update,
	(PTUpdateFunc)&TAVScrollConnected<double>::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

     TAVEditConnected<BYTE>* pEd2;
     TAVScrollConnected<BYTE>* pSc2;

     pEd2 = new TAVEditConnected<BYTE>( this, ID_MEDDLG_RGB_R_INPUT, 10,
       (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rmSelected.rgbColor.rgbRed, AModule );
     pSc2 = new TAVScrollConnectedMixer( this, ID_MEDDLG_RGB_R_SCROLL,
       (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rmSelected.rgbColor.rgbRed, 1.0, pMix, AModule );
     pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
     *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	(PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

     pEd2 = new TAVEditConnected<BYTE>( this, ID_MEDDLG_RGB_G_INPUT, 10,
       (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rmSelected.rgbColor.rgbGreen, AModule );
     pSc2 = new TAVScrollConnectedMixer( this, ID_MEDDLG_RGB_G_SCROLL,
       (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rmSelected.rgbColor.rgbGreen, 1.0, pMix, AModule );
     pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
     *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	(PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );

     pEd2 = new TAVEditConnected<BYTE>( this, ID_MEDDLG_RGB_B_INPUT, 10,
       (PTSetupFunc)&TAVScrollConnectedMixer::Setup, rmSelected.rgbColor.rgbBlue, AModule );
     pSc2 = new TAVScrollConnectedMixer( this, ID_MEDDLG_RGB_B_SCROLL,
       (PTSetupFunc)&TAVEditConnected<BYTE>::Setup, rmSelected.rgbColor.rgbBlue, 1.0, pMix, AModule );
     pEd2->Connect( pSc2 ); pSc2->Connect( pEd2 );
     *this, *new TConnector( pEd2, pSc2, (PTUpdateFunc)&TAVEditConnected<BYTE>::Update,
	(PTUpdateFunc)&TAVScrollConnectedMixer::Update, WM_USER_VALUE_VALIDATED, WM_HSCROLL );


     new TEditAutoValidate( this, ID_MEDDLG_PHONG_INPUT, 15, AModule );

     /*new TEditAutoValidate( this, ID_MEDDLG_KA_INPUT, 15, AModule );
     new TEditAutoValidate( this, ID_MEDDLG_KD_INPUT, 15, AModule );
     new TEditAutoValidate( this, ID_MEDDLG_KS_INPUT, 15, AModule );
     new TEditAutoValidate( this, ID_MEDDLG_KT_INPUT, 15, AModule );
     new TEditAutoValidate( this, ID_MEDDLG_PHONG_INPUT, 10, AModule );
     new TEditAutoValidate( this, ID_MEDDLG_REFR_INPUT, 15, AModule );
     new TEditAutoValidate( this, ID_MEDDLG_BETTA_INPUT, 15, AModule );
     new TEditAutoValidate( this, ID_MEDDLG_RGB_R_INPUT, 10, AModule );
     new TEditAutoValidate( this, ID_MEDDLG_RGB_G_INPUT, 10, AModule );
     new TEditAutoValidate( this, ID_MEDDLG_RGB_B_INPUT, 10, AModule );
       */
     new TBCheckBoxAutoValidate( this, ID_MEDDLG_LIGHTGOOUT_CHECK, NULL );
     new TBCheckBoxAutoValidate( this, ID_MEDDLG_NOLINE_CHECK, NULL );

     *this, *new TUIntValidator( ID_MEDDLG_PHONG_INPUT, "Phong", 1, UINT_MAX, rmSelected.iPhong ),

	    *new TDValidator( ID_MEDDLG_KA_INPUT, "Ka", 0, 1, rmSelected.dKa ),
	    *new TDValidator( ID_MEDDLG_KD_INPUT, "Kd", 0, 1, rmSelected.dKd ),
	    *new TDValidator( ID_MEDDLG_KS_INPUT, "Ks", 0, 1, rmSelected.dKs ),
	    *new TDValidator( ID_MEDDLG_KT_INPUT, "Kt", 0, 1, rmSelected.dKt ),

	    *new TDValidator( ID_MEDDLG_REFR_INPUT, "Refr", 0, 1, rmSelected.dRefr ),
	    *new TDValidator( ID_MEDDLG_BETTA_INPUT, "Betta", 0, 1, rmSelected.dBetta ),

	    *new TByteValidator( ID_MEDDLG_RGB_R_INPUT, "R", 0, 255, rmSelected.rgbColor.rgbRed ),
	    *new TByteValidator( ID_MEDDLG_RGB_G_INPUT, "G", 0, 255, rmSelected.rgbColor.rgbGreen ),
	    *new TByteValidator( ID_MEDDLG_RGB_B_INPUT, "B", 0, 255, rmSelected.rgbColor.rgbBlue );

     *this, *new T3DFrame( this, ID_MEDDLG_RGB_R_INPUT, AModule ),
	    *new T3DFrame( this, ID_MEDDLG_RGB_G_INPUT, AModule ),
	    *new T3DFrame( this, ID_MEDDLG_RGB_B_INPUT, AModule ),

	    *new T3DFrame( this, ID_MEDDLG_REFR_INPUT, AModule ),
	    *new T3DFrame( this, ID_MEDDLG_BETTA_INPUT, AModule ),

	    *new T3DFrame( this, ID_MEDDLG_KA_INPUT, AModule ),
	    *new T3DFrame( this, ID_MEDDLG_KD_INPUT, AModule ),
	    *new T3DFrame( this, ID_MEDDLG_KS_INPUT, AModule ),
	    *new T3DFrame( this, ID_MEDDLG_KT_INPUT, AModule ),

	    *new T3DFrame( this, ID_MEDDLG_PHONG_INPUT, AModule );

     *this,
       *new T3DThickFrame( this, ID_MEDDLG_RGB_MIX, 3, AModule );
   };


void TAssignDialog::Ok( RTMessage Msg )
 {  
   if( !GetName() )
    {
      if( SendDlgItemMessage( HWindow, ID_MEDDLG_NOLINE_CHECK,
	    BM_GETCHECK, 0, 0 ) == 1 &&
	    SendDlgItemMessage( HWindow, ID_MEDDLG_LIGHTGOOUT_CHECK,
	    BM_GETCHECK, 0, 0 ) == 1 &&
	    (rmSelected.dRefr > 0 || rmSelected.dBetta > 0)
	)       
        rmSelected.bNoLineTransp = TRUE;       
      else
       {
	 rmSelected.bNoLineTransp = FALSE;
	 rmSelected.dRefr = rmSelected.dBetta = rmSelected.dKt;
       }

      TCenterDialog::Ok( Msg );
    }
 }

void TAssignDialog::SetupWindow()
 {
   //static char *cInf = "Setup surface material.";
   TSendTxt stSend;
   stSend.handle = 0;
   TAutoStringRc str_( hInstRC, ID_MSG_34 );
   //stSend.lpTxt = cInf;
   stSend.lpTxt = str_();


   TCenterDialog::SetupWindow();

   evErr.SetOwnerDialog( HWindow );

   SendMessage( Parent->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );


   HCURSOR hcOld = SetCursor( LoadCursor(NULL, IDC_WAIT) );
   HWND hwndOldCap = SetCapture( HWindow );




   int iCurId = ID_FIRST_MEDDLG + 1;
   int iIdBmpN = MEDDLG_FIRST_N + 1,
       iIdBmpH = MEDDLG_FIRST_H + 1,
       iIdBmpS = MEDDLG_FIRST_S + 1;
   for( int i = 0; i < MEDDLG_NUMB_CONTROL_BMP; i++, iCurId++,
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


   ReleaseCapture();
   SetCapture( hwndOldCap );
   SetCursor( hcOld );

   hpalPalette = CreateMeanPalette();
   pMix->SetPal( hpalPalette );


   for( int ic = 0; ic < iNumbScrolls_AssDlg; ic++ )
     SetScrollRange( GetDlgItem( HWindow,
		     TAssignDialog::srTblRange[ ic ].id ),
                     SB_CTL,
                     TAssignDialog::srTblRange[ ic ].left,
                     TAssignDialog::srTblRange[ ic ].right,
                     FALSE );

   InitEditors();

   SetName();

   SetLightsGoOut( rmSelected.IsLightsGoOut() );

   LoadNames();

   hwndMix = GetDlgItem( HWindow, ID_MEDDLG_RGB_MIX );

   //SetMix();

   SelectFounded();

   if( rmSelected.lpName )
    {     
      RTMedium rMedFind = (RTMedium)rdblLibrary.firstThat( FindMedInstance, Pvoid(rmSelected.lpName) );
      if( rMedFind == NOOBJECT )     
	SetDlgItemText( HWindow, ID_MEDDLG_MEDIUM_COMBO, rmSelected.lpName );
    }

   SendDlgItemMessage( HWindow, (rmSelected.saShadingAlho == SA_Flat) ? ID_MEDDLG_SHADING_FLAT_RADIO:ID_MEDDLG_SHADING_PHONG_RADIO,
		BM_SETCHECK, 1, 0 );
   SendDlgItemMessage( HWindow, ID_MEDDLG_NOLINE_CHECK,
		BM_SETCHECK, (rmSelected.bNoLineTransp == TRUE) ? 1:0, 0 );
		
 }

int TAssignDialog::GetPos1( double dValue )
 {
   return int( dValue * double(PRECIZION_MULT) );
 }

double TAssignDialog::UnGetPos1( int iValue ) const
 {
   return double(iValue) / double(PRECIZION_MULT);
 }


void TAssignDialog::SetName()
 {
   SetDlgItemText( HWindow, ID_MEDDLG_MEDIUM_COMBO, rmSelected.lpName ? rmSelected.lpName:"" );
 }

void TAssignDialog::SetLightsGoOut( BOOL bFl )
 {
   const int icNumbSwt = 10;
   static int iSwt[icNumbSwt] =
    {
      ID_MEDDLG_REFR_INPUT, ID_MEDDLG_BETTA_INPUT, ID_MEDDLG_REFR_SCROLL,
      ID_MEDDLG_BETTA_SCROLL, ID_MEDDLG_KT_INPUT, ID_MEDDLG_KT_SCROLL,
      ID_MEDDLG_REFR_TXT, ID_MEDDLG_BETTA_TXT, ID_MEDDLG_KT_TXT,
      ID_MEDDLG_NOLINE_CHECK
    };

   SendMessage( GetDlgItem(HWindow, ID_MEDDLG_LIGHTGOOUT_CHECK), BM_SETCHECK, (WPARAM)bFl, 0L );

   BOOL flEn = bFl;

   if( flEn == FALSE )
    {
      rmSelected.dRefr = 
      rmSelected.dBetta = 
      rmSelected.dKt = 0;
      Update( ID_MEDDLG_BETTA_INPUT );
      Update( ID_MEDDLG_KT_INPUT );
      Update( ID_MEDDLG_REFR_INPUT );
    }

   for( int i = 0; i < icNumbSwt; i++ )
     EnableWindow( GetDlgItem(HWindow, iSwt[i]),   flEn );

 }



void TAssignDialog::CorrectError()
 {
   TCenterDialog::CorrectError(); 
 }


int TAssignDialog::GetName()
 {
   char cBuf[128];  

   GetDlgItemText( HWindow, ID_MEDDLG_MEDIUM_COMBO, cBuf, 127 );

   //if( rmSelected.lpName ) { delete []lpName; lpName = NULL; }
   rmSelected.ResetName();
   if( strlen(cBuf) )  
     rmSelected.lpName = _fstrdup( cBuf );

   SetName();

   return 0;
 }

static void LoadNamesInstance( Object _FAR &rObj, void _FAR *hwnd )
 {
   SendMessage( (HWND)hwnd, CB_ADDSTRING, 0, RTMedium(rObj).lpName ?
     (LPARAM)RTMedium(rObj).lpName:(LPARAM)"" );
 }

void TAssignDialog::LoadNames()
 {
   HWND hwnd = GetDlgItem( HWindow, ID_MEDDLG_MEDIUM_COMBO );

   SendMessage( hwnd, CB_RESETCONTENT, 0, 0 );

   rdblLibrary.forEach( LoadNamesInstance, Pvoid(hwnd) );//&&&

   //InvalidateRect( hwnd, NULL, TRUE );
 }

/*
void TAssignDialog::UpdateTiledScrollbar( int id )
 {
   HWND hwnd;
   int pos;

   switch( id )
    {
      case ID_MEDDLG_KA_INPUT:
	hwnd = GetDlgItem( HWindow, ID_MEDDLG_KA_SCROLL );
	pos = GetPos1( rmSelected.dKa );	
        break;

      case ID_MEDDLG_KD_INPUT:
	hwnd = GetDlgItem( HWindow, ID_MEDDLG_KD_SCROLL );
	pos = GetPos1( rmSelected.dKd );	
	break;

      case ID_MEDDLG_KS_INPUT:
        hwnd = GetDlgItem( HWindow, ID_MEDDLG_KS_SCROLL );
	pos = GetPos1( rmSelected.dKs );
	break;

      case ID_MEDDLG_KT_INPUT:
        hwnd = GetDlgItem( HWindow, ID_MEDDLG_KT_SCROLL );
	pos = GetPos1( rmSelected.dKt );
	break;


      case ID_MEDDLG_REFR_INPUT:
	hwnd = GetDlgItem( HWindow, ID_MEDDLG_REFR_SCROLL );
	pos = GetPos1( rmSelected.dRefr );
	break;

      case ID_MEDDLG_BETTA_INPUT:
        hwnd = GetDlgItem( HWindow, ID_MEDDLG_BETTA_SCROLL );
	pos = GetPos1( rmSelected.dBetta );
	break;

      case ID_MEDDLG_RGB_R_INPUT:
	hwnd = GetDlgItem( HWindow, ID_MEDDLG_RGB_R_SCROLL );
	pos = rmSelected.rgbColor.rgbRed;
	break;

      case ID_MEDDLG_RGB_G_INPUT:
        hwnd = GetDlgItem( HWindow, ID_MEDDLG_RGB_G_SCROLL );
	pos = rmSelected.rgbColor.rgbGreen;
	break;

      case ID_MEDDLG_RGB_B_INPUT:
	hwnd = GetDlgItem( HWindow, ID_MEDDLG_RGB_B_SCROLL );
	pos = rmSelected.rgbColor.rgbBlue;
	break;

      default:
	return;
    }

   if( GetScrollPos(hwnd, SB_CTL) != pos )
    {
      ltLock = TAssignDialog::LT_EditorsLock;
      SetScrollPos( hwnd, SB_CTL, pos, IsWindowEnabled(hwnd) );
      if( (id == ID_MEDDLG_RGB_B_INPUT || id == ID_MEDDLG_RGB_G_INPUT ||
	  id == ID_MEDDLG_RGB_R_INPUT) && IsWindowVisible( HWindow ) == TRUE
        )
	SetMix();
    }
 }*/

void TAssignDialog::WMHScroll( RTMessage Msg )
 {
   if( CallConnector( Msg, GetDlgCtrlID((HWND)HIWORD(Msg.LParam)) ) == TRUE ) Msg.Result = 0;
   else TCenterDialog::DefWndProc( Msg );
 }

void TAssignDialog::WMUserValueValidated( RTMessage msg )
 {
   CallConnector( msg, msg.WParam );
   msg.Result = 0;
 }

void TAssignDialog::WMCommand( RTMessage Msg )
 {
   switch( HIWORD(Msg.LParam) )
    {      
      case BN_CLICKED:
	switch( Msg.WParam )
	 {
	   case ID_MEDDLG_SHADING_FLAT_RADIO:
	   case ID_MEDDLG_SHADING_PHONG_RADIO:
	     rmSelected.saShadingAlho = SendDlgItemMessage( HWindow,
	       ID_MEDDLG_SHADING_FLAT_RADIO, BM_GETCHECK, 0, 0 ) ? SA_Flat:SA_Phong;
	     break;

	   /*case ID_MEDDLG_LIGHTGOOUT_CHECK:
	     rmSelected. = SendDlgItemMessage( HWindow,
	       ID_MEDDLG_LIGHTGOOUT_CHECK, BM_GETCHECK, 0, 0 );
	     break;*/
         };

	ButtonUpdate( Msg.WParam );
	break;

    /*  case EN_USER_VALUE_VALIDATED:
	UpdateTiledScrollbar( Msg.WParam );
        break;
      */
      case CBN_SELCHANGE:
        char cBuf[128]; 
        int res = SendDlgItemMessage( HWindow, ID_MEDDLG_MEDIUM_COMBO, CB_GETCURSEL,
          0, 0 );
        if( res >= 0 )
         {
	   SendDlgItemMessage( HWindow, ID_MEDDLG_MEDIUM_COMBO, CB_GETLBTEXT, res, (LPARAM)cBuf );
	   RTMedium rMedFind = (RTMedium)rdblLibrary.firstThat( FindMedInstance, Pvoid(cBuf) );

	   if( rMedFind != NOOBJECT )
            {
	      rmSelected = rMedFind;	      

	      InitEditors();
              SetName();
	      SetLightsGoOut( rmSelected.IsLightsGoOut() );

	      int iTst;
	      SendDlgItemMessage( HWindow, (iTst = rmSelected.saShadingAlho == SA_Flat) ?
		ID_MEDDLG_SHADING_FLAT_RADIO:ID_MEDDLG_SHADING_PHONG_RADIO,
		BM_SETCHECK, 1, 0 );
	      SendDlgItemMessage( HWindow, iTst ?
		ID_MEDDLG_SHADING_PHONG_RADIO:ID_MEDDLG_SHADING_FLAT_RADIO,
		BM_SETCHECK, 0, 0 );
	    }
	 }

	break;

    }

   TCenterDialog::WMCommand( Msg );
 }

/*void TAssignDialog::WMHScroll( RTMessage Msg )//WM_HSCROLL
 {  

   Msg.Result = 0;


   if( ltLock == TAssignDialog::LT_EditorsLock )
    {
      ltLock = TAssignDialog::LT_NoLocked;
      return;
    }  
   ltLock = TAssignDialog::LT_ScollbLock;


   int pos = GetScrollPos( (HWND)HIWORD(Msg.LParam), SB_CTL );


   if( Msg.WParam == SB_THUMBPOSITION ||
       Msg.WParam == SB_THUMBTRACK
     )
     pos = LOWORD( Msg.LParam );
   else
    {   
      //pos = GetScrollPos( (HWND)HIWORD(Msg.LParam), SB_CTL );
      int min, max;
      GetScrollRange( (HWND)HIWORD(Msg.LParam), SB_CTL, &min, &max );
      int widt = abs( min - max );
      int stepPg = 0.2 * double(widt);

      switch( Msg.WParam )
       {
         case SB_PAGEDOWN:
	   pos += stepPg;
	   break;

	 case SB_LINEDOWN:
	   pos++;
	   break;

	 case SB_PAGEUP:
	   pos -= stepPg;
	   break;

	 case SB_LINEUP:
	   pos--;
	   break;

	 case SB_TOP:
	   pos = min;
	   break;

	 case SB_BOTTOM:
	   pos = max;
	   break;
       }

      if( pos < min ) pos = min;
      else if( pos > max ) pos = max;
    }



   switch(  GetDlgCtrlID( (HWND)HIWORD(Msg.LParam) ) )
    {
      case ID_MEDDLG_KA_SCROLL:
	rmSelected.dKa = UnGetPos1( pos );
	Update( ID_MEDDLG_KA_INPUT );
	break;

      case ID_MEDDLG_KD_SCROLL:
	rmSelected.dKd = UnGetPos1( pos );
	Update( ID_MEDDLG_KD_INPUT );
	break;

      case ID_MEDDLG_KS_SCROLL:
	rmSelected.dKs = UnGetPos1( pos );
	Update( ID_MEDDLG_KS_INPUT );
	break;

      case ID_MEDDLG_KT_SCROLL:
	rmSelected.dKt = UnGetPos1( pos );
	Update( ID_MEDDLG_KT_INPUT );
	break;

      case ID_MEDDLG_REFR_SCROLL:
	rmSelected.dRefr = UnGetPos1( pos );
	Update( ID_MEDDLG_REFR_INPUT );
	break;

      case ID_MEDDLG_BETTA_SCROLL:
	rmSelected.dBetta = UnGetPos1( pos );
	Update( ID_MEDDLG_BETTA_INPUT );
	break;

      case ID_MEDDLG_RGB_R_SCROLL:
	rmSelected.rgbColor.rgbRed = pos;
	Update( ID_MEDDLG_RGB_R_INPUT );
	break;

      case ID_MEDDLG_RGB_G_SCROLL:
	rmSelected.rgbColor.rgbGreen = pos;
	Update( ID_MEDDLG_RGB_G_INPUT );
	break;

      case ID_MEDDLG_RGB_B_SCROLL:
	rmSelected.rgbColor.rgbBlue = pos;
	Update( ID_MEDDLG_RGB_B_INPUT );
	break;
    }

 }
  */


void TAssignDialog::ButtonUpdate( int id )
 {
   switch( id )
    {
      case ID_MEDDLG_LIGHTGOOUT_CHECK:
	SetLightsGoOut( SendMessage(GetDlgItem(HWindow, ID_MEDDLG_LIGHTGOOUT_CHECK), BM_GETCHECK, 0, 0) );
	break;

      case ID_MEDDLG_ADD_BTN:
	AddMedium();
        break;

      case ID_MEDDLG_DEL_BTN:
        DeleteMedium();
	break;
    }
 }

int FindMedInstance( const Object _FAR &rObj, void _FAR *lpName )
 {
   return strcmpi( lpName ? (LPSTR)lpName:"", RTMedium(rObj).lpName ) ? 0:1;
 }

void TAssignDialog::AddMedium()
 {
   char cBuf[128];

   if( GetName() ) return;

   GetDlgItemText( HWindow, ID_MEDDLG_MEDIUM_COMBO, cBuf, 127 );
   if( !strlen(cBuf) )
    {
      TAutoStringRc ttl_( hInstRC, ID_TITLE_ERROR );
      TAutoStringRc str_( hInstRC, ID_MSG_32 );
      BWCCMessageBox( HWindow, str_(),
       ttl_(), MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL );
      //"Need assign name",
      return;
    }

   RTMedium rMedFind = (RTMedium)rdblLibrary.firstThat( FindMedInstance, Pvoid(cBuf) );
   if( rMedFind != NOOBJECT )
    {
      char cTmp[128];
      TAutoStringRc str_( hInstRC, ID_MSG_33 );
      sprintf( cTmp, str_(), cBuf );
      //"Name [%s] already exist !  Owerwrite ?"
      TAutoStringRc ttl_( hInstRC, ID_TITLE_WARNING );
      int res = BWCCMessageBox( HWindow, cTmp,
       ttl_(), MB_ICONQUESTION | MB_YESNO | MB_APPLMODAL );

      if( res != IDYES ) return;

      rMedFind = rmSelected;
      return;
    }

    int res = SendDlgItemMessage( HWindow, ID_MEDDLG_MEDIUM_COMBO, CB_GETCURSEL, 0, 0 );

    PTMedium pMed = new TMedium();
    if( !pMed ) return;
    *pMed = rmSelected;
    rdblLibrary.add( *pMed );
    LoadNames();

    SendDlgItemMessage( HWindow, ID_MEDDLG_MEDIUM_COMBO, CB_SETCURSEL, ++res, 0 );
 }

void TAssignDialog::DeleteMedium()
 {
   char cBuf[128];

   if( GetName() ) return;

   int res = SendDlgItemMessage( HWindow, ID_MEDDLG_MEDIUM_COMBO, CB_GETCURSEL,
     0, 0 );

   if( res >= 0 )
    {
      SendDlgItemMessage( HWindow, ID_MEDDLG_MEDIUM_COMBO, CB_GETLBTEXT, res, (LPARAM)cBuf );
      RTMedium rMedFind = (RTMedium)rdblLibrary.firstThat( FindMedInstance, Pvoid(cBuf) );
      if( rMedFind == NOOBJECT ) return;

      if( res == rdblLibrary.getItemsInContainer() - 1 && res > 0 ) res--;
      rdblLibrary.detach( rMedFind, TShouldDelete::Delete );
      LoadNames();

      SendDlgItemMessage( HWindow, ID_MEDDLG_MEDIUM_COMBO, CB_SETCURSEL, res, 0 );
    }
 }


/*void TAssignDialog::DrawItem( DRAWITEMSTRUCT _FAR & DrawInfo )
 {
   if( DrawInfo.CtlID == ID_MEDDLG_RGB_MIX )
    {
      HPALETTE HpalOld = SelectPalette( DrawInfo.hDC, hpalPalette, FALSE );
      //RealizePalette( DrawInfo.hDC );

      PALETTEENTRY palEnt;

      palEnt.peRed = rmSelected.rgbColor.rgbRed;
      palEnt.peGreen = rmSelected.rgbColor.rgbGreen;
      palEnt.peBlue = rmSelected.rgbColor.rgbBlue;
      palEnt.peFlags = PC_RESERVED;

      AnimatePalette( hpalPalette, 235, 1, &palEnt );

      HBRUSH hbrDr = CreateSolidBrush( 
				       PALETTEINDEX(235)
				     );
      HBRUSH hbrOld = SelectBrush( DrawInfo.hDC, hbrDr );

      RECT r; GetClientRect( hwndMix, &r );
      Rectangle( DrawInfo.hDC, -1, -1, r.right + 1, r.bottom + 1 );

      SelectPalette( DrawInfo.hDC, HpalOld, FALSE );

      SelectObject( DrawInfo.hDC, hbrOld );
      DeleteObject( hbrDr );
    }
   else
     TCenterDialog::DrawItem( DrawInfo );
    
 }
  */


TAssignDialog::~TAssignDialog()
 {
   if( hpalPalette ) { DeleteObject(hpalPalette); hpalPalette = NULL; }

   for( int i = 0; i < MEDDLG_NUMB_CONTROL_BMP; i++ )
    {
      if( bbControlBmp[i].hbmN ) DeleteObject( bbControlBmp[i].hbmN );
      if( bbControlBmp[i].hbmH ) DeleteObject( bbControlBmp[i].hbmH );
      if( bbControlBmp[i].hbmS ) DeleteObject( bbControlBmp[i].hbmS );
    }
   _fsetmem( bbControlBmp, sizeof(TAssignDialog::TBtnBmp) * MEDDLG_NUMB_CONTROL_BMP, 0 );
 }


/*void TAssignDialog::SetMix()
 {
   if( !hpalPalette ) return;   

   if( IsWindowVisible(hwndMix) == TRUE )
     InvalidateRect( hwndMix, NULL, FALSE );
 }*/

int SelectFoundedInstance( const Object _FAR &rObj, void _FAR *pMed )
 {
   return ( RTMedium(rObj) == *PTMedium(pMed) );
 } 

void TAssignDialog::SelectFounded()
 {
   RTMedium rMed = (RTMedium)rdblLibrary.firstThat( SelectFoundedInstance, Pvoid(&rmSelected) );
   if( rMed == NOOBJECT ) return;

   SendDlgItemMessage( HWindow, ID_MEDDLG_MEDIUM_COMBO, CB_SELECTSTRING, 0, rMed.lpName ?
     (LPARAM)rMed.lpName:(LPARAM)"" );
 }


#pragma option -v.
