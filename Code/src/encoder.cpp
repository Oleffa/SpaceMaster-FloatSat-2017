

#include "../header/encoder.h"

//input pa5

#include "stm32f4xx_tim.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_misc.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_conf.h"
#include "hal/hal_pwm.h"


encoder::encoder(){

}

void encoder::encoder_init() {
        //PA5 TIM2 ETR

        GPIO_InitTypeDef GPIO_InitStructure;
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

        GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_TIM2);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

        TIM_TimeBaseInitStructure.TIM_Prescaler = 0;
        TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
        TIM_TimeBaseInitStructure.TIM_Period = 100000;
        TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
        TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

        TIM_ETRClockMode2Config(TIM2, TIM_ExtTRGPSC_OFF,
        TIM_ExtTRGPolarity_NonInverted, 0x00);

        TIM_Cmd(TIM2, ENABLE);
}

float encoder::encoder_read(float dt) {
        uint32_t val = TIM_GetCounter(TIM2);
        TIM_SetCounter(TIM2, 0);

        float temp = (float) val;
        temp /= dt;
        temp /= 16.0;
        return temp;
}
