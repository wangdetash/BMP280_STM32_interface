
void BMP280CheckStatus();
uint8_t BMP280Read8bit(uint8_t Register);
uint16_t BMP280Read16bit(uint8_t Register);
uint32_t BMP280Read24bit(uint8_t Register);
void BMP280Write8bit(uint8_t Register,uint8_t Data);
uint16_t BMP280read16_LE(uint8_t reg);
void BMP280readCoefficients();
void SetOverSamplingNPowerMode();
float BMP280readTemperature();
float BMP280readPressure();
void BMP280reset(void);
