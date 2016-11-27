/*
(C)2015 NPLink

Description: LoRa MAC layer implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr
*/
#ifndef __LORAMAC_H__
#define __LORAMAC_H__

#ifdef __cplusplus
extern "C"
{
#endif

// Includes board dependent definitions such as channels frequencies
//#include "LoRaMac_board.h"
//#include "oled_board.h"
//#include "osal.h"
#include "hal_types.h"
#include <windef.h>

//#if defined(__CC_ARM) || defined(__GNUC__)
//#define PACKED                                      __attribute__( ( __packed__ ) )
//#elif defined( __ICCARM__ )
//#define PACKED                                      __packed
//#else
//    #warning Not supported compiler type
//#endif



/*!
 * Frame direction definition
 */
#define UP_LINK                                     0
#define DOWN_LINK                                   1

/*!
 * Sets the length of the LoRaMAC footer field.
 * Mainly indicates the MIC field length
 */
#define LORAMAC_MFR_LEN                             4

typedef struct payload_buf
{
    uint8_t aes_decode_buf[100];
    uint16_t len;
}payload_buf_t;

/*!
 * LoRaMAC channels parameters definition
 */
typedef union
{
    int8_t Value;
    struct
    {
        int8_t Min : 4;
        int8_t Max : 4;
    }PACKED Fields;
}PACKED DrRange_t;

typedef struct
{
    uint16_t DCycle;
    int8_t TxMaxPower;
    uint64_t LastTxDoneTime;
    uint64_t TimeOff;
}PACKED Band_t;

typedef struct
{
    uint32_t Frequency; // Hz
    DrRange_t DrRange;  // Max datarate [0: SF12, 1: SF11, 2: SF10, 3: SF9, 4: SF8, 5: SF7, 6: SF7, 7: FSK]
                        // Min datarate [0: SF12, 1: SF11, 2: SF10, 3: SF9, 4: SF8, 5: SF7, 6: SF7, 7: FSK]
    uint8_t Band;       // Band index
}PACKED ChannelParams_t;

typedef struct
{
    uint32_t Frequency; // Hz
    uint8_t  Datarate;  // [0: SF12, 1: SF11, 2: SF10, 3: SF9, 4: SF8, 5: SF7, 6: SF7, 7: FSK]
}PACKED Rx2ChannelParams_t;

typedef struct MulticastParams_s
{
    uint32_t Address;
    uint8_t NwkSKey[16];
    uint8_t AppSKey[16];
    uint32_t DownLinkCounter;
    struct MulticastParams_s *Next;
}PACKED MulticastParams_t;

/*!
 * LoRaMAC frame types
 */
typedef enum
{
    FRAME_TYPE_JOIN_REQ              = 0x00,
    FRAME_TYPE_JOIN_ACCEPT           = 0x01,
    FRAME_TYPE_DATA_UNCONFIRMED_UP   = 0x02,
    FRAME_TYPE_DATA_UNCONFIRMED_DOWN = 0x03,
    FRAME_TYPE_DATA_CONFIRMED_UP     = 0x04,
    FRAME_TYPE_DATA_CONFIRMED_DOWN   = 0x05,
    FRAME_TYPE_RFU                   = 0x06,
    FRAME_TYPE_PROPRIETARY           = 0x07,
}PACKED LoRaMacFrameType_t;

/*!
 * LoRaMAC mote MAC commands
 */
typedef enum
{
    MOTE_MAC_LINK_CHECK_REQ          = 0x02,
    MOTE_MAC_LINK_ADR_ANS            = 0x03,
    MOTE_MAC_DUTY_CYCLE_ANS          = 0x04,
    MOTE_MAC_RX_PARAM_SETUP_ANS      = 0x05,
    MOTE_MAC_DEV_STATUS_ANS          = 0x06,
    MOTE_MAC_NEW_CHANNEL_ANS         = 0x07,
    MOTE_MAC_RX_TIMING_SETUP_ANS     = 0x08,
}PACKED LoRaMacMoteCmd_t;

/*!
 * LoRaMAC server MAC commands
 */
typedef enum
{
    SRV_MAC_LINK_CHECK_ANS           = 0x02,
    SRV_MAC_LINK_ADR_REQ             = 0x03,
    SRV_MAC_DUTY_CYCLE_REQ           = 0x04,
    SRV_MAC_RX_PARAM_SETUP_REQ       = 0x05,
    SRV_MAC_DEV_STATUS_REQ           = 0x06,
    SRV_MAC_NEW_CHANNEL_REQ          = 0x07,
    SRV_MAC_RX_TIMING_SETUP_REQ      = 0x08,
}PACKED LoRaMacSrvCmd_t;

/*!
 * LoRaMAC Battery level indicator
 */
typedef enum
{
    BAT_LEVEL_EXT_SRC                = 0x00,
    BAT_LEVEL_EMPTY                  = 0x01,
    BAT_LEVEL_FULL                   = 0xFE,
    BAT_LEVEL_NO_MEASURE             = 0xFF,
}PACKED LoRaMacBatteryLevel_t;

/*!
 * LoRaMAC header field definition
 */
typedef union
{
    uint8_t Value;
    struct
    {
        uint8_t Major           : 2;
        uint8_t RFU             : 3;
        uint8_t MType           : 3;
    }PACKED Bits;
}PACKED LoRaMacHeader_t;

/*!
 * LoRaMAC frame header field definition
 */
typedef union
{
    uint8_t Value;
    struct
    {
        uint8_t FOptsLen        : 4;
        uint8_t FPending        : 1;
        uint8_t Ack             : 1;
        uint8_t AdrAckReq       : 1;
        uint8_t Adr             : 1;
    }PACKED Bits;
}PACKED LoRaMacFrameCtrl_t;

/*!
 * LoRaMAC event flags
 */
typedef union
{
    uint8_t Value;
    struct
    {
        uint8_t Tx              : 1;
        uint8_t Rx              : 1;
        uint8_t RxData          : 1;
        uint8_t Multicast       : 1;
        uint8_t RxSlot          : 2;
        uint8_t LinkCheck       : 1;
        uint8_t JoinAccept      : 1;
    }PACKED Bits;
}PACKED LoRaMacEventFlags_t;

typedef enum
{
    LORAMAC_EVENT_INFO_STATUS_OK = 0,
    LORAMAC_EVENT_INFO_STATUS_ERROR,
    LORAMAC_EVENT_INFO_STATUS_TX_TIMEOUT,
    LORAMAC_EVENT_INFO_STATUS_RX2_TIMEOUT,
    LORAMAC_EVENT_INFO_STATUS_RX2_ERROR,
    LORAMAC_EVENT_INFO_STATUS_JOIN_FAIL,
    LORAMAC_EVENT_INFO_STATUS_DOWNLINK_FAIL,
    LORAMAC_EVENT_INFO_STATUS_ADDRESS_FAIL,
    LORAMAC_EVENT_INFO_STATUS_MIC_FAIL,
}PACKED LoRaMacEventInfoStatus_t;

/*!
 * LoRaMAC event information
 */
typedef struct
{
    LoRaMacEventInfoStatus_t Status;
    uint8_t TxAckReceived;
    uint8_t TxNbRetries;
    uint8_t TxDatarate;
    uint8_t RxPort;
    uint8_t *RxBuffer;
    uint8_t RxBufferSize;
    int16_t RxRssi;
    uint8_t RxSnr;
    uint16_t Energy;
    uint8_t DemodMargin;
    uint8_t NbGateways;
}PACKED LoRaMacEventInfo_t;


extern uint16_t g_frame_no;

/*!
 * LoRaMAC events structure
 * Used to notify upper layers of MAC events
 */
typedef struct sLoRaMacEvent
{
    /*!
     * MAC layer event callback prototype.
     *
     * \param [IN] flags Bit field indicating the MAC events occurred
     * \param [IN] info  Details about MAC events occurred
     */
    void ( *MacEvent )( LoRaMacEventFlags_t *flags, LoRaMacEventInfo_t *info );
}PACKED LoRaMacEvent_t;

extern Band_t Bands[1];
extern ChannelParams_t Channels[16];

uint8_t AES_decryption( uint8_t *payload, uint16_t size,payload_buf_t *dest,uint8_t dir);

#ifdef __cplusplus
}
#endif

#endif // __LORAMAC_H__
