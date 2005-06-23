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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wrapper.h"
#include "debug.h"


/* admin commands */

void imonc_addlink(int ci_index)
{
	char cmd[128], buffer[128];

	sprintf(cmd, "addlink %d", ci_index);
	get_command_answer(cmd, buffer, sizeof(buffer));
}


void imonc_adjust_time(int seconds)
{
	char cmd[128], buffer[128];

	sprintf(cmd, "adjust-time %d", seconds);
	get_command_answer(cmd, buffer, sizeof(buffer));
}


void imonc_delete(char *filename, char *password)
{
	char cmd[1024], buffer[128];

	if(strlen(filename) + strlen(password) < sizeof(cmd))
	{
		sprintf(cmd, "delete %s %s", filename, password);
		get_command_answer(cmd, buffer, sizeof(buffer));
	}
}


int imonc_hup_timeout(int ci_index, int value)
{
	char cmd[128], buffer[128];
	int timeout;

	if(value >= 0)
		sprintf(cmd, "hup-timeout %d %d", ci_index, value);
	else
		sprintf(cmd, "hup-timeout %d", ci_index);
	
	get_command_answer(cmd, buffer, sizeof(buffer));
	sscanf(buffer, "%d", &timeout);
	return timeout;
}


void imonc_removelink(int ci_index)
{
	char cmd[128], buffer[128];

	sprintf(cmd, "removelink %d", ci_index);
	get_command_answer(cmd, buffer, sizeof(buffer));
}


void imonc_reset_telmond_log_file(void)
{
	char buffer[128];

	get_command_answer("reset-telmond-log-file", buffer, sizeof(buffer));
}


void imonc_reset_imond_log_file(void)
{
	char buffer[128];

	get_command_answer("reset-imond-log-file", buffer, sizeof(buffer));
}


void imonc_receive(char *filename, int bytes, char *password)
{
	char cmd[1024], buffer[128];

	if(strlen(filename) + strlen(password) < sizeof(cmd))
	{
		sprintf(cmd, "receive %s %d %s", filename, bytes, password);
		get_command_answer(cmd, buffer, sizeof(buffer));
		// If ACK then transfer file in blocks of 1024 bytes
		// !!!!! send file to router
	}
}


char *imonc_send(char *filename, char *password)
{
	// !!!!! receive file from router
	return NULL;
}


char *imonc_support(char *password)
{
	// !!!!! receive support info
	return NULL;
}


void imonc_sync(void)
{
	char buffer[128];

	get_command_answer("sync", buffer, sizeof(buffer));
}


/* admin or user commands */

void imonc_dial(void)
{
	char buffer[128];

	get_command_answer("dial", buffer, sizeof(buffer));
}


t_imonc_dialmode imonc_dialmode(t_imonc_dialmode mode)
{
	static t_imonc_dialmode dialmode;

	return dialmode;
}


void imonc_disable(void)
{
	char buffer[128];

	get_command_answer("disable", buffer, sizeof(buffer));
}


void imonc_enable(void)
{
	char buffer[128];

	get_command_answer("enable", buffer, sizeof(buffer));
}


void imonc_halt(void)
{
	char buffer[128];

	get_command_answer("halt", buffer, sizeof(buffer));
}


void imonc_hangup(void)
{
	char buffer[128];

	get_command_answer("hangup", buffer, sizeof(buffer));
}


void imonc_reboot(void)
{
	char buffer[128];

	get_command_answer("reboot", buffer, sizeof(buffer));
}


void imonc_route(int ci_index)
{
	char cmd[128], buffer[128];

	sprintf(cmd, "route %d", ci_index);
	get_command_answer(cmd, buffer, sizeof(buffer));
}

/* user commands */

int imonc_channels(void)
{
	char buffer[128];
	int channels;

	get_command_answer("channels", buffer, sizeof(buffer));
	sscanf(buffer, "%d", &channels);
	return channels;
}


double imonc_charge(int channel)
{
	char buffer[128];
	double charge;

	get_command_answer("charge", buffer, sizeof(buffer));
	sscanf(buffer, "%lf", &charge);
	return charge;
}


t_imonc_time imonc_chargetime(int channel)
{
	static t_imonc_time chargetime;

	return chargetime;
}


char *imonc_circuit(int ci_index)
{
	static char buffer[128];
	char cmd[128];

	sprintf(cmd, "circuit %d", ci_index);
	get_command_answer(cmd, buffer, sizeof(buffer));
	return buffer;
}


int imonc_circuits(void)
{
	char buffer[128];
	int circuits;

	get_command_answer("circuits", buffer, sizeof(buffer));
	sscanf(buffer, "%d", &circuits);
	return circuits;
}


int imonc_cpu(void)
{
	char buffer[128];
	int cpu;

	get_command_answer("cpu", buffer, sizeof(buffer));
	sscanf(buffer, "%d", &cpu);
	return cpu;
}


t_imonc_date imonc_date(void)
{
	static t_imonc_date routerdate;
	char buffer[128];

	get_command_answer("date", buffer, sizeof(buffer));
	sscanf(buffer, "%s %d/%d/%d %d:%d:%d", routerdate.weekday,
			                       &routerdate.day,
					       &routerdate.month,
					       &routerdate.year,
					       &routerdate.hour,
					       &routerdate.minute,
					       &routerdate.second);
	return routerdate;
}


char *imonc_device(int ci_index)
{
	static char buffer[128];
	char cmd[128];

	sprintf(cmd, "device %d", ci_index);
	get_command_answer(cmd, buffer, sizeof(buffer));
	return buffer;
}


char *imonc_driverid(int channel)
{
	static char buffer[128];
	char cmd[128];

	sprintf(cmd, "driverid %d", channel);
	get_command_answer(cmd, buffer, sizeof(buffer));
	return buffer;
}


char *imonc_help(void)
{
	static char buffer[1024];

	get_command_answer("help", buffer, sizeof(buffer));
	return buffer;
}


t_imonc_inout imonc_inout(int channel)
{
	char cmd[128], buffer[128];

	sprintf(cmd, "inout %d", channel);
	get_command_answer(cmd, buffer, sizeof(buffer));

	if(!strncasecmp("incoming", buffer, 8))
		return iio_incoming;
	
	if(!strncasecmp("outgoing", buffer, 8))
		return iio_outgoing;
	
	return iio_offline;
}


char *imonc_imond_log_file(void)
{
	static char buffer[1024];

	get_command_answer("imond-log-file", buffer, sizeof(buffer));
	return buffer;
}


char *imonc_ip(int channel)
{
	static char buffer[128];
	char cmd[128];

	sprintf(cmd, "ip %d", channel);
	get_command_answer(cmd, buffer, sizeof(buffer));
	return buffer;
}


int imonc_is_allowed(t_imonc_is_allowed command)
{
	char *type = "", cmd[128], buffer[128];
	int is_allowed;

	switch(command)
	{
		case iia_dial:
			type = "dial";
			break;
		case iia_dialmode:
			type = "dialmode";
			break;
		case iia_route:
			type = "route";
			break;
		case iia_reboot:
			type = "reboot";
			break;
		case iia_imond_log:
			type = "imond-log";
			break;
		case iia_telmond_log:
			type = "telmond-log";
			break;
		case iia_mgetty_log:
			type = "mgetty.log";
			break;
	}
	
	sprintf(cmd, "is-allowed %s", type);
	get_command_answer(cmd, buffer, sizeof(buffer));
	sscanf(buffer, "%d", &is_allowed);
	return is_allowed;
}


int imonc_is_enabled(void)
{
	char buffer[128];
	int is_enabled;

	get_command_answer("is-enabled", buffer, sizeof(buffer));
	sscanf(buffer, "%d", &is_enabled);
	return is_enabled;
}


int imonc_links(int ci_index)
{
	char cmd[128], buffer[128];
	int links;

	sprintf(cmd, "links %d", ci_index);
	get_command_answer(cmd, buffer, sizeof(buffer));
	sscanf(buffer, "%d", &links);
	return links;
}


char *imonc_log_dir(t_imonc_logdir logdir)
{
	static char buffer[1024];
	char *type = "", cmd[128];

	switch(logdir)
	{
		case ild_imond:
			type = "imond";
			break;
		case ild_telmond:
			type = "telmond";
			break;
		case ild_mgetty:
			type = "mgetty";
			break;
	}
	
	sprintf(cmd, "log-dir %s", type);
	get_command_answer(cmd, buffer, sizeof(buffer));
	return buffer;
}


char *imonc_mgetty_log_file(void)
{
	static char buffer[1024];

	get_command_answer("mgetty-log-file", buffer, sizeof(buffer));
	return buffer;
}


t_imonc_time imonc_online_time(int channel)
{
	static t_imonc_time onlinetime;
	char cmd[128], buffer[128];

	sprintf(cmd, "online-time %d", channel);
	get_command_answer(cmd, buffer, sizeof(buffer));
	sscanf(buffer, "%d:%d:%d", &onlinetime.hour, 
			           &onlinetime.minute, 
				   &onlinetime.second);
	return onlinetime;
}


t_imonc_pass imonc_pass(char *password)
{
	static t_imonc_pass pass;

	return pass;
}


char *imonc_phone(int channel)
{
	static char buffer[128];
	char cmd[128];

	sprintf(cmd, "phone %d", channel);
	get_command_answer(cmd, buffer, sizeof(buffer));
	return buffer;
}


int imonc_pppoe(void)
{
	char buffer[128];
	int pppoe;

	get_command_answer("pppoe", buffer, sizeof(buffer));
	sscanf(buffer, "%d", &pppoe);
	return pppoe;
}


int imonc_quantity(int channel)
{
	char cmd[128], buffer[128];
	int quantity;

	sprintf(cmd, "quantity %d", channel);
	get_command_answer(cmd, buffer, sizeof(buffer));
	sscanf(buffer, "%d", &quantity);
	return quantity;
}


void imonc_quit(void)
{
	char buffer[128];

	get_command_answer("quit", buffer, sizeof(buffer));
}


t_imonc_rate imonc_rate(int channel)
{
	static t_imonc_rate rate;
	char cmd[128], buffer[128];

	sprintf(cmd, "rate %d", channel);
	get_command_answer(cmd, buffer, sizeof(buffer));
	sscanf(buffer, "%d %d", &rate.in, &rate.out);
	return rate;
}


t_imonc_status imonc_status(int channel)
{
	static t_imonc_status status;

	return status;
}


char *imonc_telmond_log_file(void)
{
	static char buffer[1024];

	get_command_answer("telmond-log-file", buffer, sizeof(buffer));
	return buffer;
}


t_imonc_time imonc_time(int channel)
{
	static t_imonc_time onlinetime;
	char cmd[128], buffer[128];

	sprintf(cmd, "time %d", channel);
	get_command_answer(cmd, buffer, sizeof(buffer));
	sscanf(buffer, "%d:%d:%d", &onlinetime.hour, 
			           &onlinetime.minute,
				   &onlinetime.second);
	return onlinetime;
}


int imonc_uptime(void)
{
	char buffer[128];
	int seconds;

	get_command_answer("uptime", buffer, sizeof(buffer));
	sscanf(buffer, "%d", &seconds);
	return seconds;
}


t_imonc_usage imonc_usage(int channel)
{
	char cmd[128], buffer[128];

	sprintf(cmd, "usage %d", channel);
	get_command_answer(cmd, buffer, sizeof(buffer));
	
	if(!strncasecmp("fax", buffer, 3))
		return iu_fax;
	
	if(!strncasecmp("voice", buffer, 5))
		return iu_voice;
	
	if(!strncasecmp("net", buffer, 3))
		return iu_net;
	
	if(!strncasecmp("modem", buffer, 5))
		return iu_modem;
	
	if(!strncasecmp("raw", buffer, 5))
		return iu_raw;
	
	return iu_none;
}


t_imonc_version imonc_version(void)
{
	static t_imonc_version version;
	char buffer[128];

	get_command_answer("version", buffer, sizeof(buffer));
	sscanf(buffer, "%d %d.%d.%d", &version.protokoll,
			              &version.major,
				      &version.minor,
				      &version.revision);

	return version;
}

/*
 * Helpers
 */

char *imonc_uptime_formatted(int uptime)
{
	static char buffer[128];
	int days = 0, hours = 0, minutes = 0, seconds = 0;

	if(uptime >= 0) {
		days = uptime / 86400;
		hours = (uptime % 86400) / 3600;
		minutes = ((uptime % 86400) % 3600) / 60;
		seconds = ((uptime % 86400) % 3600) % 60;
	}

	sprintf(buffer, "%d d %d h %d m %d s", days, hours, minutes, seconds);
	return buffer;
}

char *imonc_inouttostr(t_imonc_inout inout)
{
	switch(inout)
	{
		case iio_offline:	return("Offline");
		case iio_incoming:	return("Incoming");
		case iio_outgoing:	return("Outgoing");
	}
	
	return "Offline";
}


char *imonc_usagetostr(t_imonc_usage usage)

{
	switch(usage)
	{
		case iu_none:		return("None");
		case iu_fax:		return("Fax");
		case iu_voice:		return("Voice");
		case iu_net:		return("Net");
		case iu_modem:		return("Modem");
		case iu_raw:		return("Raw");
	}

	return "None";
}

    
