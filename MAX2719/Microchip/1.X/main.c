/*
 * File:   main.c
 * Author: user
 *
 * Created on 17 de Janeiro de 2020, 20:47
 */

#include <xc.h>

#pragma config FOSC = HS
#pragma config WDT = OFF
#pragma config LVP = OFF

#define _XTAL_FREQ 20000000
#define time 5

void settings(void);
void start(void);
//SPI
void SendSPI(unsigned char Data);
unsigned char ReceivedSPI(void);
void WriteSPI(unsigned char Register, unsigned char *Data, int Ndatos);
void ReadSPI(unsigned char Register, unsigned char *Data, int Nbytes);
//MAX7219
void SettingsMAX7219(void);
void CleanMAX7219(void);

int i;
unsigned char byte[5],a[8]={0x00,0xFE,0x11,0x11,0x11,0x11,0xFE,0x00};

void main(void) {
    settings();
    while(1){
        start();
    }
}

void settings(void){
    ADCON1 = 0x0F;
    TRISBbits.RB0 = 1; //SDI
    TRISBbits.RB1 = 0; //SCK
    TRISCbits.RC7 = 0; //SDO
    TRISAbits.RA5 = 0; //SS
    LATAbits.LATA5 = 1; //Desactiva el esclavo
    //SPI
    SSPSTAT = 0x40;
    SSPCON1 = 0x20;
    SSPIF = 0;
    //Configuración del MAX7219
    SettingsMAX7219();
    CleanMAX7219();
}

void start(void){
    WriteSPI(0x01,a,8); 
    __delay_ms(100);
    //ReadSPI(0x02,byte,5);
}

void SendSPI(unsigned char Data){ //Intercambio de datos
    unsigned char buffer;
    buffer = SSPBUF;
    SSPBUF = Data;
    while(SSPIF == 0){};
    SSPIF = 0;
    buffer = SSPBUF;
}

unsigned char ReceivedSPI(void){
    SSPBUF = 0x00;
    while(SSPIF == 0){};
    SSPIF = 0;
    return SSPBUF;
}

void WriteSPI(unsigned char Register, unsigned char *Data, int Ndatos){
    for(i=0; i<Ndatos; i++){
        LATAbits.LATA5 = 0;
        SendSPI(Register+i); //columna matriz de leds
        SendSPI(*Data); //Filas matriz de leds
        LATAbits.LATA5 = 1;
        __delay_ms(time);
        Data++;
    }
    __delay_ms(1000);
}

void ReadSPI(unsigned char Register, unsigned char *Data, int Nbytes){
    LATAbits.LATA5 = 0;
    SendSPI(0x03); //Lee datos de la matriz de memoria de la EEPROM comenzando en la dirección seleccionada (datasheet)
    SendSPI(Register);
    for(i=0; i<Nbytes; i++){
        *Data = ReceivedSPI();
        Data++;
    }
    LATAbits.LATA5 = 1;
    __delay_ms(1000);
}

void SettingsMAX7219(void){
    LATAbits.LATA5 = 0; //Activa el esclavo
    SendSPI(0x09); //Registro de "Decode mode"
    SendSPI(0x00); //(datasheet)
    LATAbits.LATA5 = 1; //Desactiva el esclavo
    __delay_ms(time);
    LATAbits.LATA5 = 0; //Activa el esclavo
    SendSPI(0x0A); //Registro de "Intensity"
    SendSPI(0x0F); //Brillo al máximo(datasheet)
    LATAbits.LATA5 = 1; //Desactiva el esclavo
    __delay_ms(time);
    LATAbits.LATA5 = 0; //Activa el esclavo
    SendSPI(0x0B); //Registro de "Scan limit"
    SendSPI(0x07); //Habilitar todas las columnas de la matriz de leds(datasheet)
    LATAbits.LATA5 = 1; //Desactiva el esclavo
    __delay_ms(time);
    LATAbits.LATA5 = 0; //Activa el esclavo
    SendSPI(0x0C); //Registro de "Shutdown"
    SendSPI(0x01); //(datasheet)
    LATAbits.LATA5 = 1; //Desactiva el esclavo
    __delay_ms(time);
    LATAbits.LATA5 = 0; //Activa el esclavo
    SendSPI(0x0F); //Registro de "Display test"
    SendSPI(0x00); //(datasheet)
    LATAbits.LATA5 = 1; //Desactiva el esclavo
    __delay_ms(time);
}

void CleanMAX7219(void){
    for(i=1; i<=8; i++){
        LATAbits.LATA5 = 0; //Activa el esclavo
        SendSPI(i); //Registro de "Display test"
        SendSPI(0); //(datasheet)
        LATAbits.LATA5 = 1; //Desactiva el esclavo
        __delay_ms(time);
    }    
}