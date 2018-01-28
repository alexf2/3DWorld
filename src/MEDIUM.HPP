#if !defined(_MEDIUM_)
#define _MEDIUM_

#include <owl.h>
#include <dbllist.h>

#include "autorc.hpp"
#include "centerdl.hpp"
#include "phtransf.hpp"
#include "INC\classtyp.inc"
#include "INC\medbmp.inc"

#undef _EXPORT2
#if defined(_MEDIUMDLL) || defined(_ALLDLGDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif



_CLASSDEF(TMedium)
_CLASSDEF(TAssignDialog)

static int FindMedInstance( const Object _FAR &rObj, void _FAR *lpName );
static void LoadNamesInstance( Object _FAR &rObj, void _FAR *hwnd );


class _EXPORT2 TMedium: public Object
 {

friend class TAssignDialog;
friend int FindMedInstance( const Object _FAR &rObj, void _FAR *lpName );
friend void LoadNamesInstance( Object _FAR &rObj, void _FAR *hwnd );

public:

  TMedium();
  TMedium( RTMedium );
  virtual ~TMedium();

  RTMedium operator=( RTMedium );
  int operator==( RTMedium );

  virtual classType isA() const { return classtypeMediumItem; }
  virtual hashValueType hashValue() const { return 0; }
  virtual int isEqual( const Object& ) const;
  virtual char *nameOf() const { return "Medium_Item"; }
  virtual void printOn( ostream& ) const {};

  BOOL IsLightsGoOut() const
   {
     return BOOL(!(dBetta == 0 && dKt == 0));
   }

  void ResetName();
  LPSTR GetName() const { return lpName; }
  //void AssName( LPSTR lpN ) { lpName = lpN; }

  int Save( HFILE hf );
  int Load( HFILE hf );

  void AssignColorIndex( BYTE ind ) { rgbColor.rgbReserved = ind; }
  BYTE GetColorIndex() const { return rgbColor.rgbReserved; }
  RGBQUAD _FAR & GetRGBColor() const { return (RGBQUAD)rgbColor; }

  TShadingAlho Shading() const { return saShadingAlho; }
  void Shading( TShadingAlho sa ) { saShadingAlho = sa; }

  double GdKa() const { return dKa; }
  double GdKd() const { return dKd; }
  double GdKs() const { return dKs; }
  double GdKt() const { return dKt; }

  double GdRefr() const { return dRefr; }
  double GdBetta() const { return dBetta; }

  int GPhong() const { return iPhong; }

  BOOL IsLineTransp() const { return !bNoLineTransp; }

private:
  double  dKa,   //diffusion refract
	  dKd,   //diffusion refract of lights source
	  dKs,   //
	  dKt;
  BOOL bNoLineTransp;
  RGBQUAD rgbColor;
  unsigned int iPhong;

  double  dRefr,   //min Kt
	  dBetta;  //max Kt

  LPSTR   lpName;

  TShadingAlho saShadingAlho;

  void Copy( RTMedium );
 };


/*class _EXPORT TCenterDialog: public TDialog
 {
public:
  TCenterDialog( PTWindowsObject AParent, LPSTR AName, PTModule AModule = NULL ):
*/

const int iNumbScrolls_AssDlg = 9;

class _EXPORT2 TAssignDialog: public TCenterDialog
 {
public:
  TAssignDialog( PTWindowsObject AParent, LPSTR AName, RTMedium rmSel,
    DoubleList _FAR & rdblLib, PTModule AModule = NULL );

  virtual ~TAssignDialog();

  virtual HPALETTE Palette() const { return hpalPalette; }
  virtual HWND PaletteUser() const { return hwndMix; }

protected:
  virtual void Ok( RTMessage Msg ) = [ID_FIRST + IDOK];
  virtual void SetupWindow();

  virtual void WMCommand( RTMessage Msg ) = [WM_FIRST + WM_COMMAND];
  virtual void WMHScroll( RTMessage Msg ) = [WM_FIRST + WM_HSCROLL];
  void WMUserValueValidated( RTMessage ) = [WM_FIRST + WM_USER_VALUE_VALIDATED];
 

private:  

  struct TScrollRange
   {
     int id;
     double left, right;
   };
  static TScrollRange srTblRange[ iNumbScrolls_AssDlg ];

  typedef struct {
    HBITMAP hbmN, hbmH, hbmS;
   } TBtnBmp;

  TBtnBmp bbControlBmp[ MEDDLG_NUMB_CONTROL_BMP ];


  DoubleList _FAR &rdblLibrary;
  RTMedium rmSelected;
  HWND hwndMix;
  HPALETTE hpalPalette;

  PTMixerConnected pMix;

  //TLockTiled ltLock;

  void SelectFounded();

  void SetName();
  void SetLightsGoOut( BOOL );
  //void SetMix();
  int GetName();

  static int GetPos1( double );
  double UnGetPos1( int ) const;


  //virtual void ErrorInput( RCTErrorValidate ) const;
  virtual void CorrectError();

  void LoadNames();

  //int EditStrUpdate( int id );
  void ButtonUpdate( int id );

  void AddMedium();
  void DeleteMedium();
  //void UpdateTiledScrollbar( int id );
 };


#endif
