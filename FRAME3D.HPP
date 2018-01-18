#if !defined(_3DFRAME_)
#define  _3DFRAME_

#include <owl.h>
#include <window.h>


#undef _EXPORT2
#if defined(_3DFRAMEDLL) || defined(_ALLDLGDLL)
  #define _EXPORT2 _export
#else
  #define _EXPORT2 _CLASSTYPE
#endif


_CLASSDEF(T3DFrame)
_CLASSDEF(T3DThickFrame)

class _EXPORT2 T3DFrame: public TWindow
 {
public:
  T3DFrame( PTWindowsObject AParent, int iIdFramed_, PTModule AModule = NULL );

  virtual ~T3DFrame() {};

  virtual void InitSize();

protected:
  RECT rWin;
  int w, h;

  virtual void Paint( HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo );
  virtual LPSTR GetClassName();
  virtual void GetWindowClass( WNDCLASS & AWndClass );

  virtual int Dx() const { return 0; }
  virtual int Dy() const { return 0; }

private:
  int iIdFramed;
 };

class _EXPORT2 T3DThickFrame: public T3DFrame
 {
public:
  T3DThickFrame( PTWindowsObject AParent, int iIdFramed_, int iThick_,
		 PTModule AModule = NULL );

  virtual ~T3DThickFrame();
  virtual void InitSize();

protected:
  virtual void Paint( HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo );

  virtual int Dx() const { return iThick + 2; }
  virtual int Dy() const { return iThick + 2; }

private:
  int iThick;
  HRGN hrgEx;
  RECT rInt;
 };


#endif
