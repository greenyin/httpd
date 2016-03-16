INCLUDE=-I/home/yinli/httpd/sql_connect -I/home/yinli/httpd/sql_connect/mysql_lib/include -L/home/yinli/httpd/sql_connect/mysql_lib/lib -lmysqlclient

.PHONY:all
all:debug_cgi math_cgi show_student_cgi register_cgi

debug_cgi:debug_cgi.c
	gcc -o $@ $^
math_cgi:math_cgi.c
	gcc -o $@ $^
show_student_cgi:show_student_cgi.cpp /home/yinli/httpd/sql_connect/sql_connect.cpp
	g++ -o $@ $^ ${INCLUDE}
register_cgi:register_cgi.cpp /home/yinli/httpd/sql_connect/sql_connect.cpp 
	g++ -o $@ $^ ${INCLUDE}

.PHONY:output
output:all
	cp debug_cgi ../output/htdocs/cgi_bin
	cp math_cgi ../output/htdocs/cgi_bin
	cp show_student_cgi ../output/htdocs/cgi_bin
	cp register_cgi ../output/htdocs/cgi_bin

.PHONY:clean
clean:
	rm -rf debug_cgi math_cgi show_student_cgi register_cgi
