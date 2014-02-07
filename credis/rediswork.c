#include "fmacros.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>

#include "hiredis.h"

int write_cache_hash( const char* redis_ip, short redis_port, const char* hkey, const char* entry, float value ){

        redisContext *c = redisConnect( redis_ip, redis_port );
        if (c != NULL && c->err) {
            printf("Error: %s\n", c->errstr);
        }

	char cmd_str[800];
	sprintf( cmd_str, "HINCRBYFLOAT  %s %s %f", hkey, entry, value );	

        void* reply = redisCommand( c, cmd_str );

        freeReplyObject(reply);

	return 1;
}

