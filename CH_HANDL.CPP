#pragma option -v-

#include "threshol.hpp"
#include "mymdich.hpp"
#include "mymdi.hpp"
#include "ync.hpp"
#include "INC\menu.inc"
#include "INC\dlg.inc"
#include "INC\icon.inc"
#include "INC\mdchbmp.inc"
#include "grext.hpp"
#include "status.hpp"
#include "scena.hpp"
#include "bpdlg.hpp"
#include "fheader.hpp"
#include "aligndlg.hpp"
#include "autorc.hpp"
#include "INC\io_em.inc"

#include <bwcc.h>
#include <dir.h>
#include <stdio.h>

extern HINSTANCE hInstRC;


/*static near pascal LimitsFilterPlus( double& dVal, double dLimit )
 {
   if( dVal < dLimit ) dVal = dLimit;
 }*/

static void near pascal LimitsFilterPlus( int& dVal, int dLimit )
 {
   if( dVal < dLimit ) dVal = dLimit;
 }

/*static near pascal LimitsFilterPlus( double& dVal, int dLimit )
 {
   if( dVal < dLimit ) dVal = dLimit;
 }*/



void UCSSetupMouseHandler( RTMessage, PTMyMDIChild pWin, HDC hdc )
 {
   char cBuf[50];  
   double dAngleH, dAngleV;
   TViewPoint vpView;

   pWin->GetVpoint( vpView, dAngleH, dAngleV );

   sprintf( cBuf, "H = %5d  V = %5d",
     int( TGrid::CheckTinyGrad(TGrid::RadToGrad(dAngleH)) ),
     int( TGrid::CheckTinyGrad(TGrid::RadToGrad(dAngleV)) )
          );

   TSendTxt stSend;
   stSend.handle = 2;
   stSend.lpTxt = cBuf;
   SendMessage( pWin->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );

   if( TMyMDIChild::bFlUserUCSSetupSta == TRUE )
    {
      TMyMDIChild::bFlUserUCSSetupSta = FALSE;
      return;
    }

   if( pWin->mouseOldX != pWin->mouseX || pWin->mouseOldY != pWin->mouseY )
    {
      int deltaX = pWin->mouseX - pWin->mouseOldX,
	  deltaY = pWin->mouseY - pWin->mouseOldY;
      double dAngleH, dAngleV;
      TViewPoint vpView;

      //double dScaleKey = pWin->GetScale();
      //double sX, sY;
      //pWin->GetShift( sX, sY );

      //pWin->SetupMapMode( pWin->HWindow, pWin->hdcOwn );

      pWin->TUCS::Draw( pWin->hdcOwn, pWin->HWindow, TRUE );

      pWin->TGrid::bFlCheckRadian = FALSE;

      pWin->GetVpoint( vpView, dAngleH, dAngleV );
      pWin->ChangeVPoint( vpView, dAngleH + TGrid::GradToRad(deltaY),
	dAngleV + TGrid::GradToRad(deltaX) );

      pWin->TGrid::bFlCheckRadian = TRUE;

      pWin->TUCS::Draw( pWin->hdcOwn, pWin->HWindow );
      //pWin->ChangeScale( dScaleKey );
      //pWin->ShiftAbs( sX, sY );
    }

 }

void UCSSetupMouseLBHandler( RTMessage msg, PTMyMDIChild pCh )
 {
   //pCh->mcsSave

   double dAngleH, dAngleV;
   TViewPoint vpView;

   pCh->GetVpoint( vpView, dAngleH, dAngleV );

   pCh->mcsSave.dKeyAngleH = dAngleH;
   pCh->mcsSave.dKeyAngleV = dAngleV;
   pCh->mcsSave.vpKeyView = vpView;

   ShowCursor( TRUE );
   ReleaseCapture();

   pCh->mcsSave.Restore( *pCh );
 }

void UCSSetupMouseRBHandler( RTMessage msg, PTMyMDIChild pCh )
 {
   ShowCursor( TRUE );
   ReleaseCapture();

   pCh->mcsSave.Restore( *pCh );
 }

void UCSShiftMouseHandler( RTMessage msg, PTMyMDIChild pCh, HDC hdc )
 {
   if( TMyMDIChild::phazeOfCurMode == 0 ) { DefMouseHandler( msg, pCh, hdc ); return; }

   char cBuf[50];
   POINT p; p.x = pCh->mouseX; p.y = pCh->mouseY;
   DPtoLP( hdc, &p, 1 );
   sprintf( cBuf, "X = %4d  Y = %4d : Dx = %4d  Dy = %4d",
     TMyMDIChild::staXSh, TMyMDIChild::staYSh,
     (p.x - TMyMDIChild::staXSh),
     (p.y - TMyMDIChild::staYSh) );

   TSendTxt stSend;
   stSend.handle = 2;
   stSend.lpTxt = cBuf;
   SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );


   if( pCh->ScrollForShow(pCh->mouseX, pCh->mouseY, TRUE) )
    {
      InvalidateRect( pCh->HWindow, NULL, TRUE );
      UpdateWindow( pCh->HWindow );
      //TMyMDIChild::staXSh = p.x;
      //TMyMDIChild::staYSh = p.y;
    }


   HPEN hpNew = CreatePen( PS_SOLID, 0, PALETTEINDEX(6) );
   HPEN hpOld = SelectPen( hdc, hpNew );
   int iROPOld = SetROP2( hdc, R2_XORPEN );

   if( pCh->bFlRefresh == FALSE )
    {
      MoveTo( hdc, TMyMDIChild::staXSh, TMyMDIChild::staYSh );
      LineTo( hdc, TMyMDIChild::curXSh, TMyMDIChild::curYSh );
      pCh->DrawArrow( hdc, TMyMDIChild::staXSh, TMyMDIChild::staYSh,
		   TMyMDIChild::curXSh, TMyMDIChild::curYSh );
    }
   else
    {   
     pCh->bFlRefresh = FALSE;
    /* POINT p; GetCursorPos( &p ); ScreenToClient( pCh->HWindow, &p );
     DPtoLP( hdc, &p, 1 );*/
    }

   MoveTo( hdc, TMyMDIChild::staXSh, TMyMDIChild::staYSh );
   LineTo( hdc, (TMyMDIChild::curXSh = p.x),
     (TMyMDIChild::curYSh = p.y) );
   pCh->DrawArrow( hdc, TMyMDIChild::staXSh, TMyMDIChild::staYSh,
		   TMyMDIChild::curXSh, TMyMDIChild::curYSh );


   SetROP2( hdc, iROPOld );
   SelectPen( hdc, hpOld );
   DeleteObject( hpNew );
  
 }                 

void UCSShiftMouseLBHandler( RTMessage msg, PTMyMDIChild pCh )
 {
   if( TMyMDIChild::phazeOfCurMode == 0 )
    {
      POINT p; //p.x = pCh->mouseX; p.y = pCh->mouseY;
      GetCursorPos( &p );
      ScreenToClient( pCh->HWindow, &p );
      DPtoLP( pCh->hdcOwn, &p, 1 );
      TMyMDIChild::curXSh = TMyMDIChild::staXSh = p.x;
      TMyMDIChild::curYSh = TMyMDIChild::staYSh = p.y;

      TMyMDIChild::phazeOfCurMode = 1;
      //SetCapture( pCh->HWindow );

      TSendTxt stSend;
      stSend.handle = 0;
      TAutoStringRc str_( hInstRC, ID_MSG_22 );
      //stSend.lpTxt = "Move mouse for set shifting point";
      stSend.lpTxt = str_();

      SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );

      //TMyMDIChild::bFlShiftActive = TRUE;
      pCh->mcsSave = TMDIChSave( *pCh );
      //TMyMDIChild::bFlUserUCSSetupSta = TRUE;
   
      SetCapture( pCh->HWindow );
      ShowCursor( FALSE );

      pCh->GridOnOff( FALSE );
      pCh->GridSnapOnOff( FALSE );
      pCh->AxisOnOff( TRUE );

      InvalidateRect( pCh->HWindow, NULL, TRUE );     

      return;
    }

   double shX, shY;    /////////////////////
   pCh->GetShift( shX, shY );
   POINT p[2];
   p[0].x = TMyMDIChild::curXSh;
   p[1].x = TMyMDIChild::staXSh;
   p[0].y  = TMyMDIChild::curYSh;
   p[1].y = TMyMDIChild::staYSh;
   //p.x = (TMyMDIChild::curXSh - TMyMDIChild::staXSh);
   //p.y = (TMyMDIChild::curYSh - TMyMDIChild::staYSh);
   LPtoDP( pCh->hdcOwn, p, 2 );
   //pCh->mcsSave.shiftX = shX + p.x;
   //pCh->mcsSave.shiftY = shY - p.y;
   pCh->mcsSave.shiftX = shX + (p[0].x - p[1].x);
   pCh->mcsSave.shiftY = shY + (p[0].y - p[1].y);

   ShowCursor( TRUE );
   ReleaseCapture();

   TMyMDIChild::bFlShiftActive = FALSE;
   pCh->mcsSave.Restore( *pCh );
   TMyMDIChild::phazeOfCurMode = 0;
 }

void UCSShiftMouseRBHandler( RTMessage msg, PTMyMDIChild pCh )
 {
   if( TMyMDIChild::phazeOfCurMode == 1 )
    {
      ReleaseCapture();
      ShowCursor( TRUE );
      pCh->mcsSave.Restore( *pCh );
    }
   else pCh->SetModeChild( TMyMDIChild::mcKeyMode );

   TMyMDIChild::bFlShiftActive = FALSE;
   TMyMDIChild::phazeOfCurMode = 0;
   return;
 }

void CreCylinderMouseHandler( RTMessage msg, PTMyMDIChild pCh, HDC hdc )
 {
   if( TMyMDIChild::phazeOfCurMode == 0 ) { DefMouseHandler( msg, pCh, hdc ); return; }

   if( TMyMDIChild::phazeOfCurMode == 1 )
    {
      int iROPOld = SetROP2( hdc, R2_XORPEN );
      HPEN hpFon = CreatePen( PS_SOLID, 0, PALETTEINDEX(6) );
      HPEN hpOld = SelectPen( hdc, hpFon );
      HBRUSH hbrFon = CreateSolidBrush( PALETTEINDEX(6) );
      HBRUSH hbrOld = SelectBrush( hdc, hbrFon );

      //if( pCh->ScrollForShow( pCh->mouseX, pCh->mouseY ) )
	 //InvalidateRect( pCh->HWindow, NULL, TRUE );


      if( pCh->bFlRefresh == FALSE )
       {
         RoundRect( hdc, TMyMDIChild::pfdFace->insertPoint.x-1, TMyMDIChild::pfdFace->insertPoint.y-1,
	  TMyMDIChild::pfdFace->insertPoint.x+1, TMyMDIChild::pfdFace->insertPoint.y+1, 1, 1 );

         PTPolygon pPoly = new TPolygon( TMyMDIChild::pfdFace->insertPoint.x,
	   TMyMDIChild::pfdFace->insertPoint.y, ((PTCylinderInit)TMyMDIChild::pfdFace)->radius,
	   ((PTCylinderInit)TMyMDIChild::pfdFace)->nSides );

         SelectBrush( hdc, GetStockObject(NULL_BRUSH) );
         Polygon( hdc, pPoly->pPoint, pPoly->GetNumbVert() );

	 delete pPoly;
       }
      else pCh->bFlRefresh = FALSE;

      POINT p[2];
      p[0].x = pCh->mouseX;
      p[1].x = pCh->mouseOldX;
      p[0].y = pCh->mouseY;
      p[1].y = pCh->mouseOldY;
      DPtoLP( hdc, p, 2 );
      
      //int dX = pCh->mouseX - pCh->mouseOldX,
	  //dY = pCh->mouseY - pCh->mouseOldY;
      int dX = p[0].x - p[1].x,
	  dY = p[0].y - p[1].y;
      int dCom;
      if( abs(dX) > abs(dY) ) dCom = dX; else dCom = dY;

      //dCom = double(dCom) * pCh->GetScale();      
      if( dCom > 0 ) ((PTCylinderInit)TMyMDIChild::pfdFace)->radius += dCom;
      else if( ((PTCylinderInit)TMyMDIChild::pfdFace)->radius >= abs(dCom) )
	((PTCylinderInit)TMyMDIChild::pfdFace)->radius += dCom;

      char cBuf[80];
      sprintf( cBuf, "Radius = %d", ((PTCylinderInit)TMyMDIChild::pfdFace)->radius );
      TSendTxt stSend;
      stSend.handle = 2;
      stSend.lpTxt = cBuf;
      SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );


      SelectBrush( hdc, hbrFon );
      RoundRect( hdc, TMyMDIChild::pfdFace->insertPoint.x-1, TMyMDIChild::pfdFace->insertPoint.y-1,
	TMyMDIChild::pfdFace->insertPoint.x+1, TMyMDIChild::pfdFace->insertPoint.y+1, 1, 1 );

      SelectBrush( hdc, GetStockObject(NULL_BRUSH) );
      PTPolygon  pPoly = new TPolygon( TMyMDIChild::pfdFace->insertPoint.x,
	TMyMDIChild::pfdFace->insertPoint.y, ((PTCylinderInit)TMyMDIChild::pfdFace)->radius,
	((PTCylinderInit)TMyMDIChild::pfdFace)->nSides );
      Polygon( hdc, pPoly->pPoint, pPoly->GetNumbVert() );
      delete pPoly;

      SelectPen( hdc, hpOld );
      SelectBrush( hdc, hbrOld);
      SetROP2( hdc, iROPOld );
      DeleteObject( hpFon );
      DeleteObject( hbrFon );
      return;
    }

   if( TMyMDIChild::phazeOfCurMode == 2 ) { DefMouseHandler( msg, pCh, hdc ); return; }

   if( TMyMDIChild::phazeOfCurMode == 3 )
    {
      char cBuf[50];
      POINT p; p.x = pCh->mouseX; p.y = pCh->mouseY;
      DPtoLP( hdc, &p, 1 );                        
      sprintf( cBuf, "high = %4d",  (int)sqrt(pow(p.x - TMyMDIChild::staXSh+1, 2) +
	pow( p.y - TMyMDIChild::staYSh+1, 2)) );    

      TSendTxt stSend;
      stSend.handle = 2;
      stSend.lpTxt = cBuf;
      SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
     
      if( pCh->ScrollForShow(pCh->mouseX, pCh->mouseY, TRUE) )
       {
	 InvalidateRect( pCh->HWindow, NULL, TRUE );
         UpdateWindow( pCh->HWindow );
       }

      int iROPOld = SetROP2( hdc, R2_XORPEN );
      HPEN hpFon = CreatePen( PS_SOLID, 0, PALETTEINDEX(6) );
      HPEN hpOld = SelectPen( hdc, hpFon );   

      if( pCh->bFlRefresh == FALSE )
       {
         MoveTo( hdc, TMyMDIChild::staXSh, TMyMDIChild::staYSh );
         LineTo( hdc, TMyMDIChild::curXSh, TMyMDIChild::curYSh );
       }
      else   
	pCh->bFlRefresh = FALSE;


     MoveTo( hdc, TMyMDIChild::staXSh, TMyMDIChild::staYSh );
     LineTo( hdc, (TMyMDIChild::curXSh = p.x),
       (TMyMDIChild::curYSh = p.y) );

   //MoveTo( hdc, TMyMDIChild::curXSh, TMyMDIChild::curYSh );

     SelectPen( hdc, hpOld );
     SetROP2( hdc, iROPOld );
     DeleteObject( hpFon );

     //pCh->TUCS::Draw( pCh->hdcOwn, pCh->HWindow );
    }
 }

void CreCylinderMouseLBHandler( RTMessage msg, PTMyMDIChild pCh )
 {
   if( TMyMDIChild::phazeOfCurMode == 0 )
    {
      TMyMDIChild::phazeOfCurMode = 1;
      SetCapture( pCh->HWindow );
      ShowCursor( FALSE );

      pCh->mcsSave = TMDIChSave( *pCh );

      POINT p; p.x = LOWORD(msg.LParam); p.y = HIWORD(msg.LParam);
      DPtoLP( pCh->hdcOwn, &p, 1 );
      T3DPoint pt = T3DPoint( p.x, p.y, 0 );
      //pCh->RecalcBaseToUser( &pt, 1 );
      TMyMDIChild::pfdFace->insertPoint = pt;
      /*
	int nSides;
      int radius;
      int high;
      */
      ((PTCylinderInit)TMyMDIChild::pfdFace)->radius = 0;

      TSendTxt stSend;
      stSend.handle = 0;
      //stSend.lpTxt = "Select radius of sylinder";
      char cBuf[50];
      sprintf( cBuf, "Select radius of %s", TMyMDIChild::pfdFace->GetNameStr() );
      stSend.lpTxt = cBuf;
      SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
      return;
    }

   if( TMyMDIChild::phazeOfCurMode == 1 )
    {
      TMyMDIChild::phazeOfCurMode = 2;
      pCh->mcsSave.Restore( *pCh, TRUE );
                     
      ShowCursor( TRUE );
      ReleaseCapture();

      ((PTCylinderInit)TMyMDIChild::pfdFace)->high = 0;

      TSendTxt stSend;
      stSend.handle = 0;
      //stSend.lpTxt = "Click base point for select high of cylinder";
      char cBuf[128];
      TAutoStringRc s1_( hInstRC, ID_MSG_45 );
      sprintf( cBuf, s1_(), TMyMDIChild::pfdFace->GetNameStr() );
      //"Click base point for select high of %s"
      stSend.lpTxt = cBuf;
      SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
      InvalidateRect( pCh->HWindow, NULL, TRUE );
      return;
    }

   if( TMyMDIChild::phazeOfCurMode == 2 )
    {
      ShowCursor( FALSE );
      SetCapture( pCh->HWindow );

      pCh->mcsSave = TMDIChSave( *pCh );

      TMyMDIChild::phazeOfCurMode = 3;

      pCh->RefreshMousePos( pCh->hdcOwn, LOWORD(msg.LParam), HIWORD(msg.LParam) );
      POINT p; p.x = pCh->mouseX; p.y = pCh->mouseY;
      DPtoLP( pCh->hdcOwn, &p, 1 );
      TMyMDIChild::staXSh = TMyMDIChild::curXSh = p.x;
      TMyMDIChild::curYSh = TMyMDIChild::staYSh = p.y;


      TSendTxt stSend;
      stSend.handle = 0;
      //stSend.lpTxt = "Select high of cylinder";
      char cBuf[50];
      sprintf( cBuf, "Select high of %s", TMyMDIChild::pfdFace->GetNameStr() );
      stSend.lpTxt = cBuf;
      SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );

      return;
    }

   if( TMyMDIChild::phazeOfCurMode == 3 )
    {
      POINT p; p.x = pCh->mouseX; p.y = pCh->mouseY;
      DPtoLP( pCh->hdcOwn, &p, 1 );                        
      ((PTCylinderInit)TMyMDIChild::pfdFace)->high = (int)sqrt(pow(p.x - TMyMDIChild::staXSh+1, 2) +
	pow( p.y - TMyMDIChild::staYSh+1, 2));

      pCh->ScaleValueBaseToUser( ((PTCylinderInit)TMyMDIChild::pfdFace)->high );
      pCh->ScaleValueBaseToUser( ((PTCylinderInit)TMyMDIChild::pfdFace)->radius );
      LimitsFilterPlus( ((PTCylinderInit)TMyMDIChild::pfdFace)->high, MDI_d_MinHighBody );
      LimitsFilterPlus( ((PTCylinderInit)TMyMDIChild::pfdFace)->radius, MDI_d_MinRadiusConeCylind );
    
      pCh->RecalcBaseToUser( &(TMyMDIChild::pfdFace->insertPoint), 1 );

      pCh->mcsSave.Restore( *pCh );

      ShowCursor( TRUE );

      if( TMyMDIChild::pfdFace->isA() == FT_Cylinder )
       {
	 pCh->GetScena()->SetHardwareEnviron( (PTUCS)pCh, pCh->hdcOwn, (PTWindow)pCh );
	 HCURSOR hcL;
	 HCURSOR hcOld = SetCursor( (hcL = LoadCursor(NULL, IDC_WAIT)) );

         PTCylinder pCylind = new TCylinder(
           pCh->GetScena()->GetFreeIndex(),
	   TMyMDIChild::pfdFace            );
	 SetCursor( hcOld );
         DestroyCursor( hcL );



	 if( pCylind && !pCylind->Status )
	  {	    
	    pCh->CreationAligment( pCylind );
	    pCh->GetScena()->AddBody( pCylind );
          }
         else if( pCylind )
         {
           pCh->GetApplication()->Error( pCylind->Status );
           delete pCylind;
         }
         //pCylind->TestToFile( "G:\\tbl.fce" );   
       }
      else
       {
	 pCh->GetScena()->SetHardwareEnviron( (PTUCS)pCh, pCh->hdcOwn, (PTWindow)pCh );
	 HCURSOR hcL;
	 HCURSOR hcOld = SetCursor( (hcL = LoadCursor(NULL, IDC_WAIT)) );

	 PTCone pCone = new TCone(
           pCh->GetScena()->GetFreeIndex(),
	   TMyMDIChild::pfdFace            );
	 SetCursor( hcOld );
         DestroyCursor( hcL );

	 if( pCone && !pCone->Status )
	  {	    
	    pCh->CreationAligment( pCone );
	    pCh->GetScena()->AddBody( pCone );
          }
         else if( pCone )
         {
           pCh->GetApplication()->Error( pCone->Status );
           delete pCone;
         }
         //pCylind->TestToFile( "G:\\tbl.fce" );   
       }

      pCh->CreateCylinder( TMyMDIChild::pfdFace );

      ReleaseCapture();
      //InvalidateRect( pCh->HWindow, NULL, TRUE );
      pCh->RedrawVports();

      pCh->bFlModeActive = FALSE;
    }
 }

void CreCylinderMouseRBHandler( RTMessage msg, PTMyMDIChild pCh )
 {
   if( TMyMDIChild::phazeOfCurMode == 1 || TMyMDIChild::phazeOfCurMode == 3 )
    {
      ReleaseCapture();
      ShowCursor( TRUE );
      pCh->mcsSave.Restore( *pCh );
    }
   /*else if( TMyMDIChild::phazeOfCurMode == 2 )
     ReleaseCapture();*/

   InvalidateRect( pCh->HWindow, NULL, TRUE );
   pCh->CreateCylinder( TMyMDIChild::pfdFace );
 }


void CreSphereMouseHandler( RTMessage msg, PTMyMDIChild pCh, HDC hdc )
 {
   if( TMyMDIChild::phazeOfCurMode == 0 ) { DefMouseHandler( msg, pCh, hdc ); return; }

   if( TMyMDIChild::phazeOfCurMode == 1 )
    {
      int iROPOld = SetROP2( hdc, R2_XORPEN );
      HPEN hpFon = CreatePen( PS_SOLID, 0, PALETTEINDEX(6) );
      HPEN hpOld = SelectPen( hdc, hpFon );
      HBRUSH hbrFon = CreateSolidBrush( PALETTEINDEX(6) );
      HBRUSH hbrOld = SelectBrush( hdc, hbrFon );

      //if( pCh->ScrollForShow( pCh->mouseX, pCh->mouseY ) )
	 //InvalidateRect( pCh->HWindow, NULL, TRUE );
 /* int nMerid, nHoriz;

  int radius;*/

      if( pCh->bFlRefresh == FALSE )
       {
         RoundRect( hdc, TMyMDIChild::pfdFace->insertPoint.x-1, TMyMDIChild::pfdFace->insertPoint.y-1,
	  TMyMDIChild::pfdFace->insertPoint.x+1, TMyMDIChild::pfdFace->insertPoint.y+1, 1, 1 );

         PTPolygon pPoly = new TPolygon( TMyMDIChild::pfdFace->insertPoint.x,
	   TMyMDIChild::pfdFace->insertPoint.y, ((PTSphereInit)TMyMDIChild::pfdFace)->radius,
	   ((PTSphereInit)TMyMDIChild::pfdFace)->nHoriz );

         SelectBrush( hdc, GetStockObject(NULL_BRUSH) );
         Polygon( hdc, pPoly->pPoint, pPoly->GetNumbVert() );

	 delete pPoly;
       }
      else pCh->bFlRefresh = FALSE;

      POINT p[2];
      p[0].x = pCh->mouseX;
      p[1].x = pCh->mouseOldX;
      p[0].y = pCh->mouseY;
      p[1].y = pCh->mouseOldY;
      DPtoLP( hdc, p, 2 );
      
      //int dX = pCh->mouseX - pCh->mouseOldX,
	  //dY = pCh->mouseY - pCh->mouseOldY;
      int dX = p[0].x - p[1].x,
	  dY = p[0].y - p[1].y;                   
      
      int dCom;
      if( abs(dX) > abs(dY) ) dCom = dX; else dCom = dY;

      //dCom = double(dCom) * pCh->GetScale();
      if( dCom > 0 ) ((PTSphereInit)TMyMDIChild::pfdFace)->radius += dCom;
      else if( ((PTSphereInit)TMyMDIChild::pfdFace)->radius >= abs(dCom) )
	((PTSphereInit)TMyMDIChild::pfdFace)->radius += dCom;

      char cBuf[80];
      sprintf( cBuf, "Radius = %d", ((PTSphereInit)TMyMDIChild::pfdFace)->radius );
      TSendTxt stSend;
      stSend.handle = 2;
      stSend.lpTxt = cBuf;
      SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );


      SelectBrush( hdc, hbrFon );
      RoundRect( hdc, TMyMDIChild::pfdFace->insertPoint.x-1, TMyMDIChild::pfdFace->insertPoint.y-1,
	TMyMDIChild::pfdFace->insertPoint.x+1, TMyMDIChild::pfdFace->insertPoint.y+1, 1, 1 );

      SelectBrush( hdc, GetStockObject(NULL_BRUSH) );
      PTPolygon  pPoly = new TPolygon( TMyMDIChild::pfdFace->insertPoint.x,
	TMyMDIChild::pfdFace->insertPoint.y, ((PTSphereInit)TMyMDIChild::pfdFace)->radius,
	((PTSphereInit)TMyMDIChild::pfdFace)->nHoriz );
      Polygon( hdc, pPoly->pPoint, pPoly->GetNumbVert() );
      delete pPoly;

      SelectPen( hdc, hpOld );
      SelectBrush( hdc, hbrOld);
      SetROP2( hdc, iROPOld );
      DeleteObject( hpFon );
      DeleteObject( hbrFon );
      return;
    }

 }

void CreSphereMouseLBHandler( RTMessage msg, PTMyMDIChild pCh )
 {
   if( TMyMDIChild::phazeOfCurMode == 0 )
    {
      TMyMDIChild::phazeOfCurMode = 1;
      SetCapture( pCh->HWindow );
      ShowCursor( FALSE );

      pCh->mcsSave = TMDIChSave( *pCh );

      POINT p; p.x = LOWORD(msg.LParam); p.y = HIWORD(msg.LParam);
      DPtoLP( pCh->hdcOwn, &p, 1 );
      T3DPoint pt = T3DPoint( p.x, p.y, 0 );
      //pCh->RecalcBaseToUser( &pt, 1 );
      TMyMDIChild::pfdFace->insertPoint = pt;
      /*
	int nSides;
      int radius;
      int high;
      */
      ((PTSphereInit)TMyMDIChild::pfdFace)->radius = 0;

      TSendTxt stSend;
      stSend.handle = 0;
      TAutoStringRc str_( hInstRC, ID_MSG_23 );
      //stSend.lpTxt = "Select radius of sphere";
      stSend.lpTxt = str_();
            
      SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
      return;
    }

   if( TMyMDIChild::phazeOfCurMode == 1 )
    {
      pCh->RecalcBaseToUser( &(TMyMDIChild::pfdFace->insertPoint), 1 );

      pCh->mcsSave.Restore( *pCh );

      ShowCursor( TRUE );

      pCh->GetScena()->SetHardwareEnviron( (PTUCS)pCh, pCh->hdcOwn, (PTWindow)pCh );
      HCURSOR hcL;
      HCURSOR hcOld = SetCursor( (hcL = LoadCursor(NULL, IDC_WAIT)) );

      pCh->ScaleValueBaseToUser( ((PTSphereInit)TMyMDIChild::pfdFace)->radius );
      LimitsFilterPlus( ((PTSphereInit)TMyMDIChild::pfdFace)->radius, MDI_d_MinRadiusSphere );
      

      PTSphere pSphere = new TSphere(
        pCh->GetScena()->GetFreeIndex(),
	TMyMDIChild::pfdFace  );
      SetCursor( hcOld );
      DestroyCursor( hcL );

      if( pSphere && !pSphere->Status )
       {
	 pCh->CreationAligment( pSphere );
	 pCh->GetScena()->AddBody( pSphere );
       }
      else if( pSphere )
       {
         pCh->GetApplication()->Error( pSphere->Status );
         delete pSphere;
       }

      //pSphere->TestToFile( "G:\\tbl.fce" );


      //pCh->SetModeChild( MC_Default );
      pCh->CreateSphere( TMyMDIChild::pfdFace );

      ReleaseCapture();

      pCh->RedrawVports();

      pCh->bFlModeActive = FALSE;
    }

 }

void CreSphereMouseRBHandler( RTMessage msg, PTMyMDIChild pCh )
 {
   if( TMyMDIChild::phazeOfCurMode == 1  )
    {
      ReleaseCapture();
      ShowCursor( TRUE );
      pCh->mcsSave.Restore( *pCh );
    }

   InvalidateRect( pCh->HWindow, NULL, TRUE );
   pCh->CreateSphere( TMyMDIChild::pfdFace );
 }


void CreTorusMouseHandler( RTMessage msg, PTMyMDIChild pCh, HDC hdc )
 {
   if( TMyMDIChild::phazeOfCurMode == 0 ) { DefMouseHandler( msg, pCh, hdc ); return; }

   if( TMyMDIChild::phazeOfCurMode > 0 )
    {
      int iROPOld = SetROP2( hdc, R2_XORPEN );
      HPEN hpFon = CreatePen( PS_SOLID, 0, PALETTEINDEX(6) );
      HPEN hpOld = SelectPen( hdc, hpFon );
      HBRUSH hbrFon = CreateSolidBrush( PALETTEINDEX(6) );
      HBRUSH hbrOld = SelectBrush( hdc, hbrFon );

      //if( pCh->ScrollForShow( pCh->mouseX, pCh->mouseY ) )
	 //InvalidateRect( pCh->HWindow, NULL, TRUE );
 /* int nMerid, nHoriz;

  int radius;*/

      if( pCh->bFlRefresh == FALSE && TMyMDIChild::phazeOfCurMode == 1 )
       {
         RoundRect( hdc, TMyMDIChild::pfdFace->insertPoint.x-1, TMyMDIChild::pfdFace->insertPoint.y-1,
	  TMyMDIChild::pfdFace->insertPoint.x+1, TMyMDIChild::pfdFace->insertPoint.y+1, 1, 1 );

         PTPolygon pPoly = new TPolygon( TMyMDIChild::pfdFace->insertPoint.x,
	   TMyMDIChild::pfdFace->insertPoint.y, ((PTTorusInit)TMyMDIChild::pfdFace)->rad1,
	   ((PTTorusInit)TMyMDIChild::pfdFace)->nHoriz );

         SelectBrush( hdc, GetStockObject(NULL_BRUSH) );
         Polygon( hdc, pPoly->pPoint, pPoly->GetNumbVert() );

	 delete pPoly;
       }
      else if( TMyMDIChild::phazeOfCurMode == 1 ) pCh->bFlRefresh = FALSE;

      if( TMyMDIChild::phazeOfCurMode == 2 )
       {
	 if( pCh->bFlRefresh == FALSE )
          {
	    SelectBrush( hdc, GetStockObject(NULL_BRUSH) );
	    PTPolygon  pPoly = new TPolygon( TMyMDIChild::pfdFace->insertPoint.x,
	      TMyMDIChild::pfdFace->insertPoint.y, ((PTTorusInit)TMyMDIChild::pfdFace)->rad2,
	      ((PTTorusInit)TMyMDIChild::pfdFace)->nHoriz );
	      	    
	    Polygon( hdc, pPoly->pPoint, pPoly->GetNumbVert() );
	    delete pPoly;
          }
	 else pCh->bFlRefresh = FALSE;
       }

      POINT p[2];
      p[0].x = pCh->mouseX;
      p[1].x = pCh->mouseOldX;
      p[0].y = pCh->mouseY;
      p[1].y = pCh->mouseOldY;
      DPtoLP( hdc, p, 2 );
      
      //int dX = pCh->mouseX - pCh->mouseOldX,
	  //dY = pCh->mouseY - pCh->mouseOldY;
      int dX = p[0].x - p[1].x,
	  dY = p[0].y - p[1].y;                  
      
      int dCom;
      if( abs(dX) > abs(dY) ) dCom = dX; else dCom = dY;

      //dCom = double(dCom) * pCh->GetScale();
      if( TMyMDIChild::phazeOfCurMode == 1 )
       {
         if( dCom > 0 ) ((PTTorusInit)TMyMDIChild::pfdFace)->rad1 += dCom;
         else if( ((PTTorusInit)TMyMDIChild::pfdFace)->rad1 >= abs(dCom) )
	   ((PTTorusInit)TMyMDIChild::pfdFace)->rad1 += dCom;
       }
      else
       {
         if( dCom > 0 ) ((PTTorusInit)TMyMDIChild::pfdFace)->rad2 += dCom;
         else if( ((PTTorusInit)TMyMDIChild::pfdFace)->rad2 >= abs(dCom) )
	   ((PTTorusInit)TMyMDIChild::pfdFace)->rad2 += dCom;
       }

      char cBuf[80];
      sprintf( cBuf, "Radius = %d",
	(TMyMDIChild::phazeOfCurMode == 1) ?
	  ((PTTorusInit)TMyMDIChild::pfdFace)->rad1:
	  ((PTTorusInit)TMyMDIChild::pfdFace)->rad2
	     );
      TSendTxt stSend;
      stSend.handle = 2;
      stSend.lpTxt = cBuf;
      SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );


      SelectBrush( hdc, hbrFon );
      RoundRect( hdc, TMyMDIChild::pfdFace->insertPoint.x-1, TMyMDIChild::pfdFace->insertPoint.y-1,
	TMyMDIChild::pfdFace->insertPoint.x+1, TMyMDIChild::pfdFace->insertPoint.y+1, 1, 1 );

      if( TMyMDIChild::phazeOfCurMode == 2 )
       {
         SetROP2( hdc, R2_COPYPEN );
         SelectPen( hdc, GetStockObject(WHITE_PEN) );
       }

      SelectBrush( hdc, GetStockObject(NULL_BRUSH) );
      PTPolygon  pPoly = new TPolygon( TMyMDIChild::pfdFace->insertPoint.x,
        TMyMDIChild::pfdFace->insertPoint.y, ((PTTorusInit)TMyMDIChild::pfdFace)->rad1,
	((PTTorusInit)TMyMDIChild::pfdFace)->nHoriz );
      Polygon( hdc, pPoly->pPoint, pPoly->GetNumbVert() );

      delete pPoly;
      SetROP2( hdc, R2_XORPEN );
      SelectPen( hdc, hpFon );

      if( TMyMDIChild::phazeOfCurMode == 2 )
       {
         SelectBrush( hdc, GetStockObject(NULL_BRUSH) );      
	 PTPolygon  pPoly = new TPolygon( TMyMDIChild::pfdFace->insertPoint.x,
	   TMyMDIChild::pfdFace->insertPoint.y, ((PTTorusInit)TMyMDIChild::pfdFace)->rad2,
	   ((PTTorusInit)TMyMDIChild::pfdFace)->nHoriz );
	      	    
	 Polygon( hdc, pPoly->pPoint, pPoly->GetNumbVert() );
	 delete pPoly;
       }

      SelectPen( hdc, hpOld );
      SelectBrush( hdc, hbrOld);
      SetROP2( hdc, iROPOld );
      DeleteObject( hpFon );
      DeleteObject( hbrFon );
    
      return;
    }

 }

void CreTorusMouseLBHandler( RTMessage msg, PTMyMDIChild pCh )
 {
   if( TMyMDIChild::phazeOfCurMode == 0 )
    {
      TMyMDIChild::phazeOfCurMode = 1;
      SetCapture( pCh->HWindow );
      ShowCursor( FALSE );

      pCh->mcsSave = TMDIChSave( *pCh );

      POINT p; p.x = LOWORD(msg.LParam); p.y = HIWORD(msg.LParam);
      DPtoLP( pCh->hdcOwn, &p, 1 );
      T3DPoint pt = T3DPoint( p.x, p.y, 0 );
      //pCh->RecalcBaseToUser( &pt, 1 );
      TMyMDIChild::pfdFace->insertPoint = pt;
      /*
	int nSides;
      int radius;
      int high;
      */
      ((PTTorusInit)TMyMDIChild::pfdFace)->rad1 = 0;

      TSendTxt stSend;
      stSend.handle = 0;
      TAutoStringRc str_( hInstRC, ID_MSG_24 );
      //stSend.lpTxt = "Select radius1 of torus";
      stSend.lpTxt = str_();

            
      SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
      return;
    }

   if( TMyMDIChild::phazeOfCurMode == 1 )
    {
      TMyMDIChild::phazeOfCurMode = 2;

      ((PTTorusInit)TMyMDIChild::pfdFace)->rad2 = ((PTTorusInit)TMyMDIChild::pfdFace)->rad1;

      TSendTxt stSend;
      stSend.handle = 0;
      TAutoStringRc str_( hInstRC, ID_MSG_25 );
      //stSend.lpTxt = "Select radius2 of torus";
      stSend.lpTxt = str_();

      SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
      //InvalidateRect( pCh->HWindow, NULL, TRUE );
      return;
    }

   if( TMyMDIChild::phazeOfCurMode == 2 )
    {
      pCh->RecalcBaseToUser( &(TMyMDIChild::pfdFace->insertPoint), 1 );

      pCh->mcsSave.Restore( *pCh );

      ShowCursor( TRUE );

      pCh->GetScena()->SetHardwareEnviron( (PTUCS)pCh, pCh->hdcOwn, (PTWindow)pCh );
      HCURSOR hcL;
      HCURSOR hcOld = SetCursor( (hcL = LoadCursor(NULL, IDC_WAIT)) );

      pCh->ScaleValueBaseToUser( ((PTTorusInit)TMyMDIChild::pfdFace)->rad1 );
      pCh->ScaleValueBaseToUser( ((PTTorusInit)TMyMDIChild::pfdFace)->rad2 );
      LimitsFilterPlus( ((PTTorusInit)TMyMDIChild::pfdFace)->rad1, MDI_d_MinRadiusTorus1 );
      LimitsFilterPlus( ((PTTorusInit)TMyMDIChild::pfdFace)->rad2, MDI_d_MinRadiusTorus2 );

      PTTorus pTorus = new TTorus(
        pCh->GetScena()->GetFreeIndex(),
	TMyMDIChild::pfdFace  );
      SetCursor( hcOld );
      DestroyCursor( hcL );

      if( pTorus && !pTorus->Status )
       {
	 pCh->CreationAligment( pTorus );
	 pCh->GetScena()->AddBody( pTorus );
       }
      else if( pTorus )
       {
	 pCh->GetApplication()->Error( pTorus->Status );
	 delete pTorus;
       }
      
      //pTorus->TestToFile( "G:\\tbl.fce" );

      //pCh->SetModeChild( MC_Default );
      pCh->CreateTorus( TMyMDIChild::pfdFace );

      ReleaseCapture();
      pCh->RedrawVports();


      pCh->bFlModeActive = FALSE;
    }
 }

void CreTorusMouseRBHandler( RTMessage msg, PTMyMDIChild pCh )
 {
   if( TMyMDIChild::phazeOfCurMode == 1 || TMyMDIChild::phazeOfCurMode == 2 )
    {
      ReleaseCapture();
      ShowCursor( TRUE );
      pCh->mcsSave.Restore( *pCh );
    }

   InvalidateRect( pCh->HWindow, NULL, TRUE );
   pCh->CreateTorus( TMyMDIChild::pfdFace );
 }


void CreTubeMouseHandler( RTMessage msg, PTMyMDIChild pCh, HDC hdc )
 {
   if( TMyMDIChild::phazeOfCurMode == 0 ) { DefMouseHandler( msg, pCh, hdc ); return; }

   if( TMyMDIChild::phazeOfCurMode == 1 || TMyMDIChild::phazeOfCurMode == 2 )
    {
      int iROPOld = SetROP2( hdc, R2_XORPEN );
      HPEN hpFon = CreatePen( PS_SOLID, 0, PALETTEINDEX(6) );
      HPEN hpOld = SelectPen( hdc, hpFon );
      HBRUSH hbrFon = CreateSolidBrush( PALETTEINDEX(6) );
      HBRUSH hbrOld = SelectBrush( hdc, hbrFon );

      //if( pCh->ScrollForShow( pCh->mouseX, pCh->mouseY ) )
	 //InvalidateRect( pCh->HWindow, NULL, TRUE );
/*
int nSides;

  int rad1, rad2, high;
*/
      if( pCh->bFlRefresh == FALSE && TMyMDIChild::phazeOfCurMode == 1 )
       {
         RoundRect( hdc, TMyMDIChild::pfdFace->insertPoint.x-1, TMyMDIChild::pfdFace->insertPoint.y-1,
	  TMyMDIChild::pfdFace->insertPoint.x+1, TMyMDIChild::pfdFace->insertPoint.y+1, 1, 1 );

         PTPolygon pPoly = new TPolygon( TMyMDIChild::pfdFace->insertPoint.x,
	   TMyMDIChild::pfdFace->insertPoint.y, ((PTTubeInit)TMyMDIChild::pfdFace)->rad1,
	   ((PTTubeInit)TMyMDIChild::pfdFace)->nSides );
	    
         SelectBrush( hdc, GetStockObject(NULL_BRUSH) );
         Polygon( hdc, pPoly->pPoint, pPoly->GetNumbVert() );

	 delete pPoly;
       }
      else if( TMyMDIChild::phazeOfCurMode == 1 ) pCh->bFlRefresh = FALSE;

      if( TMyMDIChild::phazeOfCurMode == 2 )
       {
	 if( pCh->bFlRefresh == FALSE )
          {
	    SelectBrush( hdc, GetStockObject(NULL_BRUSH) );
	    PTPolygon  pPoly = new TPolygon( TMyMDIChild::pfdFace->insertPoint.x,
	      TMyMDIChild::pfdFace->insertPoint.y, ((PTTubeInit)TMyMDIChild::pfdFace)->rad2,
	      ((PTTubeInit)TMyMDIChild::pfdFace)->nSides );
	      	    
	    Polygon( hdc, pPoly->pPoint, pPoly->GetNumbVert() );
	    delete pPoly;
          }
	 else pCh->bFlRefresh = FALSE;
       }


      POINT p[2];
      p[0].x = pCh->mouseX;
      p[1].x = pCh->mouseOldX;
      p[0].y = pCh->mouseY;
      p[1].y = pCh->mouseOldY;
      DPtoLP( hdc, p, 2 );
      
      //int dX = pCh->mouseX - pCh->mouseOldX,
	  //dY = pCh->mouseY - pCh->mouseOldY;
      int dX = p[0].x - p[1].x,
	  dY = p[0].y - p[1].y;
      
      int dCom;
      if( abs(dX) > abs(dY) ) dCom = dX; else dCom = dY;

      //dCom = double(dCom) * pCh->GetScale();
      if( TMyMDIChild::phazeOfCurMode == 1 )
       {
         if( dCom > 0 ) ((PTTubeInit)TMyMDIChild::pfdFace)->rad1 += dCom;
         else if( ((PTTubeInit)TMyMDIChild::pfdFace)->rad1 >= abs(dCom) )
	   ((PTTubeInit)TMyMDIChild::pfdFace)->rad1 += dCom;
       }
      else
       {
         if( dCom > 0 ) ((PTTubeInit)TMyMDIChild::pfdFace)->rad2 += dCom;
         else if( ((PTTubeInit)TMyMDIChild::pfdFace)->rad2 >= abs(dCom) )
	   ((PTTubeInit)TMyMDIChild::pfdFace)->rad2 += dCom;
       }

      char cBuf[80];
      sprintf( cBuf, "Radius = %d",
	(TMyMDIChild::phazeOfCurMode == 1) ?
	  ((PTTubeInit)TMyMDIChild::pfdFace)->rad1:
	  ((PTTubeInit)TMyMDIChild::pfdFace)->rad2
	     );
      TSendTxt stSend;
      stSend.handle = 2;
      stSend.lpTxt = cBuf;
      SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );


      SelectBrush( hdc, hbrFon );
      RoundRect( hdc, TMyMDIChild::pfdFace->insertPoint.x-1, TMyMDIChild::pfdFace->insertPoint.y-1,
	TMyMDIChild::pfdFace->insertPoint.x+1, TMyMDIChild::pfdFace->insertPoint.y+1, 1, 1 );

      if( TMyMDIChild::phazeOfCurMode == 2 )
       {
         SetROP2( hdc, R2_COPYPEN );
         SelectPen( hdc, GetStockObject(WHITE_PEN) );
       }

      SelectBrush( hdc, GetStockObject(NULL_BRUSH) );
      PTPolygon  pPoly = new TPolygon( TMyMDIChild::pfdFace->insertPoint.x,
        TMyMDIChild::pfdFace->insertPoint.y, ((PTTubeInit)TMyMDIChild::pfdFace)->rad1,
	((PTTubeInit)TMyMDIChild::pfdFace)->nSides );
      Polygon( hdc, pPoly->pPoint, pPoly->GetNumbVert() );

      delete pPoly;
      SetROP2( hdc, R2_XORPEN );
      SelectPen( hdc, hpFon );

      if( TMyMDIChild::phazeOfCurMode == 2 )
       {
         SelectBrush( hdc, GetStockObject(NULL_BRUSH) );      
	 PTPolygon  pPoly = new TPolygon( TMyMDIChild::pfdFace->insertPoint.x,
	   TMyMDIChild::pfdFace->insertPoint.y, ((PTTubeInit)TMyMDIChild::pfdFace)->rad2,
	   ((PTTubeInit)TMyMDIChild::pfdFace)->nSides );
	      	    
	 Polygon( hdc, pPoly->pPoint, pPoly->GetNumbVert() );
	 delete pPoly;
       }

      SelectPen( hdc, hpOld );
      SelectBrush( hdc, hbrOld);
      SetROP2( hdc, iROPOld );
      DeleteObject( hpFon );
      DeleteObject( hbrFon );
    
      return;
    }

   if( TMyMDIChild::phazeOfCurMode == 3 ) { DefMouseHandler( msg, pCh, hdc ); return; }

   if( TMyMDIChild::phazeOfCurMode == 4 )
    {
      char cBuf[50];
      POINT p; p.x = pCh->mouseX; p.y = pCh->mouseY;
      DPtoLP( hdc, &p, 1 );                        
      sprintf( cBuf, "high = %4d",  (int)sqrt(pow(p.x - TMyMDIChild::staXSh+1, 2) +
	pow( p.y - TMyMDIChild::staYSh+1, 2)) );    

      TSendTxt stSend;
      stSend.handle = 2;
      stSend.lpTxt = cBuf;
      SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
     
      if( pCh->ScrollForShow(pCh->mouseX, pCh->mouseY, TRUE) )
       {
	 InvalidateRect( pCh->HWindow, NULL, TRUE );
	 UpdateWindow( pCh->HWindow );
       }

      int iROPOld = SetROP2( hdc, R2_XORPEN );
      HPEN hpFon = CreatePen( PS_SOLID, 0, PALETTEINDEX(6) );
      HPEN hpOld = SelectPen( hdc, hpFon );   

      if( pCh->bFlRefresh == FALSE )
       {
         MoveTo( hdc, TMyMDIChild::staXSh, TMyMDIChild::staYSh );
         LineTo( hdc, TMyMDIChild::curXSh, TMyMDIChild::curYSh );
       }
      else   
	pCh->bFlRefresh = FALSE;


     MoveTo( hdc, TMyMDIChild::staXSh, TMyMDIChild::staYSh );
     LineTo( hdc, (TMyMDIChild::curXSh = p.x),
       (TMyMDIChild::curYSh = p.y) );

     SelectPen( hdc, hpOld );
     SetROP2( hdc, iROPOld );
     DeleteObject( hpFon );
    }

 }

void CreTubeMouseLBHandler( RTMessage msg, PTMyMDIChild pCh )
 {
   if( TMyMDIChild::phazeOfCurMode == 0 )
    {
      TMyMDIChild::phazeOfCurMode = 1;
      SetCapture( pCh->HWindow );
      ShowCursor( FALSE );

      pCh->mcsSave = TMDIChSave( *pCh );

      POINT p; p.x = LOWORD(msg.LParam); p.y = HIWORD(msg.LParam);
      DPtoLP( pCh->hdcOwn, &p, 1 );
      T3DPoint pt = T3DPoint( p.x, p.y, 0 );
      //pCh->RecalcBaseToUser( &pt, 1 );
      TMyMDIChild::pfdFace->insertPoint = pt;
      /*
	int nSides;
      int radius;
      int high;
      */
      ((PTTubeInit)TMyMDIChild::pfdFace)->rad1 = 0;

      TSendTxt stSend;
      stSend.handle = 0;
      TAutoStringRc str_( hInstRC, ID_MSG_26 );
      //stSend.lpTxt = "Select radius1 of base tube";
      stSend.lpTxt = str_();
            
      SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
      return;
    }

   if( TMyMDIChild::phazeOfCurMode == 1 )
    {
      TMyMDIChild::phazeOfCurMode = 2;

      ((PTTubeInit)TMyMDIChild::pfdFace)->rad2 = ((PTTubeInit)TMyMDIChild::pfdFace)->rad1;

      TSendTxt stSend;
      stSend.handle = 0;
      TAutoStringRc str_( hInstRC, ID_MSG_27 );
      //stSend.lpTxt = "Select radius2 of base tube";
      stSend.lpTxt = str_();

      SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
      //InvalidateRect( pCh->HWindow, NULL, TRUE );
      return;
    }

   if( TMyMDIChild::phazeOfCurMode == 2 )
    {
      TMyMDIChild::phazeOfCurMode = 3;
      pCh->mcsSave.Restore( *pCh, TRUE );
                     
      ShowCursor( TRUE );
      ReleaseCapture();

      ((PTTubeInit)TMyMDIChild::pfdFace)->high = 0;

      TSendTxt stSend;
      stSend.handle = 0;
      TAutoStringRc str_( hInstRC, ID_MSG_28 );
      //stSend.lpTxt = "Click base point for select high of tube";
      stSend.lpTxt = str_();
      
      SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
      InvalidateRect( pCh->HWindow, NULL, TRUE );
      return;
    }


   if( TMyMDIChild::phazeOfCurMode == 3 )
    {
      ShowCursor( FALSE );
      SetCapture( pCh->HWindow );

      pCh->mcsSave = TMDIChSave( *pCh );

      TMyMDIChild::phazeOfCurMode = 4;

      pCh->RefreshMousePos( pCh->hdcOwn, LOWORD(msg.LParam), HIWORD(msg.LParam) );
      POINT p; p.x = pCh->mouseX; p.y = pCh->mouseY;
      DPtoLP( pCh->hdcOwn, &p, 1 );
      TMyMDIChild::staXSh = TMyMDIChild::curXSh = p.x;
      TMyMDIChild::curYSh = TMyMDIChild::staYSh = p.y;


      TSendTxt stSend;
      stSend.handle = 0;
      TAutoStringRc str_( hInstRC, ID_MSG_29 );
      //stSend.lpTxt = "Select high of tube";
      stSend.lpTxt = str_();
           
      SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );

      return;
    }

   if( TMyMDIChild::phazeOfCurMode == 4 )
    {
      POINT p; p.x = pCh->mouseX; p.y = pCh->mouseY;
      DPtoLP( pCh->hdcOwn, &p, 1 );                        
      ((PTTubeInit)TMyMDIChild::pfdFace)->high = (int)sqrt(pow(p.x - TMyMDIChild::staXSh+1, 2) +
	pow( p.y - TMyMDIChild::staYSh+1, 2));
    
      pCh->RecalcBaseToUser( &(TMyMDIChild::pfdFace->insertPoint), 1 );

      pCh->mcsSave.Restore( *pCh );

      //pCh->SetModeChild( MC_Default );

      ShowCursor( TRUE );

      pCh->GetScena()->SetHardwareEnviron( (PTUCS)pCh, pCh->hdcOwn, (PTWindow)pCh );
      HCURSOR hcL;
      HCURSOR hcOld = SetCursor( (hcL = LoadCursor(NULL, IDC_WAIT)) );

      pCh->ScaleValueBaseToUser( ((PTTubeInit)TMyMDIChild::pfdFace)->high );
      pCh->ScaleValueBaseToUser( ((PTTubeInit)TMyMDIChild::pfdFace)->rad1 );
      pCh->ScaleValueBaseToUser( ((PTTubeInit)TMyMDIChild::pfdFace)->rad2 );
      LimitsFilterPlus( ((PTTubeInit)TMyMDIChild::pfdFace)->high, MDI_d_MinHighBody );
      LimitsFilterPlus( ((PTTubeInit)TMyMDIChild::pfdFace)->rad1, MDI_d_MinRadiusTube1 );
      LimitsFilterPlus( ((PTTubeInit)TMyMDIChild::pfdFace)->rad2, MDI_d_MinRadiusTube2 );

      PTTube pTube = new TTube(
        pCh->GetScena()->GetFreeIndex(),
	TMyMDIChild::pfdFace  );
      SetCursor( hcOld );
      DestroyCursor( hcL );

      if( pTube && !pTube->Status )
       {
	 pCh->CreationAligment( pTube );
	 pCh->GetScena()->AddBody( pTube );
       }
      else if( pTube )
       {
         pCh->GetApplication()->Error( pTube->Status );
         delete pTube;
       }

      //pTube->TestToFile( "G:\\tbl.fce" );

      pCh->CreateTube( TMyMDIChild::pfdFace );

      ReleaseCapture();

      pCh->RedrawVports();

      pCh->bFlModeActive = FALSE;
    }

 }

void CreTubeMouseRBHandler( RTMessage msg, PTMyMDIChild pCh )
 {
   if( TMyMDIChild::phazeOfCurMode == 1 || TMyMDIChild::phazeOfCurMode == 2 ||
     TMyMDIChild::phazeOfCurMode == 4 )
    {
      ReleaseCapture();
      ShowCursor( TRUE );
      pCh->mcsSave.Restore( *pCh );
    }

   //pCh->SetModeChild( MC_Default );
   InvalidateRect( pCh->HWindow, NULL, TRUE );
   pCh->CreateTube( TMyMDIChild::pfdFace );
 }

void GetWinMouseHandler( RTMessage msg, PTMyMDIChild pCh, HDC hdc )
 {
   if( TMyMDIChild::phazeOfCurMode == 0 ) { DefMouseHandler( msg, pCh, hdc ); return; }

   if( TMyMDIChild::phazeOfCurMode == 1 )
    {
      if( pCh->ScrollForShow(pCh->mouseX, pCh->mouseY, TRUE) )
       {
	 InvalidateRect( pCh->HWindow, NULL, TRUE );
         UpdateWindow( pCh->HWindow );
       }

      int iROPOld = SetROP2( hdc, R2_XORPEN );
      int iPenStyle, iPenThick;
      if( TMyMDIChild::UsedCrossing() == FALSE )
       {
	 iPenStyle = PS_SOLID;
	 iPenThick = 0;
       }
      else
       {
	 iPenStyle = PS_DASH;
	 iPenThick = 0;
       }

      HPEN hpFon = CreatePen( iPenStyle, iPenThick, PALETTEINDEX(6) );
      HPEN hpOld = SelectPen( hdc, hpFon );     
      HBRUSH hbrOld;


      if( pCh->bFlRefresh == FALSE )
       {
	 hbrOld = SelectBrush( hdc, GetStockObject(NULL_BRUSH) );
	 Rectangle( hdc, TMyMDIChild::staXSh, TMyMDIChild::staYSh,
	   TMyMDIChild::curXSh, TMyMDIChild::curYSh );
	 SelectBrush( hdc, hbrOld );
       }
      else pCh->bFlRefresh = FALSE;
              
      RECT r;
      if( TMyMDIChild::staXSh < TMyMDIChild::curXSh )
       {
	 r.left = TMyMDIChild::staXSh; r.right = TMyMDIChild::curXSh;
       }
      else
       {
	 r.left = TMyMDIChild::curXSh; r.right = TMyMDIChild::staXSh;
       }
      if( TMyMDIChild::staYSh < TMyMDIChild::curYSh )
       {
	 r.top = TMyMDIChild::staYSh; r.bottom = TMyMDIChild::curYSh;
       }
      else
       {
	 r.top = TMyMDIChild::curYSh; r.bottom = TMyMDIChild::staYSh;
       }

      char cBuf[80];
      sprintf( cBuf, "X: %3d Y: %3d  W: %3d H: %3d",
	TMyMDIChild::staXSh, TMyMDIChild::staYSh,
	r.right - r.left, r.bottom - r.top );
      TSendTxt stSend;
      stSend.handle = 2;
      stSend.lpTxt = cBuf;
      SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );

      POINT p; p.x = pCh->mouseX; p.y = pCh->mouseY;
      DPtoLP( hdc, &p, 1 );
      TMyMDIChild::curXSh = p.x;
      TMyMDIChild::curYSh = p.y;

      hbrOld = SelectBrush( hdc, GetStockObject(NULL_BRUSH) );
      Rectangle( hdc, TMyMDIChild::staXSh, TMyMDIChild::staYSh,
	TMyMDIChild::curXSh, TMyMDIChild::curYSh );
      SelectBrush( hdc, hbrOld );

      SelectPen( hdc, hpOld );
      SetROP2( hdc, iROPOld );
      DeleteObject( hpFon );
      return;
    }

 }

void GetWinMouseLBHandler( RTMessage msg, PTMyMDIChild pCh )
 {
   if( TMyMDIChild::phazeOfCurMode == 0 )
    {
      POINT p; p.x = pCh->mouseX; p.y = pCh->mouseY;
      //GetCursorPos( &p );
      //ScreenToClient( pCh->HWindow, &p );
      DPtoLP( pCh->hdcOwn, &p, 1 );
      TMyMDIChild::curXSh = TMyMDIChild::staXSh = p.x;
      TMyMDIChild::curYSh = TMyMDIChild::staYSh = p.y;

      TMyMDIChild::phazeOfCurMode = 1;
      //SetCapture( pCh->HWindow );

      TSendTxt stSend;
      stSend.handle = 0;
      TAutoStringRc str_( hInstRC, ID_MSG_30 );
      //stSend.lpTxt = "Move mouse for select width and high of window";
      stSend.lpTxt = str_();

      SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );

      //TMyMDIChild::bFlShiftActive = TRUE;
      pCh->mcsSave = TMDIChSave( *pCh );
      //TMyMDIChild::bFlUserUCSSetupSta = TRUE;
   
      SetCapture( pCh->HWindow );
      //ShowCursor( FALSE );
      SetCursor( LoadCursor(NULL, IDC_ARROW) );

      return;
    }

   RECT r;
   if( TMyMDIChild::staXSh < TMyMDIChild::curXSh )
       {
	 r.left = TMyMDIChild::staXSh; r.right = TMyMDIChild::curXSh;
       }
      else
       {
	 r.left = TMyMDIChild::curXSh; r.right = TMyMDIChild::staXSh;
       }
      if( TMyMDIChild::staYSh < TMyMDIChild::curYSh )
       {
	 r.top = TMyMDIChild::staYSh; r.bottom = TMyMDIChild::curYSh;
       }
      else
       {
	 r.top = TMyMDIChild::curYSh; r.bottom = TMyMDIChild::staYSh;
       }

   if( r.right - r.left < MDI_d_MinDeltaWindowH ) r.right = r.left + MDI_d_MinDeltaWindowH;
   if( r.bottom - r.top < MDI_d_MinDeltaWindowV ) r.bottom = r.top + MDI_d_MinDeltaWindowV;

   //ShowCursor( TRUE );
   ReleaseCapture();

   TMyMDIChild::bFlShiftActive = FALSE;
   pCh->mcsSave.Restore( *pCh );
   TMyMDIChild::phazeOfCurMode = 0;

   if( TMyMDIChild::callAfter ) TMyMDIChild::callAfter( pCh, r );

   TMyMDIChild::bFlFitActive = FALSE;
   TMyMDIChild::bFlSelectWithWindowFrameActive = FALSE;
 }

void GetWinMouseRBHandler( RTMessage msg, PTMyMDIChild pCh )
 {
   if( TMyMDIChild::phazeOfCurMode == 1 )
    {
      ReleaseCapture();
      //ShowCursor( TRUE );
      pCh->mcsSave.Restore( *pCh );
    }
   else pCh->SetModeChild( TMyMDIChild::mcKeyMode );

   TMyMDIChild::bFlShiftActive = FALSE;
   TMyMDIChild::phazeOfCurMode = 0;

   TMyMDIChild::bFlFitActive = FALSE;
   TMyMDIChild::bFlSelectWithWindowFrameActive = FALSE;
 }


void SelectMouseHandler( RTMessage msg, PTMyMDIChild pCh, HDC hdc )
 {
   if( TMyMDIChild::phazeOfCurMode == 0 ) { DefMouseHandler( msg, pCh, hdc ); return; }
 }

void SelectMouseLBHandler( RTMessage msg, PTMyMDIChild pCh )
 {
   pCh->GetScena()->SetHardwareEnviron( (PTUCS)pCh, pCh->hdcOwn, (PTWindow)pCh );
   POINT p;  p.x = pCh->mouseX;  p.y = pCh->mouseY;
   //DPtoLP( pCh->hdcOwn, &p , 1);
   TMyMDIChild::psfSelected = pCh->GetScena()->FindObject( p.x, p.y, TMyMDIChild::pCurFilter );
   //%%%%
				/*
   if( TMyMDIChild::psfSelected )
    {
      pCh->GetScena()->RecalcBodyUserToBase( *TMyMDIChild::psfSelected );
      pCh->GetScena()->RecalcBodyBaseToUser( *TMyMDIChild::psfSelected );
      InvalidateRect( pCh->HWindow, NULL, TRUE );
      return;                     
    }                             */

   if( !TMyMDIChild::psfSelected && !(TMyMDIChild::IsTransformSelected() == TRUE && pCh->GetScena()->SelectedCount(TMyMDIChild::pCurFilter)) )
     return;
                                  

   TMyMDIChild::bFlShiftActive = FALSE;
   TMyMDIChild::phazeOfCurMode = 0;

   pCh->InitMode( TMyMDIChild::mcNextOp );
 }


void SelectMouseRBHandler( RTMessage msg, PTMyMDIChild pCh )
 {
   if( TMyMDIChild::mcKeyMode < MC_MoveBody )
     pCh->SetModeChild( TMyMDIChild::mcKeyMode );
   else pCh->SetModeChild( MC_Default );

  /* TMyMDIChild::psfSelected = NULL;
   TMyMDIChild::bFlShiftActive = FALSE;
   TMyMDIChild::phazeOfCurMode = 0;*/
 }

void MoveMouseHandler( RTMessage msg, PTMyMDIChild pCh, HDC hdc )
 {
   static int bFlLocalLock = FALSE;
  

      char cBuf[128];
      POINT p; p.x = pCh->mouseX; p.y = pCh->mouseY;
      DPtoLP( hdc, &p, 1 );

      if( bFlLocalLock == TRUE )
       {
         bFlLocalLock = FALSE;
	 TMyMDIChild::curXSh = p.x;
	 TMyMDIChild::curYSh = p.y;

	 return;
       }

      T3DPoint pt[2];
      pt[0] = T3DPoint( TMyMDIChild::dDX, TMyMDIChild::dDY, TMyMDIChild::dDZ );
      //pt[1] = T3DPoint( p.x, p.y, 0 );
      if( pCh->GetScena()->IsUsedBaseUSC() == TRUE )
	pCh->RecalcBaseToUser( pt, 1 );

	sprintf( cBuf, "%s: Dx=%5.2f  Dy=%5.2f  Dz=%5.2f",
	  TMyMDIChild::masMoveAxis(),
          float( TGrid::CheckTinyCoord(pt[0].x) ),
          float( TGrid::CheckTinyCoord(pt[0].y) ),
	  float( TGrid::CheckTinyCoord(pt[0].z) )
	  );

      TSendTxt stSend;
      stSend.handle = 2;
      stSend.lpTxt = cBuf;
      SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
     
      if( pCh->ScrollForShowBody(*TMyMDIChild::psfObol) == TRUE )
       {
	 InvalidateRect( pCh->HWindow, NULL, TRUE );
	 UpdateWindow( pCh->HWindow );

	 p.x = pCh->mouseX; p.y = pCh->mouseY;
	 DPtoLP( hdc, &p, 1 );
	 TMyMDIChild::curXSh = p.x;
         TMyMDIChild::curYSh = p.y;
       }

      /*int iROPOld = SetROP2( hdc, R2_XORPEN );
      HPEN hpFon = CreatePen( PS_SOLID, 0, PALETTEINDEX(6) );
      HPEN hpOld = SelectPen( hdc, hpFon );*/   

      if( pCh->bFlRefresh == FALSE )//&&&
       {
	 TMyMDIChild::pscObol->SetHardwareEnviron( (PTUCS)pCh, pCh->hdcOwn, (PTWindow)pCh );
	 TMyMDIChild::pscObol->DrawSpecial();
       }
      else   
	pCh->bFlRefresh = FALSE;

     static const double dCorrect = MDI_d_MoveSpeedCorrect;

     TMyMDIChild::SetMove( dCorrect * double(p.x - TMyMDIChild::curXSh),
			   dCorrect * double(p.y - TMyMDIChild::curYSh) );
     ((PTObol)TMyMDIChild::psfObol)->bFlAbsTransf = TRUE;
     pCh->GetScena()->Move( *TMyMDIChild::psfObol,
       TMyMDIChild::dDX, TMyMDIChild::dDY, TMyMDIChild::dDZ );
     ((PTObol)TMyMDIChild::psfObol)->bFlAbsTransf = FALSE;

     TMyMDIChild::pscObol->SetHardwareEnviron( (PTUCS)pCh, pCh->hdcOwn, (PTWindow)pCh );
     TMyMDIChild::pscObol->DrawSpecial();

     TMyMDIChild::curXSh = p.x;
     TMyMDIChild::curYSh = p.y;



     /*SelectPen( hdc, hpOld );
     SetROP2( hdc, iROPOld );
     DeleteObject( hpFon );*/

     bFlLocalLock = TRUE;
     if( pCh->ReturnCursor( pCh->mouseX, pCh->mouseY ) == FALSE )
       bFlLocalLock = FALSE;
 }

void MoveMouseLBHandler( RTMessage msg, PTMyMDIChild pCh )//WM_LBUTTONDOWN
 {

/*   T3DPoint pt[2];
   pt[0] = T3DPoint( TMyMDIChild::staXSh, TMyMDIChild::staYSh, 0 );
   pt[1] = T3DPoint( TMyMDIChild::curXSh, TMyMDIChild::curYSh, 0 );*/
   //if( pCh->GetScena()->IsUsedBaseUSC() == TRUE )
     //pCh->RecalcBaseToUser( pt, 2 );
   int dx = TMyMDIChild::dDX,
       dy = TMyMDIChild::dDY,
       dz = TMyMDIChild::dDZ;

   /*TMyMDIChild::psfSelected->Move( pt[1].x - pt[0].x, pt[1].y - pt[0].y,
     pt[1].z - pt[0].z );*/

   PTScena pScClon = NULL;

   if( (msg.WParam & MK_SHIFT) == MK_SHIFT )
     pScClon = pCh->GetClone();

   if( pScClon )
    {
      pScClon->MoveAllSelected( dx, dy, dz );
      pCh->GetScena()->Merge( *pScClon );
      delete pScClon;
    }
   else
    {
      if( TMyMDIChild::IsTransformSelected() == TRUE && pCh->GetScena()->SelectedCount(TMyMDIChild::pCurFilter) )
	pCh->GetScena()->MoveAllSelected( dx, dy, dz );

      else
      if( TMyMDIChild::IsTransformAsTransforming() == TRUE )
       {
         if( TMyMDIChild::psfSelected->IsSelected() == FALSE )
	   pCh->GetScena()->Move( *TMyMDIChild::psfSelected, dx, dy, dz );

	 pCh->GetScena()->MoveAllSelected( dx, dy, dz );

       }
      else
	pCh->GetScena()->Move( *TMyMDIChild::psfSelected, dx, dy, dz );

    }


   pCh->SetCursorInCenter( TMyMDIChild::psfObol );
   if( TMyMDIChild::pscObol ) delete TMyMDIChild::pscObol;
   TMyMDIChild::pscObol = NULL;
   TMyMDIChild::psfObol = NULL;

   pCh->SetModeChild( TMyMDIChild::mcKeyMode );

   TMyMDIChild::bFlShiftActive = FALSE;
   TMyMDIChild::phazeOfCurMode = 0;


   ReleaseCapture();
   ShowCursor( TRUE );

   pCh->RedrawVports();
 }

void MoveMouseRBHandler( RTMessage msg, PTMyMDIChild pCh )
 {
   ReleaseCapture();
   ShowCursor( TRUE );

   pCh->SetCursorInCenter( TMyMDIChild::psfObol );
   if( TMyMDIChild::pscObol ) delete TMyMDIChild::pscObol;
   TMyMDIChild::pscObol = NULL;
   TMyMDIChild::psfObol = NULL;

   pCh->SetModeChild( TMyMDIChild::mcKeyMode );

   TMyMDIChild::bFlShiftActive = FALSE;
   TMyMDIChild::phazeOfCurMode = 0;

   InvalidateRect( pCh->HWindow, NULL, TRUE );
 }


void DelMouseHandler( RTMessage, PTMyMDIChild, HDC hdc )
 {
 }

void DelMouseLBHandler( RTMessage, PTMyMDIChild pCh )
 {
 }

void DelMouseRBHandler( RTMessage, PTMyMDIChild pCh )
 {
 }


void Sc3MouseHandler( RTMessage, PTMyMDIChild pCh, HDC hdc )
 {
   static int bFlLocalLock = FALSE;  


      static const double dCorrect = MDI_d_ScaleSpeedCorrect;

      char cBuf[50];
      POINT p; p.x = pCh->mouseX; p.y = pCh->mouseY;
      DPtoLP( hdc, &p, 1 );

      if( bFlLocalLock == TRUE )
       {
         bFlLocalLock = FALSE;
	 TMyMDIChild::curXSh = p.x;
         TMyMDIChild::curYSh = p.y;

	 return;
       }

      if( fabs(p.x - TMyMDIChild::curXSh) > fabs(p.y - TMyMDIChild::curYSh) )
	TMyMDIChild::SetScale( dCorrect * double(p.x - TMyMDIChild::curXSh) );
	//TMyMDIChild::dScale += dCorrect * double(p.x - TMyMDIChild::curXSh);
      else
	//TMyMDIChild::dScale += dCorrect * double(p.y - TMyMDIChild::curYSh);
	TMyMDIChild::SetScale( dCorrect * double(p.y - TMyMDIChild::curYSh) );

      //if( TMyMDIChild::dScale < 0.001 ) TMyMDIChild::dScale = 0.001;
      sprintf( cBuf, "New %s scale = %5.0f%%",
	TMyMDIChild::sasScaleAxis(),
	TMyMDIChild::GetScaleSys() );

      TSendTxt stSend;
      stSend.handle = 2;
      stSend.lpTxt = cBuf;
      SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
     
      
      if( pCh->ScrollForShowBody(*TMyMDIChild::psfObol) == TRUE )
       {
	 InvalidateRect( pCh->HWindow, NULL, TRUE );
	 UpdateWindow( pCh->HWindow );

	 p.x = pCh->mouseX; p.y = pCh->mouseY;
	 DPtoLP( hdc, &p, 1 );
	 TMyMDIChild::curXSh = p.x;
         TMyMDIChild::curYSh = p.y;
       }


      /*int iROPOld = SetROP2( hdc, R2_XORPEN );
      HPEN hpFon = CreatePen( PS_SOLID, 0, PALETTEINDEX(6) );
      HPEN hpOld = SelectPen( hdc, hpFon );*/   

      if( pCh->bFlRefresh == FALSE )
       {
	 TMyMDIChild::pscObol->SetHardwareEnviron( (PTUCS)pCh, pCh->hdcOwn, (PTWindow)pCh );
	 TMyMDIChild::pscObol->DrawSpecial();
       }
      else   
	pCh->bFlRefresh = FALSE;

     ((PTObol)TMyMDIChild::psfObol)->bFlAbsTransf = TRUE;
     pCh->ScaleFunc( *TMyMDIChild::psfObol );
     ((PTObol)TMyMDIChild::psfObol)->bFlAbsTransf = FALSE;

     TMyMDIChild::pscObol->SetHardwareEnviron( (PTUCS)pCh, pCh->hdcOwn, (PTWindow)pCh );
     TMyMDIChild::pscObol->DrawSpecial();

     TMyMDIChild::curXSh = p.x;
     TMyMDIChild::curYSh = p.y;



     /*SelectPen( hdc, hpOld );
     SetROP2( hdc, iROPOld );
     DeleteObject( hpFon );*/


     bFlLocalLock = TRUE;
     if( pCh->ReturnCursor( pCh->mouseX, pCh->mouseY ) == FALSE )
       bFlLocalLock = FALSE;
 }

void Sc3MouseLBHandler( RTMessage msg, PTMyMDIChild pCh )
 {

   PTScena pScClon = NULL;
   T3DPoint pt;
   
   if( TMyMDIChild::IsTransformSelected() == TRUE && pCh->GetScena()->SelectedCount(TMyMDIChild::pCurFilter) )
     pt = pCh->GetBP();
   else pt = pCh->GetBP( TMyMDIChild::psfSelected );

   if( (msg.WParam & MK_SHIFT) == MK_SHIFT )
     pScClon = pCh->GetClone();

   if( pScClon )
    {
      pScClon->ScaleXYZAllSelected(
        TMyMDIChild::dScaleX,
        TMyMDIChild::dScaleY,
        TMyMDIChild::dScaleZ,
	pt );

      pCh->GetScena()->Merge( *pScClon );
      delete pScClon;
    }
   else
    {
      if( TMyMDIChild::IsTransformSelected() == TRUE && pCh->GetScena()->SelectedCount(TMyMDIChild::pCurFilter) )
        pCh->GetScena()->ScaleXYZAllSelected(
          TMyMDIChild::dScaleX,
          TMyMDIChild::dScaleY,
          TMyMDIChild::dScaleZ,
	  pt );
      else if( TMyMDIChild::IsTransformAsTransforming() == TRUE )
      {
        if( TMyMDIChild::psfSelected->IsSelected() == FALSE )
	  pCh->GetScena()->ScaleXYZ( *TMyMDIChild::psfSelected,
	    TMyMDIChild::dScaleX,
	    TMyMDIChild::dScaleY,
            TMyMDIChild::dScaleZ,
	    pt );
				
        pCh->GetScena()->ScaleXYZAllSelected( 
	  TMyMDIChild::dScaleX,
	  TMyMDIChild::dScaleY,
	  TMyMDIChild::dScaleZ,
          pt );
      }
     else
       pCh->GetScena()->ScaleXYZ( *TMyMDIChild::psfSelected,
         TMyMDIChild::dScaleX,
         TMyMDIChild::dScaleY,
         TMyMDIChild::dScaleZ,
         pt );
    }


   pCh->SetCursorInCenter( TMyMDIChild::psfObol );
   if( TMyMDIChild::pscObol ) delete TMyMDIChild::pscObol;
   TMyMDIChild::pscObol = NULL;
   TMyMDIChild::psfObol = NULL;

   pCh->SetModeChild( TMyMDIChild::mcKeyMode );

   TMyMDIChild::bFlShiftActive = FALSE;
   TMyMDIChild::phazeOfCurMode = 0;

   ReleaseCapture();
   ShowCursor( TRUE );

   pCh->RedrawVports();
 }

void Sc3MouseRBHandler( RTMessage, PTMyMDIChild pCh )
 {
   ReleaseCapture();
   ShowCursor( TRUE );

   pCh->SetCursorInCenter( TMyMDIChild::psfObol );
   if( TMyMDIChild::pscObol ) delete TMyMDIChild::pscObol;
   TMyMDIChild::pscObol = NULL;
   TMyMDIChild::psfObol = NULL;

   pCh->SetModeChild( TMyMDIChild::mcKeyMode );

   TMyMDIChild::bFlShiftActive = FALSE;
   TMyMDIChild::phazeOfCurMode = 0;

   InvalidateRect( pCh->HWindow, NULL, TRUE );
 }

void Sc2MouseHandler( RTMessage, PTMyMDIChild pCh, HDC hdc )
 {
   static int bFlLocalLock = FALSE;


      static const double dCorrect = MDI_d_Scale2DSpeedCorrect;

      char cBuf[50];
      POINT p; p.x = pCh->mouseX; p.y = pCh->mouseY;
      DPtoLP( hdc, &p, 1 );

      if( bFlLocalLock == TRUE )
       {
         bFlLocalLock = FALSE;
	 TMyMDIChild::curXSh = p.x;
         TMyMDIChild::curYSh = p.y;

	 return;
       }

      if( fabs(p.x - TMyMDIChild::curXSh) > fabs(p.y - TMyMDIChild::curYSh) )
	TMyMDIChild::dScaleX += dCorrect * double(p.x - TMyMDIChild::curXSh);
      else
	TMyMDIChild::dScaleX += dCorrect * double(p.y - TMyMDIChild::curYSh);

      if( fabs(TMyMDIChild::dScaleX) < MDI_d_MinScaleAbs ) TMyMDIChild::dScaleX = MDI_d_MinScaleAbs;
      sprintf( cBuf, "New 2D scale = %5.0f%%", TMyMDIChild::dScaleX );

      TSendTxt stSend;
      stSend.handle = 2;
      stSend.lpTxt = cBuf;
      SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );
     

      if( pCh->ScrollForShowBody(*TMyMDIChild::psfObol) == TRUE )
       {
	 InvalidateRect( pCh->HWindow, NULL, TRUE );
	 UpdateWindow( pCh->HWindow );

	 p.x = pCh->mouseX; p.y = pCh->mouseY;
	 DPtoLP( hdc, &p, 1 );
	 TMyMDIChild::curXSh = p.x;
         TMyMDIChild::curYSh = p.y;
       }


      /*int iROPOld = SetROP2( hdc, R2_XORPEN );
      HPEN hpFon = CreatePen( PS_SOLID, 0, PALETTEINDEX(6) );
      HPEN hpOld = SelectPen( hdc, hpFon );*/   

      if( pCh->bFlRefresh == FALSE )
       {
	 TMyMDIChild::pscObol->SetHardwareEnviron( (PTUCS)pCh, pCh->hdcOwn, (PTWindow)pCh );
	 TMyMDIChild::pscObol->DrawSpecial();
       }
      else   
	pCh->bFlRefresh = FALSE;

     ((PTObol)TMyMDIChild::psfObol)->bFlAbsTransf = TRUE;
     pCh->GetScena()->Scale2D( *TMyMDIChild::psfObol, TMyMDIChild::dScaleX, pCh->GetBP((PTObol)TMyMDIChild::psfObol) );
     ((PTObol)TMyMDIChild::psfObol)->bFlAbsTransf = FALSE;

     TMyMDIChild::pscObol->SetHardwareEnviron( (PTUCS)pCh, pCh->hdcOwn, (PTWindow)pCh );
     TMyMDIChild::pscObol->DrawSpecial();

     TMyMDIChild::curXSh = p.x;
     TMyMDIChild::curYSh = p.y;


     /*SelectPen( hdc, hpOld );
     SetROP2( hdc, iROPOld );
     DeleteObject( hpFon );*/    

     bFlLocalLock = TRUE;
     if( pCh->ReturnCursor( pCh->mouseX, pCh->mouseY ) == FALSE )
       bFlLocalLock = FALSE;
 }



void Sc2MouseLBHandler( RTMessage msg, PTMyMDIChild pCh )
 {   
   pCh->GetScena()->SetHardwareEnviron( (PTUCS)pCh, pCh->hdcOwn, (PTWindow)pCh );

   
   PTScena pScClon = NULL;
   T3DPoint pt;
   
   if( TMyMDIChild::IsTransformSelected() == TRUE && pCh->GetScena()->SelectedCount(TMyMDIChild::pCurFilter) )
     pt = pCh->GetBP();
   else pt = pCh->GetBP( TMyMDIChild::psfSelected );

   if( (msg.WParam & MK_SHIFT) == MK_SHIFT )
     pScClon = pCh->GetClone();

   if( pScClon )
    {
      pScClon->SetHardwareEnviron( (PTUCS)pCh, pCh->hdcOwn, (PTWindow)pCh );
      pScClon->Scale2DAllSelected( TMyMDIChild::dScaleX, pt );

      pCh->GetScena()->Merge( *pScClon );
      delete pScClon;
    }
   else
    {
      if( TMyMDIChild::IsTransformSelected() == TRUE && pCh->GetScena()->SelectedCount(TMyMDIChild::pCurFilter) )
        pCh->GetScena()->Scale2DAllSelected( TMyMDIChild::dScaleX,
          pt );
      else if( TMyMDIChild::IsTransformAsTransforming() == TRUE )
       {
         if( TMyMDIChild::psfSelected->IsSelected() == FALSE )
	   pCh->GetScena()->Scale2D( *TMyMDIChild::psfSelected, TMyMDIChild::dScaleX,
	     pt );
         pCh->GetScena()->Scale2DAllSelected( TMyMDIChild::dScaleX,
	   pt );
       }
      else
        pCh->GetScena()->Scale2D( *TMyMDIChild::psfSelected, TMyMDIChild::dScaleX,
	 pt );
    }

   pCh->SetCursorInCenter( TMyMDIChild::psfObol );
   if( TMyMDIChild::pscObol ) delete TMyMDIChild::pscObol;
   TMyMDIChild::pscObol = NULL;
   TMyMDIChild::psfObol = NULL;

   pCh->SetModeChild( TMyMDIChild::mcKeyMode );

   TMyMDIChild::bFlShiftActive = FALSE;
   TMyMDIChild::phazeOfCurMode = 0;

   ReleaseCapture();
   ShowCursor( TRUE );

   pCh->RedrawVports();
 }

void Sc2MouseRBHandler( RTMessage, PTMyMDIChild pCh )
 {
   ReleaseCapture();
   ShowCursor( TRUE );

   pCh->SetCursorInCenter( TMyMDIChild::psfObol );
   if( TMyMDIChild::pscObol ) delete TMyMDIChild::pscObol;
   TMyMDIChild::pscObol = NULL;
   TMyMDIChild::psfObol = NULL;

   pCh->SetModeChild( TMyMDIChild::mcKeyMode );

   TMyMDIChild::bFlShiftActive = FALSE;
   TMyMDIChild::phazeOfCurMode = 0;

   InvalidateRect( pCh->HWindow, NULL, TRUE );
 }

void RotMouseHandler( RTMessage msg, PTMyMDIChild pCh, HDC hdc)
 {
      static int bFlLocalLock = FALSE;


      static const double dCorrect = MDI_d_RotateSpeedCorrect;

      char cBuf[50];
      POINT p; p.x = pCh->mouseX; p.y = pCh->mouseY;
      DPtoLP( hdc, &p, 1 );

      if( bFlLocalLock == TRUE )
      {
	bFlLocalLock = FALSE;
	TMyMDIChild::curXSh = p.x;
        TMyMDIChild::curYSh = p.y;

	return;
      }

      if( fabs(p.x - TMyMDIChild::curXSh) > fabs(p.y - TMyMDIChild::curYSh) )
	TMyMDIChild::SetAngle( dCorrect * double(p.x - TMyMDIChild::curXSh) );
      else
	TMyMDIChild::SetAngle( dCorrect * double(p.y - TMyMDIChild::curYSh) );

      sprintf( cBuf, "New %s angle = %5.0f", TMyMDIChild::resCurExis(), float(TMyMDIChild::GetAngle()) );

      TSendTxt stSend;
      stSend.handle = 2;
      stSend.lpTxt = cBuf;
      SendMessage( pCh->GetApplication()->MainWindow->HWindow, WM_COMMAND, CM_SENDTEXT, (LPARAM)&stSend );


      if( pCh->ScrollForShowBody(*TMyMDIChild::psfObol) == TRUE )
       {
	 InvalidateRect( pCh->HWindow, NULL, TRUE );
	 UpdateWindow( pCh->HWindow );

	 p.x = pCh->mouseX; p.y = pCh->mouseY;
	 DPtoLP( hdc, &p, 1 );
	 TMyMDIChild::curXSh = p.x;
         TMyMDIChild::curYSh = p.y;
       }


      /*int iROPOld = SetROP2( hdc, R2_XORPEN );
      HPEN hpFon = CreatePen( PS_SOLID, 0, PALETTEINDEX(6) );
      HPEN hpOld = SelectPen( hdc, hpFon );*/   

      if( pCh->bFlRefresh == FALSE )
       {
	 TMyMDIChild::pscObol->SetHardwareEnviron( (PTUCS)pCh, pCh->hdcOwn, (PTWindow)pCh );
	 TMyMDIChild::pscObol->DrawSpecial();
       }
      else   
	pCh->bFlRefresh = FALSE;

     ((PTObol)TMyMDIChild::psfObol)->bFlAbsTransf = TRUE;
     pCh->RotFunc( *TMyMDIChild::psfObol );
     ((PTObol)TMyMDIChild::psfObol)->bFlAbsTransf = FALSE;


     TMyMDIChild::pscObol->SetHardwareEnviron( (PTUCS)pCh, pCh->hdcOwn, (PTWindow)pCh );
     TMyMDIChild::pscObol->DrawSpecial();

     TMyMDIChild::curXSh = p.x;
     TMyMDIChild::curYSh = p.y;

     /*SelectPen( hdc, hpOld );
     SetROP2( hdc, iROPOld );
     DeleteObject( hpFon );*/

     
     bFlLocalLock = TRUE;
     if( pCh->ReturnCursor( pCh->mouseX, pCh->mouseY ) == FALSE )
       bFlLocalLock = FALSE;
 }

void RotMouseLBHandler( RTMessage msg, PTMyMDIChild pCh )
 {
   
   PTScena pScClon = NULL;
   T3DPoint pt;
   
   if( TMyMDIChild::IsTransformSelected() == TRUE && pCh->GetScena()->SelectedCount(TMyMDIChild::pCurFilter) )
     pt = pCh->GetBP();
   else pt = pCh->GetBP( TMyMDIChild::psfSelected );

   if( (msg.WParam & MK_SHIFT) == MK_SHIFT )
     pScClon = pCh->GetClone();

   if( pScClon )
    {
      pScClon->RotateXYZAllSelected( TMyMDIChild::anX,
			             TMyMDIChild::anY,
				     TMyMDIChild::anZ,
				     pt
				   );

      pCh->GetScena()->Merge( *pScClon );
      delete pScClon;
    }
   else
    {
      if( TMyMDIChild::IsTransformSelected() == TRUE && pCh->GetScena()->SelectedCount(TMyMDIChild::pCurFilter) )
	pCh->GetScena()->RotateXYZAllSelected( TMyMDIChild::anX,
					       TMyMDIChild::anY,
					       TMyMDIChild::anZ,
					       pt
					     );
      else if( TMyMDIChild::IsTransformAsTransforming() == TRUE )
       {
         if( TMyMDIChild::psfSelected->IsSelected() == FALSE )
	   pCh->GetScena()->RotateXYZ( *TMyMDIChild::psfSelected,
	                               TMyMDIChild::anX,
				       TMyMDIChild::anY,
				       TMyMDIChild::anZ,
				       pt
				     );
	 pCh->GetScena()->RotateXYZAllSelected( TMyMDIChild::anX,
					        TMyMDIChild::anY,
					        TMyMDIChild::anZ,
					        pt
					      );
       }
      else
        pCh->GetScena()->RotateXYZ( *TMyMDIChild::psfSelected,
	                            TMyMDIChild::anX,
			            TMyMDIChild::anY,
			            TMyMDIChild::anZ,
				    pt
			          );
    }

   pCh->SetCursorInCenter( TMyMDIChild::psfObol );
   if( TMyMDIChild::pscObol ) delete TMyMDIChild::pscObol;
   TMyMDIChild::pscObol = NULL;
   TMyMDIChild::psfObol = NULL;

   pCh->SetModeChild( TMyMDIChild::mcKeyMode );

   TMyMDIChild::bFlShiftActive = FALSE;
   TMyMDIChild::phazeOfCurMode = 0;

   ReleaseCapture();
   ShowCursor( TRUE );

   pCh->RedrawVports();
 }

void RotMouseRBHandler( RTMessage, PTMyMDIChild pCh )
 {
   ReleaseCapture();
   ShowCursor( TRUE );

   pCh->SetCursorInCenter( TMyMDIChild::psfObol );
   if( TMyMDIChild::pscObol ) delete TMyMDIChild::pscObol;
   TMyMDIChild::pscObol = NULL;
   TMyMDIChild::psfObol = NULL;

   pCh->SetModeChild( TMyMDIChild::mcKeyMode );

   TMyMDIChild::bFlShiftActive = FALSE;
   TMyMDIChild::phazeOfCurMode = 0;

   InvalidateRect( pCh->HWindow, NULL, TRUE );
 }
