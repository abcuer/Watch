cmake_minimum_required(VERSION 3.22)

# 设置项目名称
set(CMAKE_PROJECT_NAME Watch_f411)

# 设置 C 标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# 设置 size 工具
set(SIZE arm-none-eabi-size)

# 定义构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# 导出编译命令（便于 clangd 等工具索引）
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# 彩色诊断
set(CMAKE_COLOR_DIAGNOSTICS ON)
# 包含工具链文件（必须在 project() 前设置编译器）
include("cmake/gcc-arm-none-eabi.cmake")
# include("cmake/gcc-arm-none-eabi.cmake")
# 核心项目设置
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# 启用 C / C++ / ASM 支持
enable_language(C CXX ASM)

# 去掉警告屏蔽（调试时可删除 -w）
add_compile_options(-w)

# 创建可执行文件
add_executable(${CMAKE_PROJECT_NAME})

# 链接 STM32CubeMX 生成的库
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    m
)

# 添加 STM32CubeMX 生成的源码
add_subdirectory(cmake/stm32cubemx)

# ---------------- 自定义函数：批量添加源码目录 -----------------
function(USER_ADD_DIR dir)
    file(GLOB USER_SOURCE
        ${dir}/*.h
        ${dir}/*.c
        ${dir}/*.hpp
        ${dir}/*.cpp
        ${dir}/*.cc
    )
    target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${USER_SOURCE})
    target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${dir})
endfunction()

# ---------------- 添加用户代码目录 -----------------
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/test)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/draw)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/custom)

USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/generated)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/generated/guider_customer_fonts)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/generated/guider_fonts)   
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/generated/images)

USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/porting)

USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/core)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/draw)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/draw/arm2d)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/draw/nxp)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/draw/nxp/pxp)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/draw/nxp/vglite)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/draw/renesas)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/draw/sdl)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/draw/stm32_dma2d)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/draw/sw)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/draw/swm341_dma2d)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/layouts)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/layouts/flex)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/layouts/grid)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/libs)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/libs/barcode)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/libs/bmp)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/libs/ffmpeg)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/libs/freetype)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/libs/fsdrv)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/libs/gif)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/libs/png)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/libs/qrcode)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/libs/rlottie)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/libs/sjpg)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/others)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/others/fragment)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/others/gridnav)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/others/ime)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/others/imgfont)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/others/monkey)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/others/msg)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/others/snapshot)

USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/themes)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/themes/basic)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/themes/default)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/themes/mono)

USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/extra/widgets)

USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/font)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/hal)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/misc)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/app/lvgl/src/widgets)

USER_ADD_DIR(${CMAKE_SOURCE_DIR}/bsp)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/bsp/iic)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/bsp/delay)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/bsp/timer)

USER_ADD_DIR(${CMAKE_SOURCE_DIR}/modules/lcd)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/modules/touch)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/modules/sht31)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/modules/bmp280)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/modules/max30102)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/modules/bh1750)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/modules/mpu6050)
# ---------------- 链接选项（newlib-nano + printf浮点支持） -----------------
target_link_options(${CMAKE_PROJECT_NAME} PUBLIC
    -lc_nano
    -u _printf_float
)

# ---------------- 编译完成后生成 hex / bin -----------------
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMENT "Building hex & bin files...\nExecutable size:"
    COMMAND ${CMAKE_OBJCOPY} -Oihex  $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin
    COMMAND ${SIZE} $<TARGET_FILE:${PROJECT_NAME}>
)
