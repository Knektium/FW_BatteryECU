#ifndef CAN_ROUTER_H
#define CAN_ROUTER_H

#include <stdint.h>

typedef struct {
	uint32_t id;
	uint8_t data[8];
	uint32_t crc;
} Message_t;

/* Message struct typedefs */
typedef struct BatteryControl_s {
	uint8_t On;
} BatteryControl_t;

typedef struct BatteryStatus_s {
	uint16_t Current;
} BatteryStatus_t;

void CAN_HandleReceivedMessage(Message_t message);
extern void Handle_BatteryControl_Received(BatteryControl_t msg, uint8_t from_node_id, uint8_t to_node_id);
void Send_BatteryStatus(BatteryStatus_t *s, uint32_t to_node);


#endif /* End of CAN_ROUTER_H */
