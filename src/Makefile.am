#
# Makefile.am for openwsman-yast/src
#
plugindir = @WSMAN_PLUGIN_DIR@
y2plugindir = $(libdir)/YaST2/plugin

AM_CFLAGS = -DWSMAN_VERSION=\"@WSMAN_VERSION@\"

YAST2CORE =				\
	$(y2plugindir)/libpy2wfm.la	\
	$(y2plugindir)/libpy2scr.la	\
	$(libdir)/libycp.la		\
	$(libdir)/liby2.la		\
	$(libdir)/libyui.la

INCLUDES = \
	-I$(top_srcdir)/src/openwsman	\
	-I/usr/include/openwsman	\
	-I/usr/include/YaST2

noinst_HEADERS = WFM.h

#
# do not add any libraries needed by a plugin.
# link the plugin against any special library.
#

libwsman_yast_plugin_la_SOURCES = \
	yast.c			\
	yast_stub_identify.c	\
	yast_stub_custom.c	\
	yast_yast.cc		\
	yast.h

LIBS = \
    $(XML_LIBS) 

plugin_LTLIBRARIES= libwsman_yast_plugin.la

libwsman_yast_plugin_la_LDFLAGS= -version-info 1:0	\
	-Xlinker --whole-archive	\
	$(PERL_LDFLAGS)			\
	$(YAST2CORE)			\
	-Xlinker --no-whole-archive

libwsman_yast_plugin_la_LIBADD =       \
        $(XML_LIBS)	\
	-lpthread	\
	-ly2util
