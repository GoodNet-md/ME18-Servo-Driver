#include "ME18.h"

/** ME18 Can Command Structure
 * 16 bit position command, between -4*pi and 4*pi
 * 12 bit velocity command, between -30 and + 30 rad/s
 * 12 bit kp, between 0 and 500 N-m/rad
 * 12 bit kd, between 0 and 100 N-m*s/rad
 * 12 bit feed forward torque, between -18 and 18 N-m
 * CAN Packet is 8 8-bit words
 * Formatted as follows.  For each quantity, bit 0 is LSB
 * 0: [position[15-8]]
 * 1: [position[7-0]] 
 * 2: [velocity[11-4]]
 * 3: [velocity[3-0], kp[11-8]]
 * 4: [kp[7-0]]
 * 5: [kd[11-4]]
 * 6: [kd[3-0], torque[11-8]]
 * 7: [torque[7-0]] */


ME18_HandleTypeDef hME18 = {.init=ME18_INIT};



int float_to_uint(float x, float x_min, float x_max, int bits){
    /// Converts a float to an unsigned int, given range and number of bits ///
    float span = x_max - x_min;
    float offset = x_min;
    return (int) ((x-offset)*((float)((1<<bits)-1))/span);
}

float uint_to_float(uint16_t x, float x_min, float x_max, int bit){
    float span = x_max - x_min;
    float offset = x_min;
    return (float) (((x*span)/(float)((1<<bit)-1))+offset);
}

void ME18_reset(){
    CAN_TxHeaderTypeDef TxHeader;
    uint8_t TxData[8];
    static uint32_t mailboxidx = 0;

    TxHeader.DLC=0x08;
    TxHeader.StdId=hME18.STDID;
    TxHeader.IDE=CAN_ID_STD;
    TxHeader.RTR=CAN_RTR_DATA;

    TxData[0]=0XFF; 
    TxData[1]=0XFF; 
    TxData[2]=0XFF;
    TxData[3]=0XFF;
    TxData[4]=0XFF;
    TxData[5]=0XFF;
    TxData[6]=0XFF;
    TxData[7]=0XFE;
    if(HAL_CAN_AddTxMessage(hME18.hcan,&TxHeader,TxData,&mailboxidx) != HAL_OK){
        Error_Handler();
    }
}
void ME18_stop(){
    CAN_TxHeaderTypeDef TxHeader;
    uint8_t TxData[8];
    static uint32_t mailboxidx = 0;

    TxHeader.DLC=0x08;
    TxHeader.StdId=hME18.STDID;
    TxHeader.IDE=CAN_ID_STD;
    TxHeader.RTR=CAN_RTR_DATA;

    TxData[0]=0XFF; 
    TxData[1]=0XFF; 
    TxData[2]=0XFF;
    TxData[3]=0XFF;
    TxData[4]=0XFF;
    TxData[5]=0XFF;
    TxData[6]=0XFF;
    TxData[7]=0XFD;
    if(HAL_CAN_AddTxMessage(hME18.hcan,&TxHeader,TxData,&mailboxidx) != HAL_OK){
        Error_Handler();
    }
}
void ME18_start(){
    CAN_TxHeaderTypeDef TxHeader;
    uint8_t TxData[8];
    static uint32_t mailboxidx = 0;

    TxHeader.DLC=0x08;
    TxHeader.StdId=hME18.STDID;
    TxHeader.IDE=CAN_ID_STD;
    TxHeader.RTR=CAN_RTR_DATA;

    TxData[0]=0XFF; 
    TxData[1]=0XFF; 
    TxData[2]=0XFF;
    TxData[3]=0XFF;
    TxData[4]=0XFF;
    TxData[5]=0XFF;
    TxData[6]=0XFF;
    TxData[7]=0XFC;
    if(HAL_CAN_AddTxMessage(hME18.hcan,&TxHeader,TxData,&mailboxidx) != HAL_OK){
        Error_Handler();
    }
}
void ME18_setPos(float pos){
    if(pos > 8*360) pos = 8*360;

    int position, velocity, kp, kd, torque;
    // float radPos = (float)pos/180.0f*3.1415926f - 4.0f*3.1415926f;
    float radPos = (float)pos/180.0f*3.1415926f;
    position = float_to_uint(radPos, -4*3.1415926,4*3.1415926,16);
    velocity = float_to_uint(0,-30,30,12);
    kp = float_to_uint(50.0,0,500,12);
    kd = float_to_uint(20.0,0,200,12);
    torque = float_to_uint(0,-18,18,12);

    CAN_TxHeaderTypeDef TxHeader;
    uint8_t TxData[8];
    static uint32_t mailboxidx = 0;

    TxHeader.DLC=0x08;
    TxHeader.StdId=hME18.STDID;
    TxHeader.IDE=CAN_ID_STD;
    TxHeader.RTR=CAN_RTR_DATA;

    TxData[0]=position>>8; 
    TxData[1]=position&0xff;
    TxData[2]=velocity>>4;
    TxData[3]=((velocity&0x000f)<<4)+(kp>>8);
    TxData[4]=kp&0xff;
    TxData[5]=kd>>4;
    TxData[6]=((kd&0x000f)<<4)+(torque>>8);
    TxData[7]=torque&0x00ff;
    if(HAL_CAN_AddTxMessage(hME18.hcan,&TxHeader,TxData,&mailboxidx) != HAL_OK){
        Error_Handler();
    }
}

static void ME18_INIT(CAN_HandleTypeDef *hcan,uint32_t STDID){
    hME18.hcan = hcan;
    hME18.STDID = STDID;

    hME18.reset = ME18_reset;
    hME18.start = ME18_start;
    hME18.stop = ME18_stop;
    hME18.set_pos = ME18_setPos;
}

HAL_StatusTypeDef ME18_CanMsgDecode(uint32_t Stdid, uint8_t* fdbData){
    if (Stdid == 0x00)
    {
        uint16_t position, vel, current;
        /**
         * only position is useful
        */
        position = ((fdbData[1]<<8)|fdbData[0]);

        /**
         * Wait for someone nice to fix the decoding
         * */
        vel = (((fdbData[3]>>4)<<8)|(fdbData[2]));
        current = (((fdbData[3]&0xF)<<8) | fdbData[4] );

        hME18.FdbData.position = uint_to_float(position, -4*3.1415926f,4*3.1415926f,16);
        hME18.FdbData.velocity = uint_to_float(vel,-30.0f,30.0f,12);
        hME18.FdbData.current = uint_to_float(current, -40.0f, 40.0f, 12);

        return HAL_OK;
    }
    return HAL_ERROR;
    
}