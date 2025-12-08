./bin/test: ./src/test.c ./bin/terminal_launcher.o ./src/terminal_launcher.h
	gcc ./src/test.c ./bin/terminal_launcher.o -o ./bin/test -Wall -Wpedantic -Wextra -Werror

./bin/tmp: ./src/tmp.c ./bin/terminal_launcher.o ./src/terminal_launcher.h
	gcc ./src/tmp.c ./bin/terminal_launcher.o -o ./bin/tmp -Wall -Wpedantic -Wextra -Werror

./bin/terminal_launcher.o: ./src/terminal_launcher.c ./src/terminal_launcher.h
	gcc ./src/terminal_launcher.c -c -o ./bin/terminal_launcher.o -Wall -Wpedantic -Wextra -Werror

test: ./bin/test
	./bin/test

tmp: ./bin/tmp
	./bin/tmp

clean:
	rm ./bin/*
