
#include "GUI_Drv.h"
#include "pxp_drv.h"
BOOL g_dma2d_en = TRUE;
/*===================================================================================*/
/*
 *GPU��ͼ������������,�����֧�ֲ�������ֱ�ӷ���FALSE.
 */

#if(G2D_EN)

#define	GPU_EN	1

#endif

/*===================================================================================*/

#if 0
BOOL	GUI_GPU_Init(void)
{

	return TRUE;
}
#endif

/*============================================================================*/
/*
 * ��ָ������ԭʼ���ظ��Ƶ�out������.
 */
BOOL 	GPU_CopyBits(const SURFACE *pSurf,int x,int y,int w,int h,void *out,int width_bytes)
{
	return FALSE;
}

/*============================================================================*/
/*
 * �������.
 */
BOOL	GPU_FillRect(const SURFACE *pSurf,int x,int y,int w,int h,COLORREF c)
{
#if(GPU_EN)
  if(g_dma2d_en)
	{
		if(PXP_fill_rect(pSurf,x,y,w,h,c))
		{
			return TRUE;
		}
		return FALSE;
	}
  else
    return FALSE;
#else
	return FALSE;
#endif
}

/*============================================================================*/
/*
 * �������(ARGB��ʽ).
 */
BOOL	GPU_FillRectARGB(const SURFACE *pSurf,int x,int y,int w,int h,U8 a,U8 r,U8 g,U8 b)
{
	return FALSE;
}

/*============================================================================*/
/*
 * ����λͼ.
 */
BOOL	GPU_DrawBitmap(const SURFACE *pSurf,int x,int y,int w,int h,const U8 *bits,int width_bytes,int format)
{
#if(GPU_EN)
  if(g_dma2d_en)
	{
		switch(format)
		{

		case BM_RGB565:
			return	PXP_draw_bitmap_RGB(pSurf,x,y,w,h,width_bytes,bits,kPXP_PsPixelFormatRGB565);

		case BM_XRGB1555:
			return	PXP_draw_bitmap_RGB(pSurf,x,y,w,h,width_bytes,bits,kPXP_PsPixelFormatRGB555);

		case BM_ARGB4444:
			return	PXP_draw_bitmap_ARGB(pSurf,x,y,w,h,width_bytes,bits,kPXP_AsPixelFormatARGB4444);

//		case BM_RGB888:
//			return	PXP_draw_bitmap_RGB(pSurf,x,y,w,h,width_bytes,bits,kPXP_PsPixelFormatRGB888);

		case BM_XRGB8888:
			return	PXP_draw_bitmap_RGB(pSurf,x,y,w,h,width_bytes,bits,kPXP_PsPixelFormatRGB888);

		case BM_ARGB8888:
			return	PXP_draw_bitmap_ARGB(pSurf,x,y,w,h,width_bytes,bits,kPXP_AsPixelFormatARGB8888);

		default:
			return FALSE;

		}
	}
  else
    return FALSE;
#else
	return FALSE;
#endif

}

/*============================================================================*/
/*
 * ����λͼ.
 */
BOOL 	GPU_ScaleBitmap(const SURFACE *pSurf,int x,int y,int w,int h,const BITMAP *bm)
{
#if(0)
	{
		return	PXP_ScaleBitmap(pSurf,x,y,w,h,bm);
	}
#else
	return FALSE;
#endif
}

/*============================================================================*/


