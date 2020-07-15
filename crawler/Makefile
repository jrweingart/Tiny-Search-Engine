CFLAGS= -Wall -pedantic -std=c11 -I../utils/ -L../lib -g
LIBS= -lutils -lcurl -lpthread

all: lcrawler crawler

lcrawler: clean
					gcc $(CFLAGS) lcrawler.c $(LIBS) -o $@

crawler: clean
				gcc $(CFLAGS) crawler.c $(LIBS) -o $@

clean:
				rm -f crawler lcrawler
