/*
 * Date: 5.2.2017
 * Author: Atheel Redah
 */

#include "../header/Camera.h"
#include "../wifi/topics.h"

HAL_I2C i2c_cam(I2C_IDX1);
bool statusCAM;
bool camTrigger;
uint32_t ajklasdfjkl[HEIGHT*WIDTH*2];
image img;
int row;

Camera::Camera(const char * name) :
		Thread(name) {
}

Camera::~Camera() {
}

void Camera::init() {
	PRINTF("Camera is ready\r\n");
}

void Camera::run() {
}

/**
 * Init all GPIOs
 */

void Camera::init_camera() {
	int suc = i2c_cam.init();
	memset(this->DCMI_Buffer, (uint8_t) 0, IMAGESIZE);
	this->initGPIO();
	this->initDCMI();
	this->initDMA();
	this->initCAM();
	PRINTF("Camera Initialized");
	this->camera_ready = true;
}

void Camera::disable() {
	DCMI_Cmd(DISABLE);
	this->camera_ready = false;
	this->image_taken = false;
}

void Camera::initGPIO() {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(
	RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_DCMI); // PCLK
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_DCMI); // VSYNC
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_DCMI); // HREF
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_DCMI); // D0
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_DCMI); // D1
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource0, GPIO_AF_DCMI); // D2
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource1, GPIO_AF_DCMI); // D3
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource4, GPIO_AF_DCMI); // D4
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_DCMI); // D5
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_DCMI); // D6
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF_DCMI); // D7

	//Struct for Port-A (HREF, PCLK)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//Struct for Port-B (VSYNC, D5)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//Struct for Port-C (D0, D1)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//Struct for Port-E (D2,D3,D5,D6,D7)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	//Struct for PA10 (RESET)
	GPIO_InitStructure.GPIO_Pin = GPIO_PinSource10;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	//Struct for PC1(PowerDown)
	GPIO_InitStructure.GPIO_Pin = GPIO_PinSource1;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);

	// EXTCLK
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	RCC_MCO1Config(RCC_MCO1Source_HSI, RCC_MCO1Div_1);

}
/**
 * INIT DCMI
 */
void Camera::initDCMI() {
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI, ENABLE);
	DCMI_InitTypeDef DCMI_InitStructure;
	DCMI_DeInit();
	DCMI_InitStructure.DCMI_CaptureMode = DCMI_CaptureMode_SnapShot;
	DCMI_InitStructure.DCMI_CaptureRate = DCMI_CaptureRate_All_Frame;
	DCMI_InitStructure.DCMI_ExtendedDataMode = DCMI_ExtendedDataMode_8b;
	DCMI_InitStructure.DCMI_HSPolarity = DCMI_HSPolarity_Low;
	DCMI_InitStructure.DCMI_VSPolarity = DCMI_VSPolarity_High;
	DCMI_InitStructure.DCMI_PCKPolarity = DCMI_PCKPolarity_Rising;
	DCMI_InitStructure.DCMI_SynchroMode = DCMI_SynchroMode_Hardware;

	DCMI_Init(&DCMI_InitStructure);

	DCMI_CROPInitTypeDef DCMI_CROPInitStructure;

	DCMI_CROPInitStructure.DCMI_HorizontalOffsetCount = 0; //
	DCMI_CROPInitStructure.DCMI_CaptureCount = 2 * (WIDTH) - 1; //
	DCMI_CROPInitStructure.DCMI_VerticalStartLine = 0; //
	DCMI_CROPInitStructure.DCMI_VerticalLineCount = HEIGHT - 1; //

	DCMI_CROPConfig(&DCMI_CROPInitStructure);

	DCMI_CROPCmd(ENABLE);
}
/*
 * Init "Direct Memory Access" and "Nested Vectored Interrupt Controller"
 */
void Camera::initDMA() {
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	DMA_DeInit(DMA2_Stream1);
	DMA_InitStructure.DMA_BufferSize = IMAGESIZE / 4;
	DMA_InitStructure.DMA_Channel = DMA_Channel_1;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) &DCMI_Buffer;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_PeripheralBaseAddr = 0x50050028;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;

	DMA_Init(DMA2_Stream1, &DMA_InitStructure);

	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = DCMI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;

	NVIC_Init(&NVIC_InitStructure);
}

/**
 * Setting all registers
 */
int Camera::initCAM() {
	int result;
	AT(NOW() + 10 * MILLISECONDS);

	uint8_t reg_reset[2] = { 0x12, 0x80 };
	//reseting the cam
	result = i2c_cam.write(CAMADR, reg_reset, sizeof(reg_reset));
	AT(NOW() + 10*MILLISECONDS);

	if (result < 0) {
		i2c_cam.init();
	}

	reg_reset[1] = 0x00;
	result = i2c_cam.write(CAMADR, reg_reset, sizeof(reg_reset));
	if (result < 0) {
		i2c_cam.init();
	}
	AT(NOW() + 10*MILLISECONDS);

	int totalReg = sizeof(initReg) / 2;
	for (int i = 0; i < totalReg; i++) {
		result = i2c_cam.write(CAMADR, initReg[i], sizeof(initReg[i]));
		AT(NOW() + 2*MILLISECONDS);
		if (result < 0) {
#ifdef CAM_DEBUG
			PRINTF("Couldnt write register:%d result: %d\n", i, result);
#endif
		}
#ifdef CAM_DEBUG
		PRINTF("Writing all registers");
#endif
	}
	return result;
}

/**
 * Taking Picture
 */
void Camera::takePicture() {
	DCMI_ITConfig(DCMI_IT_FRAME, ENABLE);
	DCMI_ITConfig(DCMI_IT_OVF, ENABLE);
	DCMI_ITConfig(DCMI_IT_ERR, ENABLE);
	DMA_Cmd(DMA2_Stream1, ENABLE);
	DCMI_Cmd(ENABLE);
	DCMI_CaptureCmd(ENABLE);
	AT(NOW() + 10*MILLISECONDS);
	image_taken = true;
}

bool Camera::isReady() {
	return this->camera_ready;
}

bool Camera::pictureReady() {
	return this->image_taken;
}

/**
 * Sending image to groundstation
 *
 */
void Camera::sendImage() {

	//PRINTF("\r\n");

	for (int i = 0; i < IMAGESIZE; i++) {
		//PRINTF("\n Picture Byte[%d]=%d / %d", i, DCMI_Buffer[i], (HEIGHT*WIDTH*2));
		//ajklasdfjkl[i] = DCMI_Buffer[i];
	}
	/*Cannot publish the picture because the gs does not support*/
	//img.format = 1;
	//img.bytes = ajklasdfjkl;

	//tm_image.publish(img);

	//PRINTF("\r\n");
}

/**
 * Interrupt Handler for DCMI
 */
void DCMI_IRQHandler() {
	if (DCMI_GetFlagStatus(DCMI_FLAG_FRAMERI) == SET) {
		DCMI_ClearFlag(DCMI_FLAG_FRAMERI);
	} else if (DCMI_GetFlagStatus(DCMI_FLAG_OVFRI) == SET) {
		DCMI_ClearFlag(DCMI_FLAG_OVFRI);
	} else if (DCMI_GetFlagStatus(DCMI_FLAG_ERRRI) == SET) {
		DCMI_ClearFlag(DCMI_FLAG_ERRRI);
	}
	DCMI_CaptureCmd(DISABLE);
	DCMI_Cmd(DISABLE);
	DMA_Cmd(DMA2_Stream1, DISABLE);
	DCMI_Cmd(DISABLE);
	DCMI_ITConfig(DCMI_IT_FRAME, DISABLE);
}


