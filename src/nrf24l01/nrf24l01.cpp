#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>

#include "nrf24l01.h"


#define DEVICE_NAME  "/dev/nrf24l01"

#define SPI_IOC_MAGIC 'k'
#define SET_TX_ADDR 	_IO(SPI_IOC_MAGIC,0)
#define SET_RX_ADDR 	_IO(SPI_IOC_MAGIC,1)
#define SET_CHANNEL 	_IO(SPI_IOC_MAGIC,2)
#define SET_CRC_MODE	_IO(SPI_IOC_MAGIC,3)
#define GET_TX_ADDR 	_IO(SPI_IOC_MAGIC,4)
#define GET_RX_ADDR 	_IO(SPI_IOC_MAGIC,5)
#define GET_CHANNEL 	_IO(SPI_IOC_MAGIC,6)
#define GET_CRC_MODE 	_IO(SPI_IOC_MAGIC,7)
#define CHECK_DEVICE 	_IO(SPI_IOC_MAGIC,8)
#define SEND_MESSAGE 	_IO(SPI_IOC_MAGIC,9)
#define RECV_MESSAGE 	_IO(SPI_IOC_MAGIC,10)
	
#define DEVICE_IS_FIND 0x00 
#define DEVICE_NO_FIND 0x01
	
class NRF_OPS{
private:	
	int fd;
public:	
	int  init(struct ctl_data &cur);
	int  tx(uint8_t *buf,uint32_t count);
	int  rx(uint8_t *buf,uint32_t count);
	int  find_dev(void);
	NRF_OPS(){ cout <<"make nrf24l01"<<endl; }
	~NRF_OPS()
	{
		cout <<"close nrf24l01"<<endl;
		close(fd);
	} 
};





int NRF_OPS::init(struct ctl_data &cur)
{
	
	fd = open(DEVICE_NAME,O_RDWR);
	if(fd < 0) {
		printf("open is fail\n");
		return NRF_FAIL_NODEV;
	}

	if(find_dev()) 
		return NRF_FAIL_NOFIND;

	ioctl(fd,SET_TX_ADDR,cur.tx_addr);//设置目的地址
	ioctl(fd,SET_RX_ADDR,cur.rx_addr);//设置本机接收地址
	ioctl(fd,SET_CHANNEL,&cur.channel);//设置频道 24 ==2.424GHZ
	ioctl(fd,SET_CRC_MODE,&cur.crc);//设置CRC模式	
	return NRF_OK;
}

int NRF_OPS::tx(uint8_t*buf,uint32_t count)
{
	int ret;
	ret = write(fd, buf, count);
	return ret;
}

int NRF_OPS::rx(uint8_t *buf,uint32_t count)
{
	return  read(fd,buf,count);	
}

int NRF_OPS::find_dev(void)
{
	uint8_t is_find = 0;
	ioctl(fd,CHECK_DEVICE,&is_find);
	if(is_find == DEVICE_NO_FIND) {	
		printf("device is no find \n"); //检查设备是否存在
		return -1;
	}
	return 0;
}



static NRF_OPS ops; /*构造类*/
static struct ctl_data cur_ctl_data = {
	.tx_addr = {0x01,0x02,0x03,0x04,0x05},//目的地址
	.rx_addr = {0x01,0x02,0x03,0x04,0x05},//本机接收地址
	.channel = 24, //频道
	.crc = CRC_MODE_16,
};


/**
 * nrf24l01_init  模块初始化
 * @ctl_data: 初始化模块的控制参数块
 * Return:	
 */
int nrf24l01_init(void)
{
	int ret;
	ret = ops.init(cur_ctl_data);
	if(ret) 
		cout <<__func__<< "fail"<<endl;
	return ret;
}


/**
 * nrf24l01_tx  模块发送数据函数
 * @buf: 发送数据的缓存区
 * @count 发送数据的长度
 * Return:	返回NRF_OK表示发送成功,否则返回NRF_FAIL_TX
 */
int nrf24l01_tx(uint8_t *buf,uint32_t count)
{
	int ret;
	ret = ops.tx(buf,count);
	if(ret == 0) {
		cout << __func__<<"fail"<<endl;
		return NRF_FAIL_TX;
	}
	return NRF_OK;
}

/**
 * nrf24l01_rx  读取接收缓存的内容
 * @buf: 保存接收数据的缓存区
 * @count 期望接收的数据长度
 * Return:	返回实际接收到的数据长度
 */
uint32_t nrf24l01_rx(uint8_t *buf, uint32_t count)
{
	int ret;
	uint32_t len = count;
	count = ops.rx(buf,len);
	return count;
}

