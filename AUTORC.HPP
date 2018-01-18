#if !defined(TAUTORC)
#define TAUTORC

#include <owl.h>
#include <alloc.h>
#include <lzexpand.h>


#undef _EXPORT2
#if defined(_AUTORCDLL) || defined(_ALLLIBDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif

#undef _EXPFUNC2
#if defined(_AUTORCDLL) || defined(_ALLLIBDLL)
  #define _EXPFUNC2 _export
#else
  #define _EXPFUNC2 _EXPFUNC
#endif


template<class T> class  _EXPORT2  TAutoRc
 {
public:

  TAutoRc() {};
  TAutoRc( T tRc_ ) { tRc = tRc_; }
  virtual ~TAutoRc() {};

  TAutoRc& operator=( TAutoRc& rRc )
   {
     FreeRc();
     tRc = CopyRc( rRc.tRc );
     return *this;
   }

  virtual void FreeRc() = 0;
  int operator!() { return !tRc; }
  T operator()() const { return tRc; }
  
  virtual T CopyRc( T& rT ) const
   {
     return rT;
   }

protected:
  T tRc;
 };


template<class T> class _EXPORT2 TAutoGDIRc: public TAutoRc<T>
 {
public:
  TAutoGDIRc() { tRc = NULL; }
  TAutoGDIRc( T tRc_ ): TAutoRc<T>( tRc_ ) {}

  virtual ~TAutoGDIRc()
   {
     if( !tRc ) return;
     DeleteObject( tRc ); tRc = NULL;
   }

   TAutoGDIRc& operator=( T tRc_ )
   {
     FreeRc();
     tRc = tRc_;
     return *this;
   }

  virtual void FreeRc();
 };

template<class T> void TAutoGDIRc<T>::FreeRc()
 {
   if( tRc ) { DeleteObject( tRc ); tRc = NULL; }
 }


class _EXPORT2 TAutoCursorRc: public TAutoRc<HCURSOR>
 {
public:
  TAutoCursorRc() { tRc = NULL; }
  TAutoCursorRc( HCURSOR tRc_ ): TAutoRc<HCURSOR>( SetCursor(tRc_) ) {}

  virtual ~TAutoCursorRc()
   {
     if( !tRc ) return;
     SetCursor( tRc ); tRc = NULL;
   }

   TAutoCursorRc& operator=( HCURSOR tRc_ )
   {
     FreeRc();
     tRc = SetCursor( tRc_ );
     return *this;
   }

  virtual void FreeRc();
 };


template<class T> class _EXPORT2  TAutoMemoryRc: public TAutoRc<T>
 {
public:
  TAutoMemoryRc() { tRc = NULL; }
  TAutoMemoryRc( T tRc_ ): TAutoRc<T>( tRc_ ) {}

  virtual ~TAutoMemoryRc()
   {
     if( !tRc ) return;
     delete tRc; tRc = NULL;
   }

  TAutoMemoryRc& operator=( TAutoMemoryRc& rRc )
   {
     FreeRc();
     tRc = CopyRc( rRc.tRc );
     return *this;
   }


  TAutoMemoryRc& operator=( T tRc_ )
   {
     FreeRc();
     tRc = tRc_;
     return *this;
   }

  virtual void FreeRc();

  void ClearRc() { tRc = NULL; }
 };

template<class T> void TAutoMemoryRc<T>::FreeRc()
 {
   if( tRc ) { delete tRc; tRc = NULL; }
 }


template<class T> class _EXPORT2  TAutoMemoryHugeRc: public TAutoRc<T>
 {
public:
  TAutoMemoryHugeRc() { tRc = NULL; }
  TAutoMemoryHugeRc( T tRc_ ): TAutoRc<T>( tRc_ ) {}

  virtual ~TAutoMemoryHugeRc()
   {
     if( !tRc ) return;
     farfree(tRc); tRc = NULL;
   }

  TAutoMemoryHugeRc& operator=( TAutoMemoryHugeRc& rRc )
   {
     FreeRc();
     tRc = CopyRc( rRc.tRc );
     return *this;
   }


  TAutoMemoryHugeRc& operator=( T tRc_ )
   {
     FreeRc();
     tRc = tRc_;
     return *this;
   }

  void ClearRc() { tRc = NULL; }

  virtual void FreeRc();
 };

template<class T> void TAutoMemoryHugeRc<T>::FreeRc()
 {
   if( tRc ) { farfree(tRc); tRc = NULL; }
 }



template<class T> class _EXPORT2  TAutoFileRc: public TAutoRc<T>
 {
public:
  TAutoFileRc() { tRc = 0; }
  TAutoFileRc( T tRc_ ): TAutoRc<T>( tRc_ ) {}

  virtual ~TAutoFileRc()
   {
     if( tRc == HFILE_ERROR || !tRc ) return;
     _lclose( tRc ); tRc = 0;
   }

  TAutoFileRc& operator=( T tRc_ )
   {
     FreeRc();
     tRc = tRc_;
     return *this;
   }

   virtual void FreeRc();
 };

template<class T> void TAutoFileRc<T>::FreeRc()
 {
   if( tRc ) { _lclose( tRc ); tRc = 0; }
 }


template<class T> class _EXPORT2  TAutoFileZRc: public TAutoRc<T>
 {
public:
  TAutoFileZRc() { tRc = 0; }
  TAutoFileZRc( T tRc_ ): TAutoRc<T>( tRc_ ) {}

  virtual ~TAutoFileZRc()
   {
     if( tRc == HFILE_ERROR || !tRc ) return;
     LZClose( tRc ); tRc = 0;
   }

  TAutoFileZRc& operator=( T tRc_ )
   {
     FreeRc();
     tRc = tRc_;
     return *this;
   }


   virtual void FreeRc();
 };

template<class T> void TAutoFileZRc<T>::FreeRc()
 {
   if( tRc ) { LZClose( tRc ); tRc = 0; }
 }


class _EXPORT2  TAutoStringRc: public TAutoMemoryRc<char*>
 {
public:
  TAutoStringRc() { tRc = NULL; }
  TAutoStringRc( HINSTANCE hInst, int id, int iSize = 128 );
  TAutoStringRc( char* );
  virtual ~TAutoStringRc();

  char* operator()() const;

  virtual char* CopyRc( char*& rT ) const;
 };


#endif
