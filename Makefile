all: php2json.so

# CFLAGS = -DDEBUG -ggdb3 -O0
CFLAGS = -Wall -Wextra -O2

.c.o:
	gcc -c -o $@ $(CFLAGS) $(shell python3.6-config --includes) -fPIC $<

.pyx.c:
	cython -3 $<

php2json.so: php2json.o lib/php2json.o
	gcc -o $@ -shared $^

test: php2json.so
	PYTHONPATH=$$PWD pytest -v tests

bench: php2json.so
	python bench.py

gdb: php2json.so
	gdb --args python -m pytest -v test_phpjson.py::test_simple_string

clean:
	rm -f php2json.so *.o lib/*.o php2json.c

.SUFFIXES: .pyx
