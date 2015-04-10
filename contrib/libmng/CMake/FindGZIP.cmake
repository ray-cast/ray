FIND_PROGRAM(GZIP_EXE
             NAMES gzip
             PATHS /bin
                   /usr/bin
                   /usr/local/bin)
IF(GZIP_EXE)
 SET(GZIP_FOUND YES)
ELSE(GZIP_EXE)
 MESSAGE(WARNING "Unable to find 'gzip' program")
 SET(GZIP_FOUND NO)
ENDIF(GZIP_EXE)

