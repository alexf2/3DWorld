#if !defined(_WINDOW_DIVISION_)
#define _WINDOW_DIVISION_

#include <windows.h>
#include "ucs.hpp"
#include "INC\windiv.typ"

const int MAXRECT = 4;

typedef int TDivStyle;

struct TMyRect: public RECT
 {

public:
  TUCS::TViewPoint VpVPoint;

  TMyRect() {};
  TMyRect( const RECT& RtRect, TUCS::TViewPoint VpView )
   {
     VpVPoint = VpView;
     left   = RtRect.left;
     top    = RtRect.top;
     bottom = RtRect.bottom;
     right  = RtRect.right;
   }

  void GetDiv( double TailV, double TailH );
  void ShiftRectOnRect( double FactorV, double FactorH );

 };
typedef TMyRect *PTMyRect;

_CLASSDEF(TWindowDivision)

class  TWindowDivision
 {

public:
  RECT keyRect;

  TWindowDivision( const RECT& RtWin, TDivStyle Style );
  TWindowDivision( const RECT& RtWin, const RTWindowDivision rObj );

  CountRects() const { return CRects; }
  PTMyRect operator[]( int Index );
  RTWindowDivision operator=( RTWindowDivision wdObj );
  TDivStyle Style;

private:
  int CRects;
  TMyRect  MrtRects[ MAXRECT ];

 };

#endif
