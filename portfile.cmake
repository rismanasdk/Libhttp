vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO rismanasdk/libhttp
    REF v2.0.0
    SHA512 0
    HEAD_REF main
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()
vcpkg_cmake_config_fixup()

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
