/*
 * CRC32.h
 *
 *  Created on: 12.11.2013
 *      Author: peter
 */

#ifndef CRC32_H_
#define CRC32_H_

void gen_crc_table(void);
unsigned int update_crc(unsigned int crc_accum, unsigned char *data_blk_ptr, unsigned int data_blk_size);

#endif /* CRC32_H_ */
