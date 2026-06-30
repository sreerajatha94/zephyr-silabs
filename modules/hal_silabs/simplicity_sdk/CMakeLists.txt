# Copyright (c) 2025 Silicon Laboratories Inc.
# SPDX-License-Identifier: Apache-2.0

set(SECURITY_DIR ${ZEPHYR_HAL_SILABS_MODULE_DIR}/simplicity_sdk/platform/security)
set(LIBCRYPTOSOC_DIR ${ZEPHYR_HAL_SILABS_EXTRA_MODULE_DIR}/simplicity_sdk/util/third_party/crypto_ip/libcryptosoc)

# Get SoC series number, i.e. translate efr32bg22 -> 22, mgm240p -> 24, simg301 -> 301
if(CONFIG_SOC_SERIES MATCHES "^ef[rm]32")
  string(SUBSTRING ${CONFIG_SOC_SERIES} 7 2 SILABS_DEVICE_FAMILY_NUMBER)
elseif(CONFIG_SOC_SERIES MATCHES "^si")
  string(SUBSTRING ${CONFIG_SOC_SERIES} 4 3 SILABS_DEVICE_FAMILY_NUMBER)
elseif(CONFIG_SOC_SERIES MATCHES "^.gm")
  string(SUBSTRING ${CONFIG_SOC_SERIES} 3 2 SILABS_DEVICE_FAMILY_NUMBER)
else()
  message(FATAL_ERROR "unknown part")
endif()

zephyr_include_directories(
  inc
)

zephyr_compile_definitions_ifdef(CONFIG_SILABS_SISDK_SE
  SL_SE_MANAGER_THREADING
)

set(vse_device_families 22 27)
if(CONFIG_SOC_FAMILY_SILABS_S2 AND NOT (SILABS_DEVICE_FAMILY_NUMBER IN_LIST vse_device_families))
  zephyr_compile_definitions_ifdef(CONFIG_SILABS_SISDK_SE
    SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION
  )
endif()

# PSA Crypto
if(CONFIG_PSA_CRYPTO_DRIVER_SILABS_HSE OR CONFIG_PSA_CRYPTO_DRIVER_SILABS_VSE OR CONFIG_PSA_CRYPTO_DRIVER_SILABS_SIWX91X)

zephyr_library_named(hal_silabs_crypto)

zephyr_include_directories(
  ${SECURITY_DIR}/sl_component/sl_psa_driver/inc
  ${SECURITY_DIR}/sl_component/sl_mbedtls_support/config
  ${SECURITY_DIR}/sl_component/sl_mbedtls_support/inc
)
zephyr_library_sources(
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_psa_driver_common.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_psa_driver_ghash.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_psa_driver_init.c
#  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_psa_trng.c
)
zephyr_library_link_libraries(mbedTLS)
zephyr_library_link_libraries(tfpsacrypto)
# Crypto drivers need access to internal TF-PSA-Crypto headers
zephyr_library_include_directories($<TARGET_PROPERTY:tfpsacrypto,INCLUDE_DIRECTORIES>)

# HSE
zephyr_library_sources_ifdef(CONFIG_PSA_CRYPTO_DRIVER_SILABS_HSE
  ${SECURITY_DIR}/sl_component/se_manager/src/sl_se_manager_cipher.c
  ${SECURITY_DIR}/sl_component/se_manager/src/sl_se_manager_hash.c
  ${SECURITY_DIR}/sl_component/se_manager/src/sl_se_manager_key_derivation.c
  ${SECURITY_DIR}/sl_component/se_manager/src/sl_se_manager_key_handling.c
  ${SECURITY_DIR}/sl_component/se_manager/src/sl_se_manager_signature.c
  ${SECURITY_DIR}/sl_component/se_manager/src/sl_se_manager_attestation.c

  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_se_driver_aead.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_se_driver_builtin_keys.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_se_driver_cipher.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_se_driver_key_derivation.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_se_driver_key_management.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_se_driver_mac.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_se_driver_signature.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_se_opaque_driver_aead.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_se_opaque_driver_cipher.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_se_opaque_driver_mac.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_se_opaque_key_derivation.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_se_transparent_driver_aead.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_se_transparent_driver_cipher.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_se_transparent_driver_hash.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_se_transparent_driver_mac.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_se_transparent_key_derivation.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_se_version_dependencies.c
)

# VSE
zephyr_include_directories_ifdef(CONFIG_PSA_CRYPTO_DRIVER_SILABS_VSE
  ${LIBCRYPTOSOC_DIR}/include
  ${LIBCRYPTOSOC_DIR}/src
  ${LIBCRYPTOSOC_DIR}/src/platform/silicon_labs
)
zephyr_library_sources_ifdef(CONFIG_PSA_CRYPTO_DRIVER_SILABS_VSE
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/cryptoacc_management.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_cryptoacc_driver_key_derivation.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_cryptoacc_driver_trng.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_cryptoacc_opaque_driver_builtin_keys.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_cryptoacc_opaque_driver_mac.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_cryptoacc_transparent_driver_aead.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_cryptoacc_transparent_driver_cipher.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_cryptoacc_transparent_driver_hash.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_cryptoacc_transparent_driver_key_derivation.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_cryptoacc_transparent_driver_key_management.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_cryptoacc_transparent_driver_mac.c
  ${SECURITY_DIR}/sl_component/sl_psa_driver/src/sli_cryptoacc_transparent_driver_signature.c

  ${LIBCRYPTOSOC_DIR}/src/ba414ep_config.c
  ${LIBCRYPTOSOC_DIR}/src/ba431_config.c
  ${LIBCRYPTOSOC_DIR}/src/cryptodma_internal.c
  ${LIBCRYPTOSOC_DIR}/src/cryptolib_types.c
  ${LIBCRYPTOSOC_DIR}/src/sx_aes.c
  ${LIBCRYPTOSOC_DIR}/src/sx_blk_cipher.c
  ${LIBCRYPTOSOC_DIR}/src/sx_blk_cipher.h
  ${LIBCRYPTOSOC_DIR}/src/sx_dh_alg.c
  ${LIBCRYPTOSOC_DIR}/src/sx_ecc_curves.c
  ${LIBCRYPTOSOC_DIR}/src/sx_ecc_keygen_alg.c
  ${LIBCRYPTOSOC_DIR}/src/sx_ecdsa_alg.c
  ${LIBCRYPTOSOC_DIR}/src/sx_hash.c
  ${LIBCRYPTOSOC_DIR}/src/sx_math.c
  ${LIBCRYPTOSOC_DIR}/src/sx_memcmp.c
  ${LIBCRYPTOSOC_DIR}/src/sx_memcpy.c
  ${LIBCRYPTOSOC_DIR}/src/sx_primitives.c
  ${LIBCRYPTOSOC_DIR}/src/sx_rng.c
  ${LIBCRYPTOSOC_DIR}/src/sx_trng.c
  ${LIBCRYPTOSOC_DIR}/src/platform/silicon_labs/sli_libcryptosoc.c
)

endif() # PSA Crypto

# Zigbee stack (downstream-only prebuilt library integration)
set(BLOBS_DIR ${ZEPHYR_HAL_SILABS_MODULE_DIR}/zephyr/blobs/simplicity_sdk)
set(ZIGBEE_DIR ${ZEPHYR_HAL_SILABS_MODULE_DIR}/simplicity_sdk/protocol/zigbee)

function(add_zigbee_prebuilt_library lib_name component)
  if(NOT CONFIG_BUILD_ONLY_NO_BLOBS)
    add_library(${lib_name} STATIC IMPORTED GLOBAL)
    set_target_properties(${lib_name} PROPERTIES
      IMPORTED_LOCATION ${BLOBS_DIR}/zigbee/libs/build/gcc/cortex-m33/${component}/release/lib${component}.a
    )
    zephyr_link_libraries(${lib_name})
  endif()
endfunction()

if(CONFIG_ZIGBEE_SILABS)
  if(CONFIG_ZIGBEE_SILABS_STACK_LEAF)
    set(ZIGBEE_STACK_COMPONENT zigbee-pro-leaf-stack)
  elseif(CONFIG_ZIGBEE_SILABS_STACK_ROUTER)
    if(CONFIG_ZIGBEE_SILABS_RAIL_MUX)
      set(ZIGBEE_STACK_COMPONENT zigbee-pro-router-stack-rail-mux)
    else()
      set(ZIGBEE_STACK_COMPONENT zigbee-pro-router-stack)
    endif()
  else()
    if(CONFIG_ZIGBEE_SILABS_RAIL_MUX)
      set(ZIGBEE_STACK_COMPONENT zigbee-pro-stack-rail-mux)
    else()
      set(ZIGBEE_STACK_COMPONENT zigbee-pro-stack)
    endif()
  endif()

  add_zigbee_prebuilt_library(libzigbee_stack ${ZIGBEE_STACK_COMPONENT})

  if(CONFIG_ZIGBEE_SILABS_R23_SUPPORT)
    add_zigbee_prebuilt_library(libzigbee_r23 zigbee-r23-support)
  endif()

  if(CONFIG_ZIGBEE_SILABS_CBKE)
    add_zigbee_prebuilt_library(libzigbee_cbke_core zigbee-cbke-core)
    add_zigbee_prebuilt_library(libzigbee_cbke_dsa_sign zigbee-cbke-dsa-sign)
    add_zigbee_prebuilt_library(libzigbee_cbke_dsa_verify zigbee-cbke-dsa-verify)
    add_zigbee_prebuilt_library(libzigbee_cbke_163k1 zigbee-cbke-163k1)
    add_zigbee_prebuilt_library(libzigbee_cbke_283k1 zigbee-cbke-283k1)
    add_zigbee_prebuilt_library(libzigbee_cbke_dsa_verify_283k1 zigbee-cbke-dsa-verify-283k1)
  endif()

  if(CONFIG_ZIGBEE_SILABS_DEBUG_BASIC)
    add_zigbee_prebuilt_library(libzigbee_debug_basic zigbee-debug-basic)
  endif()

  if(CONFIG_ZIGBEE_SILABS_DEBUG_EXTENDED)
    add_zigbee_prebuilt_library(libzigbee_debug_extended zigbee-debug-extended)
  endif()

  if(CONFIG_ZIGBEE_SILABS_SOURCE_ROUTE)
    add_zigbee_prebuilt_library(libzigbee_source_route zigbee-source-route)
  endif()

  if(CONFIG_ZIGBEE_SILABS_GP)
    add_zigbee_prebuilt_library(libzigbee_gp zigbee-gp)
  endif()

  if(CONFIG_ZIGBEE_SILABS_ZLL)
    add_zigbee_prebuilt_library(libzigbee_zll zigbee-zll)
  endif()

  if(CONFIG_ZIGBEE_SILABS_DYNAMIC_COMMISSIONING)
    add_zigbee_prebuilt_library(libzigbee_dynamic_commissioning zigbee-dynamic-commissioning)
  endif()

  zephyr_compile_definitions(
    SL_ZIGBEE_CONFIG_HEADER="sl_zigbee_config.h"
  )
  zephyr_include_directories(config/zigbee)

  if(EXISTS ${ZIGBEE_DIR})
    zephyr_include_directories(
      ${ZIGBEE_DIR}/app/framework/common
      ${ZIGBEE_DIR}/app/framework/include
      ${ZIGBEE_DIR}/app/framework/security
      ${ZIGBEE_DIR}/app/framework/util
      ${ZIGBEE_DIR}/platform/base/include
      ${ZIGBEE_DIR}/platform/radio
      ${ZIGBEE_DIR}/stack/include
    )
  endif()
endif()
