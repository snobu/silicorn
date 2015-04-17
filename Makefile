all:
	if [ ! -d unicorn-hat ]; then make lib; else echo "unicorn-hat directory exists, skipping make lib..."; fi
	make silicorn

lib:
	git clone https://github.com/pimoroni/unicorn-hat
	make -C unicorn-hat/python/rpi-ws281x/lib

clean:
	-rm -f silicorn

reallyclean:
	-rm -rf silicorn unicorn-hat

silicorn:
	gcc -Wall silicorn.c -o silicorn -I unicorn-hat/python/rpi-ws281x/lib -L unicorn-hat/python/rpi-ws281x/lib -lm -lws2811

profile:
	gcc -pg -Wall silicorn.c -o silicorn -I unicorn-hat/python/rpi-ws281x/lib -L unicorn-hat/python/rpi-ws281x/lib -lm -lws2811
