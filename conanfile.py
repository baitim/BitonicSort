import os
from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import replace_in_file

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
    tool_requires = "cmake/[>=3.29]"
    requires = "opencl-icd-loader/2023.12.14"
    test_requires = "gtest/1.15.0"

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "tests/*", "compare/*"

    def configure(self):
        if self.settings.compiler == "msvc":
            self.settings.compiler.cppstd = "20"  # MSVC only supports "20", not "gnu20"
        elif "gnu" in str(self.settings.compiler):
            self.settings.compiler.cppstd = "gnu20"  # GCC uses "gnuXX" for GNU extensions
        else:
            self.settings.compiler.cppstd = "20"  # Default to "20" for other compilers

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)
        build_type = str(self.settings.build_type)
        self.folders.build = f"build/{build_type}"
        self.folders.generators = f"build/{build_type}/generators"
    
    def source(self):
        if "opencl-icd-loader" in self.requires:
            cl_loader_dir = os.path.join(self.source_folder, "OpenCL-ICD-Loader")
            self.run(f"git clone --depth 1 https://github.com/KhronosGroup/OpenCL-ICD-Loader.git {cl_loader_dir}")
            cmake_file = os.path.join(cl_loader_dir, "CMakeLists.txt")
            replace_in_file(
                self, 
                cmake_file,
                "cmake_minimum_required(VERSION 3.5)",
                "cmake_minimum_required(VERSION 3.15)"
            )

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.variables["CMAKE_POLICY_DEFAULT_CMP0091"] = "NEW"
        tc.generate()

        # Delete CMakeUserPresets.json
        cmake_user_presets = os.path.join(self.source_folder, "CMakeUserPresets.json")
        if os.path.exists(cmake_user_presets):
            os.remove(cmake_user_presets)

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