#ifndef __NRF24L01_H_
#define __NRF24L01_H_

#include <stdlib.h>
#include <stdint.h>
#include <iostream>

using namespace std;

//#include "system.h"

#define CRC_MODE_8   0x00
#define CRC_MODE_16  0x01


enum NRF_STATUS {
	NRF_OK =		0x00,  /*操作成功*/
	NRF_FAIL_NODEV,		 /*没找到设备节点*/
	NRF_FAIL_NOFIND,	 /*没找到硬件设备*/
	NRF_FAIL_TX, 		 /* 发送出错 */
};


struct ctl_data{
	uint8_t tx_addr[5];
	uint8_t rx_addr[5];
	uint8_t channel;
	uint8_t crc;
};

int nrf24l01_init(void);
int nrf24l01_tx(uint8_t *buf,uint32_t count);
uint32_t nrf24l01_rx(uint8_t *buf,uint32_t count);

#endif

