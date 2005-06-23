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

#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>				/* decl of inet_addr()	    */
#include <sys/socket.h>

#include "imonc.h"
#include "md5.h"

#include "debug.h"

static volatile int imonc_socket = -1;
static char * imonc_host_name = NULL;
static int imonc_port = IMOND_DEFAULT_PORT;
static char * imonc_pass = NULL;
static char * imonc_admin_pass = NULL;

static char * imonc_fqdn_name = NULL;

static void send_passwd(void);
static int set_host_name(char * host_name);
static int set_port(int port);
static int set_pass(char * pass);
static int set_admin_pass(char * admin_pass);


/**********************************************************************
 * test the socket handle
 **********************************************************************/
int imonc_test_socket(void)
{
	return imonc_socket;
}
/**********************************************************************/


/**********************************************************************
 * test the socket handle
 **********************************************************************/
char * imonc_get_fqdn_name(int fqdn)
{
	static char buf[128];
	char * c;

	strcpy(buf, "No FQDN");
	
	if(fqdn 
		|| ((imonc_fqdn_name != NULL) && (c = strchr(imonc_fqdn_name, '.')) == NULL))
		return imonc_fqdn_name;

	if(imonc_fqdn_name != NULL) 
	{
		strncpy(buf, imonc_fqdn_name, MIN(c - imonc_fqdn_name, 127));
		buf[c - imonc_fqdn_name] = '\0';
	}

	return(buf);
}
/**********************************************************************/


/**********************************************************************
 * send password to login
 **********************************************************************/
static void send_passwd(void)
{
	char buf[64];

	if(imonc_pass != NULL)
	{
		sprintf(buf, "pass %s", imonc_pass);
		send_command(buf);
		get_answer();
	}

	if(imonc_admin_pass != NULL)
	{
		sprintf(buf, "pass %s", imonc_admin_pass);
		send_command(buf);
		get_answer();
	}

	return;
}
/**********************************************************************/


/**********************************************************************
 * set the host name variable 
 **********************************************************************/
static int set_host_name(char * host_name)
{
	if(imonc_host_name != NULL)
	{
		if(strcmp(imonc_host_name, host_name))
		{
			free(imonc_host_name);
			imonc_host_name = strdup(host_name);
			return -1; 
		}
	}
	else
	{
		if(host_name != NULL)
		{
			imonc_host_name = strdup(host_name);
			return -1; 
		}
	}

	return 0;
}
/**********************************************************************/


/**********************************************************************
 * set the port number variable
 **********************************************************************/
static int set_port(int port)
{
	if(port != imonc_port)
	{
		imonc_port = port;
		return -1;
	}

	return 0;
}
/**********************************************************************/


/**********************************************************************
 * set the password variable
 **********************************************************************/
static int set_pass(char * pass)
{
	if(imonc_pass != NULL)
	{
		if(strcmp(imonc_pass, pass))
		{
			free(imonc_pass);
			imonc_pass = strdup(pass);
			return -1;
		}
	}
	else
	{
		if(pass != NULL)
		{
			imonc_pass = strdup(pass);
			return -1;
		}
	}

	return 0;
}
/**********************************************************************/


/**********************************************************************
 * set the admin password variable
 **********************************************************************/
static int set_admin_pass(char * admin_pass)
{
	if(imonc_admin_pass != NULL)
	{
		if(strcmp(imonc_admin_pass, admin_pass))
		{
			free(imonc_admin_pass);
			imonc_admin_pass = strdup(admin_pass);
			return -1;
		}
	}
	else
	{
		if(admin_pass != NULL)
		{
			imonc_admin_pass = strdup(admin_pass);
			return -1;
		}
	}

	return 0;
}
/**********************************************************************/

/**********************************************************************
 * service_connect(host_name, port)
 * connect to ip = host_name on port = port
 **********************************************************************/
int service_connect(char * host_name, int port, char * pass, char * admin_pass)
{
	struct sockaddr_in addr;
	static struct hostent * host_p;
	static char * last_host_name;
	int opt = 1;
	int reconnect = 0;

	if(set_host_name(host_name))
		reconnect = -1; 

	if(set_port(port))
		reconnect = -1; 

	if(set_pass(pass))
		reconnect = -1;

	if(set_admin_pass(admin_pass))
		reconnect = -1; 
	
	if(imonc_socket >= 0)
	{
		if(reconnect)
			service_disconnect();
		else
			return(imonc_socket);
	}

	memset((char*)&addr, 0, sizeof(addr));

	if((addr.sin_addr.s_addr = inet_addr(imonc_host_name)) == INADDR_NONE)
	{
		if(!host_p || strcmp(last_host_name, imonc_host_name))
		{	
			host_p = gethostbyname(imonc_host_name);

			if(!host_p)
				return(-1);

			if(last_host_name != NULL)
				free(last_host_name);
			
			last_host_name = strdup(imonc_host_name);

			if(imonc_fqdn_name != NULL)
				free(imonc_fqdn_name);

			imonc_fqdn_name = strdup(host_p->h_name);
		}

		memcpy((char*)(&addr.sin_addr),host_p->h_addr, host_p->h_length);
	}
	else
	{
		if(!host_p || strcmp(last_host_name, imonc_host_name))
		{
			host_p = gethostbyaddr(&addr.sin_addr.s_addr, sizeof(struct in_addr), AF_INET);

			if(!host_p)
				return(-2);
			
			if(last_host_name != NULL)
				free(last_host_name);
			
			last_host_name = strdup(imonc_host_name);

			if(imonc_fqdn_name != NULL)
				free(imonc_fqdn_name);

			imonc_fqdn_name = strdup(host_p->h_name);
		}
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons((unsigned short)port);

	if((imonc_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return(imonc_socket);
		
	setsockopt(imonc_socket, IPPROTO_TCP, TCP_NODELAY, (char *)&opt, sizeof(opt));

	alarm(IMONC_CONNECT_TIMEOUT);

	if(connect(imonc_socket, (struct sockaddr*)&addr, sizeof(addr)) != 0)
	{
		close(imonc_socket);
		imonc_socket = -1;
	}

	alarm(0);
	
	if(imonc_socket >= 0)
		send_passwd();
	
	return(imonc_socket);
}
/**********************************************************************/


/**********************************************************************
 * service_disconnect(fd)
 * disconnect from service
 **********************************************************************/
void service_disconnect(void)
{
	if(imonc_socket >= 0)
	{
		close(imonc_socket);
		imonc_socket = -1;
	}
	
	return;
}
/**********************************************************************/


/**********************************************************************
 * service_reconnect(int signo)
 * reconnect the service as signal signo
 **********************************************************************/
void service_reconnect(int signo)
{
	service_disconnect();
		
	if(try_reconnect())		/* actually try to connect again */
		return;
}
/**********************************************************************/


/**********************************************************************
 **********************************************************************/
int try_reconnect(void)
{
	return(1);
}
/**********************************************************************/


/**********************************************************************
 * get_command_answer(command, buffer)
 **********************************************************************/
char * get_command_answer(char * command, char * buffer, int length)
{
	send_command(command);
	strncpy(buffer, get_answer(), length);
	return(buffer);
}
/**********************************************************************/


/**********************************************************************
 * get_command_answer(command, buffer)
 **********************************************************************/
char * get_command_answer_crlf(char * command, char * buffer, int length)
{
	char * answer;
	
	send_command(command);
	answer = get_answer();
	
	if(strlen(answer) > 0)
	{
		strncpy(buffer, answer, length);
		strncat(buffer, "\n", length - strlen(buffer));
	}

	return(buffer);
}
/**********************************************************************/


/**********************************************************************
 * send_command(fd, str)
 * send command str via fd
 **********************************************************************/
int send_command(char * str)
{
	int i;
	int rc;
	char buf[256];
	int len = strlen(str) + 2;

	sprintf(buf, "%s\r\n", str);

	for(i = 0; i < len; i += rc)
	{
		rc = write(imonc_socket, buf + i, len - i);

		if(rc == -1)
			return(-1);
	}

	return(len);
}
/**********************************************************************/


/**********************************************************************
 * get_answer(fd)
 * get answer from fd
 **********************************************************************/
char * get_answer(void)
{
	static char buf[8192];
	char * tmp;
	char c[1];
	int len = 0;
	
	tmp = buf;
	
	alarm(IMONC_READ_TIMEOUT);
	
	while(read(imonc_socket, c, 1) > 0)
	{
		len++;
		
		if(c[0] == '\n')
			break;		/* break at end of line */
		
		if(c[0] != '\r')
			*tmp++ = c[0];
	}
	
	alarm(0);
	
	*tmp = '\0';
	
	if(len <= 0)
	{
		/* connection seems to have died */
		//fli4l_reconnect(0);
		buf[0] = '\0';
		return(buf);
	}
	
	if(!strncmp(buf, "OK ", 3))	/* OK xxxx	    */
	{
		return(buf + 3);
	}
	else if(len > 2 && !strcmp(buf + len - 2, "OK"))
	{
		*(buf + len - 2) = '\0';
		return(buf);
	}
	else if(len == 2 && !strcmp(buf + len - 2, "OK"))
	{
		return(buf);
	}
		
	buf[0] = '\0';
	return(buf);			/* ERR xxxx	    */
}
/**********************************************************************/


/**********************************************************************
 * get_list(fd, command)
 * get answer from fd
 **********************************************************************/
GSList * get_list(char * str)
{
	gchar * buf;
	gchar ** strarr;
	GSList * list = NULL;

	buf = get_buf(str);

	strarr = g_strsplit(buf, "\n", 0);
	
	while(*strarr != NULL)
	{
		if(strncmp(*strarr, "OK", 2) == 0)
			return(list);
		else if(strncmp(*strarr, "ERR", 3) == 0)
			return(NULL);
			
		list = g_slist_append(list, *strarr);
			
		strarr++;
	}

	return(NULL);			/* no ok? */
}
/**********************************************************************/


/**********************************************************************
 * get_buf(fd, command)
 * sends command and allocates a dynamic buffer
 **********************************************************************/
#define	BUFFER_SIZE		8192

gchar * get_buf(char * str)
{
	gchar wbuf[64];
	gint len = strlen (str);
	static gchar * buf;
	gint buffer_size = 0;
	gchar ch[1];
	gint n;
	gint at_bol = 1;		/* flag: at beginning of line   */
	gint last_line = 0;		/* flag: last line              */
	
	sprintf(wbuf, "%s\r\n", str);
	write(imonc_socket, wbuf, len + 2);

	if(buffer_size == 0)
	{
		buffer_size = BUFFER_SIZE;
		buf = malloc(buffer_size);
	}
	
	alarm(IMONC_READ_TIMEOUT);

	for(n = 0; read(imonc_socket, ch, 1) > 0; n++)
	{
		if(ch[0] == '\r')
		{
			n--;
			continue;
		}

		if(n + 1 == buffer_size)
		{
			buffer_size += BUFFER_SIZE;
			buf = realloc(buf, buffer_size);
		}

		buf[n] = ch[0];
		
		if(at_bol && ch[0] != ' ')
		{
			last_line = 1;
			at_bol = 0;
		}
		else if(ch[0] == '\n')
		{
			at_bol = 1;
						
			if(last_line == 1)
			{
				break;
			}
		}
		else
		{   
			at_bol = 0;
		}
	}
	
	alarm(0);

	buf[n] = '\0';

	return(buf);
}
/**********************************************************************/


/**********************************************************************
 * hexmd5(string)
 * A small wrapper to the md5 algorithm to use it
 * on a string and return some hex digits
 **********************************************************************/
char * hexmd5(const char * string)
{
	static unsigned char md5buffer[16];
        static char tmp[33];
        size_t i;

        md5_buffer(string, strlen(string), md5buffer);

        for(i = 0; i < 16; i++)
                sprintf(tmp + (2 * i), "%02x", md5buffer[i]);

        return(tmp);
}
/**********************************************************************/
