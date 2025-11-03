FROM registry.altlinux.org/p11/alt:latest

LABEL maintainer="Savva"
LABEL description="ALT Linux Package Comparator"
LABEL version="1.0.0"

WORKDIR /app

RUN apt-get update && apt-get install -y \
    cmake gcc-c++ libcurl-devel nlohmann-json-devel \
    git make pkg-config

COPY CMakeLists.txt altlinux_compare.cpp libaltlinux_comparison.h libaltlinux_comparison.cpp ./

RUN mkdir -p build && cd build && cmake .. && make

RUN cp build/altlinux-compare /usr/local/bin/

CMD ["/bin/bash"]