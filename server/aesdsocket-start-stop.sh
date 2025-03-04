#! /bin/sh

case "$1" in 
    start)
        echo "Starting socket server"
        start-stop-daemon -S -n aesdsocket -a usr/bin/aesdsocket -- "-d"
        ;;
    stop)
        echo "Starting socket server"
        start-stop-daemon -K -s TERM -n aesdsocket
        ;;
    *)
        echo "Usage $0 {start/stop}"
    exit 1
esac
