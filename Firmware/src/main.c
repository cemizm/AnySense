#include "CoOS.h"
#include "naza.h"
#include "modules.h"
#include "hardware.h"
#include "simpletelemtry.h"


int main() {
 	CoInitOS();

	DEBUG_INIT();

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

