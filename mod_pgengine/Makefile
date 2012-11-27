all: mod_pgengine.so

mod_pgengine.so: 
	sudo /usr/local/httpd/bin/apxs -iac \
    mod_pgengine.c \
    mod_pgengine_pagemngr.c \
    mod_pgengine_util.c \
		mod_pgengine_dbd.c

clean:
	$(shell sudo rm -rf *.so *~ *.o *.a *.la *.lo *.slo .libs)
