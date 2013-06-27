subdir=`find . -type f -iname *.c -print`
all:
	gcc $(subdir) -o mission_control -ljnxc -g -lmysqlclient
