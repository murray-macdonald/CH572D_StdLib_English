/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_usbhost.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description        : head file(ch572/ch570)
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef __CH57x_USBHOST_H__
#define __CH57x_USBHOST_H__

#ifdef __cplusplus
extern "C" {
#endif

#if DISK_LIB_ENABLE
  #if DISK_WITHOUT_USB_HUB
  /* When not using USB drive file system library or USB drive mounting under USBhub, need to disable the following definition */
    #define FOR_ROOT_UDISK_ONLY
  #endif
  /* When using USB drive file system library, need to enable the following definition, otherwise please disable it */
  #define DISK_BASE_BUF_LEN    512  /* Default disk data buffer size is 512 bytes, recommended to choose 2048 or even 4096 to support some USB drives with large sectors, if 0 then prohibit defining buffer in .H file and application specifies it in pDISK_BASE_BUF */
#endif

// Return status codes for each subroutine
#define ERR_SUCCESS            0x00  // Operation successful
#define ERR_USB_CONNECT        0x15  /* Detected USB device connection event, already connected */
#define ERR_USB_DISCON         0x16  /* Detected USB device disconnection event, already disconnected */
#define ERR_USB_BUF_OVER       0x17  /* USB transmission data error or buffer overflow due to too much data */
#define ERR_USB_DISK_ERR       0x1F  /* USB storage operation failed, during initialization may be due to unsupported USB storage, during read/write operation may be due to disk corruption or disconnection */
#define ERR_USB_TRANSFER       0x20  /* NAK/STALL and other error codes in range 0x20~0x2F */
#define ERR_USB_UNSUPPORT      0xFB  /* Unsupported USB device */
#define ERR_USB_UNKNOWN        0xFE  /* Device operation error */
#define ERR_AOA_PROTOCOL       0x41  /* Protocol version error */

/* USB device related information table, supports up to 1 device */
#define ROOT_DEV_DISCONNECT    0
#define ROOT_DEV_CONNECTED     1
#define ROOT_DEV_FAILED        2
#define ROOT_DEV_SUCCESS       3
#define DEV_TYPE_KEYBOARD      (USB_DEV_CLASS_HID | 0x20)
#define DEV_TYPE_MOUSE         (USB_DEV_CLASS_HID | 0x30)
#define DEF_AOA_DEVICE         0xF0
#define DEV_TYPE_UNKNOW        0xFF

/*
Convention: USB device address allocation rules (refer to USB_DEVICE_ADDR)
Address Value  Device Location
0x02    USB device or external HUB under the built-in Root-HUB
0x1x    USB device under port x of an external HUB under the built-in Root-HUB, x is 1~n
*/
#define HUB_MAX_PORTS          4
#define WAIT_USB_TOUT_200US    800   // Wait time for USB interrupt timeout

typedef struct
{
    uint8_t  DeviceStatus;  // Device status: 0-No device, 1-Device present but not initialized, 2-Device present but initialization/enumeration failed, 3-Device present and initialization/enumeration successful
    uint8_t  DeviceAddress; // USB address assigned to the device
    uint8_t  DeviceSpeed;   // 0 for low speed, non-zero for full speed
    uint8_t  DeviceType;    // Device type
    uint16_t DeviceVID;
    uint16_t DevicePID;
    uint8_t  GpVar[4];     // General purpose variables, used for endpoints
    uint8_t  GpHUBPortNum; // General purpose variable, number of HUB ports if device is a HUB
} _RootHubDev;

typedef struct
{
    uint8_t  DeviceStatus;  // Device status: 0-No device, 1-Device present but not initialized, 2-Device present but initialization/enumeration failed, 3-Device present and initialization/enumeration successful
    uint8_t  DeviceAddress; // USB address assigned to the device
    uint8_t  DeviceSpeed;   // 0 for low speed, non-zero for full speed
    uint8_t  DeviceType;    // Device type
    uint16_t DeviceVID;
    uint16_t DevicePID;
    uint8_t  GpVar[4]; // General purpose variables
} _DevOnHubPort;     // Assumption: no more than 1 external HUB, each external HUB has no more than HUB_MAX_PORTS ports (extras are ignored)

extern _RootHubDev   ThisUsbDev;
extern _DevOnHubPort DevOnHubPort[HUB_MAX_PORTS]; // Assumption: no more than 1 external HUB, each external HUB has no more than HUB_MAX_PORTS ports (extras are ignored)
extern uint8_t       UsbDevEndp0Size;             // Maximum packet size of endpoint 0 of the USB device
extern uint8_t       FoundNewDev;

extern uint8_t *pHOST_RX_RAM_Addr;
extern uint8_t *pHOST_TX_RAM_Addr;

extern _RootHubDev   ThisUsb2Dev;
extern _DevOnHubPort DevOnU2HubPort[HUB_MAX_PORTS]; // Assumption: no more than 1 external HUB, each external HUB has no more than HUB_MAX_PORTS ports (extras are ignored)
extern uint8_t       Usb2DevEndp0Size;              // Maximum packet size of endpoint 0 of the USB device
extern uint8_t       FoundNewU2Dev;

extern uint8_t *pU2HOST_RX_RAM_Addr;
extern uint8_t *pU2HOST_TX_RAM_Addr;

#define pSetupReq      ((PUSB_SETUP_REQ)pHOST_TX_RAM_Addr)
#define pU2SetupReq    ((PUSB_SETUP_REQ)pU2HOST_TX_RAM_Addr)
extern uint8_t Com_Buffer[];
extern uint8_t U2Com_Buffer[];

/* The following are USB host request packets */
extern const uint8_t SetupGetDevDescr[];     // Get device descriptor */
extern const uint8_t SetupGetCfgDescr[];     // Get configuration descriptor */
extern const uint8_t SetupSetUsbAddr[];      // Set USB address */
extern const uint8_t SetupSetUsbConfig[];    // Set USB configuration */
extern const uint8_t SetupSetUsbInterface[]; // Set USB interface configuration */
extern const uint8_t SetupClrEndpStall[];    // Clear endpoint STALL */

extern const uint8_t SetupGetU2DevDescr[];    // Get device descriptor */
extern const uint8_t SetupGetU2CfgDescr[];    // Get configuration descriptor */
extern const uint8_t SetupSetUsb2Addr[];      // Set USB address */
extern const uint8_t SetupSetUsb2Config[];    // Set USB configuration */
extern const uint8_t SetupSetUsb2Interface[]; // Set USB interface configuration */
extern const uint8_t SetupClrU2EndpStall[];   // Clear endpoint STALL */

/**
 * @brief   Close ROOT-HUB port, hardware has already closed it automatically, this just clears some structure status
 */
void DisableRootHubPort(void);

/**
 * @brief   Analyze ROOT-HUB status, handle device plug/unplug events on the ROOT-HUB port
 *          If device is removed, the function calls DisableRootHubPort() to close the port, for insertion events, it sets the status bit of the corresponding port
 *
 * @return  Returns ERR_SUCCESS if no events, ERR_USB_CONNECT if new connection detected, ERR_USB_DISCON if disconnection detected
 */
uint8_t AnalyzeRootHub(void);

/**
 * @brief   Set the USB device address for the current USB host operation
 *
 * @param   addr    - USB device address
 */
void SetHostUsbAddr(uint8_t addr);

/**
 * @brief   Set current USB speed
 *
 * @param   FullSpeed   - USB speed
 */
void SetUsbSpeed(uint8_t FullSpeed);

/**
 * @brief   After detecting the device, reset the bus, prepare for device enumeration, set to full speed by default
 */
void ResetRootHubPort(void);

/**
 * @brief   Enable ROOT-HUB port, set corresponding bUH_PORT_EN to 1 to open port, device disconnection may cause failure
 *
 * @return  Returns ERR_SUCCESS if new connection detected, ERR_USB_DISCON if no connection
 */
uint8_t EnableRootHubPort(void);

/**
 * @brief   Wait for USB interrupt
 *
 * @return  Returns ERR_SUCCESS if data reception or transmission successful, ERR_USB_UNKNOWN if data reception or transmission failed
 */
uint8_t WaitUSB_Interrupt(void);

/**
 * @brief   Transfer transaction, input destination endpoint address/PID token, sync flag, NAK retry total time in 20uS units (0 means no retry, 0xFFFF means infinite retry), returns 0 on success, timeout/error retry
 *          This subroutine focuses on being easy to understand, while in practical applications, to improve execution speed, this subroutine code should be optimized
 *
 * @param   endp_pid    - Token and address, high 4 bits are token_pid, low 4 bits are endpoint address
 * @param   tog         - Sync flag
 * @param   timeout     - Timeout period
 *
 * @return  ERR_USB_UNKNOWN Timeout, possibly hardware exception
 *          ERR_USB_DISCON  Device disconnected
 *          ERR_USB_CONNECT Device connected
 *          ERR_SUCCESS     Transfer completed
 */
uint8_t USBHostTransact(uint8_t endp_pid, uint8_t tog, uint32_t timeout);

/**
 * @brief   Execute control transfer, 8-byte request code in pSetupReq, DataBuf is optional send/receive buffer
 *
 * @param   DataBuf     - If data needs to be received and sent, DataBuf needs to point to a valid buffer for storing subsequent data
 * @param   RetLen      - The actual total length of successful send/receive is stored in the byte variable pointed to by RetLen
 *
 * @return  ERR_USB_BUF_OVER    Error in IN status phase
 *          ERR_SUCCESS         Data exchange successful
 */
uint8_t HostCtrlTransfer(uint8_t *DataBuf, uint8_t *RetLen);

/**
 * @brief   Copy the control transfer request packet
 *
 * @param   pReqPkt     - Control request packet address
 */
void CopySetupReqPkg(const uint8_t *pReqPkt);

/**
 * @brief   Get device descriptor, returned in pHOST_TX_RAM_Addr
 *
 * @return  ERR_USB_BUF_OVER    Descriptor length error
 *          ERR_SUCCESS         Success
 */
uint8_t CtrlGetDeviceDescr(void);

/**
 * @brief   Get configuration descriptor, returned in pHOST_TX_RAM_Addr
 *
 * @return  ERR_USB_BUF_OVER    Descriptor length error
 *          ERR_SUCCESS         Success
 */
uint8_t CtrlGetConfigDescr(void);

/**
 * @brief   Set USB device address
 *
 * @param   addr    - Device address
 *
 * @return  ERR_SUCCESS     Success
 */
uint8_t CtrlSetUsbAddress(uint8_t addr);

/**
 * @brief   Set USB device configuration
 *
 * @param   cfg     - Configuration value
 *
 * @return  ERR_SUCCESS     Success
 */
uint8_t CtrlSetUsbConfig(uint8_t cfg);

/**
 * @brief   Clear endpoint STALL
 *
 * @param   endp    - Endpoint address
 *
 * @return  ERR_SUCCESS     Success
 */
uint8_t CtrlClearEndpStall(uint8_t endp);

/**
 * @brief   Set USB device interface
 *
 * @param   cfg     - Configuration value
 *
 * @return  ERR_SUCCESS     Success
 */
uint8_t CtrlSetUsbIntercace(uint8_t cfg);

/**
 * @brief   USB host function initialization
 */
void USB_HostInit(void);
uint8_t EnumAllHubPort(void);// Enumerate all secondary USB devices under external HUBs connected to ROOT-HUB ports
void SelectHubPort(uint8_t HubPortIndex); // HubPortIndex=0 selects operation on the specified ROOT-HUB port, otherwise selects operation on the specified port of the external HUB on the specified ROOT-HUB port
uint16_t SearchTypeDevice(uint8_t type); // Search for port number of a specified device type on ROOT-HUB and external HUB ports, output port number 0xFFFF means not found.
uint8_t SETorOFFNumLock(uint8_t *buf); // NumLock LED indicator judgment

/*************************************************************/

/**
 * @brief   Initialize USB device on the specified ROOT-HUB port
 *
 * @return  Error code
 */
uint8_t InitRootDevice(void);

/**
 * @brief   Get HID device report descriptor, returned in TxBuffer
 *
 * @return  Error code
 */
uint8_t CtrlGetHIDDeviceReport(uint8_t infc);

/**
 * @brief   Get HUB descriptor, returned in Com_Buffer
 *
 * @return  Error code
 */
uint8_t CtrlGetHubDescr(void);

/**
 * @brief   Query HUB port status, returned in Com_Buffer
 *
 * @param   HubPortIndex    - Port number
 *
 * @return  Error code
 */
uint8_t HubGetPortStatus(uint8_t HubPortIndex);

/**
 * @brief   Set HUB port feature
 *
 * @param   HubPortIndex    - Port number
 * @param   FeatureSelt     - Port feature
 *
 * @return  Error code
 */
uint8_t HubSetPortFeature(uint8_t HubPortIndex, uint8_t FeatureSelt);

/**
 * @brief   Clear HUB port feature
 *
 * @param   HubPortIndex    - Port number
 * @param   FeatureSelt     - Port feature
 *
 * @return  Error code
 */
uint8_t HubClearPortFeature(uint8_t HubPortIndex, uint8_t FeatureSelt);

#ifdef __cplusplus
}
#endif

#endif // __CH57x_USBHOST_H__
