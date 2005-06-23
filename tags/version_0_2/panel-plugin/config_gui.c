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

#include <pango/pango.h>
#include <gtk/gtk.h>

#include <libxfce4util/i18n.h>
#include <libxfcegui4/dialogs.h>
#include <panel/plugins.h>
#include <panel/xfce.h>

#include "imonc.h"
#include "config_gui.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


static void config_gui_add_separator(GtkBox *p_Box);
static void config_gui_add_about_button(GtkBox *p_Box,
		GtkTooltips *p_Tooltips, struct gui_t *p_poGUI);
static void config_gui_add_connection_settings(GtkBox *p_Box,
		GtkTooltips *p_Tooltips, struct gui_t *p_poGUI);
static void config_gui_add_display_settings(GtkBox *p_Box,
		GtkTooltips *p_Tooltips, struct gui_t *p_poGUI);

static GtkWidget *config_gui_add_label(GtkTable *p_Table,
		gchar *p_lText, int p_lX1, int p_lX2, int p_lY1, int p_lY2);
static GtkWidget *config_gui_add_radio_button(GtkTable *p_pTable,
		void **p_pRadio, GtkRadioButton *p_pGroup, gchar *p_pText,
		gint p_iX1, gint p_iX2, gint p_iY1, gint p_iY2,
		GtkTooltips *p_Tooltips, gchar *p_pTooltext,
		gboolean p_bActive);
static GtkWidget *config_gui_add_labeled_entry(GtkTable *p_pTable,
		gchar *p_lText, int p_lX1, int p_lX2, int p_lY1, int p_lY2,
		gchar *p_eText, int p_eX1, int p_eX2, int p_eY1, int p_eY2,
		GtkTooltips *p_Tooltips, gchar *p_Text,
		void **p_pEntry, gboolean visibility);
static void config_gui_add_checked_entry(GtkTable *p_pTable,
		gchar *p_cText, int p_cX1, int p_cX2, int p_cY1, int p_cY2,
		gchar *p_eText, int p_eX1, int p_eX2, int p_eY1, int p_eY2,
		GtkTooltips *p_Tooltips, gchar *p_cTooltext, gchar *p_eTooltext,
		void **p_pCheckbox, void **p_pEntry);


/**********************************************************************
 * CreateImoncConfigGUI
 **********************************************************************/
int CreateImoncConfigGUI (GtkWidget *OptionDialog, struct gui_t *p_poGUI)
{
	GtkWidget *vbox;
	GtkTooltips *tooltips;

	tooltips = gtk_tooltips_new();

	// Create the underlying vertical box
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox);
	gtk_box_set_spacing(GTK_BOX(vbox), 5);
	gtk_container_add(GTK_CONTAINER(OptionDialog), vbox);

	// Add the about button
	config_gui_add_about_button(GTK_BOX(vbox), 
			GTK_TOOLTIPS(tooltips), p_poGUI);

	// Add separator
	config_gui_add_separator(GTK_BOX(vbox));

	// Add connection settings
	config_gui_add_connection_settings(GTK_BOX(vbox), 
			GTK_TOOLTIPS(tooltips), p_poGUI);

	// Add separator
	config_gui_add_separator(GTK_BOX(vbox));

	// Add display settings
	config_gui_add_display_settings(GTK_BOX(vbox), 
			GTK_TOOLTIPS(tooltips), p_poGUI);

	// Add separator
	config_gui_add_separator(GTK_BOX(vbox));

	return(0);
}
/**********************************************************************/


/**********************************************************************
 * Add an separator
 **********************************************************************/
static void config_gui_add_separator(GtkBox *p_Box)
{
	GtkWidget *separator;
	
	separator = gtk_hseparator_new();
	gtk_widget_show(separator);
	gtk_box_pack_start(p_Box, separator, TRUE, TRUE, 0);
}
/**********************************************************************/


/**********************************************************************
 * Add the About button
 **********************************************************************/
static void config_gui_add_about_button(GtkBox *p_Box,
		GtkTooltips *p_Tooltips, struct gui_t *p_poGUI)
{
	GtkWidget *alignment;
	GtkWidget *hbox;
	GtkWidget *image;
	GtkWidget *label;
	
	// About button
	p_poGUI->wPB_About = gtk_button_new();
	gtk_widget_show(p_poGUI->wPB_About);
	gtk_box_pack_start(p_Box, p_poGUI->wPB_About, FALSE, FALSE, 0);
	GTK_WIDGET_UNSET_FLAGS(p_poGUI->wPB_About, GTK_CAN_FOCUS);
	gtk_tooltips_set_tip(p_Tooltips, p_poGUI->wPB_About, _("About..."), NULL);
	gtk_button_set_relief(GTK_BUTTON(p_poGUI->wPB_About), GTK_RELIEF_NONE);

	alignment = gtk_alignment_new(0.5, 0.5, 0, 0);
	gtk_widget_show(alignment);
	gtk_container_add(GTK_CONTAINER(p_poGUI->wPB_About), alignment);

	hbox = gtk_hbox_new(FALSE, 2);
	gtk_widget_show(hbox);
	gtk_container_add(GTK_CONTAINER(alignment), hbox);

	image = gtk_image_new_from_stock("gtk-network", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show(image);
	gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);

	label = gtk_label_new_with_mnemonic(_("imonc for fli4l"));
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);

}
/**********************************************************************/

/**********************************************************************
 * Add a label 
 **********************************************************************/
static GtkWidget *config_gui_add_label(GtkTable *p_pTable,
		gchar *p_lText, int p_lX1, int p_lX2, int p_lY1, int p_lY2)
{
	GtkWidget *label;
	
	label = gtk_label_new(p_lText);
	gtk_widget_show(label);
	gtk_table_attach(p_pTable, label, p_lX1, p_lX2, p_lY1, p_lY2,
			(GtkAttachOptions)(GTK_FILL),
			(GtkAttachOptions)(0), 5, 2);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);

	return(label);
}
/**********************************************************************/


/**********************************************************************
 * Add a radio button
 **********************************************************************/
static GtkWidget *config_gui_add_radio_button(GtkTable *p_pTable,
		void **p_pRadio, GtkRadioButton *p_pGroup, gchar *p_pText,
		gint p_iX1, gint p_iX2, gint p_iY1, gint p_iY2,
		GtkTooltips *p_Tooltips, gchar *p_pTooltext,
		gboolean p_bActive)
{
	if(p_pGroup == NULL)
		*p_pRadio = gtk_radio_button_new_with_label(NULL, p_pText);
	else
		*p_pRadio = gtk_radio_button_new_with_label(
				gtk_radio_button_get_group(p_pGroup),
				p_pText);

	gtk_table_attach(p_pTable, *p_pRadio,
			p_iX1, p_iX2, p_iY1, p_iY2,
			(GtkAttachOptions)(GTK_FILL),
			(GtkAttachOptions)(0), 5, 2);
	gtk_tooltips_set_tip(p_Tooltips, *p_pRadio, p_pTooltext, NULL);
	gtk_widget_show(*p_pRadio);

	if(p_bActive)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(*p_pRadio), TRUE);

	return(*p_pRadio);
}
/**********************************************************************/

	
/**********************************************************************
 * Add a labeled entry
 **********************************************************************/
static GtkWidget *config_gui_add_labeled_entry(GtkTable *p_pTable,
		gchar *p_lText, int p_lX1, int p_lX2, int p_lY1, int p_lY2,
		gchar *p_eText, int p_eX1, int p_eX2, int p_eY1, int p_eY2,
		GtkTooltips *p_Tooltips, gchar *p_Text,
		void **p_pEntry, gboolean visibility)
{
	GtkWidget *label;
	
	label = config_gui_add_label(p_pTable,
			p_lText, p_lX1, p_lX2, p_lY1, p_lY2);

	*p_pEntry = gtk_entry_new();
	gtk_widget_show(*p_pEntry);
	gtk_table_attach(p_pTable, *p_pEntry, p_eX1, p_eX2, p_eY1, p_eY2,
			 (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions)(0), 5, 2);
	gtk_tooltips_set_tip(p_Tooltips, *p_pEntry, p_Text, NULL);
	gtk_entry_set_max_length(GTK_ENTRY(*p_pEntry), 64);
	gtk_entry_set_visibility(GTK_ENTRY(*p_pEntry), visibility);
	gtk_entry_set_text(GTK_ENTRY(*p_pEntry), p_eText);

	return(label);
}
/**********************************************************************/

/**********************************************************************
 * Add a checked entry
 **********************************************************************/
static void config_gui_add_checked_entry(GtkTable *p_pTable,
		gchar *p_cText, int p_cX1, int p_cX2, int p_cY1, int p_cY2,
		gchar *p_eText, int p_eX1, int p_eX2, int p_eY1, int p_eY2,
		GtkTooltips *p_Tooltips, gchar *p_cTooltext, gchar *p_eTooltext,
		void **p_pCheckbox, void **p_pEntry)
{
	*p_pCheckbox = gtk_check_button_new_with_label(p_cText);
	gtk_widget_show(*p_pCheckbox);
	gtk_table_attach(p_pTable, *p_pCheckbox, p_cX1, p_cX2, p_cY1, p_cY2,
			 (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions)(0), 5, 2);
	gtk_tooltips_set_tip(p_Tooltips, *p_pCheckbox, p_cTooltext, NULL);
	
	*p_pEntry = gtk_entry_new();
	gtk_widget_show(*p_pEntry);
	gtk_table_attach(p_pTable, *p_pEntry, p_eX1, p_eX2, p_eY1, p_eY2,
			 (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions)(0), 5, 2);
	gtk_tooltips_set_tip(p_Tooltips, *p_pEntry, p_eTooltext, NULL);
	gtk_entry_set_max_length(GTK_ENTRY(*p_pEntry), 64);
	gtk_entry_set_text(GTK_ENTRY(*p_pEntry), p_eText);
}
/**********************************************************************/


/**********************************************************************
 * Add the connection settings
 **********************************************************************/
static void config_gui_add_connection_settings(GtkBox *p_Box,
		GtkTooltips *p_Tooltips, struct gui_t *p_poGUI)
{
	GtkWidget *table;
	GtkWidget *label;
	
	// Table for entry fields etc.
	table = gtk_table_new(5, 2, FALSE);
	gtk_widget_show(table);
	gtk_box_pack_start(p_Box, table, FALSE, TRUE, 0);

	// label: connection settings
	label = config_gui_add_label(GTK_TABLE(table),
			_("Connection settings:"), 0, 2, 0, 1);
	gtk_widget_modify_font(GTK_WIDGET(label), 
			pango_font_description_from_string("bold"));

	gtk_table_set_row_spacing(GTK_TABLE(table), 0 ,5);
	
	// Host
	config_gui_add_labeled_entry(GTK_TABLE(table),
			_("Hostname:"), 0, 1, 1, 2,
			"fli4l", 1, 2, 1, 2,
			p_Tooltips,
			_("Input the host name of the fli4l router, then press <Enter>"),
			(void **)&(p_poGUI->wTF_Hostname), TRUE);

	// Port
	config_gui_add_labeled_entry(GTK_TABLE(table),
			_("Port:"), 0, 1, 2, 3,
			"5000", 1, 2, 2, 3,
			p_Tooltips,
			_("Input the port of the imond, then press <Enter>"),
			(void **)&(p_poGUI->wTF_Port), TRUE);
			
	// Password
	config_gui_add_labeled_entry(GTK_TABLE(table),
			_("Password:"), 0, 1, 3, 4,
			"", 1, 2, 3, 4,
			p_Tooltips,
			_("Input the password to connect to the imond, then press <Enter>"),
			(void **)&(p_poGUI->wTF_Password), FALSE);
			
	// Admin Password
	config_gui_add_labeled_entry(GTK_TABLE(table),
			_("Admin password:"), 0, 1, 4, 5,
			"", 1, 2, 4, 5,
			p_Tooltips,
			_("Input the admin password to connect to the imond, then press <Enter>"),
			(void **)&(p_poGUI->wTF_AdminPassword), FALSE);
}
/**********************************************************************/


/**********************************************************************
 * Add the display settings
 **********************************************************************/
static void config_gui_add_display_settings(GtkBox *p_Box,
		GtkTooltips *p_Tooltips, struct gui_t *p_poGUI)
{
	int i, j;
	char buf[32];
	GtkWidget *table;
	GtkWidget *label;
	GtkWidget *radio;
	
	// Table for entry fields etc.
	table = gtk_table_new(5, 6 + (IMONC_MAX_CHANNELS * 2), FALSE);
	gtk_widget_show(table);
	gtk_box_pack_start(p_Box, table, FALSE, TRUE, 0);

	// label: display settings
	label = config_gui_add_label(GTK_TABLE(table),
			_("Display settings:"), 0, 5, 0, 1);
	gtk_widget_modify_font(GTK_WIDGET(label), 
			pango_font_description_from_string("bold"));
		
	gtk_table_set_row_spacing(GTK_TABLE(table), 0, 5);
	
	// Update interval
	config_gui_add_labeled_entry(GTK_TABLE(table),
			_("Update interval:"), 0, 2, 1, 2,
			"1000", 2, 4, 1, 2,
			p_Tooltips,
			_("Input the update interval in milliseconds, then press <Enter>"),
			(void **)&(p_poGUI->wTF_Interval), TRUE);
	
	config_gui_add_label(GTK_TABLE(table),
			_("ms"), 4, 5, 1, 2);

	// Command button
	config_gui_add_checked_entry(GTK_TABLE(table),
			_("Command button:"), 0, 2, 2, 3,
			"/usr/bin/gtk-imonc -p %P %h %p", 2, 5, 2, 3,
			p_Tooltips,
			_("Show button to execute command"),
			_("Enter command to execute when button is clicked. " \
				"(%h => hostname, %p => port, %P => password)"),
			(void **)&(p_poGUI->wCB_CommandbuttonShow),
			(void **)&(p_poGUI->wTF_Command));
	
	// Display hostname
	config_gui_add_label(GTK_TABLE(table),
			_("Display hostname:"), 0, 2, 3, 4);

	radio = config_gui_add_radio_button(GTK_TABLE(table),
			(void **)&(p_poGUI->wRB_HostnameHide),
			NULL,
			_("No"), 2, 3, 3, 4,
			p_Tooltips,
			_("Do not show the hostname"),
			FALSE);
	radio = config_gui_add_radio_button(GTK_TABLE(table),
			(void **)&(p_poGUI->wRB_HostnameName),
			GTK_RADIO_BUTTON(radio), 
			_("Hostname"), 3, 4, 3, 4,
			p_Tooltips,
			_("Show only the hostname"),
			TRUE);
	radio = config_gui_add_radio_button(GTK_TABLE(table),
			(void **)&(p_poGUI->wRB_HostnameFQDN),
			GTK_RADIO_BUTTON(radio), 
			_("FQDN"), 4, 5, 3, 4,
			p_Tooltips,
			_("Show the fully qualified domainname"),
			FALSE);

	// Display Monitorbars
	config_gui_add_label(GTK_TABLE(table),
			_("Display bars:"), 0, 2, 4, 5);

	radio = config_gui_add_radio_button(GTK_TABLE(table),
			(void **)&(p_poGUI->wRB_BarsNever),
			NULL,
			_("Never"), 2, 3, 4, 5,
			p_Tooltips,
			_("Do never show the monitor bars"),
			FALSE);
	radio = config_gui_add_radio_button(GTK_TABLE(table),
			(void **)&(p_poGUI->wRB_BarsConnected),
			GTK_RADIO_BUTTON(radio), 
			_("Connected"), 3, 4, 4, 5,
			p_Tooltips,
			_("Show the monitor bars only when connected"),
			TRUE);
	radio = config_gui_add_radio_button(GTK_TABLE(table),
			(void **)&(p_poGUI->wRB_BarsAlways),
			GTK_RADIO_BUTTON(radio), 
			_("Always"), 4, 5, 4, 5,
			p_Tooltips,
			_("Show always the monitor bars"),
			FALSE);
	
	// Monitorbar colors
	config_gui_add_label(GTK_TABLE(table),
			_("Bar colors:"), 0, 2, 5, 6);

	for(i = 0; i < IMONC_MAX_CHANNELS; ++i)
	{
		// Channels
		sprintf(buf, "Channel %d", i + 1);
		config_gui_add_label(GTK_TABLE(table),
				_(buf), 
				1, 2, 
				6 + (i * 2), 7 + (i * 2));

		for(j = 0; j < IMONC_MAX_DIRECTIONS; ++j)
		{
			sprintf(buf, "%s", (j % 2) ? _("Out:") : _("In:"));
			config_gui_add_label(GTK_TABLE(table),
					_(buf),
					2, 3,
					6 + (i * 2 + j), 7 + (i * 2 + j));

			p_poGUI->wPB_Color[i][j] = gtk_button_new();
			p_poGUI->wDA_Color[i][j] = gtk_drawing_area_new();
			gtk_container_add(GTK_CONTAINER(p_poGUI->wPB_Color[i][j]),
					p_poGUI->wDA_Color[i][j]);
			gtk_widget_show(GTK_WIDGET(p_poGUI->wPB_Color[i][j]));
			gtk_widget_show(GTK_WIDGET(p_poGUI->wDA_Color[i][j]));
			gtk_table_attach(GTK_TABLE(table),
					p_poGUI->wPB_Color[i][j],
					3, 4, 
					6 + (i * 2 + j), 7 + (i * 2 + j),
					(GtkAttachOptions)(GTK_FILL),
					(GtkAttachOptions)(0), 5, 2);
		}
	}
}
/**********************************************************************/
