cmake_minimum_required(VERSION 3.12)

project(Ganxo)

# Add the capstone project
add_subdirectory(thirdparty/capstone)

#
# Ganxo
#
set(GANXO_PUBLIC_API
	include/ganxo.h
)
source_group("apis" FILES ${GANXO_PUBLIC_API})

# Core source
set(GANXO_SRC_CORE
    src/ds.c
	src/private.h
	src/memory.c
	src/hooks.c
	src/ganxo.c)
source_group("core" FILES ${GANXO_SRC_CORE})

# Disassembly source
set(GANXO_SRC_DISASM_DISABLED
	src/disasm-x86-impl.c)

set(GANXO_SRC_DISASM
	src/disasm.c
	src/disasm.h
	${GANXO_SRC_DISASM_DISABLED})
source_group("disasm" FILES ${GANXO_SRC_DISASM})

# Platform source
set(GANXO_PLATFORM_SRC_DISABLED
	src/win-papi-impl.c)

set(GANXO_PLATFORM_SRC
	${GANXO_PLATFORM_SRC_DISABLED})

source_group("platform/windows" FILES ${GANXO_PLATFORM_SRC})

add_library(ganxo_lib STATIC 
    ${GANXO_SRC_CORE}
	${GANXO_SRC_DISASM}
    ${GANXO_HEADERS}
	${GANXO_PLATFORM_SRC})

get_target_property(CAPSTONE_INCLUDE capstone-static INCLUDE_DIRECTORIES)
target_include_directories(ganxo_lib PUBLIC 
	${CAPSTONE_INCLUDE}
	include/)

target_link_libraries(ganxo_lib PUBLIC 
	capstone-static)

option(GANXO_ARCH_X86 "Ganxo x86 library" ON)
option(GANXO_ARCH_X86 "Ganxo x64 library" OFF)
if (GANXO_ARCH_X86)
	target_compile_definitions(ganxo_lib PUBLIC GANXO_ARCH_X86)
elseif (GANXO_ARCH_X64)
	target_compile_definitions(ganxo_lib PUBLIC GANXO_ARCH_X64)
endif()

option(GANXO_PLATFORM_WINDOWS "Ganxo library for MS Windows" ON)
if (GANXO_PLATFORM_WINDOWS)
	target_compile_definitions(ganxo_lib PUBLIC GANXO_PLATFORM_WINDOWS)
endif()

target_compile_definitions(ganxo_lib PRIVATE GANXO_LIBRARY)

set(GANXO_DISABLED_SOURCES
	${GANXO_SRC_DISASM_DISABLED}
	${GANXO_PLATFORM_SRC_DISABLED})

set_source_files_properties(${GANXO_DISABLED_SOURCES} PROPERTIES LANGUAGE "")

#
# Examples
#
add_subdirectory(examples/simple)

#
# Tests
#
add_subdirectory(tests/capstone)
add_subdirectory(tests/hooks)