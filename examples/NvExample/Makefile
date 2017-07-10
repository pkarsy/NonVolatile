# This Makefile needs arduino-makefile
# sudo apt-get install arduino-mk in any debian based distro
BOARD_TAG     =  uno
MONITOR_PORT  =  /dev/ttyUSB3
include /usr/share/arduino/Arduino.mk
EXTRA_FLAGS += -fshort-enums -g -Wshadow -flto

send: all
	fuser -k -STOP ${MONITOR_PORT} || true
	make do_upload
	fuser -k -CONT ${MONITOR_PORT} || true

terminal:
	gtkterm	-p ${MONITOR_PORT} -s 57600 &

