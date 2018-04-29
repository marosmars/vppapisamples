# sudo apt-get install vpp-dev 
# ls /usr/share/vpp/api
# cat l2.api.json
# cat interface.api.json

gcc vppclient.c -o client.o -lvppinfra -lvlibmemoryclient -lsvm
sudo ./client.o