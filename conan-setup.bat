if not exist build mkdir build
cd build
conan install .. -s build_type=Release  -pr:b=default
conan install .. -s build_type=Debug  -pr:b=default
