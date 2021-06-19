/*
 * @Author: zzw
 * @email: huanling.zhang@tuya.com
 * @LastEditors: zzw
 * @file name: temp_check.h
 * @Description: 温度检测,ADC通道选择
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-04-22
 * @LastEditTime: 2021-05-25
 *
 */

#include "tuya_temp_check.h"
#include "gpio_8258.h"
#include "tuya_ble_api.h"
#include "timer.h"
#include "tuya_app_adc.h"


#define R25		10000		//NTC resistance at 25°C 10K
#define 	TIME_MS		1000

unsigned long sys_time = 0;
unsigned char dp_buf[] = {0x65, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00};
//10 - 21°C Resistance Comparison Table
unsigned long temp_buf[][3] = {{10, 19787, 20187}, {11, 18854, 19044}, {12, 17972, 18336}, {13, 17137, 17483},
							   {14, 16345, 16675}, {15, 15594, 15910}, {16, 14884, 15184}, {17, 14208, 14496},
							   {18, 13568, 13842}, {19, 12959, 13221}, {20, 12382, 12632}, {21, 11833, 12073}};

/*Temperature detection, recalled when the heat function is switched on*/
int temperature_detection(void)
{
	int Rntc = 0, Vcc = 0;

	if(!clock_time_exceed(sys_time, 1000 * TIME_MS)){
		return 0;
	}

	sys_time = clock_time();

	lux_adc_init();

	Vcc = adc_sample_and_get_result();	//uint:mv
	Rntc = Vcc*R25 / (3300-Vcc);
	TUYA_APP_LOG_INFO("Rntc_val=%dΩ", Rntc);
	if (Rntc >= temp_buf[0][1] && Rntc <= temp_buf[0][2]) {		//NTC with a resistance of 19787 - 20187Ω at 10°C

		TUYA_APP_LOG_INFO("Current temperature:%d℃", temp_buf[0][0]);
	}
	dp_buf[6] = temp_buf[0][0];
	tuya_ble_dp_data_report(dp_buf, 7);

	return 0;
}


