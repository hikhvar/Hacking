#include <stdio.h>

#define OFFSET 120

int main() {
	int cookie;
	char buf[99];

	printf("buf: %08x cookie: %08x\n", &buf, &cookie);
//	printf("cookie: %i\n", cookie);
//	printf("pos: %08x val: %08x vali: %i\n", buf + OFFSET, *((int*)(buf + OFFSET)), *((int*)(buf + OFFSET)));
	for(cookie = -108; cookie < 4; cookie += 4){
		printf("pos: %08x val: %08x vali: %i\n", buf + cookie, *((int*)(buf + cookie)), *((int*)(buf + cookie)));
	}
	gets(buf);
//	printf("cookie: %i\n", cookie);
//	printf("pos: %08x val: %08x vali: %i\n", buf + OFFSET, *((int*)(buf + OFFSET)), *((int*)(buf + OFFSET)));
	printf("bla\n");
	for(cookie = -108; cookie < 4; cookie += 4){
		printf("pos: %08x val: %08x vali: %i\n", buf + cookie, *((int*)(buf + cookie)), *((int*)(buf + cookie)));
	}

	if (cookie == 0x000a0d00)
		printf("glückwunsch bernd!\n");
}
