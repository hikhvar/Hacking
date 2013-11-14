/*
 * main.cpp
 *
 *  Created on: 12.11.2013
 *      Author: peter
 */

#include "RC4.h"
#include <stdio.h>
#include <cstring>
#include <string.h>

//######## Formatting output ##########
void printAsHex(unsigned char* message, unsigned int length){
	for(unsigned int i = 0; i < length; i++){
		printf("%02X ", message[i]);
	}
}

void printMessage(unsigned char* message, unsigned int length){
	for(unsigned int i = 0; i < length; i++){
		printf("%c", message[i]);
	}
}

void printMessageForHex(unsigned char* message, unsigned int length){
	for(unsigned int i = 0; i < length; i++){
		printf(" %c ", message[i]);
	}
}
//######## Formatting output end ##########

//######## CRC32 calculation ##########
// CRC parameters (default values are for CRC-32):
const int order = 32;
const unsigned long polynom = 0x4c11db7;
const int direct = 1;
const unsigned long crcinit = 0xffffffff;
const unsigned long crcxor = 0xffffffff;
const int refin = 1;
const int refout = 1;
// internal global values:
unsigned long crcmask;
unsigned long crchighbit;
unsigned long crcinit_direct;
unsigned long crcinit_nondirect;
unsigned long crctab[256];

// subroutines
unsigned long reflect (unsigned long crc, int bitnum) {
	// reflects the lower 'bitnum' bits of 'crc'
	unsigned long i, j=1, crcout=0;

	for (i=(unsigned long)1<<(bitnum-1); i; i>>=1) {
		if (crc & i) crcout|=j;
		j<<= 1;
	}
	return (crcout);
}

void generate_crc_table() {
	// make CRC lookup table used by table algorithms
	int i, j;
	unsigned long bit, crc;

	for (i=0; i<256; i++) {
		crc=(unsigned long)i;
		if (refin) crc=reflect(crc, 8);
		crc<<= order-8;

		for (j=0; j<8; j++) {
			bit = crc & crchighbit;
			crc<<= 1;
			if (bit) crc^= polynom;
		}

		if (refin) crc = reflect(crc, order);
		crc&= crcmask;
		crctab[i]= crc;
	}
}

unsigned long crctablefast (unsigned char* p, unsigned long len) {
	// fast lookup table algorithm without augmented zero bytes, e.g. used in pkzip.
	// only usable with polynom orders of 8, 16, 24 or 32.
	unsigned long crc = crcinit_direct;

	if (refin) crc = reflect(crc, order);

	if (!refin) while (len--) crc = (crc << 8) ^ crctab[ ((crc >> (order-8)) & 0xff) ^ *p++];
	else while (len--) crc = (crc >> 8) ^ crctab[ (crc & 0xff) ^ *p++];

	if (refout^refin) crc = reflect(crc, order);
	crc^= crcxor;
	crc&= crcmask;

	return(crc);
}

unsigned long crctable (unsigned char* p, unsigned long len) {
	// normal lookup table algorithm with augmented zero bytes.
	// only usable with polynom orders of 8, 16, 24 or 32.
	unsigned long crc = crcinit_nondirect;

	if (refin) crc = reflect(crc, order);

	if (!refin) while (len--) crc = ((crc << 8) | *p++) ^ crctab[ (crc >> (order-8))  & 0xff];
	else while (len--) crc = ((crc >> 8) | (*p++ << (order-8))) ^ crctab[ crc & 0xff];

	if (!refin) while (++len < order/8) crc = (crc << 8) ^ crctab[ (crc >> (order-8))  & 0xff];
	else while (++len < order/8) crc = (crc >> 8) ^ crctab[crc & 0xff];

	if (refout^refin) crc = reflect(crc, order);
	crc^= crcxor;
	crc&= crcmask;

	return(crc);
}

unsigned long crcbitbybit(unsigned char* p, unsigned long len) {
	// bit by bit algorithm with augmented zero bytes.
	// does not use lookup table, suited for polynom orders between 1...32.
	unsigned long i, j, c, bit;
	unsigned long crc = crcinit_nondirect;

	for (i=0; i<len; i++) {
		c = (unsigned long)*p++;
		if (refin) c = reflect(c, 8);

		for (j=0x80; j; j>>=1) {
			bit = crc & crchighbit;
			crc<<= 1;
			if (c & j) crc|= 1;
			if (bit) crc^= polynom;
		}
	}

	for (i=0; i<order; i++) {
		bit = crc & crchighbit;
		crc<<= 1;
		if (bit) crc^= polynom;
	}

	if (refout) crc=reflect(crc, order);
	crc^= crcxor;
	crc&= crcmask;

	return(crc);
}

unsigned long crcbitbybitfast(unsigned char* p, unsigned long len) {
	// fast bit by bit algorithm without augmented zero bytes.
	// does not use lookup table, suited for polynom orders between 1...32.
	unsigned long i, j, c, bit;
	unsigned long crc = crcinit_direct;

	for (i=0; i<len; i++) {
		c = (unsigned long)*p++;
		if (refin) c = reflect(c, 8);

		for (j=0x80; j; j>>=1) {
			bit = crc & crchighbit;
			crc<<= 1;
			if (c & j) bit^= crchighbit;
			if (bit) crc^= polynom;
		}
	}

	if (refout) crc=reflect(crc, order);
	crc^= crcxor;
	crc&= crcmask;

	return(crc);
}

void initCRC32(){
	int i;
	unsigned long bit, crc;

	// at first, compute constant bit masks for whole CRC and CRC high bit
	crcmask = ((((unsigned long)1<<(order-1))-1)<<1)|1;
	crchighbit = (unsigned long)1<<(order-1);

	// check parameters
	if (order < 1 || order > 32) {
		printf("ERROR, invalid order, it must be between 1..32.\n");
	}

	if (polynom != (polynom & crcmask)) {
		printf("ERROR, invalid polynom.\n");
	}

	if (crcinit != (crcinit & crcmask)) {
		printf("ERROR, invalid crcinit.\n");
	}

	if (crcxor != (crcxor & crcmask)) {
		printf("ERROR, invalid crcxor.\n");
	}

	// generate lookup table
	generate_crc_table();

	// compute missing initial CRC value
	if (!direct) {
		crcinit_nondirect = crcinit;
		crc = crcinit;
		for (i=0; i<order; i++) {

			bit = crc & crchighbit;
			crc<<= 1;
			if (bit) crc^= polynom;
		}
		crc&= crcmask;
		crcinit_direct = crc;
	} else {
		crcinit_direct = crcinit;
		crc = crcinit;
		for (i=0; i<order; i++) {

			bit = crc & 1;
			if (bit) crc^= polynom;
			crc >>= 1;
			if (bit) crc|= crchighbit;
		}
		crcinit_nondirect = crc;
	}
}

unsigned int calcCRC32(unsigned char* message, unsigned int length){
	//now ones
//	return crcbitbybit((unsigned char *)message, length);
//	return crcbitbybitfast((unsigned char *)message, length);
//	return crctable((unsigned char *)message, length);
	return crctablefast((unsigned char *)message, length);
}
//######## CRC32 calculation end ##########

int main(int args, char** argv){
	//initializing crc32 algorithm
	initCRC32();

	//initializing the given input parameters
//	unsigned char iv[] = {0x54, 0xD5, 0xFE};
//	unsigned char wepKey[] = {0x00, 0x6E, 0xF8, 0xC0, 0xDE};
	unsigned char combinedKey[] = {0x54, 0xD5, 0xFE, 0x00, 0x6E, 0xF8, 0xC0, 0xDE};
	unsigned int keyLen = sizeof(combinedKey)/sizeof(unsigned char);
	unsigned char message[] = "login;authentication=user";
	unsigned int messageLen = sizeof(message)/sizeof(unsigned char)-1;
	unsigned int messageCRC;
	unsigned int messageWithCRCLen = messageLen + 4;
	unsigned char messageWithCRC[messageWithCRCLen];
	messageWithCRC[messageWithCRCLen] = '\0';
	rc4_key_t key;

	//create CRC
	strcpy((char*)messageWithCRC, (char*)message);
	messageWithCRC[messageWithCRCLen-4] = 0;
	messageWithCRC[messageWithCRCLen-3] = 0;
	messageWithCRC[messageWithCRCLen-2] = 0;
	messageWithCRC[messageWithCRCLen-1] = 0;
	messageCRC = calcCRC32(messageWithCRC, messageLen);

	//combine the message with crc (little endian)
	messageWithCRC[messageWithCRCLen-4] = *(((unsigned char*)&messageCRC)+3);
	messageWithCRC[messageWithCRCLen-3] = *(((unsigned char*)&messageCRC)+2);
	messageWithCRC[messageWithCRCLen-2] = *(((unsigned char*)&messageCRC)+1);
	messageWithCRC[messageWithCRCLen-1] = *(((unsigned char*)&messageCRC)+0);
	//big endian would be this
//	*((unsigned int*)(&messageWithCRC[messageWithCRCLen-4])) = messageCRC;

	//output the data (with crc)
	printf("Cleartext:\t\t");
	printMessageForHex(messageWithCRC, messageLen);
	printf("[crc]\n");
	printf("Before:\t\t\t");
	printAsHex(messageWithCRC, messageWithCRCLen);
	printf("\n");

	//encryption
	rc4_set_key(combinedKey, keyLen, &key);
	rc4_crypt(messageWithCRC, messageWithCRCLen, &key);
	printf("Encrypted message:\t");
	printAsHex(messageWithCRC, messageWithCRCLen);
	printf("\n");

	//change cyphertext to root
	printf("### Changing encrypted message from user to root, recalculating the crc code! ###\n");
	unsigned char fakeMessage[messageWithCRCLen];
	for(unsigned int i = 0; i < messageLen; i++) fakeMessage[i] = message[i];
	fakeMessage[messageWithCRCLen - 8] = 'r';
	fakeMessage[messageWithCRCLen - 7] = 'o';
	fakeMessage[messageWithCRCLen - 6] = 'o';
	fakeMessage[messageWithCRCLen - 5] = 't';
	fakeMessage[messageWithCRCLen - 4] = 0;
	fakeMessage[messageWithCRCLen - 3] = 0;
	fakeMessage[messageWithCRCLen - 2] = 0;
	fakeMessage[messageWithCRCLen - 1] = 0;
	unsigned int messageCRC2 = calcCRC32(fakeMessage, messageLen);
	//little endian
	fakeMessage[messageWithCRCLen-4] = *(((unsigned char*)&messageCRC)+3) ^ *(((unsigned char*)&messageCRC2)+3);
	fakeMessage[messageWithCRCLen-3] = *(((unsigned char*)&messageCRC)+2) ^ *(((unsigned char*)&messageCRC2)+2);
	fakeMessage[messageWithCRCLen-2] = *(((unsigned char*)&messageCRC)+1) ^ *(((unsigned char*)&messageCRC2)+1);
	fakeMessage[messageWithCRCLen-1] = *(((unsigned char*)&messageCRC)+0) ^ *(((unsigned char*)&messageCRC2)+0);
	//big endian
//	*((unsigned int*)(&fakeMessage[messageWithCRCLen-4])) = messageCRC ^ messageCRC2;

	//apply changes to cyphertext
	fakeMessage[messageWithCRCLen - 8] = 'u' ^ 'r';
	fakeMessage[messageWithCRCLen - 7] = 's' ^ 'o';
	fakeMessage[messageWithCRCLen - 6] = 'e' ^ 'o';
	fakeMessage[messageWithCRCLen - 5] = 'r' ^ 't';
	for(unsigned int i = messageWithCRCLen - 8; i < messageWithCRCLen; i++){
		messageWithCRC[i] ^= fakeMessage[i];
	}

	printf("Encrypted message:\t");
	printAsHex(messageWithCRC, messageWithCRCLen);
	printf("\n");

	//decryption
	rc4_key_t keyDecrypt;
	rc4_set_key(combinedKey, keyLen, &keyDecrypt);
	rc4_crypt(messageWithCRC, messageWithCRCLen, &keyDecrypt);

	printf("Decrypted message:\t");
	printAsHex(messageWithCRC, messageWithCRCLen);
	printf("\n");
//	printf("Cleartext (last 4 are crc): %s\n", messageWithCRC);
	printf("Cleartext:\t\t");
	printMessageForHex(messageWithCRC, messageLen);
	printf("[crc]\n");

	//check crc
	unsigned int oldCRC;
	//little endian
	*(((unsigned char*)&oldCRC)+3) = messageWithCRC[messageWithCRCLen-4];
	*(((unsigned char*)&oldCRC)+2) = messageWithCRC[messageWithCRCLen-3];
	*(((unsigned char*)&oldCRC)+1) = messageWithCRC[messageWithCRCLen-2];
	*(((unsigned char*)&oldCRC)+0) = messageWithCRC[messageWithCRCLen-1];
	//big endian
//	oldCRC = *((unsigned int*)&messageWithCRC[messageWithCRCLen-4]);
	messageWithCRC[messageWithCRCLen-4] = 0;
	messageWithCRC[messageWithCRCLen-3] = 0;
	messageWithCRC[messageWithCRCLen-2] = 0;
	messageWithCRC[messageWithCRCLen-1] = 0;
	messageCRC = calcCRC32(messageWithCRC, messageLen);
	if(oldCRC != messageCRC) printf("Wrong CRC codes: old=%X vs new=%X!", oldCRC, messageCRC);
	else printf("Received Correct CRC codes: received = %X calculated = %X", oldCRC, messageCRC);

	return 0;
}

