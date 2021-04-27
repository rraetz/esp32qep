/* ESP32QEP
 *
 * AUTHOR: Raphael Rätz
 * EMAIL: raphael.raetz@hotmail.com
 */


// Includes
#include "esp32qep.h"


// Global (static) variable
static QEP qep;


// Initialization function, enable interrupts from core 1
bool QEP_initialize(uint8_t pinA, uint8_t pinB)
{
    qep.pinA = pinA;
    qep.pinB = pinB;
    qep.counter = 0;
    qep.queue = xQueueCreate(1, sizeof(int32_t));
    if (qep.queue == NULL) {return false;}
    else 
    {
        pinMode(qep.pinA, INPUT_PULLUP);
        pinMode(qep.pinB, INPUT_PULLUP);
        xTaskCreatePinnedToCore(QEP_setupInterrupts, "qepTask", 1024, NULL, 0, NULL, 1); 
        return true;
    }
}


// Enable the interrupts
void QEP_setupInterrupts(void* parameter) 
{
    attachInterrupt(qep.pinA, QEP_interruptA, CHANGE);
    attachInterrupt(qep.pinB, QEP_interruptB, CHANGE);
    vTaskDelete(NULL);
}


// Interrupt after change on pin A, on IRAM for speed
void IRAM_ATTR QEP_interruptA() 
{
    int pinA = (GPIO.in >> qep.pinA) & 0x1;  
    int pinB = (GPIO.in >> qep.pinB) & 0x1;  
    if (pinA == pinB) {qep.counter++;}        
    else {qep.counter--;}
    xQueueOverwriteFromISR(qep.queue, (int32_t*)&qep.counter, pdFALSE);
}


// Interrupt after change on pin B, on IRAM for speed
void IRAM_ATTR QEP_interruptB() 
{
    int pinA = (GPIO.in >> qep.pinA) & 0x1;  
    int pinB = (GPIO.in >> qep.pinB) & 0x1;  
    if (pinA == pinB) {qep.counter--;}        
    else {qep.counter++;}
    xQueueOverwriteFromISR(qep.queue, (int32_t*)&qep.counter, pdFALSE);
}


// Get the encoder tick count via queue, thread/interrupt safe
int32_t QEP_getCounter()
{
    static int32_t counter = 0;
    if (uxQueueMessagesWaiting(qep.queue) > 0)
    {
        xQueueReceive(qep.queue, &counter, portMAX_DELAY);
    }
    return counter;
}


// Run counter reset task on core 1
void QEP_resetCounter()
{
    xTaskCreatePinnedToCore(QEP_resetCounterTask, "qepTask", 1024, NULL, 0, NULL, 1); 
}


// Disable interrups, reset counter and re-enable interrupts
void QEP_resetCounterTask(void* parameter) 
{
    detachInterrupt(qep.pinA);
    detachInterrupt(qep.pinA);
    qep.counter = 0;
    attachInterrupt(qep.pinA, QEP_interruptA, CHANGE);
    attachInterrupt(qep.pinB, QEP_interruptB, CHANGE);
    vTaskDelete(NULL);
}