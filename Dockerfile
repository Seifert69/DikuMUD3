FROM ubuntu:latest as build

LABEL description="DikuMUD III Builder"
WORKDIR /dikumud3
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y build-essential ccache bison flex libboost-all-dev libssl-dev rapidjson-dev cmake nginx vim ninja-build gdb tcpdump net-tools
RUN rm -f /etc/nginx/sites-enabled/default
COPY . .
COPY vme/nginx/web.conf /etc/nginx/conf.d/.
RUN mkdir -p /dikumud3/vme/www/docker/img
COPY vme/www/img /dikumud3/vme/www/docker/img
COPY vme/www/client/*.css /dikumud3/vme/www/docker
COPY vme/www/client/*.js /dikumud3/vme/www/docker
RUN mkdir dkr_build && cd dkr_build && cmake -G Ninja .. && ninja
VOLUME /dikumud3/vme/lib
ENTRYPOINT [ "/dikumud3/entrypoint.sh" ]
