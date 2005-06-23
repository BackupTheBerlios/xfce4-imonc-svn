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

#ifndef _config_gui_h
#define _config_gui_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>

#include <libxfce4util/i18n.h>
#include <libxfcegui4/dialogs.h>
#include <panel/plugins.h>
#include <panel/xfce.h>

#include "imonc.h"

#define SHN_HIDE	0		// Show hostname - no
#define SHN_NAME	1		// SHow hostname - hostname
#define SHN_FQDN	2		// Show hostname - fqdn

#define SMB_NEVER	0		// Show monitor bars - never
#define SMB_CONNECTED	1		// Show monitor bars - connected
#define SMB_ALWAYS	2		// Show monitor bars - always


/* Configuration GUI widgets */
typedef struct gui_t
{
	GtkWidget *wPB_About;
	// Connection settings
	GtkWidget *wTF_Hostname;
	GtkWidget *wTF_Port;
	GtkWidget *wTF_Password;
	GtkWidget *wTF_AdminPassword;
	// Display settings
	GtkWidget *wTF_Interval;
	GtkWidget *wCB_CommandbuttonShow;
	GtkWidget *wTF_Command;
	GtkWidget *wRB_HostnameHide;
	GtkWidget *wRB_HostnameName;
	GtkWidget *wRB_HostnameFQDN;
	GtkWidget *wRB_BarsNever;
	GtkWidget *wRB_BarsConnected;
	GtkWidget *wRB_BarsAlways;
	GtkWidget *wPB_Color[IMONC_MAX_CHANNELS][IMONC_MAX_DIRECTIONS];
	GtkWidget *wDA_Color[IMONC_MAX_CHANNELS][IMONC_MAX_DIRECTIONS];
} gui_t;


#ifdef __cplusplus
extern          "C" {
#endif

int CreateImoncConfigGUI(GtkWidget *ParentWindow, struct gui_t *gui);

#ifdef __cplusplus
}				/* extern "C" */
#endif

#endif				/* _config_gui_h */
