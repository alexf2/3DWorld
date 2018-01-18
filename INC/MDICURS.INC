#if !defined(_MDI_CURS_)
#define _MDI_CURS_

#define CSR_FIRST 300
#define CSR_FIRST_OTHER 350

#define CSR_OT_Disabled   (CSR_FIRST_OTHER + 1)
#define CSR_OT_Hand       (CSR_FIRST_OTHER + 2)

#define CSR_Default       (-1)
#define CSR_SetupUCS      (CSR_Default)
#define CSR_ShiftUCS      (CSR_FIRST + 3)
#define CSR_CreCylinder   (CSR_FIRST + 4)
#define CSR_Cone          (CSR_FIRST + 5)
#define CSR_Sphere        (CSR_FIRST + 6)
#define CSR_Torus         (CSR_FIRST + 7)
#define CSR_Tube          (CSR_FIRST + 8)
#define CSR_GetWindow     (CSR_FIRST + 9)

#define CSR_SelectObj     (CSR_FIRST + 10)
#define CSR_MoveObj       (CSR_Default)
#define CSR_DelObj        (CSR_Default)
#define CSR_3DScaleObj    (CSR_Default)
#define CSR_2DScaleObj    (CSR_Default)
#define CSR_RotateBody    (CSR_Default)
#define CSR_SelectBody    (CSR_SelectObj)

#define CSR_GetPoint      (CSR_FIRST + 11)
#define CSR_SetupGlobalBP (CSR_GetPoint)

#define CSR_Omni          (CSR_FIRST + 12)

#endif
