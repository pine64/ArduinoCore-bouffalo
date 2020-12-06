// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2020 Marek Kraus <gamelaster@outlook.com>

#include <Arduino.h>
#include "Wire.h"
extern "C" {
    #include <bl602_glb.h>
    #include <bl_i2c.h>
    #include <bl602_i2c.h>
    #include <bl_irq.h>
}

#define PUT_UINT32_LE(n,b,i)                      \
{                                                 \
    (b)[(i)    ] = (uint8_t) ( (n)       );       \
    (b)[(i) + 1] = (uint8_t) ( (n) >>  8 );       \
    (b)[(i) + 2] = (uint8_t) ( (n) >> 16 );       \
    (b)[(i) + 3] = (uint8_t) ( (n) >> 24 );       \
}

TwoWire Wire = TwoWire();

void i2cInterrupt(void* ctx)
{
    TwoWire* self = (TwoWire*)ctx;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t tmpval;
    tmpval = BL_RD_REG(I2C_BASE, I2C_INT_STS); // TODO: Rework
    if (BL_IS_REG_BIT_SET(tmpval,I2C_RXF_INT)){
        //
    } else if (BL_IS_REG_BIT_SET(tmpval, I2C_END_INT)) {
        I2C_Disable((I2C_ID_Type)0);
        I2C_IntMask((I2C_ID_Type)0, I2C_INT_ALL, MASK);
        i2c_clear_status(0);
        vTaskNotifyGiveFromISR(rtosMainTask, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        return;
    } else if (BL_IS_REG_BIT_SET(tmpval, I2C_NAK_INT)) {
        uint32_t temp = BL_RD_REG(I2C_BASE, I2C_BUS_BUSY);
        self->transmissionStatus = 2; // TODO: Detect if it's on data or address sending
        return;
    } else if (BL_IS_REG_BIT_SET(tmpval, I2C_TXF_INT)) {
        //
    } else if (BL_IS_REG_BIT_SET(tmpval, I2C_ARB_INT)) {
        I2C_Disable((I2C_ID_Type)0);
        I2C_IntMask((I2C_ID_Type)0, I2C_INT_ALL, MASK);
        i2c_clear_status(0);
        // TODO: Handling
        return;
    } else if (BL_IS_REG_BIT_SET(tmpval,I2C_FER_INT)) {
        I2C_Disable((I2C_ID_Type)0);
        I2C_IntMask((I2C_ID_Type)0, I2C_INT_ALL, MASK);
        i2c_clear_status(0);
        // TODO: Handling
        return;
    } else {
        I2C_Disable((I2C_ID_Type)0);
        I2C_IntMask((I2C_ID_Type)0, I2C_INT_ALL, MASK);
        i2c_clear_status(0);
    }

    if (self->transmissionDirection == 0) {
        uint32_t temp = 0;
        uint32_t val = 0;

        if (self->bufferPosition < self->bufferLength) {
            uint8_t count = self->bufferLength - self->bufferPosition;
            if (count >= 4) {
                count = 4;
            }
            for (int i = 0; i < count; i++) {
                val = *(self->buffer + self->bufferPosition + i);
                temp += val << i * 8;
            }
            BL_WR_REG(I2C_BASE, I2C_FIFO_WDATA, temp);
            self->bufferPosition += count;
        } else if (self->bufferPosition == self->bufferLength) {
            I2C_IntMask((I2C_ID_Type)0, I2C_TX_FIFO_READY_INT, MASK);
            return;
        }
    } else {
        if (self->bufferPosition < self->bufferLength) {
            uint32_t temp = 0;
            int i = 0;
            int count;

            count = self->bufferLength - self->bufferPosition;
            temp = BL_RD_REG(I2C_BASE, I2C_FIFO_RDATA);
            if (count >= 4) {
                PUT_UINT32_LE(temp, self->buffer, self->bufferPosition);
                count = 4;
            } else if (count < 4) {
                for (i = 0; i < count; i++) {
                    self->buffer[self->bufferPosition + i] = (temp & 0xff);
                    temp = (temp >> 8);
                }
            } // TODO: Refactor
            self->bufferPosition += count;  
        } else {
            I2C_IntMask((I2C_ID_Type)0, I2C_RX_FIFO_READY_INT, MASK);
            return;
        }
    }

}

void TwoWire::configureI2C(uint8_t transmissionDirection)
{
    uint32_t tmpval;
    tmpval = BL_RD_REG(I2C_BASE, I2C_INT_STS);
    tmpval = BL_SET_REG_BIT(tmpval, I2C_CR_I2C_END_CLR);
    tmpval = BL_SET_REG_BIT(tmpval, I2C_CR_I2C_NAK_CLR);
    tmpval = BL_SET_REG_BIT(tmpval, I2C_CR_I2C_ARB_CLR);
    BL_WR_REG(I2C_BASE, I2C_INT_STS, tmpval);

    tmpval = BL_RD_REG(I2C_BASE, I2C_CONFIG);
    if (transmissionDirection == 0) { // Write
        tmpval = BL_CLR_REG_BIT(tmpval, I2C_CR_I2C_PKT_DIR);
    } else { // Read
        tmpval = BL_SET_REG_BIT(tmpval, I2C_CR_I2C_PKT_DIR);
    }

    tmpval = BL_SET_REG_BITS_VAL(tmpval, I2C_CR_I2C_SLV_ADDR, this->slaveAddress);
    // TODO: Subaddress
    tmpval = BL_CLR_REG_BIT(tmpval, I2C_CR_I2C_SUB_ADDR_EN);
    tmpval = BL_SET_REG_BITS_VAL(tmpval, I2C_CR_I2C_PKT_LEN, this->bufferLength != 0 ? this->bufferLength - 1 : 0);
    BL_WR_REG(I2C_BASE, I2C_CONFIG, tmpval);
    this->transmissionDirection = transmissionDirection;
}

bool TwoWire::begin(int16_t sda, int16_t scl, uint32_t frequency)
{
    uint8_t gpiopins[2];
    gpiopins[0] = scl != -1 ? scl : 4;
    gpiopins[1] = sda != -1 ? sda : 3;
    
    GLB_GPIO_Func_Init(GPIO_FUN_I2C, (GLB_GPIO_Type*)gpiopins, 2);

    this->setClock(frequency == 0 ? 100000 : frequency);

    I2C_Disable((I2C_ID_Type)0);
    bl_irq_enable(I2C_IRQn);
    bl_irq_register_with_ctx((I2C_ID_Type)I2C_IRQn, (void*)i2cInterrupt, this);
    return true;
}

void TwoWire::setClock(uint32_t frequency)
{
	I2C_ClockSet((I2C_ID_Type)0, frequency);
}

size_t TwoWire::write(uint8_t ch)
{
    if (this->bufferLength >= BUFFER_LENGTH){
        setWriteError();
        return 0;
    }
    this->buffer[this->bufferLength++] = ch;
    return 1;
}

void TwoWire::beginTransmission(uint16_t address)
{
    this->bufferPosition = 0;
    this->bufferLength = 0;
    this->slaveAddress = address;
    this->transmissionStatus = 0;
}

uint8_t TwoWire::endTransmission(bool stop)
{
    this->configureI2C(0);
    I2C_IntMask((I2C_ID_Type)0, I2C_TX_FIFO_READY_INT, UNMASK);
    I2C_IntMask((I2C_ID_Type)0, I2C_TRANS_END_INT, UNMASK);
    I2C_IntMask((I2C_ID_Type)0, I2C_FIFO_ERR_INT, UNMASK);
    I2C_IntMask((I2C_ID_Type)0, I2C_ARB_LOST_INT, UNMASK);
    I2C_IntMask((I2C_ID_Type)0, I2C_NACK_RECV_INT, UNMASK);
    I2C_Enable((I2C_ID_Type)0);

    ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(50));
    return this->transmissionStatus;
}

uint8_t TwoWire::requestFrom(uint16_t address, uint8_t quantity)
{
    this->bufferPosition = 0;
    this->bufferLength = quantity;
    this->slaveAddress = address;
    this->transmissionStatus = 0;

    this->configureI2C(1);

    I2C_IntMask((I2C_ID_Type)0, I2C_RX_FIFO_READY_INT, UNMASK); 
    I2C_IntMask((I2C_ID_Type)0, I2C_TRANS_END_INT, UNMASK);
    I2C_IntMask((I2C_ID_Type)0, I2C_FIFO_ERR_INT, UNMASK);
    I2C_IntMask((I2C_ID_Type)0, I2C_ARB_LOST_INT, UNMASK);
    I2C_IntMask((I2C_ID_Type)0, I2C_NACK_RECV_INT, UNMASK);

    I2C_Enable((I2C_ID_Type)0);
    ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(50));
    this->bufferLength = this->bufferPosition;
    this->bufferPosition = 0;
    return this->transmissionStatus == 0 ? this->bufferLength : 0;
}

size_t TwoWire::write(const uint8_t * buffer, size_t length)
{
    // TODO:
    return 0;
}

int TwoWire::available(void)
{
    return this->bufferLength - this->bufferPosition;
}

int TwoWire::read(void)
{
    return this->bufferPosition < this->bufferLength ? this->buffer[this->bufferPosition++] : -1;
}

int TwoWire::peek(void)
{
    return this->bufferPosition < this->bufferLength ? this->buffer[this->bufferPosition] : -1;
}

void TwoWire::flush(void)
{

}
