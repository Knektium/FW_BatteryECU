#include <DAVE.h>
#include "FREERTOS/FreeRTOS.h"
#include "Tasks/MessageManager.h"

#define VCC_MILLIVOLT 4950.0
#define MAX_CELL_MILLIVOLTAGE 1900
#define MIN_CELL_MILLIVOLTAGE 950


BaseType_t get_millivoltage_battery()
{
	XMC_VADC_RESULT_SIZE_t result;

	result = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_Channel_Voltage);

	return (VCC_MILLIVOLT / 4096.0) * result * 10;
}

BaseType_t get_current()
{
	XMC_VADC_RESULT_SIZE_t result;
	BaseType_t zero_level = 2500;
	BaseType_t mvolt_a = 0;

	result = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_Channel_Current);
	mvolt_a = (VCC_MILLIVOLT / 4096.0) * result;

	if (mvolt_a <= zero_level) {
		return 0;
	}

	return ((mvolt_a - zero_level) / 4) * 10; // 100 mV = 1000 mA
}

BaseType_t get_temperature_1()
{
	XMC_VADC_RESULT_SIZE_t result;

	result = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_Channel_Temperature1);

	return (VCC_MILLIVOLT / 4096.0) * result;
}

BaseType_t get_temperature_2()
{
	XMC_VADC_RESULT_SIZE_t result;

	result = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_Channel_Temperature2);

	return (VCC_MILLIVOLT / 4096.0) * result;
}

void Handle_BatteryControl_Received(BatteryControl_t msg, uint8_t from_node_id, uint8_t to_node_id)
{
	if (89U == msg.Off) {
		uint16_t chg = 0x00;
		BUS_IO_Write(&BUS_IO_ChargerControl, chg);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_PowerOn);
	}
}

static void Task_Main(void *pvParameters)
{
	BatteryStatus_t battery_status;
	BaseType_t mvolt, mampere, temp1, temp2;
	BaseType_t button_count = 0;
	int state = 0;

	uint16_t chg = 0xFF;
	BUS_IO_Write(&BUS_IO_ChargerControl, chg);

	while (1) {
		vTaskDelay(333 / portTICK_PERIOD_MS);

		if (0 == DIGITAL_IO_GetInput(&DIGITAL_IO_Button)) {
			button_count = button_count + 1;
		} else {
			button_count = 0;
		}

		if (5 == button_count) {
			if (0 == state) {
				state = 1;
				DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_PowerOn);
			} else if (1 == state) {
				state = 2;
				DIGITAL_IO_SetOutputLow(&DIGITAL_IO_PowerOn);
			}
		}

		mvolt = get_millivoltage_battery();
		if (mvolt < MIN_CELL_MILLIVOLTAGE * 10 || mvolt > MAX_CELL_MILLIVOLTAGE * 10) {
			DIGITAL_IO_SetOutputLow(&DIGITAL_IO_PowerOn);
		}
		battery_status.BatteryVoltage = (uint16_t) mvolt;

		mampere = get_current();
		if (mampere > 4000) {
			DIGITAL_IO_SetOutputLow(&DIGITAL_IO_PowerOn);
		}
		battery_status.Current = (uint16_t) mampere;

		temp1 = get_temperature_1();
		battery_status.TemperatureOne = (uint16_t) temp1;

		temp2 = get_temperature_2();
		battery_status.TemperatureTwo = (uint16_t) temp2;

		MessageManager_Send_BatteryStatus(&battery_status, 0x00);

		PWM_SetDutyCycle(&PWM_StatusRed, 10000);
		PWM_Start(&PWM_StatusRed);
		PWM_SetDutyCycle(&PWM_StatusBlue, 10000);
		PWM_Start(&PWM_StatusBlue);
	}
}

int main(void)
{
	DAVE_STATUS_t status;

	status = DAVE_Init();

	if (status != DAVE_STATUS_SUCCESS) {
		/* Placeholder for error handler code. The while loop below can be replaced with an user error handler. */
		XMC_DEBUG("DAVE APPs initialization failed\n");

		while (1U) {
		}
	}

	MessageManager_Init();
	xTaskCreate(Task_Main, "Task_Main", 40U, NULL, (tskIDLE_PRIORITY + 2), NULL);
	vTaskStartScheduler();

	/* Placeholder for user application code. The while loop below can be replaced with user application code. */
	while (1U) {
	}
}
