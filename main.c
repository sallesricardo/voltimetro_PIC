/*
 * File:   main.c
 * Author: ricardo
 *
 * Created on 27 de Setembro de 2019, 23:04
 */


#include <xc.h>
#include <stdint.h>
#include "main.h"

#define DISPLAY PORTB
#define D_TIME 200

#define D0 PORTCbits.RC3
#define D1 PORTCbits.RC2
#define D2 PORTCbits.RC1
#define D3 PORTCbits.RC0

typedef union {
    uint16_t integer;
    uint8_t part[sizeof(uint16_t)];
} _uint16_t;

_uint16_t value_read;
uint32_t count = 0;
uint16_t miliseconds = 0;
uint32_t seconds = 0;
uint8_t dash = 0;

#define MEDIA 32
uint16_t v_array[MEDIA];
uint8_t i_array = 0;
uint32_t acc_value = 0;


uint8_t nums[] = {
//    .gfedcba
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111100, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01100111, // 9
    0b00000001
//    .abcdefg
//    0b01111110, // 0
//    0b00110000, // 1
//    0b01101101, // 2
//    0b01111001, // 3
//    0b00110011, // 4
//    0b01011011, // 5
//    0b00011111, // 6
//    0b01110000, // 7
//    0b01111111, // 8
//    0b01110011, // 9
//    0b00000001
};

void __interrupt() interrupt_function(){
    if (INTCONbits.TMR0IF){
        INTCONbits.TMR0IF = 0;
        TMR0 = 0x63;
        count++;
        miliseconds++;
        if (miliseconds == 500){
            dash = 1;
        }
        if (miliseconds >= 1000){
            miliseconds = 0;
            seconds++;
            dash = 1;
        }
    }
    if (PIR1bits.ADIF){
        PIR1bits.ADIF = 0;
        value_read.part[1] = ADRESH;
        value_read.part[0] = ADRESL;
        ADCON0bits.GO = 1;
    }
    
}

int main(void) {
    // Ports
    TRISAbits.TRISA0 = 1;
    TRISB = 0x00; //Instruct the MCU that the PORTB pins are used as Output.
    TRISC = 0xE0;
    PORTB = 0X00; //Make all output of RB3 LOW
    
    // AD
    ADCON1bits.ADFM = 1;
    ADCON1bits.ADCS2 = 1;
    ADCON0bits.ADCS = 0x01;
    ADCON1bits.PCFG = 0x0E;
    ADCON0bits.CHS = 0x00;
    ADCON0bits.ADON = 1;
    
    // Timer 0
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS = 0x03;
    TMR0 = 0x63;
    
    // Interrupts
    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1;
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
    PEIE = 1;
    GIE = 1;
    
    uint8_t i;
    for (i = 0;i < MEDIA;i++){
        v_array[i] = 0;
    }
    ADCON0bits.GO = 1;
    
    uint16_t num,d;
    uint16_t aux = 0;
    value_read.integer = 0;

    while(1) //Get into the Infinie While loop
    {
        CLRWDT();
        //num = seconds;
        //num = count / 100;
        aux = value_read.integer;
        acc_value -= v_array[i_array];
        acc_value += aux;
        v_array[i_array++] = aux;
        if (i_array >= MEDIA){
            i_array = 0;
        }
        num = acc_value / MEDIA;
        if (dash){
            RC4 = !RC4;
            dash = 0;
        }
        d = num % 10;
        DISPLAY = nums[d];
        D0=1; //LED ON
        __delay_us(D_TIME); //Wait
        D0=0; //LED OFF
        //__delay_ms(100); //Wait
        num = num / 10;
        d = num % 10;
        DISPLAY = 0x80 | nums[d];
        D1=1; //LED ON
        __delay_us(D_TIME); //Wait
        D1=0; //LED OFF
        //__delay_ms(100); //Wait
        num = num / 10;
        d = num % 10;
        DISPLAY = nums[d];
        D2=1; //LED ON
        __delay_us(D_TIME); //Wait
        D2=0; //LED OFF
        //__delay_ms(100); //Wait
        num = num / 10;
        d = num % 10;
        DISPLAY = nums[d];
        D3=1; //LED ON
        __delay_us(D_TIME); //Wait
        D3=0; //LED OFF
        ///__delay_ms(100); //Wait
        //RC4=0; //LED OFF
        CLRWDT();
        DISPLAY = 0x00;
        //__delay_ms(100); 
        //Repeat. 
    }

}
