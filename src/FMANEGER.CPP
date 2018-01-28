#pragma option -v-

#include "fmaneger.hpp"

#include "dir.h"
#include "string.h"
#include "INC\menu.inc"
#include "mymdich.hpp"

#include <owl.h> 


extern HINSTANCE hInstRC;

//ForEach void ForEach(TActionFunc Action, Pvoid PParamList)
// void (TActionFunc)(Pvoid Child, PVoid ParamList);
static void ChangeTypeOfTitle( Pvoid pChild, Pvoid pInfo )
 {
   PTMyMDIChild pCh    = (PTMyMDIChild)pChild;
   PTFileInfo   pFInfo = (PTFileInfo)pInfo;

   if( PTWindow(pCh)->isA() != classtypeMDIChWindow ||
       pCh->GetScId() != pFInfo->IdFile ) return;

   pCh->SetTitleType( TMyMDIChild::ShowCopy );
   pCh->GenTitle( pFInfo );
 }

static void ChangeTypeOfTitle_( Pvoid pChild, Pvoid pInfo )
 {
   PTMyMDIChild pCh    = (PTMyMDIChild)pChild;
   if( PTWindow(pCh)->isA() != classtypeMDIChWindow ) return;

   TConnect   *pConnect = (TConnect *)pInfo;
   PTFileInfo pChInfo = pConnect->pMan->GetInfo(pCh->GetScId());

   if( !pChInfo ) return;

   if( lstrcmpi(pChInfo->LpFileNameFull,
       pConnect->pInf->LpFileNameFull) ) return;

   pCh->SetTitleType( TMyMDIChild::NoShowCopy );
   pCh->GenTitle( pConnect->pInf );
 }


static void ChangeTitle( Pvoid pChild, Pvoid pInfo )
 {
   PTMyMDIChild pCh    = (PTMyMDIChild)pChild;
   PTFileInfo   pFInfo = (PTFileInfo)pInfo;

   if( PTWindow(pCh)->isA() != classtypeMDIChWindow ||
       pCh->GetScId() != pFInfo->IdFile ) return;

   pCh->GenTitle( pFInfo );
 }


MemBlocks* TFileInfo::mgr = NULL;
long TFileInfo::countMemMgr = 0;

void _FAR* TFileInfo::operator new( size_t size )
 {
   if( countMemMgr++ == 0 )
     TFileInfo::mgr = new MemBlocks( sizeof(TFileInfo), 25 );

   return TFileInfo::mgr->allocate( size );
 }

void TFileInfo::operator delete( void _FAR* pToDel )
 {   
   TFileInfo::mgr->free( pToDel );

   if( --countMemMgr == 0 )
    {
      delete TFileInfo::mgr;
      TFileInfo::mgr = NULL;
    }
 }



int TFileInfo::isEqual( const Object& rObj ) const
 {
   if( (TFileInfo huge *)this != (TFileInfo huge *)(&rObj) ) return FALSE;

   /*if( _fstrcmp(LpFileNameFull, ((RTFileInfo)rObj).LpFileNameFull) ) return FALSE;
   if( IdFile != ((RTFileInfo)rObj).IdFile ) return FALSE;
   if( CCopy != ((RTFileInfo)rObj).CCopy ) return FALSE;
     */
   return TRUE;
 }


int TFileInfo::MaxLenShortFName = 25;

TFileInfo::TFileInfo( LPSTR LpFNameFull, int IdFile, UINT CCopy, TDivStyle DsDiv )
 {
   this->IdFile = IdFile; this->CCopy = CCopy;
   lpNameHold = NULL;
   LpFileNameFull = LpFileName = LpFileNameShort = NULL;
   Status = 0;
   DsDivScreen = DsDiv;
   bFlWndGrowAsParent = FALSE;

   if( (LpFileNameFull = new char[ _fstrlen(LpFNameFull) + 1 ]) )
     _fstrcpy( LpFileNameFull, LpFNameFull );
   else { Status = EM_OUTOFMEMORY; return; }

   CreateShortName( LpFileNameFull );
   if( !Status ) CreateFName( LpFileNameFull );

 }


TFileInfo::TFileInfo( TFileInfo& obj )
 {
   IdFile = obj.IdFile; CCopy = obj.CCopy; 
   DsDivScreen = obj.DsDivScreen;
   LpFileNameFull = LpFileName = LpFileNameShort = NULL;
   Status = 0;
   //bFlWndGrowAsParent = FALSE;

   if( obj.LpFileNameFull )
    {                           
      if( (LpFileNameFull = new char[ _fstrlen(obj.LpFileNameFull) + 1 ]) )
	_fstrcpy( LpFileNameFull, obj.LpFileNameFull );
      else { Status = EM_OUTOFMEMORY; return; }
    }

    if( obj.LpFileName )
    {                           
      if( (LpFileName = new char[ _fstrlen(obj.LpFileName) + 1 ]) )
	_fstrcpy( LpFileName, obj.LpFileName );
      else { Status = EM_OUTOFMEMORY; return; }
    }

    if( obj.LpFileNameShort )
    {                           
      if( (LpFileNameShort = new char[ _fstrlen(obj.LpFileNameShort) + 1 ]) )
	_fstrcpy( LpFileNameShort, obj.LpFileNameShort );
      else { Status = EM_OUTOFMEMORY; return; }
    }

   if( obj.lpNameHold )
    {
      if( (lpNameHold = new char[ _fstrlen(obj.lpNameHold) + 1 ]) )
	_fstrcpy( lpNameHold, obj.lpNameHold );
      else { Status = EM_OUTOFMEMORY; return; }
    }
 }

TFileInfo::~TFileInfo()
 {
   if( LpFileNameFull ) delete []LpFileNameFull;
   if( LpFileName ) delete []LpFileName;
   if( LpFileNameShort ) delete []LpFileNameShort;
   if( lpNameHold )
    {
      OFSTRUCT of;
      OpenFile( lpNameHold, &of, OF_DELETE );

      delete []lpNameHold;
    }

   lpNameHold = LpFileNameFull = LpFileName = LpFileNameShort = NULL;
 }


void TFileInfo::CreateFName( LPSTR LpFullName )
 {
   char Drive[MAXDRIVE], Dir[MAXDIR], Name[MAXFILE], Ext[MAXEXT];


   if( LpFileName ) delete []LpFileName; LpFileName = NULL;

   _splitpath( LpFullName, Drive, Dir, Name, Ext );
   int Len = _fstrlen( Name ) + _fstrlen( Ext ) + 2;

   if( (LpFileName = new char[ Len ]) )
    {
      _fstrcpy( LpFileName, Name );
      _fstrcat( LpFileName, Ext );
    }
   else { Status = EM_OUTOFMEMORY; return; }
 }

void TFileInfo::CreateShortName( LPSTR LpFullName )
 {
   char Path[MAXPATH];
   int Len;


   if( LpFileNameShort ) delete []LpFileNameShort; LpFileNameShort = NULL;
                
   _fstrcpy( Path, LpFullName );
   if( (Len = _fstrlen(LpFullName)) > TFileInfo::MaxLenShortFName )
    {
      LPSTR LpName;
      int   LenName;      
      const char *StrIns = "\\...\\";

      for( LpName = LpFullName + Len - 1, LenName = 0;
	   LenName < Len-1 && *(LpName-1) != '\\';
	   LenName++, LpName-- );

      int SizeNeed = TFileInfo::MaxLenShortFName - LenName - _fstrlen(StrIns);
      LPSTR LpPrev, LpCur;

      for( LpPrev = LpCur = Path, LenName = 0;
	   LenName <= SizeNeed && *LpCur;
	   LenName++, LpCur++ )
	if( *LpCur == '\\' ) LpPrev = LpCur;
      *LpPrev = 0;
      _fstrcat( Path, StrIns ); _fstrcat( Path, LpName );
    }

   if( (LpFileNameShort = new char[ _fstrlen(Path) + 1 ]) )
     _fstrcpy( LpFileNameShort, Path );
   else { Status = EM_OUTOFMEMORY; return; }

 }


TFileManeger::TFileManeger( PTWindowsObject pMDIOwner, int DeepOfMenuList )
 {
   MaxDeepOfMenuList = DeepOfMenuList;
   pOwner = pMDIOwner;
   CNewFiles = 0;
   Status = 0;

 }


TFileManeger::~TFileManeger()
 {

 }

LPSTR TFileManeger::GetHoldName( int id )
 {
   PTFileInfo fiInfo = GetInfo( id );
   if( !fiInfo ) return NULL;

   if( !fiInfo->lpNameHold )
    {
      if( !(fiInfo->lpNameHold = new char[144]) ) return NULL;

      GetTempFileName( 0, "h", id, fiInfo->lpNameHold );
      if( !_fstrlen(fiInfo->lpNameHold) )
       {
	 delete [](fiInfo->lpNameHold);
	 fiInfo->lpNameHold = NULL;
         return NULL;
       }

      char drive[MAXDRIVE], dir[MAXDIR], name[MAXFILE], ext[MAXEXT];
      _splitpath( fiInfo->lpNameHold, drive, dir, name, ext );
      lstrcpy( ext, ".HLD" );
      _makepath( fiInfo->lpNameHold, drive, dir, name, ext );

      OFSTRUCT of;
      HFILE hf = OpenFile( fiInfo->lpNameHold, &of, OF_CREATE );
      _lclose( hf );

      if( hf == HFILE_ERROR )
       {
	 delete [](fiInfo->lpNameHold);
	 fiInfo->lpNameHold = NULL;
	 return NULL;
       }
    }

   return fiInfo->lpNameHold;
 }

void TFileManeger::SetMenu()
 {
   HMENU HmenuFile = GetMenu( pOwner->HWindow );
   char Buff[ 80 ];
   int  CCount;

   HmenuFile = GetSubMenu( HmenuFile, 0 );
   int CommandForDelete = UCM_FILEACESS_FIRST;

   for( ; DeleteMenu(HmenuFile, CommandForDelete, MF_BYCOMMAND); CommandForDelete++ );

   ListIterator liIter = ListIterator( ListMenuFiles );
   PTFileInfo FiInfo = (PTFileInfo)&liIter.current();

   for( CCount = 1, CommandForDelete = UCM_FILEACESS_FIRST;
	int(liIter);
	FiInfo = (PTFileInfo)&++liIter, CommandForDelete++, CCount++ )
    {
      wsprintf( Buff, "&%u  %s", CCount, FiInfo->LpFileNameShort );
     AppendMenu( HmenuFile, MF_ENABLED, CommandForDelete, Buff );
    }
 }


void TFileManeger::SetMaxDeep( int DeepOfMenuList )
 {
   MaxDeepOfMenuList = DeepOfMenuList;
   int COfNeedDelete = ListMenuFiles.getItemsInContainer() - MaxDeepOfMenuList;


   if( COfNeedDelete > 0 )
    {
      ListIterator liIter = ListIterator( ListMenuFiles );
      PTFileInfo FiInfo = (PTFileInfo)&liIter.current();

      for( int i = ListMenuFiles.getItemsInContainer() - COfNeedDelete;
	   i > 0; i--, FiInfo = (PTFileInfo)&++liIter );

      for( ; int(liIter); FiInfo = (PTFileInfo)&++liIter )
	ListMenuFiles.detach( *FiInfo, TShouldDelete::Delete );
    }

   SetMenu();
 }


TFileInfo  *TFileManeger::RegisterFile( LPSTR LpName, TDivStyle DsDiv )
 {

   //TFileInfo( LPSTR LpFNameFull, int IdFile, UINT CCopy, int FMode );
   int   Id = 1;
   UINT  CCopy = 1;
   BOOL  FlReady = FALSE;

   while( FlReady == FALSE )
    {
      ListIterator liIter = ListIterator( ListFiles );
      PTFileInfo FiInfo = (PTFileInfo)&liIter.current();

      FlReady = TRUE;
      for( ; int(liIter); FiInfo = (PTFileInfo)&++liIter )
	if( FiInfo->IdFile == Id )
	 {
	   Id++; FlReady = FALSE; break;
	 }
    }  



   CCopy = CheckDuplicates( LpName );

   TFileInfo *FiTmp = new TFileInfo( LpName, Id, CCopy, DsDiv );
   if( !FiTmp )
    {
      Status = EM_OUTOFMEMORY; return NULL;
    }
   if( FiTmp->Status )
    {
      Status = FiTmp->Status;
      delete FiTmp;
      return NULL;
    }

   ListFiles.add( *FiTmp );


   return FiTmp;
 }

UINT TFileManeger::CheckDuplicates( LPSTR LpName )
 {

   UINT CCopy = 1;

   ListIterator liIter = ListIterator( ListFiles );
   PTFileInfo FiInfo = (PTFileInfo)&liIter.current();

   for( ; int(liIter); FiInfo = (PTFileInfo)&++liIter )
     if( !lstrcmpi(LpName, FiInfo->LpFileNameFull)  &&
	 FiInfo->CCopy >= CCopy )
      {
	CCopy = FiInfo->CCopy + 1;
	pOwner->ForEach( ChangeTypeOfTitle, FiInfo );
      }

   return CCopy;
 }

PTFileInfo  TFileManeger::NewFile( TDivStyle DsDiv )
 {
   CNewFiles++;
   char Buf[128];

   wsprintf( Buf, "New-%u.3dw", CNewFiles );

   return RegisterFile( Buf, DsDiv );
 }

TFileInfo *TFileManeger::GetInfo( int Id ) const
 {
   ListIterator liIter = ListIterator( ListFiles );
   PTFileInfo FiInfo = (PTFileInfo)&liIter.current();   

   for( ; int(liIter); FiInfo = (PTFileInfo)&++liIter )
     if( FiInfo->IdFile == Id ) return FiInfo;

   return NULL;
 }

void TFileManeger::ChangeDivStyle( int Id, TDivStyle DsNewDiv )
 {
   TFileInfo *FiInfo = GetInfo( Id );

   if( FiInfo ) FiInfo->DsDivScreen = DsNewDiv;
 }


void TFileManeger::AddMenuFile( RTFileInfo FiInfo )
 {

   ListIterator liIter = ListIterator( ListMenuFiles );
   PTFileInfo FiInfoLast = (PTFileInfo)&liIter.current(), pKey;
   for( ; int(liIter); pKey = (PTFileInfo)&liIter.current(),
		       FiInfoLast = (PTFileInfo)&++liIter )
     if( !lstrcmpi(FiInfoLast->LpFileNameFull, FiInfo.LpFileNameFull) )
       goto RET;


   if( ListMenuFiles.getItemsInContainer() > MaxDeepOfMenuList )
     ListMenuFiles.detach( *pKey, TShouldDelete::Delete );

   ListMenuFiles.add( FiInfo );    
   SetMenu();

RET:
  UnHold( &FiInfo );
/*  if( FiInfo.lpNameHold )
   {
     OFSTRUCT of;
     OpenFile( FiInfo.lpNameHold, &of, OF_DELETE );

     delete [](FiInfo.lpNameHold);
     FiInfo.lpNameHold = NULL;
   }*/
 }


void TFileManeger::UnHold( int id )
 {
   PTFileInfo fiInfo = GetInfo( id );
   if( !fiInfo ) return;

   UnHold( fiInfo );
 }

void  TFileManeger::UnHold( PTFileInfo fiInfo )
 {
   if( !fiInfo ) return;

   if( fiInfo->lpNameHold )
   {
     OFSTRUCT of;
     OpenFile( fiInfo->lpNameHold, &of, OF_DELETE );

     delete [](fiInfo->lpNameHold);
     fiInfo->lpNameHold = NULL;
   }
 }

void TFileManeger::CloseFile( int Id )
 {
   TFileInfo  *pFileInf = GetInfo( Id );

   if( !pFileInf ) return;

   TConnect connect( pFileInf, this );
   ListFiles.detach( *pFileInf, TShouldDelete::NoDelete );
   AddMenuFile( *pFileInf );

   if( CheckDuplicates(pFileInf->LpFileNameFull) == 1 )
     pOwner->ForEach( ChangeTypeOfTitle_, &connect );

 }

int TFileManeger::ChangeName( int Id, LPSTR LpNewFullName )
 {
   TFileInfo  *pFileInf = GetInfo( Id );
   if( !pFileInf ) return -1;

   TFileInfo  *pNewFileInf = new TFileInfo( LpNewFullName, Id,
     CheckDuplicates( LpNewFullName ), pFileInf->DsDivScreen );

   if( !pNewFileInf ) { Status = EM_OUTOFMEMORY; return -1; }
   if( pNewFileInf->Status ) { delete pNewFileInf; return -1; }

   ListFiles.detach( *pFileInf, TShouldDelete::Delete ); //List
   ListFiles.add( *pNewFileInf );  

   pOwner->ForEach( ChangeTitle, pNewFileInf );

   return 0;
 }

LPSTR  TFileManeger::GetFileNameMenu( int IdWindows )
 {
   int Index = IdWindows - UCM_FILEACESS_FIRST;
       
   if( Index < 0 ) return NULL;

   ListIterator liIter = ListIterator( ListMenuFiles );
   PTFileInfo FiInfo = (PTFileInfo)&liIter.current();

   for( int c = 0; int(liIter) && c < Index; FiInfo = (PTFileInfo)&++liIter, c++ );

   if( !int(liIter) ) return NULL;

   return FiInfo->LpFileNameFull;
 }

PTFileInfo TFileManeger::ToggleFlAdaptDiv( int id )
 {
   PTFileInfo pInfo = GetInfo( id );
   if( !pInfo ) return NULL;

   pInfo->bFlWndGrowAsParent = (pInfo->bFlWndGrowAsParent == TRUE) ?
			       FALSE:TRUE;
   return pInfo;
 }

#pragma option -v.
