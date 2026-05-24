# STM32 CMakeLists.txt - 排除模板文件版本
cmake_minimum_required(VERSION 3.10)
project(motor_test C ASM)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# 编译选项
set(CMAKE_C_FLAGS "-mcpu=cortex-m4 -mthumb")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g -Og")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_HAL_DRIVER")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTM32F407xx")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=fpv4-sp-d16")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=hard")

# 汇编选项
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mcpu=cortex-m4 -mthumb")

# ============ 查找源文件 ============
# Core源文件
file(GLOB C_SOURCES "Core/Src/*.c")

# HAL库源文件
file(GLOB HAL_SOURCES "Drivers/STM32F4xx_HAL_Driver/Src/*.c")

# 启动文件
file(GLOB ASM_SOURCES "Core/Startup/*.s")

# ============ 关键：排除模板文件 ============
message(STATUS "=== 过滤模板文件 ===")

# 过滤Core文件（排除freertos.c如果不需要）
set(FILTERED_C_SOURCES)
foreach(file ${C_SOURCES})
    if(NOT file MATCHES "template")
        list(APPEND FILTERED_C_SOURCES ${file})
    else()
        message(STATUS "排除Core模板文件: ${file}")
    endif()
endforeach()
set(C_SOURCES ${FILTERED_C_SOURCES})

# 过滤HAL文件 - 严格排除所有模板
set(FILTERED_HAL_SOURCES)
foreach(file ${HAL_SOURCES})
    # 排除所有模板文件
    if(NOT file MATCHES ".*template.*" AND
            NOT file MATCHES ".*_template.*" AND
            NOT file MATCHES ".*Template.*")
        list(APPEND FILTERED_HAL_SOURCES ${file})
    else()
        message(STATUS "排除HAL模板文件: ${file}")
    endif()
endforeach()
set(HAL_SOURCES ${FILTERED_HAL_SOURCES})

# ============ 尝试查找FreeRTOS（如果需要） ============
set(FREERTOS_SOURCES)
if(EXISTS "${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS")
    message(STATUS "找到FreeRTOS目录")

    # FreeRTOS核心文件
    file(GLOB FREERTOS_CORE
            "Middlewares/Third_Party/FreeRTOS/Source/*.c"
    )

    # 内存管理文件（使用heap_4.c）
    file(GLOB FREERTOS_HEAP
            "Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c"
    )

    # 查找端口文件
    file(GLOB FREERTOS_PORT_FILES
            "Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c"
            "Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c"
            "Middlewares/Third_Party/FreeRTOS/Source/portable/*/*/port.c"
    )

    # CMSIS-RTOS文件
    file(GLOB CMSIS_RTOS_FILES
            "Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/*.c"
    )

    # 合并FreeRTOS文件
    set(FREERTOS_SOURCES
            ${FREERTOS_CORE}
            ${FREERTOS_HEAP}
            ${FREERTOS_PORT_FILES}
            ${CMSIS_RTOS_FILES}
    )

    # 过滤FreeRTOS模板文件
    set(FILTERED_FREERTOS_SOURCES)
    foreach(file ${FREERTOS_SOURCES})
        if(NOT file MATCHES ".*template.*")
            list(APPEND FILTERED_FREERTOS_SOURCES ${file})
        endif()
    endforeach()
    set(FREERTOS_SOURCES ${FILTERED_FREERTOS_SOURCES})

    list(LENGTH FREERTOS_SOURCES FREERTOS_COUNT)
    message(STATUS "FreeRTOS文件数量: ${FREERTOS_COUNT}")
endif()

# ============ 合并所有源文件 ============
set(ALL_SOURCES ${C_SOURCES} ${HAL_SOURCES} ${FREERTOS_SOURCES})

# ============ 打印信息 ============
list(LENGTH C_SOURCES C_SOURCES_COUNT)
list(LENGTH HAL_SOURCES HAL_SOURCES_COUNT)
list(LENGTH ALL_SOURCES ALL_SOURCES_COUNT)
message(STATUS "Core文件数量: ${C_SOURCES_COUNT}")
message(STATUS "HAL文件数量: ${HAL_SOURCES_COUNT}")
message(STATUS "总文件数量: ${ALL_SOURCES_COUNT}")

# 创建可执行文件
add_executable(${PROJECT_NAME}.elf ${ALL_SOURCES} ${ASM_SOURCES})

# ============ 包含目录 ============
include_directories(
        Core/Inc
        Drivers/STM32F4xx_HAL_Driver/Inc
        Drivers/CMSIS/Device/ST/STM32F4xx/Include
        Drivers/CMSIS/Include
)

include_directories(
        Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS
)

# 添加FreeRTOS包含目录（如果存在）
if(FREERTOS_SOURCES)
    include_directories(
            Middlewares/Third_Party/FreeRTOS/Source/include
            Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
    )

    # 添加端口目录
    if(FREERTOS_PORT_FILES)
        list(GET FREERTOS_PORT_FILES 0 FIRST_PORT_FILE)
        get_filename_component(PORT_DIR ${FIRST_PORT_FILE} DIRECTORY)
        include_directories(${PORT_DIR})
        message(STATUS "FreeRTOS端口目录: ${PORT_DIR}")
    endif()
endif()

# ============ 链接选项 ============
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mcpu=cortex-m4 -mthumb")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mfpu=fpv4-sp-d16")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mfloat-abi=hard")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -specs=nano.specs")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -specs=nosys.specs")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_NAME}.map")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T${CMAKE_SOURCE_DIR}/STM32F407VGTx_FLASH.ld")

message(STATUS "CMake配置完成")