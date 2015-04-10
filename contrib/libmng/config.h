/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* define if you want JPEG support */
#define HAVE_LIBJPEG 1

/* define if you want lcms v1 support */
/* #undef HAVE_LIBLCMS1 */

/* define if you want lcms v2 support */
#define HAVE_LIBLCMS2 1

/* Define to 1 if you have the `z' library (-lz). */
#define HAVE_LIBZ 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* define if you want chunk access support */
#define MNG_ACCESS_CHUNKS 1

/* enable building standard shared object */
#define MNG_BUILD_SO 1

/* enable verbose error text */
#define MNG_ERROR_TELLTALE 1

/* define if you want full lcms support */
#define MNG_FULL_CMS 1

/* enable support for accessing chunks */
#define MNG_STORE_CHUNKS 1

/* define if you want display support */
#define MNG_SUPPORT_DISPLAY 1

/* define if you want dynamic support */
#define MNG_SUPPORT_DYNAMICMNG 1

/* define if you want full mng support */
#define MNG_SUPPORT_FULL 1

/* define if you want read support */
#define MNG_SUPPORT_READ 1

/* enable support for debug tracing */
/* #undef MNG_SUPPORT_TRACE */

/* define if you want write support */
#define MNG_SUPPORT_WRITE 1

/* enable support for debug messages */
/* #undef MNG_TRACE_TELLTALE */

/* but: libmng.dll (!) */
#define MNG_VERSION_DLL 2

/* MAJOR number of version */
#define MNG_VERSION_MAJOR 2

/* MINOR number of version */
#define MNG_VERSION_MINOR 0

/* PATCH number of version */
#define MNG_VERSION_RELEASE 2

/* eg. libmng.so.1 */
#define MNG_VERSION_SO 2

/* Name of package */
#define PACKAGE "libmng-2.0.2"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "png-mng-implement@lists.sourceforge.net"

/* Define to the full name of this package. */
#define PACKAGE_NAME "libmng"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "libmng 2.0.2"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "libmng-2.0.2"

/* Define to the home page for this package. */
#define PACKAGE_URL "http://www.libmng.com/"

/* Define to the version of this package. */
#define PACKAGE_VERSION "2.0.2"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "2.0.2"

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Enable large inode numbers on Mac OS X 10.5.  */
#ifndef _DARWIN_USE_64_BIT_INODE
# define _DARWIN_USE_64_BIT_INODE 1
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */
