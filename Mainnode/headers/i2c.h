#define FOSC 12000000
#define CCLK FOSC*5
#define PCLK_1 CCLK/4
#define I2C_ENABLE 6
#define START 5
#define STOP 4
#define SI 3
#define AA 2
#define I2C_SPEED 100000
#define LOAD_VAL ((PCLK_1/I2C_SPEED)/2) 
#define SCL_SDA 0x00000050

#define MMA_ADDR 0X4C
#define X_DATA_REG 0X00
#define Y_DATA_REG 0X01
#define Z_DATA_REG 0X02
#define MODE_REG 0X07




void Init_I2C(void);
void start_I2C(void);
void stop_I2C(void);
void restart_I2C(void);
void write_I2C(char data);
char read_nack(void);
char read_mack(void);


void MMA_WRITE(char ,char );
char MMA_READ(char );
void accelerometer_init(void);
char convert_6_bit(char );
void read_accelerometer(void);

