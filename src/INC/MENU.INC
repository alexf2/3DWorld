#if !defined(_MENU_COMMANDS_)
#define _MENU_COMMANDS_

#if !defined(CM_EXIT)
  #include <owlrc.h>
#endif


#define  UCM_FILEACESS_FIRST       100

#define  UCM_SYSTOOLBAR_HIDE       0x8840

#define  UCM_ABOUT                 20
#define  UCM_SYSABOUT              0x8880 
#define  UCM_EXIT                  CM_EXIT
#define  UCM_CANCEL                21
#define  UCM_DEFAULTPOS_MAINWINDOW 0x8870


#define  UCM_OPEN                  23
#define  UCM_NEW_FILE              24
#define  UCM_SAVE                  25
#define  UCM_SAVE_AS               26
#define  UCM_CLOSE                 27
#define  UCM_CLOSE_ALL             CM_CLOSECHILDREN
#define  UCM_RETRIVE_SOURCE        29

#define  UCM_CLOSE_SCENE           31
#define  UCM_GETINFO               32

#define  UCM_RESTORE_CHILD_DIV     0x8860
#define  UCM_ADAPTIVE_DIVISION     0x8850
#define  UCM_AUTO_FIT              0x8840
#define  UCM_RESTORE_VIEW          0x8830
#define  UCM_SET_POPUPFLAG         0x8820
#define UCM_FULLSCREEN_BMP         0x8810

#define  UCM_SELECT_SCRVPORTS      33
#define  UCM_GRID_EXTENTS          34
#define  UCM_USE_SNAP              35
#define  UCM_USE_GRID              36
#define  UCM_USERUCS_SETUP         37
#define  UCM_AXIS_ON_OFF           38

#define  UCM_INTERNAL_ACTIVATE_CHILD 39
#define  UCM_INTERNAL_FOCUS_CHILD    40


#define  UCM_SYSTOOLBAR_DESTROYED    50

#define  UCM_CREATE_SIMPLEOBJ        51
#define  UCM_HIDEGEOMETRY            52
#define  UCM_FULLDETALL              53

#define  UCM_SCENEINFO               54

#define  UCM_MOVEBODY                55
#define  UCM_ROTATEBODY              56
#define  UCM_2DSCALEBODY             57
#define  UCM_3DSCALEBODY             58
#define  UCM_DELETEBODY              59
#define  UCM_COPYBODY                60
#define  UCM_PASTEBODY               61

#define  UCM_MDINEXT                 62
#define  UCM_MDIPREV                 63
#define  UCM_ZOOM                    64
#define  UCM_UNZOOM                  65


#define  UCM_CHANGED_CHILD_MODE      78

#define  UCM_TOGGLE_VPORTTOOL        79
#define  UCM_TOGGLE_STATUSBAR        80

#define  UCM_RENDER                  81

#define  UCM_SET_CHECKED_IN_MENU     82

#define  UCM_SELECT                  83
#define  UCM_SELECTWINDOW            84
#define  UCM_SELECTALL               85
#define  UCM_INVERTSELMODE           86

#define  UCM_GET_INVERTSELMODE_FLAG  87
#define  UCM_GET_MAINPATH            88

#define  UCM_CUTBODY                 89


#define  SYSTOOL_FIRST_COMMAND       90

#define  UCM_TB_SETUP_UCS            (SYSTOOL_FIRST_COMMAND + 1)
#define  UCM_TB_SHIFT000             (SYSTOOL_FIRST_COMMAND + 2)
#define  UCM_TB_SCALE_VIEW_DOWN      (SYSTOOL_FIRST_COMMAND + 3)
#define  UCM_TB_SCALE_VIEW_UP        (SYSTOOL_FIRST_COMMAND + 4)
#define  UCM_TB_FIT_TO_ZOOM          (SYSTOOL_FIRST_COMMAND + 5)
#define  UCM_TB_FIT_TO_WINDOW        (SYSTOOL_FIRST_COMMAND + 6)
#define  UCM_TB_HOLD                 (SYSTOOL_FIRST_COMMAND + 7)
#define  UCM_TB_FETCH                (SYSTOOL_FIRST_COMMAND + 8)

#define  UCM_TB_CREATE_SIMPLEOBJ     (SYSTOOL_FIRST_COMMAND + 9)
#define  UCM_TB_MOVEBODY             (SYSTOOL_FIRST_COMMAND + 10)
#define  UCM_TB_ROTATEBODY           (SYSTOOL_FIRST_COMMAND + 11)
#define  UCM_TB_2DSCALEBODY          (SYSTOOL_FIRST_COMMAND + 12)
#define  UCM_TB_3DSCALEBODY          (SYSTOOL_FIRST_COMMAND + 13)
#define  UCM_TB_DELETEBODY           (SYSTOOL_FIRST_COMMAND + 14)

#define  UCM_TB_SELECT               (SYSTOOL_FIRST_COMMAND + 15)
#define  UCM_TB_SELECTWINDOW         (SYSTOOL_FIRST_COMMAND + 16)
#define  UCM_TB_INVERTSELMODE        (SYSTOOL_FIRST_COMMAND + 17)

#define  UCM_TB_COPY                 (SYSTOOL_FIRST_COMMAND + 18)
#define  UCM_TB_CUT                  (SYSTOOL_FIRST_COMMAND + 19)
#define  UCM_TB_PASTE                (SYSTOOL_FIRST_COMMAND + 20)

#define  UCM_TB_CONECT               (SYSTOOL_FIRST_COMMAND + 21)
#define  UCM_TB_GROUP                (SYSTOOL_FIRST_COMMAND + 22)

#define  UCM_TB_USE_BASEUCS          (SYSTOOL_FIRST_COMMAND + 23)

#define  SYSTOOL_END_COMMAND         (SYSTOOL_FIRST_COMMAND + 24)

#define  UCM_F_ZOOM100               120
#define  UCM_F_ZOOM200               121
#define  UCM_F_ZOOM300               122
#define  UCM_F_ZOOM400               123
#define  UCM_F_ZOOM500               124
#define  UCM_F_ZOOM75                125
#define  UCM_F_ZOOM50                126
#define  UCM_F_ZOOM25                127
#define  UCM_F_ZOOM10                128
#define  UCM_F_ZOOM5                 129
#define  UCM_F_FIT_TO_ZOOM           130

#define  UCM_INVERTSELECTING         131

#define  UCM_SET_BP_VISIBLE          132
#define  UCM_USE_GLOBAL_BP           133
#define  UCM_SETUP_GLOBAL_BP         134

#define  UCM_USE_CROSSING_FRAME      135

#define  UCM_ASSIGN_MATERIAL         136
#define  UCM_SAVE_MEDIUM             137
#define  UCM_LOAD_MEDIUM             138

#define  UCM_ALIGMENT_VERTEX         139
#define  UCM_ALIGMENT_FACET          140

#define  UCM_USE_BASEUCS             141

#define UCM_AMBIENT                  142
#define UCM_OMNI_CREATE              143
#define UCM_OMNI_ADJUST	             144

#define UCM_CAMERA_CREATE            145
#define UCM_CAMERA_ADJUST            146
#define UCM_CAMERA_MOVE              147
#define UCM_CAMERA_ROLL              148
#define UCM_CAMERA_AOV               149
#define UCM_CAMERA_DOLLY             150

#define UCM_BKGND_SETUP              151
#define UCM_IMAGE_OPTIONS            152

#define UCM_VPORT_ALIGN              153
#define UCM_RENDER_FULL              154

#define UCM_PLACEHILITE              155

#define UCM_SMOTH                    156

#define UCM_LOGGING                  157

#define UCM_COPY_BMP_CLIPBOARD       158           

#endif
