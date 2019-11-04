#pragma once


//struct _list_item{

//  /* APP下方的文字 */
//  const WCHAR *pName;

//};

typedef struct
{
  WCHAR **list_objs;
  int y_num;
  u32 bg_color; /* 背景颜色 */
}list_cfg_t;

extern const WNDCLASS Dropdown_Listbox;

