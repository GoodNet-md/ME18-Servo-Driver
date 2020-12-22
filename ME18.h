#ifndef ME18_H
#define ME18_H


#include "can.h"

typedef enum{
    ME18_Func_None = 0,
    ME18_Func_Standby,
    ME18_Func_Start,
    ME18_Func_FOC
}ME18_Func;

/** ME18 Motor Handler
 * init            ----> set hcan & STDID(default 0x01)
 * reset           ----> reset Motor
 * stop            ----> stop Motor and shut down FOC Mode
 * start           ----> start Motor, switch Motor to FOC Mode
 * set_pos         ----> pos between 0~8*360 */
typedef struct 
{	
	CAN_HandleTypeDef *hcan;
	uint8_t STDID;
	
	void (*init)(CAN_HandleTypeDef *hcan,uint8_t STDID);
	void (*reset)(void);
	void (*stop)(void);
	void (*start)(void);
	void (*set_pos)(uint16_t pos);
}ME18_HandleTypeDef;

extern ME18_HandleTypeDef hME18;
static void ME18_INIT(CAN_HandleTypeDef *hcan,uint8_t STDID);
#endif