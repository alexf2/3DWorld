#if !defined(_SELECTDLG_)
#define _SELECTDLG_

#include <owl.h>
#include <bwcc.h>
#include "fdial2.h"

#include <fullscr.hpp>
#include <bmp.hpp>
#include <frame3d.hpp>
#include "autorc.hpp"
#include <indicat.hpp>
#include <centerdl.hpp>

#include "INC\dialog.inc"

LPSTR ShortName( LPSTR LpFullName, int iMaxLen );

_CLASSDEF(TMySelectDlg)
_CLASSDEF(T3DFrameSt)

class T3DFrameSt: public T3DFrame
 {
public:
  T3DFrameSt( PTWindowsObject AParent, int iIdFramed_, PTModule AModule = NULL ):
    T3DFrame( AParent, iIdFramed_, AModule )
   {
   }

  virtual int Dx() const { return -1; }
  //virtual int Dy() const { return 0; }
 };

class TMySelectDlg: public TFileDialog2
 {
public:
  TMySelectDlg( PTWindowsObject AParent, int ResourceId, LPSTR AFilePath, PTModule AModule = NULL );
  virtual ~TMySelectDlg();


  virtual void DrawItem( DRAWITEMSTRUCT _FAR & DrawInfo );
  virtual BOOL CanClose();
  virtual void HandleFName(RTMessage Msg) = [ID_FIRST + ID_FNAME];

  virtual void DefWndProc( RTMessage Msg );

  BOOL MakeLoad( BOOL bRes, LPSTR lpF );

  virtual void Destroy();

protected:
  virtual void SetupWindow();

  virtual LPSTR GetClassName();
  virtual void GetWindowClass( WNDCLASS & AWndClass );

  virtual void WMDropFile( RTMessage Msg ) = [WM_FIRST + WM_DROPFILES];

  void WMQueryNewPalette( RTMessage ) = [WM_FIRST + WM_QUERYNEWPALETTE];
  void WMPaletteChanged( RTMessage ) = [WM_FIRST + WM_PALETTECHANGED];

  virtual void WMCommand( RTMessage Msg ) = [WM_FIRST + WM_COMMAND];

private:
  struct TAuto3DStateBmp
   {
     TAutoGDIRc<HBITMAP> N, P, S;
   };

  TNoisyDta dNoisy;

  TAutoMemoryRc<PTDIB> mrcImg;
  TAutoStringRc srcMainPath;
  TAutoMemoryRc<PTDIB>  mrcTrueColor, mrc256;
  TAutoGDIRc<HBITMAP>   arcBmpStretch;
  TAutoGDIRc<HPALETTE>  arcPalBmpStretch;

  PT3DFrame pFr, pFrDir;
  int iBmpWidth, iBmpHigh;

  BOOL bFlSysPal;
  int iPalConvertSize;


  TAutoGDIRc<HBRUSH> arcHatchBrush;
  TAutoGDIRc<HBITMAP> arcAlexBmp;
  TAuto3DStateBmp a3dbBmp[ NUMB_BTNBMP ];

  HWND hwndBmpV;
  BOOL bFlNoLoad;
  double dPercent;

  void SetupNoisy();

  void CheckBtnState();
  void SetupBmpInfo();
  void ViewImage();
  void SaveImage();
  BOOL ConvertTrueColorTo256( PTIndicator );

  int GetMaxName() const;
  void DetectRadioState();

  void ShowAbout() const;
 };

_CLASSDEF(TPaletteDialog)

class TPaletteDialog: public TCenterDialog
 {
public:
  TPaletteDialog( PTWindowsObject AParent, LPSTR AName, BOOL& rTyp, int& rSize,
    PTModule AModule = NULL );    

  virtual ~TPaletteDialog();

protected:
  virtual void SetupWindow();

  virtual void WMCommand( RTMessage Msg ) = [WM_FIRST + WM_COMMAND];
  //void WMUserValueValidated( RTMessage ) = [WM_FIRST + WM_USER_VALUE_VALIDATED];

  virtual void Ok(RTMessage Msg) = [ID_FIRST + IDOK];


private:
  BOOL& rType;
  int&  rPalSize;

  BOOL bType;
  int  iPalSize; 

  void SetCtl();
  

  BOOL FindCbn( int ) const;
  void AddCbn( int );
  void SelectCbn( int );
  int GetCbn() const;
  int FindCbn( int iVal, int ) const;

 };

/*_CLASSDEF(TRoundButton)

class TRoundButton: public TButton
 {
public:
  TRoundButton( PTWindowsObject AParent, int AnId, LPSTR AText, int X, int Y,
		int W, int H, BOOL IsDefault, PTModule AModule = NULL ):
    TButton( AParent, AnId, AText, X, Y, W, H, IsDefault, AModule )
   {
   }

  TRoundButton( PTWindowsObject AParent, int ResourceId, PTModule AModule = NULL ):
    TButton( AParent, ResourceId, AModule )
   {
   }

protected:

 };*/

#endif
