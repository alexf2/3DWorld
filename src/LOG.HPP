#if !defined(_TIME_LOG_)
#define _TIME_LOG_

#include <owl.h>

_CLASSDEF(TimeLog)
class TimeLog
 {
public:
  TimeLog()
   {
     Reset();
   }

  void Reset();
  RTimeLog operator+=( RTimeLog );

  void StartCopy()
   {
     StartProcess( dwCopy );
   }
  void EndCopy()
   {
     EndProcess( dwCopy );
   }

  void StartTransform()
   {
     StartProcess( dwTransform );
   }
  void EndTransform()
   {
     EndProcess( dwTransform );
   }

  void StartOptimize()
   {
     StartProcess( dwOptimize );
   }
  void EndOptimize()
   {
     EndProcess( dwOptimize );
   }

  void StartInitBkGnd()
   {
     StartProcess( dwInitBkGnd );
   }
  void EndInitBkGnd()
   {
     EndProcess( dwInitBkGnd );
   }

  void StartPrepareRender()
   {
     StartProcess( dwPrepareRender );
   }
  void EndPrepareRender()
   {
     EndProcess( dwPrepareRender );
   }

  void StartCopyShadows()
   {
     StartProcess( dwCopyShadows );
   }
  void EndCopyShadows()
   {
     EndProcess( dwCopyShadows );
   }

  void StartRender()
   {
     StartProcess( dwRender );
   }
  void EndRender()
   {
     EndProcess( dwRender );
   }

  void StartCalcPalette()
   {
     StartProcess( dwCalcPalette );
   }
  void EndCalcPalette()
   {
     EndProcess( dwCalcPalette );
   }

  int Save( HFILE );

private:
  DWORD dwCopy,
	dwTransform,
	dwOptimize,
	dwInitBkGnd,
	dwPrepareRender,
	dwCopyShadows,

	dwRender,

	dwCalcPalette;

  void StartProcess( DWORD& );
  void EndProcess( DWORD& );
 };

#endif
