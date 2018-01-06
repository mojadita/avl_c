/* $Id: crc.h,v 2.1 2005/11/05 17:51:35 luis Exp $
 * AUTHOR: Luis Colorado <lc@luiscoloradosistemas.com>
 * DATE: Fri Nov  4 12:06:21 CET 2005
 *
 * Disclaimer:
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* Do not include anything BEFORE the line below, as it would not be
 * protected against double inclusion from other files
 */
#ifndef CRC_H
#define CRC_H

#include <stdlib.h>

static char CRC_H_RCSId[] = "\n$Id: crc.h,v 2.1 2005/11/05 17:51:35 luis Exp $\n";

/* constants */
#define CRC_TABLE_SIZE	256
#define CRC_BYTE_SIZE	8
#define CRC_BYTE_MASK	0xff

/* types */
typedef unsigned char CRC_BYTE;
typedef unsigned long CRC_STATE;
typedef CRC_STATE CRC_TABLE[CRC_TABLE_SIZE];

/* prototypes */
CRC_STATE do_crc(
	CRC_STATE state,
	CRC_BYTE *buff,
	size_t nbytes,
	CRC_TABLE table);

CRC_STATE add_crc(
	CRC_STATE state,
	CRC_BYTE *buff,
	size_t nbytes,
	CRC_TABLE table);

#endif /* CRC_H */
/* Do not include anything AFTER the line above, as it would not be
 * protected against double inclusion from other files.
 */

/* $Id: crc.h,v 2.1 2005/11/05 17:51:35 luis Exp $ */
