main:
	mkdir romfs/
	chmod +x bin/makerom
	chmod +x bin/bannertool
	gcc src/main.c -o ciabuilder
