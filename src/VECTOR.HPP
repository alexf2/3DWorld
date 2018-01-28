#if !defined(_VECTOR_)
#define _VECTOR_

#include <math.h>
#include <owl.h>

_CLASSDEF(TVector)

TVector Normalyze( RTVector );
TVector RndVector();

class  TVector
 {
public:
  double x, y, z;

  TVector() { x = y = z = 0; }
  TVector( double v ) { x = y = z = v; }
  TVector( RTVector v ) { x = v.x; y = v.y; z = v.z; }
  TVector( double vx, double vy, double vz )
   {
     x = vx; y = vy; z = vz;
   }

  TVector operator=( double v )
   {
     x = y = z = v; return *this;
   }
  //TVector operator=( RTVector v );
  TVector operator=( TVector v );
  TVector operator+( RTVector v );
  TVector operator-( RTVector v );
  TVector operator-();
  TVector operator*( double );
  TVector operator*( RTVector );
  TVector operator/( double );
  RTVector operator+=( RTVector );
  RTVector operator-=( RTVector );
  RTVector operator*=( double );
  RTVector operator*=( RTVector );
  RTVector operator/=( double );

  double operator&( RTVector v )
   {
     return x*v.x + y*v.y + z*v.z;
   }

  TVector operator^( RTVector );
  double operator!()
   {
     return sqrt( x*x + y*y + z*z );
   }
  double operator[]( int n ) { return *(&x + n); }
  int operator<( double v )
   {
     return x<v && x>-v && y<v && y>-v && z<v && z>-v;  
   }
  int operator>( double v )
   {
     return x<-v || x>v || y<-v || y>v || z<-v || z>v;
   }

 };

/*inline TVector TVector::operator=( RTVector v )
 {
   x = v.x; y = v.y; z = v.z; return *this;
 }*/

inline TVector TVector::operator=( TVector v )
 {
   x = v.x; y = v.y; z = v.z; return *this;
 }


inline TVector TVector::operator+( RTVector v )
 {
   return TVector( x+v.x, y+v.y, z+v.z );
 }

inline TVector TVector::operator-( RTVector v )
 {
   return TVector( x-v.x, y-v.y, z-v.z );
 }

inline TVector TVector::operator-()
 {
   return TVector( -x, -y, -z );
 }

inline TVector TVector::operator*( double v )
 {
   return TVector( x*v, y*v, z*v );
 }

inline TVector TVector::operator*( RTVector v )
 {
   return TVector( x*v.x, y*v.y, z*v.z );
 }

inline TVector TVector::operator/( double v )
 {
   return TVector( x/v, y/v, z/v );
 }

inline RTVector TVector::operator+=( RTVector v )
 {
   x += v.x;
   y += v.y;
   z += v.z;

   return *this;
 }

inline RTVector TVector::operator-=( RTVector v )
 {
   x -= v.x;
   y -= v.y;
   z -= v.z;

   return *this;
 }

inline RTVector TVector::operator*=( double v )
 {
   x *= v;
   y *= v;
   z *= v;

   return *this;
 }

inline RTVector TVector::operator*=( RTVector v )
 {
   x *= v.x;
   y *= v.y;
   z *= v.z;

   return *this;
 }

inline RTVector TVector::operator/=( double v )
 {
   x /= v;
   y /= v;
   z /= v;

   return *this;
 }

inline TVector VecMul( RTVector v1, RTVector v2 )
 {
   return TVector( v1.y*v2.z - v2.y*v1.z,
		   v2.x*v1.z - v1.x*v2.z,
		   v1.x*v2.y - v2.x*v1.y
		 );
 }


inline TVector TVector::operator^( RTVector v )
 {
   return TVector( y*v.z - z*v.y,
		   z*v.x - x*v.z,
		   x*v.y - y*v.x );
 }

#endif
