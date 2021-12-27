from conans import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain

class App(ConanFile):
    settings = "os", "compiler", "build_type"
    requires = [("glfw/3.3.4"),
				("glm/0.9.9.5"),
				("glew/2.2.0")]
    generators = "cmake_find_package_multi"
	
    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()
