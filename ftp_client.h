#ifndef FTP_CLIENT_H
#define FTP_CLIENT_H

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

void handle_message(int);
void ftp_request_loop(int);
bool is_valid_command(char *);
void do_ls(int);

#endif
