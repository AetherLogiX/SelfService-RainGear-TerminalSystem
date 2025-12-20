cmake_minimum_required(VERSION 3.16)
project(CampusRain VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MSVC特定设置
if(MSVC)
    add_compile_options(/Zc:__cplusplus /permissive-)
endif()

# 设置CMake路径（如果未设置）
if(NOT DEFINED CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "E:/Qt/6.9.3/mingw_64" CACHE PATH "Qt安装路径")
endif()

# 查找Qt6（如果找不到则尝试Qt5）
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Sql Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Sql Network)

# 设置Qt的MOC、UIC、RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Model
    ${CMAKE_CURRENT_SOURCE_DIR}/Control
    ${CMAKE_CURRENT_SOURCE_DIR}/client
)

# 源文件
set(SOURCES
    client/main.cpp
    client/MainWindow.cpp
    client/SlotItem.cpp
    Model/User.cpp
    Model/Stationlocal.cpp
    Model/StationUtils.cpp
    Control/DatabaseManager.cpp
    Control/ApiClient.cpp
)

# 头文件
set(HEADERS
    client/MainWindow.h
    client/SlotItem.h
    client/Styles.h
    Model/User.h
    Model/RainGear.hpp
    Model/RainGear_subclasses.hpp
    Model/RainGearFactory.h
    Model/GlobalEnum.hpp
    Model/Stationlocal.h
    Model/StationUtils.h
    Control/DBHelper.h
    Control/DatabaseManager.h
    Control/ApiClient.h
)

# 资源文件
set(RESOURCES
    rgear_icons/icons.qrc
    map_resources/map.qrc
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES})

# 链接Qt库
target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Network
)

# Windows特定配置
if(WIN32)
    # ============================================================
    # 使用项目内的 dll 文件夹（不依赖外部路径）
    # ============================================================
    set(PROJECT_DLL_DIR "${CMAKE_SOURCE_DIR}/dll")
    
    # MySQL库路径（用于链接）
    set(MYSQL_LIB_DIR "C:/Program Files/MySQL/MySQL Server 8.0/lib")
    
    # 链接MySQL库
    if(EXISTS "${MYSQL_LIB_DIR}")
        target_link_directories(${PROJECT_NAME} PRIVATE ${MYSQL_LIB_DIR})
        target_link_libraries(${PROJECT_NAME} libmysql)
    endif()
    
    # 从项目 dll 文件夹复制 MySQL 和 SSL DLL
    if(EXISTS "${PROJECT_DLL_DIR}")
        # 复制主要DLL（libmysql.dll, libcrypto, libssl）
        file(GLOB PROJECT_DLL_FILES "${PROJECT_DLL_DIR}/*.dll")
        foreach(DLL_FILE ${PROJECT_DLL_FILES})
            get_filename_component(DLL_NAME ${DLL_FILE} NAME)
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${DLL_FILE}"
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
                COMMENT "复制 ${DLL_NAME} 到输出目录"
            )
        endforeach()
        
        # 复制 sqldrivers 目录（qsqlmysql.dll）
        if(EXISTS "${PROJECT_DLL_DIR}/sqldrivers")
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/sqldrivers
                COMMENT "创建sqldrivers目录"
            )
            # 复制 qsqlmysql.dll
            if(EXISTS "${PROJECT_DLL_DIR}/sqldrivers/qsqlmysql.dll")
                add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${PROJECT_DLL_DIR}/sqldrivers/qsqlmysql.dll"
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>/sqldrivers
                    COMMENT "复制 qsqlmysql.dll 到 sqldrivers 目录"
                )
            endif()
        endif()
    else()
        message(WARNING "项目 dll 文件夹不存在: ${PROJECT_DLL_DIR}")
    endif()
    
    # ============================================================
    # 复制 MinGW 运行时库
    # ============================================================
    if(MINGW)
        if(CMAKE_CXX_COMPILER)
            get_filename_component(COMPILER_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
            set(MINGW_RUNTIME_LIBS
                "${COMPILER_DIR}/libgcc_s_seh-1.dll"
                "${COMPILER_DIR}/libstdc++-6.dll"
                "${COMPILER_DIR}/libwinpthread-1.dll"
            )
            foreach(RUNTIME_LIB ${MINGW_RUNTIME_LIBS})
                if(EXISTS "${RUNTIME_LIB}")
                    get_filename_component(LIB_NAME ${RUNTIME_LIB} NAME)
                    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${RUNTIME_LIB}"
                        $<TARGET_FILE_DIR:${PROJECT_NAME}>
                        COMMENT "复制MinGW运行时库 ${LIB_NAME}"
                    )
                endif()
            endforeach()
        endif()
    endif()
    
    # ============================================================
    # 复制 Qt 运行时 DLL
    # ============================================================
    get_target_property(QT_QMAKE_EXECUTABLE Qt${QT_VERSION_MAJOR}::qmake LOCATION)
    if(QT_QMAKE_EXECUTABLE)
        get_filename_component(QT_BIN_DIR ${QT_QMAKE_EXECUTABLE} DIRECTORY)
        get_filename_component(QT_PREFIX_DIR ${QT_BIN_DIR} DIRECTORY)
        
        # Qt 核心 DLL
        set(QT_DLLS
            "${QT_BIN_DIR}/Qt${QT_VERSION_MAJOR}Core.dll"
            "${QT_BIN_DIR}/Qt${QT_VERSION_MAJOR}Gui.dll"
            "${QT_BIN_DIR}/Qt${QT_VERSION_MAJOR}Widgets.dll"
            "${QT_BIN_DIR}/Qt${QT_VERSION_MAJOR}Sql.dll"
            "${QT_BIN_DIR}/Qt${QT_VERSION_MAJOR}Network.dll"
        )
        foreach(QT_DLL ${QT_DLLS})
            if(EXISTS "${QT_DLL}")
                get_filename_component(QT_DLL_NAME ${QT_DLL} NAME)
                add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${QT_DLL}"
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>
                    COMMENT "复制 ${QT_DLL_NAME}"
                )
            endif()
        endforeach()
        
        # Qt 平台插件 (platforms/qwindows.dll)
        set(QT_PLATFORM_PLUGIN "${QT_PREFIX_DIR}/plugins/platforms/qwindows.dll")
        if(EXISTS "${QT_PLATFORM_PLUGIN}")
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms
            )
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${QT_PLATFORM_PLUGIN}"
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms
                COMMENT "复制 Qt 平台插件"
            )
        endif()
    endif()
endif()

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Debug/Release配置
set_target_properties(${PROJECT_NAME} PROPERTIES
    DEBUG_POSTFIX "d"
)
