#include "sql_connect.h"

sql_connecter::sql_connecter(
		const string &_host,const string &_user,\
		const string &_passwd,const string &_db)
{
	mysql_base = mysql_init(NULL);
	host = _host;
	user = _user;
	passwd = _passwd;
	db = _db;
	res = NULL;
}

bool sql_connecter::begin_connect()
{
	if(mysql_real_connect(mysql_base,host.c_str(),\
				user.c_str(),passwd.c_str(),\
				db.c_str(),3306,NULL,0) == NULL){
		//cout<<"connect failed..."<<endl;
		return false;
	}else{
		//cout<<"connect success..."<<endl;
		return true;
	}
}

bool sql_connecter::close_connect()
{
	mysql_close(mysql_base);
	//cout<<"connect close"<<endl;
}

bool sql_connecter::select_sql(string field_name[],string _out_str[][5],int &_out_row)
{
	string sql = "SELECT * FROM student_five";
	if(mysql_query(mysql_base,sql.c_str()) == 0){
		cout<<"query success"<<endl;
	}else{
		cerr<<"query failed"<<endl;
		return false;
	}
	res = mysql_store_result(mysql_base);
	int row_num = mysql_num_rows(res);
	_out_row = row_num;
	int field_num = mysql_num_fields(res);

	//cout<<mysql_num_rows(res)<<endl;
	//cout<<mysql_num_fields(res)<<endl;

	MYSQL_FIELD *fd = NULL;
	int i = 0;
	for(;fd = mysql_fetch_field(res);){
		//cout<<fd->name<<"\t";
		field_name[i++] = fd->name;
	}
	//cout<<endl;

	for(int index=0;index<row_num;index++){
	    MYSQL_ROW _row = mysql_fetch_row(res);
		if(_row){
			int start = 0;
			for(;start <field_num;start++){
				_out_str[index][start] = _row[start];
				//cout<<_row[start]<<"\t";
			}
			//cout<<endl;
		}
	}
	return true;
}

bool sql_connecter::insert_sql(const string &data)
{
	string sql = "INSERT INTO student_five(name,age,school,hobby) values";
	sql += "(";
	sql += data;
	sql += ");";

	//cout<<"sql:"<<sql<<endl;
	if(mysql_query(mysql_base,sql.c_str()) == 0)
	{
		cout<<"insert success"<<endl;
		return true;
	}else{
		cerr<<"insert failed"<<endl;
		return false;
	}
}

bool sql_connecter::delete_sql(const string& student_id)
{
	string sql = "DELETE FROM student_five WHERE id=";
	sql += student_id;
	sql += ";";
	
	if(mysql_query(mysql_base,sql.c_str()) == 0)
	{
		cout<<"delete success"<<endl;
		return true;
	}else{
		cerr<<"delete failed"<<endl;
		return false;
	}
}

bool sql_connecter::update_sql(const string& name,const string& age,\
		const string& school,const string& hobby,const string& student_id)
{
	string sql = "UPDATE student_five SET name=";
    sql += "'";
	sql += name;
	sql += "',age='";
	sql += age;
	sql += "',school='";
	sql += school;
	sql += "',hobby='";
	sql += hobby;
	sql += "'";
	sql += " WHERE id='";
	sql += student_id;
	sql += "'";
	sql += ";";
	
	if(mysql_query(mysql_base,sql.c_str()) == 0){
		cout<<"update success"<<endl;
		return true;
	}else{
		cerr<<"update failed"<<endl;
		return false;
	}
}

bool sql_connecter::create_table(const string& table_name)
{
	string sql = "CREATE TABLE ";
	sql += table_name;
	sql += " (name VARCHAR(20),sex CHAR(1));";
	if(mysql_query(mysql_base,sql.c_str()) == 0)
	{
		cout<<"create table success"<<endl;
		return true;
	}else{
		cerr<<"create table failed"<<endl;
		return false;
	}
}
bool sql_connecter::delete_table(const string& table_name)
{
	string sql = "DROP TABLE ";
	sql += table_name;
	sql += ";";
	
	if(mysql_query(mysql_base,sql.c_str()) == 0)
	{
		cout<<"delete table success"<<endl;
		return true;
	}else{
		cerr<<"delete table failed"<<endl;
		return false;
	}
}

sql_connecter::~sql_connecter()
{
	close_connect();
	if(res != NULL){
		free(res);
	}
}

void sql_connecter::show_info()
{
	cout<<mysql_get_client_info()<<endl;
}

#ifdef _DEBUG_

int main()
{
    const string _host = "127.0.0.1";
	const string _user = "root";
	const string _passwd = "yinli44";
	const string _db = "student";
	//const string _data = "\"zhang\",\"22\",\"xigongyuan\",\"reading\"";

	// use update
	const string name = "zhang";
	const string age = "40";
	const string school = "xigongyuan";
	const string hobby = "running";

	string header[5];
	string _sql_data[1024][5];
	int curr_row = -1;
	string id = "1";

	sql_connecter conn(_host,_user,_passwd,_db);
	conn.begin_connect();
	//conn.insert_sql(_data);
	//conn.select_sql(header,_sql_data,curr_row);
	conn.update_sql(name,age,school,hobby,id);
	//conn.delete_sql(id);
	//conn.create_table("new_table");
	//conn.delete_table("new_table");
	return 0;
}

#endif
