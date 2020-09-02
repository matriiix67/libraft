/*
 * Copyright (C) lichuang
 */

#pragma once

#include <string>
#include "base/buffer.h"
#include "base/define.h"
#include "base/event.h"
#include "base/status.h"
#include "base/typedef.h"
#include "net/endpoint.h"

using namespace std;

namespace libraft {

class IDataHandler;
class EventLoop;

enum socketStatus {
  // init state
  kSocketInit = 0,
  // when client connecting the server
  kSocketConnecting = 1,
  // socket has been connected
  kSocketConnected = 2,
  // socket has been closed
  kSocketClosed = 3,
};

// Socket has user-space write and read stack to buffer the data. 
// When send data to socket, it will buffer the data, and then send if epoll return writeable.
// If send data fail, IDataHandler->onError will be called.
// When read data from socket, it will read data from the buffer.
class Socket : public IIOHandler {
  friend Socket* CreateServerSocket(const Endpoint& local, IDataHandler*, EventLoop* loop, fd_t);
  friend Socket* CreateClientSocket(const Endpoint&, IDataHandler*, EventLoop* loop);

public:
  virtual ~Socket();

  bool ServerSide() const {
    return server_side_;
  }

  void Write(const char* from, size_t n);
  size_t Read(char* to, size_t n);

  virtual void onRead(IOEvent*);

  virtual void onWrite(IOEvent*);

  void SetEventLoop(EventLoop *loop) {
    event_loop_ = loop;
  }

  const Endpoint& GetEndpoint() const {
    return local_endpoint_;
  }

  inline const string& String() {
    return local_endpoint_.String();
  }

  inline const string& RemoteString() {
    return remote_endpoint_.String();
  }

  int fd() const { 
    return fd_;
  }

private:
  // only be called if it is a client socket
  void connect();

  // socket constructor is private, it can only be creat from CreateServerSocket or CreateClientSocket
  Socket(const Endpoint& local, IDataHandler* handler, EventLoop* , fd_t fd);
  Socket(const Endpoint& remote, IDataHandler* h, EventLoop* );
  Socket();
  void close();

private:
  // socket fd
  fd_t fd_;

  // data handler
  IDataHandler *handler_;

  // event loop
  EventLoop* event_loop_;

  // true if write buffer has data to send
  bool is_writable_;

  // recv buffer
  Buffer read_buf_;

  // write buffer
  Buffer write_buf_;

  int status_;
  
  // if or not a server side socket
  bool server_side_;

  // locol endpoint 
  Endpoint local_endpoint_;

  // remote endpoint 
  Endpoint remote_endpoint_;

  IOEvent* event_;

  // disable copy and assign operate
  DISALLOW_COPY_AND_ASSIGN(Socket);
};

// create a server accepted socket
extern Socket* CreateServerSocket(const Endpoint& local, IDataHandler*, EventLoop* loop, fd_t);

// create a client socket
extern Socket* CreateClientSocket(const Endpoint&, IDataHandler*, EventLoop* loop);
};