#include <stdio.h>
#include "stm32f0xx_misc.h"
#include "stdlib.h"

#include "utlist.h"
#include "timer.h"

static volatile unsigned long ticks = 0;

struct DelayedCallbackInfoStruct {
	DelayedCallback callback;
	unsigned long volatile nextCallback;
	uint32_t millis;
	struct DelayedCallbackInfoStruct* next;
};

static struct DelayedCallbackInfoStruct* callbacks;

void Timer_Initialize() {
	ticks = 0;
	callbacks = NULL;

	SysTick_Config(SystemCoreClock / 8 / 1000);
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
}

DelayedCallbackInfo* Timer_Register(DelayedCallback callback, uint32_t millis) {
	return Timer_RegisterEx(callback, millis, millis);
}

DelayedCallbackInfo* Timer_RegisterEx(DelayedCallback callback, uint32_t millis,
		uint32_t first) {
	DelayedCallbackInfo* info = (DelayedCallbackInfo*) malloc(
			sizeof(DelayedCallbackInfo));

	info->callback = callback;
	info->nextCallback = ticks + first;
	info->millis = millis;
	info->next = NULL;

	LL_APPEND(callbacks, info);

	return info;
}

void Timer_Adjust(DelayedCallbackInfo* info, uint32_t millis)
{
	info->nextCallback = ticks + millis;
}

void Timer_Unregister(DelayedCallbackInfo* info) {
	LL_DELETE(callbacks, info);
	free(info);
}

unsigned long Timer_millis() {
	return ticks;
}

static void CheckCallbacks() {

	struct DelayedCallbackInfoStruct* callbackInfo;

	LL_FOREACH(callbacks, callbackInfo)
	{
		if (ticks > callbackInfo->nextCallback) {
			callbackInfo->nextCallback = ticks + callbackInfo->millis;
			callbackInfo->callback();
		}
	}

}

void SysTick_Handler() {
	ticks++;

	if (ticks % 10 == 0)
		CheckCallbacks();
}
