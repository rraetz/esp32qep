/* ESP32QEP
 *
 * DESCRIPTION: 
 * Quadrature pulse encoder module based on interrupts on 
 * core 1 of an ESP32. 
 * 
 * AUTHOR: Raphael Rätz
 * EMAIL: raphael.raetz@artorg.unibe.ch
 * 
 *
 *
 * COMMENTS:
 * Max pulse frequency = 500 kHz
 * xQueueOverwriteFromISR() --> 1.9 us  
 * Using the queue, QEP_interruptA() and QEP_interruptB() 
 * take approx 2 us. Therefore, the max interrupt frequency is 
 * approx. 500 kHz. With an encoder of 4000 (1000) pulses,
 * this equals to 7500 RPM
 */

#ifndef __ESP32QEP_H__
#define __ESP32QEP_H__


// Includes
#include <Arduino.h>
#include <stdint.h>
#include "freertos/queue.h"


// Typedef
struct QEP 
{
    uint8_t pinA;
    uint8_t pinB;
    volatile int32_t counter;
    QueueHandle_t queue;
};


// Function prototypes
bool QEP_initialize(uint8_t pinA, uint8_t pinB);
void IRAM_ATTR QEP_interruptA();
void IRAM_ATTR QEP_interruptB();
void QEP_setupInterrupts(void* parameter);
int32_t QEP_getCounter();
void QEP_resetCounter();
void QEP_resetCounterTask(void* parameter);

#endif // __ESP32QEP_H__