/*
  supplement for WinSock.
*/
#pragma once

//Data on the number of bytes in IP.
#define IPv4_SIZE     4
#define IPv6_SIZE    16
#define IP_PORT_SIZE  2

//Temporary function for cleaning the data unknown to me. I know them, you need to prefix.
#define HZ_IPV6_CLEAR(s) {if(((SOCKADDR_IN6 *)(s))->sin6_family == AF_INET6){((SOCKADDR_IN6 *)(s))->sin6_flowinfo = ((SOCKADDR_IN6 *)(s))->sin6_scope_id = 0;}}

namespace WSocket
{
  /*
    Init.
    
    Return - true - in case of success,
             false - in case of error.
  */
  bool init(void);

  /*
    Uninit.
  */
  void uninit(void);

  /*
    Checks whether there is a socket in the list fd_set.

    IN s   - socket to check.
    IN pfs - pointer to fd_set.

    Return - true - if it exists,
             false - if does not exists.
  */
  bool _fdIsSet(SOCKET s, fd_set *pfs);
  
  /*
    Read data from socket.

    IN s       - socket.
    OUT buf    - data buffer.
    IN bufSize - buffer size.
    IN timeout - the number of milliseconds to wait for data acquisition, or 0 for infinite 
				 wait on the blocked socket.

    Return     - the number of bytes read or SOCKET_ERROR.
  */
  int tcpRecv(SOCKET s, void *buf, int bufSize, DWORD timeout);

  /*
    Reading data from a socket until the buffer is full.

	IN s       - socket.
	OUT buf    - data buffer.
	IN bufSize - buffer size.
	IN timeout - the number of milliseconds to wait for data acquisition, or 0 for infinite 
				 wait on the blocked socket.

    Return     - true - in case of success,
                 false - in case of error.
  */
  bool tcpRecvAll(SOCKET s, void *buf, int bufSize, DWORD timeout);

  /*
    Read all the data from the socket until NULL.

    IN s       - socket.
    IN size    - data size.
	IN timeout - the number of milliseconds to wait for data acquisition, or 0 for infinite wait.

    Return     - true - in case of success,
                 false - in case of error.
  */
  bool tcpRecvAllToNull(SOCKET s, int size, DWORD timeout);

  /*
    Write data to a socket.

    IN s       - socket.
    IN buf     - data buffer.
    IN bufSize - buffer size.

    Return     - true - in case of success,
                 false - in case of error.
  */
  bool tcpSend(SOCKET s, const void *buf, int bufSize);

  /*
    Create a TCP connection.
    
    IN sockAddr - server data to which you want to connect.

	Return      - ID socket - When connected, after the session socket must
				  be closed through tcpClose,
                  INVALID_SOCKET - in case of error.
  */
  SOCKET tcpConnect(const SOCKADDR_STORAGE *sockAddr);
  
  /*
    Create a TCP connection.
    
    IN host - domain/IPv4/IPv6.
    IN port - port.

	Return  - ID socket - When connected, after the session socket must
  						  be closed through tcpClose,
	  					  INVALID_SOCKET - in case of error.
  */
  SOCKET tcpConnectA(const LPSTR host, const WORD port);

  /*
    Creates a TCP port to accept connections.
    
    IN sockAddr - server data.
    IN backlog  - the maximum number of pending connections or SOMAXCONN.
    
	Return      - ID socket - If successful, the port, when the port is no longer needed,
				  must be closed through tcpClose,
                  INVALID_SOCKET - in case of error.
  */
  SOCKET tcpListen(const SOCKADDR_STORAGE *sockAddr, int backlog);
  
  /*
    Creates a TCP port to accept connections on any port.
    
    IN family  - protocol type, AF_INET or AF_INET6.
    IN backlog - the maximum number of pending connections or SOMAXCONN.
    IN portMin - minimum port number in the machine byte order.
    IN portMax - maximum port number in the machine byte order.

	Return     - socket ID - If successful, the port, when the port is no longer needed,
				 must be closed through tcpClose,
                 INVALID_SOCKET - in case of error.
    
    Note: Requires initialization of Crypt.
  */
  SOCKET tcpListenRandom(int family, int backlog, WORD portMin, WORD portMax);

  /*
    Wrapper for TCP_Listen to easily create a port.
    
    IN family  - protocol type, AF_INET or AF_INET6.
    IN port    - port number in the machine byte order.
    IN backlog - the maximum number of pending connections or SOMAXCONN.

	Return     - socket ID - If successful, the port, when the port is no longer needed,
				 must be closed through tcpClose,
                 INVALID_SOCKET - in case of error.
  */
  SOCKET tcpListenEx(int family, WORD port, int backlog);
  
  /*
    The organization of the tunnel between the two connections.
    
    IN s1 - first socket.
    IN s2 - second socket.

    Note: The output of the function will be performed only after the creation of connection or error.
  */
  void tcpTunnel(SOCKET s1, SOCKET s2);
  
  /*
    The organization of the tunnel between the two connections, with the ability to interrupt event.
    
	IN s1          - first socket.
	IN s2		   - second socket.
    IN eventHandle - event routine, in which the operation of tunnel will be closed.
  */
  void tcpTunnelAndWaitForWinEvent(SOCKET s1, SOCKET s2, HANDLE eventHandle);

  /*
    Waiting for an event to read sockets.

    IN sockets         - an array of sockets on which the event is expected to.
    IN socketsCount    - the number of elements in the array of sockets.
    IN timeout         - the number of milliseconds to wait, or 0 for infinite wait.
    IN advSockets      - array of additional sockets for tracking an event read.
    IN advSocketsCount - the number of elements in the array advSockets.

    Return             - socket ID on which the event occurred,
                         INVALID_SOCKET - in case of error, timeout, 
						 triggering event on any socket in advSockets.

    Note: In the case of a timeout, WSAGetLastError() will return the WSAETIMEDOUT.
  */
  SOCKET tcpWaitForEvent(const SOCKET *sockets, DWORD socketsCount, DWORD timeout, const SOCKET *advSockets, DWORD advSocketsCount);

  /*
    Waiting for incoming connections and getting a new socket.
    
	IN sockets         - an array of sockets on which the event is expected to.
	IN socketsCount    - the number of elements in the array of sockets.
	IN timeout         - the number of milliseconds to wait, or 0 for infinite wait.
    OUT sockAddr       - buffer for client information, or NULL if the information is not needed.
    OUT index          - index in the array of sockets, for which event fired. May be
                         NULL.
						 IN advSockets      - array of additional sockets for tracking an event read.
						 IN advSocketsCount - the number of elements in the array advSockets.
    
	Return             - Socket - Upon a successful connection when the socket is no longer needed,
						 must be closed through tcpClose,
						 INVALID_SOCKET - in case of error, timeout, 
						 triggering event on any socket in advSockets.

    Note: In the case of a timeout, WSAGetLastError() will return the WSAETIMEDOUT.
  */
  SOCKET tcpWaitForIncomingAndAccept(SOCKET *sockets, DWORD socketsCount, DWORD timeout, SOCKADDR_STORAGE *sockAddr, LPDWORD index, SOCKET *advSockets, DWORD advSocketsCount);

  /*
    Closure and release of the socket.

    IN s - socket to close. possible value INVALID_SOCKET.
  */
  void tcpClose(SOCKET s);

  /*
    Enables or disables the algorithm Nagle(TCP_NODELAY). Is enabled by default for all sockets.

    IN s       - socket.
    IN disable - true - disable the algorithm.
                 false - enable the algorithm,
    
    Return     - true - in case of successful change of state,
                 false - in case of error.
  */
  bool tcpDisableDelay(SOCKET s, bool disable);

  /*
    Enables or disables TCP keepalive.

    IN s        - socket.
    IN enable   - true - enable.
                  false - disable,
    IN time     - period of inactivity in ms through which the packet is sent.
    IN interval - interval of sending packets if not reached previous.
                
               
    Return      - true - in case of successful change of state,
                  false - in case of error.
  */
  bool tcpSetKeepAlive(SOCKET s, bool enable, DWORD time, DWORD interval);

  /*
    Selecting blocking, non-blocking socket.

    IN s       - socket.
    IN disable - true -  non-blocking,
                 false - blocking.
    
    Return     - true - in case of successful change of state,
                 false - in case of error.

  */
  bool setNonBlockingMode(SOCKET s, bool enable);
  
  /*
    Creates a UDP port to accept connections.

    IN sockAddr - server data.

	Return      - socket ID - If successful, the port, when the port is no longer needed
				  must be closed through udpClose,
                  INVALID_SOCKET - in case of error.
  */
  SOCKET udpListen(const SOCKADDR_STORAGE *sockAddr);

  /*
    Closure and release of the socket.

    IN s - socket to close. possible value INVALID_SOCKET.
  */
  void udpClose(SOCKET s);

  /*
    IP conversion to a string ip:port.

    IN sockAddr - IP data structure.
    OUT buffer  - buffer string, must be no smaller than MAX_PATH.
    
    Return      - true - in the case of a successful conversion,
                  false - in case of error, string will equals to "0:0".
  */
  bool ipToStringW(const SOCKADDR_STORAGE *sockAddr, LPWSTR buffer);

  /*
    Converting a string to IP.

    OUT sockAddr - IP data structure.
    IN buffer    - string.

    Return       - true - in the case of a successful conversion,
                   false - in case of error, sockAddr will be NULL.
  */
  bool stringToIpW(SOCKADDR_STORAGE *sockAddr, const LPWSTR string);

  /*
    Checks whether the IP is local.
    
    IN sockAddr - IP data structure.

    Return      - true - is local,
                  false - is not local.
  */
  bool _isLocalIp(const SOCKADDR_STORAGE *sockAddr);

  /*
    Retrieve a list of IP-addresses of all interfaces.

    IN family - AF_INET or AF_INET6.

    Return    - NULL - in case of error,
                or address list in case of success (must be freed by Mem).
  */
  SOCKET_ADDRESS_LIST *getListOfIpAddresses(int family);

  /*
    Obtaining socket type AF_*.

    IN s   - socket.
    
    Return - AF_*.
  */
  USHORT getFamily(SOCKET s);
};
