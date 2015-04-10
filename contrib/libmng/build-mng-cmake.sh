#!/bin/sh
cmake -DWITH_LCMS2:bool=on -DBUILD_MAN:bool=on -DBUILD_SHARED_LIBS:bool=on -DCMAKE_INSTALL_PREFIX:path="/usr/local" ..
