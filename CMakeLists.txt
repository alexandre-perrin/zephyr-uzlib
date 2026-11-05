# SPDX-License-Identifier: Apache-2.0

if(CONFIG_UZLIB)
	set(UZLIB_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/uzlib")
	if(NOT EXISTS "${UZLIB_SOURCE_DIR}/src/uzlib.h")
		message(FATAL_ERROR
			"Could not locate the uzlib submodule at ${UZLIB_SOURCE_DIR}. "
			"Add this project with submodules enabled in the west manifest, then "
			"run 'west update'."
		)
	endif()

	zephyr_include_directories("${UZLIB_SOURCE_DIR}/src")

	zephyr_library()
	zephyr_library_compile_definitions(
		UZLIB_CONF_DEBUG_LOG=CONFIG_UZLIB_DEBUG_LOG
		UZLIB_CONF_PARANOID_CHECKS=CONFIG_UZLIB_PARANOID_CHECKS
		UZLIB_CONF_USE_MEMCPY=CONFIG_UZLIB_USE_MEMCPY
	)
	zephyr_library_sources("${UZLIB_SOURCE_DIR}/src/crc32.c")

	zephyr_library_sources_ifdef(CONFIG_UZLIB_DECOMPRESS
		"${UZLIB_SOURCE_DIR}/src/adler32.c"
		"${UZLIB_SOURCE_DIR}/src/tinflate.c"
		"${UZLIB_SOURCE_DIR}/src/tinfgzip.c"
		"${UZLIB_SOURCE_DIR}/src/tinfzlib.c"
	)

	zephyr_library_sources_ifdef(CONFIG_UZLIB_COMPRESS
		"${UZLIB_SOURCE_DIR}/src/genlz77.c"
		"${UZLIB_SOURCE_DIR}/src/defl_static.c"
	)
endif()
