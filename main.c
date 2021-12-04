#include <DAVE.h>
#include "FREERTOS/FreeRTOS.h"
#include "Tasks/MessageManager.h"

#define VCC_MILLIVOLT 4738.0
#define MAX_CELL_MILLIVOLTAGE 4600
#define MIN_CELL_MILLIVOLTAGE 2100


BaseType_t get_millivoltage_battery_one()
{
	XMC_VADC_RESULT_SIZE_t result;

	result = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_Channel_Voltage1);

	return (VCC_MILLIVOLT / 4096.0) * result * 10;
}

BaseType_t get_millivoltage_battery_two()
{
	XMC_VADC_RESULT_SIZE_t result;

	result = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_Channel_Voltage2);

	return (VCC_MILLIVOLT / 4096.0) * result * 10;
}

BaseType_t get_millivoltage_battery_three()
{
	XMC_VADC_RESULT_SIZE_t result;

	result = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_Channel_Voltage3);

	return (VCC_MILLIVOLT / 4096.0) * result * 10;
}

BaseType_t get_current()
{
	XMC_VADC_RESULT_SIZE_t result;
	BaseType_t zero_level = 2200;
	BaseType_t mvolt_a = 0;

	result = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_Channel_Current);
	mvolt_a = (VCC_MILLIVOLT / 4096.0) * result;

	if (mvolt_a <= zero_level) {
		return 0;
	}

	return (mvolt_a - zero_level) * 10; // 100 mV = 1000 mA
}

void Handle_BatteryControl_Received(BatteryControl_t msg, uint8_t from_node_id, uint8_t to_node_id)
{
	if (89U == msg.Off) {
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_PowerOn);
	}
}

static void Task_Main(void *pvParameters)
{
	BatteryStatus_t battery_status;
	BaseType_t mvolt, mampere;

	DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_PowerOn);
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_StatusLED);

	while (1) {
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_StatusLED);

		mvolt = get_millivoltage_battery_one();
		if (mvolt < MIN_CELL_MILLIVOLTAGE * 3 || mvolt > MAX_CELL_MILLIVOLTAGE * 3) {
			DIGITAL_IO_SetOutputLow(&DIGITAL_IO_PowerOn);
		}
		battery_status.BatteryOneVoltage = (uint16_t) mvolt;

		mvolt = get_millivoltage_battery_two();
		if (mvolt < MIN_CELL_MILLIVOLTAGE * 2 || mvolt > MAX_CELL_MILLIVOLTAGE * 2) {
			DIGITAL_IO_SetOutputLow(&DIGITAL_IO_PowerOn);
		}
		battery_status.BatteryTwoVoltage = (uint16_t) mvolt;

		mvolt = get_millivoltage_battery_three();
		if (mvolt < MIN_CELL_MILLIVOLTAGE || mvolt > MAX_CELL_MILLIVOLTAGE) {
			DIGITAL_IO_SetOutputLow(&DIGITAL_IO_PowerOn);
		}
		battery_status.BatteryThreeVoltage = (uint16_t) mvolt;

		mampere = get_current();
		if (mampere > 4000) {
			DIGITAL_IO_SetOutputLow(&DIGITAL_IO_PowerOn);
		}
		battery_status.Current = (uint16_t) mampere;

		MessageManager_Send_BatteryStatus(&battery_status, 0x00);
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_StatusLED);
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
