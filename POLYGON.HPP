#if !defined(_POLYGON_)
#define _POLYGON_

#include <owl.h>

_CLASSDEF(TPolygon)

class  TPolygon
 {
public:
  POINT FAR *pPoint;

  TPolygon( int x, int y, int rad, int sides );
  ~TPolygon();
  int GetNumbVert() const { return sidesNumb; }

private:
  int sidesNumb;
 };
                                       

#endif
