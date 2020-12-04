#!/bin/bash
gcc -o client client.c
gcc -o server server.c
gcc -o keygen keygen.c

cp client.c copy/client.c
cp server.c copy/server.c
cp keygen.c copy/keygen.c
