if not exist build mkdir build
cd build
conan install .. -s build_type=Release 
conan install .. -s build_type=Debug
