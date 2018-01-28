#pragma option -v-

#include "seqv.hpp"

int TSeqvensor::operator++()
 {
   int iRet = iCurrentValue;
   if( iCurrentValue < iLast ) iCurrentValue++;
   else iCurrentValue = iFirst;

   return iRet;
 }

int TSeqvensor::operator++( int )
 {   
   if( iCurrentValue < iLast ) iCurrentValue++;
   else iCurrentValue = iFirst;

   return iCurrentValue;
 }

int TSeqvensor::operator--()
 {
   int iRet = iCurrentValue;
   if( iCurrentValue > iFirst ) iCurrentValue--;
   else iCurrentValue = iLast;

   return iRet;
 }

int TSeqvensor::operator--( int )
 {   
   if( iCurrentValue > iFirst ) iCurrentValue--;
   else iCurrentValue = iLast;

   return iCurrentValue;
 }

LPCSTR TRotExisSeqvensor::operator()() const
 {
   static LPSTR lpLetters[] =
    {
      "X",
      "Y",
      "Z"
    };

   return lpLetters[ GetCurrent() - GetFirst() ];
 }

LPCSTR TScaleAxisSeqvensor::operator()() const
 {
   static LPSTR lpLetters[] =
    {
      "3D(isotropic)",
      "X(anisotropic)",
      "Y(anisotropic)",
      "Z(anisotropic)"
    };

   return lpLetters[ GetCurrent() - GetFirst() ];
 }

LPCSTR TMoveAxisSeqvensor::operator()() const
 {
   static LPSTR lpLetters[] =
    {
      "XY",
      "X",
      "Y",
      "Z"
    };

   return lpLetters[ GetCurrent() - GetFirst() ];
 }
