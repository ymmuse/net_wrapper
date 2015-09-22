
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "net_wrapper.h"


/**
	Package Format: 

	[4bytes]  + [4bytes] + [body...bytes]
	  cmd     body-size

 **/

int sendCommand(int sock, int cmd, const void *data, int sz) {
	unsigned char head[8] = {0}; // cmd(4bytes) + body-size(4bytes)
	
	*(int*)head = cmd;
	*(int*)(head+4) = sz;

	int ret;
	ret = writeBytes(sock, (const void*)head, sizeof(head));
	if (ret != 0)
		return ret;

	if (data != NULL && sz != 0)
		ret = writeBytes(sock, (const void*)data, sz);
	
	return ret;
}


void recvHandler(int sock, CommandHandler cmd_callback) {
	int cmd, bodysz;
	unsigned char head[8] = {0};
	const unsigned char *headp = NULL, *bodyp = NULL;

	int bufsz = 4096;
	unsigned char *memp = (unsigned char *)malloc(bufsz);

	while(1) {
		headp = readBytes(sock, (void*)head, sizeof(head));
		if (headp == NULL) {
			break;
		}
		
		cmd = *(int*)headp;
		bodysz = *(int*)(headp+4);

		if (bodysz > bufsz) {
			memp = realloc(memp, bodysz);
			if (memp == NULL) {
				fprintf(stderr,"[recvHandler] cannot realloc mem...\n");
				break;
			}
		}

		bodyp = readBytes(sock, (void*)memp, bodysz);
		if (bodyp == NULL) {
			break;
		}

		if (cmd_callback(sock, cmd, (const void*)bodyp, bodysz) != 0) {
			break;
		}
	}

	free(memp);
}

const unsigned char *readBytes(int sock, void *buf, size_t len)
{
    if (!buf) {
      return NULL;
    }

    size_t res = len;
    ssize_t stat;
    while (res > 0) {
        stat = recv(sock, (char *)(buf) + len - res, res, 0);
        if (stat > 0) {
            res -= stat;
            continue;
        }
        if (stat == 0 || errno != EINTR) { // client shutdown or error
            return NULL;
        }
    }
    return (const unsigned char *)buf;
}

int writeBytes(int sock, const void* buf, size_t size)
{
    size_t res = size;
    int ret = 0;

    while (res > 0) {
        ssize_t stat = send(sock, buf + (size - res), res, 0);
        if (stat < 0) {
            if (errno != EINTR) {
                ret =  stat;
                break;
            }
        } else {
            res -= stat;
        }
    }
    return ret;
}
