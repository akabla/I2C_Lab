#include "listener_lib.h"

// listener instance
static i2c_listener* listener_instance = nullptr;


//  Initialisation function

void i2c_listener_init(uint sda_pin, uint scl_pin, i2c_listener_event_handler event_handler) 
{
    // Clean up if re-initialized
    if (listener_instance != nullptr) {
        delete listener_instance;
    }

    // Create the single listener object
    listener_instance = new i2c_listener(sda_pin, scl_pin, event_handler);

    // Configure SDA pin
    gpio_init(sda_pin);
    gpio_set_dir(sda_pin, GPIO_IN);
    gpio_pull_up(sda_pin);

    // Configure SCL pin
    gpio_init(scl_pin);
    gpio_set_dir(scl_pin, GPIO_IN);
    gpio_pull_up(scl_pin);

    // Enable IRQs on both pins and register the global C callback
    gpio_set_irq_enabled_with_callback(
        sda_pin, 
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, 
        true, 
        &i2c_listener_trigger_handler
    );
    
    gpio_set_irq_enabled(
        scl_pin, 
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, 
        true
    );
}

// Callback function for a Hardware Interrupt

void i2c_listener_trigger_handler(uint gpio, uint32_t event) 
{
    // Ensure the instance exists before routing events
    if (listener_instance == nullptr) return;

    // Route the interrupt to the appropriate member function
    if (gpio == listener_instance->get_sda_pin()) {
        listener_instance->sda_trigger_handler(gpio, event);
    } 
    else if (gpio == listener_instance->get_scl_pin()) {
        listener_instance->scl_trigger_handler(gpio, event);
    }
}

//  Member functions of i2c_listener

void i2c_listener::reset_values() 
{
    i2c_state = I2C_STATE_IDLE;
    listener_acknowledge_state = ACKNOWLEDGE_NULL;
    i2c_listener_buffer.reset_buffer();
}

void i2c_listener::sda_trigger_handler(uint gpio, uint32_t event) 
{
    // an SDA RISE / FALL only changes the state of the I2C bus when SCL is high
    bool scl_val = gpio_get(scl);

    // START condition: SDA FALL while SCL is HIGH
    if ((event == GPIO_IRQ_EDGE_FALL) && scl_val) {
        reset_values();
        i2c_state = I2C_STATE_LISTEN;
        
        //notify the event handler communication has started(no data has been transmitted)
        _event_handler(i2c_listener_buffer.data , 0 , LISTENER_START);
        
    }
    // STOP condition: SDA RISE while SCL is HIGH
    else if ((event == GPIO_IRQ_EDGE_RISE) && scl_val) {
        i2c_state = I2C_STATE_IDLE;
        reset_values();

        //notify the event handler communication has stopped 
        _event_handler(i2c_listener_buffer.data, 0 , LISTENER_STOP);
    }
}

void i2c_listener::scl_trigger_handler(uint gpio, uint32_t event) 
{
    // only listen if communication has been previously started
    if (i2c_state != I2C_STATE_LISTEN) return;

    //read bit after after SCL RISE
    if (event == GPIO_IRQ_EDGE_RISE)
        switch (listener_acknowledge_state) {
            
            //this bit is an acknowledge bit
            case ACKNOWLEDGE_CHECK : {
                //read SDA, low represents an ACK bit, high a NACK bit
                acknowledge_bit = !gpio_get(sda); 
            
                //notify the event handler
                _event_handler(acknowledge_bit, 1 , LISTENER_ACKNOWLEDGE);

                //reset memory and ACK state for next byte
                i2c_listener_buffer.reset_buffer();
                listener_acknowledge_state = ACKNOWLEDGE_NULL;
                break;
            }
        
            case ACKNOWLEDGE_NULL : {
                //read SDA
                i2c_listener_buffer.add_bit(gpio_get(sda));

                //notify the event handler
                _event_handler(i2c_listener_buffer.data, i2c_listener_buffer.bit_count, LISTENER_LISTEN);
            
                //if 8 bits have been transmitted, the next one is an ACK bit
                if (i2c_listener_buffer.bit_count >= 8)
                    listener_acknowledge_state = ACKNOWLEDGE_CHECK;
            
                break;
            }
    }
    //the listener does not change SDA/SCL state, so nothing happens after SCL FALL
}