VPATH = .:../include:../lib
SYNTAX = rfc8259

all: libjson.so

libjson.so: json.cpp $(SYNTAX).lex.c $(SYNTAX).tab.c
	c++ -g -shared -fPIC $^ -I../include -o ../lib/$@

$(SYNTAX).lex.c: $(SYNTAX).l
	flex -o $@ $^

$(SYNTAX).tab.c: $(SYNTAX).y
	bison -rall -d $^

clean:
	@rm -f $(SYNTAX).lex.* $(SYNTAX).tab.* *.output ../lib/libjson.so
