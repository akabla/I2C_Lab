#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define MAX_NUMBER_OF_TARGETS 16

// Event types, used with the event_handler(...), after the event happened
enum target_event {
    EVENT_TARGET_START = 1,             // After controller has started communication on I2C bus
    EVENT_TARGET_RECEIVED,              /* After target has entered TARGET_STATE_RECEIVING_DATA or 
                                           has received 1 Byte*/
    EVENT_TARGET_TRANSMITTING,          // Before target starts to transmit 1 byte
    EVENT_TARGET_STOP,                  // After communication has stopped
    EVENT_TARGET_NULL                   /* After address frame has been sent, 
                                           if this target is not involved in this communication. */

};

// State machine for target
enum i2c_target_state_t {
    TARGET_STATE_IDLE = 1,              // Target waits for communication to start
    TARGET_STATE_ADDRESS_FRAME,         // Target is reading the ADRESS FRAME
    TARGET_STATE_TRANSMITTING_DATA,     // Target is transmitting data to Controller
    TARGET_STATE_RECEIVING_DATA,        // Target is receiving data from Controller
    TARGET_STATE_NULL,                  /* Communication is happening over I2C bus, 
                                           but this target is not part of it*/ 
};


// State machine for acknowledgement 
enum i2c_acknowledge_state_t {
    ACKNOWLEDGE_TARGET_TRANSMIT = 1,    //Target transmits ACK to Controller next SCL FALL
    ACKNOWLEDGE_TARGET_RECEIVE,         //Controller transmits ACK to Target next SCL FALL
    ACKNOWLEDGE_TARGET_NULL,            //No ACK bit next SCL RISE
};

// 8 bit memory buffer
struct i2c_target_buffer
{   
    volatile uint8_t data = 0;      // bites in the buffer
    volatile uint8_t bit_count = 0; // how many bites have been written to the buffer

    // Set the next bit in buffer and increment bit_count
    void set_bit(bool bit) { data = data | (bit << (7 - bit_count)); bit_count++; }

    // Get the next bit from buffer(starting from MSB), and increment bit_count
    bool get_bit(){ bool out = data & (0x01 << (7 - bit_count)); bit_count ++; return out; }

    // Reset buffer
    void reset_buffer() { data = 0; bit_count = 0; }
};

struct i2c_target_memory
{
    volatile uint8_t memory[16] = {};       // 16 byte memory
    volatile uint8_t byte_count = 0;        // a way to store the position of 1 byte
};

/// @brief a function type which specifies how to handle data in a out of the target device
/// @param buffer data in the target's buffer
/// @param memory memory is the internal memory of the target 
/// @param event the type of event triggering the event handler
typedef void (*i2c_target_event_handler)(volatile uint8_t &buffer,volatile i2c_target_memory &memory, const target_event event);


class i2c_target
{
    public:
        i2c_target(uint sda_pin, uint scl_pin, uint8_t target_address, i2c_target_event_handler event_handler)
        {
            sda = sda_pin;
            scl = scl_pin;

            target_address = target_address;
            // LSB is 0 for Controller writing to the Target
            controller_write_condition  = ((target_address << 1) & ~1); 
            
            // LSB is 1 for Controller reading from the Target
            controller_read_condition = ((target_address << 1) | 1); 

            _event_handler = event_handler;

            i2c_state = TARGET_STATE_IDLE;
            reset_values();
        }
        //
        void sda_trigger_handler(uint gpio, uint32_t event);
        void scl_trigger_handler(uint gpio, uint32_t event);

        void write_bit_to_sda(bool bit);

        /*inline*/ void reset_values();

        uint get_sda_pin() { return sda; }
        uint get_scl_pin() { return scl; }
    
    private:
        uint sda;
        uint scl;
        uint8_t target_address;
        i2c_target_event_handler _event_handler; 

        // Address + Read or write bit.
        uint8_t controller_write_condition;
        uint8_t controller_read_condition;

        // i2c input buffer
        i2c_target_buffer target_buffer;
        //8-byte internal memory of target 
        volatile i2c_target_memory target_memory;

        // i2c state machine
        volatile  i2c_target_state_t i2c_state;

        // acknowledgement state machine
        volatile i2c_acknowledge_state_t i2c_acknowledge_state;

};

static i2c_target* i2c_target_instances[MAX_NUMBER_OF_TARGETS];
static uint number_of_i2c_target_instances = 0;

void i2c_target_init(uint sda_pin, uint scl_pin, uint8_t target_address, i2c_target_event_handler event_handler);

// Trigger handler
void i2c_target_trigger_handler(uint gpio, uint32_t event);
