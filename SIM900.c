/*******************************************************************************************
 * @name    SIM900 Lib.
 * @brief   This library developed to work with SIM900.
                It initialize the module, sends and receives SMS, and deletes messages
 * @date    12/20/2016 9:09:52 PM
 * @version 1.0
 * @last-modify  05/25/2016
 * @author  Mehdi
 *******************************************************************************************/


#include <string.h>

#include "stm32f4xx_hal.h"

#include "Gen_Def.h"
#include "tm_stm32_usart.h"
#include "tm_stm32_hd44780.h"
#include "tm_stm32_delay.h"

#include "SIM900.h"
#include "AuxLib.h"


char SIM900_buffer[128];    // A common buffer used to read response from SIM900
USART_TypeDef* USART_SIM;


/**
 * @name	SIM900Init
 * @brief 	The function initializes the SIM900 module by sending
 *              "AT" command, get the response and check it out to see if module works fine
 *
 * @author	Mehdi
 *
 * @return	Messages indicates if the module works fine (SIM900_OK) or not (SIM900_TIMEOUT)
 */

int8_t SIM900Init(USART_TypeDef* USARTx)
{

	USART_SIM = USARTx; // Set the USART type

	/* Send test command */
	SIM900Cmd("AT");

    uint16_t i = 0;

    /* Check to see if any response sends back by the module and if so, check the response */
    while(i < 10)
    {
        /* Wait for response from module */
    	if (TM_USART_Gets(USART_SIM,SIM900_buffer,128) == 0)
        {
            i++;
            Delayms(10);
        } else
        {
            /* We got a response that is 6 bytes long
			 Now check it */
            TM_USART_Gets(USART_SIM,SIM900_buffer,128);

            /* Check the response from module */
            return SIM900CheckResponse(SIM900_buffer,"OK",6);
        }
    }
    return SIM900_TIMEOUT;
}


/**
 * @name	SIM900Cmd
 * @brief	The function send the given command to the module and
 *              check if the uC receives any feedback from module within 100 ms
 *
 * @author  Mehdi
 *
 * @param	cmd    The command wanted to send to module
 * @return	Messages indicates if the module works fine (SIM900_OK) or not (SIM900_TIMEOUT)
 */

int8_t SIM900Cmd(const char *cmd)
{

    uint16_t i = 0;

	/* Send Command */
	TM_USART_Puts(USART_SIM,cmd);

	/* Send CR (\r) */
    TM_USART_Putc(USART_SIM,0x0D);

    /* Get the length of the cmd sent to module */
    uint8_t len = strlen(cmd);

    /* Add 1 for trailing CR added to the cmd */
    len++;

    /* After sending cmd, wait for the response from module */
    while (i < 10 * len)
    {
        if(TM_USART_Gets(USART_SIM,SIM900_buffer,128) == 0)
        {
            i++;
            Delayms(10);
        } else
        {
        	TM_USART_Gets(USART_SIM,SIM900_buffer,128);

            return SIM900_OK;
        }
    }

    return SIM900_TIMEOUT;
}


/**
 * @name	SIM900CheckResponse
 * @brief	The function check the response received from module
 *              and return back appropriate feedback.
 *
 * @author	Mehdi
 *
 * @param	response                 the response get from module and check by the function
 * @param   check                    the word with witch "response" is compared
 * @param   len                      length of "response" should be compare with "check"
 * @param   SIM900_INVALID_RESPONSE  if two first or two last char of response are not "\n"
 * @return  SIM900_FAIL              if "response" does not match "check"
 */

int8_t SIM900CheckResponse(const char *response, const char *check, uint8_t len)
{
    len -= 2;

    /* Check leading CR LF (\r\n) */
    if (response[0] != 0x0D | response[1] != 0x0A)
        return SIM900_INVALID_RESPONSE;

    /* Check leading CR LF (\r\n) */
    if (response[len] != 0x0D | response[len+1] != 0x0A)
        return SIM900_INVALID_RESPONSE;

    for (uint8_t i = 2; i < len; i++)
    {
        if(response[i] != check[i-2])
            return SIM900_FAIL;
    }

    return SIM900_OK;
}


/**
 * @name	SIM900WaitForResponse
 * @brief	The function add a given delay to receive response from module,
 *              and if receives any response, return number of char response
 *
 * @author	Mehdi
 *
 * @param	timeout     the amount of time (milisec) uC waits
 * @return  Number of char of response
 */

int8_t SIM900WaitForResponse(uint16_t timeout)
{
    uint8_t i = 0;
    uint16_t n = 0;

    while(1)
    {
        while (TM_USART_Gets(USART_ESP,SIM900_buffer,128) == 0 && n < timeout)
        {
            n++;
            Delayms(1);
        }

        if (n == timeout)
            return 0;
        else
        {
        	/* Get the length of recieved char */
        	i = LenString(SIM900_buffer);
        	return i;
        }
    }
}


/**
 * @name	SIM900GetNetStat
 * @brief	The function fetches network state.
 *
 * @author	Mehdi
 *
 * @return  SIM900_FAIL     if "response" does not match "check"
 */

int8_t SIM900GetNetStat()
{
    SIM900Cmd("AT+CREG?");

    uint16_t i = 0;

    /* Correct response is 20 byte long, So wait until we have got 20 bytes in buffer. */
    while(i < 10)
    {
        /* Read the USART buffer */
        TM_USART_Gets(USART_ESP,SIM900_buffer,128);

        if (LenString(SIM900_buffer) < 20)
        {
            i++;
            _delay_ms(10);
        } else
        {
            if(SIM900_buffer[11] = '1')
                return SIM900_NW_REGISTERED_HOME;
            else if(SIM900_buffer[11] = '2')
                return SIM900_NW_SEARCHING;
            else if(SIM900_buffer[11] = '5')
                return SIM900_NW_REGISTED_ROAMING;
            else
                return SIM900_NW_ERROR;
        }
    }

    /* We waited so long but got no response, So tell caller that we timed out */
	return SIM900_TIMEOUT;
}


/**
 * @name	SIM900DeleteMsg
 * @brief	The function deletes the message its number given by user.
 *
 * @author	Mehdi
 *
 * @param	msgNum      No. of message should be deleted.
 * @return	messages shows the function successfully deleted the message (SIM900_OK),
 *          fail to carry out the request (SIM900_FAIL). and did not get any response from module (SIM900_TIMEOUT)
 */

int8_t SIM900DeleteMsg(uint8_t msgNum)
{

    /* Clear pending data in queue */
    USART_RxBufferFlush();

    char cmd[16];   // String for storing the command to be sent

    sprintf(cmd,"AT+CMGD=%d",msgNum);   // AT+CMGD=<n> in which "n" is No. of message

    SIM900Cmd(cmd);

    uint8_t len = SIM900WaitForResponse(1000);

    if (len == 0)
         return SIM900_TIMEOUT;

    SIM900_buffer[len - 1] = '\0';      //!!!!!!! Check the ARM library to see if it is necessary

    /* Check if the response is OK */
    if (strcasecmp(SIM900_buffer+2,"OK") == 0)
        return SIM900_OK;
    else
        return SIM900_FAIL;
}


/**
 * @name	SIM900WaitForMsg
 * @brief	The function waits for the message, and when it is received by module,
 *          the function return the slot in SIM in which the incoming message stores.
 *
 * @author	Mehdi
 *
 * @param	id (Out)       The number of the slot in SIM message stores in
 */

int8_t SIM900WaitForMsg(uint8_t *id)
{
    uint8_t len = SIM900WaitForResponse(250);   // Get the length of received response

    if (len == 0)
        return SIM900_TIMEOUT;

    SIM900_buffer[len - 1] = '\0';  //!!!!!!! Check the ARM library to see if it is necessary

    if (strcasecmp(SIM900_buffer+2,"+CMTI:",6) == 0)
    {
        char str_id[4];

        char *start;

        start = strchr(SIM900_buffer,',');  // Find the first "," in the string and put the following char in start
        start++;

        strcpy(str_id,start);

        *id = atoi(str_id);

        return SIM900_OK;
    } else
        return SIM900_FAIL;
}


/**
 * @name	SIM900ReadMsg
 * @brief	The reads the message its number given by the user.
 * 		    The command that is used to read a text message from any slot is AT+CMGR=<n>
 * 		    where <n> is an integer value indicating the sms slot to read. As I have already
 * 		    discussed that their are several slots to hold incoming messages.
 *
 * 		    The response is like this
 *
 *          +CMGR: "STATUS","OA",,"SCTS"<CR><LF>Message Body<CR><LF><CR><LF>OK<CR><LF>
 *
 * 		    where STATUS indicate the status of message it could be REC UNREAD or REC READ
 *  	    OA is the Originating Address that means the mobile number of the sender.
 * 		    SCTS is the Service Center Time Stamp.
 *
 * @author	Mehdi
 *
 * @param	msgNum (In)     The data (char) get to Transmit to through USART
 * @param	msg (Out)       the message sent to the module
 */

int8_t SIM900ReadMsg(uint8_t msgNum, char *msg)
{

    USART_RxBufferFlush();    // Clear pending data in queue

    char cmd[16];

    // Build command string
    sprintf(cmd,"AT+CMGR=%d",msgNum);

    /* Send the command to read the Msg */
    SIM900Cmd(cmd);

    uint8_t len = SIM900WaitForResponse(1000);

    if (len == 0)
        return SIM900_TIMEOUT;

	/* Check of SIM NOT Ready error */
    if (strcasecmp(SIM900_buffer+2,"+CMS ERROR: 517")==0)
    {
        return SIM900_SIM_NOT_READY;    // SIM NOT Ready
    }


    if (strcasecmp(SIM900_buffer+2,"OK")==0)    // Msg Slot Empty
    {
        return SIM900_MSG_EMPTY;
    }

    /* Now read the actual msg text */
    len = SIM900WaitForResponse(1000);

    if (len == 0)
        return SIM900_TIMEOUT;

    SIM900_buffer[len-1] = '\0';  //!!!!!!! Check the ARM library to see if it is necessary
    strcpy(msg,SIM900_buffer+1); // +1 for removing trailing LF of prev line

    return SIM900_OK;

}


/**
 * @name	SIM900SendMsg
 * @brief	The function send a given message to given phone number via the module, then return message returned.
 *
 * @author	Mehdi
 *
 * @param	num (In)        Phone number to which the message send ex "+919XXXXXXX"
 * @param 	msg (In)        Message Body ex "This a message body"
 * @param   msg_ref (Out)   After successful send, the function stores a unique message reference in this variable.
 */

int8_t SIM900SendMsg(const char *num, const char *msg, uint8_t *msg_ref)
{
    /* Clear pending data in queue */
    USART_RxBufferFlush();

    char cmd[25];

    /* Creating cmd AT+CMGS="+919XXXXXXX" */
    sprintf(cmd,"AT+CMGS= %s",num);

    cmd[8] = 0x22;  // Add " to cmd in 8th char

    uint8_t n = strlen(cmd);

    cmd[n] = 0x22;      // Add " to cmd last char
    cmd[n+1] = '\0';

    /* Send the command to send the Msg */
    SIM900Cmd(cmd);

    Delayms(100);

    /* Send the Msg */
    TM_USART_Puts(USART_SIM,msg);

    TM_USART_Puts(USART_SIM,0x1A);

    /* Wait until Msg is recieved completely */
    while((TM_USART_Gets(USART_ESP,SIM900_buffer,128) == 0) && (LenString(SIM900_buffer) < (strlen(msg)+5)));

    uint8_t len = SIM900WaitForResponse(6000);

    if (len == 0)
        return SIM900_TIMEOUT;

    SIM900_buffer[len-1] = '\0';    ///!!! Check the ARM library to see if it is necessary

    if(strcasecmp(SIM900_buffer+2,"CMGS:",5) == 0)
    {
        *msg_ref = atoi(SIM900_buffer+8);

        TM_USART_ClearBuffer(USART_SIM);     // Clear pending data in queue

        return SIM900_OK;
    } else
    {
    	TM_USART_ClearBuffer(USART_SIM);  // Clear pending data in queue
        return SIM900_FAIL;
    }
}
