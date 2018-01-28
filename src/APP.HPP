#if !defined(_APP_)
#define _APP_

#include <owl.h>
#include <bwcc.h>

#include "INC\errors.inc"
#include "INC\dlg.inc"


const UINT cErrors = 17;
enum  TypeMsg { msError, msWarning };
typedef struct
 {
   BOOL     ErrCode;
   TypeMsg  TypeError;
   UINT     idStringMessage;

 } TAppError;

class  T3dWorldApp: public TApplication
 {
public:
  T3dWorldApp( LPSTR AppName, HINSTANCE hInstance, HINSTANCE hPrevInstance,
	       LPSTR lpCmdLine, int nCmdShow );


  virtual void Error(int ErrorCode);
  virtual BOOL CanClose();

protected:
  virtual void InitMainWindow();
  virtual void InitInstance();

 };



#endif
