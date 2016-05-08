CMAKE_MINIMUM_REQUIRED(VERSION 2.4.4)
PROJECT(FreeType C)

SET(INCLUDE_LIST
	include/ft2build.h
)

SET(FREETYPE_LIST
	include/freetype/ftotval.h
	include/freetype/ftoutln.h
	include/freetype/ftpfr.h
	include/freetype/ftrender.h
	include/freetype/ftsizes.h
	include/freetype/ftsnames.h
	include/freetype/ftstroke.h
	include/freetype/ftsynth.h
	include/freetype/ftsystem.h
	include/freetype/fttrigon.h
	include/freetype/fttypes.h
	include/freetype/ftwinfnt.h
	include/freetype/ftxf86.h
	include/freetype/t1tables.h
	include/freetype/ttnameid.h
	include/freetype/tttables.h
	include/freetype/tttags.h
	include/freetype/ttunpat.h
	include/freetype/freetype.h
	include/freetype/ftadvanc.h
	include/freetype/ftbbox.h
	include/freetype/ftbdf.h
	include/freetype/ftbitmap.h
	include/freetype/ftbzip2.h
	include/freetype/ftcache.h
	include/freetype/ftchapters.h
	include/freetype/ftcid.h
	include/freetype/fterrdef.h
	include/freetype/fterrors.h
	include/freetype/ftgasp.h
	include/freetype/ftglyph.h
	include/freetype/ftgxval.h
	include/freetype/ftgzip.h
	include/freetype/ftimage.h
	include/freetype/ftincrem.h
	include/freetype/ftlcdfil.h
	include/freetype/ftlist.h
	include/freetype/ftlzw.h
	include/freetype/ftmac.h
	include/freetype/ftmm.h
	include/freetype/ftmodapi.h
	include/freetype/ftmoderr.h
)

SET(CONFIG_LIST
	include/freetype/config/ftconfig.h
	include/freetype/config/ftheader.h
	include/freetype/config/ftmodule.h
	include/freetype/config/ftoption.h
	include/freetype/config/ftstdlib.h
)

SET(INTERNAL_LIST
	include/freetype/internal/ftdriver.h
	include/freetype/internal/ftgloadr.h
	include/freetype/internal/ftmemory.h
	include/freetype/internal/ftobjs.h
	include/freetype/internal/ftpic.h
	include/freetype/internal/ftrfork.h
	include/freetype/internal/ftserv.h
	include/freetype/internal/ftstream.h
	include/freetype/internal/fttrace.h
	include/freetype/internal/ftvalid.h
	include/freetype/internal/internal.h
	include/freetype/internal/psaux.h
	include/freetype/internal/pshints.h
	include/freetype/internal/sfnt.h
	include/freetype/internal/t1types.h
	include/freetype/internal/tttypes.h
	include/freetype/internal/autohint.h
	include/freetype/internal/ftcalc.h
	include/freetype/internal/ftdebug.h
)

SET(SERVICES_LIST
	include/freetype/internal/services/svotval.h
	include/freetype/internal/services/svpfr.h
	include/freetype/internal/services/svpostnm.h
	include/freetype/internal/services/svpscmap.h
	include/freetype/internal/services/svpsinfo.h
	include/freetype/internal/services/svsfnt.h
	include/freetype/internal/services/svttcmap.h
	include/freetype/internal/services/svtteng.h
	include/freetype/internal/services/svttglyf.h
	include/freetype/internal/services/svwinfnt.h
	include/freetype/internal/services/svxf86nm.h
	include/freetype/internal/services/svbdf.h
	include/freetype/internal/services/svcid.h
	include/freetype/internal/services/svgldict.h
	include/freetype/internal/services/svgxval.h
	include/freetype/internal/services/svkern.h
	include/freetype/internal/services/svmm.h
)

SET(SOURCE_LIST
	src/base/ftbase.c
	src/base/ftbitmap.c
	src/base/ftdebug.c
	src/base/ftfstype.c
	src/base/ftgasp.c
	src/base/ftglyph.c
	src/base/ftinit.c
	src/base/ftstroke.c
	src/base/ftsystem.c
	src/base/ftbbox.c
	src/base/ftmm.c
	src/base/ftpfr.c
	src/base/ftsynth.c
	src/base/fttype1.c
	src/base/ftwinfnt.c
	src/base/ftlcdfil.c
	src/base/ftgxval.c
	src/base/ftotval.c
	src/base/ftpatent.c
	src/autofit/autofit.c
	src/bdf/bdf.c
	src/cff/cff.c
	src/cache/ftcache.c
	src/gzip/ftgzip.c
	src/lzw/ftlzw.c
	src/smooth/smooth.c
	src/pcf/pcf.c
	src/pfr/pfr.c
	src/psaux/psaux.c
	src/pshinter/pshinter.c
	src/psnames/psmodule.c
	src/raster/raster.c
	src/sfnt/sfnt.c
	src/truetype/truetype.c
	src/type1/type1.c
	src/cid/type1cid.c
	src/type42/type42.c
	src/winfonts/winfnt.c
)

SOURCE_GROUP("include" FILES ${INCLUDE_LIST})
SOURCE_GROUP("include\\freetype" FILES ${FREETYPE_LIST})
SOURCE_GROUP("include\\freetype\\config" FILES ${CONFIG_LIST})
SOURCE_GROUP("include\\freetype\\internal" FILES ${INTERNAL_LIST})
SOURCE_GROUP("include\\freetype\\internal\\services" FILES  ${SERVICES_LIST})
SOURCE_GROUP("source" FILES ${SOURCE_LIST})

INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/freetype/include)

ADD_DEFINITIONS(-UDEBUG)
ADD_DEFINITIONS(-U_DEBUG)
ADD_DEFINITIONS(-U__DEBUG__)

ADD_DEFINITIONS(-DFT2_BUILD_LIBRARY)

ADD_LIBRARY(freetype STATIC ${INCLUDE_LIST} ${FREETYPE_LIST} ${CONFIG_LIST} ${INTERNAL_LIST} ${SERVICES_LIST} ${SOURCE_LIST})