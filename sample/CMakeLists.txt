CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)

FOREACH(mySamples
    "04.EnvLighting"
	"09.Globalillumination"
    "10.AtmosphericScattering"
)
ADD_SUBDIRECTORY(${mySamples})

SET_TARGET_ATTRIBUTE(${mySamples} "sample")

ENDFOREACH()