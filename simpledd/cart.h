#ifndef _CART_H
#define _CART_H

/**********************************************************************
 This source code created by Mark A. DeLoura, Nintendo of America Inc.
 Last modification: October 10, 1997

 Purpose:
	Cartridge-specific functions.
 *********************************************************************/

void     cartCopy(const char *src, const char *dest, const int len);
void     cartInitPI(void); 

#endif
