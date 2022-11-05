all: serial_setup tones.hex

serial_setup:
	stty -F /dev/ftdi5 raw
	stty -F /dev/ftdi5 9600


tones.hex : tones.asm
	python3 ../2022-Supercon6-Badge-Tools/assembler/assemble.py $<
