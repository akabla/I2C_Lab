#include "target_lib.h"


void i2c_target_init(uint sda_pin, uint scl_pin, uint8_t target_address, i2c_target_event_handler event_handler)
{
    // You cannot have more than 16 i2c target instances, limited by number of pins
    assert(number_of_i2c_target_instances < MAX_NUMBER_OF_TARGETS);

    // init i2c pins 
    gpio_init(sda_pin);
    gpio_init(scl_pin);

    gpio_set_dir(sda_pin, GPIO_IN);
    gpio_set_dir(scl_pin, GPIO_IN);

    gpio_set_slew_rate(sda_pin, GPIO_SLEW_RATE_FAST);
    gpio_set_slew_rate(scl_pin, GPIO_SLEW_RATE_FAST);

    // create new i2c_target_instance
    i2c_target_instances[number_of_i2c_target_instances] = new i2c_target(sda_pin, scl_pin, target_address, event_handler);
    number_of_i2c_target_instances++;

    // attach triggers to pins
    gpio_set_irq_callback(&i2c_target_trigger_handler);
    
    gpio_set_irq_enabled(sda_pin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(scl_pin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    irq_set_enabled(IO_IRQ_BANK0, true);
}

void i2c_target_trigger_handler(uint gpio, uint32_t event)
{
    // loop through all instances of i2c_target
    for(int i = 0; i < number_of_i2c_target_instances; i++)
    {
        if (gpio == i2c_target_instances[i]->get_sda_pin())
            i2c_target_instances[i]->sda_trigger_handler(gpio, event);
        else if (gpio == i2c_target_instances[i]->get_scl_pin())
            i2c_target_instances[i]->scl_trigger_handler(gpio, event);
    }
}

void i2c_target::reset_values() {
    target_buffer.reset_buffer();
    i2c_acknowledge_state = ACKNOWLEDGE_TARGET_NULL; 
    i2c_state = TARGET_STATE_IDLE;
}

void i2c_target::write_bit_to_sda(bool bit) {
    //if bit is 0, pull SDA line low
    if (bit == 0) {
        gpio_set_dir(sda,GPIO_OUT);
        gpio_put(sda, 0);
    }// otherwise set SDA line it to GPIO_IN to leave it floating
    else {
        gpio_set_dir(sda,GPIO_IN);
    }
}

void i2c_target::sda_trigger_handler(uint gpio, uint32_t event)
{
    bool clock_level = gpio_get(scl);
    // Start condition is a falling edge while scl is high
    if (event == GPIO_IRQ_EDGE_FALL && clock_level)
    {
        i2c_state = TARGET_STATE_ADDRESS_FRAME;
        target_buffer.reset_buffer();
        _event_handler(target_buffer.data,target_memory,EVENT_TARGET_START);
    }
    
    // Stop condition is a rising edge while scl is high
    else if (event == GPIO_IRQ_EDGE_RISE && clock_level)
    {
        i2c_state = TARGET_STATE_IDLE;
        target_buffer.reset_buffer();
        _event_handler(target_buffer.data,target_memory,EVENT_TARGET_STOP);
    }
}

void i2c_target::scl_trigger_handler(uint gpio, uint32_t event)
{
    /*to avoid any forbidden behaviour:
    after SCL RISE the target only reads the SDA and triggers event_handler after every 8 data bits
    after SCL FALL, the target configures its pins to IN/OUT for the next SCL RISE and/or writes a bit
    */
    if (event == GPIO_IRQ_EDGE_RISE)
    {
        switch(i2c_state)
        {   
            case TARGET_STATE_ADDRESS_FRAME: {
                // Read Bit
                target_buffer.set_bit(gpio_get(sda));
            
                // after 8 bits have been sent, check it matchess the read/write condition
                if (target_buffer.bit_count == 8)
                    /*if the address frame matches the controller write to target condition,
                      put target in receiving data state*/
                    if (target_buffer.data == controller_write_condition)
                    {
                        i2c_state = TARGET_STATE_RECEIVING_DATA;
                        i2c_acknowledge_state = ACKNOWLEDGE_TARGET_TRANSMIT;

                        //call the event handler(data sent is 0)
                        target_buffer.reset_buffer();
                        _event_handler(target_buffer.data,target_memory,EVENT_TARGET_RECEIVED);
                    }
                    /*if the address frame matches the controller read from target condition,
                      put target in transmitting data state*/
                    else if (target_buffer.data == controller_read_condition)
                    {
                        i2c_state = TARGET_STATE_TRANSMITTING_DATA;
                        i2c_acknowledge_state = ACKNOWLEDGE_TARGET_TRANSMIT;
                    
                        //call the event handler(data sent is 0)
                        target_buffer.reset_buffer();
                        _event_handler(target_buffer.data,target_memory,EVENT_TARGET_TRANSMITTING);
                    }
                    else
                    {
                        //this target is not being accessed in this transmission
                        
                        i2c_state = TARGET_STATE_NULL;
                        i2c_acknowledge_state = ACKNOWLEDGE_TARGET_NULL;

                        //call event handler(data sent is 0)
                        target_buffer.reset_buffer();
                        _event_handler(target_buffer.data,target_memory, EVENT_TARGET_NULL);
                    }
                break;
            }
        
            case TARGET_STATE_TRANSMITTING_DATA: {
                //check if this bit is an acknowledge bit
                if (i2c_acknowledge_state == ACKNOWLEDGE_TARGET_RECEIVE)
                {
                    bool acknowledged = !gpio_get(sda);
                    i2c_acknowledge_state = ACKNOWLEDGE_TARGET_NULL;
                    // if not acknowledged, set target to idle state and wait for controller to end communication
                    if (!acknowledged) i2c_state = TARGET_STATE_NULL;
                }
                //if this is the 8th bit, then next SCL rise will be an ACK bit
                else if (target_buffer.bit_count  == 8)
                {
                    target_buffer.bit_count = 0;
                    i2c_acknowledge_state = ACKNOWLEDGE_TARGET_RECEIVE;
                    //call the event handler to prepare next byte
                    _event_handler(target_buffer.data, target_memory, EVENT_TARGET_TRANSMITTING);
                }
                break;
            }

            case TARGET_STATE_RECEIVING_DATA: {
                // if this bit is ACK, reset ack_state
                if (i2c_acknowledge_state == ACKNOWLEDGE_TARGET_TRANSMIT) 
                    i2c_acknowledge_state = ACKNOWLEDGE_TARGET_NULL; 
                // if this bit is not ACK, read the bit
                else if (i2c_acknowledge_state == ACKNOWLEDGE_TARGET_NULL)
                {
                    target_buffer.set_bit(gpio_get(sda));
                    // after 8 bits have been received, switch to send ACK next CLK FALL
                    if (target_buffer.bit_count == 8)
                    {
                        // send the buffer to the event handler and reset buffer
                        _event_handler(target_buffer.data,target_memory,EVENT_TARGET_RECEIVED);
                        target_buffer.reset_buffer();
                        // transmit ACK bit next SCL FALL
                        i2c_acknowledge_state = ACKNOWLEDGE_TARGET_TRANSMIT;
                    }
                }
                break;
            }

            default:
                break;
        }
    }

    // SCL must be low to change SDA
    else if (event == GPIO_IRQ_EDGE_FALL)
    {
        switch (i2c_state)
        {
            case TARGET_STATE_TRANSMITTING_DATA: {
                // if the controller expects an ACK bit (only after the ADRESS FRAME)
                if (i2c_acknowledge_state == ACKNOWLEDGE_TARGET_TRANSMIT)
                {
                    write_bit_to_sda(0);
                    i2c_acknowledge_state = ACKNOWLEDGE_TARGET_NULL;
                }
                // If the controller sends an ACK bit, release the SDA line
                else if (i2c_acknowledge_state == ACKNOWLEDGE_TARGET_RECEIVE)
                {
                    write_bit_to_sda(1);
                }
                // If this is not an ACK bit, write the next bit to the SDA line
                else if (i2c_acknowledge_state == ACKNOWLEDGE_TARGET_NULL)
                {   
                    write_bit_to_sda(target_buffer.get_bit());
                }

            
                break;
            }

            case TARGET_STATE_RECEIVING_DATA: {
                // if the controller expects an ACK bit, pull SDA LOW
                if (i2c_acknowledge_state == ACKNOWLEDGE_TARGET_TRANSMIT) write_bit_to_sda(0);
                // if not, release SDA
                else if (i2c_acknowledge_state == ACKNOWLEDGE_TARGET_NULL) write_bit_to_sda(1);
                
                break;
            }

            default:
                break;
        }
    }
}

