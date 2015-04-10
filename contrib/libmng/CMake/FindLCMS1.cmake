# - Find LCMS version 1 library
#
#  LCMS_INCLUDE_DIR
#  LCMS_LIBRARIES
#  LCMS_FOUND
#
# also defined, but not for general use are
#  LCMS_LIBRARY

FIND_PATH(LCMS1_INCLUDE_DIR lcms.h  PATHS /usr/include /usr/local/include /opt/include /opt/local/include)

SET(LCMS1_NAMES ${LCMS_NAMES} lcms liblcms liblcms_static)

FIND_LIBRARY(LCMS1_LIBRARY NAMES ${LCMS1_NAMES} )

MARK_AS_ADVANCED(LCMS1_INCLUDE_DIR LCMS1_LIBRARY)

# handle the QUIETLY and REQUIRED arguments and set LCMS_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LCMS1  DEFAULT_MSG  LCMS1_LIBRARY  LCMS1_INCLUDE_DIR)

IF(LCMS1_FOUND)
  SET(LCMS1_INCLUDE_DIRS ${LCMS1_INCLUDE_DIR})
  SET(LCMS1_LIBRARIES ${LCMS1_LIBRARY} )
ENDIF(LCMS1_FOUND)
