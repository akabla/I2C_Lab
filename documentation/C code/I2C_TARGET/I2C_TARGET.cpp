#include <stdio.h>
#include "pico/stdlib.h"
#include "target_lib.h"

// ----------------------------------------------------------------------------
// Hardware Pin Definitions
// ----------------------------------------------------------------------------

#define TARGET_ADDRESS_1 0x30 // == 0110 000X -- needs to match the first 7 bits of transmission
#define TARGET_ADDRESS_2 0x32 // == 0110 010x
#define TARGET_ADDRESS_3 0x34 // == 0110 100x
#define TARGET_ADDRESS_4 0x36 // == 0110 110x

//Target Input Pins
#define TARGET_SDA_PIN 18  
#define TARGET_SCL_PIN 19

//Target LED Pins
//TARGET 1
#define LED_PIN_IDLE_1 2 //target is waiting for communication to start
#define LED_PIN_START_1 3 //target is reading the address frame
#define LED_PIN_RECEIVED_1 4 // address matches target's and target is in read mode
#define LED_PIN_TRANSMITTING_1 5 //address matches target's and target is in write mode 

//TARGET 2
#define LED_PIN_IDLE_2 6 //target is waiting for communication to start
#define LED_PIN_START_2 7 //target is reading the address frame
#define LED_PIN_RECEIVED_2 8 // address matches target's and target is in read mode
#define LED_PIN_TRANSMITTING_2 9 //address matches target's and target is in write mode 

//TARGET 3
#define LED_PIN_IDLE_3 10 //target is waiting for communication to start
#define LED_PIN_START_3 11 //target is reading the address frame
#define LED_PIN_RECEIVED_3 12 // address matches target's and target is in read mode
#define LED_PIN_TRANSMITTING_3 13 //address matches target's and target is in write mode 

//TARGET 4
#define LED_PIN_IDLE_4 14 //target is waiting for communication to start
#define LED_PIN_START_4 15 //target is reading the address frame
#define LED_PIN_RECEIVED_4 16 // address matches target's and target is in read mode
#define LED_PIN_TRANSMITTING_4 17 //address matches target's and target is in write mode 

// ----------------------------------------------------------------------------
// LED Helper Functions
// ----------------------------------------------------------------------------

void init_leds()
{
    for(int i = 2 ; i <= 17 ; i++) {
        gpio_init(i);
        gpio_set_dir(i,1);
        gpio_put(i,0);
    }

    gpio_init(TARGET_SDA_PIN);
    gpio_init(TARGET_SCL_PIN);

    gpio_set_dir(TARGET_SCL_PIN,0);
    gpio_set_dir(TARGET_SDA_PIN,0);

    gpio_put(LED_PIN_IDLE_1,1);
    gpio_put(LED_PIN_IDLE_2,1);
    gpio_put(LED_PIN_IDLE_3,1);
    gpio_put(LED_PIN_IDLE_4,1);
}

void clear_leds_all()
{
    for(int i = 2 ; i <= 17 ; i++) gpio_put(i,0);
}

void clear_leds_1()
{
    for(int i = 2 ; i <= 5 ; i++) gpio_put(i,0);
}

void clear_leds_2()
{
    for(int i = 6 ; i <= 9 ; i++) gpio_put(i,0);
}

void clear_leds_3()
{
    for(int i = 10 ; i <= 13 ; i++) gpio_put(i,0);
}

void clear_leds_4()
{
    for(int i = 14 ; i <= 17 ; i++) gpio_put(i,0);
}

void cycle_leds()
{
    for (int i = 2 ; i<= 17 ; i ++) {
        clear_leds_all();
        gpio_put(i,1);
        sleep_ms(50);
    }
    clear_leds_all();

    gpio_put(LED_PIN_IDLE_1,1);
    gpio_put(LED_PIN_IDLE_2,1);
    gpio_put(LED_PIN_IDLE_3,1);
    gpio_put(LED_PIN_IDLE_4,1);
}

// ----------------------------------------------------------------------------
// TARGET Event Callback Handler
// ----------------------------------------------------------------------------

//TARGET 1 -- increments by 1
void target_event_handler_1(volatile uint8_t &buffer,volatile i2c_target_memory &target_memory, const target_event event) 
{
    switch(event) {
        case EVENT_TARGET_START:
        //when START condition is met
            clear_leds_1(); gpio_put(LED_PIN_START_1,1);
            break;

        case EVENT_TARGET_RECEIVED :
            clear_leds_1();
            gpio_put(LED_PIN_RECEIVED_1 , 1);

            target_memory.memory[target_memory.byte_count] = buffer + 1;
            //target_memory.byte_count ++;

            break;

        case EVENT_TARGET_TRANSMITTING:
            clear_leds_1();
            gpio_put(LED_PIN_TRANSMITTING_1,1);
            
            buffer = target_memory.memory[target_memory.byte_count];
            //target_memory.byte_count --;

            break;

        case EVENT_TARGET_STOP :
            //when STOP condition is met
            clear_leds_1();
            gpio_put(LED_PIN_IDLE_1,1);
            break;

        case EVENT_TARGET_NULL :
            //when target is not listening to I2C
            clear_leds_1();gpio_put(LED_PIN_IDLE_1,1);
            break;
        
        default :
            break;
    }
}

//TARGET 2 -- decreases by 1
void target_event_handler_2(volatile uint8_t &buffer,volatile i2c_target_memory &target_memory, const target_event event) 
{
    switch(event) {
        case EVENT_TARGET_START:
        //when START condition is met
            clear_leds_2();gpio_put(LED_PIN_START_2,1);
            break;

        case EVENT_TARGET_RECEIVED :
            clear_leds_2();
            gpio_put(LED_PIN_RECEIVED_2 , 1);

            target_memory.memory[target_memory.byte_count] = buffer - 1;
            //target_memory.byte_count ++;

            break;

        case EVENT_TARGET_TRANSMITTING:
            clear_leds_2();
            gpio_put(LED_PIN_TRANSMITTING_2,1);
            
            buffer = target_memory.memory[target_memory.byte_count];
            //target_memory.byte_count --;

            break;

        case EVENT_TARGET_STOP :
            //when STOP condition is met
            clear_leds_2();
            gpio_put(LED_PIN_IDLE_2,1);
            break;

        case EVENT_TARGET_NULL :
            //when target is not listening to I2C
            clear_leds_2();gpio_put(LED_PIN_IDLE_2,1);
            break;
        
        default :
            break;
    }
}

//TARGET 3 -- multiplies by 2
void target_event_handler_3(volatile uint8_t &buffer,volatile i2c_target_memory &target_memory, const target_event event) 
{
    switch(event) {
        case EVENT_TARGET_START:
        //when START condition is met
            clear_leds_3(); gpio_put(LED_PIN_START_3,1);
            break;

        case EVENT_TARGET_RECEIVED :
            clear_leds_3();
            gpio_put(LED_PIN_RECEIVED_3 , 1);

            target_memory.memory[target_memory.byte_count] = buffer * 2;
            //target_memory.byte_count ++;

            break;

        case EVENT_TARGET_TRANSMITTING:
            clear_leds_3();
            gpio_put(LED_PIN_TRANSMITTING_3,1);
            
            buffer = target_memory.memory[target_memory.byte_count];
            //target_memory.byte_count --;

            break;

        case EVENT_TARGET_STOP :
            //when STOP condition is met
            clear_leds_3();
            gpio_put(LED_PIN_IDLE_3,1);
            break;

        case EVENT_TARGET_NULL :
            //when target is not listening to I2C
            clear_leds_3();gpio_put(LED_PIN_IDLE_3,1);
            break;
        
        default :
            break;
    }
}

//TARGET 4 -- divides by 2
void target_event_handler_4(volatile uint8_t &buffer,volatile i2c_target_memory &target_memory, const target_event event) 
{
    switch(event) {
        case EVENT_TARGET_START:
        //when START condition is met
            clear_leds_4(); gpio_put(LED_PIN_START_4,1);
            break;

        case EVENT_TARGET_RECEIVED :
            clear_leds_4();
            gpio_put(LED_PIN_RECEIVED_4 , 1);

            target_memory.memory[target_memory.byte_count] = buffer / 2;
            //target_memory.byte_count ++;

            break;

        case EVENT_TARGET_TRANSMITTING:
            clear_leds_4();
            gpio_put(LED_PIN_TRANSMITTING_4,1);
            
            buffer = target_memory.memory[target_memory.byte_count];
            //target_memory.byte_count --;

            break;

        case EVENT_TARGET_STOP :
            //when STOP condition is met
            clear_leds_4();
            gpio_put(LED_PIN_IDLE_4,1);
            break;

        case EVENT_TARGET_NULL :
            //when target is not listening to I2C
            clear_leds_4();gpio_put(LED_PIN_IDLE_4,1);
            break;
        
        default :
            break;
    }
}

int main() {
    stdio_init_all();
   
    sleep_ms(100);
    init_leds();
    cycle_leds();

    i2c_target_init(TARGET_SDA_PIN,TARGET_SCL_PIN,TARGET_ADDRESS_1, target_event_handler_1);
    i2c_target_init(TARGET_SDA_PIN,TARGET_SCL_PIN,TARGET_ADDRESS_2, target_event_handler_2);
    i2c_target_init(TARGET_SDA_PIN,TARGET_SCL_PIN,TARGET_ADDRESS_3, target_event_handler_3);
    i2c_target_init(TARGET_SDA_PIN,TARGET_SCL_PIN,TARGET_ADDRESS_4, target_event_handler_4);
   
    while (true) {
        tight_loop_contents();
    }

    return 0;
}