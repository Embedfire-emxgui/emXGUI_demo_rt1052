/**
  *********************************************************************
  * @file    gui_resource_port.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   �����ⲿ��Դ�Ľӿ�
  *          �������������ⲿFLASH�е���Դ
  *********************************************************************
  * @attention
  * ����    :www.emXGUI.com
  *
  **********************************************************************
  */ 
  
#include <string.h>
#include "board.h" 
#include	"emXGUI.h"
#include	"emXGUI_Arch.h"
#include "x_libc.h" 
#include "gui_resource_port.h"
#include "gui_mem_port.h"
#include "./flash/bsp_qspi_flash.h"


/*=========================================================================================*/
/*������Դ�豸�Ļ����ź���*/
static GUI_MUTEX *mutex_lock=NULL;

/**
  * @brief  ��ʼ����Դ�豸���ⲿFLASH��
  * @param  ��
  * @retval �Ƿ��ʼ������
  */
BOOL RES_DevInit(void)
{  
	mutex_lock=GUI_MutexCreate();
//  if(QSPI_FLASH_Init() == 0)
//  {
  //RES_DevTest();
    //RES_DevTest();
    return TRUE;
//  }
//  else
//    return FALSE;

	
}

/**
  * @brief  ��ȡ�豸ID
  * @param  ��
  * @retval �豸ID
  */
U32 RES_DevGetID(void)
{
	U32 id;
	GUI_MutexLock(mutex_lock,5000);
#if defined(STM32F429_439xx)
	id =SPI_FLASH_ReadID();
#elif defined(STM32H743xx)
 // id = QSPI_FLASH_ReadID();
#endif  
  
	GUI_MutexUnlock(mutex_lock);
	return id;
}

/**
  * @brief  ���豸д������
  * @param  buf Ҫд�������
  * @param  addr д���Ŀ���ַ
  * @param  size д�����������size��Ӧ����BLOCK��С��
  * @retval �Ƿ�д������
  */
BOOL RES_DevWrite(u8 *buf,u32 addr,u32 size)
{
	GUI_MutexLock(mutex_lock,5000);
	
#if defined(STM32F429_439xx)
	SPI_FLASH_BufferWrite(buf,addr,size);
#elif defined(STM32H743xx)
  //BSP_QSPI_Write(buf,addr,size);
#endif    
	GUI_MutexUnlock(mutex_lock);
	return TRUE;
}

/**
  * @brief  ���豸�ж�ȡ����
  * @param  buf �洢��ȡ��������
  * @param  addr ��ȡ��Ŀ���ַ
  * @param  size ��ȡ��������
  * @retval �Ƿ��ȡ����
  */
BOOL RES_DevRead(u8 *buf,u32 addr,u32 size)
{
  
	GUI_MutexLock(mutex_lock,5000);
  __IO uint8_t *qspi_addr = (__IO uint8_t *)(APPLICATION_ADDRESS+addr);
//#if defined(STM32F429_439xx)
//	SPI_FLASH_BufferRead(buf,addr,size);
//#elif defined(STM32H743xx)
//  BSP_QSPI_FastRead(buf,addr,size);
//#endif 
  //memcpy(buf, (u8*)addr,size);
#if 1  
  for(int i = 0; i < size; i ++)
  {
    buf[i] = *qspi_addr;
    qspi_addr++;
  }
#else 
  memcpy(buf, (u8*)qspi_addr,size);
#endif  
  
	GUI_MutexUnlock(mutex_lock);
	return TRUE;
}

/**
  * @brief  ��������
  * @param  addr Ҫ������������ַ
  * @retval �������ֽ���
  */
int RES_DevEraseSector(u32 addr)
{
	GUI_MutexLock(mutex_lock,5000);
#if defined(STM32F429_439xx)  
	SPI_FLASH_SectorErase(addr&0xFFFFF000);
#elif defined(STM32H743xx)
  //BSP_QSPI_Erase_Block(addr&0xFFFFF000);
#endif     
	GUI_MutexUnlock(mutex_lock);
	return 0;
}


/**
  * @brief  �ȴ��豸��д����������
  * @param  timeout ���ȴ�ʱ�䣨ms��
  * @retval �Ƿ�ʱ
  */
BOOL RES_WaitForWriteEnd(u32 timeout)
{
	GUI_MutexLock(mutex_lock,5000);
//  SPI_FLASH_WaitForWriteEnd();
//	SPI_FLASH_BufferRead(buf,addr,size);
	GUI_MutexUnlock(mutex_lock);
	return TRUE;
}
/*=========================================================================================*/
#if 1
/**
  * @brief  ��Դ�豸����
  * @param  ��
  * @retval ��
  */
void RES_DevTest(void)
{
	u8 *buf;
	volatile u32 id;

	buf =GUI_VMEM_Alloc(4096);  
//  GUI_Printf("0x%x",id);

#if 1  
	//while(1)
	{	
    int i = 0;

//		RES_DevEraseSector(i);

//		memset(buf,i>>12,4096);
//		RES_DevWrite(buf,i,4096);

		memset(buf,0,4096);
		RES_DevRead((u8 *)buf, i,4096);

		i += 4096;
		GUI_msleep(100);

	}
#endif  
	GUI_VMEM_Free(buf);
}
#endif



/**
  * @brief  ��FLASH�е�Ŀ¼������Ӧ����Դλ��
  * @param  res_base Ŀ¼��FLASH�еĻ���ַ
  * @param  res_name[in] Ҫ���ҵ���Դ����
  * @retval -1��ʾ�Ҳ���������ֵ��ʾ��Դ��FLASH�еĻ���ַ
  */
s32 RES_GetOffset(const char *res_name)
{
	int i,len;
	CatalogTypeDef dir;

	len =x_strlen(res_name);
	for(i=0;i<GUI_CATALOG_SIZE;i+=sizeof(CatalogTypeDef))
	{
		RES_DevRead((u8*)&dir,GUI_RES_BASE+i,sizeof(CatalogTypeDef));

		if(x_strncasecmp(dir.name,res_name,len)==0)
		{
      /* dir.offset����Ի���ַ��ƫ�ƣ��˴����ؾ��Ե�ַ */
			return dir.offset + GUI_RES_BASE;
		}
	}

	return -1;
}

/**
  * @brief  ��FLASH�е�Ŀ¼������Ӧ��Դ����Ϣ
  * @param  res_base Ŀ¼��FLASH�еĻ���ַ
  * @param  res_name[in] Ҫ���ҵ���Դ����
  * @param  dir[out] Ҫ���ҵ���Դ����
  * @note   �˴�dir.offset�ᱻ��ֵΪ��Դ�ľ��Ե�ַ����
  * @retval -1��ʾ�Ҳ���������ֵ��ʾ��Դ��FLASH�еĻ���ַ
  */
s32 RES_GetInfo_AbsAddr(const char *res_name, CatalogTypeDef *dir)
{
	int i,len;
  
	len =x_strlen(res_name);
  /* �������ֱ���Ŀ¼ */
	for(i=0;i<GUI_CATALOG_SIZE;i+=sizeof(CatalogTypeDef))
	{
		RES_DevRead((u8*)dir,GUI_RES_BASE+i,sizeof(CatalogTypeDef));

		if(x_strncasecmp(dir->name,res_name,len)==0)
		{
      /* dir.offset����Ի���ַ��ƫ�ƣ��˴����ؾ��Ե�ַ */
      dir->offset += GUI_RES_BASE;
			return dir->offset ;
		}
	}
	return -1;
}


/**
  * @brief  ����Դ�豸��������
  * @param  file_name[in]: �ļ���
  * @param  buf[out]�����غ�õ��Ļ�����
  * @param  size[out]�����ݵĴ�С
  * @note   buf�Ǹ������ݵĴ�С��̬��VMEM������ģ�
  *         ʹ�����buf����Ҫ����h�ļ��е�Release_Content�����ͷ�buf�Ŀռ�
  * @retval FALSE:ʧ��; TRUE:�ɹ�
*/
BOOL RES_Load_Content(char *file_name, char** buf, u32* size) 
{	 
    int content_offset;
    CatalogTypeDef dir;
    BOOL result = TRUE;
  
    content_offset = RES_GetInfo_AbsAddr(file_name, &dir);
    if(content_offset > 0)
    {    
      /* �ļ����ݿռ� */
      *buf = (char *)GUI_VMEM_Alloc(dir.size);
      if(*buf != NULL)
      {
        /* ��������*/
        RES_DevRead((u8 *)*buf,content_offset,dir.size); 
          
        *size = dir.size;
      }
      else
        result = FALSE;
    }
    else 
      result = FALSE;
   
    return result;
}

#if 1
/**
  * @brief  ���ļ�ϵͳ��������
  * @param  file_name[in]: �ļ�·��
  * @param  buf[out]�����غ�õ��Ļ�����
  * @param  size[out]�����ݵĴ�С
  * @note   buf�Ǹ������ݵĴ�С��̬��VMEM������ģ�
  *         ʹ�����buf����Ҫ����h�ļ��е�Release_Content�����ͷ�buf�Ŀռ�
  * @retval FALSE:ʧ��; TRUE:�ɹ�
*/
BOOL FS_Load_Content(char *file_name, char** buf, u32* size) 
{	 	
    /* file objects */
    FIL     *file;												
    FRESULT fresult;  
    BOOL result = TRUE;
    UINT br;
  
    /* �ļ�����ռ� */
    file =(FIL*)GUI_VMEM_Alloc(sizeof(FIL));
      
    /* ���ļ� */		
    fresult = f_open(file, file_name, FA_OPEN_EXISTING | FA_READ );
    if (fresult != FR_OK)
    {      
      GUI_ERROR("Open file failed!");
      GUI_VMEM_Free(file);
      return FALSE;
    }    

    *size = f_size(file);    
    /* �ļ����ݿռ� */
    *buf = (char *)GUI_VMEM_Alloc(*size);
    if(*buf != NULL)
    {    
      /* ��������ͼƬ�ļ� */
      fresult = f_read(file, *buf, *size, &br);
      /* �ر��ļ� */
      f_close(file);
    }
    else
      result = FALSE;    
    
    /* �ͷſռ� */
    GUI_VMEM_Free(file);    
    
    return result;
}
#endif




/********************************END OF FILE****************************/
