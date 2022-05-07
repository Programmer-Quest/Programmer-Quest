cmake -S . -B build -DCMAKE_CXX_FLAGS="-Wall -Wextra --pedantic"
cmake --build build && ./build/ProgrammerQuest "$@"