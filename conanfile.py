from conans import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain

class App(ConanFile):
    settings = "os", "compiler", "build_type"
    requires = [("glfw/3.3.4"),
				("glm/0.9.9.5"),
				("glew/2.2.0"),
                ("spdlog/1.9.2"),
                ("shaderc/2021.1"),
                ("imgui/1.87")]
    generators = "cmake_find_package_multi"
    default_options = "shaderc:shared=False"

    def imports(self):
        dest = "../third_party/imgui_bindings"
        self.copy("imgui_impl_glfw.cpp", dst= dest, src="./res/bindings", root_package="imgui") 
        self.copy("imgui_impl_glfw.h", dst= dest, src="./res/bindings", root_package="imgui") 
        self.copy("imgui_impl_opengl3.cpp", dst=dest, src="./res/bindings", root_package="imgui")
        self.copy("imgui_impl_opengl3.h", dst=dest, src="./res/bindings", root_package="imgui") 
        self.copy("imgui_impl_opengl3_loader.h", dst=dest, src="./res/bindings", root_package="imgui") 
        self.copy("imgui_impl_vulkan.cpp", dst=dest, src="./res/bindings", root_package="imgui") 
        self.copy("imgui_impl_vulkan.h", dst=dest, src="./res/bindings", root_package="imgui") 

	
    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()
