#include "util.h"
uint32_t exp10(uint8_t power){
    uint32_t total=1;
    while(power-->0)
        total*=10;
    return total;
}
uint16_t textToInt(uint8_t *text){
    uint16_t num=0,count=0,len=strlen(text);
    while(count<len)
        num+=((uint8_t)text[count++]-48)*exp10(len-count);
    return num;
}
uint8_t parity(uint8_t number){
    return (((number&1)+((number>>1)&1)+((number>>2)&1)+((number>>3)&1)+((number>>4)&1)+((number>>5)&1)+((number>>6)&1)+((number>>7)&1))&0x1)!=1;
}
void intToText(uint8_t *str,uint8_t num){
    uint8_t pos=0,len=strlen(str);
    while(len)
        str[--len]=0;
    if(num/100>0){
        str[pos++]=(num/100)+48;
        num-=num/100*100;
    }
    if(num/10>0){
        str[pos++]=(num/10)+48;
        num-=num/10*10;
    }
    str[pos++]=num+48;
    str[pos]=0;
}
