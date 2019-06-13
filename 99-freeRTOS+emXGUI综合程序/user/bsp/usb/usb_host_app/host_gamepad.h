#ifndef __HOST_GAMEPAD_H
#define __HOST_GAMEPAD_H

#include "usb_host.h"

typedef enum {
    gamepad_A  = 0x8F,//按键4
	  gamepad_B  = 0x4F,//按键3
		gamepad_AB = 0xCF,//按键3
	
    gamepad_U  = 0x00,//按键上
	  gamepad_D  = 0xFF,//按键下
    gamepad_L  = 0x00,//按键左
    gamepad_R  = 0xFF,//按键右
	
	  gamepad_select = 0x10,//选择按键
    gamepad_start = 0x20,//开始按键

} Gamepad;


extern uint8_t GAMEPAD_Decode(uint8_t *data);

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief USB host keyboard instance structure */
typedef struct _usb_host_keyboard_instance
{
    usb_host_configuration_handle configHandle; /*!< the keybaord's configuration handle */
    usb_device_handle deviceHandle;             /*!< the keybaord's device handle */
    usb_host_class_handle classHandle;          /*!< the keybaord's class handle */
    usb_host_interface_handle interfaceHandle;  /*!< the keybaord's interface handle */
    uint8_t lastPressData[6];                   /*!< keyboard last keyboard data */
    uint16_t maxPacketSize;                     /*!< interrupt in max packet size */
    uint8_t deviceState;                        /*!< device attach/detach status */
    uint8_t prevState;                          /*!< device attach/detach previous status */
    uint8_t runState;                           /*!< keyboard application run status */
    uint8_t runWaitState; /*!< keyboard application wait status, go to next run status when the wait status success */
    uint8_t *gamepadBuffer; /*!< use to receive report descriptor and data */
} usb_host_gamepad_instance_t;

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief buffer for receiving report descriptor and data */
#define HID_BUFFER_SIZE (200U)

/*! @brief host app device attach/detach status */
typedef enum _usb_host_hid_app_state
{
    kStatus_DEV_Idle = 0, /*!< there is no device attach/detach */
    kStatus_DEV_Attached, /*!< device is attached */
    kStatus_DEV_Detached, /*!< device is detached */
} usb_host_hid_app_state_t;

/*! @brief host app run status */
typedef enum _usb_host_hid_run_state
{
    kRunIdle = 0,                /*!< idle */
    kRunSetInterface,            /*!< execute set interface code */
    kRunWaitSetInterface,        /*!< wait set interface done */
    kRunSetInterfaceDone,        /*!< set interface is done, execute next step */
    kRunWaitSetIdle,             /*!< wait set idle done */
    kRunSetIdleDone,             /*!< set idle is done, execute next step */
    kRunWaitGetReportDescriptor, /*!< wait get report descriptor done */
    kRunGetReportDescriptorDone, /*!< get report descriptor is done, execute next step */
    kRunWaitSetProtocol,         /*!< wait set protocol done */
    kRunSetProtocolDone,         /*!< set protocol is done, execute next step */
    kRunWaitDataReceived,        /*!< wait interrupt in data */
    kRunDataReceived,            /*!< interrupt in data received */
    kRunPrimeDataReceive,        /*!< prime interrupt in receive */
} usb_host_hid_run_state_t;

extern usb_host_gamepad_instance_t g_HostHidGamepad1;
extern usb_host_gamepad_instance_t g_HostHidGamepad2;
/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @brief host keyboard task function.
 *
 * This function implements the host keyboard action, it is used to create task.
 *
 * @param param   the host keyboard instance pointer.
 */
extern void USB_HostHidGamepad1Task(void *param);

/*!
 * @brief host keyboard task function.
 *
 * This function implements the host keyboard action, it is used to create task.
 *
 * @param param   the host keyboard instance pointer.
 */
extern void USB_HostHidGamepad2Task(void *param);
/*!
 * @brief host keyboard callback function.
 *
 * This function should be called in the host callback function.
 *
 * @param deviceHandle         device handle.
 * @param configurationHandle  attached device's configuration descriptor information.
 * @param eventCode            callback event code, please reference to enumeration host_event_t.
 *
 * @retval kStatus_USB_Success              The host is initialized successfully.
 * @retval kStatus_USB_NotSupported         The configuration don't contain hid keyboard interface.
 */
extern usb_status_t USB_HostGamepad1Event(usb_device_handle deviceHandle,
                                             usb_host_configuration_handle configurationHandle,
                                             uint32_t eventCode);
/*!
 * @brief host keyboard callback function.
 *
 * This function should be called in the host callback function.
 *
 * @param deviceHandle         device handle.
 * @param configurationHandle  attached device's configuration descriptor information.
 * @param eventCode            callback event code, please reference to enumeration host_event_t.
 *
 * @retval kStatus_USB_Success              The host is initialized successfully.
 * @retval kStatus_USB_NotSupported         The configuration don't contain hid keyboard interface.
 */
extern usb_status_t USB_HostGamepad2Event(usb_device_handle deviceHandle,
                                             usb_host_configuration_handle configurationHandle,
                                             uint32_t eventCode);

#endif /* __USBH_HID_GAMEPAD_H */

