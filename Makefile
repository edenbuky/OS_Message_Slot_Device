obj-m += message_slot.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default: message_slot message_sender message_reader

message_slot:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

message_sender: message_sender.c
	gcc -O3 -Wall -std=c11 message_sender.c -o message_sender

message_reader: message_reader.c
	gcc -O3 -Wall -std=c11 message_reader.c -o message_reader

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f message_sender message_reader