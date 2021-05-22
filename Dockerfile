FROM debian:bullseye as builder
WORKDIR /tmp
COPY . /tmp/src
ENV DESTDIR=/tmp/dist Boost_USE_STATIC_LIBS=False
RUN mkdir "$DESTDIR" && \
    apt update && \
    apt upgrade -y && \
    apt install -y git ninja-build cmake g++ libboost-all-dev libssl-dev && \
    cmake -G Ninja -S ./src -B ./build && \
    cmake --build ./build --target install

FROM debian:bullseye
COPY --from=builder /tmp/dist /
RUN apt update && \
    apt upgrade -y && \
    apt install -y libboost-atomic1.74.0 libboost-filesystem1.74.0 libssl1.1 && \
    rm -rf /var/lib/apt/lists/*