cmake_minimum_required(VERSION 3.5)

project(NAS_SIM_TOOLS)

include(FindPkgConfig)
pkg_search_module(CONFIG libconfig REQUIRED)
include_directories(${CONFIG_INCLUDE_DIRS})
add_definitions(-std=gnu99)
include("../macros.cmake")

ENABLE_LANGUAGE(C)

set(CMAKE_C_FLAGS
  "${CMAKE_C_FLAGS} ${C_FLAGS_PROCESSOR} -Werror -Wall -Wstrict-prototypes -g")

set(OPENAIR_DIR     $ENV{OPENAIR_DIR})
set(OPENAIR3_DIR    $ENV{OPENAIR_DIR}/openair3)
include_directories (${OPENAIR_DIR}/openair2/COMMON)
include_directories (${OPENAIR_DIR})

set(CONF2UEDATA_LIB_SRC
    ${OPENAIR_DIR}/openair3/NAS/TOOLS/conf_emm.c
    ${OPENAIR_DIR}/openair3/NAS/TOOLS/conf_user_data.c
    ${OPENAIR_DIR}/openair3/NAS/TOOLS/conf_usim.c
    ${OPENAIR_DIR}/openair3/NAS/TOOLS/conf_network.c
    ${OPENAIR_DIR}/openair3/NAS/TOOLS/conf_user_plmn.c
    ${OPENAIR_DIR}/openair3/NAS/TOOLS/conf_parser.c
    ${OPENAIR_DIR}/openair3/NAS/TOOLS/fs.c
    ${OPENAIR_DIR}/openair3/NAS/TOOLS/display.c
    ${OPENAIR_DIR}/openair3/NAS/UE/API/USIM/usim_api.c
    ${OPENAIR_DIR}/openair3/NAS/UE/API/USIM/aka_functions.c
    ${OPENAIR_DIR}/common/utils/mem/memory.c
    ${OPENAIR_DIR}/openair3/NAS/COMMON/UTIL/OctetString.c
    ${OPENAIR_DIR}/common/utils/utils.c
)

include_directories(
    ${OPENAIR_DIR}/common/utils
    ${OPENAIR_DIR}/openair3/NAS/UE
    ${OPENAIR_DIR}/openair3/NAS/COMMON
    ${OPENAIR_DIR}/openair3/NAS/UE/API/USER
    ${OPENAIR_DIR}/openair3/NAS/UE/API/USIM
    ${OPENAIR_DIR}/openair3/NAS/UE/EMM/
    ${OPENAIR_DIR}/openair3/NAS/UE/ESM/
    ${OPENAIR_DIR}/openair3/NAS/COMMON/IES/
    ${OPENAIR_DIR}/openair3/NAS/COMMON/UTIL
)

# conf2uedata binary

set(conf2uedata_SRC
    ${OPENAIR_DIR}/openair3/NAS/TOOLS/conf2uedata.c
	${CONF2UEDATA_LIB_SRC}
)

add_executable(conf2uedata  ${conf2uedata_SRC} )
target_link_libraries(conf2uedata ${CONFIG_LIBRARIES})

# usim binary

set(usim_SRC
    ${OPENAIR_DIR}/openair3/NAS/TOOLS/usim.c
	${CONF2UEDATA_LIB_SRC}
)


add_executable(usim  ${usim_SRC} )
target_link_libraries(usim ${CONFIG_LIBRARIES})

# nvram binary

set(nvram_SRC
    ${OPENAIR_DIR}/openair3/NAS/TOOLS/nvram.c
	${CONF2UEDATA_LIB_SRC}
)


add_executable(nvram  ${nvram_SRC})
target_link_libraries(nvram ${CONFIG_LIBRARIES})
