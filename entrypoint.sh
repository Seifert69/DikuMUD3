#!/bin/bash

done=0

function signals()
{
  if [ $done -eq 0 ]; then
    kill $(pgrep vme)
    kill $(pgrep mplex)
  fi
  done=1
}

trap signals INT TERM EXIT HUP


# Override the default values if the environment variables are set.
WS_HOST=${WS_HOST:-localhost}
WS_PORT=${WS_PORT:-4280}
if [ "$WS_HOST" != "localhost" ] || [ "$WS_PORT" != "4280" ] ; then
  echo "Starting websocket server on ws://${WS_HOST}:${WS_PORT}/echo"
  DOCKER_CLIENT_FPATH="/dikumud3/vme/www/docker/index.html"
  sed -i "s/encodeURI('localhost')/encodeURI('${WS_HOST}')/g" ${DOCKER_CLIENT_FPATH}
  sed -i "s/encodeURI('4280')/encodeURI('${WS_PORT}')/g" ${DOCKER_CLIENT_FPATH}
fi

# Start services.
service nginx restart
cd /dikumud3/vme/bin
rm -f vme.log mplex.log
tail -F vme.log mplex.log &
./vme &
./mplex -w -p 4280 &

while [ $done -eq 0 ]
do
  sleep 1
done

echo -e "\n\nFinished\n"