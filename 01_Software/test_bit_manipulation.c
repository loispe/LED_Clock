//I'm using this file to test out the bit manipulations for the LED matrices. It allows faster debugging than using the serial monitor 

#include <stdio.h>

void modifyHour(int bitchain){
        int h_leds = bitchain;
        int mask_a = ((1 << 3) - 1) << 3;       //mask to isolate the lower 3 bits of "aaaa" from 0b0aaaakkk 
        int mask_k = (1 << 3) - 1;              //mask to isolate "kkk" from 0b0aaaakkk
        int a, k;
        
        for(int i = 0; i <= 12; i++){
                if(i == 0){
                        h_leds = bitchain;
                }
                else{
                        a = (h_leds & mask_a) >> 2;     //apply mask and shift 0b00aaa000 >> 2 --> 0b0000aaa0
                        k = h_leds & mask_k;            //apply mask

                        if(i % 4 == 0) k <<= 1;         //true --> reached last led of the current matrix row. k << 1 correlates to selecting the next matrix row
                        else a |= 1;                    //false --> select next led in the current matrix row. a |= 1 in combination with the >> 2 creates the necessary bitchain

                        h_leds = (a << 3) | k;
                }
                printf("%x\n", h_leds);
        }
}

void main(){
    int bitchain = 0b01110001;
    modifyHour(bitchain);

}