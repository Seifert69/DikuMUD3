FROM ubuntu:latest as build

LABEL description="DikuMUD III Builder"
WORKDIR /dikumud3
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y build-essential ccache bison flex libboost-all-dev cmake
COPY . .
RUN mkdir dkr_build && cd dkr_build && cmake .. && make -j 4
ENTRYPOINT [ "/dikumud3/entrypoint.sh" ]
