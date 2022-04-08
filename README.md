# blockstore

sudo apt update 

sudo apt install gcc fuse3 libfuse3-dev make cmake 

git clone https://github.com/Microsoft/vcpkg.git 

cd vcpkg

./vcpkg install grpc benchmark

Compile code:

cd into git repository.

mkdir build 
cd build 
cmake ../ 
make

sudo mount /dev/sda4 ~/dev
