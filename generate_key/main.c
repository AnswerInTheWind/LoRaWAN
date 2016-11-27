#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "string.h"
#include "LoRaMacCrypto.h"
#include "aes.h"
#include "cmac.h"


uint8_t payload[] = {0x20, 0xfa, 0x80, 0x29, 0x74, 0x3b, 0x2d, 0x2f,0xc2, 0x99, 0x85, 0x42, 0x0f, 0x2f, 0x0a, 0xde,0x4e};



uint8_t LoRaMacRxPayload[50] = {0};

uint8_t LoRaMacAppKey[] = {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};
uint32_t mic = 0;

uint16_t LoRaMacDevNonce = 0x7b54;
uint8_t LoRaMacNwkSKey[16] = {0};
uint8_t LoRaMacAppSKey[16] = {0};

int main()
{
    uint8_t size = 0;
    uint8_t i = 0;
    size = sizeof(payload);
    memset(LoRaMacRxPayload,0,50);
    LoRaMacJoinDecrypt( payload + 1, size - 1, LoRaMacAppKey, LoRaMacRxPayload + 1 );

    LoRaMacRxPayload[0] = payload[0];

    LoRaMacJoinComputeMic( LoRaMacRxPayload, size - 4, LoRaMacAppKey, &mic );

    for(i = 0;i<size;i++)
        printf("0x%x ",LoRaMacRxPayload[i]);
    printf("\r\n");

    LoRaMacJoinComputeSKeys( LoRaMacAppKey, LoRaMacRxPayload + 1, LoRaMacDevNonce, LoRaMacNwkSKey, LoRaMacAppSKey );

    printf("NWKSKEY:");
    for(i = 0;i<16;i++)
        printf("0x%x ",LoRaMacNwkSKey[i]);
    printf("\r\n");

    printf("APPSKEY:");
    for(i = 0;i<16;i++)
        printf("0x%x ",LoRaMacAppSKey[i]);
    printf("\r\n");

    while(1);
    return 0;
}
