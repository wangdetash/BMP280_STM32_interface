#include "main.h"

/**
 *  7 bit i2c address appednded with 0 at RHS.
 */
#define BMP280_ADDRESS 		  (0xEC) /* The 7 bit address is 0x76 */
#define BMP280_ADDRESS_ALT    (0xEE) /* The 7 bit address is 0x77 */

/**
 * Registers available on the sensor.
 */
enum {
  BMP280_REGISTER_DIG_T1 = 0x88,
  BMP280_REGISTER_DIG_T2 = 0x8A,
  BMP280_REGISTER_DIG_T3 = 0x8C,
  BMP280_REGISTER_DIG_P1 = 0x8E,
  BMP280_REGISTER_DIG_P2 = 0x90,
  BMP280_REGISTER_DIG_P3 = 0x92,
  BMP280_REGISTER_DIG_P4 = 0x94,
  BMP280_REGISTER_DIG_P5 = 0x96,
  BMP280_REGISTER_DIG_P6 = 0x98,
  BMP280_REGISTER_DIG_P7 = 0x9A,
  BMP280_REGISTER_DIG_P8 = 0x9C,
  BMP280_REGISTER_DIG_P9 = 0x9E,
  BMP280_REGISTER_CHIPID = 0xD0,
  BMP280_REGISTER_VERSION = 0xD1,
  BMP280_REGISTER_SOFTRESET = 0xE0,
  BMP280_REGISTER_CAL26 = 0xE1, /**< R calibration = 0xE1-0xF0 */
  BMP280_REGISTER_STATUS = 0xF3,
  BMP280_REGISTER_CONTROL = 0xF4,
  BMP280_REGISTER_CONFIG = 0xF5,
  BMP280_REGISTER_PRESSUREDATA = 0xF7,
  BMP280_REGISTER_TEMPDATA = 0xFA,
};

/*
 *  Struct to hold calibration data.
 */
typedef struct {
  uint16_t dig_T1; /**< dig_T1 cal register. */
  int16_t dig_T2;  /**<  dig_T2 cal register. */
  int16_t dig_T3;  /**< dig_T3 cal register. */

  uint16_t dig_P1; /**< dig_P1 cal register. */
  int16_t dig_P2;  /**< dig_P2 cal register. */
  int16_t dig_P3;  /**< dig_P3 cal register. */
  int16_t dig_P4;  /**< dig_P4 cal register. */
  int16_t dig_P5;  /**< dig_P5 cal register. */
  int16_t dig_P6;  /**< dig_P6 cal register. */
  int16_t dig_P7;  /**< dig_P7 cal register. */
  int16_t dig_P8;  /**< dig_P8 cal register. */
  int16_t dig_P9;  /**< dig_P9 cal register. */
} bmp280_calib_data;

/** Oversampling rate for the sensor. */
  enum sensor_sampling {
    /** No over-sampling. */
    SAMPLING_NONE = 0x00,
    /** 1x over-sampling. */
    SAMPLING_X1 = 0x01,
    /** 2x over-sampling. */
    SAMPLING_X2 = 0x02,
    /** 4x over-sampling. */
    SAMPLING_X4 = 0x03,
    /** 8x over-sampling. */
    SAMPLING_X8 = 0x04,
    /** 16x over-sampling. */
    SAMPLING_X16 = 0x05
  };

  /** Operating mode for the sensor. */
  enum sensor_mode {
    /** Sleep mode. */
    MODE_SLEEP = 0x00,
    /** Forced mode. */
    MODE_FORCED = 0x01,
    /** Normal mode. */
    MODE_NORMAL = 0x03,
    /** Software reset. */
    MODE_SOFT_RESET_CODE = 0xB6
  };

  /** Filtering level for sensor data. */
  enum sensor_filter {
    /** No filtering. */
    FILTER_OFF = 0x00,
    /** 2x filtering. */
    FILTER_X2 = 0x01,
    /** 4x filtering. */
    FILTER_X4 = 0x02,
    /** 8x filtering. */
    FILTER_X8 = 0x03,
    /** 16x filtering. */
    FILTER_X16 = 0x04
  };

  /** Standby duration in ms */
  enum standby_duration {
    /** 1 ms standby. */
    STANDBY_MS_1 = 0x00,
    /** 62.5 ms standby. */
    STANDBY_MS_63 = 0x01,
    /** 125 ms standby. */
    STANDBY_MS_125 = 0x02,
    /** 250 ms standby. */
    STANDBY_MS_250 = 0x03,
    /** 500 ms standby. */
    STANDBY_MS_500 = 0x04,
    /** 1000 ms standby. */
    STANDBY_MS_1000 = 0x05,
    /** 2000 ms standby. */
    STANDBY_MS_2000 = 0x06,
    /** 4000 ms standby. */
    STANDBY_MS_4000 = 0x07
  };

extern I2C_HandleTypeDef hi2c1;
uint8_t i2cData[3];

int32_t t_fine;
bmp280_calib_data _bmp280_calib;

/**
 * @brief The finction cbecks the status of sensor
 */
void BMP280CheckStatus()
{
	if(HAL_I2C_IsDeviceReady(&hi2c1,BMP280_ADDRESS,2,10) == HAL_OK)
	  {
		  printf("BMP280 found\r\n");
	  }
}

/**
 * @brief Reads 8 bit register.
 * @param The register adddress to be read.
 * @return 8 bit data from the register.
 */
uint8_t BMP280Read8bit(uint8_t Register)
{
	i2cData[0] = Register;
	HAL_I2C_Master_Transmit(&hi2c1,BMP280_ADDRESS,i2cData,1,10);
	HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDRESS,&i2cData[1], 1, 10);
	return i2cData[1];
}

/**
 * @brief Reads 2 consecutive 8 bit register.
 * @param Register The starting address of register to be read.
 * @return the 16 bit obtained after reading 2 registers.
 */
uint16_t BMP280Read16bit(uint8_t Register)
{
	uint32_t Out,msb,lsb;
	i2cData[0] = Register;
	HAL_I2C_Master_Transmit(&hi2c1,BMP280_ADDRESS,i2cData,1,10);
	HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDRESS,&i2cData[0],2, 10);
	msb = (i2cData[0] << 8);
	lsb = (i2cData[1]);
	Out = msb|lsb;
	return Out;
}

/**
 * @brief Reads 3 consecutive 8 bit registers.
 * @param Register The starting register address.
 * @return the 24 bit obtained after reading 3 registers.
 */
uint32_t BMP280Read24bit(uint8_t Register)
{
	uint32_t Out,msb,lsb,xlsb;
	i2cData[0] = Register;
	HAL_I2C_Master_Transmit(&hi2c1,BMP280_ADDRESS,i2cData,1,10);
	HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDRESS,&i2cData[0],3, 10);
	msb = (i2cData[0] << 16);
	lsb = (i2cData[1] << 8);
	xlsb = (i2cData[2] >> 4);
	Out = msb|lsb|xlsb;
	return Out;
}

/**
 * @brief Reads 8 bit data from register.
 * @param Register The 8 bit register address.
 * @param Data The data to be written to the register.
 */
void BMP280Write8bit(uint8_t Register,uint8_t Data)
{
	i2cData[0] = Register;
	i2cData[1] = Data;
	HAL_I2C_Master_Transmit(&hi2c1,BMP280_ADDRESS,i2cData,2,30);
}



uint16_t BMP280read16_LE(uint8_t reg)
{
  uint16_t temp = BMP280Read16bit(reg);
  return (temp >> 8) | (temp << 8);
}


/*
 *  @brief  Reads the factory-set coefficients
 */
void BMP280readCoefficients()
{
  _bmp280_calib.dig_T1 = BMP280read16_LE(BMP280_REGISTER_DIG_T1);
  _bmp280_calib.dig_T2 = BMP280read16_LE(BMP280_REGISTER_DIG_T2);
  _bmp280_calib.dig_T3 = BMP280read16_LE(BMP280_REGISTER_DIG_T3);

  _bmp280_calib.dig_P1 = BMP280read16_LE(BMP280_REGISTER_DIG_P1);
  _bmp280_calib.dig_P2 = BMP280read16_LE(BMP280_REGISTER_DIG_P2);
  _bmp280_calib.dig_P3 = BMP280read16_LE(BMP280_REGISTER_DIG_P3);
  _bmp280_calib.dig_P4 = BMP280read16_LE(BMP280_REGISTER_DIG_P4);
  _bmp280_calib.dig_P5 = BMP280read16_LE(BMP280_REGISTER_DIG_P5);
  _bmp280_calib.dig_P6 = BMP280read16_LE(BMP280_REGISTER_DIG_P6);
  _bmp280_calib.dig_P7 = BMP280read16_LE(BMP280_REGISTER_DIG_P7);
  _bmp280_calib.dig_P8 = BMP280read16_LE(BMP280_REGISTER_DIG_P8);
  _bmp280_calib.dig_P9 = BMP280read16_LE(BMP280_REGISTER_DIG_P9);
}

/**
 * @brief The function used to set sampling rate for both temperature and
 * pressure measurement.Power mode can also be set set by writing
 * apporpriate value to the register.The function should be called
 * inorder to start measurement of both temp and pressure.
 */
void SetOverSamplingNPowerMode()
{
	BMP280Write8bit(BMP280_REGISTER_CONTROL,0x27);
}

/*
 * Reads the temperature from the device.
 * @return The temperature in degress celcius.
 */
float BMP280readTemperature()
{
  int32_t var1, var2;

  int32_t adc_T = BMP280Read24bit(BMP280_REGISTER_TEMPDATA);
  adc_T >>= 4;

  var1 = ((((adc_T >> 3) - ((int32_t)_bmp280_calib.dig_T1 << 1))) *
          ((int32_t)_bmp280_calib.dig_T2)) >>
         11;

  var2 = (((((adc_T >> 4) - ((int32_t)_bmp280_calib.dig_T1)) *
            ((adc_T >> 4) - ((int32_t)_bmp280_calib.dig_T1))) >>
           12) *
          ((int32_t)_bmp280_calib.dig_T3)) >>
         14;

  t_fine = var1 + var2;

  float T = (t_fine * 5 + 128) >> 8;
  return T / 100;
}

/*
 * Reads the barometric pressure from the device.
 * @return Barometric pressure in Pa.
 */
float BMP280readPressure()
{
  int64_t var1, var2, p;

  // Must be done first to get the t_fine variable set up
  BMP280readTemperature();

  int32_t adc_P = BMP280Read24bit(BMP280_REGISTER_PRESSUREDATA);
  adc_P >>= 4;

  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)_bmp280_calib.dig_P6;
  var2 = var2 + ((var1 * (int64_t)_bmp280_calib.dig_P5) << 17);
  var2 = var2 + (((int64_t)_bmp280_calib.dig_P4) << 35);
  var1 = ((var1 * var1 * (int64_t)_bmp280_calib.dig_P3) >> 8) +
         ((var1 * (int64_t)_bmp280_calib.dig_P2) << 12);
  var1 =
      (((((int64_t)1) << 47) + var1)) * ((int64_t)_bmp280_calib.dig_P1) >> 33;

  if (var1 == 0) {
    return 0; // avoid exception caused by division by zero
  }
  p = 1048576 - adc_P;
  p = (((p << 31) - var2) * 3125) / var1;
  var1 = (((int64_t)_bmp280_calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
  var2 = (((int64_t)_bmp280_calib.dig_P8) * p) >> 19;

  p = ((p + var1 + var2) >> 8) + (((int64_t)_bmp280_calib.dig_P7) << 4);
  return (float)p / 256;
}

/*
 *  @brief  Resets the chip via soft reset
 */
void BMP280reset()
{
	BMP280Write8bit(BMP280_REGISTER_SOFTRESET, MODE_SOFT_RESET_CODE);
}
