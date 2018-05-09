# sudo apt install luajit
# sudo apt-get install vpp-api-lua 

# IMPORTANT make sure to apply vpp-lapi.lua.diff to /usr/share/vpp/examples/lua/vpp-lapi.lua because the original lapi file does not work (at least not for 18.04 and probably also not for 18.01)

sudo luajit ./client.lua
