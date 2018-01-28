#if !defined(_DRAW_BUFFER_)
#define _DRAW_BUFFER_

#include <windows.h>

struct TDrawTitleBuf
 {
public:
  HBITMAP hbm;
  int w, h;

  TDrawTitleBuf();
  ~TDrawTitleBuf();

  void InitBuff( HDC hdc, int size, int high );
  void RequiestSize( HDC hdc, int size, int high );
 };


#endif
