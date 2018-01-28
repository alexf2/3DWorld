#if !defined(_IMAGESETUP_DLG_)
#define _IMAGESETUP_DLG_

#undef _EXPORT2
#if defined(_ISDLGDLL) || defined(_ALLDLGDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif

#include <owl.h>
#include <dialog.h>
#include <button.h>
#include <bgrpbox.h>

#include "centerdl.hpp"
#include "rgb.hpp"
#include "INC\isdlg.inc"

_CLASSDEF(TImageOptDialog)

enum TImageFormat {IF_Windowed = 1, IF_FullScreen, IF_640x480, IF_800x600,
                   IF_1024x768 };

//enum TNoisy { N_Marginal, N_Uniform, N_None };
struct TBtnValueTbl
 {
   int iIdBtn;
   int iValueVariable;
 };
const int iNumbBtn = 10;
const int iNumbBtnAlho = 6;

class _EXPORT2 TImageOptDialog: public TCenterDialog
 {
public:

  TImageOptDialog( PTWindowsObject AParent, LPSTR AName, int& iPalSize_,
		   TImageFormat& ifFormat_, TConvertAlho&  caAlhoPalette_,
		   TNoisy& nNoisy, int& iMarginalAmplitude,
		   int& iUniformAmplitude, int& iUniformThreshold,
		   PTModule AModule = NULL );

  virtual ~TImageOptDialog() {};
  
protected:
  virtual void SetupWindow();
  virtual void WMCommand( RTMessage ) = [WM_FIRST + WM_COMMAND];
  virtual void Ok( RTMessage ) = [ID_FIRST + IDOK];

private:
  static TBtnValueTbl bvtTbl[ iNumbBtn ];
  static int iAlhoBtnTbl[ iNumbBtnAlho ];

  int& iPalSize;
  TImageFormat& ifFormat;
  TConvertAlho&  caAlhoPalette;

  TNoisy& nNoisy;
  int& iMarginalAmplitude;
  int& iUniformAmplitude;
  int& iUniformThreshold;

  int Btn( int iValue ) const;
  int Value( int iBtn ) const;
  void DetectRadioState();

  void CheckNoisy();
  void SetupNoisy();
 };


#endif
