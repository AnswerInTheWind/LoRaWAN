#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "base64.h"
#include "LoRaMac.h"
#include "LoRaMacCrypto.h"

/*!
 * AES encryption/decryption cipher network session key
 */
uint8_t LoRaMacNwkSKey[] =
{
    0x2B,0x7E,0x15,0x16,0x28,0xAE,0xD2,0xA6,
    0xAB,0xF7,0x15,0x88,0x09,0xCF,0x4F,0x3C
};

/*!
 * AES encryption/decryption cipher application session key
 */
uint8_t LoRaMacAppSKey[] =
{
    0x2B,0x7E,0x15,0x16,0x28,0xAE,0xD2,0xA6,
    0xAB,0xF7,0x15,0x88,0x09,0xCF,0x4F,0x3C
};


//������̻���
void clear_input_cache()
{
    int c;
    while((c = getchar()) != '\n' && c != EOF);
}

void encrypt()
{
    uint16_t frame_no;
    uint16_t frame_no_arr[2] = {0};
    uint16_t payload_len;
    uint8_t payload_buf[100] = {0};
    uint8_t encrypt_buf[100] = {0};
    uint8_t i = 0;
    uint32_t address = 0;
    uint32_t address_arr[4] = {0};

    printf("����֡���,�м��Զ��Ÿ���(С��)\n");
    scanf("%x,%x",(int*)frame_no_arr,(int*)frame_no_arr+1);
    clear_input_cache();

    frame_no = frame_no_arr[1]<<8;
    frame_no += frame_no_arr[0];

    printf("���븺�س���\n");
    scanf("%d",(int*)&payload_len);
    clear_input_cache();

    printf("�����豸��ַ,�м��Զ��Ÿ���(С��)\n");
    scanf("0x%x,0x%x,0x%x,0x%x",(int*)address_arr,(int*)address_arr+1,(int*)address_arr+2,(int*)address_arr+3);
    clear_input_cache();

    printf("���븺��(16����),�м��Զ��Ÿ���\n");
    for(i=0;i<payload_len-1;i++)
        scanf("0x%x,",(int*)(payload_buf+i));
    scanf("0x%x",(int*)(payload_buf+i));
    clear_input_cache();

    address += address_arr[3]<<24;
    address += address_arr[2]<<16;
    address += address_arr[1]<<8;
    address += address_arr[0];

    LoRaMacPayloadEncrypt( payload_buf, payload_len, LoRaMacAppSKey, address, UP_LINK, frame_no, encrypt_buf );

    printf("AES���ܺ�ĸ�������Ϊ\n");
    for(i = 0;i<payload_len;i++)
        printf("0x%02x ",encrypt_buf[i]);
    printf("\n");
}

void Decrypt()
{
    char input[200] = {0};
    char base64_decoded[100] = {0};
    payload_buf_t AES_decoded_payload;
    uint8_t len = 0;
    uint8_t i = 0;
    uint8_t dir = 0;
    uint8_t dir_buf[20];

    printf("input JSON:data\r\n");
    scanf("%s",input);
    printf("input the direction of last input packet\r\n 1:UP_LINK \r\n 2:DOWN_LINK\r\n");
    scanf("%s",dir_buf);
    if((dir_buf[0] != '1') && (dir_buf[0] != '2'))
    {
        printf("Input Error\r\n\r\n");
    }
    else
    {
        if(dir_buf[0] == '1')
            dir = UP_LINK;
        else
            dir = DOWN_LINK;
    }
    printf("encryption : %s \r\n",input);

    len = base64_decode(input,200,base64_decoded);//����Ӧ��дbase64����ǰ���ַ����ĳ���
    memset(input,0,200);

    if(AES_decryption((uint8_t *)base64_decoded,len,&AES_decoded_payload,dir))
    {
        printf("decryption: ");
        for(i = 0;i<AES_decoded_payload.len;i++)
            printf("%x ",AES_decoded_payload.aes_decode_buf[i]);
        printf("\r\n \r\n");
    }
    else
        printf("direction ERROR\r\n\r\n");
    memset(base64_decoded,0,100);
    memset(AES_decoded_payload.aes_decode_buf,0,100);
}

int main()
{

    char select = 0;

    while(1)
    {
//        for(i =0;i<70;i++)
//            printf("0x%x,",i);

        printf("1:AES����\n");
        printf("2:BASE+AES����\n");
        printf("3:�˳�����\n");
        scanf("%c",&select);
        clear_input_cache();

        if(select == '1')
        {
            encrypt();

        }
        else if(select == '2')
        {
            Decrypt();
        }
        else if(select == '3')
        {
            exit(0);
        }

    }
    return 0;
}
