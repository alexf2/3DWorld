#pragma option -v-

#include "windiv.hpp"
#include "ucs.hpp"

#include <mem.h>



void TMyRect::GetDiv( double TailV, double TailH )
 {   
   right = double(left) + double(right - left) / TailV;
   bottom = double(top) + double(bottom - top) / TailH;   
 }


void TMyRect::ShiftRectOnRect( double FactorV, double FactorH )
 {   
   double Width = right - left;
   double Height = bottom - top;
   
   left += Width * FactorV; right += Width * FactorV;
   top += Height * FactorH; bottom += Height * FactorH;
 }

TWindowDivision::TWindowDivision( const RECT& RtWin, const RTWindowDivision rObj )
 {
   *this = TWindowDivision( RtWin, rObj.Style );

   PTMyRect pmrRect;
   for( int i = 0; (pmrRect = rObj[i]); i++ )
     MrtRects[i].VpVPoint = pmrRect->VpVPoint;
 }

TWindowDivision::TWindowDivision( const RECT& RtWin, TDivStyle Style )
 {

   CRects = 0;
   TMyRect  RtTmp;
   this->Style = Style;
   keyRect = RtWin;
 //    enum TViewPoint { VP_Top = 0, VP_Left, VP_Front, VP_Bottom, VP_Right, VP_Back,
 // VP_User, VP_Camera, VP_None };

   switch( Style )
    {
      case DS_FullWin:
	CRects = 1;
	MrtRects[0] = TMyRect( RtWin, TUCS::VP_User );
	break;

      case DS_HalfVert:
	RtTmp = TMyRect( RtWin, TUCS::VP_None );
	CRects = 2;
	RtTmp.GetDiv( 2, 1 );
	MrtRects[0] = TMyRect( RtTmp, TUCS::VP_Left );
	RtTmp.ShiftRectOnRect( 1, 0 );
	MrtRects[1] = TMyRect( RtTmp, TUCS::VP_Right );
	break;

      case DS_HalfHor:
	RtTmp = TMyRect( RtWin, TUCS::VP_None );
	CRects = 2;
	RtTmp.GetDiv( 1, 2 );
	MrtRects[0] = TMyRect( RtTmp, TUCS::VP_Left );
	RtTmp.ShiftRectOnRect( 0, 1 );
	MrtRects[1] = TMyRect( RtTmp, TUCS::VP_Right );
	break;

      case DS_4:
	RtTmp = TMyRect( RtWin, TUCS::VP_None );
	CRects = 4;
	RtTmp.GetDiv( 1, 2 );
	RtTmp.GetDiv( 2, 1 );
	MrtRects[0] = TMyRect( RtTmp, TUCS::VP_Left );

	RtTmp.ShiftRectOnRect( 1, 0 );
	MrtRects[1] = TMyRect( RtTmp, TUCS::VP_Front );

	RtTmp.ShiftRectOnRect( 0, 1 );
	MrtRects[2] = TMyRect( RtTmp, TUCS::VP_Bottom );

	RtTmp.ShiftRectOnRect( -1, 0 );
	MrtRects[3] = TMyRect( RtTmp, TUCS::VP_User );
	break;

      case DS_HalfVertAndLeftHor:
	RtTmp = TMyRect( RtWin, TUCS::VP_None );
	CRects = 3;
	RtTmp.GetDiv( 2, 1 );
	RtTmp.ShiftRectOnRect( 1, 0 );
	MrtRects[2] = TMyRect( RtTmp, TUCS::VP_Left );

	RtTmp.GetDiv( 1, 2 );
	RtTmp.ShiftRectOnRect( -1, 0 );
	MrtRects[0] = TMyRect( RtTmp, TUCS::VP_Front );

	RtTmp.ShiftRectOnRect( 0, 1 );
	MrtRects[1] = TMyRect( RtTmp, TUCS::VP_Top );
	break;

      case DS_HalfHorAndDownVert:
	RtTmp = TMyRect( RtWin, TUCS::VP_None );
	CRects = 3;
	RtTmp.GetDiv( 1, 2 );
	MrtRects[0] = TMyRect( RtTmp, TUCS::VP_Left );

	RtTmp.GetDiv( 2, 1 );
	RtTmp.ShiftRectOnRect( 0, 1 );
	MrtRects[1] = TMyRect( RtTmp, TUCS::VP_Front );
	
	RtTmp.ShiftRectOnRect( 1, 0 );
	MrtRects[2] = TMyRect( RtTmp, TUCS::VP_Top );
	break;

      case DS_HalfVertAndRightHor:  
	RtTmp = TMyRect( RtWin, TUCS::VP_None);
	CRects = 3;
	RtTmp.GetDiv( 2, 1 );
	MrtRects[0] = TMyRect( RtTmp, TUCS::VP_Front );

	RtTmp.GetDiv( 1, 2 );
	RtTmp.ShiftRectOnRect( 1, 0 );
	MrtRects[1] = TMyRect( RtTmp, TUCS::VP_Left );

	RtTmp.ShiftRectOnRect( 0, 1 );
	MrtRects[2] = TMyRect( RtTmp, TUCS::VP_Top );
	break;
				   
      case DS_HalfHorAndUpVert:
	RtTmp = TMyRect( RtWin, TUCS::VP_None );
	CRects = 3;
	RtTmp.GetDiv( 1, 2 );
	RtTmp.ShiftRectOnRect( 0, 1 );
	MrtRects[2] = TMyRect( RtTmp, TUCS::VP_Left );

	RtTmp.GetDiv( 2, 1 );
	RtTmp.ShiftRectOnRect( 0, -1 );
	MrtRects[0] = TMyRect( RtTmp, TUCS::VP_Front );

	RtTmp.ShiftRectOnRect( 1, 0 );
	MrtRects[1] = TMyRect( RtTmp, TUCS::VP_Top );
	break;

      case DS_3Vert:
        RtTmp = TMyRect( RtWin, TUCS::VP_None );
	CRects = 3;
	RtTmp.GetDiv( 3, 1 );
	MrtRects[0] = TMyRect( RtTmp, TUCS::VP_Left );

	RtTmp.ShiftRectOnRect( 1, 0 );
	MrtRects[1] = TMyRect( RtTmp, TUCS::VP_Front );

	RtTmp.ShiftRectOnRect( 1, 0 );
	MrtRects[2] = TMyRect( RtTmp, TUCS::VP_Top );
	break;

      case DS_3Hor:
	RtTmp = TMyRect( RtWin, TUCS::VP_None );
	CRects = 3;
	RtTmp.GetDiv( 1, 3 );
	MrtRects[0] = TMyRect( RtTmp, TUCS::VP_Left );

	RtTmp.ShiftRectOnRect( 0, 1 );
	MrtRects[1] = TMyRect( RtTmp, TUCS::VP_Front );

	RtTmp.ShiftRectOnRect( 0, 1 );
	MrtRects[2] = TMyRect( RtTmp, TUCS::VP_Top );
	break;

      case DS_FullAndLeft3:        
        RtTmp = TMyRect( RtWin, TUCS::VP_None );
	CRects = 4;
	RtTmp.ShiftRectOnRect( 1.0/3.0, 0 );
	RtTmp.GetDiv( 3.0/2.0, 1 );
	MrtRects[3] = TMyRect( RtTmp, TUCS::VP_Left );

	RtTmp = TMyRect( RtWin, TUCS::VP_None );
	RtTmp.GetDiv( 3, 3 );
	MrtRects[0] = TMyRect( RtTmp, TUCS::VP_Front );

	RtTmp.ShiftRectOnRect( 0, 1 );
	MrtRects[1] = TMyRect( RtTmp, TUCS::VP_Top );

	RtTmp.ShiftRectOnRect( 0, 1 );
	MrtRects[2] = TMyRect( RtTmp, TUCS::VP_User );
	break;

      case DS_FullAndRight3:
	RtTmp = TMyRect( RtWin, TUCS::VP_None );
	CRects = 4;
	RtTmp.GetDiv( 3.0/2.0, 1 );	
	MrtRects[0] = TMyRect( RtTmp, TUCS::VP_Left );

	RtTmp = TMyRect( RtWin, TUCS::VP_None );
	RtTmp.GetDiv( 3, 3 );
	RtTmp.ShiftRectOnRect( 2, 0 );
	MrtRects[1] = TMyRect( RtTmp, TUCS::VP_Front );

	RtTmp.ShiftRectOnRect( 0, 1 );
	MrtRects[2] = TMyRect( RtTmp, TUCS::VP_Top );

	RtTmp.ShiftRectOnRect( 0, 1 );
	MrtRects[3] = TMyRect( RtTmp, TUCS::VP_User );
	break;

    }

 }

TMyRect *TWindowDivision::operator[]( int Index )
 {
   if( Index >= CRects || Index < 0 ) return NULL;
   return &MrtRects[Index];
 }

RTWindowDivision TWindowDivision::operator=( RTWindowDivision wdObj )
 {
   _fmemcpy( this, &wdObj, sizeof(TWindowDivision) );
   return *this;
 }

#pragma option -v.
