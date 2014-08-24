/*
 * timer.h
 *
 *  Created on: 22.08.2014
 *      Author: cem
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>


typedef void (*DelayedCallback)(void);
// Use this type for the callback function.

struct DelayedCallbackInfoStruct;
typedef struct DelayedCallbackInfoStruct DelayedCallbackInfo;

void Timer_Initialize();
DelayedCallbackInfo* Timer_Register(DelayedCallback callback, uint32_t millis);
void Timer_Unregister(DelayedCallbackInfo* info);
unsigned long Timer_millis();

#endif /* TIMER_H_ */
