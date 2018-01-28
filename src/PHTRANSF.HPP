#if !defined(_PHOTOTRANSFER_)
#define _PHOTOTRANSFER_

enum TTraceModel { TM_Local, TM_GlobalHolls };
enum TShadingAlho { SA_Flat, SA_Phong };
enum TPolyhonalMethod { PM_RayTracing = 0, PM_StringScan = 1 };


struct TPhotoSetupTransfer
 {
   BOOL bFlAntiAliasing, bFlShadows, bFlRenderBack;
   int iNumbPointsAntiAlias;
   double dStrengtsAntialias;

   TShadingAlho saShadingLimit;
   TTraceModel tmTraceModel;
   TPolyhonalMethod polyMethod;

   BOOL bFlStrictGamma;
 };

typedef TPhotoSetupTransfer* PTPhotoSetupTransfer;
typedef TPhotoSetupTransfer& RTPhotoSetupTransfer;

#endif
