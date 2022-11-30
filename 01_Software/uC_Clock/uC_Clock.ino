#define BTN_SET 5       //Pin 5 --> Button Set
#define BTN_ADD 4       //Pin 4 --> Button Add
#define H_SER   3       //Pin 3 --> Hour-SR (14, SER): Serial data input
#define H_SCK   2       //Pin 2 --> Hour-SR (11, SCK): Shift register clock input 
#define H_RCK   A3      //Pin A3 --> Hour-SR (12, RCK): Storage register clock input 
#define M_SER   A2      //Pin A2 --> Minute-SR (14, SER): Seriaial data input
#define M_SCK   A1      //Pin A1 --> Minute-SR (11, SCK): Shift register clock input
#define M_RCK   A0      //Pin A0 --> Minute-SR (12, RCK): Storage register clock input

#define SR_CLK_P 50*10^3     // 50kHz

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

#define H_ZERO 0b1000111

