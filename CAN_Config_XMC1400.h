#ifndef CAN_NODE_BATTERY_CONF_XMC1400_H
#define CAN_NODE_BATTERY_CONF_XMC1400_H

#include "Dave/Generated/CAN_NODE/can_node.h"

#define CAN_MESSAGE_CONFIGS { \
	(CAN_NODE_LMO_t *) &CAN_NODE_Battery_LMO_BatteryControl_Config, \
	(CAN_NODE_LMO_t *) &CAN_NODE_Battery_LMO_BatteryStatus_Config, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL, \
	NULL \
}

/* Message configs */
extern XMC_CAN_MO_t CAN_NODE_Battery_LMO_BatteryControl;
extern const CAN_NODE_LMO_t CAN_NODE_Battery_LMO_BatteryControl_Config;

extern XMC_CAN_MO_t CAN_NODE_Battery_LMO_BatteryStatus;
extern const CAN_NODE_LMO_t CAN_NODE_Battery_LMO_BatteryStatus_Config;


#endif /* End of CAN_NODE_BATTERY_CONF_XMC1400_H */
