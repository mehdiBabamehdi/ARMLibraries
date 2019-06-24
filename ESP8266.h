
#ifndef ESP8266_H_
#define ESP8266_H_

/*_________________________________________________________________________________________*/

#include "AuxLib.h"

#include "stm32f4xx_hal.h"

//************************************************

//Error List
#define ESP8266_OK					     1
#define ESP8266_INVALID_RESPONSE		-1
#define ESP8266_FAIL					-2
#define ESP8266_TIMEOUT				    -3


/***************************************************
			F U N C T I O N S
****************************************************/

void ESP_SET(USART_TypeDef* USARTx);

uint8_t ESP_SendCommand (char* Command , uint8_t delayTime);


void ESP_Init(void);

void ESP_ConnectToRouter(void);

void ESP_GetIP(void);

char * ESP_GetResponse (uint8_t Rec_Status, uint8_t delayTime);

char ESP_CheckResponse(const char *response, const char *check);

void ActAcorToRes(char Res);

void ESP_SendHTTPResponse(char* ConnectionID, char* Response);

void ESP_SendCIPData(char* ConnectionID, char* HttpResponse);

void ESP_SendCloseCommand (char* ConnectionID);


#endif /* _ESP8266_H */
