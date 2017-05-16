CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)

FOREACH(mySamples
    "01.HelloWorld"
    "02.UI"
    "03.Sound"
    "04.EnvLighting"
    "07.Skinning"
	"09.Globalillumination"
    "10.AtmosphericScattering"
)
ADD_SUBDIRECTORY(${mySamples})

SET_TARGET_ATTRIBUTE(${mySamples} "sample")

ENDFOREACH()