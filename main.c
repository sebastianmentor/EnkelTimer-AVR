#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

volatile uint8_t led_state = 0;

//############################################
//#                UART                      #
//############################################

void uart_init(uint16_t ubrr) {
    // Ställ in baudrate
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;

    // Aktivera sändning
    UCSR0B = (1 << TXEN0);

    // Asynkron, 8 bit, 1 stopbit
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_send_char(char c) {
    while (!(UCSR0A & (1 << UDRE0))); // Vänta tills sändbuffert är tom
    UDR0 = c;
}

void uart_send_string(const char* str) {
    for (size_t i = 0; i < strlen(str); i++) {
        uart_send_char(str[i]);
    }
}

//############################################
//#                END UART                  #
//############################################

//############################################
//#                 TIMER                    #
//############################################

void timer1_init(void) {
    // Se kapitel 14 till 18 för olika timerkonfigureringar

    // TCCR1A = Timer/Counter1 Control Register A
    // TCCR1B = Timer/Counter1 Control Register B
    TCCR1A = 0;
    TCCR1B = 0;

    // Timer/Counter1
    TCNT1 = 0;

    // CSxx - Clock Select
    // Prescaler = 1024
    TCCR1B |= (1 << CS12) | (1 << CS10); // -- > xxxxx000 | 00000100 | 00000001 --> xxxxx101
    // 00000001 << 0 --> 00000001 == (1 << CS10)
    // 00000001 << 1 --> 00000010 
    // 00000001 << 2 --> 00000100 == (1 << CS12)
    // ...
    // 00000001 << 8 --> 00000000
    // ...
    // 
    // TIMSK1 - Timer/Counter1 Interrupt Mask
    // TIFR1 - Timer/Counter1 Interrupt Flag
    // Aktivera overflow interrupt
    TIMSK1 |= (1 << TOIE1);
}

//############################################
//#                 GPIO                     #
//############################################

void io_init(void) {
    DDRB |= (1 << DDB5); // Pin 13 som output
}

//############################################
//#                 END GPIO                 #
//############################################

//############################################
//#                   MAIN                    #
//############################################

int main(void) {
    io_init();
    uart_init(103);   // 9600 baud @ 16 MHz → UBRR = 103
    timer1_init();
    sei();            // Aktivera globala avbrott

    while (1) {
        // Ingenting behövs i huvudloopen
    }
}

//############################################
//#                 END MAIN                 #
//############################################

//############################################
//#                   ISR                    #
//############################################

// ISR --> Interupt Sub Rutine
ISR(TIMER1_OVF_vect) {
    // Växla LED
    if (led_state) {
        PORTB &= ~(1 << PORTB5);
        led_state = 0;
    } else {
        PORTB |= (1 << PORTB5);
        led_state = 1;
    }

    // Skicka meddelande via UART
    uart_send_string("Overflow\r\n");
}

//############################################
//#                 END ISR                  #
//############################################

