#pragma option -v-

#include "polygon.hpp"
#include "ucs.hpp"


TPolygon::TPolygon( int x, int y, int rad, int sides )
 {
   sidesNumb = sides;
   pPoint = new POINT[sides];
   POINT FAR *pCur = pPoint;
   double angleStep = GRAD_TO_RAD(360.0 / double(sides));
   double angle = 0;

   for( int i = 0; i < sides; i++, pCur++ )
    {
      double curX = double(rad) * cos(angle),
	     curY = double(rad) * sin(angle);

      pCur->x = curX + x; pCur->y = curY + y;
      angle += angleStep;
    }
 }

TPolygon::~TPolygon()
 {
   if( pPoint ) delete []pPoint;
   pPoint = NULL;
 }