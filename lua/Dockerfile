FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update

# Install Unix packages
RUN apt-get -y install make
RUN apt-get -y install wget
RUN apt-get -y install build-essential
RUN apt-get -y install libreadline-dev
RUN apt-get -y install unzip
RUN apt-get -y install git

WORKDIR /tmp

# Install lua
RUN wget http://www.lua.org/ftp/lua-5.4.3.tar.gz
RUN tar zxpf lua-5.4.3.tar.gz
RUN cd lua-5.4.3 && make all test && make all install

# Install luarocks
RUN wget https://luarocks.org/releases/luarocks-3.7.0.tar.gz
RUN tar zxpf luarocks-3.7.0.tar.gz
RUN cd luarocks-3.7.0 && ./configure && make && make install

# Clean
RUN rm -rf /tmp/*

# Install lua packages
RUN luarocks install busted
RUN luarocks install luacheck
RUN luarocks install lanes
RUN luarocks install inspect

# Check versions
RUN lua -v
RUN luarocks --version
RUN busted --version

WORKDIR /app
