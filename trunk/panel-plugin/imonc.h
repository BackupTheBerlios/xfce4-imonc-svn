/*
 * Copyright (c) 2005 J. Busse <jb@dataline-gmbh.de>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <errno.h>
#include <stdio.h>

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include <gtk/gtk.h>

#define IMOND_DEFAULT_PORT	5000
#define TELMOND_DEFAULT_PORT	5001

#define IMONC_MAX_CHANNELS	2
#define IMONC_MAX_DIRECTIONS	2

#define IMONC_DIRECTION_IN	0
#define IMONC_DIRECTION_OUT	1

#define	IMONC_MAX_ISDN_TX	8000.0

#define IMONC_READ_TIMEOUT	5	/* timeout for broken tcp/ip connections */
#define IMONC_CONNECT_TIMEOUT	2	/* timeout to wait for connect */

#define IMONC_ACK_STRING	"\006"	/* ACK = 0x06		    */
#define IMONC_NAK_STRING	"\025"	/* NAK = 0x15		    */
#define IMONC_ACK_CHAR		'\006'	/* ACK = 0x06		    */
#define IMONC_NAK_CHAR		'\025'	/* NAK = 0x15		    */


extern volatile int imonc_socket;

int imonc_test_socket(void);
char *imonc_get_fqdn_name(int fqdn);
int service_connect(char *host_name, int port, char *pass, char *admin_pass);
void service_disconnect(void);
void service_reconnect(int signo);
int try_reconnect(void);
char *get_command_answer(char *command, char *buffer, int length);
char *get_command_answer_crlf(char *command, char *buffer, int length);
int send_command(char *str);
char *get_answer(void);
char *get_lastcall(int telmond_port);
GSList *get_list(char *str);
gchar *get_buf(char *str);

char *hexmd5(const char *string);
