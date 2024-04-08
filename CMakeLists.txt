cmake_minimum_required(VERSION 3.21)
project(CppServer)

set(ParserPath "./lib/BufferedParsers/")
set(Parsers ${ParserPath}BufferedBodyParser.cpp ${ParserPath}BufferedHeaderParser.cpp ${ParserPath}BufferedReqLineParser.cpp ${ParserPath}BufferedRequestParser.cpp ${ParserPath}ParseResult.cpp)


add_executable(${PROJECT_NAME} consumer.cpp ./lib/Listener.cpp ./lib/MiddlewareChain.cpp ./lib/Request.cpp ./lib/FDescriptor.cpp ./lib/Connection.cpp ./lib/Response.cpp ./lib/BasicHttpServer.cpp ./lib/ConnectionPool.cpp ./lib/FileServer.cpp ${Parsers})
target_compile_options(${PROJECT_NAME} PRIVATE -g -Wall -Wextra)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
