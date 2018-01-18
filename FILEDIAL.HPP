// ObjectWindows - (C) Copyright 1992 by Borland International
//Modified by AlexCorp. 1995

#ifndef __FILEDIAL_H
#define __FILEDIAL_H

#ifndef __DIALOG_H
#include <dialog.h>
#endif

#ifndef __DIR_H
#include <dir.h>
#endif

#pragma option -Vo-
#if     defined(__BCOPT__) && !defined(_ALLOW_po)
#pragma option -po-
#endif

#define FILESPEC	MAXPATH + MAXEXT

//#include "centerdl.hpp"
#include <combobox.h>
#include <listbox.h>
#include <bstatic.h>
#include <bbutton.h>
#include <bgrpbox.h>
#include <bdivider.h>
#include <windowsx.h>

#include "INC\dlg.inc"
#include "measur.mac"

#undef _EXPORT2
#if defined(_FILEDIALDLL) || defined(_ALLDLGDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif

#undef _EXPFUNC2
#if defined(_FILEDIALDLL) || defined(_ALLDLGDLL)
  #define _EXPFUNC2 _export
#else
  #define _EXPFUNC2 _EXPFUNC
#endif


const int CBN_US_STATE = WM_USER + 50;

struct _EXPORT2 TMyOpenDialogTransfer
 {
   TMyOpenDialogTransfer();
   ~TMyOpenDialogTransfer();
   void MakeInit();
   LPSTR GetSelect();

   PTComboBoxData pComboBoxInput;
   PTComboBoxData pComboBoxType;
   char chPath[MAXPATH];

   char chFilePath[FILESPEC];
   BOOL bFlCheckName;
 };

typedef TMyOpenDialogTransfer *PTMyOpenDialogTransfer;



_CLASSDEF(TFileDialog)
typedef void (*PInitDlg)(PTFileDialog);

  /* TFileDialog */
class _EXPORT2 TFileDialog : public TDialog
{
public:
    LPSTR FilePath;
    char PathName[MAXPATH];
    char Extension[MAXEXT];
    char FileSpec[FILESPEC];

    TFileDialog(PTWindowsObject AParent, int ResourceId, PTMyOpenDialogTransfer pTransfer,
		PInitDlg init, PTModule AModule = NULL);
    virtual ~TFileDialog();


    virtual BOOL CanClose();
    void SelectFileName();
    void UpdateFileName();
    BOOL UpdateListBoxes();

  virtual void ShutDownWindow();
  virtual void TransferData( WORD Direction );

    static PTStreamable build();

  static void Init( PTFileDialog pDlg );

protected:
    virtual void SetupWindow();
    virtual void HandleFName(RTMessage Msg)
          = [ID_FIRST + ID_FNAME];
    virtual void HandleFList(RTMessage Msg)
          = [ID_FIRST + ID_FLIST];
    virtual void HandleDList(RTMessage Msg)
	  = [ID_FIRST + ID_DLIST];

  void HandleFType( RTMessage Msg ) = [ID_FIRST + ID_FTYPE];

    TFileDialog(StreamableInit) : TDialog(streamableInit) {};
  virtual void GetWindowClass( WNDCLASS FAR & AWndClass );

private:
    virtual const Pchar streamableName() const
	{ return "TFileDialog"; }

  BOOL bFlCustom;
  void KeyInput();

  HFONT hfLists;
  BOOL bFlGlobalLockHandle;

};

inline Ripstream operator >> ( Ripstream is, RTFileDialog cl )
    { return is >> (RTStreamable )cl; }
inline Ripstream operator >> ( Ripstream is, RPTFileDialog cl )
    { return is >> (RPvoid)cl; }

inline Ropstream operator << ( Ropstream os, RTFileDialog cl )
    { return os << (RTStreamable )cl; }
inline Ropstream operator << ( Ropstream os, PTFileDialog cl )
    { return os << (PTStreamable )cl; }


#pragma option -Vo.
#if     defined(__BCOPT__) && !defined(_ALLOW_po)
#pragma option -po.
#endif

#endif
