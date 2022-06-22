#include <msp430.h>
#define cs_set() P1OUT &= 0x00
#define cs_reset() P1OUT |= 0x01

  /*
   * Board ---------->                   <----------595
   * P1.2 Data Out (UCA0SIMO)            14 - Ds serial data input
   * P1.4 Serial Clock Out (UCA0CLK)     11 - SHcp shift register clock input
   * P1.0                                12 - STcp storage register input
   *
   */

void spi_init(void);
void write_spi(unsigned int data);
unsigned char count = 0;

int main(void)
{
    spi_init();
    P1DIR |= 0x40;
    P1OUT = 0x00;
	while(1){
	    for (count = 0; count <= 8; count++){
	        write_spi(1 << count);
	        __delay_cycles(50000);
	    }
	    write_spi(0x00);
	    P1OUT |= 0x40;
	    __delay_cycles(1000000);
	    P1OUT &= 0x00;
	}
}

void write_spi(unsigned int data){
    cs_reset();
    cs_set();
    UCA0TXBUF = data;
}

void spi_init(void){
    WDTCTL = WDTPW | WDTHOLD;                     // stop watchdog timer
    P1OUT = 0x00;                                 // P1 setup for LED & reset output
    P1DIR |= BIT0 + BIT5;                         //
    P1SEL = BIT1 + BIT2 + BIT4;
    P1SEL2 = BIT1 + BIT2 + BIT4;
    UCA0CTL0 |= UCCKPL + UCMSB + UCMST + UCSYNC;  // 3-pin, 8-bit SPI master
    UCA0CTL1 |= UCSSEL_2;                         // SMCLK
    UCA0BR0 |= 0x02;                              // /2
    UCA0BR1 = 0;                                  //
    UCA0MCTL = 0;                                 // No modulation
    UCA0CTL1 &= ~UCSWRST;                         // **Initialize USCI state machine**

    __delay_cycles(75);                           // Wait for slave to initialize
}
