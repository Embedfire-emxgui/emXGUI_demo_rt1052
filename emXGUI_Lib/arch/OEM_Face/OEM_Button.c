
#include "emXGUI.h"
#include "emXGUI_Arch.h"

/*=========================================================================================*/
////本文件实现"按钮"控件的外观绘制.


#define	PUSHBTN_TEXTCOLOR_0		XRGB8888(0,0,0)
#define	PUSHBTN_TEXTCOLOR_1		XRGB8888(0,20,30)

#define	PUSHBTN_BACKCOLOR_0		XRGB8888(230,230,235)
#define	PUSHBTN_BACKCOLOR_1		XRGB8888(150,175,210)

#define	PUSHBTN_BODERCOLOR_0	XRGB8888(20,30,50)
#define	PUSHBTN_BODERCOLOR_1	XRGB8888(10,80,120)


static void __draw_backgnd(HDC hdc,RECT *prc,CTLCOLOR *cr,u32 state)
{
	RECT rc;
	COLOR_RGB32 c1,c2,c;

	rc = *prc;

	if(state&BST_PUSHED)
	{
		c1 =RGB32_AVG(cr->BorderColor,cr->BackColor);
		c1 =RGB32_Offset(c1,-15,-15,-15);
		c2 =cr->BackColor;
		SetPenColor(hdc,MapRGB888(hdc,c1));
		DrawRect(hdc,&rc);

		InflateRect(&rc,-1,-1);
		SetBrushColor(hdc,MapRGB888(hdc,c2));
		FillRect(hdc,&rc);

	}
	else
	{
		c =cr->BackColor;

		c1 =RGB32_AVG(cr->BorderColor,cr->BackColor);
		c1 =RGB32_Offset(c1,-5,-5,-5);
		SetPenColor(hdc,MapRGB888(hdc,c1));
		DrawRect(hdc,&rc);

		InflateRect(&rc,-1,-1);
		c1 =RGB32_Offset(cr->BackColor,+55,+55,+55);
		SetPenColor(hdc,MapRGB888(hdc,c1));
		DrawRect(hdc,&rc);

		InflateRect(&rc,-1,-1);
		rc.h = (rc.h*7)>>4;
		c1 =RGB32_Offset(c,+15,+15,+15);
		c2 =RGB32_Offset(c,-3,-3,-3);
		GradientFillRect(hdc,&rc,MapRGB888(hdc,c1),MapRGB888(hdc,c2),TRUE);

		OffsetRect(&rc,0,rc.h);
		rc.h =prc->y+prc->h-rc.y-2;

		c1 =RGB32_Offset(c,-7,-7,-7);
		c2 =RGB32_Offset(c,-12,-12,-12);
		SetBrushColor(hdc,MapRGB888(hdc,c1));
		FillRect(hdc,&rc);

	}

}

static void _SetColor(HWND hwnd,CTLCOLOR *cr,u32 style,u32 state)
{ //Button控件的颜色控制(注意CTLCOLOR各成员是用标准RGB32格式,即RGB888或ARGB8888).

	if(state&BST_PUSHED)
	{//是按下状态.
		cr->TextColor =PUSHBTN_TEXTCOLOR_1;     //文字颜色.
		cr->BackColor =PUSHBTN_BACKCOLOR_1;     //背景颜色.
		cr->BorderColor =PUSHBTN_BODERCOLOR_1;  //背景颜色.
	}
	else
	{//是弹起状态.
		cr->TextColor =PUSHBTN_TEXTCOLOR_0;
		cr->BackColor =PUSHBTN_BACKCOLOR_0;
		cr->BorderColor =PUSHBTN_BODERCOLOR_0;
	}

	if(style&WS_DISABLED)
	{//如果是“禁止”风格的,让它呈暗灰色.

		//先把各颜色RGB分量同步降低(使原色变暗).
		cr->TextColor =RGB32_Offset(cr->TextColor,-10,-10,-10);
		cr->BackColor =RGB32_Offset(cr->BackColor,-10,-10,-10);
		cr->BorderColor =RGB32_Offset(cr->BorderColor,-10,-10,-10);
		cr->ForeColor =RGB32_Offset(cr->ForeColor,-10,-10,-10);

		//再把各颜色与灰色做平均运算(使原色变灰).
		cr->TextColor =RGB32_AVG(cr->TextColor,RGB888(120,120,128));
		cr->BackColor =RGB32_AVG(cr->BackColor,RGB888(120,120,128));
		cr->BorderColor =RGB32_AVG(cr->BorderColor,RGB888(120,120,128));
		cr->ForeColor =RGB32_AVG(cr->ForeColor,RGB888(120,120,128));

	}
}

static void _PaintDefault(DRAWITEM_HDR *di,CTLCOLOR *cr,const WCHAR *pText)
{ //绘默认风格的按钮(用户可按自己的喜好实现...).

	HDC hdc =di->hDC;
	RECT rc=di->rc;
	int state =di->State;

	//画按键背景(按状态值).
	__draw_backgnd(hdc,&rc,cr,state);

	//绘制按钮上的文字.
	if(state&BST_PUSHED) //判断按钮状态.
	{//按下状态.
		OffsetRect(&rc,+1,+1);
		SetTextColor(hdc,MapXRGB8888(hdc,cr->TextColor));
		DrawText(hdc,pText,-1,&rc,DT_CENTER|DT_VCENTER);
	}
	else
	{//弹起状态.

		SetTextColor(hdc,MapXRGB8888(hdc,cr->TextColor));
		DrawText(hdc,pText,-1,&rc,DT_CENTER|DT_VCENTER);
	}

}

static void _Paint_FLAT(DRAWITEM_HDR *di,CTLCOLOR *cr,const WCHAR *pText)
{ //绘制平面风格的按钮.

	HDC hdc =di->hDC;
	RECT rc=di->rc;
	u32 state =di->State;

	SetTextColor(hdc,MapXRGB8888(hdc,cr->TextColor));
	SetPenColor(hdc,MapXRGB8888(hdc,cr->BorderColor));
	SetBrushColor(hdc,MapXRGB8888(hdc,cr->BackColor));

	if(state&BST_PUSHED)
	{//按下状态.
		OffsetRect(&rc,+1,+1); //按下时,使按钮上的文字往右下方向偏移一点点,看上去像下沉的样式.

		DrawText(hdc,pText,-1,&rc,DT_CENTER|DT_VCENTER|DT_BKGND|DT_BORDER);
	}
	else
	{//弹起状态.

		SetTextColor(hdc,MapXRGB8888(hdc,cr->TextColor));
		DrawText(hdc,pText,-1,&rc,DT_CENTER|DT_VCENTER|DT_BKGND|DT_BORDER);
	}

}

static void _Paint_ROUND(DRAWITEM_HDR *di,CTLCOLOR *cr,const WCHAR *pText)
{ //绘制平面风格的按钮.

	HDC hdc =di->hDC;
	RECT rc=di->rc;
	u32 state =di->State;
  COLOR_RGB32 c;
	SetTextColor(hdc,MapXRGB8888(hdc,cr->TextColor));
	SetPenColor(hdc,MapXRGB8888(hdc,cr->BorderColor));
	SetBrushColor(hdc,MapXRGB8888(hdc,cr->BackColor));
	__draw_backgnd(hdc,&rc,cr,state);
	FillRoundRect(hdc,&rc,10);
	DrawRoundRect(hdc,&rc,10);
	c =RGB32_AVG(cr->BorderColor,cr->BackColor);
	SetPenColor(hdc,MapRGB888(hdc,c));	
  
	if(state&BST_PUSHED)
	{
		//HLine(hdc,rc.x+1,rc.y+1,rc.x+rc.w-2);
		OffsetRect(&rc,+1,+1);
	}
	else
	{
		COLORREF c1,c2;

		c1 =MapRGB888(hdc,RGB32_Offset(cr->BackColor,33,33,33));
		c2 =MapRGB888(hdc,RGB32_Offset(cr->BackColor,-33,-33,-33));

		SetPenColor(hdc,c1);
		HLine(hdc,rc.x+3,rc.y+1,rc.x+rc.w-3);

		SetPenColor(hdc,c2);
		HLine(hdc,rc.x+3,rc.y+rc.h-2,rc.x+rc.w-3);

		//HLine(hdc,rc.x+3,rc.y+rc.h-2,rc.x+rc.w-3);
	}

	DrawText(hdc,pText,-1,&rc,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

}

static void _Paint(DRAWITEM_HDR *di,CTLCOLOR *cr,const WCHAR *pText)
{ //绘制按钮.

//	if(di->Style&BS_FLAT)
//	{ //平面风格的.
//		_Paint_FLAT(di,cr,pText);
//	}
//	else if(di->Style&BS_3D)
//	{ //3D风格的.

//	}
//	else if(di->Style&BS_ROUND)
//	{ //圆角风格的.
//    _Paint_ROUND(di,cr,pText);
//	}
//	else
//	{ //其它默认风格的.
//		_PaintDefault(di,cr,pText);
//	}
  switch(di->Style & BS_FACE_MASK)
  {
    case BS_FLAT:
    {
      _Paint_FLAT(di,cr,pText);
      break;
    }

    case BS_ROUND:
    {
      _Paint_ROUND(di,cr,pText);
      break;
    }
    case BS_3D:
    default:
      _PaintDefault(di,cr,pText);
    
  }
}

static win_oem_t oem=
{
	_SetColor,
	_Paint,
};

/*
static win_oem_t oem_flat=
{
	_SetColor,
	_Paint_FLAT,
};
*/
win_oem_t* GetButtonOEM(void)
{
	return &oem;
	//return &oem_flat;
}



