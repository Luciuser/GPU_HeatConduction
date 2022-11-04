cmake_minimum_required(VERSION 3.3)
################################################################################
set(CMAKE_CXX_STANDARD 14)

IF(MSVC)  # win32平台
    add_definitions(-D_ENABLE_EXTENDED_ALIGNED_STORAGE)
endif()
 
# packages
find_package(CUDA)
# nvcc flags
#set(CUDA_NVCC_FLAGS -gencode arch=compute_20,code=sm_20;-G;-g)
#set(CUDA_NVCC_FLAGS -gencode arch=compute_52,code=sm_52;-G;-g)
if(NOT DEFINED ${CMAKE_CUDA_ARCHITECTURES})
    set(CMAKE_CUDA_ARCHITECTURES 61 75)
endif()
message(STATUS "CUDA architectures set to ${CMAKE_CUDA_ARCHITECTURES}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common_teacher)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../extern/book/common)
message(${CUDA_TOOLKIT_ROOT_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../extern/image)
message(${CUDA_TOOLKIT_ROOT_DIR})
include_directories(${CUDA_TOOLKIT_ROOT_DIR}/include)
include_directories(${CUDA_TOOLKIT_ROOT_DIR}/include/cuda-samples-master/Common) 

link_directories(${CUDA_TOOLKIT_ROOT_DIR}/lib/x64)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib)

# 添加源文件 
set(main_SRC main.cpp)

# 通用文件   
file(GLOB_RECURSE Common_SRC "common/*.h" "common/*.cpp")

# 老师的通用文件
file(GLOB_RECURSE CommonTeacher_SRC "common_teacher/*.h" "common_teacher/*.hpp" "common_teacher/GL/*.h")

# 大作业，热源扩散
file(GLOB_RECURSE HeatSouceCPU "10_heatSourceCPU/*.h" "10_heatSourceCPU/*.hpp" "10_heatSourceCPU/*.cpp")	
file(GLOB_RECURSE HeatSouceGPU "10_heatSourceGPU/*.h" "10_heatSourceGPU/*.cuh" "10_heatSourceGPU/*.cu")	
file(GLOB_RECURSE HeatSouceGPUTest "10_heatSourceGPU_Test/*.cpp")	

#源文件分组 
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${main_SRC}) 
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${Common_SRC})
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${CommonTeacher_SRC})
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${HeatSouceCPU})
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${HeatSouceGPU})
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${HeatSouceGPUTest})

CUDA_ADD_EXECUTABLE(HeatSourceExe ${Common_SRC} ${CommonTeacher_SRC} ${HeatSouceCPU} ${HeatSouceGPU} ${HeatSouceGPUTest})
target_link_libraries (HeatSourceExe cublas) 
target_link_libraries (HeatSourceExe glut64)
