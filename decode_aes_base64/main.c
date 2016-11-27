#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "base64.h"
#include "LoRaMac.h"



int main()
{
    char input[200] = {0};
    char base64_decoded[100] = {0};
    payload_buf_t AES_decoded_payload;
    uint8_t len = 0;
    uint8_t i = 0;
    uint8_t dir = 0;
    uint8_t dir_buf[20];

    while(1)
    {
        printf("input JSON:data\r\n");
        scanf("%s",input);
        printf("input the direction of last input packet\r\n 1:UP_LINK \r\n 2:DOWN_LINK\r\n");
        scanf("%s",dir_buf);
        if((dir_buf[0] != '1') && (dir_buf[0] != '2'))
        {
            printf("Input Error\r\n\r\n");
            continue;
        }
        else
        {
            if(dir_buf[0] == '1')
                dir = UP_LINK;
            else
                dir = DOWN_LINK;
        }
        printf("encryption : %s \r\n",input);

        len = base64_decode(input,200,base64_decoded);//长度应填写base64解码前的字符串的长度
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
    return 0;
}
