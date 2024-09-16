# Local client server program in C

Program I made that allows communication over sockets using linux socket libraries in C.

## Installation

Requires GCC to compile

```sh
# Clone the repository
git clone https://github.com/michaelzhang4/local_client_server.git

# Navigate to the project directory
cd local_client_server

cd tcp_socket
# For tcp, run these commands in different terminals
make singleserver
make singleclient

# OR
make multiserver
make multiclient

cd udp_socket
# For udp, run these commands in different terminals
make server
make client

# Use this to clean up executables in either folder
make clean
```

## Notes

Works on Linux only.

Udp is only one way communication through client.

Tcp programs allow bidirectional communication with multiserver allowing communication with up to 3 client programs at a time.

## Screenshot

<img width="1533" alt="WindowsTerminal_LDZ78NNLMw" src="https://github.com/user-attachments/assets/a4069fbe-0d95-4693-a2e6-d473b1ca2dd3">

