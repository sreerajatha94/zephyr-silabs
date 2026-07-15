# Copyright (c) 2025 Silicon Laboratories Inc.
# SPDX-License-Identifier: Apache-2.0

set(SL_OT_UPSTREAM_DIR ${ZEPHYR_BASE}/../modules/lib/openthread)
set(SL_OT_CORE_CONFIG_FILE openthread-core-zephyr-config.h)

if(CONFIG_SOC_SERIES MATCHES "^efr32mg")
  set(SL_OT_STACK_DEVICE_SERIES efr32mg${SILABS_DEVICE_FAMILY_NUMBER})
else()
  message(FATAL_ERROR
    "OPENTHREAD_SILABS_LIBRARY currently supports only EFR32MG device series, got CONFIG_SOC_SERIES=${CONFIG_SOC_SERIES}.")
endif()

if(CONFIG_OPENTHREAD_FTD)
  set(SL_OT_STACK_ROLE ftd)
  zephyr_compile_definitions(OPENTHREAD_FTD=1)
elseif(CONFIG_OPENTHREAD_MTD)
  set(SL_OT_STACK_ROLE mtd)
  zephyr_compile_definitions(OPENTHREAD_MTD=1)
else()
  message(FATAL_ERROR "OPENTHREAD_SILABS_LIBRARY requires CONFIG_OPENTHREAD_FTD or CONFIG_OPENTHREAD_MTD.")
endif()

set(SL_OT_STACK_PREBUILT
  openthread/libs/libsl_ot_stack_${SL_OT_STACK_ROLE}_${SL_OT_STACK_DEVICE_SERIES}_gcc.a)

if(NOT EXISTS "${BLOBS_DIR}/${SL_OT_STACK_PREBUILT}")
  message(FATAL_ERROR
    "OPENTHREAD_SILABS_LIBRARY requires the certified OpenThread stack library at:\n"
    "  ${BLOBS_DIR}/${SL_OT_STACK_PREBUILT}\n"
    "Install the hal_silabs module blobs or switch back to CONFIG_OPENTHREAD_SOURCES.")
endif()

set(SL_OT_CLI_SOURCES
  ${SL_OT_UPSTREAM_DIR}/src/cli/cli.cpp
  ${SL_OT_UPSTREAM_DIR}/src/cli/cli_ba.cpp
  ${SL_OT_UPSTREAM_DIR}/src/cli/cli_bbr.cpp
  ${SL_OT_UPSTREAM_DIR}/src/cli/cli_br.cpp
  ${SL_OT_UPSTREAM_DIR}/src/cli/cli_coap.cpp
  ${SL_OT_UPSTREAM_DIR}/src/cli/cli_coap_secure.cpp
  ${SL_OT_UPSTREAM_DIR}/src/cli/cli_commissioner.cpp
  ${SL_OT_UPSTREAM_DIR}/src/cli/cli_dataset.cpp
  ${SL_OT_UPSTREAM_DIR}/src/cli/cli_dns.cpp
  ${SL_OT_UPSTREAM_DIR}/src/cli/cli_history.cpp
  ${SL_OT_UPSTREAM_DIR}/src/cli/cli_joiner.cpp
  ${SL_OT_UPSTREAM_DIR}/src/cli/cli_link_metrics.cpp
  ${SL_OT_UPSTREAM_DIR}/src/cli/cli_logging.cpp
  ${SL_OT_UPSTREAM_DIR}/src/cli/cli_mac_filter.cpp
  ${SL_OT_UPSTREAM_DIR}/src/cli/cli_mdns.cpp
  ${SL_OT_UPSTREAM_DIR}/src/cli/cli_mesh_diag.cpp
  ${SL_OT_UPSTREAM_DIR}/src/cli/cli_network_data.cpp
  ${SL_OT_UPSTREAM_DIR}/src/cli/cli_ping.cpp
  ${SL_OT_UPSTREAM_DIR}/src/cli/cli_srp_client.cpp
  ${SL_OT_UPSTREAM_DIR}/src/cli/cli_srp_server.cpp
  ${SL_OT_UPSTREAM_DIR}/src/cli/cli_tcat.cpp
  ${SL_OT_UPSTREAM_DIR}/src/cli/cli_tcp.cpp
  ${SL_OT_UPSTREAM_DIR}/src/cli/cli_udp.cpp
  ${SL_OT_UPSTREAM_DIR}/src/cli/cli_utils.cpp
)

set(SL_OT_TCP_SUPPORT_SOURCES
  ${SL_OT_UPSTREAM_DIR}/third_party/tcplp/bsdtcp/cc/cc_newreno.c
  ${SL_OT_UPSTREAM_DIR}/third_party/tcplp/bsdtcp/tcp_fastopen.c
  ${SL_OT_UPSTREAM_DIR}/third_party/tcplp/bsdtcp/tcp_input.c
  ${SL_OT_UPSTREAM_DIR}/third_party/tcplp/bsdtcp/tcp_output.c
  ${SL_OT_UPSTREAM_DIR}/third_party/tcplp/bsdtcp/tcp_reass.c
  ${SL_OT_UPSTREAM_DIR}/third_party/tcplp/bsdtcp/tcp_sack.c
  ${SL_OT_UPSTREAM_DIR}/third_party/tcplp/bsdtcp/tcp_subr.c
  ${SL_OT_UPSTREAM_DIR}/third_party/tcplp/bsdtcp/tcp_timer.c
  ${SL_OT_UPSTREAM_DIR}/third_party/tcplp/bsdtcp/tcp_timewait.c
  ${SL_OT_UPSTREAM_DIR}/third_party/tcplp/bsdtcp/tcp_usrreq.c
  ${SL_OT_UPSTREAM_DIR}/third_party/tcplp/lib/bitmap.c
  ${SL_OT_UPSTREAM_DIR}/third_party/tcplp/lib/cbuf.c
  ${SL_OT_UPSTREAM_DIR}/third_party/tcplp/lib/lbuf.c
)

add_hal_silabs_prebuilt_library(libsl_ot_stack ${SL_OT_STACK_PREBUILT})

zephyr_compile_definitions(OPENTHREAD_PROJECT_CORE_CONFIG_FILE=\"${SL_OT_CORE_CONFIG_FILE}\")

zephyr_system_include_directories(
  ${SL_OT_UPSTREAM_DIR}/include
  ${SL_OT_UPSTREAM_DIR}/examples/platforms
)

zephyr_library_named(hal_silabs_openthread_shims)
zephyr_library_compile_definitions(
  OPENTHREAD_PROJECT_CORE_CONFIG_FILE=\"${SL_OT_CORE_CONFIG_FILE}\"
  OPENTHREAD_PLATFORM_POSIX=0
  OPENTHREAD_PLATFORM_NEXUS=0
  OPENTHREAD_RADIO=0
  PACKAGE_NAME=\"OPENTHREAD\"
  PACKAGE_VERSION=\"unknown\"
  MBEDTLS_SSL_EXPORT_KEYS
)
zephyr_library_compile_definitions_ifdef(CONFIG_OPENTHREAD_FTD
  OPENTHREAD_FTD=1
  OPENTHREAD_MTD=0
)
zephyr_library_compile_definitions_ifdef(CONFIG_OPENTHREAD_MTD
  OPENTHREAD_FTD=0
  OPENTHREAD_MTD=1
)
zephyr_library_include_directories(
  ${SL_OT_UPSTREAM_DIR}/include
  ${SL_OT_UPSTREAM_DIR}/src
  ${SL_OT_UPSTREAM_DIR}/src/core
  ${SL_OT_UPSTREAM_DIR}/src/include
  ${SL_OT_UPSTREAM_DIR}/third_party/tcplp/bsdtcp
  ${SL_OT_UPSTREAM_DIR}/third_party/tcplp/lib
  ${ZEPHYR_BASE}/modules/openthread/platform
)
zephyr_library_compile_options_ifdef(CONFIG_OPENTHREAD_TCP_ENABLE
  -Wno-sign-compare
  -Wno-unused-parameter
)
zephyr_library_sources(long_int_shim.cpp)
zephyr_library_sources_ifndef(CONFIG_OPENTHREAD_DIAG
  diag_stubs.c
)
zephyr_library_sources_ifdef(CONFIG_OPENTHREAD_SHELL ${SL_OT_CLI_SOURCES})
zephyr_library_sources_ifdef(CONFIG_OPENTHREAD_TCP_ENABLE ${SL_OT_TCP_SUPPORT_SOURCES})

string(REPLACE " " ";" SILABS_OT_MBEDTLS_LIBS " ${CONFIG_OPENTHREAD_MBEDTLS_LIB_NAME}")
zephyr_link_libraries(${SILABS_OT_MBEDTLS_LIBS})
