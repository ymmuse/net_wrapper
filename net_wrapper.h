
#ifndef NET_WRAPPER_H
#define NET_WRAPPER_H

#include <sys/types.h>

// success will return 0; otherwise is error
typedef int(*CommandHandler)(int sock, int cmd, const void *data, int sz);


void recvHandler(int sock, CommandHandler callback);
int sendCommand(int sock, int cmd, const void *data, int sz);

const unsigned char *readBytes(int sock, void *buf, size_t len);
int writeBytes(int sock, const void* buf, size_t size);


#endif
