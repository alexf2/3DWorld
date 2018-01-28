#if !defined(TAuto)
#define TAuto

#include <owl.h>


_CLASSDEF(TAutomat)

typedef int** TJmpTbl; //class, current state
/*struct TClassOfChar {
  int iLo, iHi;
 };*/
//typedef TClassOfChar* PTClassifyTbl; //last class is invalid
   

class  TAutomat
 {
public:
  enum TAutomatState { A_ErrorInit = 0, A_UnknownChar = 1, A_Start = 2, A_End = 3, A_Error = 4 };

  TAutomat( TJmpTbl pCTbl, TJmpTbl pTbl_, const int iNumbClass, const int iNumbState ):
    iNumC( iNumbClass ),
    iNumS( iNumbState )
   {
     lpCur = lpInputStr = NULL;
     pClassTbl = pCTbl;
     pTbl = pTbl_;
     iCurState = A_Start;
     iNextState = A_End;
   }

  int Work( LPSTR lpS ); //returned next state 

protected:
  virtual BOOL MakeJmp( int iCurrentState, int iNextState );
  //prepare jmp and return exit flag

private:
  TJmpTbl pTbl;
  TJmpTbl pClassTbl;
  const int iNumC, iNumS;


  int iCurState, iNextState;
  LPSTR lpInputStr, lpCur;

  int GetClass() const;    
 };


#endif
