#include <Wire.h>

#define DS1307_ADDR 0x68
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

#define H_INIT 0b01110001                 //Bitchain for turning on LED_12. 0b0kkkkaaa  where a = anode, k = kathode. E.g. "kkkk" adresses the matrix columns. "1110" will set column 0 to '0'; 1, 2 and 3 to '1'
#define M_INIT 0b1111111000000001

#define BTN_DELAY 200                     //150 ms delay after each button press to enhance usability and prevent bouncing
#define REFRESH_CYCLE 10000000            //Refresh the time every 10 seconds. 
#define SET_MINUTE 0x00
#define SET_HOUR 0xFF



//sets the output of the desired shift register with a given bitchain
//--------------------------------------------------------------------------------------------------------------------------------------------
void srDataOut(uint16_t data, uint8_t sck, uint8_t rck, uint8_t ser, boolean isHour = false){

       uint16_t MSB = 0x8000;      //16 Bit data
       uint8_t lim = M_MATRIX_M + M_MATRIX_N;

       if(isHour){                 //8 Bit data
              MSB = 0x0080;   
              lim = H_MATRIX_M + H_MATRIX_N + 1;
       } 

       digitalWrite(rck, LOW);

       for(uint8_t i = 0; i < lim; i++){
              digitalWrite(ser, (data & MSB) != 0);     //(data & 128) != 0 checks state of the 8th (MSB) bit, returns its value
              data <<= 1;
              
              digitalWrite(sck, HIGH);                  
              digitalWrite(sck, LOW);
       }

       digitalWrite(rck, HIGH);
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//Generates the bitchain for the hour led matrix from a decimal input number between 0 and 11
uint8_t generateHourBitchain(uint8_t hour_dec){
       uint8_t h_bitchain = H_INIT;
       uint8_t mask_k = ((1 << 3) - 1) << 3;       //mask to isolate the lower three "k"'s of "kkkk" from 0b0kkkkaaa 
       uint8_t mask_a = (1 << 3) - 1;              //mask to isolate "aaa" from 0b0kkkkaaa
       uint8_t k, a;

       //implement check if hour_dec is between 0 and 11. Otherwise return a predefined error bitchain!

       for (uint8_t i = 0; i <= (hour_dec % 12); i++){
              if(i != 0){
                     k = (h_bitchain & mask_k) >> 2;    //apply mask and shift 0b00kkk000 >> 2 --> 0b0000kkk0
                     a = h_bitchain & mask_a;

                     if(i % 4 == 0) a <<= 1;            //true --> reached last led of the current matrix row. a << 1 correlates to selecting the next matrix row
                     else k |= 1;                       //false --> select next led in the current matrix row. k |= 1 in combination with the >> 2 creates the expected bitchain

                     h_bitchain = ((k & 0xF) << 3) | a;         //recreate bitchain pattern 0b0kkkkaaa. Remove trailing ones for good measure. Otherwise they would be cut of by the limiting datatype uint8_t 
              }
       }
       return h_bitchain;
}
//--------------------------------------------------------------------------------------------------------------------------------------------
//Generates the bitchain for the minute led matrix from a decimal input number between 0 and 59
uint16_t generateMinuteBitchain(uint8_t minute_dec){
        uint16_t m_bitchain = M_INIT;
        uint16_t mask_k = ((1 << 8) - 1) << 8;       //mask to isolate the lower 3 bits of "kkkk" from 0b0kkkkaaa
        uint16_t mask_a = (1 << 8) - 1;              //mask to isolate "aaa" from 0b0kkkkaaa
        uint8_t a, k;
        
        for(uint8_t i = 0; i <= minute_dec; i++){
                if(i != 0){
                     k = (m_bitchain & mask_k) >> 7;     //apply mask and shift 0b00kkk000 >> 2 --> 0b0000kkk0
                     a = m_bitchain & mask_a;            //apply mask

                     if(i % 8 == 0) a <<= 1;         //true --> reached last led of the current matrix row. a << 1 correlates to selecting the next matrix row
                     else k |= 1;                    //false --> select next led in the current matrix row. k |= 1 in combination with the >> 2 creates the necessary bitchain

                     m_bitchain = ((k & 0xFF) << 8) | a;         //& 0xFF not necessary, since its a 16 bit int and the trailing bit will be lost when shifted by 8 anyway. Leaving it for good measure
                }
        }
       return m_bitchain;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//Requests the time from DS1307 via I2C
void showCurrentTime(){
       Wire.beginTransmission(DS1307_ADDR);
       Wire.write(0x00);
       Wire.endTransmission();
       Wire.requestFrom(DS1307_ADDR, 3);

       uint8_t sec = bcdToDec(Wire.read());
       uint8_t min = bcdToDec(Wire.read());
       uint8_t hour = bcdToDec(Wire.read() & 0b111111);

       srDataOut(generateHourBitchain(hour), H_SCK, H_RCK, H_SER, true);
       srDataOut(generateMinuteBitchain(min), M_SCK, M_RCK, M_SER);
}


//--------------------------------------------------------------------------------------------------------------------------------------------
//Enables the time setting with "set" and "add" button. Writes the result to the DS1307 via I2C
void setCurrentTime(){

       uint8_t set_press_cnt = 0;
       uint8_t hour = 0, min = 0;

       srDataOut(generateMinuteBitchain(0), M_SCK, M_RCK, M_SER);
       srDataOut(generateHourBitchain(0), H_SCK, H_RCK, H_SER, true);

       while(set_press_cnt < 2){
              
              delay(BTN_DELAY * 3);

              if(set_press_cnt == 0){
                     //Readout add-button presses and turn on leds respectively
                     //Hour LEDs: 
                     while(digitalRead(BTN_SET) == 0){
                            if (digitalRead(BTN_ADD) == 1) hour += 1;
                            if (hour >= 12) hour = 0;
                            srDataOut(generateHourBitchain(hour), H_SCK, H_RCK, H_SER);
                            delay(BTN_DELAY);
                     }
                     set_press_cnt += 1;
              }
              else if(set_press_cnt = 1){
                     //Minute LEDs:
                     while(digitalRead(BTN_SET) == 0){
                            if (digitalRead(BTN_ADD) == 1) min += 1;
                            if (min >= 60) min = 0;
                            srDataOut(generateMinuteBitchain(min), M_SCK, M_RCK, M_SER);
                            delay(BTN_DELAY);
                     }
                     delay(BTN_DELAY);
                     break;
              }
       }

       Wire.beginTransmission(DS1307_ADDR);
       Wire.write(uint8_t(0x00));
       Wire.write(decToBcd(0));
       Wire.write(decToBcd(min));
       Wire.write(decToBcd(hour));
       Wire.write(uint8_t(0x00));
       Wire.endTransmission();
}

//--------------------------------------------------------------------------------------------------------------------------------------------
uint8_t bcdToDec(uint8_t value){
       return ((value / 16 * 10) + (value % 16));
}

uint8_t decToBcd(uint8_t value){
       return ((value / 10 * 16) + (value % 10));
}

//--------------------------------------------------------------------------------------------------------------------------------------------
void setup(){
       Wire.begin();
       
       pinMode(BTN_SET, INPUT);
       pinMode(BTN_ADD, INPUT);
       pinMode(H_SER, OUTPUT);
       pinMode(H_SCK, OUTPUT);
       pinMode(H_RCK, OUTPUT);
       pinMode(M_SER, OUTPUT);
       pinMode(M_SCK, OUTPUT);
       pinMode(M_RCK, OUTPUT);

       srDataOut(0x0000, M_SCK, M_RCK, M_SER, false);
       srDataOut(0x00 , H_SCK, H_RCK, H_SER, true);

       //Startup animation :)
       for(uint8_t i = 0; i < 3; i++){
              for(uint8_t j = 0; j < 60; j++){
                     if(j % 5 == 0){
                            srDataOut(generateHourBitchain(12 - (j/5)), H_SCK, H_RCK, H_SER, true);
                     }
                     srDataOut(generateMinuteBitchain(j), M_SCK, M_RCK, M_SER);
                     delay(25);
              }
       }
}
//--------------------------------------------------------------------------------------------------------------------------------------------
void loop(){

       uint8_t time_init = millis();

       //Check time set button
       if(digitalRead(BTN_SET) == 1)
              setCurrentTime();

       //Update time every REFRESH_CYCLE seconds
       if(time_init - millis() > REFRESH_CYCLE)
              showCurrentTime();
}
