
make:
	gcc creator.c -o creator `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0` -lm -lrt

	gcc producer.c -o producer `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0` -lm -lrt
	
	gcc consumer.c -o consumer `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0` -lm -lrt

	gcc finalizer.c -o finalizer `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0` -lm -lrt
	
