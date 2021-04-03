/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 * $Id: webserver-example.c,v 1.1 2006/06/17 22:41:25 adamdunkels Exp $
 */

/**
 * \file
 *         Example showing how to use the web server
 * \author
 *         Adam Dunkels <adam@sics.se>
 *         extended by Marc Burkhardt <fossisoft@googlemail.com>
 */
#include <stdio.h> /* For printf() */
#include <string.h>
#include <stdlib.h>
#include "uip.h"

#include "contiki-net.h"
#include "httpd.h"
#include "httpd-cgi.h"

#include "lib/petsciiconv.h"

#include "webserver-nogui.h"
#include "webserver-strings.c"
#include "tempvalues.h"
#include "mqtt-service.h"

int nMyTemperatur=20;
int nMyTemperaturDecimal=0;

PROCESS(get_temp_process,"get temperature process");

/*---------------------------------------------------------------------------*/
void gettemp()
{
	int* DDRB=(int*) 56579;
	int* CRA=(int*) 56590;
	int* PRB=(int*)56577;
	int* SDR=(int*)56588;
	int* ICR=(int*) 56589;
	int* KEY=(int*)198;
	
	int nValue=0;// read value from SDR register
	int nTemp=0;
	int xPos=0;
	
	//First init all CIA registers
	*DDRB=255;//all lines are output lines
	*CRA=8+64;*CRA=8;//first clear SDR and then set it to input
	*PRB=0;*SDR=0;//empty buffer
	*ICR=127;//Set Interrupt flag
	
	
	// toggle eight times the clock line for eight bits

	for (xPos=0;xPos<8;xPos++){
		*PRB=0;
		*PRB=1;
	}

	//Wait until interrupt is set
	while (*ICR!=0){}
	// getting the byte value from the CIA
	nValue=*SDR;
	// Getting the temperature from the array.	
	nTemp=arrTemp[nValue];
	//Calculate temperature
	*PRB=2; //reset TLC549
	
	nMyTemperatur=nTemp/10;
	nMyTemperaturDecimal=abs(nTemp%10);
	
}

static unsigned short 
make_temp(void *p)
{
	uint16_t numprinted;
	numprinted=snprintf( (char *)uip_appdata, uip_mss(),"%d.%d",nMyTemperatur,nMyTemperaturDecimal);
  return numprinted;
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(MyTemp_thread(struct httpd_state *s, char *ptr))
{
  PSOCK_BEGIN(&s->sout);
  PSOCK_GENERATOR_SEND(&s->sout, make_temp, s->u.ptr);
  PSOCK_END(&s->sout);
}

HTTPD_CGI_CALL(MyTemp, MyTemp_name, MyTemp_thread);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(get_temp_process, ev, data)
{
  static struct etimer timer;
static uip_ipaddr_t server_address;

// Allocate buffer space for the MQTT client
static uint8_t in_buffer[64];
static uint8_t out_buffer[64];
static uint8_t message[64];

// Setup an mqtt_connect_info_t structure to describe
// how the connection should behave
static mqtt_connect_info_t connect_info;
	connect_info.client_id = "c64contiki";
	connect_info.username =NULL;
	connect_info.password = NULL;
	connect_info.will_topic = NULL;
	connect_info.will_message = NULL;
	connect_info.keepalive = 60;
	connect_info.will_qos = 0;
	connect_info.will_retain = 0;
	connect_info.clean_session = 1;
  
  PROCESS_BEGIN();

  // Set the server address
  uip_ipaddr(&server_address, 192,168,1,43);
 
  // Initialise the MQTT client
  mqtt_init(in_buffer, sizeof(in_buffer),
            out_buffer, sizeof(out_buffer));

  // Ask the client to connect to the server
  // and wait for it to complete.
  mqtt_connect(&server_address, UIP_HTONS(1883),
				1, &connect_info);
  PROCESS_WAIT_EVENT_UNTIL(ev == mqtt_event);

  httpd_cgi_add(&MyTemp);

  // Set the server address
  etimer_set(&timer, 5*CLOCK_SECOND); 
  while(1) 
  {
	PROCESS_WAIT_EVENT(); 

	if (ev == PROCESS_EVENT_TIMER)
	{
		//printf("Timer Event\n");
		etimer_reset(&timer);
		gettemp();
        // Relay the received message to a new topic
		if (mqtt_ready()){
 		     sprintf(message,"%d.%d",nMyTemperatur,nMyTemperaturDecimal);
            //printf("temp = %s\n", message);
       	    //printf("publish %d",);
			mqtt_publish("c64temp", message, 0, 1);
		}
	}
  }
  mqtt_disconnect();
  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&webserver_nogui_process,&get_temp_process, &mqtt_process);
/*---------------------------------------------------------------------------*/
