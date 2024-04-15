HTTP server library that parses requests over TCP/IP sockets, routes to handlers, and sends HTTP responses.

## High-level
User code interacts with BasicHttpServer to implement path handlers, and uses Request and Response to query parsed HTTP requests and send HTTP responses.

## Low-level
The Listener and Connection classes interact directly with the C POSIX API and are used by the higher level library components.

## Example
Consumer.cpp provides an example of using the library.

The public folder contains test front-end files and need to be located in the same directory as the application if using the consumer example.