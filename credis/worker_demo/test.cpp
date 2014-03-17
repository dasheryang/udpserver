#include <stdio.h>
#include <cstdlib>

#include "../rediswork.c"

int main( int argc, char* argv[] ){
	char ip[] = "127.0.0.1";
	short port = 6379;	
	
	char hkey[] = "uk";
	char hentry[] = "10098";
	
	int r = touch_cache_unique_entry( ip, port, hkey, hentry );
	printf( "touch result: %d", r );
	
	return 13;
}
