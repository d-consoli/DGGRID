cmake_minimum_required (VERSION 3.9)

add_executable(dggrid
   DgHexSF.cpp
   OpBasic.cpp
   SubOpBasic.cpp
   SubOpBasicMulti.cpp
   SubOpBinPts.cpp
   SubOpDGG.cpp
   SubOpGen.cpp
   SubOpGenHelper.cpp
   SubOpIn.cpp
   SubOpMain.cpp
   SubOpOut.cpp
   SubOpStats.cpp
   SubOpTransform.cpp
   clipper.cpp
   dggrid.cpp
)


set(JULIA_INCLUDE_DIR "/home/opengeohub/.julia/juliaup/julia-1.11.1+0.x64.linux.gnu/bin/../include/julia")
set(JULIA_ARTIFACTS_DIR "/home/opengeohub/.julia/artifacts/dee31c8c121f2be2dd7c7b2ec3d41b2be90cc8c7/include")
include_directories(${JULIA_INCLUDE_DIR} ${JULIA_ARTIFACTS_DIR} )
find_library(JULIA_LIBRARIES NAMES julia HINTS "/home/opengeohub/.julia/juliaup/julia-1.11.1+0.x64.linux.gnu/lib")
add_library(jldggrid SHARED 
   DgHexSF.cpp
   OpBasic.cpp
   SubOpBasic.cpp
   SubOpBasicMulti.cpp
   SubOpBinPts.cpp
   SubOpDGG.cpp
   SubOpGen.cpp
   SubOpGenHelper.cpp
   SubOpIn.cpp
   SubOpMain.cpp
   SubOpOut.cpp
   SubOpStats.cpp
   SubOpTransform.cpp
   clipper.cpp
   jldggrid.cpp
)


if(GDAL_FOUND)
  target_compile_definitions(dggrid PRIVATE USE_GDAL)
endif()

# If GDAL isn't present than ${GDAL_LIBRARIES} gives an empty string
target_link_libraries(dggrid PRIVATE dglib dgaplib shapelib ${GDAL_LIBRARIES})
target_link_libraries(jldggrid PRIVATE dglib dgaplib shapelib ${JULIA_LIBRARIES})
target_compile_features(dggrid PRIVATE cxx_std_11)
