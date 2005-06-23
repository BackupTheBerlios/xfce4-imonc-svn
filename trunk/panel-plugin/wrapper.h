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

#include "imonc.h"

typedef enum IMONC_DIALMODE
{
	idm_noset = -1,
	idm_off = 0,
	idm_auto = 1,
	idm_manual = 2
} t_imonc_dialmode;

typedef struct IMONC_TIME
{
	int hour;
	int minute;
	int second;
} t_imonc_time;

typedef struct IMONC_DATE
{
	char weekday[3];
	int day;
	int month;
	int year;
	int hour;
	int minute;
	int second;
} t_imonc_date;

typedef enum IMONC_INOUT
{
	iio_offline = 0,
	iio_incoming = 1,
	iio_outgoing = 2
} t_imonc_inout;

typedef enum IMONC_IS_ALLOWED
{
	iia_dial,
	iia_dialmode,
	iia_route,
	iia_reboot,
	iia_imond_log,
	iia_telmond_log,
	iia_mgetty_log
} t_imonc_is_allowed;

typedef enum IMONC_LOGDIR
{
	ild_imond,
	ild_telmond,
	ild_mgetty
} t_imonc_logdir;

typedef enum IMONC_PASS
{
	ip_userpassword,
	ip_adminpassword,
	ip_adminmode
} t_imonc_pass;

typedef struct IMONC_RATE
{
	int in;
	int out;
} t_imonc_rate;

typedef enum IMONC_STATUS
{
	is_offline,
	is_dialing,
	is_online,
} t_imonc_status;

typedef enum IMONC_USAGE
{
	iu_none,
	iu_fax,
	iu_voice,
	iu_net,
	iu_modem,
	iu_raw
} t_imonc_usage;

typedef struct IMONC_VERSION
{
	int protokoll;
	int major;
	int minor;
	int revision;
} t_imonc_version;

/* admin commands */

void imonc_addlink(int ci_index);
void imonc_adjust_time(int seconds);
void imonc_delete(char *filename, char *password);
int imonc_hup_timeout(int ci_index, int value);
void imonc_removelink(int ci_index);
void imonc_reset_telmond_log_file(void);
void imonc_reset_imond_log_file(void);
void imonc_receive(char *filename, int bytes, char *password);
char *imonc_send(char *filename, char *password);
char *imonc_support(char *password);
void imonc_sync(void);

/* admin or user commands */

void imonc_dial(void);
t_imonc_dialmode imonc_dialmode(t_imonc_dialmode mode);
void imonc_disable(void);
void imonc_enable(void);
void imonc_halt(void);
void imonc_hangup(void);
void imonc_reboot(void);
void imonc_route(int ci_index);

/* user commands */

int imonc_channels(void);
double imonc_charge(int channel);
t_imonc_time imonc_chargetime(int channel);
char *imonc_circuit(int ci_index);
int imonc_circuits(void);
int imonc_cpu(void);
t_imonc_date imonc_date(void);
char *imonc_device(int ci_index);
char *imonc_driverid(int channel);
char *imonc_help(void);
t_imonc_inout imonc_inout(int channel);
char *imonc_imond_log_file(void);
char *imonc_ip(int channel);
int imonc_is_allowed(t_imonc_is_allowed command);
int imonc_is_enabled(void);
int imonc_links(int ci_index);
char *imonc_log_dir(t_imonc_logdir logdir);
char *imonc_mgetty_log_file(void);
t_imonc_time imonc_online_time(int channel);
t_imonc_pass imonc_pass(char *password);
char *imonc_phone(int channel);
int imonc_pppoe(void);
int imonc_quantity(int channel);
void imonc_quit(void);
t_imonc_rate imonc_rate(int channel);
t_imonc_status imonc_status(int channel);
char *imonc_telmond_log_file(void);
t_imonc_time imonc_time(int channel);
int imonc_uptime(void);
t_imonc_usage imonc_usage(int channel);
t_imonc_version imonc_version(void);

/* helpers */

char *imonc_uptime_formatted(int uptime);
char *imonc_inouttostr(t_imonc_inout inout);
char *imonc_usagetostr(t_imonc_usage usage);
