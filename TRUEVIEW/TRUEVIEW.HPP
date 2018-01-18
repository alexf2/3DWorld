#include <owl.h>

const UINT cErrors = 17;
enum  TypeMsg { msError, msWarning };
typedef struct
 {
   BOOL     ErrCode;
   TypeMsg  TypeError;
   UINT     idStringMessage;

 } TAppError;

_CLASSDEF(TTrueViewApp)

class  TTrueViewApp: public TApplication
 {
public:
  TTrueViewApp( LPSTR AppName, HINSTANCE hInstance, HINSTANCE hPrevInstance,
	       LPSTR lpCmdLine, int nCmdShow );
  ~TTrueViewApp();

  virtual void Error( int ErrorCode );

protected:
  virtual void InitMainWindow();

private:
  LPSTR lpInitPath;

 };
