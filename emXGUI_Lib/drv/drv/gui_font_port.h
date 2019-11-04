#ifndef __GUI_FONT_PORT_H
#define	__GUI_FONT_PORT_H

#ifdef __cplusplus
extern "C" {
#endif 
  
#include	"emXGUI.h"
#include	"gui_drv_cfg.h"
  
  /** 
  * @brief  �������
  *  Ϊ��Ӧ��ͬ�ֱ��ʵ���Ļ��ʹ�ò�ͬ��Ĭ������ 
  */   
typedef struct
{
  const char *default_en;  //Ĭ��Ӣ������
  const char *default_extern_cn;  //Ĭ����������
  
  const char *default_extern_logo50;  //LOGO����
  const char *default_extern_logo100;  //LOGO����
  const char *default_extern_logo252;  //LOGO����
  
  const char *default_extern_ctrl32;  //�������壨32��
  const char *default_extern_ctrl48;  //�������壨48��  
  const char *default_extern_ctrl64;  //�������壨64�� 
  const char *default_extern_ctrl72;  //�������壨72��  
  const char *default_extern_ctrl100;  //�������壨100��  
}FONT_PARAM_TypeDef;

/* ����������� */
extern const FONT_PARAM_TypeDef gui_font_param[];

/* Ĭ������ */
extern HFONT defaultFont;
extern HFONT logoFont50;
extern HFONT logoFont100;
extern HFONT logoFont252;

extern HFONT ctrlFont32;
extern HFONT ctrlFont48;
extern HFONT ctrlFont64;
extern HFONT ctrlFont72;
extern HFONT ctrlFont100;
/* Ĭ��Ӣ������ */
extern HFONT defaultFontEn;
  
/* ���ڱ���Ƿ�����Դ�ļ��޷��ҵ� */
extern BOOL res_not_found_flag;

HFONT GUI_Default_FontInit(void);
HFONT GUI_Init_Extern_Font_Stream(const char* res_name);
HFONT GUI_Init_Extern_Font_2RAM(const char* res_name,u8** buf);
HFONT GUI_Init_Extern_Font(void);
  
#ifdef	__cplusplus
}
#endif

#endif /* __GUI_FONT_PORT_H */
