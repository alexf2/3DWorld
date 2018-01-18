#if !defined(_MY_MDI_)
#define _MY_MDI_

#include <owl.h>
#include <mdi.h>
#include <bwcc.h>

#include "INC\icon.inc"
#include "INC\errors.inc"
#include "INC\menu.inc"
#include "INC\dlg.inc"
#include "INC\systbbmp.inc"

#include "abdialog.hpp"
#include "windiv.hpp"
#include "mymdich.hpp"
#include "fmaneger.hpp"
#include "ucs.hpp"
#include "filedial.hpp"
#include "scena.hpp"
#include "status.hpp"
#include "viewsdlg.hpp"
#include "toolbar.hpp"
#include "facedial.hpp"

#include <dbllist.h>


_CLASSDEF(T3dWorldApp)
_CLASSDEF(TMyMDIClient)
_CLASSDEF(TMyMDIFrame)

struct TTr
 {
   int id;
   HFILE hf;
 };


           //TMDIClient
class  TMyMDIClient: public TMDIClient //have free spece in down side for Status Line
 {
public:
  TMyMDIClient( PTMDIFrame AParent, PTModule AModule = NULL ):
    TMDIClient( AParent, AModule )
   {
    
   }
  virtual BOOL Create();

  PTMyMDIChild GetChildPtrForHWND( HWND ) const;

protected:
  virtual void SetupWindow();
  virtual void WMSize( RTMessage msg ) = [WM_FIRST + WM_SIZE];
  //void WMMDIDestroy( RTMessage ) = [WM_FIRST + WM_MDIDESTROY];
  void WMMDIMaximize( RTMessage ) = [WM_FIRST + WM_MDIMAXIMIZE];
  void WMMDIRestore( RTMessage ) = [WM_FIRST + WM_MDIRESTORE];


private:
  TRECT rtStatusLine;

 };



class  TMyMDIFrame : public TMDIFrame
{
  friend class T3dWorldApp;
public:
  typedef int TStrategyTable[SYSTOOL_NUMB_BTN][3];
  typedef int TTypeModesTable[NUMB_MODES][3];

  PTWindow ActiveChild_;

  TMyMDIFrame(LPSTR ATitle, LPSTR MenuId, PTModule AModule = NULL );

  virtual ~TMyMDIFrame();

  virtual void Destroy();
  virtual BOOL AssignMenu( LPSTR MenuName );

  virtual PTWindowsObject InitChild();
  virtual BOOL CanClose();
  virtual void InitClientWindow();
  int GetStatH();

  BOOL CondVisyalMDI( PTMyMDIChild pCh ) const;

  virtual classType isA() const;

  BOOL IsInvertSelectMode() const { return bFlInvertSelectMode; }

  LPCSTR MainPath() const { return lpMainPath; }

  void MakeChildTop( RTMyMDIChild rChild );

  virtual BOOL CloseChildren();

  void CloseScene( int idClose );
  void SetCheckedInMenu( BOOL bFlDefault = FALSE );//TMDIFrame
  void CheckToolbar( ModeChild mcMode );
  void CheckToolbarIndepend();
  void InternalChildActivate();
  int CountChildren();

  BOOL IsUserCommand( int cm ) const;

protected:
  virtual void GetWindowClass( WNDCLASS _FAR & );
  virtual LPSTR GetClassName();
  virtual void SetupWindow();
  void DefaultMove( BOOL );
  virtual void DefWndProc( RTMessage Msg );
  virtual void WMSize( RTMessage msg ) = [WM_FIRST + WM_SIZE];
  virtual void WMMove( RTMessage msg ) = [WM_FIRST + WM_MOVE];
  void WMEnable( RTMessage msg ) = [WM_FIRST + WM_ENABLE];

  virtual void WMDropFile( RTMessage Msg ) = [WM_FIRST + WM_DROPFILES];


private:

  static TStrategyTable iStrateg;
  static TTypeModesTable iTypeModes;

  LPSTR lpMainPath;

  PTStatusWindow  pStatusWnd;
  int dafaultHandleStatusLine;
  int commonHandleStatusLine;
  TViewDlgTransfer vdtTransfer;

  DoubleList listSysToolBarBmp;
  DoubleList listBodysBmp;
  DoubleList dblMedium;
  PTToolBarWindow pSysTool;

  DoubleList listClipboard;

  BOOL bFlInvertSelectMode;

  HFONT hfHint;
  //int hStatus;

  static TStrategy GetCommandStrategy( int command );
  static int* FindTypeMode( ModeChild mcM );
  static int* FindTypeMode2( int idTbBtn );
  static int* FindBtn( ModeChild idMode );

  int SaveMDI( HFILE );
  int LoadMDI( HFILE hf, PTScena pscScena );

  void ChangeBounds();
  void ResizeAdaptiveChildren();
  void MoveSysToolbar();

  virtual void WMCreate(RTMessage Msg) = [WM_FIRST + WM_CREATE];
  void WMQueryNewPalette( RTMessage ) = [WM_FIRST + WM_QUERYNEWPALETTE];
  void WMPaletteChanged( RTMessage ) = [WM_FIRST + WM_PALETTECHANGED];


  void CMAbout( RTMessage ) = [CM_FIRST + UCM_ABOUT];
  virtual void CMExit( RTMessage ) = [CM_FIRST + UCM_EXIT];
  void CMSelectScrVports( RTMessage ) = [CM_FIRST + UCM_SELECT_SCRVPORTS];

  void CMLogging( RTMessage ) = [CM_FIRST + UCM_LOGGING];

  void CMOpen( RTMessage ) = [CM_FIRST + UCM_OPEN];
  void CMNewFile( RTMessage ) = [CM_FIRST + UCM_NEW_FILE];
  void CMSave( RTMessage ) = [CM_FIRST + UCM_SAVE];
  void CMSaveAs( RTMessage ) = [CM_FIRST + UCM_SAVE_AS];
  void CMClose( RTMessage ) = [CM_FIRST + UCM_CLOSE];
  //void CMCloseAll( RTMessage ) = [CM_FIRST + UCM_CLOSE_ALL];

  void CMRestoreFromSource( RTMessage ) = [CM_FIRST + UCM_RETRIVE_SOURCE];

  void CMGridExtents( RTMessage ) = [CM_FIRST + UCM_GRID_EXTENTS];
  void CMUseSnap( RTMessage ) = [CM_FIRST + UCM_USE_SNAP];
  void CMUseGrid( RTMessage ) = [CM_FIRST + UCM_USE_GRID];
  void CMAxisOnOff( RTMessage ) = [CM_FIRST + UCM_AXIS_ON_OFF];
  void CMUserUcsSetup( RTMessage ) = [CM_FIRST + UCM_USERUCS_SETUP];
  void CMVPortAlign( RTMessage ) = [CM_FIRST + UCM_VPORT_ALIGN];
  void CMCreateSimpleObj( RTMessage ) = [CM_FIRST + UCM_CREATE_SIMPLEOBJ];
  void CMHideGeometry( RTMessage ) = [CM_FIRST + UCM_HIDEGEOMETRY];
  void CMFullDetall( RTMessage ) = [CM_FIRST + UCM_FULLDETALL];

  void CMSceneInfo( RTMessage ) = [CM_FIRST + UCM_SCENEINFO];

  void CMMoveBody( RTMessage ) = [CM_FIRST + UCM_MOVEBODY];
  void CMDeleteBody( RTMessage ) = [CM_FIRST + UCM_DELETEBODY];
  void CM3DScaleBody( RTMessage ) = [CM_FIRST + UCM_3DSCALEBODY];
  void CM2DScaleBody( RTMessage ) = [CM_FIRST + UCM_2DSCALEBODY];
  void CMRotateBody( RTMessage ) = [CM_FIRST + UCM_ROTATEBODY];
  void CMRender( RTMessage ) = [CM_FIRST + UCM_RENDER];
  void CMBkgndSetup( RTMessage ) = [CM_FIRST + UCM_BKGND_SETUP];
  void CMImageOptions( RTMessage ) = [CM_FIRST + UCM_IMAGE_OPTIONS];
  void CMRenderFull( RTMessage ) = [CM_FIRST + UCM_RENDER_FULL];
  


  void CMToggleVTool( RTMessage ) = [CM_FIRST + UCM_TOGGLE_VPORTTOOL];
  void CMToggleStatus( RTMessage ) = [CM_FIRST + UCM_TOGGLE_STATUSBAR];

  void CMHold( RTMessage ) = [CM_FIRST + UCM_TB_HOLD];
  void CMFetch( RTMessage ) = [CM_FIRST + UCM_TB_FETCH];

  void CMUserUcsShift( RTMessage );
  void CMUserUcsScale( RTMessage );
  void CMFitToZoom( RTMessage );
  void CMFitToWindow( RTMessage );

  void CMConnect( RTMessage );
  void CMGroup( RTMessage );


  void CMSelect( RTMessage ) = [CM_FIRST + UCM_SELECT];
  void CMSelectWindow( RTMessage ) = [CM_FIRST + UCM_SELECTWINDOW];
  void CMSelectAll( RTMessage ) = [CM_FIRST + UCM_SELECTALL];
  void CMInvertSelMode( RTMessage ) = [CM_FIRST + UCM_INVERTSELMODE];
  void CMInvertSelecting( RTMessage ) = [CM_FIRST + UCM_INVERTSELECTING];

  void CMSetBPVisible( RTMessage ) = [CM_FIRST + UCM_SET_BP_VISIBLE];
  void CMUseGlobalBP( RTMessage ) = [CM_FIRST + UCM_USE_GLOBAL_BP];
  void CMSetupGlobalBP( RTMessage ) = [CM_FIRST + UCM_SETUP_GLOBAL_BP];

  void CMAligmentVertex( RTMessage ) = [CM_FIRST + UCM_ALIGMENT_VERTEX];
  void CMAligmentFacet( RTMessage ) = [CM_FIRST + UCM_ALIGMENT_FACET];

  void CMUseBaseUCS( RTMessage ) = [CM_FIRST + UCM_USE_BASEUCS];

  void CMUseCrossingFrame( RTMessage ) = [CM_FIRST + UCM_USE_CROSSING_FRAME];

  void CMCut( RTMessage ) = [CM_FIRST + UCM_CUTBODY];
  void CMCopy( RTMessage ) = [CM_FIRST + UCM_COPYBODY];
  void CMPaste( RTMessage ) = [CM_FIRST + UCM_PASTEBODY];

  void CMMDINext( RTMessage ) = [CM_FIRST + UCM_MDINEXT];
  void CMMDIPrev( RTMessage ) = [CM_FIRST + UCM_MDIPREV];

  void CMZoom( RTMessage ) = [CM_FIRST + UCM_ZOOM];
  void CMUnZoom( RTMessage ) = [CM_FIRST + UCM_UNZOOM];


  void CMAmbient( RTMessage ) = [CM_FIRST + UCM_AMBIENT];
  void CMCreateOmni( RTMessage ) = [CM_FIRST +  UCM_OMNI_CREATE];
  void CMOmniAdjust( RTMessage ) = [CM_FIRST +  UCM_OMNI_ADJUST];
  void CMPLaceHilite( RTMessage ) = [CM_FIRST + UCM_PLACEHILITE];


  virtual void WMCommand(RTMessage Msg) = [WM_FIRST + WM_COMMAND];
  virtual void WMNcCalcSize( RTMessage Msg ) = [WM_FIRST + WM_NCCALCSIZE];
  
  void CMAssignMaterial( RTMessage ) = [CM_FIRST + UCM_ASSIGN_MATERIAL];
  void CMSMoth( RTMessage ) = [CM_FIRST + UCM_SMOTH];
  void CMSaveMedium( RTMessage ) = [CM_FIRST + UCM_SAVE_MEDIUM];
  void CMLoadMedium( RTMessage ) = [CM_FIRST + UCM_LOAD_MEDIUM];


  void CMZoomToFit( RTMessage ) = [CM_FIRST + UCM_F_FIT_TO_ZOOM];

  void CMCopyBmpClip( RTMessage ) = [CM_FIRST + UCM_COPY_BMP_CLIPBOARD];




  void EnableWinConnectMenu( UINT uiFl );

  void CreateScreenLayoutFor( TFileInfo *pFileInf, RTWindowDivision dsLayput, PTScena pScena );

  void Open( LPSTR lpName );

  void ToggleAdaptDiv( PTFileInfo pInfo );

  void AssignNewVPoints( PTMyMDIChild, RTWindowDivision );
  void LoadVPoints( PTMyMDIChild, RTWindowDivision );

  TDivStyle  DsDefaultScreenLayout;  //current def. divstyle
  PTFileManeger fmManeger;

  TFileInfo  *pfiCurrentInit; //uses internaly only for tmp. transfer InitChild();
			      // cann't delete !
  TViewPoint vpCurrentInit;   //tmp. transfer  InitChild();
  PTMyRect pRectCur;   //uses internaly
  int iDivIndexCurrent; //uses internaly
  BOOL bFlCurrentVisibleFlag;

  TMyOpenDialogTransfer  openDlgTransfer;
  void OpenDlgTransferInit();

  TMyOpenDialogTransfer  saveDlgTransfer;
  void SaveDlgTransferInit();

  TMyOpenDialogTransfer  openMedDlgTransfer;
  void OpenMedDlgTransferInit();

  TMyOpenDialogTransfer  saveMedDlgTransfer;
  void SaveMedDlgTransferInit();

  List listOfScene;
  PTScena  GetScena( UINT id );

};


#endif

