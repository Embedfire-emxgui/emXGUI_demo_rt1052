#pragma once


//struct _list_item{

//  /* APP�·������� */
//  const WCHAR *pName;

//};

typedef struct
{
  WCHAR **list_objs;
  int y_num;
  u32 bg_color; /* ������ɫ */
}list_cfg_t;

extern const WNDCLASS Dropdown_Listbox;

