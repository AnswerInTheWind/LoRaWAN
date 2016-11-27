/*
(C)2015 NPLink

Description: LoRa MAC layer implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr
*/
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "utilities.h"


#include "LoRaMacCrypto.h"
#include "LoRaMac.h"

/*!
 * Maximum PHY layer payload size
 */
#define LORAMAC_PHY_MAXPAYLOAD                      128//64


/*!
 * Unique Devices IDs register set ( STM32L1xxx )
 */
#define         ID1                                 ( 0x1FF80050 )
#define         ID2                                 ( 0x1FF80054 )
#define         ID3                                 ( 0x1FF80064 )

/*!
* the packet serial number of mote send
*/
u16 g_frame_no = 0;

/*!
 * Mote Address
 */
//static uint32_t LoRaMacDevAddr = 0x7cd3d382;

/*!
 * LoRaMAC frame counter. Each time a packet is received the counter is incremented.
 * Only the 16 LSB bits are received
 */
static uint32_t DownLinkCounter = 0;

///*!
// * Device IEEE EUI
// */
//static uint8_t *LoRaMacDevEui;
//
///*!
// * Application IEEE EUI
// */
//static uint8_t *LoRaMacAppEui;
//
///*!
// * AES encryption/decryption cipher application key
// */
//static uint8_t *LoRaMacAppKey;
//
///*!
// * Buffer containing the upper layer data.
// */
//static uint8_t LoRaMacPayload[LORAMAC_PHY_MAXPAYLOAD];
//static uint8_t LoRaMacRxPayload[LORAMAC_PHY_MAXPAYLOAD];

/*!
 * AES encryption/decryption cipher network session key
 */
static uint8_t LoRaMacNwkSKey[] =
{
    0xde ,0x3 ,0x33 ,0x1a ,0xeb ,0x42 ,0x54 ,0xe9 ,
    0x72 ,0x7b ,0x6f ,0xaf ,0xbf ,0x13 ,0xdb ,0x3d
};

/*!
 * AES encryption/decryption cipher application session key
 */
static uint8_t LoRaMacAppSKey[] =
{
    0xe0 ,0x46 ,0x9e ,0x44 ,0x9c ,0x57 ,0x47 ,0x8c ,
    0xbe ,0xa7 ,0x25 ,0xda ,0x84 ,0xf0 ,0x13 ,0x97
};

/*!
 * Function to be executed on Rx Done event
 */
uint8_t AES_decryption( uint8_t *payload, uint16_t size,payload_buf_t *dest,uint8_t dir)
{
    LoRaMacHeader_t macHdr;
    LoRaMacFrameCtrl_t fCtrl;

    uint8_t pktHeaderLen = 0;
    uint32_t address = 0;
    uint16_t sequenceCounter = 0;
    uint8_t appPayloadStartIndex = 0;
    uint8_t port = 0xFF;
    uint8_t frameLen = 0;
    uint32_t mic = 0;
    uint32_t micRx = 0;

    uint8_t *nwkSKey = LoRaMacNwkSKey;
    uint8_t *appSKey = LoRaMacAppSKey;
    uint32_t downLinkCounter = 0;

    uint8_t isMicOk = false;

    macHdr.Value = payload[pktHeaderLen++];//0 cjk

    switch( macHdr.Bits.MType )
    {
        case FRAME_TYPE_JOIN_ACCEPT:
            break;
        case FRAME_TYPE_DATA_CONFIRMED_DOWN:
        case FRAME_TYPE_DATA_UNCONFIRMED_DOWN:
        case FRAME_TYPE_DATA_UNCONFIRMED_UP:
        {
            address = payload[pktHeaderLen++];// 1 cjk
            address |= ( payload[pktHeaderLen++] << 8 );// 2 cjk
            address |= ( payload[pktHeaderLen++] << 16 );// 3 cjk
            address |= ( payload[pktHeaderLen++] << 24 );// 4 cjk

            fCtrl.Value = payload[pktHeaderLen++];// 5 cjk

            sequenceCounter = payload[pktHeaderLen++];// 6 cjk
            sequenceCounter |= payload[pktHeaderLen++] << 8;// 7 cjk

            appPayloadStartIndex = 8 + fCtrl.Bits.FOptsLen;

            micRx |= payload[size - LORAMAC_MFR_LEN];
            micRx |= ( payload[size - LORAMAC_MFR_LEN + 1] << 8 );
            micRx |= ( payload[size - LORAMAC_MFR_LEN + 2] << 16 );
            micRx |= ( payload[size - LORAMAC_MFR_LEN + 3] << 24 );

//            downLinkCounter = ( downLinkCounter & 0xFFFF0000 ) | sequenceCounter;

            LoRaMacComputeMic( payload, size - LORAMAC_MFR_LEN, nwkSKey, address, dir, sequenceCounter, &mic );

            if( ( isMicOk == true ) || ( micRx == mic ) )
            {
                DownLinkCounter = downLinkCounter;

                if( ( ( size - 4 ) - appPayloadStartIndex ) > 0 )
                {
                    port = payload[appPayloadStartIndex++];// 8 cjk

                    frameLen = ( size - 4 ) - appPayloadStartIndex;

                    if( port == 0 )
                    {
                        LoRaMacPayloadDecrypt( payload + appPayloadStartIndex,
                                               frameLen,
                                               nwkSKey,
                                               address,
                                               DOWN_LINK,
                                               sequenceCounter,
                                               dest->aes_decode_buf);
                        dest->aes_decode_buf[frameLen] = 0;
                        dest->len = frameLen;
    //                                  // Decode frame payload MAC commands
    //                                  LoRaMacProcessMacCommands( LoRaMacRxPayload, 0, frameLen );
                    }
                    else
                    {
                        LoRaMacPayloadDecrypt( payload + appPayloadStartIndex,
                                               frameLen,
                                               appSKey,
                                               address,
                                               dir,
                                               sequenceCounter,
                                               dest->aes_decode_buf);
                        dest->aes_decode_buf[frameLen] = 0;
                        dest->len = frameLen;
    //                                  LoRaMacEventFlags.Bits.RxData = 1;
    //                                  LoRaMacEventInfo.RxPort = port;
    //                                  LoRaMacEventInfo.RxBuffer = LoRaMacRxPayload;
    //                                  LoRaMacEventInfo.RxBufferSize = frameLen;
                    }

                    return 1;
//                    memcpy(,LoRaMacRxPayload,frameLen);
                }
            }
        }
        break;
        case FRAME_TYPE_PROPRIETARY:
        //Intentional falltrough
        default:
            break;
    }

    return 0;
}

