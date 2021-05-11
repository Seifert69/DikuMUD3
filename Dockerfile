FROM ubuntu:20.10 as build

LABEL description="DikuMUD III Builder"
WORKDIR /dikumud3
RUN apt-get update && apt-get install -y build-essential ccache bison flex libboost-all-dev
COPY . .
RUN cd vme/src && make clean && make all -j4
RUN cd vme/etc && make all
RUN cd vme/zone && ../bin/vmc -m -I../include/ *.zon
ENTRYPOINT [ "/dikumud3/entrypoint.sh" ]
