#if !defined(_TSEQVENSOR_)
#define _TSEQVENSOR_

#include <owldefs.h>
#include <windows.h>

_CLASSDEF(TSeqvensor)
_CLASSDEF(TRotExisSeqvensor)
_CLASSDEF(TScaleAxisSeqvensor)
_CLASSDEF(TMoveAxisSeqvensor)

class  TSeqvensor
 {
public:

  TSeqvensor( int iCurVal, int iF, int iL )
   {
     iCurrentValue = iCurVal;
     iFirst = iF; iLast = iL; 
   }

  int GetCurrent() const { return iCurrentValue; }
  int GetFirst() const { return iFirst; }
  int GetLast() const { return iLast; }

  int operator++();
  int operator++( int );
  int operator--();
  int operator--( int );

  virtual LPCSTR operator()() const = 0;

private:
  int iCurrentValue, iFirst, iLast;
 };


class  TRotExisSeqvensor: public TSeqvensor
 {
public:
  enum RotExis { AX_x = 0, AX_y, AX_z };

  TRotExisSeqvensor(): TSeqvensor( TRotExisSeqvensor::AX_x,
				   TRotExisSeqvensor::AX_x,
				   TRotExisSeqvensor::AX_z )
   {};

  virtual LPCSTR operator()() const;
 };


class  TScaleAxisSeqvensor: public TSeqvensor
 {
public:
  enum TScaleAxis { SA_3d, SA_x, SA_y, SA_z };

  TScaleAxisSeqvensor(): TSeqvensor( TScaleAxisSeqvensor::SA_3d,
				     TScaleAxisSeqvensor::SA_3d,
				     TScaleAxisSeqvensor::SA_z )
   {};

  virtual LPCSTR operator()() const;
 };


class  TMoveAxisSeqvensor: public TSeqvensor
 {
public:
  enum TMoveAxis { MA_xy, MA_x, MA_y, MA_z };

  TMoveAxisSeqvensor(): TSeqvensor( TMoveAxisSeqvensor::MA_xy,
				    TMoveAxisSeqvensor::MA_xy,
				    TMoveAxisSeqvensor::MA_z )
   {};

  virtual LPCSTR operator()() const;
 };



#endif
