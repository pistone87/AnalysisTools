CPPFLAGS=-Wall $$(root-config --cflags)
LDFLAGS=$$(root-config --ldflags --libs)

all:  DrawMacro

%: %.o
	g++ $(LDFLAGS) $^ -o $@

%.o: %.C
	g++ $(CPPFLAGS) -c $<

.PHONY: clean

clean:
	rm -f *.o
