## aes_base64 ##

里面放的是aes base64 相关的操作

有LoRaWAN payload 的加密及解密

加密模拟的是节点端(16进制数据负载)

解密模拟的是服务器端(JSON包数据负载)

其中修改密码在LoRaMac.c 文件

## generate_key ##

里面放的是OTAA 生成NwkSKey和AppSKey的算法

