#ifndef ME18_H
#define ME18_H


#include "can.h"

typedef enum{
    ME18_Func_None = 0,
    ME18_Func_Standby,
    ME18_Func_Start,
    ME18_Func_FOC
}ME18_Func;

typedef enum{
    ME18_STOP = 0,
    ME18_RUN
}ME18_STATE;

/** ME18 Motor Handler
 * init            ----> set hcan & STDID(default 0x01)
 * reset           ----> reset Motor
 * stop            ----> stop Motor and shut down FOC Mode
 * start           ----> start Motor, switch Motor to FOC Mode
 * set_pos         ----> pos between -720~720 */
typedef struct 
{	
	CAN_HandleTypeDef *hcan;	//发送can消息使用句柄
	uint32_t STDID;				//发送can消息Stdid
    struct 
	{
		float position;			//反馈获得的电机
		float velocity;			//反馈获得的电机速度
		float current; 			//反馈获得的电机电流
	}FdbData;	

	ME18_STATE state;

	void (*init)(CAN_HandleTypeDef *hcan,uint32_t STDID);  	//进行初始化 传入can句柄和舵机can通信的标准标识符
	void (*reset)(void);									//矫正舵机让舵机转的更准 执行时舵机会自转两周 
	void (*stop)(void);										//关闭舵机FOC工作模式 此时舵机停止				
	void (*start)(void);									//开启舵机FOC工作模式
	void (*set_pos)(float pos);								//传入舵机转动的目标角度 范围-720~720 单位 (度°)
}ME18_HandleTypeDef;

extern ME18_HandleTypeDef hME18;
static void ME18_INIT(CAN_HandleTypeDef *hcan,uint32_t STDID);

HAL_StatusTypeDef ME18_CanMsgDecode(uint32_t Stdid, uint8_t* fdbData);

#endif