#! /bin/sh

sudo docker build -t foo . && sudo docker run -it foo

echo "docker has compiled c++ and ran"

sudo docker rmi foo -f
