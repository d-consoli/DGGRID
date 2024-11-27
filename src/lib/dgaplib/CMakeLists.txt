cmake_minimum_required (VERSION 3.9)
set(JULIA_INCLUDE_DIR "/home/opengeohub/.julia/juliaup/julia-1.11.1+0.x64.linux.gnu/bin/../include/julia")
set(JULIA_ARTIFACTS_DIR "/home/opengeohub/.julia/artifacts/dee31c8c121f2be2dd7c7b2ec3d41b2be90cc8c7/include")
include_directories(${JULIA_INCLUDE_DIR} ${JULIA_ARTIFACTS_DIR} )
add_library(dgaplib
  lib/DgApParamList.cpp
  lib/DgApSubOperation.cpp
)

if(GDAL_FOUND)
  target_compile_definitions(dgaplib PRIVATE USE_GDAL)
endif()

# If GDAL isn't present than ${GDAL_INCLUDE_DIRS} and ${GDAL_LIBRARIES} give
# empty strings
target_include_directories(dgaplib PUBLIC include ../dglib/include ${GDAL_INCLUDE_DIRS})
target_link_libraries(dgaplib PRIVATE ${GDAL_LIBRARIES} shapelib proj4lib)
target_compile_features(dgaplib PRIVATE cxx_std_11)
