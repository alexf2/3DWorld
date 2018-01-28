//#pragma option -v

#include "centerdl.hpp"
#include "auto.hpp"
#include "2darray.hpp"
#include <bwcc.h>
#include <stdio.h>

#include "autorc.hpp"
#include "INC\io_em.inc"

#include "limit.h"
   
#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif


extern HINSTANCE hInstRC;

void _EXPFUNC2 CenterWindow( HWND hwnd )
 {
   RECT WinRect;
   POINT PtLeftUp;
   int  W, H;  


   GetWindowRect( hwnd, &WinRect );
   W = WinRect.right - WinRect.left; H = WinRect.bottom - WinRect.top;

   WinRect.left = (GetSystemMetrics(SM_CXSCREEN) - W) / 2; 
   WinRect.top = (GetSystemMetrics(SM_CYSCREEN) - H) / 2;

   MoveWindow( hwnd, WinRect.left, WinRect.top, W, H, FALSE );
 }

/*void CenterWindow( RTDialog rDlg )//TDialog
 {
   rDlg.Attr.X = (GetSystemMetrics(SM_CXSCREEN) - rDlg.Attr.W) / 2;
   rDlg.Attr.Y = (GetSystemMetrics(SM_CYSCREEN) - rDlg.Attr.H) / 2;
 }*/

void SetFontGlobalInstance( Pvoid Child, Pvoid font )
 {
   PTWindowsObject pCh = (PTWindowsObject)Child;

   if( pCh->Parent )
     SendDlgItemMessage( pCh->Parent->HWindow, pCh->GetId(), WM_SETFONT, (WPARAM)font, FALSE );
 }

TCenterDialog::~TCenterDialog()
 {
   l3DFrames.flush( TShouldDelete::NoDelete );
 }

void TCenterDialog::SetFontGlobal( HFONT hf )//TWindowsObject
 {
   ForEach( SetFontGlobalInstance, (Pvoid)hf );
 }

void TCenterDialog::CenterCtlH( int iIdCtl )
 {
   HWND hwnd = GetDlgItem( HWindow, iIdCtl );
   RECT r; GetWindowRect( hwnd, &r );

   POINT p; p.x = r.left; p.y = r.top;
   ScreenToClient( HWindow, &p );
   r.left = p.x; r.top = p.y;

   p.x = r.right; p.y = r.bottom;
   ScreenToClient( HWindow, &p );
   r.right = p.x; r.bottom = p.y;

   RECT rW; GetClientRect( HWindow, &rW );
   int widt = r.right - r.left;
   int left = (rW.right - widt) / 2;

   MoveWindow( hwnd, left, r.top, widt, r.bottom - r.top, TRUE );
 }

static void InitFrame( Object _FAR &rObj, void _FAR * )
 {
   RT3DFrame(rObj).InitSize();
 }

void TCenterDialog::SetupWindow()
 {
   TDialog::SetupWindow();
   CenterWindow( HWindow );
   l3DFrames.forEach( InitFrame, 0 );
 }

void TCenterDialog::WMPaletteChanged( RTMessage Msg )
 {
   if( (HWND)(Msg.WParam) == HWindow )
    {
      Msg.Result = 0;
      return;
    }
   else WMQueryNewPalette( Msg );
 }


void TCenterDialog::WMQueryNewPalette( RTMessage Msg )
 {
   HPALETTE HpalOld;
   int i;

   if( !Palette() ) { Msg.Result = 0; return; }

   HDC hdc = GetDC( PaletteUser() );

   HpalOld = SelectPalette( hdc, Palette(), FALSE );
   i = RealizePalette( hdc );
   SelectPalette( hdc, HpalOld, FALSE);

   ReleaseDC( PaletteUser(), hdc );

   if (i > 0)
     InvalidateRect( PaletteUser(), NULL, FALSE );


   Msg.Result = i;
 }



void TCenterFileDialog::SetupWindow()
 {
   TFileDialog::SetupWindow();
   CenterWindow( HWindow );
 }


struct TAllValDta
 {
   BOOL bFl;
   RTErrorValidate rEV;

   TAllValDta( BOOL bFl_, RTErrorValidate rEV_):
     rEV( rEV_ )
    {
      bFl = bFl_;
    }
 };
typedef TAllValDta _FAR *PTAllValDta;

int AllValidateInstance( const Object _FAR &rObj, void _FAR *pD )
 {                    
   PTAllValDta(pD)->rEV = ((RTValidator)rObj)( PTAllValDta(pD)->bFl );

   return PTAllValDta(pD)->rEV.IdCtlError() ? 1:0;
   //TErrorValidate operator()( BOOL bFlAutoCorrect );
 }

void TCenterDialog::AllValidate()
 {
   TAllValDta avd( bFlValidateMode, evErr );
   lValidators.firstThat( AllValidateInstance, Pvoid(&avd) );
 }

BOOL TCenterDialog::CanClose()
 {
   if( !evErr.IdCtlError() ) AllValidate();

   if( evErr.IdCtlError() )
    {
      CorrectError();
      return FALSE;
    }
   else return TRUE;
 }


static int FindValidatorInstance( const Object _FAR &rObj, void _FAR *id )
 {
   return  RTValidator(rObj).GetId() == (int)id;
 }

TCenterDialog::TValidateResult TCenterDialog::operator()( int iIdCtl, BOOL bFlAutoCorrect = TRUE )
 {
   static BOOL bFlActive = FALSE;

   if( bFlActive == TRUE ) return VR_Reenter;
   bFlActive = TRUE;

   //lValidators
   RTValidator rValidat = (RTValidator)lValidators.firstThat( FindValidatorInstance, Pvoid(iIdCtl) );
   if( rValidat == NOOBJECT )
    {
      bFlActive = FALSE;
      return VR_NoValidator;
    }

   evErr = rValidat( bFlAutoCorrect );
   int key = evErr.IdCtlError();
   //if(  key  ) CorrectError();

   bFlActive = FALSE;
   return key ? VR_Invalid:VR_Ok;				
 }

void TCenterDialog::CorrectError()
 {
   if( evErr.IdCtlError() && evErr.Msg() )
    {
      int idErrCtl = evErr.IdCtlError();
      evErr.Clear();

      TAutoStringRc ttl_( hInstRC, ID_TITLE_ERROR );
      BWCCMessageBox( HWindow, evErr.Msg(),
	ttl_(), MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL );

      PostMessage( HWindow, WM_NEXTDLGCTL, (WPARAM)GetDlgItem(HWindow, idErrCtl), TRUE );
    }
 }

struct TConData
 {
   RTMessage rMsg;
   int id;
   TConData( RTMessage rMsg_, int id_ ):
     rMsg( rMsg_ )
    {
      id = id_;
    }
 };
typedef TConData* PTConData;
//_PTRDEF(TConData)


static int CallConnectorInstance( const Object _FAR &rObj, void _FAR *pCD )
 {
   return RTConnector(rObj)(PTConData(pCD)->rMsg, PTConData(pCD)->id) == TRUE;
 }

BOOL TCenterDialog::CallConnector( RTMessage msg, int id )
 {
   //if( !HWindow || IsWindowVisible(HWindow) == FALSE ) return FALSE;

   TConData cd( msg, id );
   RTConnector rCon = (RTConnector)lConnectors.firstThat( CallConnectorInstance, Pvoid(&cd) );

   if( rCon != NOOBJECT ) return TRUE;
   return FALSE;
 }

/*void TCenterDialog::DefWndProc( RTMessage msg )
 {

   //int iMsg, int id );

   TDialog::DefWndProc( msg );   
 }*/

void TCenterDialog::DrawItem( DRAWITEMSTRUCT _FAR & DrawInfo )
 {
   PTWindowsObject pCh = ChildWithId( DrawInfo.CtlID );

   if( pCh )
     SendMessage( PTWindow(pCh)->HWindow, WM_USER_OWNERPAINTREQVEST,
       pCh->GetId(), (LPARAM)&DrawInfo );

 }

RTCenterDialog TCenterDialog::operator,( RT3DFrame rFr )
 {
   l3DFrames.add( rFr );

   return *this;
 }

RTCenterDialog TCenterDialog::operator,( RTConnector rCon )
 {
   lConnectors.add( rCon );

   return *this;
 }

RTCenterDialog TCenterDialog::operator,( RTValidator rValidat ) //add validator
 {
   lValidators.add( rValidat );

   return *this;
 }

RTCenterDialog TCenterDialog::operator,( RT3DThickFrame rFr ) 
 {
   l3DFrames.add( rFr );

   return *this;
 }


struct TInitCtl
 {
   PTApplication pApp;
   HWND hwnd;

   TInitCtl( PTApplication pApp_, HWND hwnd_ )
    {
      pApp = pApp_; hwnd = hwnd_;
    }
 };
typedef TInitCtl* PTInitCtl;
//_PTRDEF(TInitCtl)

static void SetValidatorsOwnerInstance( Object _FAR &rObj, void _FAR * pInit )
 {
   RTValidator(rObj).InitControl( PTInitCtl(pInit)->hwnd, PTInitCtl(pInit)->pApp );
 }

void TCenterDialog::InitEditors()
 {
   TInitCtl ic( GetApplication(), HWindow );
   lValidators.forEach( SetValidatorsOwnerInstance, Pvoid(&ic) );
 }

static int UpdateInstance( const Object _FAR &rObj, void _FAR *pId )
 {
   return RTValidator(rObj).GetId() == *(int*)pId;
 }

void TCenterDialog::Update( int id )
 {
   RTValidator rV = (RTValidator)lValidators.firstThat( UpdateInstance, Pvoid(&id) );
   if( rV != NOOBJECT ) rV.SetData();
 }


LPCSTR TValidator::Name() const
 {
   return asrName() ? asrName():"No name";
 }

TValidator::TValidateErrors TValidator::Check( LPSTR lpDta )
  {
   TValidator::TValidateErrors res2 = CheckFormat( lpDta );				
   if( res2 != VE_Ok ) return res2;

   GetData( lpDta );

   res2 = CheckBounds( lpDta );
   if( res2 != VE_Ok ) return res2;

   return VE_Ok;
  }

TErrorValidate TValidator::operator()( BOOL bFlAutoCorrect )
 {
   char cBuf[256] = "1";

   int res = GetDlgItemText( hwndDlg, iIdConnectedCtl, cBuf, 255 );
   if( !res && lstrlen(cBuf) == 1 && *cBuf == '1' )
    {
      GetErrorMsg( VE_NoCtl, cBuf, 255 );
      return TErrorValidate( iIdConnectedCtl, cBuf );
    }

   TValidator::TValidateErrors res2 = Check( cBuf );
   if( res2 != VE_Ok )
    {
      if( bFlAutoCorrect == TRUE ) SetData();
      GetErrorMsg( res2, cBuf, 255 );
      return TErrorValidate( iIdConnectedCtl, cBuf );
    }

   AccertData();
   SetData();

   TErrorValidate evRet;
   if( res ) evRet = TErrorValidate( 0, "" );
   else
    {
      GetErrorMsg( VE_Empty, cBuf, 255 );
      evRet = TErrorValidate( 0, cBuf );
    }

   return evRet;
 }

void TValidator::GetErrorMsg( TValidator::TValidateErrors veErr, LPSTR lpBuf, int iLen ) const
 {
   int id = 0;

   switch( veErr )
    {
      case VE_NoCtl:
	//lpM = "Invalid control";
	id = ID_MSG_84;
	break;

      case VE_Empty:
	//lpM = "Emty input";
	id = ID_MSG_85;
	break;

      case VE_BadFormat:
	//lpM = "Invalid data format";
	id = ID_MSG_86;
	break;

      case VE_OutOfRange:
	//lpM = "Value out of range";
	id = ID_MSG_87;
	break;

      default:
	//lpM = "";
	id = ID_MSG_88;
        break;
    };

   TAutoStringRc s1_;
   if( id )
    {
      s1_ = TAutoStringRc( hInstRC, id );
      if( veErr == VE_OutOfRange || veErr == VE_BadFormat )
	sprintf( lpBuf, s1_(), Name() );
      else
        _fstrncpy( lpBuf, s1_(), iLen );
    }
   else *lpBuf = 0;
 }



TValidator::TValidateErrors CheckFormat( unsigned int, LPSTR lpStr )
 {
   const int iNumbClass = 4;
   const int iNumbState = 9;
   //int ctUIntClass[1][1];
   //int jtUIntTbl[1][1];
   /*static TClassOfChar ctUIntClass[iNumbClass] =
    {      
      {48, 57},//0..9
      {32, 32},//space
      {0, 0},
      {0, 255}//invalid
    };*/
  static char bFlInit = 0;
  static T2DArray<int> ctUIntClass( 2, iNumbClass );
  static T2DArray<int> jtUIntTbl( iNumbState, iNumbClass );
  if( !bFlInit )
   {
     bFlInit = 1;

     ctUIntClass,
       48, 57, TLineDef(0),
       32, 32, TLineDef(0),
       0, 0,   TLineDef(0),
       0, 255, TLineDef(0);

     jtUIntTbl,
       0, 1, 7, 3, 4, 7, 7, 7, 4, TLineDef(0),
       0, 1, 5, 3, 4, 5, 4, 8, 8, TLineDef(0),
       0, 1, 0, 3, 4, 4, 4, 3, 3, TLineDef(0),
       0, 1, 1, 3, 4, 1, 1, 1, 1, TLineDef(0);
   }
  
   /*static int jtUIntTbl[iNumbClass][iNumbState] =
    {      
      {0, 1, 7, 3, 7, 7, 7, 4, 4},//0..9
      {0, 1, 5, 3, 5, 4, 8, 8, 4},//space
      {0, 1, 0, 3, 4, 4, 3, 3, 4},//0
      {0, 1, 1, 3, 1, 1, 1, 1, 4}//0..255
    };*/

   TAutomat aAuto( ctUIntClass(), jtUIntTbl(), iNumbClass, iNumbState );

   LPSTR lpTmp = _fstrdup( lpStr );
   if( !lpTmp ) return TValidator::VE_BadFormat;
   AnsiToOem( lpStr, lpTmp );

   TValidator::TValidateErrors res = (aAuto.Work(lpStr) != TAutomat::A_End) ? TValidator::VE_BadFormat:
	  TValidator::VE_Ok;

   delete lpTmp;
   return res;
 }


TValidator::TValidateErrors CheckFormat( BYTE, LPSTR lpStr )
 {
   unsigned int tmp;
   return CheckFormat( tmp, lpStr );
 }


TValidator::TValidateErrors CheckFormat( int, LPSTR lpStr )
 {
   const int iNumbClass = 6;
   const int iNumbState = 9;
   //int ctIntClass[1][1];
   //int jtIntTbl[1][1];
  static char bFlInit = 0;
  static T2DArray<int> ctIntClass( 2, iNumbClass );
  static T2DArray<int> jtIntTbl( iNumbState, iNumbClass );
  if( !bFlInit )
   {
     bFlInit = 1;

     ctIntClass,
       43, 43, TLineDef(0),
       45, 45, TLineDef(0),
       48, 57, TLineDef(0),
       32, 32, TLineDef(0),
       0, 0,   TLineDef(0),
       0, 255, TLineDef(0);

     jtIntTbl,
       0, 1, 6, 3, 4, 6, 4, 4, 4,  TLineDef(0),
       0, 1, 6, 3, 4, 6, 4, 4, 4,  TLineDef(0),
       0, 1, 7, 3, 4, 7, 7, 7, 4,  TLineDef(0);
     jtIntTbl,
       0, 1, 5, 3, 4, 5, 4, 8, 8,  TLineDef(0),
       0, 1, 0, 3, 4, 4, 4, 3, 3,  TLineDef(0),
       0, 1, 1, 3, 4, 1, 1, 1, 1, TLineDef(0);
   }

   /*static TClassOfChar ctIntClass[iNumbClass]=
    {
      {43, 43},//+
      {45, 45},//-
      {48, 57},//0..9
      {32, 32},//space
      {0, 0},
      {0, 255}//invalid
    };*/
 
   /*static int jtIntTbl[iNumbClass][iNumbState] =
    {
      {0, 1, 6, 3, 6, 4, 4, 4, 4},//-
      {0, 1, 6, 3, 6, 4, 4, 4, 4},//+
      {0, 1, 7, 3, 7, 7, 7, 4, 4},//0..9
      {0, 1, 5, 3, 5, 4, 8, 8, 4},//space
      {0, 1, 0, 3, 4, 4, 3, 3, 4},//0
      {0, 1, 1, 3, 1, 1, 1, 1, 4}//0..255
    };*/

   TAutomat aAuto( ctIntClass(), jtIntTbl(), iNumbClass, iNumbState );

   LPSTR lpTmp = _fstrdup( lpStr );
   if( !lpTmp ) return TValidator::VE_BadFormat;
   AnsiToOem( lpStr, lpTmp );

   TValidator::TValidateErrors res = (aAuto.Work(lpStr) != TAutomat::A_End) ? TValidator::VE_BadFormat:
	  TValidator::VE_Ok;

   delete lpTmp;
   return res;
 }



TValidator::TValidateErrors CheckFormat( double, LPSTR lpStr )
 {
   const int iNumbClass = 9;
   const int iNumbState = 14;
   //int ctDClass[1][1];
   //int jtDTbl[1][1];
   static char bFlInit = 0;
   static T2DArray<int> ctDClass( 2, iNumbClass );
   static T2DArray<int> jtDTbl( iNumbState, iNumbClass );
   if( !bFlInit )
    {
      bFlInit = 1;

      ctDClass,
        43, 43, TLineDef(0),
        45, 45, TLineDef(0),
        48, 57, TLineDef(0),
	46, 46, TLineDef(0),
        32, 32, TLineDef(0),
        69, 69, TLineDef(0),
        101, 101, TLineDef(0),
        0, 0,     TLineDef(0),
	0, 255, TLineDef(0);

      jtDTbl,     
	0, 1, 6, 3, 4, 6, 4, 4, 4, 4, 11, 4, 4, 4,  TLineDef(0),
	0, 1, 6, 3, 4, 6, 4, 4, 4, 4, 11, 4, 4, 4,  TLineDef(0);
      jtDTbl,
	0, 1, 7, 3, 4, 7, 7, 7, 9, 9, 12, 12, 12, 4,TLineDef(0),
	0, 1, 4, 3, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4,   TLineDef(0);
      jtDTbl,
	0, 1, 5, 3, 4, 5,  4, 4, 4, 4, 4, 4, 13, 13,TLineDef(0),
	0, 1, 4, 3, 4, 4, 4, 10, 4, 10, 4, 4, 4, 4, TLineDef(0);
      jtDTbl,
	0, 1, 4, 3, 4, 4, 4, 10, 4, 10, 4, 4, 4, 4, TLineDef(0),
	0, 1, 0, 3, 4, 4, 4, 3, 4, 3, 4, 4, 3, 3,   TLineDef(0);
      jtDTbl,
	0, 1, 1, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, TLineDef(0);

    }
   /*static TClassOfChar ctDClass[iNumbClass]=
    {
      {43, 43},//+
      {45, 45},//-
      {48, 57},//0..9
      {46, 46},//.
      {32, 32},//space
      {69, 69},//E
      {101, 101},//e
      {0, 0},
      {0, 255}//invalid
    };*/
   /*static int jtDTbl[iNumbClass][iNumbState] =
    {
      {0, 1, 6, 3, 4, 6, 4, 4, 4, 4, 11, 4, 4, 4},//+
      {0, 1, 6, 3, 4, 6, 4, 4, 4, 4, 11, 4, 4, 4},//-
      {0, 1, 7, 3, 4, 7, 7, 7, 9, 9, 12, 12, 12, 4},//0..9
      {0, 1, 4, 3, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4},//.
      {0, 1, 5, 3, 4, 5,  4, 4, 4, 4, 4, 4, 13, 13},//space
      {0, 1, 4, 3, 4, 4, 4, 10, 4, 10, 4, 4, 4, 4 },//E
      {0, 1, 4, 3, 4, 4, 4, 10, 4, 10, 4, 4, 4, 4 },//e
      {0, 1, 0, 3, 4, 4, 4, 3, 4, 3, 4, 4, 3, 3},//0
      {0, 1, 1, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4}//0..255
    };*/

   TAutomat aAuto( ctDClass(), jtDTbl(), iNumbClass, iNumbState );

   LPSTR lpTmp = _fstrdup( lpStr );
   if( !lpTmp ) return TValidator::VE_BadFormat;
   AnsiToOem( lpStr, lpTmp );

   TValidator::TValidateErrors res = (aAuto.Work(lpStr) != TAutomat::A_End) ? TValidator::VE_BadFormat:
	  TValidator::VE_Ok;

   delete lpTmp;
   return res;
 }
/*
void GetData( int& rInt, LPSTR lpStr )
 {
   sscanf( lpStr, "%d", &rInt );
 }

void GetData( double& rD, LPSTR lpStr )
 {
   float fTmp;
   sscanf( lpStr, "%f", &fTmp );
   rD = fTmp;
 }

void GetData( unsigned int &rInt, LPSTR lpStr )
 {
   sscanf( lpStr, "%u", &rInt );
 }

void GetData( BYTE& rInt, LPSTR lpStr )
 {
   unsigned int uiTmp;
   sscanf( lpStr, "%u", &uiTmp );
   rInt = uiTmp;
 }


void SetData( int& iVal, LPSTR lpStr, int len )
 {
   sprintf( lpStr, "%d", iVal );
 }

void SetData( double& dVal, LPSTR lpStr, int len )
 {
   float fTmp = dVal;
   sprintf( lpStr, "%1.3f", fTmp );
 }

void SetData( unsigned int &uiVal, LPSTR lpStr, int len )
 {
   sprintf( lpStr, "%u", uiVal );
 }

void SetData( BYTE& bVal, LPSTR lpStr, int len )
 {
   unsigned int uiTmp = bVal;
   sprintf( lpStr, "%u", uiTmp );
 }

*/
void TIntValidator::GetErrorMsg( TValidator::TValidateErrors veErr, LPSTR lpBuf, int iLen ) const
 {
   if( veErr == VE_OutOfRange )
    {
      TAutoStringRc s1_( hInstRC, ID_MSG_80 );
      sprintf( lpBuf, s1_()/*"Value [%s] out of range. Need  %d <= value <= %d"*/,
        Name(),
	TCheckValidator<int>::GetLow(), TCheckValidator<int>::GetHigh() );
      return;
    }
   else TCheckValidator<int>::GetErrorMsg( veErr, lpBuf, iLen );
 }

void TDValidator::GetErrorMsg( TValidator::TValidateErrors veErr, LPSTR lpBuf, int iLen ) const
 {
   if( veErr == VE_OutOfRange )
    {
      TAutoStringRc s1_( hInstRC, ID_MSG_81 );
      sprintf( lpBuf, s1_()/*"Value [%s] out of range. Need  %1.3f <= value <= %1.3f"*/,
        Name(),
	float(TCheckValidator<double>::GetLow()), float(TCheckValidator<double>::GetHigh()) );
      return;
    }
   else TCheckValidator<double>::GetErrorMsg( veErr, lpBuf, iLen );
 }


void TUIntValidator::GetErrorMsg( TValidator::TValidateErrors veErr, LPSTR lpBuf, int iLen ) const
 {
   if( veErr == VE_OutOfRange )
    {
      TAutoStringRc s1_( hInstRC, ID_MSG_82 );
      sprintf( lpBuf, s1_()/*"Value [%s] out of range. Need  %u <= value <= %u"*/,
	Name(),
	TCheckValidator<unsigned int>::GetLow(), TCheckValidator<unsigned int>::GetHigh() );
      return;
    }
   else TCheckValidator<unsigned int>::GetErrorMsg( veErr, lpBuf, iLen );
 }
     
void TByteValidator::GetErrorMsg( TValidator::TValidateErrors veErr, LPSTR lpBuf, int iLen ) const
 {
   if( veErr == VE_OutOfRange )
    {
      TAutoStringRc s1_( hInstRC, ID_MSG_83 );
      sprintf( lpBuf, s1_()/*"Value [%s] out of range. Need  %u <= value <= %u"*/,
         Name(),
	(unsigned int)TCheckValidator<BYTE>::GetLow(), (unsigned int)TCheckValidator<BYTE>::GetHigh() );
      return;
    }
   else TCheckValidator<BYTE>::GetErrorMsg( veErr, lpBuf, iLen );
 }


void TValidator::SetData()
 {
   if( hwndDlg )
     //SendMessage( hwndDlg, WM_COMMAND, GetId(), MAKELONG(NULL, EN_USER_VALUE_VALIDATED) );
     SendMessage( hwndDlg, WM_USER_VALUE_VALIDATED, GetId(), 0 );
 }



int CheckLimit( int, LPSTR lpStr )
 {
   return LimitInt( lpStr );
 }

int CheckLimit( BYTE, LPSTR lpStr  )
 {
   return LimitByte( lpStr );
 }

int CheckLimit( double, LPSTR  )
 {
   return 0;
 }

int CheckLimit( unsigned int, LPSTR lpStr )
 {
   return LimitUInt( lpStr );
 }


