#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#include "listener_lib.h"

// ----------------------------------------------------------------------------
// Hardware Pin Definitions
// ----------------------------------------------------------------------------

//Listener Input Pins
#define LISTENER_SDA_PIN 10
#define LISTENER_SCL_PIN 11  

//Listener LED Pins
#define LED_PIN_7 7
#define LED_PIN_6 6
#define LED_PIN_5 5
#define LED_PIN_4 4
#define LED_PIN_3 3
#define LED_PIN_2 2
#define LED_PIN_1 1
#define LED_PIN_0 0
constexpr uint8_t LED_PINS[] = {LED_PIN_7, LED_PIN_6, LED_PIN_5, LED_PIN_4, LED_PIN_3, LED_PIN_2, LED_PIN_1, LED_PIN_0};

//Listener Acknowledge PIN
#define LED_PIN_ACKNOWLEDGE 8

//for LEDS
#define ON  1
#define OFF 0

// ----------------------------------------------------------------------------
// LED Helper Functions
// ----------------------------------------------------------------------------

void init_leds() {
    
    //set memory bits with PWM
    for(uint8_t i = 0 ; i <= 7 ; i++) {
        gpio_set_function(LED_PINS[i] , GPIO_FUNC_PWM);
        pwm_set_wrap(pwm_gpio_to_slice_num(LED_PINS[i]), 255);
        pwm_set_enabled(pwm_gpio_to_slice_num(LED_PINS[i]), true);
    }

    //initialise acknowledge LED
    gpio_init(LED_PIN_ACKNOWLEDGE);
    gpio_set_dir(LED_PIN_ACKNOWLEDGE, true);
    gpio_put(LED_PIN_ACKNOWLEDGE,OFF);
}

void set_leds_byte(const uint8_t value, const uint8_t bit_count){
    for (uint8_t i = 0 ; i <= 7; i++)
        if (bit_count >= 8 - i)
            if (value & (0x01 << i))
                pwm_set_gpio_level(LED_PINS[i],255);
            else 
                pwm_set_gpio_level(LED_PINS[i],0);
        else
            pwm_set_gpio_level(LED_PINS[i],32);
}

void flash_leds(uint32_t total_time_ms){
    //used during Pico start-up
    gpio_put(LED_PIN_ACKNOWLEDGE,ON);

    for(int i=1;i<=128;i*=2) {
        set_leds_byte(i,8);
        sleep_us(total_time_ms * 100); 
    }

    set_leds_byte(0,8);
    gpio_put(LED_PIN_ACKNOWLEDGE,OFF);
}

// ----------------------------------------------------------------------------
// Listener Event Handler
// ----------------------------------------------------------------------------

void listener_event_handler(volatile uint8_t &buffer, const uint bit_count, const listener_event event) 
{
    switch (event) {
        case LISTENER_START :
            //dim all buffer LEDs and turn off ACK LED
            set_leds_byte(0,0); gpio_put(LED_PIN_ACKNOWLEDGE,OFF);
            break;
        
        case LISTENER_LISTEN :
            //set to HIGH/LOW all LEDs which have been written to and turn off acknowledge LED
            set_leds_byte(buffer,bit_count);
            gpio_put(LED_PIN_ACKNOWLEDGE,OFF);
            break;

        case LISTENER_ACKNOWLEDGE:
            //turn on the ACKNOWLEDGE LED if it has been transmitted
            gpio_put(LED_PIN_ACKNOWLEDGE,buffer);
            break;
        
        case LISTENER_STOP :
            //turn off all LEDS
            set_leds_byte(0,8); gpio_put(LED_PIN_ACKNOWLEDGE,OFF);
            break;
    }
}

int main() {
    stdio_init_all();
   
    sleep_ms(500);

    // Start-up LED sequence
    init_leds();
    flash_leds(250);flash_leds(250);
    
    // Initialise the listener instance
    i2c_listener_init(LISTENER_SDA_PIN, LISTENER_SCL_PIN, listener_event_handler);

    while (true) {
        tight_loop_contents();
    }
}