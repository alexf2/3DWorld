//#pragma option -v-

#include "threshol.hpp"
#include "ucs.hpp"
#include <windowsx.h>
#include <mem.h>



//    VP_Top, VP_Left, VP_Front, VP_Bottom, VP_Right, VP_Back,
//    VP_User, VP_Camera, VP_None


BOOL TGrid::bFlCheckRadian;
			    
const TViewParam TUCS::vpVpointsData[] =
   {                          //cos
     {  GRAD_TO_RAD(90),                0, "Top  X/Z"    },
     {                0,  GRAD_TO_RAD(90), "Left  Z/Y"   },
     {                0,                0, "Front  X/Y"  },
     { GRAD_TO_RAD(-90),                0, "Bottom  X/Z" },
     {                0, GRAD_TO_RAD(-90), "Right  Z/Y"  },
     {                0, GRAD_TO_RAD(180), "Back  X/Y"   },
     {  GRAD_TO_RAD(20),  GRAD_TO_RAD(30), "User"        },
     {                0,                0, "Camera"      },
     {                0,                0, "None"        }
   };

double TGrid::GradToRad( double dGrad )
 {
   return dGrad / 180.0 * M_PI;
 }

double TGrid::RadToGrad( double dRad )
 {
   return dRad * 180.0 / M_PI;
 }

double TGrid::CheckTinyRadian( double dVal )
 {
   if( TGrid::bFlCheckRadian == FALSE ) return dVal;
   return (fabs(dVal) < UCS_d_AngleRadZero) ? 0:dVal;
 }

double TGrid::CheckTinyGrad( double dGrad )
 {
   return (fabs(dGrad) < UCS_d_AngleGradZero) ? 0:dGrad;
 }

double TGrid::CheckTinyCoord( double dVal )
 {
   return (fabs(dVal) < UCS_d_GeometricZero) ? 0:dVal;
 }


TUCS::TUCS( TViewPoint vpView, int angleH, int angleV ):
  TGrid( TRECT(-200, 100, 200, -100), 10, 10 ),
  vpViewPoint( vpView ),
  globalBasePoint( 0, 0, 0 )
 {
   bFlIsBasePointVisible = FALSE;
/*   if( !angleH && !angleV )
    {
      h = vpVpointsData[ vpView ].angleH;
      v = vpVpointsData[ vpView ].angleV;
    }
   else
    {
      h = angleH;
      v = angleV;
    } */

   ChangeVPoint( vpView, TGrid::GradToRad(angleH), TGrid::GradToRad(angleV) );
   bFlVisyalAxis = FALSE;
 }

int TUCS::FindViewForAngles( double angleH, double angleV )//radian
 {
   for( int i = 0; i < 6; i++ )
     if( !TGrid::CheckTinyRadian(TUCS::vpVpointsData[i].angleH - TGrid::CheckTinyRadian(angleH)) &&
	 !TGrid::CheckTinyRadian(TUCS::vpVpointsData[i].angleV - TGrid::CheckTinyRadian(angleV))
       )
       return i;

   return -1;
 }

void TUCS::ChangeVPoint( TViewPoint vpView, double angleH, double angleV )
 {                                                 //radian

   vpViewPoint = vpView;

   if( !angleH && !angleV )
    {
      h = vpVpointsData[ vpView ].angleH;
      v = vpVpointsData[ vpView ].angleV;
    }
   else
    {
      h = angleH;
      v = angleV;
      if( (h != vpVpointsData[ vpView ].angleH ||
	  v != vpVpointsData[ vpView ].angleV) &&
	  vpViewPoint != TUCS::VP_None && vpViewPoint != TUCS::VP_Camera )
	vpViewPoint = TUCS::VP_User;

      if( vpViewPoint == TUCS::VP_User )
       {
         int iFind;
         if( (iFind = TUCS::FindViewForAngles(angleH, angleV)) != -1 )
          {
	    h = vpVpointsData[ iFind ].angleH;
	    v = vpVpointsData[ iFind ].angleV;
	    vpViewPoint = iFind;
	  }
       }
    }
                  
   dCosH = cos( h );
   dSinH = sin( h );
   dCosV = cos( v );
   dSinV = sin( v );
 }


static void SpecialOut( HDC hdc, RT3DPoint rPoint, LPCSTR lpcStr,
  TEXTMETRIC &tm )
 {
   double x, y;

   if( rPoint.y > 0 ) y = rPoint.y + tm.tmHeight + 1;
   else y = rPoint.y;
   x = rPoint.x;
   TextOut( hdc, x, y, lpcStr, 1 );
   //TextOut( hdc, rPoint.x, rPoint.y, lpcStr, 1 );
 }


void TUCS::Draw( HDC hdc, HWND hWindow, BOOL bFlErase )
 {   
   //if( GetGridState() == TRUE )

   if( vpViewPoint == TUCS::VP_None || vpViewPoint == TUCS::VP_Camera ) return;

   TGrid::Draw( hdc );

   if( IsVisibleBasePoint() == TRUE )
    {
      COLORREF coLbl = RGB(0, 0, 0);

      //double dDiv = ((GetScale() < 1 && GetScale()) ? GetScale():1);
      double dDiv = (GetScale() > 0.001) ? 1.0/GetScale():1;
      const int dx = float(UCS_i_BasePointMarkerSizeX) / dDiv,
		dy = float(UCS_i_BasePointMarkerSizeY) / dDiv;

      T3DPoint pt = GetGlobalBase();
      HPEN hp = CreatePen( PS_SOLID, UCS_i_SolidAxis, coLbl );
      HPEN hpOld = SelectPen( hdc, hp );
            //&&&
      RecalcUserToBase( &pt, 1 );

      SetPixel( hdc, pt.x, pt.y, coLbl );
      MoveTo( hdc, pt.x - dx, pt.y - dy );
      LineTo( hdc, pt.x + dx, pt.y + dy );

      MoveTo( hdc, pt.x + dx, pt.y - dy );
      LineTo( hdc, pt.x - dx, pt.y + dy );

      SelectPen( hdc, hpOld );
      DeleteObject( hp );
    }

   if( bFlVisyalAxis == FALSE ) return;

   T3DPoint pO[4]; _fsetmem( pO, sizeof(T3DPoint) * 4, 0 );
   pO[1].x = UCS_i_AxisSizeX;
   pO[2].y = UCS_i_AxisSizeY;
   pO[3].z = UCS_i_AxisSizeZ;

   static char *letter[4] = { "o", "x", "y", "z" };

   RecalcUserToBase( pO + 1, 3 );
   RoundPoint( pO + 1, 3 );

   COLORREF coLight, coDark, coOld;
   int iROPOld;

   if( bFlErase == TRUE )
    {
      coLight = PALETTEINDEX(MDI_i_BkColorIndexInSYSPalette);
      coDark = PALETTEINDEX(MDI_i_BkColorIndexInSYSPalette);
      iROPOld = SetROP2( hdc, R2_COPYPEN );
    }
   else
    {
      coLight = RGB( MDI_i_AxisFaceColorRGBr, MDI_i_AxisFaceColorRGBg, MDI_i_AxisFaceColorRGBb );
      coDark = PALETTEINDEX(MDI_i_AxisBackColorIndexInSYSPalette);
      iROPOld = SetROP2( hdc, R2_COPYPEN );
    }

   HPEN  hpenFor  = CreatePen( PS_SOLID, UCS_i_SolidAxis, coLight ),
	 hpenBack = CreatePen( PS_SOLID, UCS_i_SolidAxis, coDark ),
	 hpenOld;
   HFONT hfOld = SelectFont( hdc, GetStockFont(SYSTEM_FONT) );
   TEXTMETRIC m;
   GetTextMetrics( hdc, &m );
   UINT uiOldAlign = SetTextAlign( hdc, TA_CENTER | TA_TOP);


   hpenOld = SelectPen( hdc, hpenBack );
   coOld = SetTextColor( hdc, coDark );

   COLORREF colorBkOld = SetBkColor( hdc, PALETTEINDEX(6) );
   int iBkModeOld = SetBkMode( hdc, TRANSPARENT );

   for( int i = 1; i < 4; i++ )
    {
      if( TGrid::CheckTinyCoord(pO[i].z) < 0 )
       {
	 MoveTo( hdc, 0, 0 );
         LineTo( hdc, pO[i].x, pO[i].y );
	 SpecialOut( hdc, pO[i], letter[i], m );
       }
    }

    SelectPen( hdc, hpenFor );
    SetTextColor( hdc, coLight );
    for( i = 1; i < 4; i++ )
    {
      if( TGrid::CheckTinyCoord(pO[i].z) >= 0 )
       {
	 MoveTo( hdc, 0, 0 );
         LineTo( hdc, pO[i].x, pO[i].y );
	 SpecialOut( hdc, pO[i], letter[i], m );
       }
    }


   SetBkMode( hdc, iBkModeOld );
   SetROP2( hdc, iROPOld );
   SetBkColor( hdc, colorBkOld );
   SetTextAlign( hdc, uiOldAlign );
   SetTextColor( hdc, coOld );
   SelectObject( hdc, hpenOld );
   SelectFont( hdc, hfOld );
   DeleteObject( hpenFor );
   DeleteObject( hpenBack );

   //ChangeScale( dScaleKey );
 }


void TUCS::RoundPoint( PT3DPoint ppPoints, unsigned long ulSize )
 {
   PT3DPoint pPoint = ppPoints;
   unsigned long i;
		     //floor	 
   for( i = 0; i < ulSize; i++, pPoint++ )
    {
      double delta = fabs(pPoint->x - floor(pPoint->x));
      if( delta < 0.5 ) pPoint->x = floor(pPoint->x);
      else pPoint->x = ceil(pPoint->x);

      delta = fabs(pPoint->y - floor(pPoint->y));
      if( delta < 0.5 ) pPoint->y = floor(pPoint->y);
      else pPoint->y = ceil(pPoint->y);

      delta = fabs(pPoint->z - floor(pPoint->z));
      if( delta < 0.5 ) pPoint->z = floor(pPoint->z);
      else pPoint->z = ceil(pPoint->z);
    }
 }

 //h, v
void TUCS::RecalcUserToBase( PT3DPoint ppPoints, unsigned long ulSize )
 {
   PT3DPoint pPoint = ppPoints;
   unsigned long i;

   for( i = 0; i < ulSize; i++, pPoint++ )
    {
      double x = pPoint->x, y = pPoint->y, z = pPoint->z;

      pPoint->x = x*dCosV + y*dSinH*dSinV + z*dSinV*dCosH;
      pPoint->y = y*dCosH - z*dSinH;
      pPoint->z = -x*dSinV + y*dSinH*dCosV + z*dCosH*dCosV;

      pPoint->x *= GetScale();
      pPoint->y *= GetScale();
      pPoint->z *= GetScale();
    }
 }


void TUCS::RecalcBaseToUser( PT3DPoint ppPoints, unsigned long ulSize )
 {
   PT3DPoint pPoint = ppPoints;
   unsigned long i;

   for( i = 0; i < ulSize; i++, pPoint++ )
    {
      double x = pPoint->x, y = pPoint->y, z = pPoint->z;

      x /= GetScale();
      y /= GetScale();
      z /= GetScale();

      pPoint->x = x*dCosV - z*dSinV;
      pPoint->y = y*dCosH + x*dSinH*dSinV + z*dCosV*dSinH;
      pPoint->z = -y*dSinH + x*dCosH*dSinV + z*dCosH*dCosV;
    }
 }


void TGrid::Draw( HDC hdc )
 {
   if( bFlVisyalGreed == FALSE ) return;
   COLORREF coColor = RGB(MDI_i_GridColorRGBr, MDI_i_GridColorRGBg, MDI_i_GridColorRGBb );

   int staX, endX, staY, endY;

   if( bounds.left > bounds.right )
    {
      staX = bounds.right;
      endX = bounds.left;
    }
   else
   {
     staX = bounds.left;
     endX = bounds.right;
   }

   if( bounds.top > bounds.bottom )
    {
      staY = bounds.bottom;
      endY = bounds.top;
    }
   else
   {
     staY = bounds.top;
     endY = bounds.bottom;
   }

   for( int y = staY; y <= endY; y += spcY )
     for( int x = staX; x <= endX; x += spcX )
       SetPixel( hdc, x, y, coColor );
 }

void TGrid::SetupMapMode( HWND hWindow, HDC hdc )
 {   
   RECT rect;
   double iCx, iCy;

   GetClientRect( hWindow, &rect );
   SetMapMode( hdc, MM_ISOTROPIC );

   SetWindowExt( hdc, (iCx = rect.right - rect.left + 1),
		 (iCy = rect.bottom - rect.top + 1) );

   SetViewportExt( hdc, iCx, -iCy );
   SetViewportOrg( hdc, iCx / 2.0 + shiftX, iCy / 2.0 + shiftY );
 }

//BOOL GetSnapState() const { return bFlUseSnap; }
void TGrid::RefreshMousePos( HDC hdc, int newX, int newY )
 {
   static const double dRefrX = 0.6, dRefrY = 0.6;

   mouseOldX = mouseX; mouseOldY = mouseY;

   if( GetSnapState() == TRUE && GetGridState() == TRUE )
    {
      int dX = abs(newX - mouseX),
	  dY = abs(newY - mouseY);
      int deltaX = abs(double(spcX) * dRefrX - deltaMouseX),
	  deltaY = abs(double(spcY) * dRefrY - deltaMouseY);

      POINT p[2];
      p[1].x = mouseX; p[1].y = mouseY;
      p[0].x = newX; p[0].y = newY;
      DPtoLP( hdc, p, 2 );
      if( dX >= deltaX )
       {
	 p[0].x = NearstGridX( p[0].x );
	 deltaMouseX = 0;
       }
      else
       {
	 p[0].x = p[1].x;
	 deltaMouseX += dX;
       }
      if( dY >= deltaY )
       {
	 p[0].y = NearstGridY( p[0].y );
	 deltaMouseY = 0;
       }
      else
       {
	 p[0].y = p[1].y;
	 deltaMouseX += dY;
       }
      LPtoDP( hdc, p, 2 );
      mouseX = p[0].x;
      mouseY = p[0].y;
    }
   else
    {
      mouseX = newX;
      mouseY = newY;
    }
 }

int TGrid::NearstGridX( int x )
 {
   int staX, endX;

   if( bounds.left > bounds.right )
    {
      staX = bounds.right;
      endX = bounds.left;
    }
   else
   {
     staX = bounds.left;
     endX = bounds.right;
   }

  if( x < staX || x > endX ) return x;

  int left = (x / spcX) * spcX;
  int right = (x < 0) ? (left - spcX):(left + spcX);
  if( x < 0 ) { int tmp = left; left = right; right = tmp; }
  int d1 = abs(left - x), d2 = abs(right - x);

  return  ((d1 < d2) ? left:right);
 }

int TGrid::NearstGridY( int y )
 {
   int staY, endY;

   if( bounds.top > bounds.bottom )
    {
      staY = bounds.bottom;
      endY = bounds.top;
    }
   else
   {
     staY = bounds.top;
     endY = bounds.bottom;
   }

  if( y < staY || y > endY ) return y;

  int left = (y / spcY) * spcY;
  int right = (y < 0) ? (left - spcY):(left + spcY);
  if( y < 0 ) { int tmp = left; left = right; right = tmp; }
  int d1 = abs(left - y), d2 = abs(right - y);

  return  ((d1 < d2) ? left:right);
 }

void TRECT::Normalyze()
 {
   if( left > right )
    {
      int tmp = left;
      left = right; right = tmp;
    }

   if( top > bottom )
    {
      int tmp = top;
      top = bottom; bottom = tmp;
    }
 }

#pragma option -v.
