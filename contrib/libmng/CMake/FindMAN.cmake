FIND_PROGRAM(MAN_EXE
             NAMES man
             PATHS /bin
                   /usr/bin
                   /usr/local/bin)
IF(MAN_EXE)
 SET(MAN_FOUND YES)
ELSE(MAN_EXE)
# MESSAGE(WARNING "Unable to find 'man' program.MAN pages not installed")
 SET(MAN_FOUND NO)
ENDIF(MAN_EXE)
