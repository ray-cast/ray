## Process this file with automake to produce Makefile.in
MAINTAINERCLEANFILES = Makefile.in
ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = 1.3 foreign no-dependencies

# include the app subdirectories in the distribution
EXTRA_DIST = makefiles doc contrib


# libmng release @VERSION@
libmng_la_LDFLAGS = \
	-version-number @MNG_MAJOR_NR@:@MNG_MINOR_NR@:@MNG_RELEASE_NR@

lib_LTLIBRARIES = libmng.la

include_HEADERS = libmng.h libmng_conf.h libmng_types.h
noinst_HEADERS = libmng_chunk_io.h libmng_chunk_prc.h libmng_chunks.h \
	libmng_cms.h libmng_data.h libmng_display.h libmng_dither.h \
	libmng_error.h libmng_filter.h libmng_jpeg.h libmng_memory.h \
	libmng_object_prc.h libmng_objects.h libmng_pixels.h \
	libmng_read.h libmng_trace.h libmng_write.h libmng_zlib.h

libmng_la_SOURCES = libmng_callback_xs.c libmng_chunk_io.c \
	libmng_chunk_prc.c libmng_chunk_xs.c libmng_cms.c \
	libmng_display.c libmng_dither.c libmng_error.c \
	libmng_filter.c libmng_hlapi.c libmng_jpeg.c \
	libmng_object_prc.c libmng_pixels.c libmng_prop_xs.c \
	libmng_read.c libmng_trace.c libmng_write.c libmng_zlib.c 

man_MANS = doc/man/libmng.3 doc/man/jng.5 doc/man/mng.5

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libmng.pc

uninstall-hook:
	rm -f $(DESTDIR)$(pkgconfigdir)/libmng.pc
	rm -f $(DESTDIR)$(includedir)/libmng.h
	rm -f $(DESTDIR)$(includedir)/libmng_conf.h
	rm -f $(DESTDIR)$(includedir)/libmng_types.h
