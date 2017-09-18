// Author barodeur

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <ev.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define PORT_NO 3033
#define BUFFER_SIZE 99999
#define RESPONSE_SIZE 99999

void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
char* load_file(char* input_file_name);

char *ROOT;

int main()
{
  struct ev_loop *loop = ev_default_loop(0);
  ROOT = getenv("PWD");
  int sd;
  struct sockaddr_in addr;
  int addr_len = sizeof(addr);
  struct ev_io w_accept;

  // Create server socket
  if( (sd = socket(PF_INET, SOCK_STREAM, 0)) < 0 )
  {
    perror("socket error");
    return -1;
  }

  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT_NO);
  addr.sin_addr.s_addr = INADDR_ANY;

  // Bind socket to address
  if (bind(sd, (struct sockaddr*) &addr, sizeof(addr)) != 0)
  {
    perror("bind error");
  }

  // Start listing on the socket
  if (listen(sd, 2) < 0)
  {
    perror("listen error");
    return -1;
  }

  // Initialize and start a watcher to accepts client requests
  ev_io_init(&w_accept, accept_cb, sd, EV_READ);
  ev_io_start(loop, &w_accept);

  // Start infinite loop
  while (1)
  {
    ev_loop(loop, 0);
  }

  return 0;
}

char* load_file(char* input_file_name){
  char *file_contents;
  long input_file_size;
  FILE *input_file = fopen(input_file_name, "rb");
  fseek(input_file, 0, SEEK_END);
  input_file_size = ftell(input_file);
  rewind(input_file);
  file_contents = malloc(input_file_size * (sizeof(char)));
  fread(file_contents, sizeof(char), input_file_size, input_file);
  fclose(input_file);
  return file_contents;
}

void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int client_sd;
  struct ev_io *w_client = (struct ev_io*) malloc (sizeof(struct ev_io));

  if(EV_ERROR & revents)
  {
    perror("got invalid event");
    return;
  }

  // Accept client request
  client_sd = accept(watcher->fd, (struct sockaddr *)&client_addr, &client_len);

  if (client_sd < 0)
  {
    perror("accept error");
    return;
  }

  // Initialize and start watcher to read client requests
  ev_io_init(w_client, read_cb, client_sd, EV_READ);
  ev_io_start(loop, w_client);
}

/* Read client message */
void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
  char *reqline[3], data_to_send[RESPONSE_SIZE], path[99999];
  int rcvd, fd, bytes_read;
  char buffer[BUFFER_SIZE];
  ssize_t req;

  memset((void*)buffer, (int)'\0', 99999);
  if(EV_ERROR & revents)
  {
    perror("got invalid event");
    return;
  }

  // Receive message from client socket
  req = recv(watcher->fd, buffer, BUFFER_SIZE, 0);

  if(req < 0)
  {
    perror("read error");
    return;
  }
  else if(req == 0)
  {
    // Stop and free watchet if client socket is closing
    ev_io_stop(loop,watcher);
    free(watcher);
    perror("peer might be closing");
    return;
  }
  else
  {
    printf("buffer %s\n", buffer);
    reqline[0] = strtok (buffer, " \t\n");
    if ( strncmp(reqline[0], "GET\0", 4)==0 )
    {
      reqline[1] = strtok (NULL, " \t");
      reqline[2] = strtok (NULL, " \t\n");
      if (strncmp(reqline[2], "HTTP/1.0", 8) != 0 && strncmp(reqline[2], "HTTP/1.1", 8)!=0)
      {
        send(watcher->fd, "HTTP/1.0 400 Bad Request\n", req, 0);
      }
      else
      {
        if (strncmp(reqline[1], "/\0", 2)==0)
        {
          //Because if no file is specified, index.html will be opened by default
          reqline[1] = "/index.html";
        }

        strcpy(path, ROOT);
        strcpy(&path[strlen(ROOT)], "/html");
        strcpy(&path[strlen("/html") + strlen(ROOT)], reqline[1]);
        printf("file: %s\n", path);
        //FILE FOUND
        if ((fd = open(path, O_RDONLY)) != -1)
        {
          char *header_string = malloc(99999);
          char *body;
          body = load_file(path);
          sprintf(header_string, "HTTP/1.0 200 OK\nContent-Type: text/html\nContent-Length: %ld\nConnection: Closed\n\n", strlen(body));
          strcat(header_string, body);
          // printf("%s", header_string);
          send(watcher->fd, header_string,
                     strlen(header_string), 0);

          while ( (bytes_read=read(fd, data_to_send, RESPONSE_SIZE))>0 )
          {
            send(watcher->fd, data_to_send, bytes_read, 0);
          }
          bzero(data_to_send, bytes_read);
        }
        else
        { //FILE NOT FOUND
          send(watcher->fd, "HTTP/1.0 404 Not Found\n", req, 0);
        }
      }
    }
    ev_io_stop(loop,watcher);
    free(watcher);
    close(watcher->fd);
    perror("stop event");
  }

  bzero(buffer, req);
}
