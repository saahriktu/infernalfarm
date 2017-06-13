all:
	cc -o infernalfarm infernalfarm.c -lcurses -lmenu
install:
	cp infernalfarm /usr/local/games/
