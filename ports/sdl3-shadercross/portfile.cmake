vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO libsdl-org/SDL_shadercross
    REF "7b7365a"
    HEAD_REF main
    SHA512 "52efd2c2507d6ae874cdc177945e15494920f11148e9e9cf8da27fb5ccacb5fcbe44581005e132a84631e9d438616aa1247b7ae23f4ef1785203cdcb08af19af"
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DSDLSHADERCROSS_VENDORED=OFF 
        -DSDLSHADERCROSS_DXC=ON
        -DSDLSHADERCROSS_SHARED=ON
        -DSDLSHADERCROSS_SPIRVCROSS_SHARED=OFF
        -DSDLSHADERCROSS_CLI=OFF
        -DSDLSHADERCROSS_TESTS=OFF
        -DSDLSHADERCROSS_INSTALL=ON
        -DSDLSHADERCROSS_INSTALL_CMAKEDIR_ROOT=lib/cmake/SDL3_shadercross
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(PACKAGE_NAME SDL3_shadercross CONFIG_PATH lib/cmake/SDL3_shadercross)
vcpkg_copy_pdbs()

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE.txt")
