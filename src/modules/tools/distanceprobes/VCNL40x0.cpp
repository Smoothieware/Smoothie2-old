/*
Copyright (c) 2012 Vishay GmbH, www.vishay.com
author: DS, version 1.21

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "VCNL40x0.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VCNL40x0::VCNL40x0(PinName sda, PinName scl, unsigned char addr) : _i2c(sda, scl), _addr(addr) {
    _i2c.frequency(1000000);                                // set I2C frequency to 1MHz
}

VCNL40x0::~VCNL40x0() {
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void VCNL40x0::Init_VCNL40x0(PinName sda, PinName scl, unsigned char addr) {
    _i2c.frequency(1000000);                                // set I2C frequency to 1MHz
}

VCNL40x0Error_e VCNL40x0::SetCommandRegister (unsigned char Command) {

    _send[0] = REGISTER_COMMAND;                            // VCNL40x0 Configuration reister
    _send[1] = Command;
    _i2c.write(VCNL40x0_ADDRESS,_send, 2);                  // Write 2 bytes on I2C

    return VCNL40x0_ERROR_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VCNL40x0Error_e VCNL40x0::ReadCommandRegister (unsigned char *Command) {

    _send[0] = REGISTER_COMMAND;                            // VCNL40x0 Configuration register
    _i2c.write(VCNL40x0_ADDRESS,_send, 1);                  // Write 1 byte on I2C
    _i2c.read(VCNL40x0_ADDRESS+1,_receive, 1);              // Read 1 byte on I2C

    *Command = (unsigned char)(_receive[0]);

    return VCNL40x0_ERROR_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VCNL40x0Error_e VCNL40x0::ReadID (unsigned char *ID) {

    _send[0] = REGISTER_ID;                                 // VCNL40x0 product ID revision register
    _i2c.write(VCNL40x0_ADDRESS, _send, 1);                 // Write 1 byte on I2C
    _i2c.read(VCNL40x0_ADDRESS+1, _receive, 1);             // Read 1 byte on I2C

    *ID = (unsigned char)(_receive[0]);

    return VCNL40x0_ERROR_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VCNL40x0Error_e VCNL40x0::SetCurrent (unsigned char Current) {

    _send[0] = REGISTER_PROX_CURRENT;                       // VCNL40x0 IR LED Current register
    _send[1] = Current;
    _i2c.write(VCNL40x0_ADDRESS,_send, 2);                  // Write 2 bytes on I2C

    return VCNL40x0_ERROR_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VCNL40x0Error_e VCNL40x0::ReadCurrent (unsigned char *Current) {

    _send[0] = REGISTER_PROX_CURRENT;                       // VCNL40x0 IR LED current register
    _i2c.write(VCNL40x0_ADDRESS,_send, 1);                  // Write 1 byte on I2C
    _i2c.read(VCNL40x0_ADDRESS+1,_receive, 1);              // Read 1 byte on I2C

    *Current = (unsigned char)(_receive[0]);

    return VCNL40x0_ERROR_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VCNL40x0Error_e VCNL40x0::SetProximityRate (unsigned char ProximityRate) {

    _send[0] = REGISTER_PROX_RATE;                          // VCNL40x0 Proximity rate register
    _send[1] = ProximityRate;
    _i2c.write(VCNL40x0_ADDRESS,_send, 2);                  // Write 2 bytes on I2C

    return VCNL40x0_ERROR_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VCNL40x0Error_e VCNL40x0::SetAmbiConfiguration (unsigned char AmbiConfiguration) {

    _send[0] = REGISTER_AMBI_PARAMETER;                     // VCNL40x0 Ambilight configuration
    _send[1] = AmbiConfiguration;
    _i2c.write(VCNL40x0_ADDRESS,_send, 2);                  // Write 2 bytes on I2C

    return VCNL40x0_ERROR_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VCNL40x0Error_e VCNL40x0::SetInterruptControl (unsigned char InterruptControl) {

    _send[0] = REGISTER_INTERRUPT_CONTROL;                  // VCNL40x0 Interrupt Control register
    _send[1] = InterruptControl;
    _i2c.write(VCNL40x0_ADDRESS,_send, 2);                  // Write 2 bytes on I2C

    return VCNL40x0_ERROR_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VCNL40x0Error_e VCNL40x0::ReadInterruptControl (unsigned char *InterruptControl) {

    _send[0] = REGISTER_INTERRUPT_CONTROL;                  // VCNL40x0 Interrupt Control register
    _i2c.write(VCNL40x0_ADDRESS,_send, 1);                  // Write 1 byte on I2C
    _i2c.read(VCNL40x0_ADDRESS+1,_receive, 1);              // Read 1 byte on I2C

    *InterruptControl = (unsigned char)(_receive[0]);

    return VCNL40x0_ERROR_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VCNL40x0Error_e VCNL40x0::SetInterruptStatus (unsigned char InterruptStatus) {

    _send[0] = REGISTER_INTERRUPT_STATUS;                   // VCNL40x0 Interrupt Status register
    _send[1] = InterruptStatus;
    _i2c.write(VCNL40x0_ADDRESS,_send, 2);                  // Write 2 bytes on I2C

    return VCNL40x0_ERROR_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VCNL40x0Error_e VCNL40x0::SetModulatorTimingAdjustment (unsigned char ModulatorTimingAdjustment) {

    _send[0] = REGISTER_PROX_TIMING;                        // VCNL40x0 Modulator Timing Adjustment register
    _send[1] = ModulatorTimingAdjustment;
    _i2c.write(VCNL40x0_ADDRESS,_send, 2);                  // Write 2 bytes on I2C

    return VCNL40x0_ERROR_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VCNL40x0Error_e VCNL40x0::ReadInterruptStatus (unsigned char *InterruptStatus) {

    _send[0] = REGISTER_INTERRUPT_STATUS;                   // VCNL40x0 Interrupt Status register
    _i2c.write(VCNL40x0_ADDRESS,_send, 1);                  // Write 1 byte on I2C
    _i2c.read(VCNL40x0_ADDRESS+1,_receive, 1);              // Read 1 byte on I2C

    *InterruptStatus = (unsigned char)(_receive[0]);

    return VCNL40x0_ERROR_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VCNL40x0Error_e VCNL40x0::ReadProxiValue (unsigned int *ProxiValue) {

    _send[0] = REGISTER_PROX_VALUE;                         // VCNL40x0 Proximity Value register
    _i2c.write(VCNL40x0_ADDRESS, _send, 1);                 // Write 1 byte on I2C
    _i2c.read(VCNL40x0_ADDRESS+1, _receive, 2);             // Read 2 bytes on I2C
    *ProxiValue = ((unsigned int)_receive[0] << 8 | (unsigned char)_receive[1]);

    return VCNL40x0_ERROR_OK;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VCNL40x0Error_e VCNL40x0::ReadAmbiValue (unsigned int *AmbiValue) {

    _send[0] = REGISTER_AMBI_VALUE;                          // VCNL40x0 Ambient Light Value register
    _i2c.write(VCNL40x0_ADDRESS, _send, 1);                  // Write 1 byte on I2C
    _i2c.read(VCNL40x0_ADDRESS+1, _receive, 2);              // Read 2 bytes on I2C
    *AmbiValue = ((unsigned int)_receive[0] << 8 | (unsigned char)_receive[1]);

    return VCNL40x0_ERROR_OK;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VCNL40x0Error_e VCNL40x0::SetLowThreshold (unsigned int LowThreshold) {

    unsigned char LoByte=0, HiByte=0;
    
    LoByte = (unsigned char)(LowThreshold & 0x00ff);
    HiByte = (unsigned char)((LowThreshold & 0xff00)>>8);
    
    _send[0] = REGISTER_INTERRUPT_LOW_THRES;                // VCNL40x0 Low Threshold Register, Hi Byte
    _send[1] = HiByte;
    _i2c.write(VCNL40x0_ADDRESS,_send, 2);                  // Write 2 bytes on I2C
    
    _send[0] = REGISTER_INTERRUPT_LOW_THRES+1;              // VCNL40x0 Low Threshold Register, Lo Byte
    _send[1] = LoByte;
    _i2c.write(VCNL40x0_ADDRESS,_send, 2);                  // Write 2 bytes on I2C

    return VCNL40x0_ERROR_OK;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VCNL40x0Error_e VCNL40x0::SetHighThreshold (unsigned int HighThreshold) {

    unsigned char LoByte=0, HiByte=0;
    
    LoByte = (unsigned char)(HighThreshold & 0x00ff);
    HiByte = (unsigned char)((HighThreshold & 0xff00)>>8);
    
    _send[0] = REGISTER_INTERRUPT_HIGH_THRES;               // VCNL40x0 High Threshold Register, Hi Byte
    _send[1] = HiByte;
    _i2c.write(VCNL40x0_ADDRESS,_send, 2);                  // Write 2 bytes on I2C
    
    _send[0] = REGISTER_INTERRUPT_HIGH_THRES+1;             // VCNL40x0 High Threshold Register, Lo Byte
    _send[1] = LoByte;
    _i2c.write(VCNL40x0_ADDRESS,_send, 2);                  // Write 2 bytes on I2C

    return VCNL40x0_ERROR_OK;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VCNL40x0Error_e VCNL40x0::ReadProxiOnDemand (unsigned int *ProxiValue) {

    unsigned char Command=0;

    // enable prox value on demand
    SetCommandRegister (COMMAND_PROX_ENABLE | COMMAND_PROX_ON_DEMAND);
 
    // wait on prox data ready bit
    do {
        ReadCommandRegister (&Command);                     // read command register
    } while (!(Command & COMMAND_MASK_PROX_DATA_READY));

    ReadProxiValue (ProxiValue);                            // read prox value

    SetCommandRegister (COMMAND_ALL_DISABLE);               // stop prox value on demand
    
    return VCNL40x0_ERROR_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VCNL40x0Error_e VCNL40x0::ReadAmbiOnDemand (unsigned int *AmbiValue) {

    unsigned char Command=0;

    // enable ambi value on demand
    SetCommandRegister (COMMAND_PROX_ENABLE | COMMAND_AMBI_ON_DEMAND);

    // wait on ambi data ready bit
    do {
        ReadCommandRegister (&Command);                     // read command register
    } while (!(Command & COMMAND_MASK_AMBI_DATA_READY));

    ReadAmbiValue (AmbiValue);                              // read ambi value    

    SetCommandRegister (COMMAND_ALL_DISABLE);               // stop ambi value on demand    

    return VCNL40x0_ERROR_OK;
}

