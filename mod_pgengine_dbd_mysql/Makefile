all: mod_dummy.so

mod_dummy.so: 
	sudo /usr/local/httpd/bin/apxs -iac -I../mod_form \
		   -I../mod_pgengine \
	     -I/usr/include/mysql \
	     -lmysqlclient \
		   mod_pgengine_dbd_mysql.c

clean:
	$(shell sudo rm -rf *.so *~ *.o *.a *.la *.lo *.slo .libs)
