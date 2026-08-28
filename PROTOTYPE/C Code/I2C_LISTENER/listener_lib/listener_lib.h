#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Event types
enum listener_event {
    LISTENER_START = 1,     // a controller has opened communication
    LISTENER_LISTEN,        // the listener has read a non-ACK bit
    LISTENER_ACKNOWLEDGE,   // the listener has read the ACK bit bit
    LISTENER_STOP,          // communication has stopped
};

// State machine for i2c (for listener)
enum i2c_state_t {
    I2C_STATE_IDLE = 1,         // no communication is happening
    I2C_STATE_LISTEN,           // the listener is listening 
};

// State machine for acknowledgement 
enum acknowledge_state_t {
    ACKNOWLEDGE_CHECK = 1, // the listener will check for the ACK bit next clock SCL edge rise
    ACKNOWLEDGE_NULL       // the next bit is not an ACK bit
};

// only records the current byte being transmitted
struct listener_8bit_memory
{   

    volatile uint8_t data = 0;
    volatile uint8_t bit_count = 0;

    /*bits are being transmitted MSB first, 
    for example after 3 bits, the buffer will be :
    XXX0 0000*/
    void add_bit(bool bit) { data |= (bit << (7 - bit_count)); bit_count ++; }

    // Reset memory
    void reset_buffer() { data = 0 ; bit_count = 0; }
};

/// @brief a function type which specifies how to handle data the listener sees on the I2C bus
/// @param buffer the value of the buffer
/// @param bit_count the number of bits in the buffer
/// @param event the type of event triggering the event handler
typedef void (*i2c_listener_event_handler)(volatile uint8_t &buffer, const uint bit_count, const listener_event event);


class i2c_listener
{
    public:
        i2c_listener(uint sda_pin, uint scl_pin, i2c_listener_event_handler event_handler)
        {
            sda = sda_pin;
            scl = scl_pin;

            _event_handler = event_handler;

            i2c_state = I2C_STATE_IDLE;
            reset_values();
        }

        // Functions are declared but not used yet
        void sda_trigger_handler(uint gpio, uint32_t event);
        void scl_trigger_handler(uint gpio, uint32_t event);

        void reset_values();

        uint get_sda_pin() { return sda; }
        uint get_scl_pin() { return scl; }
    
    private:
        uint sda;
        uint scl;

        // The Event Handler Function
        i2c_listener_event_handler _event_handler; 

        // Listener Buffer (8 bits)
        listener_8bit_memory i2c_listener_buffer;
        volatile uint8_t acknowledge_bit;

        // i2c State Machine
        volatile  i2c_state_t i2c_state;

        // Acknowledgement State Machine
        volatile acknowledge_state_t listener_acknowledge_state;

};

// Initialisation Function Declaration
void i2c_listener_init(uint sda_pin, uint scl_pin, i2c_listener_event_handler event_handler);

// Trigger handler Function Declaration
void i2c_listener_trigger_handler(uint gpio, uint32_t event);
