#if !defined(_TBKGNDDLG_)
#define _TBKGNDDLG_

#include <owl.h>
#include <dialog.h>
#include <button.h>
#include <bgrpbox.h>
#include <list.h>

#include "centerdl.hpp"
#include "bmp.hpp"
#include "rgb.hpp"
#include "autoctl.hpp"
#include "autorc.hpp"
#include "bkgnd.hpp"

#include "INC\bkgnddlg.inc"

#undef _EXPORT2
#if defined(_BKGNDDLGDLL) || defined(_ALLDLGDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif

_CLASSDEF(TBkGndDialog)
_CLASSDEF(TGradientConnected)


class _EXPORT2 TGradientConnected: public TMixerConnected
 {
public:
  TGradientConnected( PTWindowsObject AParent, int AnId, LPSTR ATitle, int X, int Y,
                      int W, int H,		      
		      TAutoMemoryRc<PTDIB>&  grcBmp_,
                      PTModule AModule = NULL ):

    TMixerConnected( AParent, AnId, ATitle, X, Y, W, H, rgb0, NULL, 235, AModule ),    
    grcBmp( grcBmp_ )
   {
   }


  TGradientConnected( PTWindowsObject AParent, int ResourceId,
		      TAutoMemoryRc<PTDIB>&  grcBmp_,
		      PTModule AModule = NULL ):

    TMixerConnected( AParent, ResourceId, rgb0, NULL, 235, AModule ),    
    grcBmp( grcBmp_ )
   {
   }

  virtual void Paint( HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo );

private:
  TAutoMemoryRc<PTDIB>&  grcBmp;

  RGBQUAD rgb0;
 };

const int iNumbGradient = 4;
const int iNumbScrolls = 22;
const int iNumbOvlItems = 96;
const int iNumbFontUsers = 5;
const int iNumbAVButtons = 13;

class _EXPORT2 TBkGndDialog: public TCenterDialog
 {
public:
  TBkGndDialog( PTWindowsObject pOwn, LPSTR lpSrcName,
		TAutoMemoryRc<PTBackGraund>& rmrcBkGnd_, PTModule pModule );

  virtual HPALETTE Palette() const;
  virtual HWND PaletteUser() const { return hwndMix; }

  virtual ~TBkGndDialog();

protected:
  virtual void SetupWindow();

  virtual void WMCommand( RTMessage Msg ) = [WM_FIRST + WM_COMMAND];

  virtual void WMHScroll( RTMessage Msg ) = [WM_FIRST + WM_HSCROLL];
  void WMUserValueValidated( RTMessage ) = [WM_FIRST + WM_USER_VALUE_VALIDATED];

  virtual void Ok( RTMessage ) = [ID_FIRST + IDOK];

private:
  struct TIndexGradientTbl
   {
     classType classGradient;
     int iIndex;
   };

  struct TScrollRange
   {
     int id;
     double left, right;
   };

  struct TOvlItem
   {
     int id;
     classType clItem;
   };

  struct TBitmapContain
   {
     TAutoGDIRc<HBITMAP> grcN, grcS, grcH;
   };


  _CLASSDEF(TCtlData)
  friend class TCtlData;
  class TCtlData: public Object
   {
  //friend class TBkGndDialog;
  public:
    int iIdCtl;
    HWND hwnd;
    classType cl;

    TCtlData() { hwnd = NULL; iIdCtl = -1; cl = 0; }
    TCtlData( int id, HWND hwnd_, classType cl_ )
      {
	iIdCtl = id; hwnd = hwnd_; cl = cl_;
      }

    ~TCtlData();

    virtual classType isA() const { return 1; }
    virtual hashValueType hashValue() const { return 0; }
    virtual int isEqual( const Object& rObj ) const
     {
       return &(TBkGndDialog::RTCtlData)rObj == this;
     }
    virtual char *nameOf() const { return "TCTL_DATA"; }
    virtual void printOn( ostream& ) const {};
   };


  TBitmapContain grcBmp[ BKGND_NUMBBMP ];

  static TIndexGradientTbl igtTbl[ iNumbGradient ];
  static TIndexGradientTbl igtTblRadio[ iNumbGradient ];
  static TScrollRange srTblRange[ iNumbScrolls ];
  static TOvlItem oiTblOvlItems[ iNumbOvlItems ];
  static int iTblFontUsers[ iNumbFontUsers ];
  static int iTblAVButtons[ iNumbAVButtons ];
  List laCtl[ iNumbGradient ];
  PTBackGraund bkgndItems[ iNumbGradient ];
  PTBackGraund bkgndItemsKey[ iNumbGradient ];
  int iIndexSelected;
   
  TAutoMemoryRc<PTBackGraund>& rmrcBkGnd;
  TAutoMemoryRc<PTDIB>  mrcGradientBmp;
  TAutoMemoryRc<PTColorTable>  mrcPalette;
  TAutoMemoryHugeRc<TRGB huge*> mrcDrawBuf;

  int iWid, iHigh;

  BOOL bFlOk;

  //PTAVScrollConnectedMixer psbRGBr, psbRGBg, psbRGBb;
  HWND hwndMix;

  PTGradientConnected pMix;

  int IndexOf( RTBackGraund ) const;
  int IndexOf( classType ) const;
  classType ClassOf( int ) const;
  int ClassOfRadio( int ) const;
  int IdOfClass( int ) const;

  void DeleteOvlCtl();
  void CreateOvlCtl();
  void RefreshBmp( BOOL bFlNoCheck );

  RTBkGndDialog operator+( RTCtlData );
 };



#endif
