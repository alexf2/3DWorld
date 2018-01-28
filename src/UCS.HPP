#if !defined(_UCS_)
#define _UCS_

#include <windows.h>
#include <owldefs.h>
#include <math.h>

#define GRAD_TO_RAD(grad) (double(grad) / 180.0 * M_PI)
#define RAD_TO_GRAD(rad) (double(rad) * 180.0 / M_PI)

struct TViewParam
 {
   double angleH, angleV;	   //rotate around X
	                   //rotate around Y
   PSTR pcDeskr;
 };

_CLASSDEF(T3DPoint)
class  T3DPoint
 {
public:
   T3DPoint() { x = y = z = 0; }
   T3DPoint( double x, double y, double z )
    {
      this->x = x; this->y = y; this->z = z;
    }
   T3DPoint( RT3DPoint pt )
    {
      x = pt.x; y = pt.y; z = pt.z;
    }

   double x, y, z;
 };

//typedef T3DPoint* PT3DPoint;


struct TRECT: public RECT
 {
   TRECT()
    {
      left = right = top = bottom = 0;
    };

   TRECT( int left, int top, int right, int bottom )
    {
      this->left = left; this->top = top;
      this->right = right; this->bottom = bottom;
    }

   BOOL InRect( POINT _FAR &rP )
    {
      return BOOL(rP.x >= left && rP.x <= right && rP.y >= top && rP.y <= bottom);
    }

   void Normalyze();
 };

typedef TRECT _FAR *PTRECT;
typedef TRECT _FAR &RTRECT;

_CLASSDEF(TGrid)

class  TGrid
 {
public:
  RECT bounds;
  int spcX, spcY;
  static BOOL bFlCheckRadian;

  int mouseX, mouseY,
      mouseOldX, mouseOldY;

  TGrid( TRECT bounds, int spcX, int spcY )
   {
     this->bounds = bounds;
     this->spcX = spcX; this->spcY = spcY;
     scale = 1;
     bFlVisyalGreed = bFlUseSnap = FALSE;
     mouseX = mouseY = 0;
     deltaMouseX = deltaMouseY = 0;
     bFlCheckRadian = TRUE;
     shiftX = shiftY = 0;
   }

  void Draw( HDC hdc );
  void SetupMapMode( HWND hWindow, HDC hdc );

  BOOL ToggleGrid()
   {
     bFlVisyalGreed = (bFlVisyalGreed == TRUE) ? FALSE:TRUE;
     return bFlVisyalGreed;
   }

  BOOL ToggleSnap()
   {
     bFlUseSnap = (bFlUseSnap == TRUE) ? FALSE:TRUE;
     return bFlUseSnap;
   }

  void GridOnOff( BOOL bFl ) { bFlVisyalGreed = bFl; }
  void GridSnapOnOff( BOOL bFl ) { bFlUseSnap = bFl; }
  BOOL GetGridState() const { return bFlVisyalGreed; }
  BOOL GetSnapState() const { return bFlUseSnap; }

  void RefreshMousePos( HDC hdc, int newX, int newY );

  void ChangeScale( double dNewScale ) { scale = dNewScale; }
  double GetScale() const { return scale; }

  void Shift( double sX, double sY ) { shiftX += sX; shiftY += sY; }
  void ShiftAbs( double sX, double sY ) { shiftX = sX; shiftY = sY; }
  void GetShift( double& sX, double& sY ) { sX = shiftX; sY = shiftY; }

  static double GradToRad( double dGrad );
  static double RadToGrad( double dRad );

  static double CheckTinyRadian( double dVal ); //makes 0.0 if very < > 0
  static double CheckTinyCoord( double dVal );
  static double CheckTinyGrad( double dGrad );

private:
  BOOL bFlVisyalGreed;
  BOOL bFlUseSnap;
  int deltaMouseX, deltaMouseY;

  double scale; //for base UCS
  double shiftX, shiftY;

  int NearstGridX( int x );
  int NearstGridY( int y );

 };

_CLASSDEF(TUCS)

class  TUCS: public TGrid
 {
public:
  enum TViewPoint { VP_Top = 0, VP_Left, VP_Front, VP_Bottom, VP_Right, VP_Back,
		    VP_User, VP_Camera, VP_None };


  TUCS( TViewPoint vpView, int angleH = 0, int angleV = 0 );


  void Draw( HDC hdc, HWND hWindow, BOOL bFlErase = FALSE );


  virtual void ChangeVPoint( TViewPoint vpView, double angleH = 0, double angleV = 0 );
  void GetVpoint( TViewPoint& vpView, double& angleH, double& angleV )
   {
     vpView = vpViewPoint; angleH = h; angleV = v;
   }
  TViewPoint GetVpoint_() const { return vpViewPoint; }

  LPSTR GetVPointDescr() const
   {
     return vpVpointsData[vpViewPoint].pcDeskr;
   }
  void AxisOnOff( BOOL bFlVis ) { bFlVisyalAxis = bFlVis;  }

  BOOL GetVisState() const { return bFlVisyalAxis; }

  void RecalcUserToBase( PT3DPoint ppPoints, unsigned long ulSize );
  void RecalcBaseToUser( PT3DPoint ppPoints, unsigned long ulSize );

  void RoundPoint( PT3DPoint ppPoints, unsigned long ulSize );

  static int FindViewForAngles( double angleH, double angleV );
		    // Uses as index in  vpVpointsData[]

  T3DPoint GetGlobalBase() const { return globalBasePoint; }
  void SetGlobalBase( T3DPoint pt ) { globalBasePoint = pt; }
  BOOL IsVisibleBasePoint() const { return bFlIsBasePointVisible; }
  void SetVisibleBasePoint( BOOL bFl ) { bFlIsBasePointVisible = bFl; }
		       
private:
  static const TViewParam vpVpointsData[];

  double h, v;
  TViewPoint vpViewPoint;

  double dCosH, dSinH, dCosV, dSinV;

  BOOL  bFlVisyalAxis;

  T3DPoint globalBasePoint;
  BOOL  bFlIsBasePointVisible;

 };


#endif
