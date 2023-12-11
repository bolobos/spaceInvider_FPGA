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

struct TDevice;
struct TDevice {
	int 			m_iDescIo;
	const char 	m_strPathDev[62];
};

#endif // #if !defined __PRJOLED_H__


