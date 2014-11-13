#include <stdio.h>
#include <string.h>
#include "crc.h"
#include "crc32ieee8023.h"
#include "fprintbuf.h"

#define DEB(X) __FILE__":%d:%s: " X, __LINE__, __func__

void main_end();

int main()
{
	const size_t data_size = (char *)main_end - (char *)main;
	CRC_BYTE buffer[data_size + sizeof (CRC_STATE)];
	
	printf(DEB("data_size = 0x%x\n"), data_size);

	memcpy(buffer, main, data_size);

	add_crc(0, buffer, sizeof buffer, crc32ieee8023);

	fprintbuf(stdout, sizeof buffer, buffer, DEB(""));

	printf(DEB("do_crc(buffer) -> 0x%08x\n"),
		do_crc(0, buffer, data_size, crc32ieee8023));
	printf(DEB("do_crc(buffer + CRC) -> 0x%08x\n"),
		do_crc(0, buffer, sizeof buffer, crc32ieee8023));

}

void main_end() {}
	
