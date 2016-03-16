#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void math(char *const data_string)
{
	if(!data_string){
		return;
	}
	char *data1 = NULL;
	char *data2 = NULL;
	char *start = data_string;
	while(*start != '\0'){
		if(*start == '=' && data1 == NULL){
			data1=start+1;
			start++;
			continue;
		}
		if(*start == '&'){
			*start = '\0';
		}
		if(*start == '=' && data1 != NULL){
			data2 = start+1;
			break;
		}
		start++;
	}
	int int_data1 = atoi(data1);
	int int_data2 = atoi(data2);
	int res1 = int_data1 + int_data2;
	int res2 = int_data1 - int_data2;
	int res3 = int_data1 * int_data2;
	float res4 = (float)int_data1 / (float)int_data2;
	printf("<p>math [add]  :%d + %d = %d</p>\n",int_data1,int_data2,res1);
	printf("<p>math [sub]  :%d - %d = %d</p>\n",int_data1,int_data2,res2);
	printf("<p>math [multi]:%d * %d = %d</p>\n",int_data1,int_data2,res3);
	printf("<p>math [div]  :%d / %d = %f</p>\n",int_data1,int_data2,res4);
}
int main()
{
	int content_length = -1;
	char method[1024];
	char query_string[1024];
	char post_data[4096];
	memset(method,'\0',sizeof(method));
	memset(query_string,'\0',sizeof(query_string));
	memset(post_data,'\0',sizeof(post_data));

	printf("<html>\n");
	printf("</head>Math</head>\n");
	printf("<body>\n");
	strcpy(method,getenv("REQUEST_METHOD"));
	if(strcasecmp("GET",method) == 0){
		strcpy(query_string,getenv("QUERY_STRING"));
		math(query_string);
	}
	if(strcasecmp("POST",method) == 0){
		content_length = atoi(getenv("CONTENT_LENGTH"));
		int i = 0;
		for(;i < content_length;i++){
			read(0,&post_data[i],1);
		}
		post_data[i] = '\0';
		math(post_data);
	}else{
		//do nothing
		return 1;
	}

	printf("</body>\n");
	printf("</html>\n");
	return 0;
}
