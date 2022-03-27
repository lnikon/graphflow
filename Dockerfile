FROM ubuntu:latest as BASE

ARG DEBIAN_FRONTEND=noninteractive
RUN set -ex;                \
    apt-get -y update;      \
    apt-get -y upgrade;     \
    apt-get -y install      \
    sudo                    \
    bash                    \
    build-essential         \
    perl                    \
    libffi-dev              \
    python3-dev             \
    git                     \
    cmake                   \
    python3                 \
    python3-pip             \
    openssh-server          \
    rsync                   \
    wget;

RUN rm -rf /var/lib/apt/lists/*

# Configure conan
ENV CONAN_CPU_COUNT=16
RUN python3 -m pip install pip
RUN python3 -m pip install conan
RUN conan profile new default --detect
RUN conan profile update settings.compiler.libcxx=libstdc++11 default

# Download, build and install upcxx
FROM BASE as UPCXX_BUILD_INSTALL
# Prepare build args
ENV THREAD_COUNT=16

# Prepare env vars
ARG UPCXX_VERSION="2021.9.0"
ENV UPCXX_VERSION $UPCXX_VERSION

ARG UPCXX_RELEASE="upcxx-$UPCXX_VERSION"
ENV UPCXX_RELEASE $UPCXX_RELEASE

ARG UPCXX_RELEASE_URL="https://bitbucket.org/berkeleylab/upcxx/downloads/$UPCXX_RELEASE.tar.gz"
ENV UPCXX_RELEASE_URL $UPCXX_RELEASE_URL

ARG UPCXX_INSTALL="/workspace/libs/upcxx"
ENV UPCXX_INSTALL $UPCXX_INSTALL

WORKDIR /workspace/libs
RUN wget -c $UPCXX_RELEASE_URL
RUN tar zxvf $UPCXX_RELEASE.tar.gz

WORKDIR ./$UPCXX_RELEASE
RUN ./configure --prefix=$UPCXX_INSTALL
RUN make -j $THREAD_COUNT all
RUN make install
RUN rm -f $UPCXX_RELEASE.tar.gz

# Should be called with mount .:/workspace/pgasgraph
FROM UPCXX_BUILD_INSTALL AS PGASGRAPH_DEVELOPMENT
WORKDIR /workspace/pgasgraph
ENTRYPOINT ["/bin/bash"]

# Download, build and install pgasgraph
FROM UPCXX_BUILD_INSTALL as PGASGRAPH_BUILD_INSTALL
WORKDIR /workspace/pgasgraph
RUN git clone https://github.com/lnikon/pgas-graph.git .
RUN mkdir build
RUN conan install -r conancenter --profile conanprofile.toml -if ./build .
RUN cmake -S. -B./build
RUN cmake --build ./build -j$THREAD_COUNT
