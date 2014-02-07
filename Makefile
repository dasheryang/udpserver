.PHONY: all clean proto

all: epstat proto worklib

clean:
	rm -f ./*.o
	rm -f ./*.a 
	
	rm -f ./proto/*.h
	rm -f ./proto/*.cc
	
	rm -f ./credis/*.o
	rm -f ./credis/*.a
proto:
	/usr/local/protobuf/bin/protoc -I./proto --cpp_out=./proto ./proto/stat_msg.proto
worklib:
	gcc -c ./credis/rediswork.c ./credis/hiredis/*.c -I./credis/hiredis/
	ar -r libcppcache.a  rediswork.o hiredis.o async.o sds.o net.o
	

epstat: proto worklib
	c++ -Wall -g stat.cpp ./proto/stat_msg.pb.cc -I/usr/local/src/boost -I./proto -I/usr/local/protobuf/include/ -L/usr/local/protobuf/lib/ -lprotobuf ./*.a /usr/local/src/boost/stage/lib/*.a  -o epstat
