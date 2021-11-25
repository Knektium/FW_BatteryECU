/*
 * main.c
 *
 *  Created on: 2021 Oct 06 22:49:09
 *  Author: Jack
 */

#include <DAVE.h>                 //Declarations from DAVE Code Generation (includes SFR declaration)
#include "FREERTOS/task.h"

#define VCC_MILLIVOLT 4738.0
#define MAX_CELL_MILLIVOLTAGE 4600
#define MIN_CELL_MILLIVOLTAGE 2100

BaseType_t get_millivoltage_battery_one()
{
	XMC_VADC_RESULT_SIZE_t result;

	result = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_Channel_Voltage1);

	return (VCC_MILLIVOLT / 4096.0) * result;
}

BaseType_t get_millivoltage_battery_two()
{
	XMC_VADC_RESULT_SIZE_t result;

	result = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_Channel_Voltage2);

	return (VCC_MILLIVOLT / 4096.0) * result;
}

BaseType_t get_millivoltage_battery_three()
{
	XMC_VADC_RESULT_SIZE_t result;

	result = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_Channel_Voltage3);

	return (VCC_MILLIVOLT / 4096.0) * result;
}

BaseType_t get_current()
{
	XMC_VADC_RESULT_SIZE_t result;
	BaseType_t mvolt_a = 0;

	result = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_Channel_Current);
	mvolt_a = (VCC_MILLIVOLT / 4096.0) * result;

	return (mvolt_a - 2500) * 10; // 100 mV = 1000 mA
}

static void Task_Main(void *pvParameters)
{
	BaseType_t mvolt, mampere;

	DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_PowerOn);
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_StatusLED);

	while (1) {
		vTaskDelay(1000 / portTICK_PERIOD_MS);

		mvolt = get_millivoltage_battery_one();
		if (mvolt < MIN_CELL_MILLIVOLTAGE * 3 || mvolt > MAX_CELL_MILLIVOLTAGE * 3) {
			DIGITAL_IO_SetOutputLow(&DIGITAL_IO_PowerOn);
		}

		mvolt = get_millivoltage_battery_two();
		if (mvolt < MIN_CELL_MILLIVOLTAGE * 2 || mvolt > MAX_CELL_MILLIVOLTAGE * 2) {
			DIGITAL_IO_SetOutputLow(&DIGITAL_IO_PowerOn);
		}

		mvolt = get_millivoltage_battery_three();
		if (mvolt < MIN_CELL_MILLIVOLTAGE || mvolt > MAX_CELL_MILLIVOLTAGE) {
			DIGITAL_IO_SetOutputLow(&DIGITAL_IO_PowerOn);
		}

		mampere = get_current();
		if (mampere > 1500) {
			DIGITAL_IO_SetOutputLow(&DIGITAL_IO_PowerOn);
		}
	}
}

/**

 * @brief main() - Application entry point
 *
 * <b>Details of function</b><br>
 * This routine is the application entry point. It is invoked by the device startup code. It is responsible for
 * invoking the APP initialization dispatcher routine - DAVE_Init() and hosting the place-holder for user application
 * code.
 */

int main(void)
{
  DAVE_STATUS_t status;

  status = DAVE_Init();           /* Initialization of DAVE APPs  */

  if(status != DAVE_STATUS_SUCCESS)
  {
    /* Placeholder for error handler code. The while loop below can be replaced with an user error handler. */
    XMC_DEBUG("DAVE APPs initialization failed\n");

    while(1U)
    {

    }
  }
  xTaskCreate(Task_Main, "Task_Main", 40U, NULL, (tskIDLE_PRIORITY + 2), NULL);
  vTaskStartScheduler();
  /* Placeholder for user application code. The while loop below can be replaced with user application code. */
  while(1U)
  {

  }
}
