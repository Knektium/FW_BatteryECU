/*
 * This file is autogenerated by Meddela, any modification done to this file
 * will be lost when the code is regenerated.
 */
#include <DAVE.h>
#include "Tasks/MessageManager.h"


XMC_CAN_MO_t CAN_NODE_Battery_LMO_BatteryControl = {
	.can_mo_type      = XMC_CAN_MO_TYPE_RECMSGOBJ,
	.can_identifier   = 0x50000aU,
	.can_id_mode      = XMC_CAN_FRAME_TYPE_EXTENDED_29BITS,
	.can_priority     = XMC_CAN_ARBITRATION_MODE_IDE_DIR_BASED_PRIO_2,
	.can_id_mask      = 0x500000U,
	.can_ide_mask     = 1U,
	.can_mo_ptr       = (CAN_MO_TypeDef*) CAN_MO1,
	.can_data_length  = 8U,
	.can_data[1]      = 0x00000000U,
	.can_data[0]      = 0x00000000U
};

const CAN_NODE_LMO_t CAN_NODE_Battery_LMO_BatteryControl_Config = {
	.mo_ptr  = (XMC_CAN_MO_t*) &CAN_NODE_Battery_LMO_BatteryControl,
	.number  = 1U,
	.tx_event_enable = false,
	.rx_event_enable = true,
	.tx_sr   = 2U,
	.rx_sr   = 1U
};

XMC_CAN_MO_t CAN_NODE_Battery_LMO_BatteryStatus = {
	.can_mo_type      = XMC_CAN_MO_TYPE_TRANSMSGOBJ,
	.can_identifier   = 0x600319U,
	.can_id_mode      = XMC_CAN_FRAME_TYPE_EXTENDED_29BITS,
	.can_priority     = XMC_CAN_ARBITRATION_MODE_IDE_DIR_BASED_PRIO_2,
	.can_id_mask      = 0x600000U,
	.can_ide_mask     = 1U,
	.can_mo_ptr       = (CAN_MO_TypeDef*) CAN_MO2,
	.can_data_length  = 8U,
	.can_data[1]      = 0x00000000U,
	.can_data[0]      = 0x00000000U
};

const CAN_NODE_LMO_t CAN_NODE_Battery_LMO_BatteryStatus_Config = {
	.mo_ptr  = (XMC_CAN_MO_t*) &CAN_NODE_Battery_LMO_BatteryStatus,
	.number  = 2U,
	.tx_event_enable = true,
	.rx_event_enable = false,
	.tx_sr   = 2U,
	.rx_sr   = 1U
};


void CAN_NODE_0_SendMessage(uint8_t message_index, uint8_t *data)
{
	CAN_NODE_MO_UpdateData(CAN_NODE_0.lmobj_ptr[message_index], (uint8_t *) data);
	CAN_NODE_MO_Transmit(CAN_NODE_0.lmobj_ptr[message_index]);
}

void CAN_NODE_0_ErrorHandler(void)
{
        uint32_t status = 0x00U;

        // Check for Node error
        status = CAN_NODE_GetStatus(&CAN_NODE_0);
        if (status & XMC_CAN_NODE_STATUS_ALERT_WARNING)
        {
                // Clear the flag
                CAN_NODE_DisableEvent(&CAN_NODE_0, XMC_CAN_NODE_EVENT_ALERT);
        }
}

void CAN_NODE_0_TransmitHandler(void)
{
        // Empty
}

void CAN_NODE_0_ReceiveHandler(void)
{
        uint32_t status = 0x00U;

        for (uint8_t i = 0; i < CAN_NODE_0.mo_count; i++) {
                CAN_NODE_LMO_t *can_obj = CAN_NODE_0.lmobj_ptr[i];
                XMC_CAN_MO_t *mo = can_obj->mo_ptr;

                if (XMC_CAN_MO_TYPE_RECMSGOBJ != mo->can_mo_type) {
                        continue;
                }

                status = CAN_NODE_MO_GetStatus((void *) can_obj);
                if (status & XMC_CAN_MO_STATUS_RX_PENDING) {
                        Message_t message;
                        uint8_t *can_data = mo->can_data_byte;

                        CAN_NODE_MO_ClearStatus((void *) can_obj, XMC_CAN_MO_RESET_STATUS_RX_PENDING);
                        CAN_NODE_MO_Receive((void *) can_obj);

                        message.id = mo->can_identifier;
                        message.data[0] = can_data[0];
                        message.data[1] = can_data[1];
                        message.data[2] = can_data[2];
                        message.data[3] = can_data[3];
                        message.data[4] = can_data[4];
                        message.data[5] = can_data[5];
                        message.data[6] = can_data[6];
                        message.data[7] = can_data[7];

                        MessageManager_PushReceivedMessage(&message);
                }
        }
}
