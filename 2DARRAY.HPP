#if !defined(_T2DArray_)
#define _T2DArray_

#include <bwcc.h>

class TLineDef
 {
public:
  TLineDef( int ) {}

 };


template<class T> class T2DArray
 {
public:

//friend template<class T> T2DString operator[]( T2DArray<T>& rA, int ind );
//friend template<class T> T& operator[]( T2DString rS, int ind )

  T2DArray( int iClm, int iStr );
  T2DArray( const T2DArray& rObj )
   {
     Copy( rObj );
   }
  virtual ~T2DArray();

  int Clm() const { return iClm; }
  int Str() const { return iStr; }

  T2DArray& operator,( TLineDef );
  T2DArray& operator,( T );
  T** operator()() const { return (T**)pData; }
  T2DArray& operator=( const T2DArray& );

private:
  int iClm, iStr;
  T *pData;

  int iCountStr, iCountClm;

  void Copy( const T2DArray& rObj );
 };


template<class T> T2DArray<T>& operator=( const T2DArray<T>& rObj )
 {
   if( pData ) {delete pData; pData = NULL; }
   Copy( rObj );
   return *this;
 }

template<class T> void T2DArray<T>::Copy( const T2DArray<T>& rObj )
 {
   iClm = rObj.iClm; iStr = rObj.iStr;
   pData = new T[ iClm * iStr ];
   if( pData ) _fmemcpy( pData, rObj.pData, sizeof(T) * iClm * iStr );
 }

template<class T> T2DArray<T>& T2DArray<T>::operator,( TLineDef )
 {
   iCountStr++; iCountClm = 0;
   return *this;
 }

template<class T> T2DArray<T>& T2DArray<T>::operator,( T tElem )
 {
   if( iCountClm >= iClm || iCountStr >= iStr )
    {
      BWCCMessageBox( NULL, "Bounds error 2DArray", "Error", MB_ICONSTOP | MB_OK | MB_APPLMODAL );
      return *this;
    }

   pData[ iCountStr * iClm + iCountClm++ ] = tElem;
   return *this;
 }


/*template<class T> T& operator[]( T2DArray::T2DString rS, int ind )
 {
   return *(&rS + ind);
 }

template<class T> T2DArray<T>::T2DString operator[]( T2DArray<T>& rA, int ind )
 {
   return *(pData + iClm * ind);
 }*/


template<class T> T2DArray<T>::T2DArray( int iClm_, int iStr_ )
 {
   iClm = iClm_; iStr = iStr_;
   pData = new T[ iClm * iStr ];
   iCountStr = iCountClm = 0;
 }

template<class T> T2DArray<T>::~T2DArray()
 {
   if( pData ) { delete pData; pData = NULL; }
 }

#endif
