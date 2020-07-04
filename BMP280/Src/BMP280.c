#include "main.h"

extern I2C_HandleTypeDef hi2c1;

/**
 * The finction cbecks the status of sensor
 */
void BMP280CheckStatus()
{
	if(HAL_I2C_IsDeviceReady(&hi2c1,0xEC,2,10) == HAL_OK)
	  {
		  printf("BMP280 found\r\n");
	  }
}
