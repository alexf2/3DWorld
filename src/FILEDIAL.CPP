//#pragma option -v-




// ObjectWindows - (C) Copyright 1992 by Borland International
//Modified by AlexCorp. 1995

/* --------------------------------------------------------
  FILEDIAL.CPP
  Defines type TFileDialog.  This defines the basic
  behavior of all file dialogs.
  -------------------------------------------------------- */

#include "filedial.hpp"
#include <string.h>
#include <dir.h>

#include <windows.h>
#include <bwcc.h>

#ifdef __DLL__
  extern PTModule pModule;
#else
  static PTModule pModule = NULL;
#endif

extern HINSTANCE hInstRC;

void TFileDialog::GetWindowClass( WNDCLASS FAR & AWndClass )
 {
   TDialog::GetWindowClass( AWndClass );

   AWndClass.style |= CS_SAVEBITS;
 }


TMyOpenDialogTransfer::TMyOpenDialogTransfer()
 {
   pComboBoxInput = new TComboBoxData();
   pComboBoxType  = new TComboBoxData();
   *chPath = 0;
   _fstrcpy( chFilePath, "*.*" );
   bFlCheckName = TRUE;
 }


TMyOpenDialogTransfer::~TMyOpenDialogTransfer()//TFileDialog
 {
   if( pComboBoxInput ) delete pComboBoxInput; pComboBoxInput = NULL;
   if( pComboBoxType ) delete pComboBoxType; pComboBoxType = NULL;  
 }

LPSTR TMyOpenDialogTransfer::GetSelect()
 {
   return chFilePath;
 }

void TMyOpenDialogTransfer::MakeInit()
 {
   char chBuf[MAXPATH+MAXEXT + 1];
   char chDrive[MAXDRIVE], chDir[MAXDIR], chName[MAXFILE],
        chExt[MAXEXT];


   _fstrcpy( chBuf, chPath );
   if( chBuf[_fstrlen(chBuf) - 1] != '\\' ) _fstrcat( chBuf, "\\" );
   _splitpath( (String&)((*(pComboBoxInput->Strings))[0]), chDrive, chDir,
               chName, chExt );
   _makepath( chFilePath, "", "", chName, chExt );
   _fstrcat( chBuf, chFilePath );
   _fstrcpy( chFilePath, chBuf );

 }


LPSTR GetFileName(LPSTR FilePath)
{
  LPSTR P;

  P = _fstrrchr(FilePath, '\\');
  if ( !P )
    P = _fstrrchr(FilePath, ':');
  if ( !P )
    return FilePath;
  else
    return P + 1;
}

LPSTR GetExtension(LPSTR FilePath)
{
  LPSTR P;

  P = _fstrchr( GetFileName(FilePath), '.');
  if ( !P )
    return _fstrchr(FilePath, '\0');
  else
    return P;
}

BOOL HasWildCards(LPSTR FilePath)
{
  return _fstrchr(FilePath, '\*') || _fstrchr(FilePath, '\?');
}

/* Constructor for a TFileDialog. Uses the contents of AFilePath as
   the initial mask (for example, "*.*") for the files to be listed
   in the file list box. AFilePath is also used as a buffer in which
   the name of the file retrieved from the user is returned.  The
   resource identifier is set to SD_FILESAVE or SD_FILEOPEN, which
   correspond to dialog resources in the OWL-supplied filedial.dlg
   file.  If the resource identifier is reset a matching resource
   with similar controls must be supplied. */

static PTModule pTmpM = NULL;

TFileDialog::TFileDialog( PTWindowsObject AParent, int ResourceId, PTMyOpenDialogTransfer pTransfer,
  PInitDlg init, PTModule AModule ):
  TDialog( AParent, ResourceId, AModule )
{
  FilePath = pTransfer->chFilePath;
  Extension[0] = '\0';

  TransferBuffer = pTransfer;
  bFlCustom = FALSE;
  bFlGlobalLockHandle = FALSE;
  hfLists = NULL;

  pTmpM = AModule;
  (init)( this );
  pTmpM = NULL;
}

TFileDialog::~TFileDialog()
 {
   if( hfLists ) DeleteFont( hfLists );
 }

void TFileDialog::Init( PTFileDialog pDlg )
 {

   TBStatic   *pTmpSt;
   TComboBox  *pTmpCombo;
   TListBox   *pTmpList;
   TBButton   *pTmpBut;
   TBGroupBox *pTmpGrp;
   TBDivider  *pTmpDiv;
 
 
   pTmpSt = new TBStatic( pDlg, -1, "File &Name:", RX(14), RY(13), RX(36), RY(10), 50, pTmpM );
   pTmpSt->Attr.Style = WS_CHILD | WS_VISIBLE | WS_GROUP;
   pTmpSt->DisableTransfer();
   //CONTROL "File &Name:", -1, STATIC_CLASS, WS_CHILD | WS_VISIBLE | WS_GROUP, 14, 13, 36, 10

   pTmpCombo = new TComboBox( pDlg, ID_FNAME, RX(51), RY(12), RX(114), RY(62), CBS_DROPDOWN | CBS_AUTOHSCROLL | CBS_OEMCONVERT | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_GROUP | WS_TABSTOP,
     MAXPATH, pTmpM );
   //pTmpCombo->TransferBuffer = pInit->pComboBoxInput;
   pTmpCombo->EnableTransfer();
   //pTmpCombo->Attr = CBS_DROPDOWN | CBS_AUTOHSCROLL | CBS_OEMCONVERT | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_GROUP | WS_TABSTOP;
   //CONTROL "", ID_FNAME, "COMBOBOX", CBS_DROPDOWN | CBS_AUTOHSCROLL | CBS_OEMCONVERT | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_GROUP | WS_TABSTOP, 52, 11, 114, 62

   pTmpSt = new TBStatic( pDlg, -2, "File &Type:", RX(14), RY(38), RX(36), RY(10), 50, pTmpM );
   pTmpSt->Attr.Style = WS_CHILD | WS_VISIBLE | WS_GROUP;
   pTmpSt->DisableTransfer();
   //CONTROL "File &Type:", -1, STATIC_CLASS, WS_CHILD | WS_VISIBLE | WS_GROUP, 15, 34, 36, 10
  
   pTmpCombo = new TComboBox( pDlg, ID_FTYPE, RX(51), RY(36), RX(114), RY(72), CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | CBS_OEMCONVERT | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_GROUP | WS_TABSTOP,
     MAXPATH, pTmpM );
   pTmpCombo->EnableTransfer();
   pTmpCombo->EnableKBHandler();
   //pTmpCombo->TransferBuffer = pInit->pComboBoxType;
   //CONTROL "", ID_FTYPE, "COMBOBOX", CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | CBS_OEMCONVERT | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_GROUP | WS_TABSTOP, 52, 21, 114, 72

   pTmpSt = new TBStatic( pDlg, -3, "Directory:", RX(9), RY(63), RX(36), RY(9), 50, pTmpM );
   pTmpSt->Attr.Style = WS_CHILD | WS_VISIBLE | WS_GROUP;
   pTmpSt->DisableTransfer();
   //CONTROL "Directory:", -1, STATIC_CLASS, WS_CHILD | WS_VISIBLE | WS_GROUP, 9, 63, 36, 9

   pTmpSt = new TBStatic( pDlg, ID_FPATH, "", RX(45), RY(63), RX(127), RY(9), MAXPATH, pTmpM );
   pTmpSt->Attr.Style = WS_CHILD | WS_VISIBLE | WS_GROUP;
   //pTmpSt->TransferBuffer = pInit->chPath;
   pTmpSt->EnableTransfer();
   //TCONTROL "", ID_FPATH, STATIC_CLASS, WS_CHILD | WS_VISIBLE | WS_GROUP, 45, 63, 127, 9

   pTmpSt = new TBStatic( pDlg, -4, "&Files:", RX(14), RY(84), RX(23), RY(10), 50, pTmpM );
   pTmpSt->Attr.Style = WS_CHILD | WS_VISIBLE | WS_GROUP;
   pTmpSt->DisableTransfer();
   //CONTROL "&Files:", -1, STATIC_CLASS, WS_CHILD | WS_VISIBLE | WS_GROUP, 14, 85, 23, 10

   pTmpList = new TListBox( pDlg, ID_FLIST, RX(14), RY(96), RX(64), RY(85), pTmpM );
   pTmpList->Attr.Style = LBS_STANDARD | LBS_DISABLENOSCROLL | WS_CHILD | WS_VISIBLE | WS_TABSTOP;
   //pTmpList->TransferBuffer = pInit->pListFileNames;
   pTmpList->DisableTransfer();
   //CONTROL "", ID_FLIST, "LISTBOX", LBS_STANDARD | LBS_DISABLENOSCROLL | WS_CHILD | WS_VISIBLE | WS_TABSTOP, 14, 98, 64, 82

   pTmpSt = new TBStatic( pDlg, -5, "&Directories:", RX(102), RY(84), RX(46), RY(10), 50, pTmpM );
   pTmpSt->Attr.Style = WS_CHILD | WS_VISIBLE | WS_GROUP;
   pTmpSt->DisableTransfer();
   //CONTROL "&Directories:", -1, STATIC_CLASS, WS_CHILD | WS_VISIBLE | WS_GROUP, 102, 86, 46, 10

   pTmpList = new TListBox( pDlg, ID_DLIST, RX(102), RY(96), RX(64), RY(85), pTmpM );
   pTmpList->Attr.Style = LBS_STANDARD | LBS_DISABLENOSCROLL | WS_CHILD | WS_VISIBLE | WS_TABSTOP;
   //pTmpList->TransferBuffer = pInit->pListDir;
   pTmpList->DisableTransfer();
   //CONTROL "", ID_DLIST, "LISTBOX", LBS_STANDARD | LBS_DISABLENOSCROLL | WS_CHILD | WS_VISIBLE | WS_TABSTOP, 102, 98, 64, 82

   pTmpBut = new TBButton( pDlg, IDOK, "&OK", RX(192), RY(23), RX(45), RY(30), TRUE, pTmpM );
   pTmpBut->Attr.Style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON;
   pTmpBut->DisableTransfer();
   //CONTROL "&OK", IDOK, BUTTON_CLASS, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, 192, 23, 45, 30

   pTmpBut = new TBButton( pDlg, IDCANCEL, "&Cancel", RX(192), RY(72), RX(40), RY(31), TRUE, pTmpM );
   pTmpBut->Attr.Style = WS_CHILD | WS_VISIBLE | WS_TABSTOP;
   pTmpBut->DisableTransfer();
   //CONTROL "&Cancel", IDCANCEL, BUTTON_CLASS, WS_CHILD | WS_VISIBLE | WS_TABSTOP, 192, 72, 40, 31

   pTmpDiv = new TBDivider( pDlg, 4096, "", RX(182) + 1, RY(1), RX(4), RY(190), TRUE, FALSE, pTmpM );
   pTmpDiv->DisableTransfer();
   //pTmpDiv->Attr.Style = 3 | WS_CHILD | WS_VISIBLE;
   //CONTROL "", 4096, SHADE_CLASS, 3 | WS_CHILD | WS_VISIBLE, 181, 1, 3, 167

   pTmpGrp = new TBGroupBox( pDlg, 4097, "", RX(9), RY(81), RX(75), RY(103), pTmpM );
   pTmpGrp->Attr.Style = 1 | WS_CHILD | WS_VISIBLE;
   pTmpGrp->DisableTransfer();
   //CONTROL "", 4097, SHADE_CLASS, 1 | WS_CHILD | WS_VISIBLE, 9, 81, 75, 103

   pTmpGrp = new TBGroupBox( pDlg, 4097, "", RX(97), RY(81), RX(75), RY(103), pTmpM );
   pTmpGrp->Attr.Style = 1 | WS_CHILD | WS_VISIBLE;
   pTmpGrp->DisableTransfer();
   //CONTROL "", 4097, SHADE_CLASS, 1 | WS_CHILD | WS_VISIBLE, 97, 81, 75, 103

   pTmpGrp = new TBGroupBox( pDlg, 4097, "", RX(97), RY(81), RX(75), RY(103), pTmpM );
   pTmpGrp->Attr.Style = 1 | WS_CHILD | WS_VISIBLE;
   pTmpGrp->DisableTransfer();
   //CONTROL "", 4097, SHADE_CLASS, 1 | WS_CHILD | WS_VISIBLE, 97, 81, 75, 103

   pTmpGrp = new TBGroupBox( pDlg, 4097, "", RX(9), RY(6), RX(164), RY(48), pTmpM );
   pTmpGrp->Attr.Style = 1 | WS_CHILD | WS_VISIBLE;
   pTmpGrp->DisableTransfer();
   //CONTROL "", 4097, SHADE_CLASS, 1 | WS_CHILD | WS_VISIBLE, 9, 6, 164, 48
   //WM_GETFONT


   //TransferData( TF_SETDATA );
 }


/* Returns TRUE if a valid file name has been retrieved from the user to
   indicate that the file dialog can then be closed.  Retrieves the text
   of the edit control, and updates PathName.  Calls UpdateListBoxes;
   returns FALSE if UpdateListBoxes returns TRUE.  If the edit control
   contains an invalid file name, also returns FALSE. */
BOOL TFileDialog::CanClose()
{
  WORD PathLen;
  OFSTRUCT AnOfstruct;


  char chStr[MAXPATH+1], chDrive[MAXDRIVE], chDir[MAXDIR], chName[MAXFILE],
       chExt[MAXEXT];

   KeyInput();

   SendDlgItemMessage( HWindow, ID_FNAME, CB_SHOWDROPDOWN, FALSE, 0 );

   GetDlgItemText( HWindow, ID_FNAME, chStr, MAXPATH );
   _splitpath( chStr, chDrive, chDir, chName, chExt );
   if( _fstrlen(chDrive) && !_fstrlen(chName) && !_fstrlen(chExt) )
    {
      _makepath( chStr, chDrive, chDir, "*", ".*" );
      SetDlgItemText( HWindow, ID_FNAME, chStr );
    }


  GetDlgItemText(HWindow, ID_FNAME, PathName, MAXPATH);
  if ( lstrcmpi(&PathName[_fstrlen(PathName) - 2], "..") == 0 )
    _fstrcat(PathName, "\\");  // otherwise OpenFile messes up
  if ( OpenFile(PathName, &AnOfstruct, OF_PARSE) == -1 )
  {
    //MessageBeep(0);
    char chMsg[MAXPATH + 50];

    wsprintf( chMsg, "Path: [%s] cann't acess", PathName );           
    BWCCMessageBox( HWindow, chMsg, "Error", MB_ICONSTOP | MB_OK | MB_APPLMODAL );
    SelectFileName();
    return FALSE;
  }
  // Note that szPathName initially uses OEM char set.
  OemToAnsi((LPSTR)(AnOfstruct.szPathName), (LPSTR)(AnOfstruct.szPathName));
  _fstrncpy(PathName, (LPSTR)(AnOfstruct.szPathName), MAXPATH);
  PathName[MAXPATH-1] = '\0';
  PathLen = _fstrlen(PathName);
  if ( PathName[PathLen - 1] == '\\' ||
       HasWildCards(PathName) ||
       GetFocus() == GetDlgItem(HWindow, ID_DLIST) )
  {
    if ( PathName[PathLen - 1] == '\\' )
      _fstrncat(PathName, FileSpec, (MAXPATH-1) - _fstrlen(PathName));
    if ( !UpdateListBoxes() )
    {
      char chMsg[MAXPATH + 50];

      wsprintf( chMsg, "Path: [%s] cann't acess", PathName );
      BWCCMessageBox( HWindow, chMsg, "Error", MB_ICONSTOP | MB_OK | MB_APPLMODAL );
      //MessageBeep(0);
      SelectFileName();
    }
    return FALSE;
  }
  _fstrncat(PathName, "\\", (MAXPATH-1) - _fstrlen(PathName));
  _fstrncat(PathName, FileSpec, (MAXPATH-1) - _fstrlen(PathName));

  if ( UpdateListBoxes() )
    return FALSE;
  PathName[PathLen] = '\0';
  if ( GetExtension(PathName)[0] == '\0' )
  {
    _fstrncat(PathName, Extension, (MAXPATH-1) - _fstrlen(PathName));
    PathName[MAXPATH-1] = '\0';
  }
  AnsiLower(_fstrcpy(FilePath, PathName));

  if( ((TMyOpenDialogTransfer *)TransferBuffer)->bFlCheckName == TRUE )
   {
     if( OpenFile(PathName, &AnOfstruct, OF_EXIST) == -1 )
      {
        char chMsg[MAXPATH + 50];
                                       
	wsprintf( chMsg, "Path: [%s] cann't acess", PathName );
	BWCCMessageBox( HWindow, chMsg, "Error", MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL );
	SelectFileName();

        return FALSE;
      }
   }

  return TRUE;
}

static void SetFontInstance( Pvoid pCh, Pvoid font )
 {
   SendDlgItemMessage( PTWindowsObject(pCh)->HWindow, PTWindowsObject(pCh)->GetId(),
     WM_SETFONT, (WPARAM)font, FALSE );
 }

/* Sets up the file dialog.  Limits the number of characters which
   can be entered into the edit control to MAXPATH - 1.  Then calls
   UpdateListBoxes and SelectFileName. */
void TFileDialog::SetupWindow()
{
  MoveWindow( HWindow, 0, 0, RX(237) + 12, RY(192) + 30, FALSE );
  //18, 54, 237, 192

  TDialog::SetupWindow();


   HFONT hf, hf2;
   hf = (HFONT)SendMessage( HWindow, WM_GETFONT, 0L, 0L );

   hfLists = CreateFont( -12, 0, 0, 0, FW_MEDIUM,
     0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_STROKE_PRECIS,
     DEFAULT_QUALITY, DEFAULT_PITCH, "Courier New" );


   hf2 = hfLists ? hfLists:hf;


   //ForEach( SetFontInstance, Pvoid(hf) );
   SendDlgItemMessage( HWindow, -1, WM_SETFONT, (WPARAM)hf, FALSE );
   SendDlgItemMessage( HWindow, -2, WM_SETFONT, (WPARAM)hf, FALSE );
   SendDlgItemMessage( HWindow, -3, WM_SETFONT, (WPARAM)hf, FALSE );
   SendDlgItemMessage( HWindow, -4, WM_SETFONT, (WPARAM)hf, FALSE );
   SendDlgItemMessage( HWindow, -5, WM_SETFONT, (WPARAM)hf, FALSE );
   SendDlgItemMessage( HWindow, ID_FNAME, WM_SETFONT, (WPARAM)hf, FALSE );
   SendDlgItemMessage( HWindow, ID_FTYPE, WM_SETFONT, (WPARAM)hf, FALSE );
   SendDlgItemMessage( HWindow, ID_FPATH, WM_SETFONT, (WPARAM)hf, FALSE );
                                         
   SendDlgItemMessage( HWindow, ID_FLIST, WM_SETFONT, (WPARAM)hf2, FALSE );
   SendDlgItemMessage( HWindow, ID_DLIST, WM_SETFONT, (WPARAM)hf2, FALSE );


  SendDlgItemMessage(HWindow, ID_FNAME, CB_LIMITTEXT, MAXPATH-1, 0);
  _fstrncpy(PathName, FilePath, MAXPATH);
  PathName[MAXPATH-1] = '\0';

  if ( !UpdateListBoxes() )
  {
    _fstrcpy(PathName, "*.*");
    UpdateListBoxes();
  }

  SelectFileName();
}

void TFileDialog::KeyInput()
 {                              
   static const int iMaxHistoryLen = 25;
   char chStr[MAXPATH+1], chDrive[MAXDRIVE], chDir[MAXDIR], chName[MAXFILE],
        chExt[MAXEXT];


   if( bFlCustom == TRUE )
    {                  
      long count;

      GetDlgItemText( HWindow, ID_FNAME, chStr, MAXPATH );

      //bFlCustom = FALSE;
      if( (count = SendDlgItemMessage( HWindow, ID_FNAME, CB_GETCOUNT, 0, 0 )) > iMaxHistoryLen )
	SendDlgItemMessage( HWindow, ID_FNAME, CB_DELETESTRING, count, 0 );

      BOOL bFlFound = FALSE;
      for( int i = 0; i < count && bFlFound == FALSE; i++ )
       {
	 char chBuf[MAXPATH+1];
	 SendDlgItemMessage( HWindow, ID_FNAME, CB_GETLBTEXT, i, (LPARAM)chBuf );
         if( !lstrcmpi(chBuf, chStr) ) bFlFound = TRUE;
       }

      if( bFlFound == FALSE )
	SendDlgItemMessage( HWindow, ID_FNAME, CB_INSERTSTRING, 0, (LPARAM)chStr );

    }

   bFlCustom = FALSE;
 }

void TFileDialog::ShutDownWindow()
 {
   //TransferData( TF_GETDATA );
   TDialog::ShutDownWindow();
 }


/* Responds to messages from the edit control.  Enables the OK button
   if the edit control contains text. */
void TFileDialog::HandleFName(TMessage& Msg)
{

   static BOOL bFlLock = FALSE;

   if( bFlLock == TRUE || bFlGlobalLockHandle == TRUE ) return;

   bFlLock = TRUE;
   if( HIWORD(Msg.LParam) == (unsigned)CBN_EDITUPDATE ) bFlCustom = TRUE;
   else if( HIWORD(Msg.LParam) == (unsigned)CBN_KILLFOCUS && 
     bFlCustom == TRUE )
     KeyInput();
   else if( HIWORD(Msg.LParam) == CBN_SELENDOK )
    {                                             
      char chBuf[MAXPATH];
      int pos;

      SendDlgItemMessage( HWindow, ID_FNAME, CB_GETLBTEXT,
	(pos = SendDlgItemMessage(HWindow, ID_FNAME, CB_GETCURSEL, 0, 0)),
	(LPARAM)chBuf );

      if( pos >= 0 )
       {
         SendDlgItemMessage( HWindow, ID_FNAME, CB_DELETESTRING, pos, 0 );
         SendDlgItemMessage( HWindow, ID_FNAME, CB_INSERTSTRING, 0, (LPARAM)chBuf );
	 SendDlgItemMessage( HWindow, ID_FNAME, CB_SETCURSEL, 0, 0 );
       }
    }
                                       

  if ( HIWORD(Msg.LParam) == EN_CHANGE )
    EnableWindow(GetDlgItem(HWindow, IDOK),
      SendMessage((HWND)LOWORD(Msg.LParam), WM_GETTEXTLENGTH, 0, 0) != 0 );

  bFlLock = FALSE;
}

/* Responds to messages from the file list box.  Updates PathName with
   the name of the selected file and calls UpdateFileName, when the
   selection in the list box changes.  Attempts to close the dialog if
   the selected entry was double-clicked.  Clears the selection in the
   list box when it loses the focus. */
void TFileDialog::HandleFList(TMessage& Msg)
{
  switch (HIWORD(Msg.LParam)) {
    case LBN_SELCHANGE:
    case LBN_DBLCLK:
         DlgDirSelect(HWindow, PathName, ID_FLIST);
         UpdateFileName();
         if ( HIWORD(Msg.LParam) == LBN_DBLCLK )
           CloseWindow(IDOK);
         break;
    case LBN_KILLFOCUS:
	 SendMessage((HWND)LOWORD(Msg.LParam), LB_SETCURSEL, (WORD)-1, 0);
         break;
  }
}

/* Responds to messages from the directory list box.  Updates PathName
   when the selection in the list box changes.  Calls UpdateListBoxes if
   the selected entry was double-clicked, else calls UpdateFileName.
   Clears the selection in the list box when it loses the focus.  */
void TFileDialog::HandleDList(TMessage& Msg)
{
  switch (HIWORD(Msg.LParam)) {
    case LBN_SELCHANGE:
    case LBN_DBLCLK:
         DlgDirSelect(HWindow, PathName, ID_DLIST);
         _fstrcat(PathName, FileSpec);
         if ( HIWORD(Msg.LParam) == LBN_DBLCLK )
           UpdateListBoxes();
         else
           UpdateFileName();
         break;
    case LBN_KILLFOCUS:
	 SendMessage((HWND)LOWORD(Msg.LParam), LB_SETCURSEL, (WORD)-1, 0);
         break;
  }
}

/* Selects the text in the edit control, and sets the focus to the
   edit control. */
void TFileDialog::SelectFileName()
{
  SendDlgItemMessage(HWindow, ID_FNAME, EM_SETSEL, 0, 0x7FFF000L);
  SetFocus(GetDlgItem(HWindow, ID_FNAME));
}

/* Sets the text of the edit control to PathName, and selects the
   text. */
void TFileDialog::UpdateFileName()
{
  SetDlgItemText(HWindow, ID_FNAME, AnsiLower(PathName));
  SendDlgItemMessage(HWindow, ID_FNAME, EM_SETSEL, 0, 0x7FFF0000L);
}

/* Attempts to update the file and/or directory list boxes.  If updated,
   calls UpdateFileName and returns TRUE.  Else returns FALSE. */
BOOL TFileDialog::UpdateListBoxes()
{
  int Result;
  char Path[MAXPATH];

  Path[0] = '\0';
  if ( GetDlgItem(HWindow, ID_FLIST) )
  {
    _fstrcpy(Path, PathName);
    Result = DlgDirList(HWindow, Path, ID_FLIST, ID_FPATH, 0);
    if ( Result )
      DlgDirList(HWindow, "*.*", ID_DLIST, 0, 0xC010);
  }
  else
  {
      _fstrncpy(Path, PathName, MAXPATH);
      Path[MAXPATH-1] = '\0';
      _fstrncat(Path, "*.*", (MAXPATH-1) - _fstrlen(Path));
      Result = DlgDirList(HWindow, Path, ID_DLIST, ID_FPATH, 0xC010);
  }
  if ( Result )
  {
    _fstrncpy(FileSpec, GetFileName(PathName), FILESPEC);
    FileSpec[FILESPEC-1] = '\0';
    _fstrcpy(PathName, FileSpec);
    UpdateFileName();
    return TRUE;
  }
  return FALSE;
}


void TFileDialog::HandleFType( RTMessage Msg )
 {
   char chTmp[MAXEXT + MAXFILE], *pCh;// chTmp2[MAXEXT + MAXFILE];
   static BOOL bFlLock = FALSE;

   if( bFlLock == TRUE || bFlGlobalLockHandle == TRUE ) return;


   switch( HIWORD(Msg.LParam) )
    {
      case CBN_SELENDOK:
	bFlLock = TRUE;
	SendDlgItemMessage( HWindow, ID_FTYPE, CB_SHOWDROPDOWN, FALSE, 0 );
      case CBN_SELCHANGE:
	bFlLock = TRUE;
	GetDlgItemText( HWindow, ID_FTYPE, chTmp, MAXEXT + MAXFILE - 1 );
	if( (pCh = _fstrchr( chTmp, ' ' )) ) *pCh = 0;
        SetDlgItemText( HWindow, ID_FNAME, chTmp );

	CanClose();

	bFlLock = FALSE;
        break;
    }

 }

void TFileDialog::TransferData( WORD Direction )
 {
   /*
   bFlGlobalLockHandle = TRUE;
   TDialog::TransferData( Direction );
   bFlGlobalLockHandle = FALSE;*/
 }


TStreamable *TFileDialog::build()
{
  return new TFileDialog(streamableInit);
}

TStreamableClass RegFileDialog("TFileDialog", TFileDialog::build,
					    __DELTA(TFileDialog));


