#include <DAVE.h>
#include "CAN_Config.h"
#include "CAN_Router.h"

void CAN_HandleReceivedMessage(Message_t message)
{
	uint8_t from_node_id = (message.id >> 4U) & 0xFFU;
	uint8_t to_node_id = (message.id >> 12U) & 0xFFU;

	if ((message.id & CAN_MESSAGE_BATTERYCONTROL_MASK) == CAN_MESSAGE_BATTERYCONTROL_MASK) {
		BatteryControl_t s;

		// Parse BatteryControl to struct

		s.On = 0U;
		s.On += ((message.data[0U]) & 1U);

		Handle_BatteryControl_Received(s, from_node_id, to_node_id);
	}
}

void Send_BatteryStatus(BatteryStatus_t *s, uint32_t to_node)
{
	uint8_t data[8] = { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };

	data[0U] |= ((uint8_t) (s->Current >> 0U) << 0U) & 255U;
	data[1U] |= ((uint8_t) (s->Current >> 8U) << 0U) & 255U;
	
	CAN_NODE_MO_UpdateData(CAN_NODE_0.lmobj_ptr[CAN_MESSAGE_BATTERYSTATUS_INDEX], (uint8_t *) data);
	CAN_NODE_MO_Transmit(CAN_NODE_0.lmobj_ptr[CAN_MESSAGE_BATTERYSTATUS_INDEX]);
}

