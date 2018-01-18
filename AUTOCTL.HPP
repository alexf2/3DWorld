#if !defined(_AUTO_VALIDATE_CTL_)
#define _AUTO_VALIDATE_CTL_

#include <owl.h>
//#include <list.h>
//#include <dialog.h>
#include <bbutton.h>
#include <BCHKBOX.H>
#include <SCROLLBA.H>
#include <BRADIO.H>
#include <COMBOBOX.H>
#include <edit.H>
#include <object.h>
#include <bstatbmp.h>
#include "INC\classtyp.inc"
//#include "autorc.hpp"

#undef _EXPORT2
#if defined(_AUTOCTLDLL) || defined(_ALLDLGDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif

#undef _EXPFUNC2
#if defined(_AUTOCTLDLL) || defined(_ALLDLGDLL)
  #define _EXPFUNC2 _export
#else
  #define _EXPFUNC2 _EXPFUNC
#endif


_CLASSDEF(TBButtonAutoValidate)
_CLASSDEF(TBCheckBoxAutoValidate)
_CLASSDEF(TScrollBarAutoValidate)
_CLASSDEF(TBRadioButtonAutoValidate)
_CLASSDEF(TEditAutoValidate)
_CLASSDEF(TComboBoxAutoValidate)
//_CLASSDEF(TAVEditConnected)
//_CLASSDEF(TAVScrollConnected<BYTE>)
_CLASSDEF(TAVScrollConnectedMixer)
_CLASSDEF(TMixerConnected)

#define WM_USER_VALUE_VALIDATED (WM_USER + 1)
#define WM_USER_OWNERPAINTREQVEST (WM_USER + 2)

void _EXPFUNC2 GetData( int&, LPSTR );
void _EXPFUNC2 GetData( double&, LPSTR );
void _EXPFUNC2 GetData( unsigned int &, LPSTR );
void _EXPFUNC2 GetData( BYTE&, LPSTR );
void _EXPFUNC2 SetData( int&, LPSTR, int );
void _EXPFUNC2 SetData( double&, LPSTR, int );
void _EXPFUNC2 SetData( unsigned int &, LPSTR, int );
void _EXPFUNC2 SetData( BYTE&, LPSTR, int );


_CLASSDEF(TConnector)

typedef void (TWindowsObject::* PTUpdateFunc)( RTMessage msg );
typedef void (TWindowsObject::* PTSetupFunc)();

class _EXPORT2 TConnector: public Object
 {
public:
  TConnector( PTWindowsObject pObj1_, PTWindowsObject pObj2_,
	      PTUpdateFunc pUpFunc1, PTUpdateFunc pUpFunc2,
	      int iMsg1_, int iMsg2_
	    ):
    ltLock( TConnector::LT_NoLocked )
   {
     pObj1 = pObj1_;
     pObj2 = pObj2_;
     pUpdate1 = pUpFunc1;
     pUpdate2 = pUpFunc2;
     iMsg1 = iMsg1_;
     iMsg2 = iMsg2_;
   }

  virtual classType isA() const { return classtypeTConnector; }
  virtual hashValueType hashValue() const { return 0; }
  virtual int isEqual( const Object& rObj ) const
   {
     return (TConnector huge *)this == (TConnector huge *)&(RTConnector)rObj;
   }
  virtual char *nameOf() const { return "TConnector"; }
  virtual void printOn( ostream& ) const {};

  BOOL operator()( RTMessage msg, int id ) { return MakeUpdate(msg, id ); }

  BOOL MakeUpdate( RTMessage msg, int id );

private:
  enum TLockTiled { LT_NoLocked, LT_1Lock, LT_2Lock };

  PTWindowsObject pObj1, pObj2;
  PTUpdateFunc  pUpdate1, pUpdate2;
  int iMsg1, iMsg2;

  TLockTiled ltLock;
 };


class _EXPORT2 TComboBoxAutoValidate: public TComboBox
 {
public:
  TComboBoxAutoValidate( PTWindowsObject AParent, int AnId, int X, int Y,
			 int W, int  H, DWORD AStyle, WORD ATextLen,
			 PTModule ATModule = NULL ):
    TComboBox( AParent, AnId, X, Y, W, H, AStyle, ATextLen, ATModule )
     {
     }

  TComboBoxAutoValidate( PTWindowsObject AParent, int ResourceId,
			 WORD ATextLen, PTModule ATModule = NULL ):
    TComboBox( AParent, ResourceId, ATextLen, ATModule )
     {
     }


  virtual void WMLButtonDown( RTMessage Msg ) = [WM_FIRST + WM_LBUTTONDOWN];

protected:
  virtual void WMSetFocus( RTMessage Msg ) = [WM_FIRST + WM_SETFOCUS];

 };


class _EXPORT2 TEditAutoValidate: public TEdit
 {
public:
  TEditAutoValidate( PTWindowsObject AParent, int AnId, LPSTR AText, int X,
		     int Y, int W, int H, WORD ATextLen, BOOL Multiline ):
    TEdit( AParent, AnId, AText, X, Y, W, H, ATextLen, Multiline )
     {
     }

  TEditAutoValidate( PTWindowsObject AParent, int ResourceId, WORD ATextLen,
		     PTModule ATModule = NULL ):
    TEdit( AParent, ResourceId, ATextLen, ATModule )
     {
     }

  virtual void DefWndProc( RTMessage Msg );
  virtual void WMLButtonDown( RTMessage Msg ) = [WM_FIRST + WM_LBUTTONDOWN];

protected:
  virtual void WMSetFocus( RTMessage Msg ) = [WM_FIRST + WM_SETFOCUS];
  virtual void WMKillFocus( RTMessage Msg ) = [WM_FIRST + WM_KILLFOCUS];

 };

template<class T> class _EXPORT2 TAVEditConnected: public TEditAutoValidate
 {
public:
  TAVEditConnected( PTWindowsObject AParent, int AnId, LPSTR AText, int X,
		    int Y, int W, int H, WORD ATextLen, BOOL Multiline,
		    PTSetupFunc pSetup_, T& rDta_ ):
    TEditAutoValidate( AParent, AnId, AText, X, Y, W, H, ATextLen, Multiline ),
    rDta( rDta_ )
     {
       pTile = NULL; pSetup = pSetup_;
       bFlHaveValidator = FALSE;
     }

  TAVEditConnected( PTWindowsObject AParent, int ResourceId, WORD ATextLen,
		    PTSetupFunc pSetup_, T& rDta_,
		    PTModule ATModule = NULL ):
    TEditAutoValidate( AParent, ResourceId, ATextLen, ATModule ),
    rDta( rDta_ )
   {
     pTile = NULL; pSetup = pSetup_;
     bFlHaveValidator = FALSE;
   }

  void Connect( PTWindowsObject pObj ) { pTile = pObj; }

  void SetValidatorMode( BOOL bFl ) { bFlHaveValidator = bFl; }  
  void Update( RTMessage msg );
  void Setup();

private:
  PTWindowsObject pTile;
  PTSetupFunc pSetup;
  T& rDta;
  BOOL bFlHaveValidator;
 };

template<class T> void TAVEditConnected<T>::Update( RTMessage msg )
 {
   msg.Result = 0;
   if( !Parent ) return;

   if( bFlHaveValidator == FALSE )
    {
      char cBuf[256] = "1";
      GetDlgItemText( Parent->HWindow, GetId(), cBuf, 255 );
      ::GetData( rDta, cBuf );
    }
   if( pTile ) (pTile->*pSetup)();
 }

template<class T> void TAVEditConnected<T>::Setup()
 {
   if( !Parent ) return;

   char cBuf[128];

   ::SetData( rDta, cBuf, 127 );
   SetDlgItemText( Parent->HWindow, GetId(), cBuf );
   SendMessage( Parent->HWindow, WM_USER_VALUE_VALIDATED, GetId(), 0 );
 }



class _EXPORT2 TBRadioButtonAutoValidate: public TBRadioButton
 {
public:
  TBRadioButtonAutoValidate( PTWindowsObject AParent, int AnId, LPSTR ATitle,
			     int X, int Y, int W, int H, PTGroupBox AGroup,
			     PTModule AModule = NULL ):
    TBRadioButton( AParent, AnId, ATitle, X, Y, W, H, AGroup, AModule )
     {
     }

  TBRadioButtonAutoValidate( PTWindowsObject AParent, int ResourceId,
			     PTGroupBox AGroup, PTModule AModule = NULL ):
    TBRadioButton( AParent, ResourceId, AGroup, AModule )
     {
     }

protected:
  virtual void WMSetFocus( RTMessage Msg ) = [WM_FIRST + WM_SETFOCUS];

 };


class _EXPORT2 TScrollBarAutoValidate: public TScrollBar
 {
public:
  TScrollBarAutoValidate( PTWindowsObject AParent, int AnId, int X, int Y,
			  int W, int H, BOOL IsHScrollBar, PTModule AModule = NULL ):
    TScrollBar( AParent, AnId, X, Y, W, H, IsHScrollBar, AModule )
     {
     }

  TScrollBarAutoValidate( PTWindowsObject AParent, int ResourceId, PTModule AModule = NULL ):
    TScrollBar( AParent, ResourceId, AModule )
     {
     }

  virtual void WMLButtonDown( RTMessage Msg ) = [WM_FIRST + WM_LBUTTONDOWN];

  int GetNewPos( RTMessage msg );

protected:
  virtual void WMSetFocus( RTMessage Msg ) = [WM_FIRST + WM_SETFOCUS];

 };

template<class T> class _EXPORT2 TAVScrollConnected: public TScrollBarAutoValidate
 {

friend class TAVScrollConnectedMixer;

public:
  TAVScrollConnected( PTWindowsObject AParent, int AnId, int X, int Y,
		      int W, int H, BOOL IsHScrollBar,
		      PTSetupFunc pSetup_, T& rDta_, double dPrecision_,
		      PTModule AModule = NULL ):
    TScrollBarAutoValidate( AParent, AnId, X, Y, W, H, IsHScrollBar, AModule ),
    rDta( rDta_ )
     {
       dPrecision = dPrecision_;
       pTile = NULL; pSetup = pSetup_;
     }

  TAVScrollConnected( PTWindowsObject AParent, int ResourceId,
		    PTSetupFunc pSetup_, T& rDta_, double dPrecision_,
		    PTModule AModule = NULL ):		    
    TScrollBarAutoValidate( AParent, ResourceId, AModule ),
    rDta( rDta_ )
   {
     dPrecision = dPrecision_;
     pTile = NULL; pSetup = pSetup_;
   }

  void Connect( PTWindowsObject pObj ) { pTile = pObj; }

  void Update( RTMessage msg );
  void Setup();

private:
  PTWindowsObject pTile;
  PTSetupFunc pSetup;
  T& rDta;
  double dPrecision;
 };

template<class T> void TAVScrollConnected<T>::Update( RTMessage msg )
 {
   if( !Parent ) return;

   rDta = double(GetNewPos( msg )) / (dPrecision > 0.01 ? dPrecision:1);
   if( pTile ) (pTile->*pSetup)();
 }

template<class T> void TAVScrollConnected<T>::Setup()
 {
   if( !Parent ) return;

   int pos = rDta * dPrecision;

   if( GetScrollPos(HWindow, SB_CTL) != pos )
     SetScrollPos( HWindow, SB_CTL, pos, IsWindowEnabled(HWindow) );
 }


class _EXPORT2 TAVScrollConnectedMixer: public TAVScrollConnected<BYTE>
 {
public:
  TAVScrollConnectedMixer( PTWindowsObject AParent, int AnId, int X, int Y,
		      int W, int H, BOOL IsHScrollBar,
		      PTSetupFunc pSetup_, BYTE& rDta_, double dPrecision_,
		      PTMixerConnected pMix_,
		      PTModule AModule = NULL ):
    TAVScrollConnected<BYTE>( AParent, AnId, X, Y, W, H, IsHScrollBar,
		              pSetup_, rDta_, dPrecision_, AModule )
     {
       pMix = pMix_;
     }

  TAVScrollConnectedMixer( PTWindowsObject AParent, int ResourceId,
		    PTSetupFunc pSetup_, BYTE& rDta_, double dPrecision_,
		    PTMixerConnected pMix_,
		    PTModule AModule = NULL ):		    
    TAVScrollConnected<BYTE>( AParent, ResourceId, pSetup_, rDta_, dPrecision_,
		              AModule )
   {
     pMix = pMix_;
   }

  void Setup();


private:
  PTMixerConnected pMix;

};

class _EXPORT2 TMixerConnected: public TBStaticBmp
 {
public:
  TMixerConnected( PTWindowsObject AParent, int AnId, LPSTR ATitle, int X, int Y,
	  int W, int H,
          RGBQUAD& rDta_, HPALETTE hpal_, int indReserved_,
	  PTModule AModule = NULL ):
    TBStaticBmp( AParent, AnId, ATitle, X, Y, W, H, AModule ),
    rRGB( rDta_ )
     {
       hpal = hpal_;
       indReserved = indReserved_;
     }


  TMixerConnected( PTWindowsObject AParent, int ResourceId,
		   RGBQUAD& rDta_, HPALETTE hpal_, int indReserved_,
		   PTModule AModule = NULL ):
    TBStaticBmp( AParent, ResourceId, AModule ),
    rRGB( rDta_ )
     {
       hpal = hpal_;
       indReserved = indReserved_;
     }

  void Update( RTMessage ) {};
  void Setup();
  void SetPal( HPALETTE hpal_ ) { hpal = hpal_; }

  virtual void Paint( HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo );

protected:
  virtual void WM_UserOwnerPaintRequiest( RTMessage ) = [WM_FIRST + WM_USER_OWNERPAINTREQVEST];

private:
  RGBQUAD& rRGB;
  HPALETTE hpal;
  int indReserved;
 };


class _EXPORT2 TBCheckBoxAutoValidate: public TBCheckBox
 {
public:
  TBCheckBoxAutoValidate( PTWindowsObject AParent, int AnId, LPSTR ATitle,
			  int X, int Y, int W, int H, PTGroupBox AGroup,
			  PTModule ATModule = NULL ):
    TBCheckBox( AParent, AnId, ATitle, X, Y, W, H, AGroup, ATModule )
     {
     }

  TBCheckBoxAutoValidate( PTWindowsObject AParent, int ResourceId,
			  PTGroupBox AGroup, PTModule ATModule = NULL ):
    TBCheckBox( AParent, ResourceId, AGroup, ATModule )
     {
     }

protected:
  virtual void WMSetFocus( RTMessage Msg ) = [WM_FIRST + WM_SETFOCUS];

 };


class _EXPORT2 TBButtonAutoValidate: public TBButton
 {
public:
  TBButtonAutoValidate( PTWindowsObject AParent, int AnId, LPSTR AText,
			int X, int Y, int W, int H, BOOL IsDefault,
			PTModule AModule = NULL ):
    TBButton( AParent, AnId, AText, X, Y, W, H, IsDefault, AModule )
     {
     }

  TBButtonAutoValidate( PTWindowsObject AParent, int ResourceId,
		       PTModule AModule = NULL ):
    TBButton( AParent, ResourceId, AModule )
     {
     }
  

protected:
  virtual void WMSetFocus( RTMessage Msg ) = [WM_FIRST + WM_SETFOCUS];

 };


#endif
