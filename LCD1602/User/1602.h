
#define Clear_Screen  0x01//����ʾ
#define Cursor_Home   0x02//��0x03��������Ƶ���Ļԭ��λ�ã����Ͻǣ�
#define Mode_Set      0x04//ģʽ���ã�D0=0��ʾ���ƶ���D1=0�������
#define Cursor_Shift_right      0x02//������ƣ����Mode_Setʹ�ã�Mode_Set+Cursor_Shift_right
#define Display_Shift           0x01//��ʾ�ƶ������Mode_Setʹ�ã�Mode_Set+Display_Shift/Mode_Set+Cursor_Shift_right+Display_Shift
#define Display_OnOff 0x08//��ʾ���ؿ��ƣ�D2=0�أ�D1=0��겻��ʾ��D0=0��겻��˸
#define Display_On              0x04//D2=1��ʾ��
#define Display_Cursor          0x02//D1=1��ʾ���
#define Cursor_Blink            0x01//D0=1�����˸
#define Shift         0x10//���/��ʾ��λ��D3=0��ʾ���ƶ�������ƶ���D2=0�ƶ���������
#define Shift_right             0x04//�ƶ���������
#define Shift_Display           0x08//��ʾ�ƶ�����겻�ƶ�
#define Function_Set  0x20//�����趨��D4=0��λ���ݽӿڣ�D7-D4����D3=0һ����ʾ��D2=0 5x8�����ַ�
#define Data_Interface_8        0x10//D4=1��λ���ݽӿڣ�D7-D0��
#define Double_Line_Display     0x08//������ʾ
#define Character_Shape_10      0x04//5x10�ַ�
#define Set_CGRAM_Address  0x40//����CGRAM��ַ
#define Set_DDRAM_Address  0x80//����DDRAM��ַ
