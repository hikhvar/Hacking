/*
 * main.cpp
 *
 *  Created on: 12.11.2013
 *      Author: peter
 */

#include "RC4.h"
#include "CRC32.h"
#include <stdio.h>
#include <cstring>


void printAsHex(unsigned char* message, unsigned int length){
	for(unsigned int i = 0; i < length; i++){
		printf("%02X ", message[i]);
	}
}

int main(int args, char** argv){
//	unsigned char iv[] = {0x54, 0xD5, 0xFE};
//	unsigned char wepKey[] = {0x00, 0x6E, 0xF8, 0xC0, 0xDE};
	unsigned char combinedKey[] = {0x54, 0xD5, 0xFE, 0x00, 0x6E, 0xF8, 0xC0, 0xDE};
	unsigned int keyLen = sizeof(combinedKey)/sizeof(unsigned char);
	unsigned char message[] = "login;authentication=user";
	unsigned int messageLen = sizeof(message)/sizeof(unsigned char)-1;
	unsigned int messageCRC = 0xFFFFFFFF;
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
	gen_crc_table();
	messageCRC = update_crc(-1, messageWithCRC, messageWithCRCLen);
	printf("crc: %i\n", messageCRC);

	//combine the message with crc
//	messageWithCRC[messageWithCRCLen-4] = *(((unsigned char*)&messageCRC)+3);
//	messageWithCRC[messageWithCRCLen-3] = *(((unsigned char*)&messageCRC)+2);
//	messageWithCRC[messageWithCRCLen-2] = *(((unsigned char*)&messageCRC)+1);
//	messageWithCRC[messageWithCRCLen-1] = *(((unsigned char*)&messageCRC)+0);
	*((unsigned int*)(&messageWithCRC[messageWithCRCLen-4])) = messageCRC;

	//encryption
	printf("Cleartext: %s\n", messageWithCRC);
	printf("Before:\t\t\t");
	printAsHex(messageWithCRC, messageWithCRCLen);
	printf("\n");

	rc4_set_key(combinedKey, keyLen, &key);
	rc4_crypt(messageWithCRC, messageWithCRCLen, &key);

	printf("Crypted message:\t");
	printAsHex(messageWithCRC, messageWithCRCLen);
	printf("\n");

	//change to root
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
	unsigned int messageCRC2 = update_crc(-1, fakeMessage, messageWithCRCLen);
	*((unsigned int*)(&fakeMessage[messageWithCRCLen-4])) = messageCRC ^ messageCRC2;

	fakeMessage[messageWithCRCLen - 8] = 'u' ^ 'r';
	fakeMessage[messageWithCRCLen - 7] = 's' ^ 'o';
	fakeMessage[messageWithCRCLen - 6] = 'e' ^ 'o';
	fakeMessage[messageWithCRCLen - 5] = 'r' ^ 't';
	for(unsigned int i = messageWithCRCLen - 8; i < messageWithCRCLen; i++){
		messageWithCRC[i] ^= fakeMessage[i];
	}

	//decryption
	rc4_key_t keyDecrypt;
	rc4_set_key(combinedKey, keyLen, &keyDecrypt);
	rc4_crypt(messageWithCRC, messageWithCRCLen, &keyDecrypt);

	printf("Decrypted message:\t");
	printAsHex(messageWithCRC, messageWithCRCLen);
	printf("\n");
	printf("Cleartext: %s\n", messageWithCRC);

	//check crc
	unsigned int oldCRC = *((unsigned int*)&messageWithCRC[messageWithCRCLen-4]);
	messageWithCRC[messageWithCRCLen-4] = 0;
	messageWithCRC[messageWithCRCLen-3] = 0;
	messageWithCRC[messageWithCRCLen-2] = 0;
	messageWithCRC[messageWithCRCLen-1] = 0;
	messageCRC = update_crc(-1, messageWithCRC, messageWithCRCLen);
	if(oldCRC != messageCRC) printf("Wrong CRC codes: old=%X vs new=%X!", oldCRC, messageCRC);
	else printf("Correct CRC codes: old=%X vs new=%X!", oldCRC, messageCRC);

	return 0;
}
