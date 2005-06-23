plugindir = @XFCE4_PANEL_PLUGINSDIR@

plugin_LTLIBRARIES =			\
	libimonc.la

libimonc_la_LDFLAGS =			\
	-avoid-version			\
	-export-dynamic			\
	-module

libimonc_la_SOURCES =			\
	main.c				\
	config_gui.c			\
	config_gui.h			\
	imonc.c				\
	imonc.h				\
	wrapper.c			\
	wrapper.h			\
	md5.c				\
	md5.h				\
	debug.c				\
	debug.h

libimonc_la_CFLAGS =			\
	@XFCE4_PANEL_CFLAGS@

libimonc_la_LIBADD = 			\
	@XFCE4_PANEL_LIBS@
