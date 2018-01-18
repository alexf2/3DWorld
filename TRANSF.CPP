#pragma option -v-

#include "surface.hpp"
#include "INC\errors.inc"
#include <stdio.h>
#include <values.h>



void TSurface::Move( double dx, double dy, double dz )
 {
   DoubleListIterator  dbIt = DoubleListIterator( listVertex, FALSE );
   PTVertex  pVert = (PTVertex)&dbIt.current();

   for( ; int(dbIt); pVert = (PTVertex)&--dbIt )
    {
      pVert->x += dx; pVert->y += dy; pVert->z += dz;
    }
 }

void TSurface::RotateZ( double angleRad, BOOL bFlFlipNormals )
 {
   DoubleListIterator  dbIt = DoubleListIterator( listVertex, FALSE );
   PTVertex  pVert = (PTVertex)&dbIt.current();

   for( ; int(dbIt); pVert = (PTVertex)&--dbIt )
    {
      double x = pVert->x, y = pVert->y, z = pVert->z;
      pVert->x = cos(angleRad) * x - sin(angleRad) * y;
      pVert->y = sin(angleRad) * x + cos(angleRad) * y;
    }

 }

void TSurface::RotateX( double angleRad, BOOL bFlFlipNormals  )
 {
   DoubleListIterator  dbIt = DoubleListIterator( listVertex, FALSE );
   PTVertex  pVert = (PTVertex)&dbIt.current();

   for( ; int(dbIt); pVert = (PTVertex)&--dbIt )
    {
      double x = pVert->x, y = pVert->y, z = pVert->z;
 
      pVert->y = y * cos(angleRad) - z * sin(angleRad);
      pVert->z = y * sin(angleRad) + z * cos(angleRad);
    }

 }

void TSurface::RotateY( double angleRad, BOOL bFlFlipNormals )
 {
   DoubleListIterator  dbIt = DoubleListIterator( listVertex, FALSE );
   PTVertex  pVert = (PTVertex)&dbIt.current();

   for( ; int(dbIt); pVert = (PTVertex)&--dbIt )
    {
      double x = pVert->x, y = pVert->y, z = pVert->z;
      pVert->x = x * cos(angleRad) + z * sin(angleRad);
      pVert->z = z * cos(angleRad) - x * sin(angleRad);
    }

 }

void TSurface::ScaleXYZ( double scX_, double scY_, double scZ_, T3DPoint center )
 {
   double scX = scX_ / 100.0,
	  scY = scY_ / 100.0,
	  scZ = scZ_ / 100.0;
   DoubleListIterator  dbIt = DoubleListIterator( listVertex, FALSE );
   PTVertex  pVert = (PTVertex)&dbIt.current();

   //T3DPoint pt = GetGeomCenter();

   for( ; int(dbIt); pVert = (PTVertex)&--dbIt )
    {
      double x = pVert->x, y = pVert->y, z = pVert->z;
      pVert->x = x * scX + (1 - scX) * center.x;
      pVert->y = y * scY + (1 - scY) * center.y;
      pVert->z = z * scZ + (1 - scZ) * center.z;
    }
 }

void TSurface::Scale2D( double sc_, T3DPoint center )
 {
   double sc = sc_ / 100.0;
   DoubleListIterator  dbIt = DoubleListIterator( listVertex, FALSE );
   PTVertex  pVert = (PTVertex)&dbIt.current();

   //T3DPoint pt = GetGeomCenter();

   for( ; int(dbIt); pVert = (PTVertex)&--dbIt )
    {
      double x = pVert->x, y = pVert->y, z = pVert->z;

      pVert->x = x * sc + (1 - sc) * center.x;
      pVert->y = y * sc + (1 - sc) * center.y;
    }

 }
#pragma option -v.
