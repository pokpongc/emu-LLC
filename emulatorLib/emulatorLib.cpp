#include "emulatorLib.h"
#include "mbed.h"
#include <cstdint>

//AMT21 encoder functions
uint16_t readAMT(RawSerial &ser, uint8_t id, DigitalOut flow_pin){
    //max 3400 Hz
    flow_pin = 1;
    ser.putc(id);
    wait_us(75);
    flow_pin = 0;
    // overwatch.flags_set(0x01);
    uint8_t lb = ser.getc();
    uint8_t hb = ser.getc();
    uint8_t K0 = (hb & 0x40)>>6;
    uint8_t K1 = (hb & 0x80)>>7;
    uint16_t cal_data = lb |((hb&0x3F)<<8);
    uint8_t K0_cal = calK0(cal_data);
    uint8_t K1_cal = calK1(cal_data);
    if((K0 == K0_cal) && (K1 == K1_cal)) return cal_data;
    // else {
    //     overwatch.flags_set(0xF1);
    //     return 0;
    // }
}

unsigned char calK0(uint16_t data){
    unsigned char parity=0;
    while(data){
        parity^=(data &1);
        data>>=2;
    }
    return !parity;
}

unsigned char calK1(uint16_t data){
    unsigned char parity=0;
    data >>= 1;
    while(data){
        parity^=(data &1);
        data>>=2;
    }
    return !parity;
}

// class Actuator {
//     Private:
//         DigitalOut en_pin;
//         DigitalOut dir_pin;
//         PwmOut step_pin;
//     Public:
//         void set_frequency(float);
// };



// void Actuator::set_frequency(float frequency) {
//     step_pin.write(0.5f)
// }