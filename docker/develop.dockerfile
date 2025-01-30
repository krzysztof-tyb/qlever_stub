FROM ubuntu:24.04

ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8
ENV LC_CTYPE=C.UTF-8
ENV DEBIAN_FRONTEND=noninteractive

ARG TARGETPLATFORM
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y wget
RUN wget https://apt.kitware.com/kitware-archive.sh && chmod +x kitware-archive.sh && ./kitware-archive.sh
RUN apt-get update && apt-get install -y build-essential cmake libicu-dev tzdata pkg-config uuid-runtime uuid-dev git libjemalloc-dev ninja-build libzstd-dev libssl-dev libboost1.83-dev libboost-program-options1.83-dev libboost-iostreams1.83-dev libboost-url1.83-dev


# Set user and group
ARG user=builder
ARG group=builder
ARG uid=1001
ARG gid=1001
RUN groupadd -g ${gid} ${group}
RUN useradd -u ${uid} -g ${group} -s /bin/sh -m ${user} # <--- the '-m' create a user home directory

# Switch to user
USER ${uid}:${gid}
