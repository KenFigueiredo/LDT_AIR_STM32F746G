//RECEIVER

#include "stm32f7xx_hal.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "cmsis_os.h"
#include "ethernetif.h"

#include <string.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;
#ifdef __cplusplus
extern "C" {
#endif

	void SysTick_Handler(void);
	void ETH_IRQHandler(void);
	
#ifdef __cplusplus
}
#endif

struct netif gnetif;

#pragma region IP address definition
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   0
#define IP_ADDR3   80
   
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   0
#define GW_ADDR3   1 
#pragma endregion

static void SystemClock_Config(void);
static void MainThread(void const * argument);
static void Netif_Config(void);
static void MPU_Config(void);
static void GPIOInit();
char* split_string(char* str, char* delim);
void getByteArray(char* data, int count, int count2, int len);
int getIByte(int data, int count);
char getCByte(char data, int count);
void handleOneWayMsg();
void handleMultiFile(bool first); 


static void timerInit();
void delayMicros(uint32_t del);

uint32_t microTicks, multiplier, bitDelay = 10, mDel = 9;
int pktSize, pathSize, totalParts, curPart;
char* path; char* payload;
char buf[100];
static int onOff;

#define ONEWAYID 	0x33
#define EXITID		0x35
#define MULTFID 	0x55
#define ZERO		0
#define SEVEN		7

int main(void) {
	MPU_Config();
	SCB_EnableICache();
	SCB_EnableDCache();
	HAL_Init();  
	SystemClock_Config(); 
	GPIOInit();
	timerInit();

	osThreadDef(Start, MainThread, osPriorityRealtime, 0, configMINIMAL_STACK_SIZE * 5);
	osThreadCreate(osThread(Start), NULL);
  
	osKernelStart();
	
	/* We should never get here as control is now taken by the scheduler */
	for (;;)
		;
}

void SysTick_Handler(void) {
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
	osSystickHandler();
}

void ETH_IRQHandler(void) {
	ETHERNET_IRQHandler();
}

void delayMicros(uint32_t del) {
	del = del * multiplier - 10;
	while (del--)
		;
}

static void MainThread(void const * argument) { 
		
	struct netconn *pListeningConnection, *pAcceptedConnection;
	err_t err;
		
	tcpip_init(NULL, NULL);
	Netif_Config();
	int portNum = 5419;
	
	gnetif.ip_addr.addr = (unsigned)1342220480; //192.168.0.80
	gnetif.netmask.addr = (unsigned)16777215; //255.255.255.0
	gnetif.gw.addr = (unsigned)16820416; //192.168.0.1
	
	printf("Hosting server at: %d.%d.%d.%d:%d ",
		(unsigned)gnetif.ip_addr.addr & 0xFF,
		((unsigned)gnetif.ip_addr.addr >> 8) & 0xFF,
		((unsigned)gnetif.ip_addr.addr >> 16) & 0xFF,
		((unsigned)gnetif.ip_addr.addr >> 24) & 0xFF,
		portNum);
	printf("[%d.%d.%d.%d : ",
		(unsigned)gnetif.netmask.addr & 0xFF,
		((unsigned)gnetif.netmask.addr >> 8) & 0xFF,
		((unsigned)gnetif.netmask.addr >> 16) & 0xFF,
		((unsigned)gnetif.netmask.addr >> 24) & 0xFF);
	printf("%d.%d.%d.%d]\n",
		(unsigned)gnetif.gw.addr & 0xFF,
		((unsigned)gnetif.gw.addr >> 8) & 0xFF,
		((unsigned)gnetif.gw.addr >> 16) & 0xFF,
		((unsigned)gnetif.gw.addr >> 24) & 0xFF);
	

	pListeningConnection = netconn_new(NETCONN_TCP);
	if (!pListeningConnection)
		asm("bkpt 255");
	err = netconn_bind(pListeningConnection, NULL, portNum);
    
	if (err != ERR_OK)
		asm("bkpt 255");
	netconn_listen(pListeningConnection);

	bool connected = false;
	GPIOG->BSRR = 0x0080;

	for (;;) {
		if (connected) {
			struct netbuf *inbuf = NULL;
			cout << "Client connected. ";
			GPIOG->BSRR = 0x0080 << 16;
			GPIOG->BSRR = 0x0040;
			

			err = netconn_recv(pAcceptedConnection, &inbuf);
			if (err != ERR_OK)
				asm("bkpt 255");

			unsigned short len = netbuf_len(inbuf);
			char* data = new char[len];
			netbuf_copy(inbuf, data, len);
								
			string str(split_string(data, "\n"));
			delete []data;
			cout << "IP: " << str << endl;
			str = "Hello " + str;
			netconn_write(pAcceptedConnection, (const unsigned char*)str.c_str(), str.length(), NETCONN_NOCOPY);
			netbuf_free(inbuf);
			netbuf_delete(inbuf);	

			while (connected) {
				ostringstream oss;
				string str = "Receiever ready, awaiting signal...\n";
				cout << str;
				netconn_write(pAcceptedConnection, (const unsigned char*)str.c_str(), str.length(), NETCONN_NOCOPY);	
				
				bool readin = false;
				int g = 0, h = 0, i = 0, j = 0, k = 0, w = 0, x = 0, y = 0, z = 0, seven = 7;
				uint32_t val;
				char keyWord = 0;
				bitDelay = 11;
				
				for (int i = 0; i < 100; i++) {
					buf[i] = 0;
				}
				
				path = (char *)pvPortMalloc(100);
				payload = (char *)pvPortMalloc(200);
				for (int b = 0; b < 100; b++){
					path[b] = 0;
				}
				
				for (int b = 0; b < 200; b++){
					payload[b] = 0;
				}
				
				while (!readin) {
					if ((GPIOC->IDR & GPIO_PIN_7) > 0) {
						while ((GPIOC->IDR & GPIO_PIN_7) > 0) {;}	
						x = 0, keyWord = 0;
						
						while (x < 8) {
							keyWord |= ((GPIOC->IDR & GPIO_PIN_7) >> x);
							x++;
							HAL_Delay(bitDelay);
						}
						
						if (keyWord == ONEWAYID) {
							pktSize = 0;
							x = 0, i = 0, j = 0;
						
							while ((GPIOC->IDR & GPIO_PIN_7) >> 7);	
	
							while (x < 8) {
								pktSize |= ((GPIOC->IDR & GPIO_PIN_7) >> x);
								x++;
								HAL_Delay(bitDelay);
							}
	
							while ((GPIOC->IDR & GPIO_PIN_7) >> 7);
	
							while (i < pktSize) {
								while (j < 8) {
									buf[i] |= ((GPIOC->IDR & GPIO_PIN_7) >> j);
									j++;
									HAL_Delay(bitDelay);          
								}
								i++;
								j = 0;
								while ((GPIOC->IDR & GPIO_PIN_7) >> 7);
							}
							
							string str = "key:!\n"; // ! == 0x33
							netconn_write(pAcceptedConnection, (const unsigned char*)str.c_str(), str.length(), NETCONN_NOCOPY);
							string str2(buf);
							cout << "Payload: " << str2 << endl;
							netconn_write(pAcceptedConnection, (const unsigned char*)str2.c_str(), str2.length(), NETCONN_NOCOPY);

							for (int i = 0; i < 100; i++) {
								buf[i] = 0;
							}
							
							readin = true;
						}
						
						else if (keyWord == MULTFID) {
							bool first = true;	
							pktSize = 0, pathSize = 0, totalParts = 1, curPart = 0;
							
							while (curPart != totalParts) {	
								g = 0, h = 0, i = 0, j = 0, k = 0, w = 0, x = 0, y = 0, z = 0;
								
								while ((GPIOC->IDR & GPIO_PIN_7) != 0);	// wait for sync
	
								if (first) {
									while (x < 8) {
										val = HAL_GetTick();
										pathSize |= ((GPIOC->IDR & GPIO_PIN_7) >> x);
										x++;
										while (HAL_GetTick() - val <= bitDelay);
									}

									while ((GPIOC->IDR & GPIO_PIN_7) != 0);	// wait for sync
		
									while (y < pathSize) {
										while (z < 8) {
											val = HAL_GetTick();
											path[y] |= ((GPIOC->IDR & GPIO_PIN_7) >> z);
											z++;
											while (HAL_GetTick() - val <= bitDelay);           
										}
										
										y++;
										z = 0;
										while ((GPIOC->IDR & GPIO_PIN_7) != 0);
										first = false;
									}

									while ((GPIOC->IDR & GPIO_PIN_7) != 0);	// wait for sync
									
									while (w < 8) {
										val = HAL_GetTick();
										totalParts |= ((GPIOC->IDR & GPIO_PIN_7) >> w);
										w++;
										while (HAL_GetTick() - val <= bitDelay);
									}
									
									while ((GPIOC->IDR & GPIO_PIN_7) != 0);	// wait for sync		
								}
								
								while (h < 8) {
									curPart |= ((GPIOC->IDR & GPIO_PIN_7) >> h);
									h++;
									val = HAL_GetTick();
									while (HAL_GetTick() - val <= bitDelay);
								}
								
								while ((GPIOC->IDR & GPIO_PIN_7) != 0);	// wait for sync
		
								while (g < 8) {
									val = HAL_GetTick();
									pktSize |= ((GPIOC->IDR & GPIO_PIN_7) >> g);
									g++;
									while (HAL_GetTick() - val <= bitDelay);
								}

								while ((GPIOC->IDR & GPIO_PIN_7) != 0);	// wait for sync
		
								while (i < pathSize) {
									while (j < 8) {
										val = HAL_GetTick();
										payload[i] |= ((GPIOC->IDR & GPIO_PIN_7) >> j);
										j++;
										while (HAL_GetTick() - val <= bitDelay);          
									}
									i++;
									j = 0;
									while ((GPIOC->IDR & GPIO_PIN_7) != 0);
								}
								
								oss << "key:" << keyWord << " cur:" << curPart << " tot:" << totalParts;
								string str = oss.str();
								netconn_write(pAcceptedConnection, (const unsigned char*)path, pathSize, NETCONN_NOCOPY);			
								netconn_write(pAcceptedConnection, (const unsigned char*)str.c_str(), str.length(), NETCONN_NOCOPY);
								netconn_write(pAcceptedConnection, (const unsigned char*)payload, pktSize, NETCONN_NOCOPY);
							}
							//vPortFree(payload);
							//vPortFree(path);
							for (int b = 0; b < 100; b++) {
								path[b] = 0;
							}
				
							for (int b = 0; b < 200; b++) {
								payload[b] = 0;
							}
							readin = true;
						}
						
						else if (keyWord == EXITID) {
							connected = false;
							readin = true;
							GPIOG->BSRR = 0x0040 << 16;
							GPIOG->BSRR = 0x0080;
						}
						
						else {
							//ignore
						}
					}
				}
			}

			netconn_close(pAcceptedConnection);
			netconn_delete(pAcceptedConnection);
		}
		err = netconn_accept(pListeningConnection, &pAcceptedConnection);
		if (err == ERR_OK) connected = true;
	}
}

void handleOneWayMsg() {	
	int x = 0, i = 0, j = 0;
	uint32_t val;
	while ((GPIOC->IDR & GPIO_PIN_7) != 0)
		;	
	
	while (x < 8) {
		pktSize |= ((GPIOC->IDR & GPIO_PIN_7) >> x);
		x++;
		val = HAL_GetTick();
		while (HAL_GetTick() - val <= bitDelay)
			;
	}
	
	while ((GPIOC->IDR & GPIO_PIN_7) != 0)
		;
	
	while (i < pktSize) {
		while (j < 8) {
			buf[i] |= ((GPIOC->IDR & GPIO_PIN_7) >> j);
			j++;
			val = HAL_GetTick();
			while (HAL_GetTick() - val <= bitDelay)
				;           
		}
		i++;
		j = 0;
		while ((GPIOC->IDR & GPIO_PIN_7) != 0)
			;
	}
}

void handleMultiFile(bool first) {
	
	int zero = 0, seven = 7;	
	
	while ((GPIOC->IDR & GPIO_PIN_7) != 0)
		;	// wait for sync
	
	if (first) {	
		pathSize = getIByte(0, seven); // get total path size
		while ((GPIOC->IDR & GPIO_PIN_7) != 0)
			;	// wait for sync
		
		path = (char *)pvPortMalloc(pathSize);
		getByteArray(path, zero, seven, pathSize);
		while ((GPIOC->IDR & GPIO_PIN_7) != 0)
			;	// wait for sync
		
		totalParts = getIByte(0, seven); // get total number of packets	
		while ((GPIOC->IDR & GPIO_PIN_7) != 0)
			;	// wait for sync		
	}
		
	curPart = getIByte(0, seven); // get current part out of totalSize
	while ((GPIOC->IDR & GPIO_PIN_7) != 0)
		;	// wait for sync
		
	pktSize = getIByte(0, seven); // get current packetSize
	while ((GPIOC->IDR & GPIO_PIN_7) != 0)
		;	// wait for sync
		
	payload = (char *)pvPortMalloc(pktSize); // allot heap for payload
	getByteArray(payload, zero, seven, pktSize); //fill payload array with data
	
}

char getCByte(char data, int count) {
	while (count < 8) {
		uint32_t val = HAL_GetTick();
		data |= ((GPIOC->IDR & GPIO_PIN_7) >> count);
		count++;
		while (HAL_GetTick() - val <= bitDelay)
			;
	}
	
	return data;
}

int getIByte(int data, int count) {
	while (count < 8) {
		data |= ((GPIOC->IDR & GPIO_PIN_7) >> count);
		count++;
		delayMicros(mDel);
		//uint32_t val = HAL_GetTick();
		//while (HAL_GetTick() - val <= bitDelay);
	}
	
	return data;
}

void getByteArray(char* data, int count, int count2, int len) {	
	while (count < len) {
		while (count2 < 8) {
			data[count] |= ((GPIOC->IDR & GPIO_PIN_7) >> count2);
			count2++;
			uint32_t val = HAL_GetTick();
			while (HAL_GetTick() - val <= bitDelay)
				;           
		}
		count++;
		count2 = 0;
		while ((GPIOC->IDR & GPIO_PIN_7) != 0)
			;
	}
}
			
static void GPIOInit() {
	__GPIOC_CLK_ENABLE();
	__GPIOG_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_Init, GPIOG_Init;

	GPIO_Init.Pin = GPIO_PIN_7;
	GPIO_Init.Mode = GPIO_MODE_INPUT;
	GPIO_Init.Speed =  GPIO_SPEED_HIGH;
	GPIO_Init.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_Init);
	
	GPIOG_Init.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	GPIOG_Init.Mode = GPIO_MODE_OUTPUT_PP;
	GPIOG_Init.Speed =  GPIO_SPEED_HIGH;
	GPIOG_Init.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOG, &GPIOG_Init);
	
}

char* split_string(char* str, char* delim) {
	char* split = strtok(str, delim);

	if (split != NULL) {
		return split;
	}

	else
		return str;
}

static void Netif_Config(void) {
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;	
  
	IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
	IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
	IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
  
	netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
	netif_set_default(&gnetif);
  
	if (netif_is_link_up(&gnetif))
		netif_set_up(&gnetif);
	else
		netif_set_down(&gnetif);
}

static void SystemClock_Config(void) {
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;
	HAL_StatusTypeDef ret = HAL_OK;

	  /* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 432;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 9;

	ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
	if (ret != HAL_OK) {
		while (1) {
			;
		}
	}

	  /* Activate the OverDrive to reach the 216 MHz Frequency */
	ret = HAL_PWREx_EnableOverDrive();
	if (ret != HAL_OK) {
		while (1) {
			;
		}
	}
  
	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
	if (ret != HAL_OK) {
		while (1) {
			;
		}
	}
}

/**
  * @brief  Configure the MPU attributes as Write Through for SRAM1/2.
  * @note   The Base Address is 0x20010000 since this memory interface is the AXI.
  *         The Region Size is 256KB, it is related to SRAM1 and SRAM2  memory size.
  * @param  None
  * @retval None
  */
static void MPU_Config(void) {
	MPU_Region_InitTypeDef MPU_InitStruct;
  
	/* Disable the MPU */
	HAL_MPU_Disable();

	  /* Configure the MPU attributes as WT for SRAM */
	MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress = 0x20010000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_256KB;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER0;
	MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
	MPU_InitStruct.SubRegionDisable = 0x00;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	  /* Enable the MPU */
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

static void timerInit() {
	multiplier = HAL_RCC_GetHCLKFreq() / 54000000;
}
