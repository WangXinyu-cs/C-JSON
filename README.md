# C-JSON
This C++ program realizing JSON serializing and parsing is my salute to Oracle.
Named Json Gadget, this program is a JSON tool implemented by C++ intended for my own use, anyone may use it freely under the LGPL license except China corporations that I have no the least confidence and pleasure to serve.
I tried to follow RFC8259 while coding. One can use it to construct a JSON structure then serialize it to a JSON text, or feed it with a JSON text and parse to restore the JSON structure. Simply including json.h and linking libjson.so, one can manipulate JSON easily.
A programmer may call setPreciaion to adjust number precision before serializing JSON. In case a coder encounters troubles with multiple-bytes character string, consider to set environment variables $LANG or $LC_ALL to some kind of UTF-8 values before trying again.
Reentrance feature is seriously considered and Json Gadget is thread-safe in non-malicious use,the routine parseJson can be executed multiple times concurrently in a single process.
When feeling puzzled to use Json Gadget one can refer to the example test.cpp, which seems to run correctly on my PC.
