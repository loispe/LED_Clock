#define BTN_SET 5       //Pin 5 --> Button Set
#define BTN_ADD 4       //Pin 4 --> Button Add
#define H_SER   3       //Pin 3 --> Hour-SR (14, SER): Serial data input
#define H_SCK   2       //Pin 2 --> Hour-SR (11, SCK): Shift register clock input 
#define H_RCK   A3      //Pin A3 --> Hour-SR (12, RCK): Storage register clock input 
#define M_SER   A2      //Pin A2 --> Minute-SR (14, SER): Seriaial data input
#define M_SCK   A1      //Pin A1 --> Minute-SR (11, SCK): Shift register clock input
#define M_RCK   A0      //Pin A0 --> Minute-SR (12, RCK): Storage register clock input

/*
Matrix mxn definition:
         n = 7
        
        xxxxxxx
        xxxxxxx
 m = 5  xxxxxxx
        xxxxxxx
        xxxxxxx
*/

#define H_MATRIX_M 3
#define H_MATRIX_N 4
#define M_MATRIX_M 8
#define M_MATRIX_N 8

#define H_INIT 0b01110001                 //Bitchain for turning on LED_12. 0b0aaaakkk  where a = anode, k = kathode. E.g. "aaaa" adresses the matrix columns. "1110" will set column 0 to '0'; 1, 2 and 3 to '1'
#define M_INIT 0b0000000000000000

//sets the output of the desired shift register with a given bitchain
void srDataOut(uint8_t data, uint8_t sck, uint8_t rck, uint8_t ser){

       digitalWrite(rck, LOW);

       for(uint8_t i = 0; i <= (H_MATRIX_M + H_MATRIX_N); i++){
              digitalWrite(ser, (data & 128) != 0);     //(data & 128) != 0 checks state of the 8th (MSB) bit, returns its value
              data <<= 1;
              
              digitalWrite(sck, HIGH);
              digitalWrite(sck, LOW);
       }

       digitalWrite(rck, HIGH);
}

uint8_t generateHourBitchain(uint8_t hour_dec){
       uint8_t h_bitchain = H_INIT;
       uint8_t mask_a = ((1 << 3) - 1) << 3;       //mask to isolate the lower three "a"'s of "aaaa" from 0b0aaaakkk 
       uint8_t mask_k = (1 << 3) - 1;              //mask to isolate "kkk" from 0b0aaaakkk
       uint8_t a, k;

       //implement check if hour_dec is between 0 and 11. Otherwise return a predefined error bitchain!

       for (uint8_t i = 0; i <= hour_dec; i++){
              if(i != 0){
                     a = (h_bitchain & mask_a) >> 2;    //apply mask and shift 0b00aaa000 >> 2 --> 0b0000aaa0
                     k = h_bitchain & mask_k;

                     if(i % 4 == 0) k <<= 1;            //true --> reached last led of the current matrix row. k << 1 correlates to selecting the next matrix row
                     else a |= 1;                       //false --> select next led in the current matrix row. a |= 1 in combination with the >> 2 creates the expected bitchain

                     h_bitchain = (a << 3) | k;         //recreate bitchain pattern 0b0aaaakkk
              }
       }
       return h_bitchain;
}

void setup(){
       pinMode(BTN_SET, INPUT);
       pinMode(BTN_ADD, INPUT);
       pinMode(H_SER, OUTPUT);
       pinMode(H_SCK, OUTPUT);
       pinMode(H_RCK, OUTPUT);
       pinMode(M_SER, OUTPUT);
       pinMode(M_SCK, OUTPUT);
       pinMode(M_RCK, OUTPUT);

}

void loop(){

       //simple test of hour led implementation
       for(uint8_t i = 0; i < 12; i++){
              srDataOut(generateHourBitchain(i), H_SCK, H_RCK, H_SER);
              delay(25);
       }
}
