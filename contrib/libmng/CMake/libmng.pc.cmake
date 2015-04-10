prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=${prefix}/@MNG_INSTALL_LIB_DIR@
includedir=${prefix}/@MNG_INSTALL_INCLUDE_DIR@

Name: libmng
Description: Loads PNG/MNG/JNG files
Version: @MNG_VERSION@
Libs: -L${libdir} -lmng
Libs.private: @MNG_LIBS_PRIVATE@
Cflags: -I${includedir}
