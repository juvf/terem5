#ifndef CHECKSUM__H
#define CHECKSUM__H

class Checksum
{
public:
/*
  Name  : CRC-16
  Poly  : 0x8005   x^16 + x^15 + x^2 + 1
  Init  : 0x0000
  Revert: true
  XorOut: 0x0000
  Check : 0x4B37 ("123456789")
  MaxLen: 4095 байт (32767 бит) - обнаружение
    одинарных, двойных, тройных и всех нечетных ошибок
*/
	static unsigned short crc16(const unsigned char * pcBlock, unsigned short len);
};

#endif /*CHECKSUM_H_*/
