
make creator: creator.c
	gcc creator.c -o creator `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0` -lm -lrt
	./creator $(buff_name) $(buff_size)

make producer: producer.c
	gcc producer.c -o producer `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0` -lm -lrt
	./producer $(buff_name) $(t_mean)


make producer: consumer.c
	gcc consumer.c -o consumer `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0` -lm -lrt
	./consumer $(buff_name) $(t_mean)
