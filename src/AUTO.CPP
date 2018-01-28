#include "auto.hpp"

int TAutomat::Work( LPSTR lpS )
 {
   iCurState = A_Start;
   iNextState = A_End;
   lpCur = lpInputStr = lpS;
   if( !lpS ) { iNextState = A_ErrorInit; return iNextState; }

   while( MakeJmp( iCurState, (iNextState = ((int*)pTbl)[GetClass() * iNumS + iCurState]) ) == FALSE )
    {
      iCurState = iNextState; lpCur++;
    }

   return iNextState;
 }

int TAutomat::GetClass() const
 {
   for( int i = 0; i < iNumC; i++ )
     if( int(*lpCur) >= ((int*)pClassTbl)[i * 2 + 0] && int(*lpCur) <= ((int*)pClassTbl)[i * 2 + 1] )
       return i;

   return iNumC - 1; //invalid class
 }

BOOL TAutomat::MakeJmp( int iCurrentState, int iNextState )
 {
   switch( iNextState )
    {
      case A_ErrorInit:
	return TRUE;
      case A_UnknownChar:
	return TRUE;
      case A_Start:
	return FALSE;
      case A_End:
	return TRUE;
      case A_Error:
	return TRUE;
      default:
	return FALSE;
    };
 }
