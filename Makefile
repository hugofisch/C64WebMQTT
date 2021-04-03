CONTIKI_PROJECT = webserver-example
all: $(CONTIKI_PROJECT)

APPS = webserver

UIP_ACTIVE_OPEN = 1
override webserver_src = webserver-nogui.c httpd.c http-strings.c psock.c memb.c \
                httpd-fs.c httpd-cgi.c uip.c mqtt-msg.c mqtt-service.c
# The webserver application normally contains a built-in file system and support
# for server-side includes.
#
# This webserver example supports building the alternative webserver application
# which serves files from an cfs file system. To build the alternative webserver
# run make with the parameter HTTPD-CFS=1.

CONTIKI = ..\contiki-2.5
include $(CONTIKI)/Makefile.include
