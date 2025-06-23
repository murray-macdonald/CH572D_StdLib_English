/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_usbhost.c
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description        : source file(ch572/ch570)
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH57x_common.h"
#if DISK_LIB_ENABLE
  #include "CHRV3UFI.H"
#endif

/* Set HID upload rate */
__attribute__((aligned(4))) const uint8_t SetupSetHIDIdle[] = {0x21, HID_SET_IDLE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
/* Get HID device report descriptor */
__attribute__((aligned(4))) const uint8_t SetupGetHIDDevReport[] = {0x81, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_REPORT,
                                                                    0x00, 0x00, 0x41, 0x00};
/* Get HUB descriptor */
__attribute__((aligned(4))) const uint8_t SetupGetHubDescr[] = {HUB_GET_HUB_DESCRIPTOR, HUB_GET_DESCRIPTOR, 0x00,
                                                                USB_DESCR_TYP_HUB, 0x00, 0x00, sizeof(USB_HUB_DESCR), 0x00};

__attribute__((aligned(4))) uint8_t Com_Buffer[128]; // Define user temporary buffer, used for processing descriptors during enumeration, also can be used as a general temporary buffer after enumeration

/*********************************************************************
 * @fn      AnalyzeHidIntEndp
 *
 * @brief   Analyze the HID interrupt endpoint address from the descriptor, if HubPortIndex is 0 save to ROOTHUB, if non-zero save to structure under HUB
 *
 * @param   buf     - Buffer address of data to be analyzed; HubPortIndex: 0 indicates root HUB, non-0 indicates port number under external HUB
 *
 * @return  Number of endpoints
 */
uint8_t AnalyzeHidIntEndp(uint8_t *buf, uint8_t HubPortIndex)
{
    uint8_t i, s, l;
    s = 0;

    if(HubPortIndex)
    {
        memset(DevOnHubPort[HubPortIndex - 1].GpVar, 0, sizeof(DevOnHubPort[HubPortIndex - 1].GpVar)); //Clear array
    }
    else
    {
        memset(ThisUsbDev.GpVar, 0, sizeof(ThisUsbDev.GpVar)); //Clear array
    }

    for(i = 0; i < ((PUSB_CFG_DESCR)buf)->wTotalLength; i += l) // Search for interrupt endpoint descriptors, skip configuration and interface descriptors
    {
        if(((PUSB_ENDP_DESCR)(buf + i))->bDescriptorType == USB_DESCR_TYP_ENDP                         // Is endpoint descriptor
           && (((PUSB_ENDP_DESCR)(buf + i))->bmAttributes & USB_ENDP_TYPE_MASK) == USB_ENDP_TYPE_INTER // Is interrupt endpoint
           && (((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_DIR_MASK))                    // Is IN endpoint
        {                                                                                              // Save interrupt endpoint address, bit 7 used for sync flag, clear 0
            if(HubPortIndex)
            {
                DevOnHubPort[HubPortIndex - 1].GpVar[s] = ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK;
            }
            else
            {
                ThisUsbDev.GpVar[s] = ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK; // Interrupt endpoint address, can also save wMaxPacketSize and bInterval as needed
            }
            PRINT("%02x ", (uint16_t)ThisUsbDev.GpVar[s]);
            s++;
            if(s >= 4)
            {
                break; //Only analyze up to 4 endpoints
            }
        }
        l = ((PUSB_ENDP_DESCR)(buf + i))->bLength; // Current descriptor length, skip it
        if(l > 16)
        {
            break;
        }
    }
    PRINT("\n");
    return (s);
}

/*********************************************************************
 * @fn      AnalyzeBulkEndp
 *
 * @brief   Analyze bulk endpoints, GpVar[0] and GpVar[1] store upload endpoints. GpVar[2] and GpVar[3] store download endpoints
 *
 * @param   buf     - Buffer address of data to be analyzed; HubPortIndex: 0 indicates root HUB, non-0 indicates port number under external HUB
 *
 * @return  0
 */
uint8_t AnalyzeBulkEndp(uint8_t *buf, uint8_t HubPortIndex)
{
    uint8_t i, s1, s2, l;
    s1 = 0;
    s2 = 2;

    if(HubPortIndex)
    {
        memset(DevOnHubPort[HubPortIndex - 1].GpVar, 0, sizeof(DevOnHubPort[HubPortIndex - 1].GpVar)); //Clear array
    }
    else
    {
        memset(ThisUsbDev.GpVar, 0, sizeof(ThisUsbDev.GpVar)); //Clear array
    }

    for(i = 0; i < ((PUSB_CFG_DESCR)buf)->wTotalLength; i += l) // Search for interrupt endpoint descriptors, skip configuration and interface descriptors
    {
        if((((PUSB_ENDP_DESCR)(buf + i))->bDescriptorType == USB_DESCR_TYP_ENDP)                         // Is endpoint descriptor
           && ((((PUSB_ENDP_DESCR)(buf + i))->bmAttributes & USB_ENDP_TYPE_MASK) == USB_ENDP_TYPE_BULK)) // Is bulk endpoint

        {
            if(HubPortIndex)
            {
                if(((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_DIR_MASK)
                {
                    DevOnHubPort[HubPortIndex - 1].GpVar[s1++] = ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK;
                }
                else
                {
                    DevOnHubPort[HubPortIndex - 1].GpVar[s2++] = ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK;
                }
            }
            else
            {
                if(((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_DIR_MASK)
                {
                    ThisUsbDev.GpVar[s1++] = ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK;
                }
                else
                {
                    ThisUsbDev.GpVar[s2++] = ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK;
                }
            }

            if(s1 == 2)
            {
                s1 = 1;
            }
            if(s2 == 4)
            {
                s2 = 3;
            }
        }
        l = ((PUSB_ENDP_DESCR)(buf + i))->bLength; // Current descriptor length, skip it
        if(l > 16)
        {
            break;
        }
    }
    return (0);
}

/*********************************************************************
 * @fn      InitRootDevice
 *
 * @brief   Initialize USB device on specified ROOT-HUB port
 *
 * @param   none
 *
 * @return  Error code
 */
uint8_t InitRootDevice(void)
{
    uint8_t i, s;
    uint8_t cfg, dv_cls, if_cls;

    PRINT("Reset host port\n");
    ResetRootHubPort(); // After detecting the device, reset the USB bus of the corresponding port
    for(i = 0, s = 0; i < 100; i++)
    { // Wait for the USB device to reconnect after reset, 100mS timeout
        mDelaymS(1);
        if(EnableRootHubPort() == ERR_SUCCESS)
        { // Enable port
            i = 0;
            s++;
            if(s > 100)
            {
                break; // Already stable connection for 100mS
            }
        }
    }
    if(i)
    { // Device not connected after reset
        DisableRootHubPort();
        PRINT("Disable host port because of disconnect\n");
        return (ERR_USB_DISCON);
    }
    SetUsbSpeed(ThisUsbDev.DeviceSpeed); // Set current USB speed

    PRINT("GetDevDescr: ");
    s = CtrlGetDeviceDescr(); // Get device descriptor
    if(s == ERR_SUCCESS)
    {
        for(i = 0; i < ((PUSB_SETUP_REQ)SetupGetDevDescr)->wLength; i++)
        {
            PRINT("x%02X ", (uint16_t)(Com_Buffer[i]));
        }
        PRINT("\n");

        ThisUsbDev.DeviceVID = ((PUSB_DEV_DESCR)Com_Buffer)->idVendor; //Save VID PID information
        ThisUsbDev.DevicePID = ((PUSB_DEV_DESCR)Com_Buffer)->idProduct;
        dv_cls = ((PUSB_DEV_DESCR)Com_Buffer)->bDeviceClass;

        s = CtrlSetUsbAddress(((PUSB_SETUP_REQ)SetupSetUsbAddr)->wValue);
        if(s == ERR_SUCCESS)
        {
            ThisUsbDev.DeviceAddress = ((PUSB_SETUP_REQ)SetupSetUsbAddr)->wValue; // Save USB address

            PRINT("GetCfgDescr: ");
            s = CtrlGetConfigDescr();
            if(s == ERR_SUCCESS)
            {
                for(i = 0; i < ((PUSB_CFG_DESCR)Com_Buffer)->wTotalLength; i++)
                {
                    PRINT("x%02X ", (uint16_t)(Com_Buffer[i]));
                }
                PRINT("\n");
                /* Analyze configuration descriptor, get endpoint data/addresses/sizes, update variables like endp_addr and endp_size */
                cfg = ((PUSB_CFG_DESCR)Com_Buffer)->bConfigurationValue;
                if_cls = ((PUSB_CFG_DESCR_LONG)Com_Buffer)->itf_descr.bInterfaceClass; // Interface class code

                if((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_STORAGE))
                { // This is a USB storage class device, basically confirmed as a USB drive
#ifdef FOR_ROOT_UDISK_ONLY
                    CHRV3DiskStatus = DISK_USB_ADDR;
                    return (ERR_SUCCESS);
                }
                else
                {
                    return (ERR_USB_UNSUPPORT);
                }
#else
                    s = CtrlSetUsbConfig(cfg); // Set USB device configuration
                    if(s == ERR_SUCCESS)
                    {
                        ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
                        ThisUsbDev.DeviceType = USB_DEV_CLASS_STORAGE;
                        PRINT("USB-Disk Ready\n");
                        SetUsbSpeed(1); // Default to full speed
                        return (ERR_SUCCESS);
                    }
                }
                else if((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_PRINTER) && ((PUSB_CFG_DESCR_LONG)Com_Buffer)->itf_descr.bInterfaceSubClass == 0x01)
                {                              // This is a printer class device
                    s = CtrlSetUsbConfig(cfg); // Set USB device configuration
                    if(s == ERR_SUCCESS)
                    {
                        //	Need to save endpoint information for the main program to perform USB transfers
                        ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
                        ThisUsbDev.DeviceType = USB_DEV_CLASS_PRINTER;
                        PRINT("USB-Print Ready\n");
                        SetUsbSpeed(1); // Default to full speed
                        return (ERR_SUCCESS);
                    }
                }
                else if((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_HID) && ((PUSB_CFG_DESCR_LONG)Com_Buffer)->itf_descr.bInterfaceSubClass <= 0x01)
                { // This is a HID class device, keyboard/mouse etc.
                    //  Analyze HID interrupt endpoint address from the descriptor
                    s = AnalyzeHidIntEndp(Com_Buffer, 0); // Analyze HID interrupt endpoint address from the descriptor
                    PRINT("AnalyzeHidIntEndp %02x\n", (uint16_t)s);
                    //  Save interrupt endpoint address, bit 7 used for sync flag, clear 0
                    if_cls = ((PUSB_CFG_DESCR_LONG)Com_Buffer)->itf_descr.bInterfaceProtocol;
                    s = CtrlSetUsbConfig(cfg); // Set USB device configuration
                    if(s == ERR_SUCCESS)
                    {
                        s = CtrlGetHIDDeviceReport(dv_cls); //Get report descriptor
                        if(s == ERR_SUCCESS)
                        {
                            for(i = 0; i < 64; i++)
                            {
                                PRINT("x%02X ", (uint16_t)(Com_Buffer[i]));
                            }
                            PRINT("\n");
                        }
                        //	Need to save endpoint information for the main program to perform USB transfers
                        ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
                        if(if_cls == 1)
                        {
                            ThisUsbDev.DeviceType = DEV_TYPE_KEYBOARD;
                            //	Further initialization, such as setting keyboard indicator LEDs etc.
                            PRINT("USB-Keyboard Ready\n");
                            SetUsbSpeed(1); // Default to full speed
                            return (ERR_SUCCESS);
                        }
                        else if(if_cls == 2)
                        {
                            ThisUsbDev.DeviceType = DEV_TYPE_MOUSE;
                            //	To query mouse status later, descriptor should be analyzed to get interrupt port address, length and other information
                            PRINT("USB-Mouse Ready\n");
                            SetUsbSpeed(1); // Default to full speed
                            return (ERR_SUCCESS);
                        }
                        s = ERR_USB_UNSUPPORT;
                    }
                }
                else if(dv_cls == USB_DEV_CLASS_HUB)
                { // This is a HUB class device, hub etc.
                    s = CtrlGetHubDescr();
                    if(s == ERR_SUCCESS)
                    {
                        PRINT("Max Port:%02X ", (((PXUSB_HUB_DESCR)Com_Buffer)->bNbrPorts));
                        ThisUsbDev.GpHUBPortNum = ((PXUSB_HUB_DESCR)Com_Buffer)->bNbrPorts; // Save HUB port count
                        if(ThisUsbDev.GpHUBPortNum > HUB_MAX_PORTS)
                        {
                            ThisUsbDev.GpHUBPortNum = HUB_MAX_PORTS; // Because when defining DevOnHubPort structure, it's assumed each HUB has no more than HUB_MAX_PORTS ports
                        }
                        s = CtrlSetUsbConfig(cfg); // Set USB device configuration
                        if(s == ERR_SUCCESS)
                        {
                            ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
                            ThisUsbDev.DeviceType = USB_DEV_CLASS_HUB;
                            // Need to save endpoint information for the main program to perform USB transfers; originally interrupt endpoints can be used for HUB event notifications, but this program uses status query control transfers instead
                            // Power on all HUB ports, query port status, initialize HUB ports with connected devices, and initialize devices
                            for(i = 1; i <= ThisUsbDev.GpHUBPortNum; i++) // Power on all HUB ports
                            {
                                DevOnHubPort[i - 1].DeviceStatus = ROOT_DEV_DISCONNECT; // Clear device status on external HUB port
                                s = HubSetPortFeature(i, HUB_PORT_POWER);
                                if(s != ERR_SUCCESS)
                                {
                                    PRINT("Ext-HUB Port_%1d# power on error\n", (uint16_t)i); // Port power-on failed
                                }
                            }
                            PRINT("USB-HUB Ready\n");
                            SetUsbSpeed(1); // Default to full speed
                            return (ERR_SUCCESS);
                        }
                    }
                }
                else
                {                              // Can be further analyzed
                    s = CtrlSetUsbConfig(cfg); // Set USB device configuration
                    if(s == ERR_SUCCESS)
                    {
                        //	Need to save endpoint information for the main program to perform USB transfers
                        ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
                        ThisUsbDev.DeviceType = DEV_TYPE_UNKNOW;
                        SetUsbSpeed(1);       // Default to full speed
                        return (ERR_SUCCESS); /* Unknown device initialization successful */
                    }
                }
#endif
            }
        }
    }

    PRINT("InitRootDev Err = %02X\n", (uint16_t)s);
#ifdef FOR_ROOT_UDISK_ONLY
    CHRV3DiskStatus = DISK_CONNECT;
#else
    ThisUsbDev.DeviceStatus = ROOT_DEV_FAILED;
#endif
    SetUsbSpeed(1); // Default to full speed
    return (s);
}

/*********************************************************************
 * @fn      InitDevOnHub
 *
 * @brief   Initialize secondary USB devices after enumerating external HUB
 *
 * @param   HubPortIndex    - Specify external HUB
 *
 * @return  Error code
 */
uint8_t InitDevOnHub(uint8_t HubPortIndex)
{
    uint8_t i, s, cfg, dv_cls, if_cls;
    uint8_t ifc;
    PRINT("Init dev @ExtHub-port_%1d ", (uint16_t)HubPortIndex);
    if(HubPortIndex == 0)
    {
        return (ERR_USB_UNKNOWN);
    }
    SelectHubPort(HubPortIndex); // Select the specified port of the external HUB on the specified ROOT-HUB port, and select speed
    PRINT("GetDevDescr: ");
    s = CtrlGetDeviceDescr(); // Get device descriptor
    if(s != ERR_SUCCESS)
    {
        return (s);
    }
    DevOnHubPort[HubPortIndex - 1].DeviceVID = ((uint16_t)((PUSB_DEV_DESCR)Com_Buffer)->idVendor); //Save VID PID information
    DevOnHubPort[HubPortIndex - 1].DevicePID = ((uint16_t)((PUSB_DEV_DESCR)Com_Buffer)->idProduct);

    dv_cls = ((PUSB_DEV_DESCR)Com_Buffer)->bDeviceClass; // Device class code
    cfg = (1 << 4) + HubPortIndex;                       // Calculate a USB address, avoid address overlap
    s = CtrlSetUsbAddress(cfg);                          // Set USB device address
    if(s != ERR_SUCCESS)
    {
        return (s);
    }
    DevOnHubPort[HubPortIndex - 1].DeviceAddress = cfg; // Save allocated USB address
    PRINT("GetCfgDescr: ");
    s = CtrlGetConfigDescr(); // Get configuration descriptor
    if(s != ERR_SUCCESS)
    {
        return (s);
    }
    cfg = ((PUSB_CFG_DESCR)Com_Buffer)->bConfigurationValue;
    for(i = 0; i < ((PUSB_CFG_DESCR)Com_Buffer)->wTotalLength; i++)
    {
        PRINT("x%02X ", (uint16_t)(Com_Buffer[i]));
    }
    PRINT("\n");
    /* Analyze configuration descriptor, get endpoint data/addresses/sizes, update variables like endp_addr and endp_size */
    if_cls = ((PXUSB_CFG_DESCR_LONG)Com_Buffer)->itf_descr.bInterfaceClass; // Interface class code
    if(dv_cls == 0x00 && if_cls == USB_DEV_CLASS_STORAGE)                   // This is a USB storage class device, basically confirmed as a USB drive
    {
        AnalyzeBulkEndp(Com_Buffer, HubPortIndex);
        for(i = 0; i != 4; i++)
        {
            PRINT("%02x ", (uint16_t)DevOnHubPort[HubPortIndex - 1].GpVar[i]);
        }
        PRINT("\n");
        s = CtrlSetUsbConfig(cfg); // Set USB device configuration
        if(s == ERR_SUCCESS)
        {
            DevOnHubPort[HubPortIndex - 1].DeviceStatus = ROOT_DEV_SUCCESS;
            DevOnHubPort[HubPortIndex - 1].DeviceType = USB_DEV_CLASS_STORAGE;
            PRINT("USB-Disk Ready\n");
            SetUsbSpeed(1); // Default to full speed
            return (ERR_SUCCESS);
        }
    }
    else if((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_HID) && (((PXUSB_CFG_DESCR_LONG)Com_Buffer)->itf_descr.bInterfaceSubClass <= 0x01)) // This is a HID class device, keyboard/mouse etc.
    {
        ifc = ((PXUSB_CFG_DESCR_LONG)Com_Buffer)->cfg_descr.bNumInterfaces;
        s = AnalyzeHidIntEndp(Com_Buffer, HubPortIndex); // Analyze HID interrupt endpoint address from the descriptor
        PRINT("AnalyzeHidIntEndp %02x\n", (uint16_t)s);
        if_cls = ((PXUSB_CFG_DESCR_LONG)Com_Buffer)->itf_descr.bInterfaceProtocol;
        s = CtrlSetUsbConfig(cfg); // Set USB device configuration
        if(s == ERR_SUCCESS)
        {
            for(dv_cls = 0; dv_cls < ifc; dv_cls++)
            {
                s = CtrlGetHIDDeviceReport(dv_cls); //Get report descriptor
                if(s == ERR_SUCCESS)
                {
                    for(i = 0; i < 64; i++)
                    {
                        PRINT("x%02X ", (uint16_t)(Com_Buffer[i]));
                    }
                    PRINT("\n");
                }
            }
            // Need to save endpoint information for the main program to perform USB transfers
            DevOnHubPort[HubPortIndex - 1].DeviceStatus = ROOT_DEV_SUCCESS;
            if(if_cls == 1)
            {
                DevOnHubPort[HubPortIndex - 1].DeviceType = DEV_TYPE_KEYBOARD;
                //Further initialization, such as setting keyboard indicator LEDs etc.
                if(ifc > 1)
                {
                    PRINT("USB_DEV_CLASS_HID Ready\n");
                    DevOnHubPort[HubPortIndex - 1].DeviceType = USB_DEV_CLASS_HID; //Composite HID device
                }
                PRINT("USB-Keyboard Ready\n");
                SetUsbSpeed(1); // Default to full speed

                return (ERR_SUCCESS);
            }
            else if(if_cls == 2)
            {
                DevOnHubPort[HubPortIndex - 1].DeviceType = DEV_TYPE_MOUSE;
                //For future mouse status queries, should analyze descriptor to get interrupt port address, length and other information
                if(ifc > 1)
                {
                    PRINT("USB_DEV_CLASS_HID Ready\n");
                    DevOnHubPort[HubPortIndex - 1].DeviceType = USB_DEV_CLASS_HID; //Composite HID device
                }
                PRINT("USB-Mouse Ready\n");
                SetUsbSpeed(1); // Default to full speed

                return (ERR_SUCCESS);
            }
            s = ERR_USB_UNSUPPORT;
        }
    }
    else if(dv_cls == USB_DEV_CLASS_HUB) // This is a HUB class device, hub, etc.
    {
        DevOnHubPort[HubPortIndex - 1].DeviceType = USB_DEV_CLASS_HUB;
        PRINT("This program don't support Level 2 HUB\n"); // To support multi-level HUB cascading, please refer to this program for extension
        s = HubClearPortFeature(i, HUB_PORT_ENABLE);       // Disable HUB port
        if(s != ERR_SUCCESS)
        {
            return (s);
        }
        s = ERR_USB_UNSUPPORT;
    }
    else //Other devices
    {
        AnalyzeBulkEndp(Com_Buffer, HubPortIndex); //Analyze bulk endpoints
        for(i = 0; i != 4; i++)
        {
            PRINT("%02x ", (uint16_t)DevOnHubPort[HubPortIndex - 1].GpVar[i]);
        }
        PRINT("\n");
        s = CtrlSetUsbConfig(cfg); // Set USB device configuration
        if(s == ERR_SUCCESS)
        {
            // Need to save endpoint information for the main program to perform USB transfers
            DevOnHubPort[HubPortIndex - 1].DeviceStatus = ROOT_DEV_SUCCESS;
            DevOnHubPort[HubPortIndex - 1].DeviceType = dv_cls ? dv_cls : if_cls;
            SetUsbSpeed(1);       // Default to full speed
            return (ERR_SUCCESS); //Unknown device initialization successful
        }
    }
    PRINT("InitDevOnHub Err = %02X\n", (uint16_t)s);
    DevOnHubPort[HubPortIndex - 1].DeviceStatus = ROOT_DEV_FAILED;
    SetUsbSpeed(1); // Default to full speed
    return (s);
}

/*********************************************************************
 * @fn      EnumHubPort
 *
 * @brief   Enumerate each port of the external HUB on the specified ROOT-HUB port, check for connection or removal events on each port, and initialize secondary USB devices
 *
 * @param   RootHubIndex    - ROOT_HUB0 and ROOT_HUB1
 *
 * @return  Error code
 */
uint8_t EnumHubPort()
{
    uint8_t i, s;

    for(i = 1; i <= ThisUsbDev.GpHUBPortNum; i++) // Query if there are changes on the hub ports
    {
        SelectHubPort(0);        // Select the specified ROOT-HUB port, set current USB speed and the USB address of the device being operated
        s = HubGetPortStatus(i); // Get port status
        if(s != ERR_SUCCESS)
        {
            return (s); // The HUB might have disconnected
        }
        if(((Com_Buffer[0] & (1 << (HUB_PORT_CONNECTION & 0x07))) && (Com_Buffer[2] & (1 << (HUB_C_PORT_CONNECTION & 0x07)))) || (Com_Buffer[2] == 0x10))
        {                                                          // Device connection discovered
            DevOnHubPort[i - 1].DeviceStatus = ROOT_DEV_CONNECTED; // Device connected
            DevOnHubPort[i - 1].DeviceAddress = 0x00;
            s = HubGetPortStatus(i); // Get port status
            if(s != ERR_SUCCESS)
            {
                return (s); // The HUB might have disconnected
            }
            DevOnHubPort[i - 1].DeviceSpeed = Com_Buffer[1] & (1 << (HUB_PORT_LOW_SPEED & 0x07)) ? 0 : 1; // Low speed or full speed
            if(DevOnHubPort[i - 1].DeviceSpeed)
            {
                PRINT("Found full speed device on port %1d\n", (uint16_t)i);
            }
            else
            {
                PRINT("Found low speed device on port %1d\n", (uint16_t)i);
            }
            mDelaymS(200);                            // Wait for device power to stabilize
            s = HubSetPortFeature(i, HUB_PORT_RESET); // Reset the port with connected device
            if(s != ERR_SUCCESS)
            {
                return (s); // The HUB might have disconnected
            }
            PRINT("Reset port and then wait in\n");
            do // Query the reset port until reset completes, and display the status after completion
            {
                mDelaymS(1);
                s = HubGetPortStatus(i);
                if(s != ERR_SUCCESS)
                {
                    return (s); // The HUB might have disconnected
                }
            } while(Com_Buffer[0] & (1 << (HUB_PORT_RESET & 0x07))); // Wait while the port is being reset
            mDelaymS(100);
            s = HubClearPortFeature(i, HUB_C_PORT_RESET);      // Clear reset completion flag
                                                               //             s = HubSetPortFeature( i, HUB_PORT_ENABLE );                              // Enable HUB port
            s = HubClearPortFeature(i, HUB_C_PORT_CONNECTION); // Clear connection or removal change flag
            if(s != ERR_SUCCESS)
            {
                return (s);
            }
            s = HubGetPortStatus(i); // Read status again, recheck if the device is still present
            if(s != ERR_SUCCESS)
            {
                return (s);
            }
            if((Com_Buffer[0] & (1 << (HUB_PORT_CONNECTION & 0x07))) == 0)
            {
                DevOnHubPort[i - 1].DeviceStatus = ROOT_DEV_DISCONNECT; // Device is no longer present
            }
            s = InitDevOnHub(i); // Initialize secondary USB device
            if(s != ERR_SUCCESS)
            {
                return (s);
            }
            SetUsbSpeed(1); // Default to full speed
        }
        else if(Com_Buffer[2] & (1 << (HUB_C_PORT_ENABLE & 0x07))) // Device connection error
        {
            HubClearPortFeature(i, HUB_C_PORT_ENABLE); // Clear connection error flag
            PRINT("Device on port error\n");
            s = HubSetPortFeature(i, HUB_PORT_RESET); // Reset the port with connected device
            if(s != ERR_SUCCESS)
                return (s); // The HUB might have disconnected
            do              // Query the reset port until reset completes, and display the status after completion
            {
                mDelaymS(1);
                s = HubGetPortStatus(i);
                if(s != ERR_SUCCESS)
                    return (s);                                      // The HUB might have disconnected
            } while(Com_Buffer[0] & (1 << (HUB_PORT_RESET & 0x07))); // Wait while the port is being reset
        }
        else if((Com_Buffer[0] & (1 << (HUB_PORT_CONNECTION & 0x07))) == 0) // Device has been disconnected
        {
            if(DevOnHubPort[i - 1].DeviceStatus >= ROOT_DEV_CONNECTED)
            {
                PRINT("Device on port %1d removed\n", (uint16_t)i);
            }
            DevOnHubPort[i - 1].DeviceStatus = ROOT_DEV_DISCONNECT; // Device is disconnected
            if(Com_Buffer[2] & (1 << (HUB_C_PORT_CONNECTION & 0x07)))
            {
                HubClearPortFeature(i, HUB_C_PORT_CONNECTION); // Clear removal change flag
            }
        }
    }
    return (ERR_SUCCESS); // Return operation successful
}

/*********************************************************************
 * @fn      EnumAllHubPort
 *
 * @brief   Enumerate all secondary USB devices after external HUB on all ROOT-HUB ports
 *
 * @return  Error code
 */
uint8_t EnumAllHubPort(void)
{
    uint8_t s;

    if((ThisUsbDev.DeviceStatus >= ROOT_DEV_SUCCESS) && (ThisUsbDev.DeviceType == USB_DEV_CLASS_HUB)) // HUB enumeration successful
    {
        SelectHubPort(0);    // Select the specified ROOT-HUB port, set current USB speed and the USB address of the device being operated
        s = EnumHubPort();   // Enumerate each port on the external HUB on the specified ROOT-HUB port, check for connection or removal events on each port
        if(s != ERR_SUCCESS) // The HUB might have disconnected
        {
            PRINT("EnumAllHubPort err = %02X\n", (uint16_t)s);
        }
        SetUsbSpeed(1); // Default to full speed
    }
    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      SearchTypeDevice
 *
 * @brief   Search for the port number of a specified device type on ROOT-HUB and external HUB ports, output port number of 0xFFFF if not found.
 *          Of course, you can also search by USB vendor VID and product PID (need to record VID and PID for each device in advance), as well as specify a search sequence number
 *
 * @param   type    - Device type to search for
 *
 * @return  Output: high 8 bits are ROOT-HUB port number, low 8 bits are external HUB port number, if low 8 bits are 0 then the device is directly on the ROOT-HUB port
 */
uint16_t SearchTypeDevice(uint8_t type)
{
    uint8_t RootHubIndex; //CH554 only has one USB port, RootHubIndex = 0, only need to look at the low eight bits of the return value
    uint8_t HubPortIndex;

    RootHubIndex = 0;
    if((ThisUsbDev.DeviceType == USB_DEV_CLASS_HUB) && (ThisUsbDev.DeviceStatus >= ROOT_DEV_SUCCESS)) // External hub HUB and enumeration successful
    {
        for(HubPortIndex = 1; HubPortIndex <= ThisUsbDev.GpHUBPortNum; HubPortIndex++) // Search each port of the external HUB
        {
            if(DevOnHubPort[HubPortIndex - 1].DeviceType == type && DevOnHubPort[HubPortIndex - 1].DeviceStatus >= ROOT_DEV_SUCCESS)
            {
                return (((uint16_t)RootHubIndex << 8) | HubPortIndex); // Type matches and enumeration successful
            }
        }
    }
    if((ThisUsbDev.DeviceType == type) && (ThisUsbDev.DeviceStatus >= ROOT_DEV_SUCCESS))
    {
        return ((uint16_t)RootHubIndex << 8); // Type matches and enumeration successful, on the ROOT-HUB port
    }

    return (0xFFFF);
}

/*********************************************************************
 * @fn      SETorOFFNumLock
 *
 * @brief   NumLock LED control determination
 *
 * @param   buf     - LED control key value
 *
 * @return  Error code
 */
uint8_t SETorOFFNumLock(uint8_t *buf)
{
    uint8_t tmp[] = {0x21, 0x09, 0x00, 0x02, 0x00, 0x00, 0x01, 0x00};
    uint8_t len, s;
    if((buf[2] == 0x53) & ((buf[0] | buf[1] | buf[3] | buf[4] | buf[5] | buf[6] | buf[7]) == 0))
    {
        for(s = 0; s != sizeof(tmp); s++)
        {
            ((uint8_t *)pSetupReq)[s] = tmp[s];
        }
        s = HostCtrlTransfer(Com_Buffer, &len); // Execute control transfer
        if(s != ERR_SUCCESS)
        {
            return (s);
        }
    }
    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      CtrlGetHIDDeviceReport
 *
 * @brief   Get HID device report descriptor, returned in TxBuffer
 *
 * @param   none
 *
 * @return  Error code
 */
uint8_t CtrlGetHIDDeviceReport(uint8_t infc)
{
    uint8_t s;
    uint8_t len;

    CopySetupReqPkg(SetupSetHIDIdle);
    pSetupReq->wIndex = infc;
    s = HostCtrlTransfer(Com_Buffer, &len); // Execute control transfer
    if(s != ERR_SUCCESS)
    {
        return (s);
    }

    CopySetupReqPkg(SetupGetHIDDevReport);
    pSetupReq->wIndex = infc;
    s = HostCtrlTransfer(Com_Buffer, &len); // Execute control transfer
    if(s != ERR_SUCCESS)
    {
        return (s);
    }

    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      CtrlGetHubDescr
 *
 * @brief   Get HUB descriptor, returned in Com_Buffer
 *
 * @param   none
 *
 * @return  Error code
 */
uint8_t CtrlGetHubDescr(void)
{
    uint8_t s;
    uint8_t len;

    CopySetupReqPkg(SetupGetHubDescr);
    s = HostCtrlTransfer(Com_Buffer, &len); // Execute control transfer
    if(s != ERR_SUCCESS)
    {
        return (s);
    }
    if(len < ((PUSB_SETUP_REQ)SetupGetHubDescr)->wLength)
    {
        return (ERR_USB_BUF_OVER); // Descriptor length error
    }
    //  if ( len < 4 ) return( ERR_USB_BUF_OVER );                                 // Descriptor length error
    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      HubGetPortStatus
 *
 * @brief   Query HUB port status, return in Com_Buffer
 *
 * @param   HubPortIndex    - Port number
 *
 * @return  Error code
 */
uint8_t HubGetPortStatus(uint8_t HubPortIndex)
{
    uint8_t s;
    uint8_t len;

    pSetupReq->bRequestType = HUB_GET_PORT_STATUS;
    pSetupReq->bRequest = HUB_GET_STATUS;
    pSetupReq->wValue = 0x0000;
    pSetupReq->wIndex = 0x0000 | HubPortIndex;
    pSetupReq->wLength = 0x0004;
    s = HostCtrlTransfer(Com_Buffer, &len); // Execute control transfer
    if(s != ERR_SUCCESS)
    {
        return (s);
    }
    if(len < 4)
    {
        return (ERR_USB_BUF_OVER); // Descriptor length error
    }
    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      HubSetPortFeature
 *
 * @brief   Set HUB port feature
 *
 * @param   HubPortIndex    - Port number
 * @param   FeatureSelt     - Port feature
 *
 * @return  Error code
 */
uint8_t HubSetPortFeature(uint8_t HubPortIndex, uint8_t FeatureSelt)
{
    pSetupReq->bRequestType = HUB_SET_PORT_FEATURE;
    pSetupReq->bRequest = HUB_SET_FEATURE;
    pSetupReq->wValue = 0x0000 | FeatureSelt;
    pSetupReq->wIndex = 0x0000 | HubPortIndex;
    pSetupReq->wLength = 0x0000;
    return (HostCtrlTransfer(NULL, NULL)); // Execute control transfer
}

/*********************************************************************
 * @fn      HubClearPortFeature
 *
 * @brief   Clear HUB port feature
 *
 * @param   HubPortIndex    - Port number
 * @param   FeatureSelt     - Port feature
 *
 * @return  Error code
 */
uint8_t HubClearPortFeature(uint8_t HubPortIndex, uint8_t FeatureSelt)
{
    pSetupReq->bRequestType = HUB_CLEAR_PORT_FEATURE;
    pSetupReq->bRequest = HUB_CLEAR_FEATURE;
    pSetupReq->wValue = 0x0000 | FeatureSelt;
    pSetupReq->wIndex = 0x0000 | HubPortIndex;
    pSetupReq->wLength = 0x0000;
    return (HostCtrlTransfer(NULL, NULL)); // Execute control transfer
}
