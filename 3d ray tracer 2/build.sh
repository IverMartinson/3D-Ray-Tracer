clear
gcc -Ofast -march=native -flto -funroll-loops -o built.cppb main.cpp -lSDL2 -lSDL2_ttf -lm -lstdc++ -lpthread
if [ $? -ne 0 ]; then
  exit 1
fi
./built.cppb