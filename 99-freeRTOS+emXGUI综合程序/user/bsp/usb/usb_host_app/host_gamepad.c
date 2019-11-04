#include "host_gamepad.h"
#include "usb_host_config.h"
#include "usb_host.h"
#include "usb_host_hid.h"
#include "host_gamepad.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
uint8_t Gamepad_Val[2];/*手柄键值*/
uint8_t Gamepad_state = 0;/*手柄状态，是否枚举成功*/

/*!
 * @brief process hid data and print gamepad action.
 *
 * @param buffer   hid data buffer.
 */
static void USB_HostGamepad1ProcessBuffer(usb_host_gamepad_instance_t *gamepadInstance);

/*!
 * @brief process hid data and print gamepad action.
 *
 * @param buffer   hid data buffer.
 */
static void USB_HostGamepad2ProcessBuffer(usb_host_gamepad_instance_t *gamepadInstance);
/*!
 * @brief host gamepad interrupt in transfer callback.
 *
 * This function is used as callback function when call USB_HostHidRecv .
 *
 * @param param    the host gamepad instance pointer.
 * @param data     data buffer pointer.
 * @param dataLength data length.
 * @status         transfer result status.
 */
static void USB_HostHidInCallback(void *param, uint8_t *data, uint32_t dataLength, usb_status_t status);

/*******************************************************************************
 * Variables
 ******************************************************************************/

USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_Gamepad1Buffer[HID_BUFFER_SIZE]; /*!< use to receive report descriptor and data */
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_Gamepad2Buffer[HID_BUFFER_SIZE]; /*!< use to receive report descriptor and data */
usb_host_gamepad_instance_t g_HostHidGamepad1;
usb_host_gamepad_instance_t g_HostHidGamepad2;

/*******************************************************************************
 * Code
 ******************************************************************************/
/**
* @brief  手柄报文解码
*         Decode Mouse data
* @param  data : Pointer to Mouse HID data buffer
* @retval None
*/
uint8_t  GAMEPAD_Decode(uint8_t *data)
{
		uint8_t LR_Value;
		uint8_t UD_Value;
		uint8_t AB_Value;
		uint8_t Fun_Value;
		uint8_t value=0;

		if(data[7] == 0x00)/*不带摇杆的手柄*/
		{	
			LR_Value = data[3];
			UD_Value = data[4];
			AB_Value = data[5];
			Fun_Value = data[6];
		}
		else if(data[7] == 0xC0)/*带摇杆的手柄*/
		{
			LR_Value = data[0];
			UD_Value = data[1];
			AB_Value = data[5];
			Fun_Value = data[6];			
		}
		/*根据USB接收到的数据解码对应按键*/
		switch(LR_Value)
		{
			case gamepad_L:
				value |=0x40;
				break;
			
			case gamepad_R:
				value |=0x80;
				break;
			
			default:
				value &=0x3F;
				break;
		}
		switch(UD_Value)
		{
			case gamepad_U:
				value |=0x10;
				break;
			
			case gamepad_D:
				value |=0x20;
				break;
			
			default:
				value &=0xCF;
				break;
		}
		switch(AB_Value)
		{
			case gamepad_A:
				value |=0x01;
				break;
			
			case gamepad_B:
				value |=0x02;
				break;
			
			case gamepad_AB:
				value |=0x03;
				break;
			
			default:
				value &=0xFC;
				break;
		}

		switch(Fun_Value)
		{
			case gamepad_select:
				value |=0x04;
				break;
			
			case gamepad_start:
				value |=0x08;
				break;
			
			default:
				value &=0xF3;
				break;
		}	
	return value;
}

static void USB_HostGamepad1ProcessBuffer(usb_host_gamepad_instance_t *gamepadInstance)
{
    uint8_t *data;
		static uint8_t keytemp;

		data = gamepadInstance->gamepadBuffer;
		Gamepad_Val[0] = GAMEPAD_Decode(data);
	
		if(keytemp != Gamepad_Val[0])
			usb_echo("Gamepad1 = 0x%x\r\n",Gamepad_Val[0]);

		keytemp = Gamepad_Val[0];
}
static void USB_HostGamepad2ProcessBuffer(usb_host_gamepad_instance_t *gamepadInstance)
{
    uint8_t *data;
		static uint8_t keytemp;

		data = gamepadInstance->gamepadBuffer;
	
		Gamepad_Val[1] = GAMEPAD_Decode(data);
	
		if(keytemp != Gamepad_Val[1])
		usb_echo("Gamepad2 = 0x%x\r\n",Gamepad_Val[1]);

		keytemp = Gamepad_Val[1];

}
static void USB_HostHidInCallback(void *param, uint8_t *data, uint32_t dataLength, usb_status_t status)
{
    usb_host_gamepad_instance_t *gamepadInstance = (usb_host_gamepad_instance_t *)param;

    if (gamepadInstance->runWaitState == kRunWaitDataReceived)
    {
        if (gamepadInstance->deviceState == kStatus_DEV_Attached)
        {
            if (status == kStatus_USB_Success)
            {
                gamepadInstance->runState = kRunDataReceived; /* go to process data */
            }
            else
            {
                gamepadInstance->runState = kRunPrimeDataReceive; /* go to prime next receiving */
            }
        }
    }
}

static void USB_HostHidControlCallback(void *param, uint8_t *data, uint32_t dataLength, usb_status_t status)
{
    usb_host_gamepad_instance_t *gamepadInstance = (usb_host_gamepad_instance_t *)param;

    if (gamepadInstance->runWaitState == kRunWaitSetInterface) /* set interface done */
    {
        gamepadInstance->runState = kRunSetInterfaceDone;
    }
    else if (gamepadInstance->runWaitState == kRunWaitSetIdle) /* hid set idle done */
    {
        gamepadInstance->runState = kRunSetIdleDone;
    }
    else if (gamepadInstance->runWaitState == kRunWaitGetReportDescriptor) /* hid get report descriptor done */
    {
        gamepadInstance->runState = kRunGetReportDescriptorDone;
    }
    else if (gamepadInstance->runWaitState == kRunWaitSetProtocol) /* hid set protocol done */
    {
        gamepadInstance->runState = kRunSetProtocolDone;
    }
    else
    {
    }
}
void USB_HostHidGamepad1Task(void *param)
{
    usb_host_gamepad_instance_t *gamepadInstance = (usb_host_gamepad_instance_t *)param;
    usb_host_hid_descriptor_t *hidDescriptor;
    uint32_t gamepadReportLength = 0;
    uint8_t *descriptor;
    uint32_t endPosition;
    uint8_t index;

    /* device state changes, process once for each state */
    if (gamepadInstance->deviceState != gamepadInstance->prevState)
    {
        gamepadInstance->prevState = gamepadInstance->deviceState;
        switch (gamepadInstance->deviceState)
        {
            case kStatus_DEV_Idle:
                break;

            case kStatus_DEV_Attached: /* deivce is attached and numeration is done */
                gamepadInstance->runState = kRunSetInterface;
                /* hid class initialization */
                if (USB_HostHidInit(gamepadInstance->deviceHandle, &gamepadInstance->classHandle) !=
                    kStatus_USB_Success)
                {
                    usb_echo("host hid class initialize fail\r\n");
                }
                else
                {
                    usb_echo("Gamepad1 attached\r\n");
										Gamepad_state |= 0X0F;
                }
                for (index = 0; index < 6; ++index)
                {
                    gamepadInstance->lastPressData[index] = 0x00;
                    gamepadInstance->gamepadBuffer[index] = 0x00;
                }
                gamepadInstance->gamepadBuffer[6] = gamepadInstance->gamepadBuffer[7] = 0x00;
                break;

            case kStatus_DEV_Detached: /* device is detached */
                gamepadInstance->runState = kRunIdle;
                gamepadInstance->deviceState = kStatus_DEV_Idle;
                USB_HostHidDeinit(gamepadInstance->deviceHandle,
                                  gamepadInstance->classHandle); /* hid class de-initialization */
                gamepadInstance->classHandle = NULL;
                for (index = 0; index < 6; ++index)
                {
                    gamepadInstance->lastPressData[index] = 0x00;
                    gamepadInstance->gamepadBuffer[index] = 0x00;
                }
                gamepadInstance->gamepadBuffer[6] = gamepadInstance->gamepadBuffer[7] = 0x00;
                usb_echo("Gamepad1 detached\r\n");
								
                break;

            default:
                break;
        }
    }

    switch (gamepadInstance->runState)
    {
        case kRunIdle:
            break;

        case kRunSetInterface: /* 1. set hid interface */
            gamepadInstance->runWaitState = kRunWaitSetInterface;
            gamepadInstance->runState = kRunIdle;
            if (USB_HostHidSetInterface(gamepadInstance->classHandle, gamepadInstance->interfaceHandle, 0,
                                        USB_HostHidControlCallback, gamepadInstance) != kStatus_USB_Success)
            {
                usb_echo("set interface error\r\n");
            }
            break;

        case kRunSetInterfaceDone: /* 2. hid set idle */
            gamepadInstance->maxPacketSize =
                USB_HostHidGetPacketsize(gamepadInstance->classHandle, USB_ENDPOINT_INTERRUPT, USB_IN);

            /* first: set idle */
            gamepadInstance->runWaitState = kRunWaitSetIdle;
            gamepadInstance->runState = kRunIdle;
            if (USB_HostHidSetIdle(gamepadInstance->classHandle, 0, 0, USB_HostHidControlCallback, gamepadInstance) !=
                kStatus_USB_Success)
            {
                usb_echo("error in USB_HostHidSetIdle\r\n");
            }
            break;

        case kRunSetIdleDone: /* 3. hid get report descriptor */
            /* get report descriptor */
            hidDescriptor = NULL;
            descriptor = (uint8_t *)((usb_host_interface_t *)gamepadInstance->interfaceHandle)->interfaceExtension;
            endPosition = (uint32_t)descriptor +
                          ((usb_host_interface_t *)gamepadInstance->interfaceHandle)->interfaceExtensionLength;

            while ((uint32_t)descriptor < endPosition)
            {
                if (*(descriptor + 1) == USB_DESCRIPTOR_TYPE_HID) /* descriptor type */
                {
                    hidDescriptor = (usb_host_hid_descriptor_t *)descriptor;
                    break;
                }
                else
                {
                    descriptor = (uint8_t *)((uint32_t)descriptor + (*descriptor)); /* next descriptor */
                }
            }

            if (hidDescriptor != NULL)
            {
                usb_host_hid_class_descriptor_t *hidClassDescriptor;
                hidClassDescriptor = (usb_host_hid_class_descriptor_t *)&(hidDescriptor->bHidDescriptorType);
                for (index = 0; index < hidDescriptor->bNumDescriptors; ++index)
                {
                    hidClassDescriptor += index;
                    if (hidClassDescriptor->bHidDescriptorType == USB_DESCRIPTOR_TYPE_HID_REPORT)
                    {
                        gamepadReportLength =
                            (uint16_t)USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(hidClassDescriptor->wDescriptorLength);
                        break;
                    }
                }
            }
            if (gamepadReportLength > HID_BUFFER_SIZE)
            {
                usb_echo("hid buffer is too small\r\n");
                gamepadInstance->runState = kRunIdle;
                return;
            }

            if (gamepadReportLength > 0) /* when report descriptor length is zero, go to next step */
            {
                gamepadInstance->runWaitState = kRunWaitGetReportDescriptor;
                gamepadInstance->runState = kRunIdle;
                /* second: get report descriptor */
                USB_HostHidGetReportDescriptor(gamepadInstance->classHandle, gamepadInstance->gamepadBuffer,
                                               gamepadReportLength, USB_HostHidControlCallback, gamepadInstance);
                break;
            }

        case kRunGetReportDescriptorDone: /* 4. hid set protocol */
            gamepadInstance->runWaitState = kRunWaitSetProtocol;
            gamepadInstance->runState = kRunIdle;
            /* third: set protocol */
            if (USB_HostHidSetProtocol(gamepadInstance->classHandle, USB_HOST_HID_REQUEST_PROTOCOL_REPORT,
                                       USB_HostHidControlCallback, gamepadInstance) != kStatus_USB_Success)
            {
                usb_echo("error in USB_HostHidSetProtocol\r\n");
            }
            break;

        case kRunSetProtocolDone: /* 5. start to receive data */
            gamepadInstance->runWaitState = kRunWaitDataReceived;
            gamepadInstance->runState = kRunIdle;
            if (USB_HostHidRecv(gamepadInstance->classHandle, gamepadInstance->gamepadBuffer,
                                gamepadInstance->maxPacketSize, USB_HostHidInCallback,
                                gamepadInstance) != kStatus_USB_Success)
            {
                usb_echo("error in USB_HostHidRecv\r\n");
            }
            break;

        case kRunDataReceived: /* process received data and receive next data */
            USB_HostGamepad1ProcessBuffer(gamepadInstance);

            gamepadInstance->runWaitState = kRunWaitDataReceived;
            gamepadInstance->runState = kRunIdle;
            if (USB_HostHidRecv(gamepadInstance->classHandle, gamepadInstance->gamepadBuffer,
                                gamepadInstance->maxPacketSize, USB_HostHidInCallback,
                                gamepadInstance) != kStatus_USB_Success)
            {
                usb_echo("Error in USB_HostHidRecv\r\n");
            }
            break;

        case kRunPrimeDataReceive: /* receive data */
            gamepadInstance->runWaitState = kRunWaitDataReceived;
            gamepadInstance->runState = kRunIdle;
            if (USB_HostHidRecv(gamepadInstance->classHandle, gamepadInstance->gamepadBuffer,
                                gamepadInstance->maxPacketSize, USB_HostHidInCallback,
                                gamepadInstance) != kStatus_USB_Success)
            {
                usb_echo("error in USB_HostHidRecv\r\n");
            }
            break;

        default:
            break;
    }
}
void USB_HostHidGamepad2Task(void *param)
{
    usb_host_gamepad_instance_t *gamepadInstance = (usb_host_gamepad_instance_t *)param;
    usb_host_hid_descriptor_t *hidDescriptor;
    uint32_t gamepadReportLength = 0;
    uint8_t *descriptor;
    uint32_t endPosition;
    uint8_t index;

    /* device state changes, process once for each state */
    if (gamepadInstance->deviceState != gamepadInstance->prevState)
    {
        gamepadInstance->prevState = gamepadInstance->deviceState;
        switch (gamepadInstance->deviceState)
        {
            case kStatus_DEV_Idle:
                break;

            case kStatus_DEV_Attached: /* deivce is attached and numeration is done */
                gamepadInstance->runState = kRunSetInterface;
                /* hid class initialization */
                if (USB_HostHidInit(gamepadInstance->deviceHandle, &gamepadInstance->classHandle) !=
                    kStatus_USB_Success)
                {
                    usb_echo("host hid class initialize fail\r\n");
                }
                else
                {
                    usb_echo("Gamepad2 attached\r\n");
										Gamepad_state |= 0XF0;
                }
                for (index = 0; index < 6; ++index)
                {
                    gamepadInstance->lastPressData[index] = 0x00;
                    gamepadInstance->gamepadBuffer[index] = 0x00;
                }
                gamepadInstance->gamepadBuffer[6] = gamepadInstance->gamepadBuffer[7] = 0x00;
                break;

            case kStatus_DEV_Detached: /* device is detached */
                gamepadInstance->runState = kRunIdle;
                gamepadInstance->deviceState = kStatus_DEV_Idle;
                USB_HostHidDeinit(gamepadInstance->deviceHandle,
                                  gamepadInstance->classHandle); /* hid class de-initialization */
                gamepadInstance->classHandle = NULL;
                for (index = 0; index < 6; ++index)
                {
                    gamepadInstance->lastPressData[index] = 0x00;
                    gamepadInstance->gamepadBuffer[index] = 0x00;
                }
                gamepadInstance->gamepadBuffer[6] = gamepadInstance->gamepadBuffer[7] = 0x00;
                usb_echo("Gamepad2 detached\r\n");
                break;

            default:
                break;
        }
    }

    switch (gamepadInstance->runState)
    {
        case kRunIdle:
            break;

        case kRunSetInterface: /* 1. set hid interface */
            gamepadInstance->runWaitState = kRunWaitSetInterface;
            gamepadInstance->runState = kRunIdle;
            if (USB_HostHidSetInterface(gamepadInstance->classHandle, gamepadInstance->interfaceHandle, 0,
                                        USB_HostHidControlCallback, gamepadInstance) != kStatus_USB_Success)
            {
                usb_echo("set interface error\r\n");
            }
            break;

        case kRunSetInterfaceDone: /* 2. hid set idle */
            gamepadInstance->maxPacketSize =
                USB_HostHidGetPacketsize(gamepadInstance->classHandle, USB_ENDPOINT_INTERRUPT, USB_IN);

            /* first: set idle */
            gamepadInstance->runWaitState = kRunWaitSetIdle;
            gamepadInstance->runState = kRunIdle;
            if (USB_HostHidSetIdle(gamepadInstance->classHandle, 0, 0, USB_HostHidControlCallback, gamepadInstance) !=
                kStatus_USB_Success)
            {
                usb_echo("error in USB_HostHidSetIdle\r\n");
            }
            break;

        case kRunSetIdleDone: /* 3. hid get report descriptor */
            /* get report descriptor */
            hidDescriptor = NULL;
            descriptor = (uint8_t *)((usb_host_interface_t *)gamepadInstance->interfaceHandle)->interfaceExtension;
            endPosition = (uint32_t)descriptor +
                          ((usb_host_interface_t *)gamepadInstance->interfaceHandle)->interfaceExtensionLength;

            while ((uint32_t)descriptor < endPosition)
            {
                if (*(descriptor + 1) == USB_DESCRIPTOR_TYPE_HID) /* descriptor type */
                {
                    hidDescriptor = (usb_host_hid_descriptor_t *)descriptor;
                    break;
                }
                else
                {
                    descriptor = (uint8_t *)((uint32_t)descriptor + (*descriptor)); /* next descriptor */
                }
            }

            if (hidDescriptor != NULL)
            {
                usb_host_hid_class_descriptor_t *hidClassDescriptor;
                hidClassDescriptor = (usb_host_hid_class_descriptor_t *)&(hidDescriptor->bHidDescriptorType);
                for (index = 0; index < hidDescriptor->bNumDescriptors; ++index)
                {
                    hidClassDescriptor += index;
                    if (hidClassDescriptor->bHidDescriptorType == USB_DESCRIPTOR_TYPE_HID_REPORT)
                    {
                        gamepadReportLength =
                            (uint16_t)USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(hidClassDescriptor->wDescriptorLength);
                        break;
                    }
                }
            }
            if (gamepadReportLength > HID_BUFFER_SIZE)
            {
                usb_echo("hid buffer is too small\r\n");
                gamepadInstance->runState = kRunIdle;
                return;
            }

            if (gamepadReportLength > 0) /* when report descriptor length is zero, go to next step */
            {
                gamepadInstance->runWaitState = kRunWaitGetReportDescriptor;
                gamepadInstance->runState = kRunIdle;
                /* second: get report descriptor */
                USB_HostHidGetReportDescriptor(gamepadInstance->classHandle, gamepadInstance->gamepadBuffer,
                                               gamepadReportLength, USB_HostHidControlCallback, gamepadInstance);
                break;
            }

        case kRunGetReportDescriptorDone: /* 4. hid set protocol */
            gamepadInstance->runWaitState = kRunWaitSetProtocol;
            gamepadInstance->runState = kRunIdle;
            /* third: set protocol */
            if (USB_HostHidSetProtocol(gamepadInstance->classHandle, USB_HOST_HID_REQUEST_PROTOCOL_REPORT,
                                       USB_HostHidControlCallback, gamepadInstance) != kStatus_USB_Success)
            {
                usb_echo("error in USB_HostHidSetProtocol\r\n");
            }
            break;

        case kRunSetProtocolDone: /* 5. start to receive data */
            gamepadInstance->runWaitState = kRunWaitDataReceived;
            gamepadInstance->runState = kRunIdle;
            if (USB_HostHidRecv(gamepadInstance->classHandle, gamepadInstance->gamepadBuffer,
                                gamepadInstance->maxPacketSize, USB_HostHidInCallback,
                                gamepadInstance) != kStatus_USB_Success)
            {
                usb_echo("error in USB_HostHidRecv\r\n");
            }
            break;

        case kRunDataReceived: /* process received data and receive next data */
            USB_HostGamepad2ProcessBuffer(gamepadInstance);

            gamepadInstance->runWaitState = kRunWaitDataReceived;
            gamepadInstance->runState = kRunIdle;
            if (USB_HostHidRecv(gamepadInstance->classHandle, gamepadInstance->gamepadBuffer,
                                gamepadInstance->maxPacketSize, USB_HostHidInCallback,
                                gamepadInstance) != kStatus_USB_Success)
            {
                usb_echo("Error in USB_HostHidRecv\r\n");
            }
            break;

        case kRunPrimeDataReceive: /* receive data */
            gamepadInstance->runWaitState = kRunWaitDataReceived;
            gamepadInstance->runState = kRunIdle;
            if (USB_HostHidRecv(gamepadInstance->classHandle, gamepadInstance->gamepadBuffer,
                                gamepadInstance->maxPacketSize, USB_HostHidInCallback,
                                gamepadInstance) != kStatus_USB_Success)
            {
                usb_echo("error in USB_HostHidRecv\r\n");
            }
            break;

        default:
            break;
    }
}

usb_status_t USB_HostGamepad1Event(usb_device_handle deviceHandle,
                                      usb_host_configuration_handle configurationHandle,
                                      uint32_t eventCode)
{
    usb_host_configuration_t *configuration;
    usb_host_interface_t *interface;
    uint32_t infoValue;
    usb_status_t status = kStatus_USB_Success;
    uint8_t interfaceIndex;
    uint8_t id;
		/*判断手柄是否为新增手柄，若不是则退出*/
		USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceAddress, &infoValue);
		if(infoValue==3)
		return status;
		
    switch (eventCode)
    {
        case kUSB_HostEventAttach:
            /* judge whether is configurationHandle supported */
            configuration = (usb_host_configuration_t *)configurationHandle;
            for (interfaceIndex = 0; interfaceIndex < configuration->interfaceCount; ++interfaceIndex)
            {
                interface = &configuration->interfaceList[interfaceIndex];
                id = interface->interfaceDesc->bInterfaceClass;
                if (id != USB_HOST_HID_CLASS_CODE)
                {
                    continue;
                }
                id = interface->interfaceDesc->bInterfaceSubClass;
                if ((id != USB_HOST_HID_SUBCLASS_CODE_NONE) && (id != USB_HOST_HID_SUBCLASS_CODE_BOOT))
                {
                    continue;
                }
                id = interface->interfaceDesc->bInterfaceProtocol;
                if (id != USB_HOST_HID_PROTOCOL_NONE)
                {
                    continue;
                }
                else
                {
                    if (g_HostHidGamepad1.deviceState == kStatus_DEV_Idle)
                    {
                        /* the interface is supported by the application */
                        g_HostHidGamepad1.gamepadBuffer = s_Gamepad1Buffer;
                        g_HostHidGamepad1.deviceHandle = deviceHandle;
                        g_HostHidGamepad1.interfaceHandle = interface;
                        g_HostHidGamepad1.configHandle = configurationHandle;
                        return kStatus_USB_Success;
                    }
                    else
                    {
                        continue;
                    }
                }
            }
            status = kStatus_USB_NotSupported;
            break;

        case kUSB_HostEventNotSupported:
            break;

        case kUSB_HostEventEnumerationDone:
            if (g_HostHidGamepad1.configHandle == configurationHandle)
            {
                if ((g_HostHidGamepad1.deviceHandle != NULL) && (g_HostHidGamepad1.interfaceHandle != NULL))
                {
                    /* the device enumeration is done */
                    if (g_HostHidGamepad1.deviceState == kStatus_DEV_Idle)
                    {
                        g_HostHidGamepad1.deviceState = kStatus_DEV_Attached;

                        USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDevicePID, &infoValue);
                        usb_echo("hid Gamepad1 attached:pid=0x%x", infoValue);
                        USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceVID, &infoValue);
                        usb_echo("vid=0x%x ", infoValue);
                        USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceAddress, &infoValue);
                        usb_echo("address=%d\r\n", infoValue);
                    }
                    else
                    {
                        usb_echo("not idle host gamepad instance\r\n");
                        status = kStatus_USB_Error;
                    }
                }
            }
            break;

        case kUSB_HostEventDetach:
            if (g_HostHidGamepad1.configHandle == configurationHandle)
            {
                /* the device is detached */
                g_HostHidGamepad1.configHandle = NULL;
                if (g_HostHidGamepad1.deviceState != kStatus_DEV_Idle)
                {
                    g_HostHidGamepad1.deviceState = kStatus_DEV_Detached;
                }
            }
            break;

        default:
            break;
    }
    return status;
}
usb_status_t USB_HostGamepad2Event(usb_device_handle deviceHandle,
                                      usb_host_configuration_handle configurationHandle,
                                      uint32_t eventCode)
{
    usb_host_configuration_t *configuration;
    usb_host_interface_t *interface;
    uint32_t infoValue;
    usb_status_t status = kStatus_USB_Success;
    uint8_t interfaceIndex;
    uint8_t id;
	
		/*判断手柄是否为新增手柄，若不是则退出*/
		USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceAddress, &infoValue);
		if(infoValue==2)
			return status;
		
    switch (eventCode)
    {
        case kUSB_HostEventAttach:
            /* judge whether is configurationHandle supported */
            configuration = (usb_host_configuration_t *)configurationHandle;
            for (interfaceIndex = 0; interfaceIndex < configuration->interfaceCount; ++interfaceIndex)
            {
                interface = &configuration->interfaceList[interfaceIndex];
                id = interface->interfaceDesc->bInterfaceClass;
                if (id != USB_HOST_HID_CLASS_CODE)
                {
                    continue;
                }
                id = interface->interfaceDesc->bInterfaceSubClass;
                if ((id != USB_HOST_HID_SUBCLASS_CODE_NONE) && (id != USB_HOST_HID_SUBCLASS_CODE_BOOT))
                {
                    continue;
                }
                id = interface->interfaceDesc->bInterfaceProtocol;
                if (id != USB_HOST_HID_PROTOCOL_NONE)
                {
                    continue;
                }
                else
                {
                    if (g_HostHidGamepad2.deviceState == kStatus_DEV_Idle)
                    {
                        /* the interface is supported by the application */
                        g_HostHidGamepad2.gamepadBuffer = s_Gamepad2Buffer;
                        g_HostHidGamepad2.deviceHandle = deviceHandle;
                        g_HostHidGamepad2.interfaceHandle = interface;
                        g_HostHidGamepad2.configHandle = configurationHandle;
                        return kStatus_USB_Success;
                    }
                    else
                    {
                        continue;
                    }
                }
            }
            status = kStatus_USB_NotSupported;
            break;

        case kUSB_HostEventNotSupported:
            break;

        case kUSB_HostEventEnumerationDone:
            if (g_HostHidGamepad2.configHandle == configurationHandle)
            {
                if ((g_HostHidGamepad2.deviceHandle != NULL) && (g_HostHidGamepad2.interfaceHandle != NULL))
                {
                    /* the device enumeration is done */
                    if (g_HostHidGamepad2.deviceState == kStatus_DEV_Idle)
                    {
                        g_HostHidGamepad2.deviceState = kStatus_DEV_Attached;

                        USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDevicePID, &infoValue);
                        usb_echo("hid Gamepad1 attached:pid=0x%x", infoValue);
                        USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceVID, &infoValue);
                        usb_echo("vid=0x%x ", infoValue);
                        USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceAddress, &infoValue);
                        usb_echo("address=%d\r\n", infoValue);
                    }
                    else
                    {
                        usb_echo("not idle host gamepad instance\r\n");
                        status = kStatus_USB_Error;
                    }
                }
            }
            break;

        case kUSB_HostEventDetach:
            if (g_HostHidGamepad2.configHandle == configurationHandle)
            {
                /* the device is detached */
                g_HostHidGamepad2.configHandle = NULL;
                if (g_HostHidGamepad2.deviceState != kStatus_DEV_Idle)
                {
                    g_HostHidGamepad2.deviceState = kStatus_DEV_Detached;
                }
            }
            break;

        default:
            break;
    }
    return status;
}
