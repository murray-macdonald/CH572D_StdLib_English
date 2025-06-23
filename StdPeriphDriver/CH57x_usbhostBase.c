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

uint8_t       UsbDevEndp0Size; // Maximum packet size for endpoint 0 of the USB device
uint8_t       FoundNewDev;
_RootHubDev   ThisUsbDev;                  //ROOT port
_DevOnHubPort DevOnHubPort[HUB_MAX_PORTS]; // Assumption: no more than 1 external HUB, each external HUB has no more than HUB_MAX_PORTS ports (additional ports are ignored)

uint8_t *pHOST_RX_RAM_Addr;
uint8_t *pHOST_TX_RAM_Addr;

/* Get device descriptor */
__attribute__((aligned(4))) const uint8_t SetupGetDevDescr[] = {USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00,
                                                                USB_DESCR_TYP_DEVICE, 0x00, 0x00, sizeof(USB_DEV_DESCR), 0x00};
/* Get configuration descriptor */
__attribute__((aligned(4))) const uint8_t SetupGetCfgDescr[] = {USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00,
                                                                USB_DESCR_TYP_CONFIG, 0x00, 0x00, 0x04, 0x00};
/* Set USB address */
__attribute__((aligned(4))) const uint8_t SetupSetUsbAddr[] = {USB_REQ_TYP_OUT, USB_SET_ADDRESS, USB_DEVICE_ADDR, 0x00,
                                                               0x00, 0x00, 0x00, 0x00};
/* Set USB configuration */
__attribute__((aligned(4))) const uint8_t SetupSetUsbConfig[] = {USB_REQ_TYP_OUT, USB_SET_CONFIGURATION, 0x00, 0x00, 0x00,
                                                                 0x00, 0x00, 0x00};
/* Set USB interface configuration */
__attribute__((aligned(4))) const uint8_t SetupSetUsbInterface[] = {USB_REQ_RECIP_INTERF, USB_SET_INTERFACE, 0x00, 0x00,
                                                                    0x00, 0x00, 0x00, 0x00};
/* Clear endpoint STALL */
__attribute__((aligned(4))) const uint8_t SetupClrEndpStall[] = {USB_REQ_TYP_OUT | USB_REQ_RECIP_ENDP, USB_CLEAR_FEATURE,
                                                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/*********************************************************************
 * @fn      DisableRootHubPort
 *
 * @brief   Close the ROOT-HUB port; hardware has already automatically closed it, this just clears some structure states
 *
 * @param   none
 *
 * @return  none
 */
void DisableRootHubPort(void)
{
#ifdef FOR_ROOT_UDISK_ONLY
    CHRV3DiskStatus = DISK_DISCONNECT;
#endif
#ifndef DISK_BASE_BUF_LEN
    ThisUsbDev.DeviceStatus = ROOT_DEV_DISCONNECT;
    ThisUsbDev.DeviceAddress = 0x00;
#endif
}

/*********************************************************************
 * @fn      AnalyzeRootHub
 *
 * @brief   Analyze ROOT-HUB status, process device plug-in/removal events on ROOT-HUB port
 * If a device is removed, the function calls DisableRootHubPort() to close the port; if inserted, set the corresponding port status bit
 *
 * @param   none
 *
 * @return  Returns ERR_SUCCESS if no changes, ERR_USB_CONNECT if new connection detected, ERR_USB_DISCON if disconnection detected
 */
uint8_t AnalyzeRootHub(void)
{
    uint8_t s;

    s = ERR_SUCCESS;

    if(R8_USB_MIS_ST & RB_UMS_DEV_ATTACH)
    { // Device exists
#ifdef DISK_BASE_BUF_LEN
        if(CHRV3DiskStatus == DISK_DISCONNECT
#else
        if(ThisUsbDev.DeviceStatus == ROOT_DEV_DISCONNECT // Device insertion detected
#endif
           || (R8_UHOST_CTRL & RB_UH_PORT_EN) == 0x00)
        {                         // Device insertion detected, but not yet enabled, indicating it was just inserted
            DisableRootHubPort(); // Close the port
#ifdef DISK_BASE_BUF_LEN
            CHRV3DiskStatus = DISK_CONNECT;
#else
            ThisUsbDev.DeviceSpeed = R8_USB_MIS_ST & RB_UMS_DM_LEVEL ? 0 : 1;
            ThisUsbDev.DeviceStatus = ROOT_DEV_CONNECTED; // Set connection flag
#endif
            PRINT("USB dev in\n");
            s = ERR_USB_CONNECT;
        }
    }

#ifdef DISK_BASE_BUF_LEN
    else if(CHRV3DiskStatus >= DISK_CONNECT)
    {
#else
    else if(ThisUsbDev.DeviceStatus >= ROOT_DEV_CONNECTED)
    { // Device removal detected
#endif
        DisableRootHubPort(); // Close the port
        PRINT("USB dev out\n");
        if(s == ERR_SUCCESS)
        {
            s = ERR_USB_DISCON;
        }
    }
    //	R8_USB_INT_FG = RB_UIF_DETECT;                                                  // Clear interrupt flag
    return (s);
}

/*********************************************************************
 * @fn      SetHostUsbAddr
 *
 * @brief   Set the USB device address currently operated by the USB host
 *
 * @param   addr    - USB device address
 *
 * @return  none
 */
void SetHostUsbAddr(uint8_t addr)
{
    R8_USB_DEV_AD = (R8_USB_DEV_AD & RB_UDA_GP_BIT) | (addr & MASK_USB_ADDR);
}

/*********************************************************************
 * @fn      SetUsbSpeed
 *
 * @brief   Set the current USB speed
 *
 * @param   FullSpeed   - USB speed
 *
 * @return  none
 */
void SetUsbSpeed(uint8_t FullSpeed)
{
#ifndef DISK_BASE_BUF_LEN
    if(FullSpeed) // Full speed
    {
        R8_USB_CTRL &= ~RB_UC_LOW_SPEED;  // Full speed
        R8_UH_SETUP &= ~RB_UH_PRE_PID_EN; // Disable PRE PID
    }
    else
    {
        R8_USB_CTRL |= RB_UC_LOW_SPEED; // Low speed
    }
#endif
    (void)FullSpeed;
}

/*********************************************************************
 * @fn      ResetRootHubPort
 *
 * @brief   After detecting a device, reset the bus, prepare for device enumeration, set to full speed by default
 *
 * @param   none
 *
 * @return  none
 */
void ResetRootHubPort(void)
{
    UsbDevEndp0Size = DEFAULT_ENDP0_SIZE; // Maximum packet size for endpoint 0 of the USB device
    SetHostUsbAddr(0x00);
    R8_UHOST_CTRL &= ~RB_UH_PORT_EN;                                      // Disable the port
    SetUsbSpeed(1);                                                       // Default to full speed
    R8_UHOST_CTRL = (R8_UHOST_CTRL & ~RB_UH_LOW_SPEED) | RB_UH_BUS_RESET; // Default to full speed, start reset
    mDelaymS(15);                                                         // Reset time 10mS to 20mS
    R8_UHOST_CTRL = R8_UHOST_CTRL & ~RB_UH_BUS_RESET;                     // End reset
    mDelayuS(250);
    R8_USB_INT_FG = RB_UIF_DETECT; // Clear interrupt flag
}

/*********************************************************************
 * @fn      EnableRootHubPort
 *
 * @brief   Enable ROOT-HUB port, set the corresponding bUH_PORT_EN to 1 to enable port, device disconnection may cause return failure
 *
 * @param   none
 *
 * @return  Returns ERR_SUCCESS if new connection detected, ERR_USB_DISCON if no connection
 */
uint8_t EnableRootHubPort(void)
{
#ifdef DISK_BASE_BUF_LEN
    if(CHRV3DiskStatus < DISK_CONNECT)
        CHRV3DiskStatus = DISK_CONNECT;
#else
    if(ThisUsbDev.DeviceStatus < ROOT_DEV_CONNECTED)
        ThisUsbDev.DeviceStatus = ROOT_DEV_CONNECTED;
#endif
    if(R8_USB_MIS_ST & RB_UMS_DEV_ATTACH)
    { // Device exists
#ifndef DISK_BASE_BUF_LEN
        if((R8_UHOST_CTRL & RB_UH_PORT_EN) == 0x00)
        { // Not yet enabled
            ThisUsbDev.DeviceSpeed = (R8_USB_MIS_ST & RB_UMS_DM_LEVEL) ? 0 : 1;
            if(ThisUsbDev.DeviceSpeed == 0)
            {
                R8_UHOST_CTRL |= RB_UH_LOW_SPEED; // Low speed
            }
        }
#endif
        R8_UHOST_CTRL |= RB_UH_PORT_EN; // Enable HUB port
        return (ERR_SUCCESS);
    }
    return (ERR_USB_DISCON);
}

#ifndef DISK_BASE_BUF_LEN
/*********************************************************************
 * @fn      SelectHubPort
 *
 * @brief   Select the HUB port to operate on
 *
 * @param   HubPortIndex    - Select a specific port of the external HUB on the specified ROOT-HUB port
 *
 * @return  None
 */
void SelectHubPort(uint8_t HubPortIndex)
{
    if(HubPortIndex) // Select a specific port of the external HUB on the specified ROOT-HUB port
    {
        SetHostUsbAddr(DevOnHubPort[HubPortIndex - 1].DeviceAddress); // Set the USB device address currently operated by the USB host
        SetUsbSpeed(DevOnHubPort[HubPortIndex - 1].DeviceSpeed);      // Set the current USB speed
        if(DevOnHubPort[HubPortIndex - 1].DeviceSpeed == 0)           // Communication with low-speed USB device through external HUB requires PRE PID
        {
            R8_UEP1_CTRL |= RB_UH_PRE_PID_EN; // Enable PRE PID
            mDelayuS(100);
        }
    }
    else
    {
        SetHostUsbAddr(ThisUsbDev.DeviceAddress); // Set the USB device address currently operated by the USB host
        SetUsbSpeed(ThisUsbDev.DeviceSpeed);      // Set the USB device speed
    }
}
#endif

/*********************************************************************
 * @fn      WaitUSB_Interrupt
 *
 * @brief   Wait for USB interrupt
 *
 * @param   none
 *
 * @return  Returns ERR_SUCCESS if data is received or sent successfully, returns ERR_USB_UNKNOWN if data receive or send failed
 */
uint8_t WaitUSB_Interrupt(void)
{
    uint16_t i;
    for(i = WAIT_USB_TOUT_200US; i != 0 && (R8_USB_INT_FG & RB_UIF_TRANSFER) == 0; i--)
    {
        ;
    }
    return ((R8_USB_INT_FG & RB_UIF_TRANSFER) ? ERR_SUCCESS : ERR_USB_UNKNOWN);
}

/*********************************************************************
 * @fn      USBHostTransact
 *
 * @brief   Transport transaction - input endpoint address/PID token, synchronization flag, and NAK retry time in 20uS units
 *          (0 means no retry, 0xFFFF means unlimited retries), returns 0 on success, retries on timeout/error
 *          This subroutine emphasizes ease of understanding, while in practical applications, the code should be optimized for better performance
 *
 * @param   endp_pid    - Token and address, high 4 bits are token_pid token, low 4 bits are endpoint address
 * @param   tog         - Synchronization flag
 * @param   timeout     - Timeout period
 *
 * @return  ERR_USB_UNKNOWN Timeout, possible hardware exception
 *          ERR_USB_DISCON  Device disconnected
 *          ERR_USB_CONNECT Device connected
 *          ERR_SUCCESS     Transfer completed
 */
uint8_t USBHostTransact(uint8_t endp_pid, uint8_t tog, uint32_t timeout)
{
    uint8_t TransRetry;

    uint8_t  s, r;
    uint16_t i;

    R8_UH_RX_CTRL = R8_UH_TX_CTRL = tog;
    TransRetry = 0;

    do
    {
        R8_UH_EP_PID = endp_pid; // Specify token PID and destination endpoint number
        R8_USB_INT_FG = RB_UIF_TRANSFER;
        for(i = WAIT_USB_TOUT_200US; i != 0 && (R8_USB_INT_FG & RB_UIF_TRANSFER) == 0; i--)
        {
            ;
        }
        R8_UH_EP_PID = 0x00; // Stop USB transmission
        if((R8_USB_INT_FG & RB_UIF_TRANSFER) == 0)
        {
            return (ERR_USB_UNKNOWN);
        }

        if(R8_USB_INT_FG & RB_UIF_DETECT)
        { // USB device plug/unplug event
            //			mDelayuS( 200 );                                                       // Wait for transfer completion
            R8_USB_INT_FG = RB_UIF_DETECT;
            s = AnalyzeRootHub(); // Analyze ROOT-HUB status

            if(s == ERR_USB_CONNECT)
                FoundNewDev = 1;
#ifdef DISK_BASE_BUF_LEN
            if(CHRV3DiskStatus == DISK_DISCONNECT)
            {
                return (ERR_USB_DISCON);
            } // USB device disconnection event
            if(CHRV3DiskStatus == DISK_CONNECT)
            {
                return (ERR_USB_CONNECT);
            } // USB device connection event
#else
            if(ThisUsbDev.DeviceStatus == ROOT_DEV_DISCONNECT)
            {
                return (ERR_USB_DISCON);
            } // USB device disconnection event
            if(ThisUsbDev.DeviceStatus == ROOT_DEV_CONNECTED)
            {
                return (ERR_USB_CONNECT);
            } // USB device connection event
#endif
            mDelayuS(200); // Wait for transfer completion
        }

        if(R8_USB_INT_FG & RB_UIF_TRANSFER) // Transfer completion event
        {
            if(R8_USB_INT_ST & RB_UIS_TOG_OK)
            {
                return (ERR_SUCCESS);
            }
            r = R8_USB_INT_ST & MASK_UIS_H_RES; // USB device response status
            if(r == USB_PID_STALL)
            {
                return (r | ERR_USB_TRANSFER);
            }
            if(r == USB_PID_NAK)
            {
                if(timeout == 0)
                {
                    return (r | ERR_USB_TRANSFER);
                }
                if(timeout < 0xFFFFFFFF)
                {
                    timeout--;
                }
                --TransRetry;
            }
            else
                switch(endp_pid >> 4)
                {
                    case USB_PID_SETUP:
                    case USB_PID_OUT:
                        if(r)
                        {
                            return (r | ERR_USB_TRANSFER);
                        }      // Not timeout/error, unexpected response
                        break; // Timeout retry
                    case USB_PID_IN:
                        if(r == USB_PID_DATA0 || r == USB_PID_DATA1)
                        { // If not synchronized, discard and retry
                        } // Retry due to synchronization issue
                        else if(r)
                        {
                            return (r | ERR_USB_TRANSFER);
                        }      // Not timeout/error, unexpected response
                        break; // Timeout retry
                    default:
                        return (ERR_USB_UNKNOWN); // Impossible case
                        break;
                }
        }
        else
        {                         // Other interrupts, should not happen
            R8_USB_INT_FG = 0xFF; /* Clear interrupt flag */
        }
        mDelayuS(15);
    } while(++TransRetry < 3);
    return (ERR_USB_TRANSFER); // Response timeout
}

/*********************************************************************
 * @fn      HostCtrlTransfer
 *
 * @brief   Execute control transfer, 8-byte request code in pSetupReq, DataBuf is an optional send/receive buffer
 *
 * @param   DataBuf     - If you need to receive and send data, DataBuf needs to point to a valid buffer for subsequent data storage
 * @param   RetLen      - Actual successful send/receive total length is stored in the byte variable pointed to by RetLen
 *
 * @return  ERR_USB_BUF_OVER    Error in IN status phase
 *          ERR_SUCCESS         Data exchange successful
 */
uint8_t HostCtrlTransfer(uint8_t *DataBuf, uint8_t *RetLen)
{
    uint16_t RemLen = 0;
    uint8_t  s, RxLen, RxCnt, TxCnt;
    uint8_t *pBuf;
    uint8_t *pLen;

    pBuf = DataBuf;
    pLen = RetLen;
    mDelayuS(200);
    if(pLen)
    {
        *pLen = 0; // Actual total length successfully sent/received
    }

    R8_UH_TX_LEN = sizeof(USB_SETUP_REQ);
    s = USBHostTransact(USB_PID_SETUP << 4 | 0x00, 0x00, 200000 / 20); // SETUP stage, 200mS timeout
    if(s != ERR_SUCCESS)
    {
        return (s);
    }
    R8_UH_RX_CTRL = R8_UH_TX_CTRL = RB_UH_R_TOG | RB_UH_R_AUTO_TOG | RB_UH_T_TOG | RB_UH_T_AUTO_TOG; // Default DATA1
    R8_UH_TX_LEN = 0x01;                                                                             // By default, no data so status stage is IN
    RemLen = pSetupReq->wLength;
    PRINT("wLength: %x\n", RemLen);
    if(RemLen && pBuf) // Need to send/receive data
    {
        PRINT("bRequestType: %x\n", pSetupReq->bRequestType);
        if(pSetupReq->bRequestType & USB_REQ_TYP_IN) // Receive
        {
            while(RemLen)
            {
                mDelayuS(200);
                s = USBHostTransact(USB_PID_IN << 4 | 0x00, R8_UH_RX_CTRL, 200000 / 20); // IN data
                if(s != ERR_SUCCESS)
                {
                    return (s);
                }
                RxLen = R8_USB_RX_LEN < RemLen ? R8_USB_RX_LEN : RemLen;
                RemLen -= RxLen;
                if(pLen)
                {
                    *pLen += RxLen; // Actual total length successfully received
                }
                for(RxCnt = 0; RxCnt != RxLen; RxCnt++)
                {
                    *pBuf = pHOST_RX_RAM_Addr[RxCnt];
                    pBuf++;
                }
                if(R8_USB_RX_LEN == 0 || (R8_USB_RX_LEN & (UsbDevEndp0Size - 1)))
                {
                    break; // Short packet
                }
            }
            R8_UH_TX_LEN = 0x00; // Status stage is OUT
        }
        else // Send
        {
            while(RemLen)
            {
                mDelayuS(200);
                R8_UH_TX_LEN = RemLen >= UsbDevEndp0Size ? UsbDevEndp0Size : RemLen;
                for(TxCnt = 0; TxCnt != R8_UH_TX_LEN; TxCnt++)
                {
                    pHOST_TX_RAM_Addr[TxCnt] = *pBuf;
                    pBuf++;
                }
                s = USBHostTransact(USB_PID_OUT << 4 | 0x00, R8_UH_TX_CTRL, 200000 / 20); // OUT data
                if(s != ERR_SUCCESS)
                {
                    return (s);
                }
                RemLen -= R8_UH_TX_LEN;
                if(pLen)
                {
                    *pLen += R8_UH_TX_LEN; // Actual total length successfully sent
                }
            }
            PRINT("Send: %d\n", *pLen);
            //          R8_UH_TX_LEN = 0x01;                                                     // Status stage is IN
        }
    }
    mDelayuS(200);
    s = USBHostTransact((R8_UH_TX_LEN ? USB_PID_IN << 4 | 0x00 : USB_PID_OUT << 4 | 0x00), RB_UH_R_TOG | RB_UH_T_TOG, 200000 / 20); // STATUS stage
    if(s != ERR_SUCCESS)
    {
        return (s);
    }
    if(R8_UH_TX_LEN == 0)
    {
        return (ERR_SUCCESS); // Status OUT
    }
    if(R8_USB_RX_LEN == 0)
    {
        return (ERR_SUCCESS); // Status IN, check IN status return data length
    }
    return (ERR_USB_BUF_OVER); // Error in IN status phase
}

/*********************************************************************
 * @fn      CopySetupReqPkg
 *
 * @brief   Copy the control transfer request packet
 *
 * @param   pReqPkt     - Control request packet address
 *
 * @return  none
 */
void CopySetupReqPkg(const uint8_t *pReqPkt) // Copy the control transfer request packet
{
    uint8_t i;
    for(i = 0; i != sizeof(USB_SETUP_REQ); i++)
    {
        ((uint8_t *)pSetupReq)[i] = *pReqPkt;
        pReqPkt++;
    }
}

/*********************************************************************
 * @fn      CtrlGetDeviceDescr
 *
 * @brief   Get device descriptor, returned in pHOST_TX_RAM_Addr
 *
 * @param   none
 *
 * @return  ERR_USB_BUF_OVER    Descriptor length error
 *          ERR_SUCCESS         Success
 */
uint8_t CtrlGetDeviceDescr(void)
{
    uint8_t s;
    uint8_t len;

    UsbDevEndp0Size = DEFAULT_ENDP0_SIZE;
    CopySetupReqPkg(SetupGetDevDescr);
    s = HostCtrlTransfer(Com_Buffer, &len); // Execute control transfer
    if(s != ERR_SUCCESS)
    {
        return (s);
    }
    UsbDevEndp0Size = ((PUSB_DEV_DESCR)Com_Buffer)->bMaxPacketSize0; // Maximum packet size for endpoint 0, this is simplified processing; normally should get first 8 bytes, immediately update UsbDevEndp0Size, then continue
    if(len < ((PUSB_SETUP_REQ)SetupGetDevDescr)->wLength)
    {
        return (ERR_USB_BUF_OVER); // Descriptor length error
    }
    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      CtrlGetConfigDescr
 *
 * @brief   Get configuration descriptor, returned in pHOST_TX_RAM_Addr
 *
 * @param   none
 *
 * @return  ERR_USB_BUF_OVER    Descriptor length error
 *          ERR_SUCCESS         Success
 */
uint8_t CtrlGetConfigDescr(void)
{
    uint8_t s;
    uint8_t len;

    CopySetupReqPkg(SetupGetCfgDescr);
    s = HostCtrlTransfer(Com_Buffer, &len); // Execute control transfer
    if(s != ERR_SUCCESS)
    {
        return (s);
    }
    if(len < ((PUSB_SETUP_REQ)SetupGetCfgDescr)->wLength)
    {
        return (ERR_USB_BUF_OVER); // Return length error
    }

    len = ((PUSB_CFG_DESCR)Com_Buffer)->wTotalLength;
    CopySetupReqPkg(SetupGetCfgDescr);
    pSetupReq->wLength = len;               // Total length of the complete configuration descriptor
    s = HostCtrlTransfer(Com_Buffer, &len); // Execute control transfer
    if(s != ERR_SUCCESS)
    {
        return (s);
    }

#ifdef DISK_BASE_BUF_LEN
    if(len > 64)
        len = 64;
    memcpy(TxBuffer, Com_Buffer, len); // For USB flash drive operations, need to copy to TxBuffer
#endif

    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      CtrlSetUsbAddress
 *
 * @brief   Set USB device address
 *
 * @param   addr    - Device address
 *
 * @return  ERR_SUCCESS     Success
 */
uint8_t CtrlSetUsbAddress(uint8_t addr)
{
    uint8_t s;

    CopySetupReqPkg(SetupSetUsbAddr);
    pSetupReq->wValue = addr;         // USB device address
    s = HostCtrlTransfer(NULL, NULL); // Execute control transfer
    if(s != ERR_SUCCESS)
    {
        return (s);
    }
    SetHostUsbAddr(addr); // Set the USB device address currently operated by the USB host
    mDelaymS(10);         // Wait for the USB device to complete operation
    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      CtrlSetUsbConfig
 *
 * @brief   Set USB device configuration
 *
 * @param   cfg     - Configuration value
 *
 * @return  ERR_SUCCESS     Success
 */
uint8_t CtrlSetUsbConfig(uint8_t cfg)
{
    CopySetupReqPkg(SetupSetUsbConfig);
    pSetupReq->wValue = cfg;               // USB device configuration
    return (HostCtrlTransfer(NULL, NULL)); // Execute control transfer
}

/*********************************************************************
 * @fn      CtrlClearEndpStall
 *
 * @brief   Clear endpoint STALL
 *
 * @param   endp    - Endpoint address
 *
 * @return  ERR_SUCCESS     Success
 */
uint8_t CtrlClearEndpStall(uint8_t endp)
{
    CopySetupReqPkg(SetupClrEndpStall); // Clear endpoint error
    pSetupReq->wIndex = endp;                     // Endpoint address
    return (HostCtrlTransfer(NULL, NULL));        // Execute control transfer
}

/*********************************************************************
 * @fn      CtrlSetUsbIntercace
 *
 * @brief   Set USB device interface
 *
 * @param   cfg     - Configuration value
 *
 * @return  ERR_SUCCESS     Success
 */
uint8_t CtrlSetUsbIntercace(uint8_t cfg)
{
    CopySetupReqPkg(SetupSetUsbInterface);
    pSetupReq->wValue = cfg;               // USB device configuration
    return (HostCtrlTransfer(NULL, NULL)); // Execute control transfer
}

/*********************************************************************
 * @fn      USB_HostInit
 *
 * @brief   USB host function initialization
 *
 * @param   none
 *
 * @return  none
 */
void USB_HostInit(void)
{
    R8_USB_CTRL = RB_UC_HOST_MODE;
    R8_UHOST_CTRL = 0;
    R8_USB_DEV_AD = 0x00;

    R8_UH_EP_MOD = RB_UH_EP_TX_EN | RB_UH_EP_RX_EN;
    R16_UH_RX_DMA = (uint32_t)pHOST_RX_RAM_Addr;
    R16_UH_TX_DMA = (uint32_t)pHOST_TX_RAM_Addr;

    R8_UH_RX_CTRL = 0x00;
    R8_UH_TX_CTRL = 0x00;
    R8_USB_CTRL = RB_UC_HOST_MODE | RB_UC_INT_BUSY | RB_UC_DMA_EN;
    R8_UH_SETUP = RB_UH_SOF_EN;
    R8_USB_INT_FG = 0xFF;
    DisableRootHubPort();
    R8_USB_INT_EN = RB_UIE_TRANSFER | RB_UIE_DETECT;

    FoundNewDev = 0;
}
