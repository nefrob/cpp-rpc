# cpp-rpc

C++ Linux event-based RPC client and server.

Project is currently **WIP**!

### Dependencies

```
sudo apt-get install cmake
sudo apt install build-essential
```

### Setup

```
git clone https://github.com/nefrob/cpp-rpc
cd cpp-rpc
mkdir build && cd build
cmake .. && make
cd ../bin
./server listen_ip listen_port
./client server_ip server_port
```

### Example

```
./server 127.0.0.1 2000
./client 127.0.0.1 2000

TODO:
```

### Questions?

- Post issues in the [Issue Tracker](https://github.com/nefrob/cpp-rpc/issues)