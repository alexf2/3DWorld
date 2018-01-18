#include "log.hpp"

#include <stdio.h>
#include <time.h>
#include <string.h>


static LPSTR near pascal ConvertToStr( DWORD dwTimeMs, LPSTR lpTime )
 {
   DWORD dwSec = dwTimeMs / 1000L;
   DWORD dwMin = dwSec / 60L;
   DWORD dwH = dwMin / 60L;

   dwSec = dwSec % 60L;
   dwMin = dwMin % 60L;

   sprintf( lpTime, "%2d:%2d:%2d", int(dwH), int(dwMin), int(dwSec) );

   return lpTime;
 }


int TimeLog::Save( HFILE hf )
 {
#define MAKE_STR( txt, var )  (sprintf( cBuf, "   %-25s:  %s\r\n", (LPSTR)txt, ConvertToStr(var, cTime) ));

   char cBuf[256];
   char cTime[50];

   MAKE_STR( "Copy scena", dwCopy )
   _lwrite( hf, cBuf, _fstrlen(cBuf) );
   MAKE_STR( "Transforming objects", dwTransform )
   _lwrite( hf, cBuf, _fstrlen(cBuf) );
   MAKE_STR( "Optimize data", dwOptimize )
   _lwrite( hf, cBuf, _fstrlen(cBuf) );
   MAKE_STR( "Init backgraund", dwInitBkGnd )
   _lwrite( hf, cBuf, _fstrlen(cBuf) );
   MAKE_STR( "Prepare render", dwPrepareRender )
   _lwrite( hf, cBuf, _fstrlen(cBuf) );
   MAKE_STR( "Create shadows data", dwCopyShadows )
   _lwrite( hf, cBuf, _fstrlen(cBuf) );

   MAKE_STR( "Render", dwRender )
   _lwrite( hf, cBuf, _fstrlen(cBuf) );

   MAKE_STR( "Calc palette", dwCalcPalette )
   _lwrite( hf, cBuf, _fstrlen(cBuf) );

   sprintf( cBuf, "---------------------------------------\r\n" );
   _lwrite( hf, cBuf, _fstrlen(cBuf) );
   MAKE_STR( "Total time", dwCopy + dwTransform + dwOptimize + dwInitBkGnd +
     dwPrepareRender + dwCopyShadows + dwRender + dwCalcPalette )
   _lwrite( hf, cBuf, _fstrlen(cBuf) );

   sprintf( cBuf, "\r\n" );
   _lwrite( hf, cBuf, _fstrlen(cBuf) );

   return 0;
 }

void TimeLog::StartProcess( DWORD& rdwT )
 {
   rdwT = GetTickCount();
 }

void TimeLog::EndProcess( DWORD& rdwT )
 { 
   rdwT = GetTickCount() - rdwT;
 }

void TimeLog::Reset()
 {
   dwCopy = dwTransform = dwOptimize = dwInitBkGnd = dwPrepareRender =
   dwCopyShadows = dwRender = dwCalcPalette = 0;
 }

RTimeLog TimeLog::operator+=( RTimeLog rL )
 {
   dwCopy += rL.dwCopy;
   dwTransform += rL.dwTransform;
   dwOptimize += rL.dwOptimize;
   dwInitBkGnd += rL.dwInitBkGnd;
   dwPrepareRender += rL.dwPrepareRender;
   dwCopyShadows += rL.dwCopyShadows;
   dwRender += rL.dwRender;
   dwCalcPalette += rL.dwCalcPalette;

   return *this;
 }

