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
    wget;

RUN rm -rf /var/lib/apt/lists/*

RUN python3 -m pip install pip
RUN python3 -m pip install conan

# Download, build and install upcxx
FROM BASE as UPCXX_BUILDER
# Prepare build args
ARG THREAD_COUNT=16

# Prepare env vars
ENV UPCXX_VERSION="2021.9.0"
ENV UPCXX_RELEASE="upcxx-$UPCXX_VERSION"
ENV UPCXX_RELEASE_URL="https://bitbucket.org/berkeleylab/upcxx/downloads/$UPCXX_RELEASE.tar.gz"
ENV UPCXX_INSTALL="/workspace/libs/upcxx"
ENV PATH="$UPCXX_INSTALL/bin:$PATH"
#ENV UPCXX_ROOT="$UPCXX_INSTALL/share/cmake"

WORKDIR /workspace/libs
RUN wget -c $UPCXX_RELEASE_URL
RUN tar zxvf $UPCXX_RELEASE.tar.gz

WORKDIR ./$UPCXX_RELEASE
RUN ./configure --prefix=$UPCXX_INSTALL
RUN make -j $THREAD_COUNT all
RUN make install
RUN rm -f $UPCXX_RELEASE.tar.gz

# Download, build and install pgasgraph
FROM UPCXX_BUILDER as PGASGRAPH_BUILDER
ARG THREAD_COUNT=16
ENV CONAN_CPU_COUNT=16

WORKDIR /workspace/pgasgraph
RUN git clone https://github.com/lnikon/pgas-graph.git .
RUN mkdir build
RUN conan profile new default --detect
RUN conan profile update settings.compiler.libcxx=libstdc++11 default
RUN conan install --build conancenter -if ./build .
RUN cmake -S. -B./build
RUN cmake --build ./build -j$THREAD_COUNT

