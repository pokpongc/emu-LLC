#include "mbed.h"
#include "platform/mbed_thread.h"
#include "emulatorPin.h"
#include "emulatorLib.h"
#include "eeprom.h"
#include <Eigen/Dense.h>
#include <math.h>

#define PI 3.14159265359

using namespace Eigen;


AnalogIn temp_adc(ADC_TEMP);
EEPROM memory(I2C4_SDA, I2C4_SCL, 0, 16384);
RawSerial uart4(UART4_TX, UART4_RX, 1000000);
BusOut onBoardLed(LD1, LD2, LD3, LD4);

RawSerial enc1(UART5_TX, UART5_RX, 115200);
DigitalOut fch1(FLOW_CH1);
RawSerial enc2(UART2_TX, UART2_RX, 115200);
DigitalOut fch2(FLOW_CH2);

DigitalOut output_enable_1(OE1);
DigitalOut output_enable_2(OE2);
DigitalOut output_enable_3(OE3);
// PwmOut q6_step(Y14);
// DigitalOut q6_dir(Y18);
// DigitalOut q6_en(Y21);
Actuator q6(Y14, Y18, Y21);

DigitalOut pilot(OD2);

Thread idleThread(osPriorityLow);
Ticker enct;

uint16_t raw_position = 0;
double position;
double velocity = 0;
double cov = 1;
double zp;
double ye;
float sigma_a = 0.1;
float sigma_w = 0.1;
float Q = pow(sigma_a, 2);
float R = pow(sigma_w, 2);
float dt = 0.001;
MatrixXd pp(2,2);
MatrixXd i2(2,2);
VectorXd xhat(2);
VectorXd xhat_new(2);
VectorXd K(2);
MatrixXd F(2,2);
VectorXd G(2);
RowVectorXd H(2);

float prev_pos = 0.0;

void idleRoutine(){
    for (;;){
        onBoardLed = (onBoardLed | 1) & 1;
        pilot = 1;
        ThisThread::sleep_for(500);
        onBoardLed = (onBoardLed | 2) & 2;
        pilot = 0;
        ThisThread::sleep_for(500);
    }
}

void uart4Callback(){
    uart4.putc(uart4.getc());
}

void initializeMsg(){
    output_enable_1 = 0;
    output_enable_2 = 0;
    output_enable_3 = 0;
    ThisThread::sleep_for(10);
    uart4.printf("InternalTemp: %.2f C\n", temp_adc.read()*100);
    uart4.printf("SystemCoreClock: %ld Hz\n", SystemCoreClock);
}

void kal_test(){
    raw_position = readAMT(enc2, 0x3C, fch2);
    // position = (raw_position/16383.0)*2*PI;
    // velocity = (position-prev_pos)/dt;
    // xhat_new = F*xhat;
    // zp = H*xhat;
    // ye = velocity - zp;
    // pp = F*pp*F.transpose()+G*G.transpose()*Q;
    // K = pp*H.transpose()*(1/(H*pp*H.transpose()+R));
    // xhat_new += K*(ye);
    // pp = (i2-K*H)*pp;
    // xhat = xhat_new;
    // prev_pos = position;
}

int main(){
    initializeMsg();

    pp << cov,0, 0,cov;
    i2 << 1,0, 0,1;
    xhat << 0,0;
    xhat_new << 0,0;
    K << 0,0;
    F << 1,0, dt,1;
    G << pow(dt,2)/2,dt;
    H << 0,1;

    idleThread.start(idleRoutine);
    // enct.attach(&kal_test, dt);

    uart4.attach(&uart4Callback);

    // q6_en = 0;
    // q6_dir = 1;
    // q6_step.write(0.50f);
    // q6_step.period(1/200.0f);

    q6.enable();
    q6.unHold();
    q6.setFrequency(200.0f);

    output_enable_1 = 1;
    output_enable_2 = 1;
    output_enable_3 = 1;
    while(1){
        
        // position = (raw_position/16383.0)*2*PI;
        // velocity = (position-prev_pos)/dt;
        // uart4.printf("%.2f,%.4f\n",position, velocity);
        ThisThread::sleep_for(1000);
    }
}
