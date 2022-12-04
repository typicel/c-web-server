#include "server.h"

int send_response(int fd, char* header, char* content_type, void* body, int content_length){
    char response[MAX_RESPONSE_SIZE];
    int tot_length, rv;

    strcpy(response, header);
    strcat(response, content_type);
    strcat(response, body);
    strcat(response, "\r\n");

    tot_length = content_length + strlen(header) + strlen(content_type);
    rv = send(fd, response, tot_length, 0);
    if(rv < 0){
        perror("webserver (send_response)");
    }

    return rv;
}


void resp_404(int fd){
    char filepath[MAX_DIR_LENGTH];
    struct filedata* file;
    char* mime_type;

    // Find 404.html
    snprintf(filepath, sizeof filepath, "%s/404.html", SERVER_FILES);
    file = file_load(filepath);
    mime_type = get_mime_type(filepath);

    int rv = send_response(fd, "HTTP/1.1 404 NOT FOUND\r\n" , mime_type, file->data, file->size);
    if(rv < 0){
        printf("Error sending response\n");
        return;
    }

    file_free(file);
}


void handle_http_request(int fd, struct sockaddr_in client_addr){
    char buffer[BUFFER_SIZE], filepath[MAX_DIR_LENGTH];
    struct filedata* file;
    int value = read(fd, buffer, BUFFER_SIZE);
    if(value < 0){
        perror("webserver (read)");
        return;
    }

    // Parse the request
    char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
    sscanf(buffer, "%s %s %s", method, uri, version);
    printf("[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), method, version, uri);

    // Check the URI, and grab the requested file
    if(strcmp(uri, "/") == 0){
        snprintf(filepath, sizeof filepath, "%s/index.html", SERVER_FILES);
    } else {
        snprintf(filepath, sizeof filepath, "%s%s", SERVER_FILES, uri);
    }

    // Get the correct file based on the uri
    file = file_load(filepath);
    if(file == NULL){
        free(file);
        resp_404(fd);
        return;
    }

    char* mime_type = get_mime_type(filepath);

    // Write response to the socket
    int rv = send_response(fd, "HTTP/1.1 200 OK\r\n", mime_type, file->data, file->size);
    if(rv < 0){
        perror("Error sending response\n");
        return;
    }
    file_free(file);

    return;
}

void* thread_function(void* arg){
    thread_info* t_info = (thread_info*)arg;
    struct sockaddr_in client_addr;
    int client_addrlen = sizeof(client_addr);

    int* pclient = malloc(sizeof(int));
    while(1){
        array_get(t_info->socket_numbers, pclient);

        int socketn = getsockname(*pclient, (struct sockaddr*)&client_addr, (socklen_t*)&client_addrlen);
        if(socketn < 0){
            perror("[T]: webserver (getsocketname)");
            continue;
        }

        handle_http_request(*pclient, client_addr);
        close(*pclient);

    }
    free(pclient);
}

thread_info* thread_init(array* socket_numbers){
    thread_info* new_thread_info = malloc(sizeof(thread_info));
    pthread_t tid;

    new_thread_info->socket_numbers = socket_numbers;

    if(pthread_create(&tid, NULL, thread_function, new_thread_info) != 0){
        perror("Failed to initialize thread");
        free(new_thread_info);
        exit(1);
    }

    new_thread_info->tid = tid;
    return new_thread_info;
}

int main(void){

    thread_info* thread_pool[THREAD_POOL_SIZE];
    array socket_nums;
    struct sockaddr_in host_addr;
    int socketfd, host_addrlen = sizeof(host_addr);

    array_init(&socket_nums);

    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketfd == -1){
        perror("webserver (socket)\n");
        return 1;
    }

    // Bind the socket
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
 
    if(bind(socketfd, (struct sockaddr*)&host_addr, host_addrlen) != 0) {
        perror("webserver (bind)");
        return 1;
    }
    printf("Socket bound on port %d\n", PORT);

    // Listen
    if(listen(socketfd, SOMAXCONN) != 0){
        perror("webserver (listen)");
        return 1;
    }

    for(int i = 0; i < THREAD_POOL_SIZE; i++){
        thread_info* new_thread = thread_init(&socket_nums);
        thread_pool[i] = new_thread;
    }

    // Accept new connections
    while(1){
        int newsocketfd = accept(socketfd, (struct sockaddr*)&host_addr, (socklen_t*)&host_addrlen);

        if(newsocketfd < 0){
            perror("webserver (accept)");
            continue;
        }

        array_put(&socket_nums, newsocketfd);
        printf("File descriptor: %d\n", newsocketfd);
    }

    return 0;
}