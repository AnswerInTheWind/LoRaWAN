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


//
//LoRaMacStatus_t PrepareFrame( LoRaMacHeader_t *macHdr, LoRaMacFrameCtrl_t *fCtrl, uint8_t fPort, void *fBuffer, uint16_t fBufferSize )
//{
//    uint16_t i;
//    uint8_t pktHeaderLen = 0;
//    uint32_t mic = 0;
//    const void* payload = fBuffer;
//    uint8_t payloadSize = fBufferSize;
//    uint8_t framePort = fPort;
//
//    LoRaMacBufferPktLen = 0;
//
//    NodeAckRequested = false;
//
//    if( fBuffer == NULL )
//    {
//        fBufferSize = 0;
//    }
//
//    LoRaMacBuffer[pktHeaderLen++] = macHdr->Value;
//
//    switch( macHdr->Bits.MType )
//    {
//        case FRAME_TYPE_JOIN_REQ:
//            RxWindow1Delay = LoRaMacParams.JoinAcceptDelay1 - RADIO_WAKEUP_TIME;
//            RxWindow2Delay = LoRaMacParams.JoinAcceptDelay2 - RADIO_WAKEUP_TIME;
//
//            LoRaMacBufferPktLen = pktHeaderLen;
//
//            memcpyr( LoRaMacBuffer + LoRaMacBufferPktLen, LoRaMacAppEui, 8 );
//            LoRaMacBufferPktLen += 8;
//            memcpyr( LoRaMacBuffer + LoRaMacBufferPktLen, LoRaMacDevEui, 8 );
//            LoRaMacBufferPktLen += 8;
//
//            LoRaMacDevNonce = Radio.Random( );
//
//            LoRaMacBuffer[LoRaMacBufferPktLen++] = LoRaMacDevNonce & 0xFF;
//            LoRaMacBuffer[LoRaMacBufferPktLen++] = ( LoRaMacDevNonce >> 8 ) & 0xFF;
//
//            LoRaMacJoinComputeMic( LoRaMacBuffer, LoRaMacBufferPktLen & 0xFF, LoRaMacAppKey, &mic );
//
//            LoRaMacBuffer[LoRaMacBufferPktLen++] = mic & 0xFF;
//            LoRaMacBuffer[LoRaMacBufferPktLen++] = ( mic >> 8 ) & 0xFF;
//            LoRaMacBuffer[LoRaMacBufferPktLen++] = ( mic >> 16 ) & 0xFF;
//            LoRaMacBuffer[LoRaMacBufferPktLen++] = ( mic >> 24 ) & 0xFF;
//
//            break;
//        case FRAME_TYPE_DATA_CONFIRMED_UP:
//            NodeAckRequested = true;
//            //Intentional falltrough
//        case FRAME_TYPE_DATA_UNCONFIRMED_UP:
//            if( IsLoRaMacNetworkJoined == false )
//            {
//                return LORAMAC_STATUS_NO_NETWORK_JOINED; // No network has been joined yet
//            }
//
//            fCtrl->Bits.AdrAckReq = AdrNextDr( fCtrl->Bits.Adr, true, &LoRaMacParams.ChannelsDatarate );
//
//            if( ValidatePayloadLength( fBufferSize, LoRaMacParams.ChannelsDatarate, MacCommandsBufferIndex ) == false )
//            {
//                return LORAMAC_STATUS_LENGTH_ERROR;
//            }
//
//            RxWindow1Delay = LoRaMacParams.ReceiveDelay1 - RADIO_WAKEUP_TIME;
//            RxWindow2Delay = LoRaMacParams.ReceiveDelay2 - RADIO_WAKEUP_TIME;
//
//            if( SrvAckRequested == true )
//            {
//                SrvAckRequested = false;
//                fCtrl->Bits.Ack = 1;
//            }
//
//            LoRaMacBuffer[pktHeaderLen++] = ( LoRaMacDevAddr ) & 0xFF;
//            LoRaMacBuffer[pktHeaderLen++] = ( LoRaMacDevAddr >> 8 ) & 0xFF;
//            LoRaMacBuffer[pktHeaderLen++] = ( LoRaMacDevAddr >> 16 ) & 0xFF;
//            LoRaMacBuffer[pktHeaderLen++] = ( LoRaMacDevAddr >> 24 ) & 0xFF;
//
//            LoRaMacBuffer[pktHeaderLen++] = fCtrl->Value;
//
//            LoRaMacBuffer[pktHeaderLen++] = UpLinkCounter & 0xFF;
//            LoRaMacBuffer[pktHeaderLen++] = ( UpLinkCounter >> 8 ) & 0xFF;
//
//            // Copy the MAC commands which must be re-send into the MAC command buffer
//            memcpy1( &MacCommandsBuffer[MacCommandsBufferIndex], MacCommandsBufferToRepeat, MacCommandsBufferToRepeatIndex );
//            MacCommandsBufferIndex += MacCommandsBufferToRepeatIndex;
//
//            if( ( payload != NULL ) && ( payloadSize > 0 ) )
//            {
//                if( ( MacCommandsBufferIndex <= LORA_MAC_COMMAND_MAX_LENGTH ) && ( MacCommandsInNextTx == true ) )
//                {
//                    fCtrl->Bits.FOptsLen += MacCommandsBufferIndex;
//
//                    // Update FCtrl field with new value of OptionsLength
//                    LoRaMacBuffer[0x05] = fCtrl->Value;
//                    for( i = 0; i < MacCommandsBufferIndex; i++ )
//                    {
//                        LoRaMacBuffer[pktHeaderLen++] = MacCommandsBuffer[i];
//                    }
//                }
//            }
//            else
//            {
//                if( ( MacCommandsBufferIndex > 0 ) && ( MacCommandsInNextTx ) )
//                {
//                    payloadSize = MacCommandsBufferIndex;
//                    payload = MacCommandsBuffer;
//                    framePort = 0;
//                }
//            }
//            MacCommandsInNextTx = false;
//            // Store MAC commands which must be re-send in case the device does not receive a downlink anymore
//            MacCommandsBufferToRepeatIndex = ParseMacCommandsToRepeat( MacCommandsBuffer, MacCommandsBufferIndex, MacCommandsBufferToRepeat );
//            if( MacCommandsBufferToRepeatIndex > 0 )
//            {
//                MacCommandsInNextTx = true;
//            }
//            MacCommandsBufferIndex = 0;
//
//            if( ( payload != NULL ) && ( payloadSize > 0 ) )
//            {
//                LoRaMacBuffer[pktHeaderLen++] = framePort;
//
//                if( framePort == 0 )
//                {
//                    LoRaMacPayloadEncrypt( (uint8_t* ) payload, payloadSize, LoRaMacNwkSKey, LoRaMacDevAddr, UP_LINK, UpLinkCounter, LoRaMacPayload );
//                }
//                else
//                {
//                    LoRaMacPayloadEncrypt( (uint8_t* ) payload, payloadSize, LoRaMacAppSKey, LoRaMacDevAddr, UP_LINK, UpLinkCounter, LoRaMacPayload );
//                }
//                memcpy1( LoRaMacBuffer + pktHeaderLen, LoRaMacPayload, payloadSize );
//            }
//            LoRaMacBufferPktLen = pktHeaderLen + payloadSize;
//
//            LoRaMacComputeMic( LoRaMacBuffer, LoRaMacBufferPktLen, LoRaMacNwkSKey, LoRaMacDevAddr, UP_LINK, UpLinkCounter, &mic );
//
//            LoRaMacBuffer[LoRaMacBufferPktLen + 0] = mic & 0xFF;
//            LoRaMacBuffer[LoRaMacBufferPktLen + 1] = ( mic >> 8 ) & 0xFF;
//            LoRaMacBuffer[LoRaMacBufferPktLen + 2] = ( mic >> 16 ) & 0xFF;
//            LoRaMacBuffer[LoRaMacBufferPktLen + 3] = ( mic >> 24 ) & 0xFF;
//
//            LoRaMacBufferPktLen += LORAMAC_MFR_LEN;
//
//            break;
//        case FRAME_TYPE_PROPRIETARY:
//            if( ( fBuffer != NULL ) && ( fBufferSize > 0 ) )
//            {
//                memcpy1( LoRaMacBuffer + pktHeaderLen, ( uint8_t* ) fBuffer, fBufferSize );
//                LoRaMacBufferPktLen = pktHeaderLen + fBufferSize;
//            }
//            break;
//        default:
//            return LORAMAC_STATUS_SERVICE_UNKNOWN;
//    }
//
//    return LORAMAC_STATUS_OK;
//}

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

