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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>

#include <libxfce4util/i18n.h>
#include <libxfcegui4/libxfcegui4.h>
#include <libxfcegui4/dialogs.h>
#include <panel/xfce.h>
#include <panel/plugins.h>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include "config_gui.h"
#include "wrapper.h"

#include "debug.h"


#define PLUGIN_NAME			"imonc"

#define	IMONC_DEFAULT_HOSTNAME		"fli4l"

/* Configuration Keywords */
#define CONF_HOST			"Host"
#define CONF_PORT			"Port"
#define CONF_PASSWORD			"Password"
#define CONF_ADMINPASSWORD		"AdminPassword"
#define CONF_INTERVAL			"Interval"
#define CONF_DISPLAYCOMMANDBUTTON	"DisplayCommandbutton"
#define CONF_COMMAND			"Command"
#define CONF_DISPLAYHOSTNAME		"DisplayHostname"
#define CONF_DISPLAYBARS		"DisplayBar"
#define	CONF_COLOR_BAR			"ColorBar"		// Channel no. and
								// direction is added
								// to the name

/* Configurable parameters */
typedef struct param_t
{
	gchar acHost[256];
	gint iDisplayHostname;
	gint iPort;
	gchar acPassword[32];
	gchar acAdminPassword[32];
	gboolean bDisplayCommandbutton;
	gchar acCommand[256];
	gint iDisplayBars;
	GdkColor wBarColor[IMONC_MAX_CHANNELS][IMONC_MAX_DIRECTIONS];
} param_t;

typedef struct conf_t
{
	GtkWidget *wTopLevel;
	struct gui_t oGUI;		/* Configuration/option dialog */
	struct param_t oParam;
} conf_t;

/* Plugin monitor widgets */
typedef struct monitor_t
{
	GtkWidget *wEventBox;
	GtkWidget *hBox;
	GtkWidget *wBox;
	GtkWidget *wTitleEventbox;
	GtkWidget *wTitle;
	GtkTooltips *wTitleTooltip;
	GtkWidget *webLine[IMONC_MAX_CHANNELS];
	GtkWidget *wLine[IMONC_MAX_CHANNELS];
	GtkTooltips *wTooltip[IMONC_MAX_CHANNELS];
	GtkWidget *wBar[IMONC_MAX_CHANNELS][IMONC_MAX_DIRECTIONS];
	GtkWidget *wCommand;
	GtkTooltips *wCommandTooltip;
} monitor_t;

typedef struct plugin_t {
	guint iTimerId;			/* Cyclic update */
	guint iTimerInterval;		/* Timer interval */
	struct conf_t oConf;
	struct monitor_t oMonitor;
} plugin_t;


/**********************************************************************
 * GetHostTooltip 
 **********************************************************************/
static gchar *GetHostTooltip(char *buffer, int length)
{
	gint cpu = imonc_cpu();
	t_imonc_date routerdate = imonc_date();
	gint uptime = imonc_uptime();
	gint enabled = imonc_is_enabled();
	t_imonc_version version = imonc_version();

	sprintf(buffer, "CPU usage:\t\t%d %%\n" \
			"Router date:\t\t%s %02d.%02d.%04d %02d:%02d:%02d\n" \
			"Router uptime:\t\t%s\n" \
			"Dialmode auto:\t\t%s\n" \
			"Version\tProtokoll:\t%d\n" \
			"\t\tImond:\t%d.%d.%d",
		cpu,
		routerdate.weekday,
		routerdate.day,
		routerdate.month,
		routerdate.year,
		routerdate.hour,
		routerdate.minute,
		routerdate.second,
		imonc_uptime_formatted(uptime),
		enabled == 0 ? "no" : "yes",
		version.protokoll,
		version.major,
		version.minor,
		version.revision);

	return(buffer);
}
/**********************************************************************/


/**********************************************************************
 * GetLineTooltip 
 **********************************************************************/
static gchar *GetLineTooltip(gint line, gchar *buffer, gint length)
{
	t_imonc_inout inout = imonc_inout(line);
	gchar *phone = imonc_phone(line);
	gchar *driverid = imonc_driverid(line);
	t_imonc_usage usage = imonc_usage(line);
	gchar *ip = imonc_ip(line);
	t_imonc_rate rate = imonc_rate(line);
	gint quantity = imonc_quantity(line);
	t_imonc_time online_time = imonc_online_time(line);
	t_imonc_time online_time_sum = imonc_time(line);
	

	sprintf(buffer, "Direction:\t\t\t%s\n" \
			"Connectionname:\t%s\n" \
			"Driver:\t\t\t%s\n" \
			"Connectiontype:\t%s\n" \
			"IP address:\t\t%s\n" \
			"Transferrate:\t\tin:%d out:%d\n" \
			"Bytes transfered:\t%d\n" \
			"Onlinetime:\t\t%02d:%02d:%02d\n" \
			"Onlinetime (sum):\t%02d:%02d:%02d",
		imonc_inouttostr(inout),
		phone,
		driverid,
		imonc_usagetostr(usage),
		ip,
		rate.in, rate.out,
		quantity,
		online_time.hour, online_time.minute, online_time.second,
		online_time_sum.hour, online_time_sum.minute, online_time_sum.second);

	return(buffer);
}
/**********************************************************************/


/**********************************************************************
 * DisplayStatus
 **********************************************************************/
static gint DisplayStatus(struct plugin_t *p_poPlugin)
{
	struct param_t *poConf = &(p_poPlugin->oConf.oParam);
	struct monitor_t *poMonitor = &(p_poPlugin->oMonitor);
	gint i, j;
	gchar *answer;
	gint channels;
	t_imonc_rate rate;
	GdkColor color;
	gchar command[128], label[128], tooltip[1024];
	gboolean online;

	if(poConf->bDisplayCommandbutton)
	{
		gtk_widget_show(GTK_WIDGET(poMonitor->wCommand));
		gtk_tooltips_set_tip(poMonitor->wCommandTooltip, 
			GTK_WIDGET(poMonitor->wCommand),
			poConf->acCommand, NULL);
	}
	else
	{
		gtk_widget_hide(GTK_WIDGET(poMonitor->wCommand));
	}
	
	for(i = 0; i < IMONC_MAX_CHANNELS; ++i)
	{
		for(j = 0; j < IMONC_MAX_DIRECTIONS; ++j)
		{
			if(poConf->iDisplayBars == SMB_NEVER)
			{
				gtk_widget_hide(GTK_WIDGET(poMonitor->wBar[i][j]));
			}

			if(poConf->iDisplayBars == SMB_ALWAYS)
			{
				gtk_widget_show(GTK_WIDGET(poMonitor->wBar[i][j]));
			}
		}
	}

	if(imonc_test_socket() >= 0)
	{
		p_poPlugin->iTimerInterval = 1000;	// 1 second

		if(poConf->iDisplayHostname == SHN_NAME 
			|| poConf->iDisplayHostname == SHN_FQDN)
		{
			gdk_color_parse("black", &color);
			gtk_widget_modify_fg(GTK_WIDGET(poMonitor->wTitle),
					GTK_STATE_NORMAL, &color);
			gtk_widget_show(GTK_WIDGET(poMonitor->wTitle));
			gtk_label_set_text(GTK_LABEL(poMonitor->wTitle),
					imonc_get_fqdn_name(poConf->iDisplayHostname == SHN_FQDN));
		}
		else
		{
			gtk_widget_hide(GTK_WIDGET(poMonitor->wTitle));
		}

		tooltip[0] = '\0';
		gtk_tooltips_set_tip(poMonitor->wTitleTooltip, 
			GTK_WIDGET(poMonitor->wTitleEventbox),
			GetHostTooltip(tooltip, sizeof(tooltip)), 
			NULL);
				
		send_command("channels");
		answer = get_answer();

		if(strlen(answer) > 0)
		{
			// ISDN
			channels = atoi(answer);
			
			for(i = 0; i < channels; ++i)
			{
				online = FALSE;
				sprintf(command, "status %d", i + 1);
				send_command(command);
				answer = get_answer();
			
				if(!strncasecmp(answer, "offline", 7))
					gdk_color_parse("red", &color);
				if(!strncasecmp(answer, "calling", 7))
					gdk_color_parse("yellow", &color);
				if(!strncasecmp(answer, "online", 6))
				{
					gdk_color_parse("green", &color);
					online = TRUE;
				}

				gtk_widget_modify_fg(GTK_WIDGET(poMonitor->wLine[i]), 
							GTK_STATE_NORMAL, &color);

				sprintf(label, "%s", answer);
				gtk_label_set_text(GTK_LABEL(poMonitor->wLine[i]), label);

				tooltip[0] = '\0';
				gtk_tooltips_set_tip(poMonitor->wTooltip[i], 
					GTK_WIDGET(poMonitor->webLine[i]),
					GetLineTooltip(i + 1, tooltip, sizeof(tooltip)), 
					NULL);

				if(online)
				{
					rate = imonc_rate(i + 1);
				
					for(j = 0; j < IMONC_MAX_DIRECTIONS; ++j)
					{
						if(poConf->iDisplayBars == SMB_CONNECTED)
							gtk_widget_show(GTK_WIDGET(poMonitor->wBar[i][j]));
						gtk_progress_bar_set_fraction(
								GTK_PROGRESS_BAR(poMonitor->wBar[i][j]),
								(j % 2 ? rate.out : rate.in) / IMONC_MAX_ISDN_TX);
					}
				}
				else
				{
					for(j = 0; j < IMONC_MAX_DIRECTIONS; ++j)
					{
						if(poConf->iDisplayBars == SMB_CONNECTED)
							gtk_widget_hide(GTK_WIDGET(poMonitor->wBar[i][j]));
					}
				}
			}
		}
		else
		{
			send_command("pppoe");
			answer = get_answer();

			if(strlen(answer) > 0)
			{
				// DSL
				channels = atoi(answer);
				
				for(i = 0; i < channels; ++i)
				{
					sprintf(command, "rate %d", i);
					send_command(command);
					
					if(i == 0)
						sprintf(label, "Incomming: %s", get_answer());
					else
						sprintf(label, "Outgoing : %s", get_answer());
					
					gtk_label_set_text(GTK_LABEL(poMonitor->wLine[i]), label);
				}
			}
		}
	}
	else
	{
		// Connections lost
		p_poPlugin->iTimerInterval = 10000;		// retry in 10 seconds

		// Show hostname in red
		gdk_color_parse("red", &color);
		gtk_widget_modify_fg(GTK_WIDGET(poMonitor->wTitle),
				GTK_STATE_NORMAL, &color);
		gtk_label_set_text(GTK_LABEL(poMonitor->wTitle),
				imonc_get_fqdn_name(poConf->iDisplayHostname == SHN_FQDN));
		
		// Show always the hostname
		gtk_widget_show(GTK_WIDGET(poMonitor->wTitle));
		
		service_connect(poConf->acHost, poConf->iPort,
			poConf->acPassword, poConf->acAdminPassword);
	}
	
	return(0);
}
/**********************************************************************/


/**********************************************************************
 * SetTimer
 **********************************************************************/
static gboolean SetTimer(void *p_pvPlugin)
{
	struct plugin_t *poPlugin = (plugin_t *)p_pvPlugin;

	if (poPlugin->iTimerId)
	{
		g_source_remove(poPlugin->iTimerId);
		poPlugin->iTimerId = 0;
	}
	
	XFCE_PANEL_LOCK();
	DisplayStatus(poPlugin);
	XFCE_PANEL_UNLOCK();
	poPlugin->iTimerId = g_timeout_add(poPlugin->iTimerInterval, 
			(GtkFunction)SetTimer, poPlugin);
	return (1);
}
/**********************************************************************/


/**********************************************************************
 * SetTimer
 **********************************************************************/
static void SetSingleBarColor(struct plugin_t *p_poPlugin, gint p_iLine, gint p_iDir)
{
	struct plugin_t *poPlugin = p_poPlugin;
	struct param_t *poConf = &(poPlugin->oConf.oParam);
	struct monitor_t *poMonitor = &(poPlugin->oMonitor);
	GtkRcStyle *poStyle;

	poStyle = gtk_widget_get_modifier_style(
			GTK_WIDGET(poMonitor->wBar[p_iLine][p_iDir]));

	if(!poStyle)
		poStyle = gtk_rc_style_new();
	else
	{
		poStyle->color_flags[GTK_STATE_PRELIGHT] |= GTK_RC_BG;
		poStyle->bg[GTK_STATE_PRELIGHT] = poConf->wBarColor[p_iLine][p_iDir];
	}

	gtk_widget_modify_style(
			GTK_WIDGET(poMonitor->wBar[p_iLine][p_iDir]), poStyle);
}
/**********************************************************************/

	
/**********************************************************************
 * Start the command
 **********************************************************************/
static void StartCommand(GtkWidget *p_wPB, void *p_pvPlugin)
{
	struct plugin_t *poPlugin = (plugin_t *)p_pvPlugin;
	struct param_t *poConf = &(poPlugin->oConf.oParam);
	gchar acCommand[1024];
	gint i, j, len;

	memset(acCommand, '\0', sizeof(acCommand));

	for(len = strlen(poConf->acCommand), i = 0, j = 0;
			i < len && j < sizeof(acCommand);
			++i)
	{
		if(poConf->acCommand[i] == '%' && i < len)
		{
			switch(poConf->acCommand[i + 1])
			{
				case 'p':
					{
						static char p[6];
						sprintf(p, "%d", poConf->iPort);
						strcat(acCommand, p);
					}
					j = strlen(acCommand);
					++i;
					break;
					
				case 'P':
					if(strlen(poConf->acPassword) > 0)
					{
						strcat(acCommand, poConf->acPassword);
						j = strlen(acCommand);
					}
					++i;
					break;

				case 'h':
					strcat(acCommand, poConf->acHost);
					j = strlen(acCommand);
					++i;
					break;
			};

		}
		else
			acCommand[j++] = poConf->acCommand[i];
	}

	if(strlen(acCommand) > 0)
	{
		xfce_exec(acCommand, FALSE, FALSE, NULL);
	}
}
/**********************************************************************/


/**********************************************************************
 * CreateMonitorBars
 **********************************************************************/
static int CreateMonitorBars(struct plugin_t *p_poPlugin, gint p_iOrientation)
{
	struct plugin_t *poPlugin = p_poPlugin;
	struct param_t *poConf = &(poPlugin->oConf.oParam);
	struct monitor_t *poMonitor = &(poPlugin->oMonitor);
	gint i, j;
	gchar label[128];
	GtkWidget *image;
			    
	// First create a horizontal box
	poMonitor->hBox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(poMonitor->hBox);
	gtk_container_set_border_width(GTK_CONTAINER(poMonitor->hBox), border_width);
	gtk_container_add(GTK_CONTAINER(poMonitor->wEventBox), poMonitor->hBox);

	// Add the monitor bars
	for(i = 0; i < IMONC_MAX_CHANNELS; ++i)
	{
		for(j = 0; j < IMONC_MAX_DIRECTIONS; ++j)
		{
			poMonitor->wBar[i][j] = gtk_progress_bar_new();
			gtk_progress_bar_set_orientation(
					GTK_PROGRESS_BAR(poMonitor->wBar[i][j]),
					(p_iOrientation == HORIZONTAL ?
						GTK_PROGRESS_BOTTOM_TO_TOP :
						GTK_PROGRESS_LEFT_TO_RIGHT));
			SetSingleBarColor(p_poPlugin, i, j);
			gtk_widget_show(GTK_WIDGET(poMonitor->wBar[i][j]));
			gtk_box_pack_start(GTK_BOX(poMonitor->hBox),
					GTK_WIDGET(poMonitor->wBar[i][j]),
					FALSE, FALSE, 0);
		}
	}
	
	// Add a vertical box for the text
	poMonitor->wBox = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(poMonitor->wBox);
	gtk_container_add(GTK_CONTAINER(poMonitor->hBox), poMonitor->wBox);

	// Create the Eventbox for the hostname tooltip
	poMonitor->wTitleEventbox = gtk_event_box_new();
	gtk_widget_show(poMonitor->wTitleEventbox);
	gtk_box_pack_start(GTK_BOX(poMonitor->wBox), 
			GTK_WIDGET(poMonitor->wTitleEventbox), 
			FALSE, FALSE, 0);
	
	// Create the hostname label
	poMonitor->wTitle = gtk_label_new(poConf->acHost);
	gtk_widget_show(poMonitor->wTitle);
	gtk_container_add(GTK_CONTAINER(poMonitor->wTitleEventbox), poMonitor->wTitle);

	// Set the hostname tooltip to ""
	poMonitor->wTitleTooltip = gtk_tooltips_new();
	gtk_tooltips_set_tip(poMonitor->wTitleTooltip, 
			GTK_WIDGET(poMonitor->wTitleEventbox),
			"", NULL);

	// Create the status labels for each channel
	for(i = 0; i < IMONC_MAX_CHANNELS; ++i)
	{
		// Create the label
		sprintf(label, "Line%d: -------", i + 1);
		poMonitor->wLine[i] = gtk_label_new(label);
		gtk_widget_show(poMonitor->wLine[i]);

		// Create the eventbox for the tooltip
		poMonitor->webLine[i] = gtk_event_box_new();
		gtk_widget_show(poMonitor->webLine[i]);
		gtk_container_add(GTK_CONTAINER(poMonitor->webLine[i]), poMonitor->wLine[i]);
		gtk_box_pack_start(GTK_BOX(poMonitor->wBox), 
				GTK_WIDGET(poMonitor->webLine[i]), 
				FALSE, FALSE, 0);

		// Set the tooltip to ""
		poMonitor->wTooltip[i] = gtk_tooltips_new();
		gtk_tooltips_set_tip(poMonitor->wTooltip[i], 
				GTK_WIDGET(poMonitor->webLine[i]),
				"", NULL);
	}
	
	// Create the command button
	poMonitor->wCommand = gtk_button_new();
	gtk_button_set_relief(GTK_BUTTON(poMonitor->wCommand), GTK_RELIEF_NONE);
	g_signal_connect(GTK_WIDGET(poMonitor->wCommand), "clicked",
			G_CALLBACK(StartCommand), p_poPlugin);
	image = gtk_image_new_from_stock("gtk-network", GTK_ICON_SIZE_BUTTON);
        gtk_widget_show(image);
	gtk_container_add(GTK_CONTAINER(poMonitor->wCommand), image);
	gtk_box_pack_start(GTK_BOX(poMonitor->hBox), 
			GTK_WIDGET(poMonitor->wCommand), 
			FALSE, FALSE, 0);
	poMonitor->wCommandTooltip = gtk_tooltips_new();
	gtk_tooltips_set_tip(poMonitor->wCommandTooltip, 
			GTK_WIDGET(poMonitor->wCommand),
			"", NULL);
	
	return (0);
}
/**********************************************************************/


/**********************************************************************
 * NewPlugin
 **********************************************************************/
static plugin_t *NewPlugin(void)
{
	/* New instance of the plugin (constructor) */
	struct plugin_t *poPlugin;
	struct param_t *poConf;
	struct monitor_t *poMonitor;
	struct sigaction act;
	gint i, j;
	gchar *colors[4] = { "green", "red", "white", "yellow" };

	poPlugin = g_new(plugin_t, 1);
	memset(poPlugin, 0, sizeof(plugin_t));
	poConf = &(poPlugin->oConf.oParam);
	poMonitor = &(poPlugin->oMonitor);

	// Set the default values
	strcpy(poConf->acHost, IMONC_DEFAULT_HOSTNAME);
	poConf->iPort = IMOND_DEFAULT_PORT;
	poConf->bDisplayCommandbutton = FALSE;
	poConf->iDisplayHostname = SHN_NAME;
	poConf->iDisplayBars = SMB_ALWAYS;

	for(i = 0; i < IMONC_MAX_CHANNELS; ++i)
	{
		for(j = 0; j < IMONC_MAX_DIRECTIONS; ++j)
		{
			gdk_color_parse(colors[((i * IMONC_MAX_DIRECTIONS) + j) % 4],
					&poConf->wBarColor[i][j]);
		}
	}
	
	poPlugin->iTimerId = 0;
	poPlugin->iTimerInterval = 1000;

	// Create the initial eventbox
	poMonitor->wEventBox = gtk_event_box_new();
	gtk_widget_show(poMonitor->wEventBox);

	// Set the alarm signal handler for imonc
	siginterrupt(SIGALRM, 1);

	act.sa_handler = service_reconnect;

	sigemptyset(&act.sa_mask);

	if(sigaction(SIGPIPE, &act, NULL) < 0)
	{
		puts("xfce4-imonc-plugin: Cannot set sigaction\n");
		// Dont't know what to do here
	}

	signal(SIGALRM, service_reconnect);
	
	return(poPlugin);
}
/**********************************************************************/


/**********************************************************************
 * plugin_create_control
 **********************************************************************/
static gboolean plugin_create_control(Control *p_poCtrl)
{
	struct plugin_t *poPlugin;

	TRACE_IMONC("plugin_create_control()\n");
	poPlugin = NewPlugin();
	gtk_container_add(GTK_CONTAINER(p_poCtrl->base),
			  poPlugin->oMonitor.wEventBox);
	p_poCtrl->data = (gpointer)poPlugin;
	p_poCtrl->with_popup = FALSE;
	gtk_widget_set_size_request(p_poCtrl->base, -1, -1);
	return(TRUE);
}
/**********************************************************************/


/**********************************************************************
 * plugin_free
 **********************************************************************/
static void plugin_free(Control *ctrl)
{
	struct plugin_t *poPlugin;
	struct param_t *poConf;

	TRACE_IMONC("plugin_free()\n");
	g_return_if_fail(ctrl != NULL);
	g_return_if_fail(ctrl->data != NULL);
	poPlugin = (plugin_t *)ctrl->data;
	poConf = &(poPlugin->oConf.oParam);
    
	if(poPlugin->iTimerId)
		g_source_remove(poPlugin->iTimerId);
   
	service_disconnect();
    
	g_free(poPlugin);
}
/**********************************************************************/


/**********************************************************************
 * plugin_read_config
 **********************************************************************/
static void plugin_read_config(Control *p_poCtrl, xmlNodePtr p_poParent)
{
	gint i, j;
	struct plugin_t *poPlugin = (plugin_t *)p_poCtrl->data;
	struct param_t *poConf = &(poPlugin->oConf.oParam);
	struct monitor_t *poMonitor = &(poPlugin->oMonitor);
	xmlNodePtr poNode;
	gchar *pc;
	gchar name[32];
	
	TRACE_IMONC("plugin_read_config()\n");

	if(!p_poParent)
		return;

	for(poNode = p_poParent->children; poNode; poNode = poNode->next)
	{
		if(!(xmlStrEqual(poNode->name, PLUGIN_NAME)))
			continue;

		if((pc = xmlGetProp(poNode, (CONF_HOST))))
		{
			memset(poConf->acHost, 0, sizeof(poConf->acHost));
			strncpy(poConf->acHost, pc, sizeof(poConf->acHost) - 1);
			gtk_label_set_text(GTK_LABEL(poMonitor->wTitle), poConf->acHost);
			xmlFree(pc);
		}

		if((pc = xmlGetProp(poNode, (CONF_PORT))))
		{
			poConf->iPort = atoi(pc);
			xmlFree(pc);
		}

		if((pc = xmlGetProp(poNode, (CONF_PASSWORD))))
		{
			memset(poConf->acPassword, 0, sizeof(poConf->acPassword));
			strncpy(poConf->acPassword, pc, sizeof(poConf->acPassword) - 1);
			xmlFree(pc);
		}

		if((pc = xmlGetProp(poNode, (CONF_ADMINPASSWORD))))
		{
			memset(poConf->acAdminPassword, 0, sizeof(poConf->acAdminPassword));
			strncpy(poConf->acAdminPassword, pc, sizeof(poConf->acAdminPassword) - 1);
			xmlFree(pc);
		}
		
		if((pc = xmlGetProp(poNode, (CONF_INTERVAL))))
		{
			poPlugin->iTimerInterval = atoi(pc);
			xmlFree(pc);
		}

		if((pc = xmlGetProp(poNode, (CONF_DISPLAYCOMMANDBUTTON))))
		{
			poConf->bDisplayCommandbutton = atoi(pc);
			xmlFree(pc);
		}

		if((pc = xmlGetProp(poNode, (CONF_COMMAND))))
		{
			memset(poConf->acCommand, 0, sizeof(poConf->acCommand));
			strncpy(poConf->acCommand, pc, sizeof(poConf->acCommand) - 1);
			xmlFree(pc);
		}

		if((pc = xmlGetProp(poNode, (CONF_DISPLAYHOSTNAME))))
		{
			poConf->iDisplayHostname = atoi(pc);
			xmlFree(pc);
		}
		
		if((pc = xmlGetProp(poNode, (CONF_DISPLAYBARS))))
		{
			poConf->iDisplayBars = atoi(pc);
			xmlFree(pc);
		}
		
		for(i = 0; i < IMONC_MAX_CHANNELS; ++i)
		{
			for(j = 0; j < IMONC_MAX_DIRECTIONS; ++j)
			{
				sprintf(name, "%s_%d_%d", CONF_COLOR_BAR, i, j);
				if((pc = xmlGetProp(poNode, name)))
				{
					gdk_color_parse(pc,
							&poConf->wBarColor[i][j]);
				}
			}
		}
	}
	
	service_connect(poConf->acHost, poConf->iPort,
		poConf->acPassword, poConf->acAdminPassword);

	SetTimer(poPlugin);
}
/**********************************************************************/


/**********************************************************************
 * plugin_write_config
 **********************************************************************/
static void plugin_write_config(Control *p_poCtrl, xmlNodePtr p_poParent)
{
	gint i, j;
	struct plugin_t *poPlugin = (plugin_t *)p_poCtrl->data;
	struct param_t *poConf = &(poPlugin->oConf.oParam);
	xmlNodePtr poRoot;
	gchar name[32], value[32];

	TRACE_IMONC("plugin_write_config()\n");

	poRoot = xmlNewTextChild(p_poParent, NULL, PLUGIN_NAME, NULL);

	xmlSetProp(poRoot, CONF_HOST, poConf->acHost);
	sprintf(value, "%d", poConf->iPort);
	xmlSetProp(poRoot, CONF_PORT, value);
	xmlSetProp(poRoot, CONF_PASSWORD, poConf->acPassword);
	xmlSetProp(poRoot, CONF_ADMINPASSWORD, poConf->acAdminPassword);
	sprintf(value, "%d", poPlugin->iTimerInterval);
	xmlSetProp(poRoot, CONF_INTERVAL, value);
	sprintf(value, "%d", poConf->bDisplayCommandbutton);
	xmlSetProp(poRoot, CONF_DISPLAYCOMMANDBUTTON, value);
	xmlSetProp(poRoot, CONF_COMMAND, poConf->acCommand);
	sprintf(value, "%d", poConf->iDisplayHostname);
	xmlSetProp(poRoot, CONF_DISPLAYHOSTNAME, value);
	sprintf(value, "%d", poConf->iDisplayBars);
	xmlSetProp(poRoot, CONF_DISPLAYBARS, value);
	
	for(i = 0; i < IMONC_MAX_CHANNELS; ++i)
	{
		for(j = 0; j < IMONC_MAX_DIRECTIONS; ++j)
		{
			sprintf(name, "%s_%d_%d", CONF_COLOR_BAR, i, j);
			g_snprintf(value, 8, "#%02X%02X%02X",
					(guint)poConf->wBarColor[i][j].red >> 8,
					(guint)poConf->wBarColor[i][j].green >> 8,
					(guint)poConf->wBarColor[i][j].blue >> 8);
			xmlSetProp(poRoot, name, value);
		}
	}
}
/**********************************************************************/


/*
 * GUI - Callbacks
 * */

/**********************************************************************
 * SetHost
 **********************************************************************/
static void SetHost(GtkWidget *p_wTF, void *p_pvPlugin)
{
	struct plugin_t *poPlugin = (plugin_t *)p_pvPlugin;
	struct param_t *poConf = &(poPlugin->oConf.oParam);
	struct monitor_t *poMonitor = &(poPlugin->oMonitor);
	const gchar *pcHost = gtk_entry_get_text(GTK_ENTRY(p_wTF));

	TRACE_IMONC ("SetHost()\n");

	memset(poConf->acHost, 0, sizeof(poConf->acHost));
	strncpy(poConf->acHost, pcHost, sizeof(poConf->acHost) - 1);

	service_connect(poConf->acHost, poConf->iPort,
		poConf->acPassword, poConf->acAdminPassword);

	gtk_label_set_text(GTK_LABEL(poMonitor->wTitle), poConf->acHost);
}
/**********************************************************************/


/**********************************************************************
 * SetPort
 **********************************************************************/
static void SetPort(GtkWidget *p_wTF, void *p_pvPlugin)
{
	struct plugin_t *poPlugin = (plugin_t *)p_pvPlugin;
	struct param_t *poConf = &(poPlugin->oConf.oParam);
	const gchar *acPort = gtk_entry_get_text(GTK_ENTRY(p_wTF));

	poConf->iPort = atoi(acPort);
	
	service_connect(poConf->acHost, poConf->iPort,
		poConf->acPassword, poConf->acAdminPassword);
}
/**********************************************************************/


/**********************************************************************
 * SetPassword
 **********************************************************************/
static void SetPassword(GtkWidget *p_wTF, void *p_pvPlugin)
{
	struct plugin_t *poPlugin = (plugin_t *)p_pvPlugin;
	struct param_t *poConf = &(poPlugin->oConf.oParam);
	const gchar *pcPassword = gtk_entry_get_text(GTK_ENTRY(p_wTF));

	memset(poConf->acPassword, 0, sizeof(poConf->acPassword));
	strncpy(poConf->acPassword, pcPassword, sizeof(poConf->acPassword) - 1);
	
	service_connect(poConf->acHost, poConf->iPort,
		poConf->acPassword, poConf->acAdminPassword);
}
/**********************************************************************/


/**********************************************************************
 * SetAdminPassword
 **********************************************************************/
static void SetAdminPassword(GtkWidget *p_wTF, void *p_pvPlugin)
{
	struct plugin_t *poPlugin = (plugin_t *)p_pvPlugin;
	struct param_t *poConf = &(poPlugin->oConf.oParam);
	const gchar *pcAdminPassword = gtk_entry_get_text(GTK_ENTRY(p_wTF));

	memset(poConf->acAdminPassword, 0, sizeof(poConf->acAdminPassword));
	strncpy(poConf->acAdminPassword, pcAdminPassword, sizeof(poConf->acAdminPassword) - 1);
	
	service_connect(poConf->acHost, poConf->iPort,
		poConf->acPassword, poConf->acAdminPassword);
}
/**********************************************************************/


/**********************************************************************
 * SetUpdateInterval
 **********************************************************************/
static void SetUpdateInterval(GtkWidget *p_wTF, void *p_pvPlugin)
{
	struct plugin_t *poPlugin = (plugin_t *)p_pvPlugin;
	const gchar *acUpdateInterval = gtk_entry_get_text(GTK_ENTRY(p_wTF));

	poPlugin->iTimerInterval = atoi(acUpdateInterval);
}
/**********************************************************************/


/**********************************************************************
 * SetDisplayHostname
 **********************************************************************/
static void SetDisplayCommandbutton(GtkWidget *p_wCB, void *p_pvPlugin)
{
	struct plugin_t *poPlugin = (plugin_t *)p_pvPlugin;
	struct param_t *poConf = &(poPlugin->oConf.oParam);
	struct gui_t *poGUI = &(poPlugin->oConf.oGUI);
	gboolean active = gtk_toggle_button_get_active(
			GTK_TOGGLE_BUTTON(p_wCB));
	const gchar *pcCommand = gtk_entry_get_text(GTK_ENTRY(poGUI->wTF_Command));

	TRACE_IMONC("SetDisplayCommandbutton\n");
	
	gtk_widget_set_sensitive(poGUI->wTF_Command, active);
	poConf->bDisplayCommandbutton = active;
	memset(poConf->acCommand, 0, sizeof(poConf->acCommand));
	strncpy(poConf->acCommand, pcCommand, sizeof(poConf->acCommand) - 1);
}
/**********************************************************************/


/**********************************************************************
 * SetDisplayHostname
 **********************************************************************/
static void SetDisplayHostname(GtkWidget *p_wRB, void *p_pvPlugin)
{
	struct plugin_t *poPlugin = (plugin_t *)p_pvPlugin;
	struct param_t *poConf = &(poPlugin->oConf.oParam);
	struct gui_t *poGUI = &(poPlugin->oConf.oGUI);

	TRACE_IMONC("SetDisplayHostname\n");
	
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(p_wRB)))
	{
		if(p_wRB == poGUI->wRB_HostnameHide)
			poConf->iDisplayHostname = SHN_HIDE;
		if(p_wRB == poGUI->wRB_HostnameName)
			poConf->iDisplayHostname = SHN_NAME;
		if(p_wRB == poGUI->wRB_HostnameFQDN)
			poConf->iDisplayHostname = SHN_FQDN;
	}
}
/**********************************************************************/


/**********************************************************************
 * SetDisplayBars
 **********************************************************************/
static void SetDisplayBars(GtkWidget *p_wRB, void *p_pvPlugin)
{
	struct plugin_t *poPlugin = (plugin_t *)p_pvPlugin;
	struct param_t *poConf = &(poPlugin->oConf.oParam);
	struct gui_t *poGUI = &(poPlugin->oConf.oGUI);

	TRACE_IMONC("SetDisplayBars\n");
	
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(p_wRB)))
	{
		if(p_wRB == poGUI->wRB_BarsNever)
			poConf->iDisplayBars = SMB_NEVER;
		if(p_wRB == poGUI->wRB_BarsConnected)
			poConf->iDisplayBars = SMB_CONNECTED;
		if(p_wRB == poGUI->wRB_BarsAlways)
			poConf->iDisplayBars = SMB_ALWAYS;
	}
}
/**********************************************************************/



/**********************************************************************
 * SetColor
 **********************************************************************/
static void SetColor(GtkWidget *p_wPB, void *p_pvPlugin)
{
	gint i, j;
	struct plugin_t *poPlugin = (plugin_t *)p_pvPlugin;
	struct param_t *poConf = &(poPlugin->oConf.oParam);
	struct gui_t *poGUI = &(poPlugin->oConf.oGUI);
	GtkWidget *dialog;
	GtkColorSelection *colorsel;
	gint response;
	gint channel = -1, dir = -1;

	TRACE_IMONC("SetDisplayBars\n");

	for(i = 0; i < IMONC_MAX_CHANNELS; ++i)
	{
		for(j = 0; j < IMONC_MAX_DIRECTIONS; ++j)
		{
			if(p_wPB == poGUI->wPB_Color[i][j])
			{
				channel = i;
				dir = j;
				break;
			}
		}

		if(channel >= 0)
			break;
	}
	
	dialog = gtk_color_selection_dialog_new(_("Select color"));
	gtk_window_set_transient_for(GTK_WINDOW(dialog),
			GTK_WINDOW(poPlugin->oConf.wTopLevel));
	colorsel = GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(dialog)->colorsel);
	gtk_color_selection_set_previous_color(colorsel,
			&poConf->wBarColor[channel][dir]);
	gtk_color_selection_set_current_color(colorsel,
			&poConf->wBarColor[channel][dir]);
	gtk_color_selection_set_has_palette(colorsel, TRUE);

	response = gtk_dialog_run(GTK_DIALOG(dialog));

	if(response == GTK_RESPONSE_OK)
	{
		gtk_color_selection_get_current_color(colorsel,
				&poConf->wBarColor[channel][dir]);
		gtk_widget_modify_bg(poGUI->wDA_Color[channel][dir],
				GTK_STATE_NORMAL,
				&poConf->wBarColor[channel][dir]);
	}
	
	gtk_widget_destroy(dialog);
}
/**********************************************************************/


/**********************************************************************
 * UpdateConf
 **********************************************************************/
static void UpdateConf(GtkWidget *w, void *p_pvPlugin)
{
	struct plugin_t *poPlugin = (plugin_t *)p_pvPlugin;
	struct param_t *poConf = &(poPlugin->oConf.oParam);
	struct gui_t *poGUI = &(poPlugin->oConf.oGUI);

	TRACE_IMONC("UpdateConf()\n");
	SetHost(poGUI->wTF_Hostname, poPlugin);

	SetPort(poGUI->wTF_Port, poPlugin);
	SetPassword(poGUI->wTF_Password, poPlugin);
	SetAdminPassword(poGUI->wTF_AdminPassword, poPlugin);
	SetDisplayCommandbutton(poGUI->wCB_CommandbuttonShow, poPlugin);
	SetUpdateInterval(poGUI->wTF_Interval, poPlugin);

	if(poConf->iDisplayHostname == SHN_HIDE)
		SetDisplayHostname(poGUI->wRB_HostnameHide, poPlugin);
	if(poConf->iDisplayHostname == SHN_NAME)
		SetDisplayHostname(poGUI->wRB_HostnameName, poPlugin);
	if(poConf->iDisplayHostname == SHN_FQDN)
		SetDisplayHostname(poGUI->wRB_HostnameFQDN, poPlugin);

	if(poConf->iDisplayBars == SMB_NEVER)
		SetDisplayHostname(poGUI->wRB_BarsNever, poPlugin);
	if(poConf->iDisplayBars == SMB_CONNECTED)
		SetDisplayHostname(poGUI->wRB_BarsConnected, poPlugin);
	if(poConf->iDisplayBars == SMB_ALWAYS)
		SetDisplayHostname(poGUI->wRB_BarsAlways, poPlugin);
	
	SetTimer(poPlugin);
}
/**********************************************************************/


/**********************************************************************
 * About
 **********************************************************************/
static void About(GtkWidget *w, void *unused)
{
	xfce_info("%s %s - imond client plugin\n"
		  "Display fli4l router information "
		  "on Linux systems\n\n"
		  "(c) 2005, J. Busse <jb@dataline-gmbh.de>\n",
		  PACKAGE, VERSION);
}
/**********************************************************************/


/**********************************************************************
 * plugin_create_options
 **********************************************************************/
static void plugin_create_options(Control *p_poCtrl, 
				  GtkContainer *p_pxContainer,
				  GtkWidget *p_wDone)
{
	gint i, j;
	struct plugin_t *poPlugin = (plugin_t *)p_poCtrl->data;
	struct param_t *poConf = &(poPlugin->oConf.oParam);
	struct gui_t *poGUI = &(poPlugin->oConf.oGUI);
	gchar buf[32];

	TRACE_IMONC("plugin_create_options()\n");

	poPlugin->oConf.wTopLevel = gtk_widget_get_toplevel(p_wDone);

	CreateImoncConfigGUI(GTK_WIDGET(p_pxContainer), poGUI);

	g_signal_connect(GTK_WIDGET(poGUI->wPB_About), "clicked",
			 G_CALLBACK(About), 0);
	
	// Hostname
	gtk_entry_set_text(GTK_ENTRY(poGUI->wTF_Hostname), poConf->acHost);
	g_signal_connect(GTK_WIDGET(poGUI->wTF_Hostname), "activate",
			 G_CALLBACK(SetHost), poPlugin);

	// Port
	sprintf(buf, "%d", poConf->iPort);
	gtk_entry_set_text(GTK_ENTRY(poGUI->wTF_Port), buf);
	g_signal_connect(GTK_WIDGET(poGUI->wTF_Port), "activate",
			 G_CALLBACK(SetPort), poPlugin);

	// Password
	gtk_entry_set_text(GTK_ENTRY(poGUI->wTF_Password), poConf->acPassword);
	g_signal_connect(GTK_WIDGET(poGUI->wTF_Password), "activate",
			 G_CALLBACK(SetPassword), poPlugin);

	// Admin password
	gtk_entry_set_text(GTK_ENTRY(poGUI->wTF_AdminPassword), poConf->acAdminPassword);
	g_signal_connect(GTK_WIDGET(poGUI->wTF_AdminPassword), "activate",
			 G_CALLBACK(SetAdminPassword), poPlugin);

	// Update interval
	sprintf(buf, "%d", poPlugin->iTimerInterval);
	gtk_entry_set_text(GTK_ENTRY(poGUI->wTF_Interval), buf);
	g_signal_connect(GTK_WIDGET(poGUI->wTF_Interval), "activate",
			 G_CALLBACK(SetUpdateInterval), poPlugin);

	// Display command button & command entry
	gtk_toggle_button_set_active(
			GTK_TOGGLE_BUTTON(poGUI->wCB_CommandbuttonShow),
			poConf->bDisplayCommandbutton);
	gtk_widget_set_sensitive(
			GTK_WIDGET(poGUI->wTF_Command),
			poConf->bDisplayCommandbutton);
	gtk_entry_set_text(GTK_ENTRY(poGUI->wTF_Command), poConf->acCommand);
	g_signal_connect(GTK_WIDGET(poGUI->wCB_CommandbuttonShow), "toggled",
			 G_CALLBACK(SetDisplayCommandbutton), poPlugin);
	
	// Display hostname
	if(poConf->iDisplayHostname == SHN_HIDE)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(poGUI->wRB_HostnameHide), TRUE);
	if(poConf->iDisplayHostname == SHN_NAME)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(poGUI->wRB_HostnameName), TRUE);
	if(poConf->iDisplayHostname == SHN_FQDN)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(poGUI->wRB_HostnameFQDN), TRUE);
	g_signal_connect(GTK_WIDGET(poGUI->wRB_HostnameHide), "toggled",
			G_CALLBACK(SetDisplayHostname), poPlugin);
	g_signal_connect(GTK_WIDGET(poGUI->wRB_HostnameName), "toggled",
			G_CALLBACK(SetDisplayHostname), poPlugin);
	g_signal_connect(GTK_WIDGET(poGUI->wRB_HostnameFQDN), "toggled",
			G_CALLBACK(SetDisplayHostname), poPlugin);

	// Display monitor bars 
	if(poConf->iDisplayBars == SMB_NEVER)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(poGUI->wRB_BarsNever), TRUE);
	if(poConf->iDisplayBars == SMB_CONNECTED)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(poGUI->wRB_BarsConnected), TRUE);
	if(poConf->iDisplayBars == SMB_ALWAYS)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(poGUI->wRB_BarsAlways), TRUE);
	g_signal_connect(GTK_WIDGET(poGUI->wRB_BarsNever), "toggled",
			G_CALLBACK(SetDisplayBars), poPlugin);
	g_signal_connect(GTK_WIDGET(poGUI->wRB_BarsConnected), "toggled",
			G_CALLBACK(SetDisplayBars), poPlugin);
	g_signal_connect(GTK_WIDGET(poGUI->wRB_BarsAlways), "toggled",
			G_CALLBACK(SetDisplayBars), poPlugin);

	for(i = 0; i < IMONC_MAX_CHANNELS; ++i)
	{
		for(j = 0; j < IMONC_MAX_DIRECTIONS; ++j)
		{
			g_signal_connect(GTK_WIDGET(poGUI->wPB_Color[i][j]), "clicked",
						G_CALLBACK(SetColor), poPlugin);
			gtk_widget_modify_bg(poGUI->wDA_Color[i][j], GTK_STATE_NORMAL,
					&poConf->wBarColor[i][j]);
			gtk_widget_set_size_request(poGUI->wDA_Color[i][j], 64, 12);
		}
	}
	
	// DONE
	g_signal_connect(GTK_WIDGET(p_wDone), "clicked",
			 G_CALLBACK(UpdateConf), poPlugin);
}
/**********************************************************************/


/**********************************************************************
 * plugin_attach_callback
 **********************************************************************/
static void plugin_attach_callback(Control *ctrl, const gchar *signal,
				   GCallback cb, gpointer data)
{
	TRACE_IMONC("plugin_attach_callback()\n");
}
/**********************************************************************/


/**********************************************************************
 * plugin_set_size
 **********************************************************************/
static void plugin_set_size(Control *p_poCtrl, gint p_size)
{
	struct plugin_t *poPlugin = (plugin_t *)p_poCtrl->data;
	struct monitor_t *poMonitor = &(poPlugin->oMonitor);
	gint i, j;
	gint size1, size2;

	TRACE_IMONC("plugin_set_size()\n");

	if(settings.orientation == HORIZONTAL)
	{
		size1 = 6 + 2 * p_size;
		size2 = icon_size[p_size];
	}
	else
	{
		size1 = icon_size[p_size];
		size2 = 6 + 2 * p_size;
	}

	for(i = 0; i < IMONC_MAX_CHANNELS; ++i)
	{
		for(j = 0; j < IMONC_MAX_DIRECTIONS; ++j)
		{
			gtk_widget_set_size_request(
					GTK_WIDGET(poMonitor->wBar[i][j]),
					size1, size2);
			gtk_widget_queue_resize(
					GTK_WIDGET(poMonitor->wBar[i][j]));
		}
	}
}
/**********************************************************************/


/**********************************************************************
 * plugin_set_orientation
 **********************************************************************/
static void plugin_set_orientation(Control *p_poCtrl, gint p_iOrientation)
{
	struct plugin_t *poPlugin = (plugin_t *)p_poCtrl->data;
	struct monitor_t *poMonitor = &(poPlugin->oMonitor);

	TRACE_IMONC("plugin_set_orientation()\n");

	if(poPlugin->iTimerId)
	{
		g_source_remove(poPlugin->iTimerId);
		poPlugin->iTimerId = 0;
	}

	if(GTK_IS_WIDGET(poMonitor->hBox))
		gtk_container_remove(GTK_CONTAINER(poMonitor->wEventBox),
			GTK_WIDGET(poMonitor->hBox));

	CreateMonitorBars(poPlugin, p_iOrientation);
	SetTimer(poPlugin);
}
/**********************************************************************/


/**********************************************************************
 * xfce_control_class_init
 **********************************************************************/
G_MODULE_EXPORT void xfce_control_class_init(ControlClass *cc)
{
	TRACE_IMONC("xfce_control_class_init()\n");

	cc->name = PLUGIN_NAME;
	cc->caption = _("Imonc for fli4l");
	cc->create_control = (CreateControlFunc)plugin_create_control;
	cc->free = plugin_free;
	cc->attach_callback = plugin_attach_callback;
	cc->read_config = plugin_read_config;
	cc->write_config = plugin_write_config;
	cc->create_options = plugin_create_options;
	cc->set_size = plugin_set_size;
	cc->set_orientation = plugin_set_orientation;
	cc->set_theme = NULL;
}
/**********************************************************************/


/**********************************************************************
 * XFCE_PLUGIN_CHECK_INIT
 **********************************************************************/
XFCE_PLUGIN_CHECK_INIT
/**********************************************************************/
