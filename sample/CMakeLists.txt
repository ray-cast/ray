CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)

FOREACH(mySamples
    "01.HelloWorld"
    "02.UI"
    "03.Sound"
    "04.EnvLighting"
    "05.SSSSS"
    "06.CubeWorld"
    "07.Skinning"
    "08.OIT"
    "09.SSR"
	"10.Globalillumination"
)
ADD_SUBDIRECTORY(${mySamples})

SET_TARGET_ATTRIBUTE(${mySamples} "sample")

ENDFOREACH()