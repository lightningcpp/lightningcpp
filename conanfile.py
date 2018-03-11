from conans import ConanFile, CMake, tools
import zipfile

class LightningcppConan(ConanFile):
    name = "lightningcpp"
    version = '0.1.0'
    url = 'https://github.com/'
    license = 'LGPL'
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    description = ""
    requires = "asio/1.11.0@bincrafters/stable", "re2/master@conan-cpp/latest", "gtest/1.8.0@bincrafters/stable"
    options = { "shared": [True, False], "build_tests":  [True, False], "build_samples": [True, False]}
    default_options = "shared=False", "*:shared=False", "build_tests=False", "build_samples=False"
    exports_sources = "*"

    def build(self):
        cmake = CMake(self)
        cmake.configure(defs={
                "CMAKE_INSTALL_PREFIX": self.package_folder
            }, source_dir=".")
        cmake.build(target="install")

    def package(self):
        self.copy('*.h', dst='include', src='.')
        if self.options.shared:
            if self.settings.os == "Macos":
                self.copy(pattern="*.dylib", dst="lib", keep_path=False)
            else:
                self.copy(pattern="*.so*", dst="lib", keep_path=False)
        else:
            self.copy(pattern="*.a", dst="lib", src='lib', keep_path=False)

    def package_info(self):
        pass

