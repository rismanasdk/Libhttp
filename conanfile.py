from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import copy
import os

class LibHttpConan(ConanFile):
    name = "libhttp"
    version = "2.0.0"
    package_type = "header-library"

    license = "MIT"
    url = "https://github.com/rismanasdk/libhttp"
    homepage = "https://github.com/rismanasdk/libhttp"
    description = "Header-only C++ HTTP/1.1 client library with OpenSSL and zlib support"
    topics = ("http", "https", "header-only", "networking", "client")

    settings = "os", "arch", "compiler", "build_type"
    exports_sources = (
        "CMakeLists.txt",
        "libhttp-config.cmake.in",
        "LICENSE",
        "*.h",
    )
    no_copy_source = True

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        self.requires("openssl/3.3.2")
        self.requires("zlib/1.3.1")

    def package_id(self):
        self.info.clear()

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        toolchain = CMakeToolchain(self)
        toolchain.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()

    def package(self):
        copy(self, "LICENSE", src=self.source_folder, dst=os.path.join(self.package_folder, "licenses"))
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "libhttp")
        self.cpp_info.set_property("cmake_target_name", "libhttp::libhttp")
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []
