import os
from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class graphRecipe(ConanFile):
    name = "bitonic_sort"
    version = "1.0"
    user = "baitim"

    license = ""
    author = ""
    url = ""
    description = "bitonic_sort"
    topics = ""

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    requires = "opencl-icd-loader/2023.12.14"
    test_requires = "gtest/1.15.0"

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "tests/*", "compare/*"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        os.environ["CONAN_PACKAGE"] = "1"
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.set_property("cmake_find_mode", "config")
        self.cpp_info.set_property("cmake_file_name", "BitonicSort")
        self.cpp_info.set_property("cmake_target_name", "BitonicSort::BitonicSort")
        self.cpp_info.libs = ["BitonicSort"]
        self.cpp_info.includedirs = ["include"]