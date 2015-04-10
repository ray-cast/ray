dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.65)
#
define([MAJ_NR],[2])
define([MIN_NR],[0])
define([REL_NR],[2])
#
#AC_INIT (PACKAGE, VERSION, [BUG-REPORT], [TARNAME], [URL])
AC_INIT( [libmng],[MAJ_NR.MIN_NR.REL_NR],
[png-mng-implement@lists.sourceforge.net],
[libmng-MAJ_NR.MIN_NR.REL_NR],
[http://www.libmng.com/])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([libmng_hlapi.c])
#
AC_CONFIG_HEADERS(config.h)
#
MNG_MAJOR_NR=MAJ_NR
MNG_MINOR_NR=MIN_NR
MNG_RELEASE_NR=REL_NR
#
AC_SUBST(MNG_MAJOR_NR)
AC_SUBST(MNG_MINOR_NR)
AC_SUBST(MNG_RELEASE_NR)
#
AM_INIT_AUTOMAKE([-Wall -Werror])

dnl pass the version string on the the makefiles
AC_SUBST(PACKAGE)
AC_SUBST(VERSION)
AC_DEFINE([MNG_VERSION_MAJOR],[MAJ_NR],[MAJOR number of version])
AC_DEFINE([MNG_VERSION_MINOR],[MIN_NR],[MINOR number of version])
AC_DEFINE([MNG_VERSION_RELEASE],[REL_NR],[PATCH number of version])
AC_DEFINE([MNG_VERSION_SO],[MAJ_NR],[eg. libmng.so.1])
AC_DEFINE([MNG_VERSION_DLL],[MAJ_NR],[ but: libmng.dll (!)])

dnl Checks for programs.
AC_PROG_CC
AC_ISC_POSIX
AM_PROG_LIBTOOL
AC_PROG_INSTALL
AC_LIBTOOL_WIN32_DLL
#
AC_C_BIGENDIAN
#
dnl support for files >2GB
AC_SYS_LARGEFILE

dnl Check for required header files
AC_HEADER_STDC

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

dnl need pow and fabs
AC_CHECK_FUNC(pow, , AC_CHECK_LIB(m, pow, LIBS="$LIBS -lm"))


dnl what functionality we want to add (read, write, display).
dnl all on by default. see libmng_conf.h for full descriptions

dnl not building a standard shared object?
AC_ARG_ENABLE(buildso,
[  --disable-buildso       disable building standard shared object])
if test "x$enable_buildso" != "xno"; then
  AC_DEFINE([MNG_BUILD_SO], [1], [enable building standard shared object])
fi

dnl we only support the full mng spec for now (no LC or VLC)
AC_DEFINE([MNG_SUPPORT_FULL], [1], [define if you want full mng support])

dnl remove support in library to read images?
AC_ARG_ENABLE(read,
[  --disable-read          remove read support from library])
if test "x$enable_read" != "xno"; then
 AC_DEFINE([MNG_SUPPORT_READ], [1], [define if you want read support])
fi

dnl remove support in library to write images?
AC_ARG_ENABLE(write,
[  --disable-write         remove write support from library])
if test "x$enable_write" != "xno"; then
  AC_DEFINE([MNG_SUPPORT_WRITE], [1], [define if you want write support])
fi

dnl remove support in library to display images?
AC_ARG_ENABLE(display,
[  --disable-display       remove display support from library])
if test "x$enable_display" != "xno"; then
  AC_DEFINE([MNG_SUPPORT_DISPLAY], [1], [define if you want display support])
fi

dnl remove support for 'dynamic' MNG?
AC_ARG_ENABLE(dynamic,
[  --disable-dynamic       remove dynamic MNG support from library])
if test "x$enable_dynamic" != "xno"; then
 AC_DEFINE([MNG_SUPPORT_DYNAMICMNG], [1], [define if you want dynamic support])
fi

dnl remove support in library to access chunks?
AC_ARG_ENABLE(chunks,
[  --disable-chunks        remove support for chunk access])
if test "x$enable_chunks" != "xno"; then
 AC_DEFINE([MNG_ACCESS_CHUNKS], [1], [define if you want chunk access support])
fi

dnl disable support for accessing chunks that have been previously read?
AC_ARG_ENABLE(storechunks,
[  --disable-storechunks   remove support for access of previous chunks])
if test "x$enable_storechunks" != "xno"; then
  AC_DEFINE([MNG_STORE_CHUNKS], [1], [enable support for accessing chunks])
fi

dnl enable support for debug tracing callbacks and messages?
AC_ARG_ENABLE(trace,
[  --enable-trace          include support for debug tracing callbacks],[
if test "x$enable_trace" = "xyes"; then
  AC_DEFINE([MNG_SUPPORT_TRACE], [1], [enable support for debug tracing])
  AC_DEFINE([MNG_TRACE_TELLTALE], [1], [enable support for debug messages])
fi
])

dnl verbose error text
dnl this should always be on
AC_DEFINE([MNG_ERROR_TELLTALE], [1], [enable verbose error text])


dnl libz is required.
AC_ARG_WITH(zlib,
[  --with-zlib[=DIR]       use zlib include/library files in DIR],[
  if test -d "$withval"; then
    CPPFLAGS="$CPPFLAGS -I$withval/include"
    LDFLAGS="$LDFLAGS -L$withval/lib"
  fi
])
AC_CHECK_HEADER(zlib.h,
    AC_CHECK_LIB(z, gzread, , AC_MSG_ERROR(zlib library not found)),
    AC_MSG_ERROR(zlib header not found)
)

dnl check for jpeg library
AC_ARG_WITH(jpeg,
[  --with-jpeg[=DIR]       use jpeg include/library files in DIR],
[with_jpeg=$withval],[with_jpeg=_auto])

  if test "x$with_jpeg" != "xno" -a "x$with_jpeg" != "xyes" -a \
	"x$with_jpeg" != "x_auto"; then
    # Save in case test with directory specified fails
    _cppflags=${CPPFLAGS}
    _ldflags=${LDFLAGS}
    _restore=1

    CPPFLAGS="${CPPFLAGS} -I$withval/include"
    LDFLAGS="${LDFLAGS} -L$withval/lib"
  else
    _restore=0
  fi

  if test "x$with_jpeg" != "xno"; then
    AC_CHECK_HEADER(jpeglib.h,
      AC_CHECK_LIB(jpeg, jpeg_read_header, [
	LIBS="$LIBS -ljpeg"
	AC_DEFINE([HAVE_LIBJPEG], [1], [define if you want JPEG support])
	_restore=0
      ],
    	AC_MSG_WARN(jpeg library not found)),
      AC_MSG_WARN(jpeg header not found)
    )
  fi

  test $_restore -eq 1 && CPPFLAGS=$_cppflags LDFLAGS=$_ldflags

dnl check for lcms2 library
AC_ARG_WITH(lcms2,
[  --with-lcms2[=DIR]       use lcms2 include/library files in DIR],
[with_lcms2=$withval],[with_lcms2=_auto])

  if test "x$with_lcms2" != "xno" -a "x$with_lcms2" != "xyes" -a \
	"x$with_lcms2" != "x_auto"; then
    # Save in case test with directory specified fails
    _cppflags=$CPPFLAGS
    _ldflags=$LDFLAGS
    _restore=1

    CPPFLAGS="$CPPFLAGS -I$withval/include"
    LDFLAGS="$LDFLAGS -L$withval/lib"
  else
    _restore=0
  fi

  if test "x$with_lcms2" != "xno"; then
    AC_CHECK_HEADER(lcms2.h, [
      have_lcms2=yes
      AC_CHECK_LIB(lcms2, cmsFreeToneCurve, [
        LIBS="$LIBS -llcms2"
        AC_DEFINE([HAVE_LIBLCMS2], [1], [define if you want lcms v2 support])
        dnl for now this implies MNG_INCLUDE_LCMS in the headers:
        AC_DEFINE([MNG_FULL_CMS], [1], [define if you want full lcms support])
	_restore=0
	have_lcms2=yes
      ],[
	have_lcms2=no
      ])
    ])
    dnl give feedback only if the user asked specifically for lcms2
    if test "x$with_lcms2" != "x_auto" -a "x$have_lcms2" != "xyes"; then
      AC_MSG_WARN([lcms2 not found... disabling LCMS v2 support])
    fi
  fi

if test "x$with_lcms2" != "x_auto" -a "x$have_lcms2" != "xyes"; then
dnl check for lcms library
AC_ARG_WITH(lcms,
[  --with-lcms[=DIR]       use lcms include/library files in DIR],
[with_lcms=$withval],[with_lcms=_auto])

  if test "x$with_lcms" != "xno" -a "x$with_lcms" != "xyes" -a \
	"x$with_lcms" != "x_auto"; then
    # Save in case test with directory specified fails
    _cppflags=$CPPFLAGS
    _ldflags=$LDFLAGS
    _restore=1

    CPPFLAGS="$CPPFLAGS -I$withval/include"
    LDFLAGS="$LDFLAGS -L$withval/lib"
  else
    _restore=0
  fi

  if test "x$with_lcms" != "xno"; then
    AC_CHECK_HEADER(lcms.h, [
      have_lcms=yes
      AC_CHECK_LIB(lcms, cmsCreateRGBProfile, [
        LIBS="$LIBS -llcms"
        AC_DEFINE([HAVE_LIBLCMS1], [1], [define if you want lcms v1 support])
        dnl for now this implies MNG_INCLUDE_LCMS in the headers:
        AC_DEFINE([MNG_FULL_CMS], [1], [define if you want full lcms support])
	_restore=0
	have_lcms=yes
      ],[
	have_lcms=no
      ])
    ])
    dnl give feedback only if the user asked specifically for lcms
    if test "x$with_lcms" != "x_auto" -a "x$have_lcms" != "xyes"; then
      AC_MSG_WARN([lcms not found... disabling LCMS v1 support])
    fi
  fi
fi
  test $_restore -eq 1 && CPPFLAGS=$_cppflags LDFLAGS=$_ldflags

AC_CONFIG_FILES([Makefile 
	libmng.pc:libmng.pc.in 
])
AC_OUTPUT
