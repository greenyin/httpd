#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sql_connect.h"
using namespace std;

const string _remote_ip = "127.0.0.1";
const string _remote_user = "root";
const string _remote_passwd = "yinli44";
const string _remote_db = "student";

int main()
{
	int content_length = -1;
	char method[1024];
    char post_data[1024];
	char query_string[1024];
	memset(post_data,'\0',sizeof(post_data));
	memset(query_string,'\0',sizeof(query_string));
	memset(method,'\0',sizeof(method));

	cout<<"<html>"<<endl;
	cout<<"<head>show student</head>"<<endl;
	cout<<"<body>"<<endl;

	char name[64];
	char age[64];
	char school[64];
	char hobby[64];
	memset(name,'\0',sizeof(name));
	memset(age,'\0',sizeof(age));
	memset(school,'\0',sizeof(school));
	memset(hobby,'\0',sizeof(hobby));

	strcpy(method,getenv("REQUEST_METHOD"));
	cout<<"<p>method:"<<method<<"</p>"<<endl;
	if(strcasecmp("GET",method) == 0){
		strcpy(query_string,getenv("QUERY_STRING"));
	    cout<<"<p>query_string:"<<query_string<<"</p>"<<endl;
	    sscanf(query_string,"name=%[a-z]&age=%[0-9]&school=%[a-z]&hobby=%[a-z]",name,age,school,hobby);
	}else{
	    content_length = atoi(getenv("CONTENT_LENGTH"));
	    int i = 0;
	    for(;i<content_length;i++){
	    	read(0,&post_data[i],1);
	    }
	    post_data[i] = '\0';
        cout<<"<p>post_data:"<<post_data<<"</p>"<<endl;
	    sscanf(post_data,"name=%[a-z]&age=%[0-9]&school=%[a-z]&hobby=%[a-z]",name,age,school,hobby);
	}

	cout<<"<p>name:"<<name<<"</p>"<<endl;
	cout<<"<p>age:"<<age<<"</p>"<<endl;
	cout<<"<p>school:"<<school<<"</p>"<<endl;
	cout<<"<p>hobby:"<<hobby<<"</p>"<<endl;

	string insert_data = "\"";
	insert_data += name;
	insert_data += "\",\"";
	insert_data += age;
	insert_data += "\",\"";
	insert_data += school;
	insert_data += "\",\"";
	insert_data += hobby;
	insert_data += "\"";
	cout<<"<p>"<<insert_data<<"</p>"<<endl;
	const string _host = _remote_ip;
	const string _user = _remote_user;
	const string _passwd = _remote_passwd;
	const string _db = _remote_db;
	sql_connecter conn(_host,_user,_passwd,_db);
	conn.begin_connect();
	conn.insert_sql(insert_data);

	cout<<"</body>"<<endl;
	cout<<"</html>"<<endl;
}
