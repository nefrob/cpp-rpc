# cpp-rpc

C++ Linux event-based RPC server (with client to test).

### Dependencies

```
sudo apt-get install cmake
sudo apt install build-essential
sudo apt install protobuf-compiler
```

### Setup

```
git clone https://github.com/nefrob/cpp-rpc
cd cpp-rpc
mkdir build && cd build
cmake .. && make
cd ../bin
```

### Example

First run the server: `./server -i 127.0.0.1 -p 2000 -v`. You can add the `-h`  argument to see usage options. Type `s` to stop the server.

In a separate shell run the client `./client -i 127.0.0.1 -p 2000`. Type `e` to input an RPC echoe request. Type `s` to stop the client.

### Design

Having worked with blocking IO and server design, this project was completed out of curiousity to try event-based programming. Below are some general design decisions I made at different stages of implementation. This project is not robust or super easily extensible, however it does offer some interfaces (across event loop and rpc code) for further input.

- Event loop:
    - Why event-based? The standard multi-threading blocking-IO model for servers has high context switching and uses a lot of memory, as each connection gets its own thread. Using `epoll` plus an event loop to manage events reduces both these issues for higher scalability (if done right). 

        Unfortunately event-based models do suffer with shorter-lived connections, and are much harder to implement/debug/read. Furthermore, the event loop can become the bottleneck in servicing connections.
    
    - I used `epoll` to wait on events, calling their `handleEvent` method on an event being raised. All file descriptors managed by the event loop must be non-blocking in the tasks they perform, as otherwise they will stall the event loop. 
    
        To eliminate locking for event updates, all changes to events take place in the event loop. There is a single lock for queueing `thunks` to be executed in the event loop thread. The event loop only acquires this lock once per wakeup (triggered by timer event), so other threads can queue tasks while the event loop is processing events. This makes concurrency programming easier and without a specific use case to optimize shouldn't matter for performance.

    - Events are stored as `shared_ptr`s in the event loop so that other objects can manage them or let them fall out of scope when deregistered in the event loop.

- RPC:
    - The RPC server starts an event loop and registers a listening socket or "`Acceptor`" event to handle incoming connections.

    - Simple read/write socket events are implemented to allow for network communication. When receiving a message it is passed to an RPC message handler to generate the response. In this project only `echo` requests are accepted, however more complicated operations could be added. Any read/writes to get the response should be non-blocking / asynchronous to avoid stalling the event loop.

    - Google's [protocol buffers](https://developers.google.com/protocol-buffers) are used to explore best practices in sending RPC request/response data. Since this project is Linux and C++ focused most other their neutrality properties are not used.

- Future:
    - A non-blocking RPC client would complement the RPC server well.

    - Add more generalized use cases and further RPC request/response templates. 


    - non blocking client
    - make more robust (however not goal here)
    - multiple event loops, can accept on same listen_sock or other load balancing

### Questions?

- Post issues in the [Issue Tracker](https://github.com/nefrob/cpp-rpc/issues).