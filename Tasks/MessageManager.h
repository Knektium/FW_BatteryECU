/*
 * This file is autogenerated by Meddela, any modification done to this file
 * will be lost when the code is regenerated.
 */
#ifndef TASKS_MESSAGEMANAGER_H_
#define TASKS_MESSAGEMANAGER_H_

#include <stdint.h>
#include "FREERTOS/FreeRTOS.h"

extern TaskHandle_t xMessageManagerHandle;

typedef struct {
	uint32_t id;
	uint8_t data[8];
	uint32_t crc;
} Message_t;

/* Message struct typedefs */
typedef struct BatteryControl_s {
	uint8_t Off;
} BatteryControl_t;

typedef struct BatteryStatus_s {
	uint16_t Current;
	uint16_t BatteryVoltage;
	uint16_t TemperatureOne;
	uint16_t TemperatureTwo;
} BatteryStatus_t;

extern void MessageManager_Send_BatteryStatus(BatteryStatus_t *s, uint32_t to_node);
extern void Handle_BatteryControl_Received(BatteryControl_t msg, uint8_t from_node_id, uint8_t to_node_id);
extern void MessageManager_PushReceivedMessage(Message_t *message);
extern void MessageManager_HandleReceivedMessage(Message_t message);
extern void MessageManager_Init(void);
extern void MessageManager_Main(void *pvParameters);

#endif /* End of TASKS_MESSAGEMANAGER_H */
