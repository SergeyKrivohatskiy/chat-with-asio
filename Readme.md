Asynch chat with boost asio
===========================

## Dependencies:
- `curl` (install it with apt-get for ubuntu: `sudo apt-get install curl`)

## Build:
1. `git clone https://github.com/SergeyKrivohatskiy/chat-with-asio`
2. `cd chat-with-asio`
3. `./download_and_build_dependencies.sh` - download and build boost and protobuf libs (it will take some time)
4. `export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$PWD/protobuf/install_out/lib" && ./protobuf/install_out/bin/protoc -I=. --cpp_out=./src ./message.proto` - create message class from `message.proto`
5. `make` - build chat server executable


