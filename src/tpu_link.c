/*  ti_link - link program for TI calculators
 *  Copyright (C) 1999-2002  Romain Lievin
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* "Ti/Pc USB" link cable unit (my own link cable) */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "cabl_def.h"
#include "export.h"

#if defined(__LINUX__)

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <fcntl.h>
#include <strings.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>

#include "typedefs.h"
#include "export.h"
#include "cabl_err.h"
#include "cabl_def.h"
#include "cabl_ext.h"
#include "logging.h"

/**************/
/* Linux part */
/**************/

int tpu_close();

int tpu_init()
{
  return 0;
}

int tpu_open()
{
  return 0;
}

int tpu_put(byte data)
{
  return 0;
}

int tpu_get(byte *data)
{
  return 0;
}

int tpu_probe()
{
  return 0;
}

int tpu_close()
{
  return 0;
}

int tpu_exit()
{
  return 0;
}

int tpu_check(int *status)
{
  return 0;
}

int tpu_supported()
{
  return SUPPORT_OFF;
}

#elif defined(__WIN32__)

/************************/
/* Windows 32 bits part */
/************************/

/*
	The code below is experimental even if it works fine
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#include "usb_defs.h"

#include "cabl_def.h"
#include "export.h"
#include "cabl_err.h"
#include "cabl_ext.h"
#include "plerror.h"

//extern int time_out; // Timeout value for cables in 0.10 seconds

static struct _lIn lIn;	// PC -> device buffer
static struct _lOut lOut;	// device -> USB buffer
static DWORD nBytes;		// Effective number of returned bytes 

static HANDLE hDevice = 0;

static unsigned char getb ();
static void putb (unsigned char value);

int tpu_init(uint io_addr, char *dev)
{
	// Open the USB device
	hDevice = CreateFile(USB_DEVICE_FILENAME, 
						GENERIC_WRITE | GENERIC_READ,
						FILE_SHARE_WRITE | FILE_SHARE_READ, 
						NULL, OPEN_EXISTING, 0, 0);
	if(!hDevice)
	{
		DISPLAY_ERROR("CreateFile\n");
		print_last_error();
		return ERR_CREATE_FILE;
	}
	putb(3);

	return 0;
}

int tpu_open()
{
	putb(3);

	tdr.count = 0;
	toSTART(tdr.start);
	
	return 0;
}

int tpu_put(byte data)
{
  int bit;
  TIME clk;
  int b;

  tdr.count++;
  for(bit=0; bit<8; bit++)
    {
      if(data & 1)
	{
	  putb(1);
	  toSTART(clk);
	  do 
	    { 
	      if(toELAPSED(clk, time_out)) return ERR_SND_BIT_TIMEOUT;
	    }
	  while((getb() & 1));
	  putb(3);
	  toSTART(clk);
	  do 
	    { 
	      if(toELAPSED(clk, time_out)) return ERR_SND_BIT_TIMEOUT;
	    }
	  while((getb() & 1)==0x00);
	}
      else
	{
	  putb(2);
	  toSTART(clk);
          do 
	    {
	      if(toELAPSED(clk, time_out)) return ERR_SND_BIT_TIMEOUT;
	    }
	  while(getb() & 2);
	  putb(3);
	  // I commented it but this part is necessary in normal function
	  /*
	    toSTART(clk);
	    do 
	    { 
	    if(toELAPSED(clk, time_out)) return ERR_SND_BIT_TIMEOUT;
	    }
	    while((getb() & 2)==0x00);
	  */
        }
      data>>=1;
    }
  
  return 0;
}

int tpu_get(byte *d)
{
  int bit;
  byte data=0;
  byte v;
  int b;
  TIME clk;

  tdr.count++;
  for(bit=0; bit<8; bit++)
    {
      toSTART(clk);
      while((v=getb() & 3) == 3)
	{
	  if(toELAPSED(clk, time_out)) return ERR_RCV_BIT_TIMEOUT;
	}
      if(v == 1)
	{
	  data=(data >> 1) | 0x80;
	  putb(2);
	  while((getb() & 2) == 0x00);
	  putb(3);
	}
      else
	{
	  data=(data >> 1) & 0x7F;
	  putb(1);
	  while((getb() & 1)==0x00);
	  putb(3);
	}
      //for(i=0; i<delay; i++) getb();
	 }
  *d=data;
  printf("get: 0x%02x\n", data);

  return 0;
}

int tpu_close()
{
  putb(3);
  //putb(0);
  //DISPLAY("Port 1: 0x%02x\n", getb());
  
  return 0;
}

int tpu_exit()
{
  return 0;
}

int tpu_probe()
{
  return 0;
}

int tpu_check(int *status)
{
  return 0;
}

unsigned char getb ()
{
	BOOL b;

	// Read port
	lIn.bFunction = DC_READ_PORT;
	lIn.bValue1 = 1;	// Port 0

	// Send device command
	b = DeviceIoControl(hDevice, FNCT_NUMBER, &lIn, sizeof(lIn), &lOut, sizeof(lOut),
						&nBytes, NULL);
	if(!b)
	{
		DISPLAY_ERROR("DeviceIoControl\n");
		print_last_error();
		//return ERR_USB_DEVICE_CMD;
	}
	//DISPLAY("getb: 0x%02x\n", lOut.bValue1);

	return lOut.bValue1;
}

void putb (unsigned char value)
{
	BOOL b;

	//DISPLAY("putb: 0x%02x\n", value);
	// Write port
	lIn.bFunction = DC_WRITE_RAM;
	lIn.bValue1 = PORT1_IN_RAM;
	lIn.bValue2 = value;

	// Send device command
	b = DeviceIoControl(hDevice, FNCT_NUMBER, &lIn, sizeof(lIn), &lOut, sizeof(lOut),
						&nBytes, NULL);
	if(!b)
	{
		DISPLAY_ERROR("DeviceIoControl\n");
		print_last_error();
		//return ERR_USB_DEVICE_CMD;
	}
}

int tpu_supported()
{
  return SUPPORT_ON;
}

#else // unsupported platforms

/************************/
/* Unsupported platform */
/************************/

int tpu_init()
{
  return 0;
}

int tpu_open()
{
  return 0;
}

int tpu_put(byte data)
{
  return 0;
}

int tpu_get(byte *d)
{
  return 0;
}

int tpu_probe()
{
  return 0;
}

int tpu_close()
{
  return 0;
}

int tpu_exit()
{
  return 0;
}

int tpu_check(int *status)
{
  return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1)) // swap the 2 lowest bits

int tpu_set_red_wire(int b)
{
  return 0;
}

int tpu_set_white_wire(int b)
{
  return 0;
}

int tpu_get_red_wire()
{
  return 0;
}

int tpu_get_white_wire()
{
  return 0;
}

int tpu_supported()
{
  return SUPPORT_OFF;
}

#endif

