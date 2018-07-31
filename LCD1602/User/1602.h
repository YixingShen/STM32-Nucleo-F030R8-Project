
#define Clear_Screen  0x01//清显示
#define Cursor_Home   0x02//或0x03，将光标移到屏幕原点位置（左上角）
#define Mode_Set      0x04//模式设置，D0=0显示不移动，D1=0光标左移
#define Cursor_Shift_right      0x02//光标右移，结合Mode_Set使用，Mode_Set+Cursor_Shift_right
#define Display_Shift           0x01//显示移动，结合Mode_Set使用，Mode_Set+Display_Shift/Mode_Set+Cursor_Shift_right+Display_Shift
#define Display_OnOff 0x08//显示开关控制，D2=0关，D1=0光标不显示，D0=0光标不闪烁
#define Display_On              0x04//D2=1显示打开
#define Display_Cursor          0x02//D1=1显示光标
#define Cursor_Blink            0x01//D0=1光标闪烁
#define Shift         0x10//光标/显示移位，D3=0显示不移动，光标移动；D2=0移动方向：左移
#define Shift_right             0x04//移动方向：右移
#define Shift_Display           0x08//显示移动，光标不移动
#define Function_Set  0x20//功能设定，D4=0四位数据接口（D7-D4），D3=0一行显示，D2=0 5x8点阵字符
#define Data_Interface_8        0x10//D4=1八位数据接口（D7-D0）
#define Double_Line_Display     0x08//两行显示
#define Character_Shape_10      0x04//5x10字符
#define Set_CGRAM_Address  0x40//设置CGRAM地址
#define Set_DDRAM_Address  0x80//设置DDRAM地址
