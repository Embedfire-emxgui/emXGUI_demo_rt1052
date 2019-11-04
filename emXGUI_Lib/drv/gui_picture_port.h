#ifndef __GUI_PICTURE_PORT_H
#define	__GUI_PICTURE_PORT_H

//#include "stm32f4xx.h"
#include "emXGUI.h"

#ifdef	__cplusplus
extern "C"{
#endif

void _ShowBMPEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY) ;
void _ShowJPEGEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY) ; 
void _ShowGIFEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY,uint32_t ctime);  
void _ShowGIF(uint8_t * imgBuffer, uint32_t BufferSize,uint16_t usPOSX, uint16_t usPOSY,uint32_t ctime)  ;
void _DrawPNG_file(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY); 
BOOL PIC_BMP_Draw_Res(HDC hdc, int x, int y, char *file_name, const RECT *lprc) ;

#ifdef	__cplusplus
}
#endif

#endif /* __GUI_PICTURE_PORT_H */
