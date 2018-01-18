#if !defined(_CENTER_DLG_)
#define _CENTER_DLG_

#include <owl.h>
#include <list.h>
#include <dialog.h>
#include <bbutton.h>
#include <BCHKBOX.H>
#include <SCROLLBA.H>
#include <BRADIO.H>
#include <COMBOBOX.H>
#include <edit.H>
#include "filedial.hpp"
#include "INC\classtyp.inc"
#include "autorc.hpp"
#include "autoctl.hpp"
#include "frame3d.hpp"

#undef _EXPORT2
#if defined(_CENTERDLDLL) || defined(_ALLDLGDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif

#undef _EXPFUNC2
#if defined(_CENTERDLDLL) || defined(_ALLDLGDLL)
  #define _EXPFUNC2 _export
#else
  #define _EXPFUNC2 _EXPFUNC
#endif


void _EXPFUNC2 CenterWindow( HWND hwnd );
//void CenterWindow( RTDialog rDlg );

_CLASSDEF(TCenterDialog)
_CLASSDEF(TErrorValidate)
_CLASSDEF(TValidator)
//_CLASSDEF(TCheckValidator<int>)
//_CLASSDEF(TCheckValidator<double>)
//_CLASSDEF(TIntValidator)
//_CLASSDEF(TDValidator)





class _EXPORT2 TErrorValidate
 {
public:
  TErrorValidate() { Clear(); lpMsg = NULL; }
  TErrorValidate( int iIdCtl_, LPCSTR lpMsg_ )
   {     
     Copy( iIdCtl_, lpMsg_ );
   }
  TErrorValidate( RTErrorValidate rObj )
   {
     Copy( rObj.iIdCtl, rObj.lpMsg );
   }

  ~TErrorValidate()
   {
     if( lpMsg ) { delete lpMsg; lpMsg = NULL; }
   }

  RTErrorValidate operator=( RTErrorValidate rObj )
   {
     if( lpMsg ) delete lpMsg;
     Copy( rObj.iIdCtl, rObj.lpMsg );
     return *this;
   }

  void Clear() { iIdCtl = 0; }
  int IdCtlError() const { return iIdCtl; }
  LPCSTR Msg() const { return lpMsg; }

  void SetOwnerDialog( HWND hwnd ) { hwndDlg = hwnd; }

  HWND CtlErrorHandle() const { return GetDlgItem(hwndDlg, iIdCtl); }

private:
  int iIdCtl;
  LPSTR lpMsg;
  HWND hwndDlg;

  void Copy( int iIdCtl_, LPCSTR lpMsg_ )
   {
     iIdCtl = iIdCtl_;
     lpMsg = lpMsg_ ?  _fstrdup( lpMsg_ ):NULL;
   }
 };



class  _EXPORT2 TCenterDialog: public TDialog
 {

friend class TBButtonAutoValidate;
friend class TBCheckBoxAutoValidate;
friend class TScrollBarAutoValidate;
friend class TBRadioButtonAutoValidate;
friend class TEditAutoValidate;
friend class TComboBoxAutoValidate;

public:
  enum TValidateResult { VR_Ok = 0, VR_Invalid = 1, VR_NoValidator = 2, VR_Reenter = 3 };

  TCenterDialog( PTWindowsObject AParent, LPSTR AName, PTModule AModule = NULL ):
    TDialog( AParent, AName, AModule )
   {
     bFlValidateMode = TRUE;
   };


  TCenterDialog( PTWindowsObject AParent, int ResourceId, PTModule AModule = NULL ):
    TDialog( AParent, ResourceId, AModule )
   {
     bFlValidateMode = TRUE;
   };

  virtual ~TCenterDialog();

  void SetValidateMode( BOOL bFl ) { bFlValidateMode = bFl; }
  BOOL GetValidateMode() const { return bFlValidateMode; }

  void SetFontGlobal( HFONT hf );
  void CenterCtlH( int iIdCtl );

  TValidateResult operator()( int iIdCtl, BOOL bFlAutoCorrect ); //set evErr
  virtual void CorrectError();
  RTCenterDialog operator,( RTValidator ); //add validator
  RTCenterDialog operator,( RTConnector );
  RTCenterDialog operator,( RT3DFrame );
  RTCenterDialog operator,( RT3DThickFrame );
  void RemoveAllConnectors() { lConnectors.flush( TShouldDelete::Delete ); }
  void RemoveAllValidators() { lValidators.flush( TShouldDelete::Delete ); }

  virtual BOOL CanClose();

  virtual classType isA() const { return classtypeCenterDlg; }

  int FocusedId() const { return GetDlgCtrlID( GetFocus() ); }
  BOOL HavError() const { return BOOL(evErr.IdCtlError() != 0); }

  //virtual void DefWndProc( RTMessage );
  BOOL CallConnector( RTMessage msg, int id );
  virtual void DrawItem( DRAWITEMSTRUCT _FAR & DrawInfo );

  virtual HPALETTE Palette() const { return NULL; }
  virtual HWND PaletteUser() const { return HWindow; }

protected:
  TErrorValidate  evErr;

  virtual void SetupWindow();
  void InitEditors();
  void Update( int id );
  void AllValidate(); 

  virtual void WMQueryNewPalette( RTMessage ) = [WM_FIRST + WM_QUERYNEWPALETTE];
  virtual void WMPaletteChanged( RTMessage ) = [WM_FIRST + WM_PALETTECHANGED];

private:
  List lValidators;
  List lConnectors;
  List l3DFrames;
  BOOL bFlValidateMode;
 };




class _EXPORT2 TValidator: public Object
 {

friend void TCenterDialog::Update( int id );

public:
  enum TValidateErrors { VE_NoCtl = -2, VE_Empty = -1, VE_Ok = 0,
			 VE_BadFormat = 1, VE_OutOfRange = 2 };

  TValidator( int id, LPSTR lpName ):
    asrName( lpName )
   {
     iIdConnectedCtl = id; hwndDlg = NULL;
     pAppl = NULL;
   }
  virtual ~TValidator() {};

  virtual classType isA() const { return classtypeValidator; }
  virtual hashValueType hashValue() const { return 0; }
  virtual int isEqual( const Object& rObj ) const
   {
     return (TValidator huge *)this == (TValidator huge *)&(RTValidator)rObj;
   }
  virtual char *nameOf() const { return "TValidator"; }
  virtual void printOn( ostream& ) const {};


  void InitControl( HWND hwnd, PTApplication pApp )
   {
     hwndDlg = hwnd; pAppl = pApp; SetData();
   }

  int GetId() const { return iIdConnectedCtl; }
  HWND GetHwndDlg() const { return hwndDlg; }
  TErrorValidate operator()( BOOL bFlAutoCorrect ); // TRUE - restores string

  virtual TValidator::TValidateErrors Check( LPSTR lpDta );

  LPCSTR Name() const;

protected:
  PTApplication pAppl;

  virtual TValidator::TValidateErrors CheckFormat( LPSTR lpDta ) = 0;
  virtual void GetData( LPSTR lpDta ) = 0;
  virtual void SetData();
  virtual TValidator::TValidateErrors CheckBounds( LPSTR ) = 0;
  virtual void AccertData() = 0;

  //virtual LPCSTR GetErrorMsg( TValidateErrors ) const;
  virtual void GetErrorMsg( TValidator::TValidateErrors veErr, LPSTR lpBuf, int iLen ) const;
private:
  int iIdConnectedCtl;
  HWND hwndDlg;
  TAutoStringRc asrName;

 };


template<class T> class _EXPORT2 TCheckValidator: public TValidator
 {
public:
  TCheckValidator( int id, LPSTR lpName, T tLowBound, T tHighBound, T& rtValidatedValue ):
    TValidator( id, lpName ),
    tLow( tLowBound ),
    tHigh( tHighBound ),
    rtValue( rtValidatedValue ),
    tTmpValue( rtValidatedValue )
   {
   }

  virtual TValidator::TValidateErrors CheckFormat( LPSTR lpDta );
  virtual void GetData( LPSTR lpDta );
  virtual void SetData();
  virtual TValidator::TValidateErrors CheckBounds( LPSTR );
  virtual void AccertData();

  T GetLow() const { return tLow; }
  T GetHigh() const { return tHigh; }

private:
  T& rtValue;
  T tTmpValue;

  T tLow, tHigh;
 };

TValidator::TValidateErrors _EXPFUNC2 CheckFormat( int, LPSTR );
TValidator::TValidateErrors _EXPFUNC2 CheckFormat( double, LPSTR );
TValidator::TValidateErrors _EXPFUNC2 CheckFormat( unsigned int, LPSTR );
TValidator::TValidateErrors _EXPFUNC2 CheckFormat( BYTE, LPSTR );


/*void GetData( int&, LPSTR );
void GetData( double&, LPSTR );
void GetData( unsigned int &, LPSTR );
void GetData( BYTE&, LPSTR );
void SetData( int&, LPSTR, int );
void SetData( double&, LPSTR, int );
void SetData( unsigned int &, LPSTR, int );
void SetData( BYTE&, LPSTR, int );*/


template<class T> TValidator::TValidateErrors TCheckValidator<T>::CheckFormat( LPSTR lpDta )
 {
   T tType;

   return ::CheckFormat( tType, lpDta );
 }

template<class T> void TCheckValidator<T>::GetData( LPSTR lpDta )
 {
   ::GetData( tTmpValue, lpDta );
 }

template<class T> void TCheckValidator<T>::SetData()
 {
   char cBuf[128];

   ::SetData( rtValue, cBuf, 127 );
   SetDlgItemText( GetHwndDlg(), GetId(), cBuf );

   TValidator::SetData();
 }

int _EXPFUNC2 CheckLimit( int, LPSTR );
int _EXPFUNC2 CheckLimit( BYTE, LPSTR );
int _EXPFUNC2 CheckLimit( double, LPSTR );
int _EXPFUNC2 CheckLimit( unsigned int, LPSTR );

template<class T> TValidator::TValidateErrors TCheckValidator<T>::CheckBounds( LPSTR lpStr )
 {
   T tTmp;
   if( !CheckLimit(tTmp, lpStr) && tTmpValue >= tLow && tTmpValue <= tHigh ) return VE_Ok;
   return VE_OutOfRange;
 }

template<class T> void TCheckValidator<T>::AccertData()
 {
   rtValue = tTmpValue;
 }

class _EXPORT2 TIntValidator: public TCheckValidator<int>
 {
public:
  TIntValidator( int id, LPSTR lpName, int tLowBound, int tHighBound, int& rtValidatedValue ):
    TCheckValidator<int>( id, lpName, tLowBound, tHighBound, rtValidatedValue )
   {
   }

protected:  
  virtual void GetErrorMsg( TValidator::TValidateErrors veErr, LPSTR lpBuf, int iLen ) const;

 };

class _EXPORT2 TDValidator: public TCheckValidator<double>
 {
public:
  TDValidator( int id, LPSTR lpName, double tLowBound, double tHighBound, double& rtValidatedValue ):
    TCheckValidator<double>( id, lpName, tLowBound, tHighBound, rtValidatedValue )
   {
   }

protected:
  virtual void GetErrorMsg( TValidator::TValidateErrors veErr, LPSTR lpBuf, int iLen ) const;


 };

class _EXPORT2 TUIntValidator: public TCheckValidator<unsigned int>
 {
public:
  TUIntValidator( int id, LPSTR lpName, unsigned int tLowBound, unsigned int tHighBound, unsigned int &rtValidatedValue ):
    TCheckValidator<unsigned int>( id, lpName, tLowBound, tHighBound, rtValidatedValue )
   {
   }

protected:  
  virtual void GetErrorMsg( TValidator::TValidateErrors veErr, LPSTR lpBuf, int iLen ) const;

 };

class _EXPORT2 TByteValidator: public TCheckValidator<BYTE>
 {
public:
  TByteValidator( int id, LPSTR lpName, BYTE tLowBound, BYTE tHighBound, BYTE& rtValidatedValue ):
    TCheckValidator<BYTE>( id, lpName, tLowBound, tHighBound, rtValidatedValue )
   {
   }

protected:  
  virtual void GetErrorMsg( TValidator::TValidateErrors veErr, LPSTR lpBuf, int iLen ) const;

 };


_CLASSDEF(TCenterFileDialog)

class _EXPORT2 TCenterFileDialog: public TFileDialog
 {
public:
  TCenterFileDialog( PTWindowsObject AParent, int ResourceId, PTMyOpenDialogTransfer pTransfer,
		     PInitDlg init, PTModule AModule = NULL):
    TFileDialog( AParent, ResourceId, pTransfer, init, AModule )

   {
   };

protected:
  virtual void SetupWindow();

 };

#endif

