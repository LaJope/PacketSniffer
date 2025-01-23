FROM gcc:9

# Set the working directory
WORKDIR /usr/src

RUN set -ex;\
    apt-get update;\
    apt-get install -y cmake flex bison;

# Copy the current directory contents into the container at /usr/src
COPY . .

# Create a build directory and build the project
RUN set -ex
    RUN mkdir -p build
    RUN cmake -S . -B build
    RUN cmake --build build
