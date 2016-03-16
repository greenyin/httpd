#include "httpd.h"

static void usage(const char *proc)
{
	printf("usage : %s [port]\n",proc);
}

void print_debug(const char *msg)
{
#ifdef _DEBUG_
	printf("%s\n",msg);
#endif
}

void print_log(const char* fun,int line,int err_no,const char* err_str)
{
	printf("%s %d %d %s\n",fun,line,err_no,err_str);
}

void clear_header(int client)
{
	char buf[1024];
	memset(buf,'\0',sizeof(buf));
	int ret = 0;
	do{
		ret = get_line(client,buf,sizeof(buf));
	}while(ret>0 && strcmp(buf,"\n")!=0);
}
//return <= --> failed
int get_line(int sock,char* buf,int max_len)
{
	if(!buf || max_len < 0){
		return 0;
	}
	int i = 0;
	int n = 0;
	char c = '\0';
	while(i< max_len-1 && c != '\n'){
		n = recv(sock,&c,1,0);
		if(n > 0){
			if(c == '\r'){
				n = recv(sock,&c,1,MSG_PEEK);
				if(n > 0 && c == '\n'){//windows
					recv(sock,&c,1,0);
				}else{//whatever last is \n
					c = '\n';
				}
			}
			buf[i++] = c;
		}else{
			c = '\n';
		}
	}
	buf[i] = '\0';
	return i;
}

static void bad_request(int client)
{
	char buf[1024];
	sprintf(buf,"HTTP/1.0 400 BAD REQUEST\r\n");
	send(client,buf,strlen(buf),0);
	sprintf(buf,"Content_Type : text/html\r\n");
	send(client,buf,strlen(buf),0);
	sprintf(buf,"\r\n");
	send(client,buf,strlen(buf),0);
	sprintf(buf,"<p> your message is a bad request");
	send(client,buf,strlen(buf),0);
}

static void not_found(int client)
{

}
static void server_error(int client)
{

}
static void server_unavailable(int client)
{

}
void echo_error_to_client(int client,int error_code)
{
	switch(error_code){
		case 400://request error
			bad_request(client);
			break;
		case 404://not found
			not_found(client);
			break;
		case 500://service error
		    server_error(client);
			break;
		case 503://server unavailable
		    server_unavailable(client);
			break;
		default:
			break;
	}
}
void echo_html(int client,const char *path,unsigned int file_size)
{
	if(!path){
		return;
	}
	int in_fd = open(path,O_RDONLY);
	if(in_fd < 0){
		print_debug("open index html error");
		echo_error_to_client(client,404);
		return;
	}

	print_debug("open index html success");
	char echo_line[1024];
	memset(echo_line,'\0',sizeof(echo_line));
	strncpy(echo_line, HTTP_VERSION, strlen(HTTP_VERSION)+1);
	strcat(echo_line, " 200 OK");
	strcat(echo_line, "\r\n\r\n");
	//send(client, echo_line, sizeof(echo_line), 0);//BUG!!
	send(client, echo_line, strlen(echo_line), 0);
	print_debug("send echo head success");
	//printf("%d\n",file_size);
	//printf("%s\n", echo_line);
	if(sendfile(client,in_fd,NULL,file_size) < 0){
		print_debug("send file error");
		echo_error_to_client(client,404);
		close(in_fd);
		return;
	}
	print_debug("send file success");
	close(in_fd);
}

void exe_cgi(int sock_client,const char* path,const char* method,const char*query_string)
{
	char buf[_COMM_SIZE_];
	int numchars = 0;
	int content_length = -1;

	//pipe
	int cgi_input[2] = {0,0};
	int cgi_output[2] = {0,0};
	pid_t id;

	if(strcasecmp(method,"GET") == 0){
		clear_header(sock_client);
	}else{
		//POST
		do{
			memset(buf,'\0',sizeof(buf));
			numchars = get_line(sock_client,buf,sizeof(buf));
			if(strncasecmp(buf,"Content-Length:",strlen("Content-Length:"))==0){
				content_length = atoi(&buf[16]);
			}
		}while(numchars>0&&strcmp(buf,"\n")!=0);
	    if(content_length == -1){
	    	echo_error_to_client(sock_client,404);
	    	return;
	    }
	}

	memset(buf,'\0',sizeof(buf));
	strcpy(buf,HTTP_VERSION);
	strcat(buf," 200 OK\r\n\r\n");
	send(sock_client,buf,strlen(buf),0);

	if(pipe(cgi_input) == -1){
		echo_error_to_client(sock_client,404);
	}
	if(pipe(cgi_output) == -1){
		close(cgi_input[0]);
		close(cgi_input[1]);
		echo_error_to_client(sock_client,404);
		return;
	}

	if((id = fork()) < 0){
		close(cgi_input[0]);
		close(cgi_input[1]);
		close(cgi_output[0]);
		close(cgi_output[1]);
		echo_error_to_client(sock_client,404);
		return;
	}else if(id == 0){//child
		char method_env[_COMM_SIZE_];
		char query_env[_COMM_SIZE_/10];
		char content_len_env[_COMM_SIZE_];
		memset(method_env,'\0',sizeof(method_env));
		memset(query_env,'\0',sizeof(query_env));
		memset(content_len_env,'\0',sizeof(content_len_env));

		close(cgi_input[1]);
		close(cgi_output[0]);

		dup2(cgi_input[0],0);
		dup2(cgi_output[1],1);

		sprintf(method_env,"REQUEST_METHOD=%s",method);
		putenv(method_env);
		if(strcasecmp("GET",method) == 0){//GET
			sprintf(query_env,"QUERY_STRING=%s",query_string);
			putenv(query_env);
		}else{//POST
			sprintf(content_len_env,"CONTENT_LENGTH=%d",content_length);
			putenv(content_len_env);
		}

		execl(path,path,NULL);
		exit(1);

	}else{//father
		close(cgi_input[0]);
		close(cgi_output[1]);

		int i = 0;
		char c = '\0';
		if(strcasecmp("POST",method) == 0){
			for(;i<content_length;i++){
				recv(sock_client,&c,1,0);
				write(cgi_input[1],&c,1);
			}
		}
		while(read(cgi_output[0],&c,1) > 0){
			send(sock_client,&c,1,0);
		}

		close(cgi_input[1]);
		close(cgi_output[0]);

		waitpid(id,NULL,0);
	}
}

void* accept_request(void *arg)
{
	print_debug("get a new client\n");
	pthread_detach(pthread_self());//detach
	int sock_client = (int)arg;

	int cgi = 0;
	char *query_string = NULL;
	char method[_COMM_SIZE_/10];
	char url[_COMM_SIZE_];
	char buffer[_COMM_SIZE_];
	char path[_COMM_SIZE_];
	memset(method,'\0',sizeof(method));
	memset(url,'\0',sizeof(url));
	memset(buffer,'\0',sizeof(buffer));
	memset(path,'\0',sizeof(path));
//#ifdef _DEBUG_
//	while(get_line(sock_client,buffer,sizeof(buffer))>0){
//		printf("%s\n",buffer);
//		fflush(stdout);
//	}
//	printf("\n");
//#endif

	if(get_line(sock_client,buffer,sizeof(buffer)) < 0){
		echo_error_to_client(sock_client,404);
		return (void*)-1;
	}

	int i = 0;
	int j = 0;

	//get method
	while(!isspace(buffer[j]) && i< sizeof(method)-1 && j < sizeof(buffer)){
		method[i] = buffer[j];
		i++;j++;
	}
	//clear space
	while(isspace(buffer[j]) && j < sizeof(buffer)){
		j++;
	}
	//get URL
	i = 0;
	while(!isspace(buffer[j]) && (i < sizeof(url)-1) && (j < sizeof(buffer))){
		url[i] = buffer[j];
		i++;j++;
	}

	print_debug(method);
	print_debug(url);

	if(strcasecmp(method,"GET") && strcasecmp(method,"POST")){
		echo_error_to_client(sock_client,404);
		return (void*)-1;
	}
	if(strcasecmp(method,"POST") == 0){
		cgi = 1;
	}

	if(strcasecmp(method,"GET") == 0){
		query_string = url;
		while(*query_string != '?' && *query_string != '\0'){
			query_string++;
		}
		if(*query_string == '?'){//url =/xxx/xxx + arg
			*query_string = '\0';
			query_string++;
			cgi = 1;
		}
	}

	sprintf(path,"htdocs%s",url);
    if(path[strlen(path)-1] =='/' ){
		strcat(path,MAIN_PAGE);
	}

	print_debug(path);

	struct stat st;
	if(stat(path,&st) < 0){//failed,does not exist
		print_debug("miss cgi");
		clear_header(sock_client);
		echo_error_to_client(sock_client,404);
	}else{//file exist
		if(S_ISDIR(st.st_mode)){
			strcat(path,"/");
			strcat(path,MAIN_PAGE);
		}else if(st.st_mode & S_IXUSR ||\
				 st.st_mode & S_IXGRP ||\
				 st.st_mode & S_IXOTH){
			cgi = 1;
		}else{
		}
		if(cgi){
			printf("enter cgi...\n");
			exe_cgi(sock_client,path,method,query_string);
		}else{
			printf("enter clear header...\n");
			clear_header(sock_client);
			print_debug("begin enter our echo html");
			echo_html(sock_client,path,st.st_size);
		}
	}
	close(sock_client);
	return NULL;
}
static int start(int port)
{
	int listen_sock = socket(AF_INET,SOCK_STREAM,0);
	if(listen_sock == -1){
		print_log(__FUNCTION__,__LINE__,errno,strerror(errno));
		exit(1);
	}

	int flags = 1;
	setsockopt(listen_sock,SOL_SOCKET,SO_REUSEPORT,&flags,sizeof(flags));

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = htonl(INADDR_ANY);

	socklen_t len = sizeof(local);

	if(bind(listen_sock,(struct sockaddr*)&local,len) == -1){
		print_log(__FUNCTION__,__LINE__,errno,strerror(errno));
		exit(2);
	}

	if(listen(listen_sock,_BACK_LOG_) == -1){
		print_log(__FUNCTION__,__LINE__,errno,strerror(errno));
		exit(3);
	}
	return listen_sock;
}

//./httpd port
int main(int argc,char* argv[])
{
	if(argc != 2){
		usage(argv[0]);
		exit(1);
	}

	int port = atoi(argv[1]);
	int sock = start(port);

	struct sockaddr_in client;
	socklen_t len = 0;
	while(1){
		int new_sock = accept(sock,(struct sockaddr*)&client,&len);
		if(new_sock < 0){
			print_log(__FUNCTION__,__LINE__,errno,strerror(errno));
			//printf("error\n");
			continue;
		}
		pthread_t new_thread;
		pthread_create(&new_thread,NULL,accept_request,(void*)new_sock);
	}
	return 0;
}
