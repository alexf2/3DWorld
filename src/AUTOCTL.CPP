//#pragma option -v

#include "autoctl.hpp"
#include "centerdl.hpp"
#include <stdio.h>

#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;

BOOL MakeValidate( RTControl rCtl, RTMessage Msg, int id = 0 )
 {
   if( rCtl.Parent && PTWindowsObject(rCtl.Parent)->isA() == classtypeCenterDlg )    
    {
      if( PTCenterDialog(rCtl.Parent)->HavError() == TRUE )
       {
	 Msg.Result = 1;
	 return FALSE;
       }

      if( (*PTCenterDialog(rCtl.Parent))( (id ? id:(PTCenterDialog(rCtl.Parent)->FocusedId())),
	PTCenterDialog(rCtl.Parent)->GetValidateMode() ) == TCenterDialog::VR_Invalid )
       {	 
	 Msg.Result = 1;
	 return FALSE;
       }
    }

   return TRUE;
 }

void TComboBoxAutoValidate::WMLButtonDown( RTMessage Msg )//WM_LBUTTONDOWN
 {
   if( ::MakeValidate( *this, Msg ) == FALSE )
    {
      PTCenterDialog(Parent)->CorrectError();
      return;
    }

   TComboBox::DefWndProc( Msg );
 }

void TComboBoxAutoValidate::WMSetFocus( RTMessage Msg )
 {
   if( Msg.Message == WM_SETFOCUS )
    {      
      if( ::MakeValidate( *this, Msg ) == FALSE )
       {
         PTCenterDialog(Parent)->CorrectError();
         return;
       }

    }

   TComboBox::DefWndProc( Msg );
 }



void TEditAutoValidate::DefWndProc( RTMessage Msg )
 {
   if( Msg.Message == WM_KEYUP && Msg.WParam == VK_RETURN )
    {
      if( ::MakeValidate( *this, Msg, GetId() ) == FALSE )
       {
         PTCenterDialog(Parent)->CorrectError();
         return;
       }

    }

   TEdit::DefWndProc( Msg );
 }

void TEditAutoValidate::WMLButtonDown( RTMessage Msg )
 {
   if( ::MakeValidate( *this, Msg, (GetFocus() == HWindow) ? GetId():0 ) == FALSE )
    {
      PTCenterDialog(Parent)->CorrectError();
      return;
    }

   TEdit::DefWndProc( Msg );
 }

void TEditAutoValidate::WMSetFocus( RTMessage Msg )
 {
   static char bFlReenter = 0;

   if( !bFlReenter && ::MakeValidate( *this, Msg ) == FALSE )
    {
      bFlReenter = 1;
      PTCenterDialog(Parent)->CorrectError();
      bFlReenter = 0;
      return;
    }
   TEdit::DefWndProc( Msg );
 }

void TEditAutoValidate::WMKillFocus( RTMessage Msg )
 {
   ::MakeValidate( *this, Msg, GetId() );
   TEdit::DefWndProc( Msg );
 }

void TScrollBarAutoValidate::WMLButtonDown( RTMessage Msg )
 {  
   if( ::MakeValidate( *this, Msg ) == FALSE )
    {
      PTCenterDialog(Parent)->CorrectError();
      return;
    }
   TScrollBar::DefWndProc( Msg );
 }

void TScrollBarAutoValidate::WMSetFocus( RTMessage Msg )
 {   
   if( ::MakeValidate( *this, Msg ) == FALSE )
    {
      PTCenterDialog(Parent)->CorrectError();
      return;
    }
   TScrollBar::DefWndProc( Msg );
 }


void TBRadioButtonAutoValidate::WMSetFocus( RTMessage Msg )
 {
   if( ::MakeValidate( *this, Msg ) == FALSE )
    {
      PTCenterDialog(Parent)->CorrectError();
      return;
    }
   TBRadioButton::DefWndProc( Msg );
 }


void TBButtonAutoValidate::WMSetFocus( RTMessage Msg )
 {   
   if( ::MakeValidate( *this, Msg ) == FALSE )
    {
      PTCenterDialog(Parent)->CorrectError();
      return;
    }
   TBButton::DefWndProc( Msg );
 }


void TBCheckBoxAutoValidate::WMSetFocus( RTMessage Msg )
 {   
   if( ::MakeValidate( *this, Msg ) == FALSE )
    {
      PTCenterDialog(Parent)->CorrectError();
      return;
    }
   TBCheckBox::DefWndProc( Msg );
 }

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


int TScrollBarAutoValidate::GetNewPos( RTMessage Msg )
 {
   Msg.Result = 0;

   int pos = GetScrollPos( HWindow, SB_CTL );


   if( Msg.WParam == SB_THUMBPOSITION ||
       Msg.WParam == SB_THUMBTRACK
     )
     pos = LOWORD( Msg.LParam );
   else
    {   
      //pos = GetScrollPos( (HWND)HIWORD(Msg.LParam), SB_CTL );
      int min, max;
      GetScrollRange( HWindow, SB_CTL, &min, &max );
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

   return pos;
 }

BOOL TConnector::MakeUpdate( RTMessage msg, int id )
 {
   if( msg.Message == iMsg1 && id == pObj1->GetId() ||
       msg.Message == iMsg2 && id == pObj2->GetId()
     )
    {
      if( msg.Message == iMsg1 )
       {
	 ltLock = TConnector::LT_1Lock;
	 (pObj1->*pUpdate1)( msg );
       }
      else
       {
	 if( ltLock == TConnector::LT_1Lock )
          {
            ltLock = TConnector::LT_NoLocked;
	    return TRUE;
          }
	 ltLock = TConnector::LT_2Lock;
	 (pObj2->*pUpdate2)( msg );
       }

      return TRUE;
    }

   return FALSE;
 }

void TMixerConnected::Setup()
 {
   HDC hdc = GetDC( HWindow );
   PAINTSTRUCT ps; _fmemset( &ps, sizeof(ps), 0 );
   Paint( hdc, ps );
   ReleaseDC( HWindow, hdc );
 }

void TMixerConnected::WM_UserOwnerPaintRequiest( RTMessage msg )
 {
   PAINTSTRUCT ps; _fsetmem( &ps, sizeof(ps), 0 );
   Paint( ((DRAWITEMSTRUCT*)msg.LParam)->hDC, ps );
   msg.Result = 0;
 }


void TMixerConnected::Paint( HDC PaintDC, PAINTSTRUCT _FAR & )
 {
   HPALETTE HpalOld = SelectPalette( PaintDC, hpal, FALSE );

   PALETTEENTRY palEnt;

   palEnt.peRed = rRGB.rgbRed;
   palEnt.peGreen = rRGB.rgbGreen;
   palEnt.peBlue = rRGB.rgbBlue;
   palEnt.peFlags = PC_RESERVED;

   AnimatePalette( hpal, indReserved, 1, &palEnt );

   HBRUSH hbrDr = CreateSolidBrush( PALETTEINDEX(indReserved) );

   HBRUSH hbrOld = SelectBrush( PaintDC, hbrDr );

   RECT r; GetClientRect( HWindow, &r );
   Rectangle( PaintDC, -1, -1, r.right + 1, r.bottom + 1 );

   SelectPalette( PaintDC, HpalOld, FALSE );

   SelectObject( PaintDC, hbrOld );
   DeleteObject( hbrDr );
 }




void TAVScrollConnectedMixer::Setup()
 {
   if( !Parent ) return;

   int pos = rDta * dPrecision;

   if( GetScrollPos(HWindow, SB_CTL) != pos )
    {
      SetScrollPos( HWindow, SB_CTL, pos, IsWindowEnabled(HWindow) );
      pMix->Setup();
    }
 }
