/* ---------------------------------------------------------------------
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ioctlgpio.h
 *
 *   To access by ioctl() user-space API to the GPIO under Linux
 *
 * Copyright (C) 2023 GEii IUT-Acy, USMB.
 *
 * Author:
 * 	Pascal Aygalinc <pascal.aygalinc@univ-smb.fr>
 * ---------------------------------------------------------------------
 */

#if !defined __PRJOLED_H__
#define      __PRJOLED_H__

#include <linux/gpio.h>
#include <poll.h>
#include <stdio.h>   
#include <string.h>  
#include <unistd.h>  
#include <fcntl.h>   
#include <errno.h>   
#include <syslog.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <termios.h>


struct TDevice;
struct TDevice {
	int 			m_iDescIo;
	const char 	m_strPathDev[62];
};

struct TDeviceCharGpIO;
struct TDeviceCharGpIO{
	struct TDevice 			m_DevChar;
	struct gpiochip_info 		m_chipinfo;
	struct gpioline_info 		m_lineinfo;
	struct gpiohandle_request 	m_request;
	struct gpiohandle_data 		m_data;
	struct gpioevent_request  	m_event_request[64];
	struct gpioevent_data     	m_event_data[64];
	struct pollfd 					m_pollfd[64];
};

void InitComp(void);

size_t DateLocale(const char strFormatDate[], char strDate[],size_t sztTaillestrDate);

#endif // #if !defined __PRJOLED_H__


