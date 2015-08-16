#include "CoOS.h"
#include "naza.h"
#include "modules.h"
#include "hardware.h"
#include "simpletelemtry.h"


int main() {
 	CoInitOS();

	hardware_led_init();
	hardware_led_toggle_red();
	hardware_led_toggle_green();

	hardware_i2c_stretch();

	//initialize
	simpleTelemetry_initialize();
	naza_initialize();
	modules_initialize();

	//start tasks
	naza_start();
	modules_start();


	CoStartOS();

	while(1){

	}
}

