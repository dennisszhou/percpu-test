obj-m += percpu-test.o

all:
	make -C /data/users/dennisz/percpu M=$(PWD) modules

clean:
	make -C /data/users/dennisz/percpu M=$(PWD) clean
