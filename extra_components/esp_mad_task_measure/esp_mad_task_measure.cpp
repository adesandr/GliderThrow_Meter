/**
 * \file      esp_mad_task_measure.cpp
 * \author    Alain Désandré - alain.desandre@wanadoo.fr
 * \version   1.0
 * \date      28 octobre 2018
 * \brief     This task process the measure and calibration of the MPU6050
 *
 * \details   This task handles the periodicall MPU6050 measurement and computes the 
 *            angle and travel of the wing surface. This file is part of the esp_mad
 *            project. travel, chordSurface and angle are globals and used by the 
 *			  task_server (see esp_mad_task_http_server.c) to respond to the periodical 
 *			  ajax script 'get' request done by the esp.html page.
 * 
 */

/*-----------------------------------------
 *-            INCLUDES        
 *-----------------------------------------*/
#include <esp_log.h>
#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "MPU6050.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "sdkconfig.h"
#include <driver/i2c.h>
#include "math.h"
#include <Esp_mad.h>
#include <Esp_mad_Globals_Variables.h>

/*-----------------------------------------
 *-            LOCALS VARIABLES        
 *-----------------------------------------*/
static int16_t ax, ay, az;                       // raw measure
static int16_t gx, gy, gz;
static uint8_t Accel_range;
static uint8_t Gyro_range;

//Change this 3 variables if you want to fine tune to your needs.
static int buffersize=1000;     //Amount of readings used to average, make it higher to get more precision but sketch will be slower  (default:1000)
static int acel_deadzone=8;     //Acelerometer error allowed, make it lower to get more precision, but sketch may not converge  (default:8)
static int giro_deadzone=1;     //Giro error allowed, make it lower to get more precision, but sketch may not converge  (default:1)

static int mean_ax,mean_ay,mean_az,mean_gx,mean_gy,mean_gz,state=0;
static int ax_offset,ay_offset,az_offset,gx_offset,gy_offset,gz_offset;

static MPU6050 mpu = MPU6050();

/**
 * 	@fn			void meansensors(void)
 *	@brief		average sensors reading 
 * 	@param[in]	void
 *	@return		void
 *
 */
void meansensors(void){
  long i=0,buff_ax=0,buff_ay=0,buff_az=0,buff_gx=0,buff_gy=0,buff_gz=0;

  while (i<(buffersize+101)){
    // read raw accel/gyro measurements from device
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    if (i>100 && i<=(buffersize+100)){ //First 100 measures are discarded
      buff_ax=buff_ax+ax;
      buff_ay=buff_ay+ay;
      buff_az=buff_az+az;
      buff_gx=buff_gx+gx;
      buff_gy=buff_gy+gy;
      buff_gz=buff_gz+gz;
    }

    if (i==(buffersize+100)){
      mean_ax=buff_ax/buffersize;
      mean_ay=buff_ay/buffersize;
      mean_az=buff_az/buffersize;
      mean_gx=buff_gx/buffersize;
      mean_gy=buff_gy/buffersize;
      mean_gz=buff_gz/buffersize;
    }
    i++;
    vTaskDelay(2/portTICK_PERIOD_MS); //Needed so we don't get repeated measures
  }
} /* End meansensors() */

/**
 *	@fn 		void calibration(void)
 *	@brief		MPU6050 calibration
 * 	@param[in]	void
 *	@return		void
 *
 */
void calibration(void){
  ax_offset=-mean_ax/8;
  ay_offset=-mean_ay/8;
  az_offset=(16384-mean_az)/8;

  gx_offset=-mean_gx/4;
  gy_offset=-mean_gy/4;
  gz_offset=-mean_gz/4;
  
  while (1){
    int ready=0;
    mpu.setXAccelOffset(ax_offset);
    mpu.setYAccelOffset(ay_offset);
    mpu.setZAccelOffset(az_offset);

    mpu.setXGyroOffset(gx_offset);
    mpu.setYGyroOffset(gy_offset);
    mpu.setZGyroOffset(gz_offset);

    meansensors();

    if (abs(mean_ax)<=acel_deadzone) ready++;
    else ax_offset=ax_offset-mean_ax/acel_deadzone;

    if (abs(mean_ay)<=acel_deadzone) ready++;
    else ay_offset=ay_offset-mean_ay/acel_deadzone;

    if (abs(16384-mean_az)<=acel_deadzone) ready++;
    else az_offset=az_offset+(16384-mean_az)/acel_deadzone;

    if (abs(mean_gx)<=giro_deadzone) ready++;
    else gx_offset=gx_offset-mean_gx/(giro_deadzone+1);

    if (abs(mean_gy)<=giro_deadzone) ready++;
    else gy_offset=gy_offset-mean_gy/(giro_deadzone+1);

    if (abs(mean_gz)<=giro_deadzone) ready++;
    else gz_offset=gz_offset-mean_gz/(giro_deadzone+1);

    if (ready==6) break;
  }
} /* End calibration */

/**
 *	@fn 		void InitMPU6050(void)
 *  @brief		MPU6050 Initialisation	
 *	@param[in]	void
 *	@return		void	
 * 
 */
void InitMPU6050(void)
{
	static const char tagI[] = "Init ->";

	/*--- Display message ---*/
  	ESP_LOGI(tagI, "Calibration Start...\n");

    /*--- reset offsets   ---*/
  	mpu.setXAccelOffset(0);
  	mpu.setYAccelOffset(0);
  	mpu.setZAccelOffset(0);
  	mpu.setXGyroOffset(0);
    mpu.setYGyroOffset(0);
	mpu.setZGyroOffset(0);

  	/*--- Step 1 : first reading ---*/
  	ESP_LOGI(tagI,"First reading...\n");
	meansensors();
  	vTaskDelay(100/portTICK_PERIOD_MS);

  	/*--- Step 2 : Compute offsets ---*/
  	ESP_LOGI(tagI,"Compute offsets...\n");
	calibration();
  	vTaskDelay(100/portTICK_PERIOD_MS);

  	/*--- New reads and offsets display ---*/
  	meansensors();
	ESP_LOGI(tagI, "FINISHED!\n");
	ESP_LOGI(tagI, "offset: ax: %d - ay: %d - az: %d - gx: %d - gy: %d - gz: %d\n",
			ax_offset, ay_offset, az_offset,gx_offset,gy_offset,gz_offset);
	ESP_LOGI(tagI, "Read w. off : ax : %d - ay :%d - az :%d - gx : %d - gy :%d - gz :%d\n",
			mean_ax,mean_ay,mean_az,mean_gx,mean_gy,mean_gz);
  
	/*--- Set the end of calibration ---*/
	BInit = 1;

  	/*--- Set offsets with the compute values ---*/
  	mpu.setXAccelOffset(ax_offset);
  	mpu.setYAccelOffset(ay_offset);
  	mpu.setZAccelOffset(az_offset);
  	mpu.setXGyroOffset(gx_offset);
  	mpu.setYGyroOffset(gy_offset);
  	mpu.setZGyroOffset(gz_offset);
  
} /* End Init() */

/**
 *	@fn 		void task_measure(void*)
 *  @brief		MPU6050 periodicall compute	
 *	@param[in]	void*
 *	@return		void	
 * 
 */
void task_measure(void*){

	static const char tagd[] = "task_measure->";

	/*--- I2C Configuration and initialization ---*/
	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = (gpio_num_t)PIN_SDA;
	conf.scl_io_num = (gpio_num_t)PIN_CLK;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = 400000;
	ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
	ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));

	/*--- MPU6050 Initialization ---*/
	ESP_LOGI(tagd,"MPU6050 initialization ...");
	mpu.initialize();

	/*--- I2C Connection Test ---*/
	if(mpu.testConnection())
		ESP_LOGI(tagd, "I2C connection OK\n");
	
	else
	 	ESP_LOGI(tagd, "I2C connection Error\n");

	/*--- MPU6050 Calibration ---*/
	InitMPU6050();

	/*--- Infinite loop ---*/
	while(1){

		/*--- Compute Y angle in degree. Complementary filter is used to combine accelero and gyro datas      ---*/
    	/*--- see  http://www.pieter-jan.com/node/11 for more information regarding the complementary filter  ---*/
    	/*--- or https://delta-iot.com/la-theorie-du-filtre-complementaire/ (in french)                       ---*/
    	/*--- Basically complementary filter avoid used of kallman filter, quiet difficult to implement in    ---*/
    	/*--- small platform. Gyro are used for fast motion as accelero are used for slow motion.             ---*/
		/*--- The formula to compute angle is angle = 0.98 * (angle + gyrData * dt) + (0.02 * accData)        ---*/
		/*--- dt is 10 ms (so 0.01).                                                                          ---*/
		/*--- Raw GyrData need to be divide by the sensitivity scale factor (131). see MPU6050 datasheet p12. ---*/                     
    	mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    	angle=0.98*(angle+float(gy)*0.01/131) + 0.02*atan2((double)ax,(double)az)*180/PI;

    	/*--- Compute Control surface travel using : 2* sin(angle/2)* chord. Angle for sinus function needs  ---*/
    	/*--- to be converted in radian (angleDegre = angleRadian *(2*PI)/360)                               ---*/ 
    	travel = chordControlSurface * sin((angle*(2.0*PI)/360.0)/2.0) * 2.0;

		ESP_LOGD(tagd, "angle %f - travel %f\n",angle,travel);
		ESP_LOGD(tagd, "(abs)angle %d - (abs)travel %d\n",(int)abs(angle), (int)abs(travel));

		vTaskDelay(10/portTICK_PERIOD_MS);
		
		}

    /*--- this part will not be executed but we stick the free rtos recomandation ---*/
	vTaskDelete(NULL);

} /* end task_measure() */
