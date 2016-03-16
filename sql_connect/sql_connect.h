#ifndef _SQL_CONNECT_
#define _SQL_CONNECT_

#include <iostream>
#include <string>
#include "mysql.h"
using namespace std;

class sql_connecter{
	public:
		sql_connecter(const string &host,const string &user,\
				const string &passwd,const string &db);
		bool begin_connect();
		bool close_connect();
		bool insert_sql(const string &data);
		bool select_sql(string field_name[],string _out_data[][5],int &_out_row);
        bool update_sql(const string& name,const string& age,\
		const string& school,const string& hobby,const string& student_id);
		bool delete_sql(const string& student_id);
		bool delete_table(const string& table_name);
	    bool create_table(const string& table_name);
		~sql_connecter();
		void show_info();
	private:
		MYSQL_RES *res;
		MYSQL *mysql_base;
		string host;
		string user;
		string passwd;
		string db;
};

#endif
