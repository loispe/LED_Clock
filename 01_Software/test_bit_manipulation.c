//I'm using this file to test out the bit manipulations for the LED matrices. It allows faster debugging than using the serial monitor 

#include <stdio.h>

void modifyHour(int bitchain){
        int h_leds = bitchain;
        int mask_k = ((1 << 3) - 1) << 3;       //mask to isolate the lower 3 bits of "kkkk" from 0b0kkkkaaa
        int mask_a = (1 << 3) - 1;              //mask to isolate "aaa" from 0b0kkkkaaa
        int a, k;
        
        for(int i = 0; i <= 12; i++){
                if(i == 0){
                        h_leds = bitchain;
                }
                else{
                        k = (h_leds & mask_k) >> 2;     //apply mask and shift 0b00kkk000 >> 2 --> 0b0000kkk0
                        a = h_leds & mask_a;            //apply mask

                        if(i % 4 == 0) a <<= 1;         //true --> reached last led of the current matrix row. a << 1 correlates to selecting the next matrix row
                        else k |= 1;                    //false --> select next led in the current matrix row. k |= 1 in combination with the >> 2 creates the necessary bitchain

                        h_leds = (k << 3) | a;
                }
                printf("%x\n", h_leds);
        }
}

void modifyMinute(int bitchain){
        int m_leds = bitchain;
        int mask_k = ((1 << 8) - 1) << 8;       //mask to isolate the lower 3 bits of "kkkk" from 0b0kkkkaaa
        int mask_a = (1 << 8) - 1;              //mask to isolate "aaa" from 0b0kkkkaaa
        int a, k;
        
        for(int i = 0; i < 60; i++){
                if(i == 0){
                        m_leds = bitchain;
                }
                else{
                        k = (m_leds & mask_k) >> 7;     //apply mask and shift 0b00kkk000 >> 2 --> 0b0000kkk0
                        a = m_leds & mask_a;            //apply mask

                        if(i % 8 == 0) a <<= 1;         //true --> reached last led of the current matrix row. a << 1 correlates to selecting the next matrix row
                        else k |= 1;                    //false --> select next led in the current matrix row. k |= 1 in combination with the >> 2 creates the necessary bitchain

                        m_leds = ((k & 0xFF) << 8) | a;         //& 0xFF not necessary, since its a 16 bit int and the trailing bit will be lost when shifted by 8 anyway. Leaving it for good measure
                }
                printf("%x\n", m_leds);
        }
}

void main(){
    int bitchain_h = 0b01110001;
    int bitchain_m = 0b1111111000000001;
    
    //modifyHour(bitchain);
    modifyMinute(bitchain_m);
}