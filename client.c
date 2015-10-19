#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>

#define MAX_FILEPATH_SIZE 256
#define MAX_BLOCK_SIZE 256
#define PORT 7777

char name_prefix [10] = "download_";

int main(int argc, char **argv)
{
	char file_path[MAX_FILEPATH_SIZE];
	printf("Input file path: ");
	scanf("%s", file_path);
	//char* result_file_name = set_result_file_name(file_path);
	char *file_name;
	char *tokens;
	char *last_token = NULL;
	char* buffer = (char*)malloc(strlen(file_path) + 1);
	strcpy(buffer, file_path);

	tokens = strtok(buffer, "/");

	while (tokens != NULL){
		last_token = (char *)malloc(strlen(tokens) + 1);
		strcpy(last_token, tokens);
		tokens = strtok(NULL, "/");
	}

	file_name = (char *)malloc(strlen(last_token) + strlen(name_prefix) + 1);
	strcpy(file_name, name_prefix);
	strcat(file_name, last_token);
	struct sockaddr_in server_socket;
	int server_socket_id = socket(AF_INET, SOCK_STREAM, 0);

	if (server_socket_id< 0){
		printf("Error occured while openning socket.\n");
		return -1;
	}

	server_socket.sin_family = AF_INET;
	server_socket.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_socket.sin_port = htons(PORT);

	if (connect(server_socket_id, (struct sockaddr *)&server_socket, sizeof(server_socket)) < 0){
		printf("Error occured while trying to set up a connection.\n");
		return -2;
	}

	if (send(server_socket_id, file_path, strlen(file_path), 0) < 0){
		printf("Send failed.\n");
		return -3;
	}

	FILE* file;
	file = fopen(file_name, "wb");
	int bytes_received;
	char data_block[MAX_BLOCK_SIZE];

	while ((bytes_received = read(server_socket_id, data_block, MAX_BLOCK_SIZE)) > 0){
		fwrite(data_block, 1, bytes_received, file);
	}

	if (bytes_received < 0)
		printf("Reading failed.\n");

	printf("Download finished.\n");
	fclose(file);
	close(server_socket_id);
	return 0;
}


