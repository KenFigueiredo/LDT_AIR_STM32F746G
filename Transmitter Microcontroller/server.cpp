#include "stm32f7xx_hal.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "cmsis_os.h"
#include "ethernetif.h"

#include <string.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>


using namespace std;
#ifdef __cplusplus
extern "C" {
#endif

	void SysTick_Handler(void);
	void ETH_IRQHandler(void);
	void SysTick_Init(void);
	
#ifdef __cplusplus
}
#endif

static TIM_HandleTypeDef microTimer = { 
	.Instance = TIM5
};

extern "C" void TIM5_IRQHandler() {
	HAL_TIM_IRQHandler(&microTimer);
}

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
static void initGPIO();
static void timerInit();
char* split_string(char* str, char* delim);
void sendData(char * data, int len);
void delayMicros(uint32_t del);
void sync();
void sendMessage(string s);
void sendMultiFile(char* data, int len, int parts, int total, string path);
void laseByteArray(char* in, int count, int count2, int len);
void laseByteArray(string in, int count, int count2, int len);
void laseByte(char in, int count);
void laseByte(int in, int count);

#define EXITID		0x35
#define ONEWAYID 	0x33
#define MULTFID 	0x55
#define ZERO		0
#define SEVEN		7

uint32_t microTicks, multiplier, bitDelay = 10, frameDelay = bitDelay + 2, mDel = 9, mfDel = mDel * 2;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	microTicks++;
}

int main(void) {
	MPU_Config();
	SCB_EnableICache();
	SCB_EnableDCache();
	HAL_Init(); 
	SystemClock_Config();
	initGPIO();
	timerInit();
	
	/*
			string s = "Hello World!";
			int i = 0, j = 7, pktSize = s.length();
			uint32_t bitDelay = 5, frameDelay = 10;
			for (;;) {	
				GPIOC->BSRR = 0x0040;
				delayMicros(delay);
				GPIOC->BSRR = 0x0040 << 16;
				delayMicros(delay);
		
				while (i < pktSize) {
					while (j >= 0) {
						if (((s.at(i) >> j) & 1) == 1) GPIOC->BSRR = 0x0040; //GPIO_PIN_7 ON;
						else GPIOC->BSRR = 0x0040 << 16;	//GPIO_PIN_7 OFF;
						j--;
						delayMicros(bitDelay);
					}							
					i += 1;
					j = 7;
					//Send sync code
					GPIOC->BSRR = 0x0040; //GPIO_PIN_7 ON;
					delayMicros(frameDelay);
					GPIOC->BSRR = 0x0040 << 16; //GPIO_PIN_7 OFF;		
				}
		
				i = 0;
			}
	*/
	osThreadDef(Start, MainThread, osPriorityRealtime, 0, configMINIMAL_STACK_SIZE * 5);
	osThreadCreate(osThread(Start), NULL);
  
	osKernelStart();
	
	/* We should never get here as control is now taken by the scheduler */
	for (;;) {
		;
	}
}

void delayMicros(uint32_t del) {
	//uint32_t x = microTicks;
	//while ((microTicks - x) <= del){;}
	del = del * multiplier - 10;
	while (del--)
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

static void MainThread(void const * argument) { 
		
	struct netconn *pListeningConnection, *pAcceptedConnection;
	err_t err;
		
	tcpip_init(NULL, NULL);
	Netif_Config();
	int portNum = 5419;
	
	gnetif.ip_addr.addr = (unsigned)1342220480; //192.168.0.80
	gnetif.netmask.addr = (unsigned)16777215; //255.255.255.0
	gnetif.gw.addr = (unsigned)16820416; //192.168.0.1
	
	/*
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
	*/
	
	pListeningConnection = netconn_new(NETCONN_TCP);
	if (!pListeningConnection)
		asm("bkpt 255");
	err = netconn_bind(pListeningConnection, NULL, portNum);
    
	if (err != ERR_OK)
		asm("bkpt 255");
	netconn_listen(pListeningConnection);

	bool connected = false;
	char* data;
	unsigned short len;
	string ack = "ACK\n";
	string unblock = "BLKRLS\n";
	GPIOG->BSRR = 0x0080;

	for (;;) {		
		if (connected) {
			// FIRST CONNECT ROUTINE ****************************
			stringstream ss;
			struct netbuf *inbuf = NULL;
			//cout << "Client connected. ";
			GPIOG->BSRR = 0x0080 << 16;
			GPIOC->BSRR = 0x0040;
			
	// Wait for client to send their credentials
			err = netconn_recv(pAcceptedConnection, &inbuf);
			if (err != ERR_OK) asm("bkpt 255");
			len = netbuf_len(inbuf);
			data = new char[len];
			netbuf_copy(inbuf, data, len);
			netbuf_free(inbuf);
			netbuf_delete(inbuf);

			// Once buffer is read, split the string to get the user name
			string str(split_string(data, "\n"));
			delete []data;
			
//send the acknowledgement of the connection back to the user.
			//cout << "IP: " << str << endl;
			str = "Hello " + str;
			netconn_write(pAcceptedConnection, (const unsigned char*)str.c_str(), str.length(), NETCONN_NOCOPY);

			while (connected) {
		// TRANSMISSION OUTPUT ROUTINE ***************************

			//Wait for user console to send its next command and get it
				err = netconn_recv(pAcceptedConnection, &inbuf);
				if (err != ERR_OK) asm("bkpt 255");	
				netconn_write(pAcceptedConnection, (const unsigned char*)ack.c_str(), ack.length(), NETCONN_NOCOPY); //send ACK		
				len = netbuf_len(inbuf);
				data = new char[len];
				netbuf_copy(inbuf, data, len);
				netbuf_free(inbuf);
				netbuf_delete(inbuf);

				string st(split_string(data, "\r")); // store the next command in a string for easier comparison
				delete []data;

			//If a multipart file needs to be sent, start this routine.
				if (st.compare("MULTIFILE") == 0) {

				// Wait for file path then get it from the ethernet buffer
					err = netconn_recv(pAcceptedConnection, &inbuf);
					if (err != ERR_OK) asm("bkpt 255");
					netconn_write(pAcceptedConnection, (const unsigned char*)ack.c_str(), ack.length(), NETCONN_NOCOPY); //send ACK				
					len = netbuf_len(inbuf);
					data = new char[len];
					netbuf_copy(inbuf, data, len);
					netbuf_free(inbuf);
					netbuf_delete(inbuf);		

													
					string path(split_string(data, "\n")); // store the file path in a string for later use
					delete []data;

					// Wait for num of parts of the then get it from the ethernet buffer								
					err = netconn_recv(pAcceptedConnection, &inbuf);
					if (err != ERR_OK) asm("bkpt 255");	
					netconn_write(pAcceptedConnection, (const unsigned char*)ack.c_str(), ack.length(), NETCONN_NOCOPY); //send ACK			
					len = netbuf_len(inbuf);
					data = new char[len];
					netbuf_copy(inbuf, data, len);
					netbuf_free(inbuf);
					netbuf_delete(inbuf);		
									
					int parts = atoi(data);
					delete []data;
						
				// Alert the console that the laser has started
					GPIOG->BSRR = 0x0040;
					GPIOC->BSRR = 0x0080 << 16;
					string str2 = "Starting Laser, file: " + path + "\n";
					cout << str2;
					netconn_write(pAcceptedConnection, (const unsigned char*)str2.c_str(), str2.length(), NETCONN_NOCOPY);					
					
				// Lase packets	
					for (int i = 1; i <= parts; i++) {
						// wait for buffer readin to start lasing
						err = netconn_recv(pAcceptedConnection, &inbuf); 
						if (err != ERR_OK) asm("bkpt 255");
						netconn_write(pAcceptedConnection, (const unsigned char*)ack.c_str(), ack.length(), NETCONN_NOCOPY); //send ACK	
						len = netbuf_len(inbuf);
						data = (char *)pvPortMalloc(len); //new char[len];
						netbuf_copy(inbuf, data, len);
						netbuf_free(inbuf);
						netbuf_delete(inbuf);
						sendMultiFile(data, len, i, parts, path);
						vPortFree(data); 

						cout << unblock;
						netconn_write(pAcceptedConnection, (const unsigned char*)unblock.c_str(), unblock.length(), NETCONN_NOCOPY); //send unblock
					}	
								
				//Alert console that the laser has completed										
					str = "OPCOMPLETE\n";
					//cout << str;
					netconn_write(pAcceptedConnection, (const unsigned char*)str.c_str(), str.length(), NETCONN_NOCOPY);
									
					GPIOG->BSRR = 0x0040 << 16;		
				}
						
			//If the payload is a simply ascii text message, start this routine
				else if (st.compare("1WAYMSG") == 0) {

					// Wait for payload then get it from the ethernet buffer
					err = netconn_recv(pAcceptedConnection, &inbuf);
					if (err != ERR_OK) asm("bkpt 255");

					len = netbuf_len(inbuf);
					data = new char[len];
					netbuf_copy(inbuf, data, len);
					netconn_write(pAcceptedConnection, (const unsigned char*)ack.c_str(), ack.length(), NETCONN_NOCOPY);
					netbuf_free(inbuf);
					netbuf_delete(inbuf);

					string s(split_string(data, "\n"));
					delete []data;

				// Alert the console that the laser has started
					GPIOG->BSRR = 0x0040;
					GPIOC->BSRR = 0x0080 << 16;
					string str2 = "Starting Laser, message: " + s;
					netconn_write(pAcceptedConnection, (const unsigned char*)str2.c_str(), str2.length(), NETCONN_NOCOPY);
										
				// send message to receiver
					char oneWay = ONEWAYID, pktSize = s.length();
					int x = 7;
					uint32_t val;

				// Send sync
					GPIOC->BSRR = 0x0080;
					HAL_Delay(frameDelay);
					GPIOC->BSRR = 0x0080 << 16;

					while (x >= 0) {	
						if ((oneWay >> x) & 1) GPIOC->BSRR = 0x0080;
						else GPIOC->BSRR = 0x0080 << 16;
						x--;
						HAL_Delay(bitDelay);
					}

				//laseByte(oneWay, SEVEN); // Send ONEWAYID keyword	
					sync(); //Send Sync code

					laseByte(pktSize, SEVEN); //Send pkt size
					sync(); // Send sync

					laseByteArray(s, ZERO, SEVEN, pktSize); //Send payload		
						
					GPIOC->BSRR = 0x0080 << 16;	//GPIO_PIN_7 OFF;

				//Alert console that the laser has completed										
					str = "OPCOMPLETE\n";
					netconn_write(pAcceptedConnection, (const unsigned char*)str.c_str(), str.length(), NETCONN_NOCOPY);
									
					GPIOG->BSRR = 0x0040 << 16;	
				}
						
			//If the payload is the EXIT OPcode then gracefully disconnect from the client.
				else if (st.compare("EXIT") == 0) {
					connected = false;
					GPIOG->BSRR = 0x0080;
					GPIOC->BSRR = 0x0040 << 16;
				}

				else if (st.compare("ALIGN") == 0) {
					HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_6);
					HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
				}
						
			//Else no other OP codes so it must be an error.
				else {
					//cout << "ERROR: No packet identifier" << endl;
				}
			}
			//printf("Client disconnected\n");
			netconn_close(pAcceptedConnection);
			netconn_delete(pAcceptedConnection);
		}
		err = netconn_accept(pListeningConnection, &pAcceptedConnection);
		if (err == ERR_OK) connected = true;
	}
}

void sendMessage(string s) {
	char oneWay = ONEWAYID, pktSize = s.length();
	int x = 7, y = 7, i = 0, j = 7;
	uint32_t val;

	//sync(); // Send sync
	//laseByte(oneWay, SEVEN); // Send ONEWAYID keyword	
	//sync(); //Send Sync code
	//laseByte(pktSize, SEVEN); //Send pkt size
	//sync(); // Send sync
	//laseByteArray(s, ZERO, SEVEN, pktSize); //Send payload		


	GPIOC->BSRR = 0x0080;
	HAL_Delay(frameDelay);
	GPIOC->BSRR = 0x0080 << 16;

	while (x >= 0) {	
		if ((oneWay >> x) & 1) GPIOC->BSRR = 0x0080;
		else GPIOC->BSRR = 0x0080 << 16;	
		x--;
		HAL_Delay(bitDelay);
	}

	GPIOC->BSRR = 0x0080;
	HAL_Delay(frameDelay);
	GPIOC->BSRR = 0x0080 << 16;
	
	while (y >= 0) {	
		if ((pktSize >> y) & 1) GPIOC->BSRR = 0x0080;
		else GPIOC->BSRR = 0x0080 << 16;	
		y--;
		HAL_Delay(bitDelay);
	}

	GPIOC->BSRR = 0x0080;
	HAL_Delay(frameDelay);
	GPIOC->BSRR = 0x0080 << 16;

	while (i < pktSize) {
		while (j >= 0) {
			if ((s.at(i) >> j) & 1) GPIOC->BSRR = 0x0080;
			else GPIOC->BSRR = 0x0080 << 16;
			j--;
			HAL_Delay(bitDelay);
		}

		i++;
		j = 7;

		GPIOC->BSRR = 0x0080;
		HAL_Delay(frameDelay);
		GPIOC->BSRR = 0x0080 << 16;
	}
				
	GPIOC->BSRR = 0x0080 << 16;	//GPIO_PIN_7 OFF;
}

void sendMultiFile(char* data, int len, int parts, int total, string path) {
	char multF = MULTFID, pktSize = len, curPart = parts, totPart = total;
	int zero = 0, seven = 7, pathSize = path.size();
	uint32_t val = 0, bitDelay = 5, frameDelay = bitDelay * 2;
	bool first = !(parts ^ 1);

	//Send Sync code
	sync();
	
//If this is the first packet for the file, send basic info for receiver.
	if (first) {			
		laseByte(multF, seven); // Send MULTIFILE keyword		
		sync(); //Send Sync code
	
		laseByte(pathSize, seven); // Send filename size
		sync(); // send sync code
		laseByteArray(path, zero, seven, pathSize); // send filename
		sync(); //Send Sync code

		laseByte(totPart, seven); // Send total number of pkts
		sync(); //Send Sync code
	}	
	//Send current packet segment id, size and payload
	laseByte(curPart, seven); //Send cur part
	sync(); //Send Sync code
		
	laseByte(pktSize, seven); //Send pkt size	
	sync(); //Send Sync code

	laseByteArray(data, zero, seven, pktSize); //Send payload
						
			//GPIOC->BSRR = 0x0080 << 16;	//GPIO_PIN_7 OFF;
}

void laseByteArray(char* in, int count, int count2, int len) {
	
	while (count < len) {
		while (count2 >= 0) {
			if (((in[count] >> count2) & 1) == 1) GPIOC->BSRR = 0x0080;
			else GPIOC->BSRR = 0x0080 << 16;
			count2--;
			HAL_Delay(bitDelay);
		}

		count++;
		count2 = 7;	
		sync(); //Send Sync code
	}

}

void laseByteArray(string in, int count, int count2, int len) {
	
	while (count < len) {
		while (count2 >= 0) {
			if ((in.at(count) >> count2) & 1) GPIOC->BSRR = 0x0080;
			else GPIOC->BSRR = 0x0080 << 16;
			count2--;
			HAL_Delay(bitDelay);
		}

		count++;
		count2 = 7;
		sync(); //Send Sync code
	}

}

void laseByte(char in, int count) {
	while (count >= 0) {	
		if ((in >> count) & 1) GPIOC->BSRR = 0x0080;
		else GPIOC->BSRR = 0x0080 << 16;
		count--;
		HAL_Delay(bitDelay);
	}
}

void laseByte(int in, int count) {
	while (count >= 0) {
		if ((in >> count) & 1) GPIOC->BSRR = 0x0080;
		else GPIOC->BSRR = 0x0080 << 16;
		count--;
		HAL_Delay(bitDelay);
	}
}

void sync() {
	GPIOC->BSRR = 0x0080;
	HAL_Delay(frameDelay);
	GPIOC->BSRR = 0x0080 << 16;
}

char* split_string(char* str, char* delim) {
	char* split = strtok(str, delim);

	if (split != NULL) {
		return split;
	}

	else
		return str;
}

static void initGPIO() {
	__GPIOC_CLK_ENABLE();
	__GPIOG_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_Init;
	GPIO_Init.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Init.Speed =  GPIO_SPEED_HIGH;
	GPIO_Init.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_Init);
	HAL_GPIO_Init(GPIOG, &GPIO_Init);
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
	/*
		__TIM5_CLK_ENABLE();
		microTimer.Init.Prescaler = 26; // TIMCntCLK = TIMCLK / (Prescaler + 1) : 54MHz 
		microTimer.Init.CounterMode = TIM_COUNTERMODE_UP;
		microTimer.Init.Period = 10; // increase counter every 10 ticks
		microTimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // no extra clk division
		microTimer.Init.RepetitionCounter = 0;
		microTicks = 0;
		HAL_TIM_Base_Init(&microTimer);
		HAL_TIM_Base_Start_IT(&microTimer);

		HAL_NVIC_SetPriority(TIM5_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(TIM5_IRQn);
		//printf("%d \n", );
	*/
	
	multiplier = HAL_RCC_GetHCLKFreq() / 54000000;
}


