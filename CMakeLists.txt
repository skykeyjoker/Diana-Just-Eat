cmake_minimum_required(VERSION 3.8)

project(Ordering-System LANGUAGES CXX)

# Server
add_subdirectory(Server/Ordering-System-Server)

# Client
add_subdirectory(Client/Ordering-System-Client)