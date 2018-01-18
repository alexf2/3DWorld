#if !defined(_FILE_MANEGER_)
#define _FILE_MANEGER_

#include <owl.h>
#include <list.h>
#include <memmgr.h>

#include "INC\errors.inc"
#include "windiv.hpp"

#define TYP_FILE_INFO 100

_CLASSDEF(TFileInfo)

class   TFileInfo: public Object
 {

public:
  TFileInfo( LPSTR LpFNameFull, int IdFile, UINT CCopy, TDivStyle DsDiv );
  TFileInfo( TFileInfo& );
  ~TFileInfo();

  virtual classType isA() const { return TYP_FILE_INFO; }
  virtual hashValueType hashValue() const { return 0; }
  virtual int isEqual( const Object& ) const;
  virtual char *nameOf() const { return "File_Info"; }
  virtual void printOn( ostream& ) const {};

  static void SetMaxLShortFName( int Len ) { MaxLenShortFName = Len; }
   
  LPSTR  LpFileNameFull, LpFileName, LpFileNameShort;
  LPSTR  lpNameHold;
  int    IdFile;
  UINT   CCopy;
  TDivStyle DsDivScreen;
  BOOL   bFlWndGrowAsParent;

  int Status;


  void _FAR* operator new( size_t size );
  void operator delete( void _FAR* pToDel );

private:
  static int MaxLenShortFName;
  void CreateFName( LPSTR LpFullName );
  void CreateShortName( LPSTR LpFullName );


  static MemBlocks *mgr;
  static long countMemMgr;
 };


_CLASSDEF(TFileManeger)

class  TFileManeger
 {

public:
  TFileManeger( PTWindowsObject pMDIOwner, int DeepOfMenuList );
  ~TFileManeger();

  void SetMaxDeep( int DeepOfMenuList );
  int GetMaxDeep( void ) const { return MaxDeepOfMenuList; }

  TFileInfo  *RegisterFile( LPSTR LpName, TDivStyle DsDiv );
  TFileInfo  *NewFile( TDivStyle DsDiv );
  void CloseFile( int Id );

  int ChangeName( int Id, LPSTR LpNewFullName );
  void ChangeDivStyle( int Id, TDivStyle DsNewDiv );
  TFileInfo *GetInfo( int Id ) const;

  LPSTR  GetFileNameMenu( int IdWindows );
  UINT CheckDuplicates( LPSTR LpName );       //return yuor copy number

  PTFileInfo ToggleFlAdaptDiv( int id );

  LPSTR GetHoldName( int id );

  void UnHold( int id );
  void UnHold( PTFileInfo fiInfo );


  int Status;

private:
  List  ListFiles;
  List  ListMenuFiles;
  int   MaxDeepOfMenuList;
  int   CNewFiles;

  //TApplication *AppOwner;

  void SetMenu();
  void AddMenuFile( TFileInfo& FiInfo  );

  PTWindowsObject pOwner;

 };

struct TConnect
 {
   TConnect( PTFileInfo pInf, PTFileManeger pMan )
    {
      this->pInf = pInf; this->pMan = pMan;
    }

   PTFileInfo pInf;
   PTFileManeger pMan;
 };


#endif
