cmake_minimum_required(VERSION 2.8)

FOREACH(myTools
"normals_fitting"
)
# 添加参与编译的子目录
ADD_SUBDIRECTORY(${myTools})
SET_TARGET_ATTRIBUTE(${myTools} "tools")

ENDFOREACH()