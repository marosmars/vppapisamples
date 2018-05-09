package.path = package.path .. ";/usr/share/vpp/examples/lua/vpp-lapi.lua"

vpp = require "vpp-lapi"

function vpp_connect ()
	vac_path = "/usr/lib/x86_64-linux-gnu/libvppapiclient.so"
	vpp:init({ vac_path = vac_path })

	vpp:json_api("/usr/share/vpp/api/vpe.api.json")
	vpp:json_api("/usr/share/vpp/api/memclnt.api.json")
	vpp:json_api("/usr/share/vpp/api/interface.api.json")
	vpp:json_api("/usr/share/vpp/api/l2.api.json")

	vpp:connect("vppclient")
end

function dump_interfaces ()
	print("Dumping interfaces")
	reply = vpp:api_call("sw_interface_dump")
	--print(vpp.dump(reply))
	for k, v in pairs(reply) do
	  print("\t Interface: ", v.sw_if_index)
	end
end

function dump_bds ()
	print("Dumping bds")
	reply = vpp:api_call("bridge_domain_dump", {bd_id = tonumber("FFFFFFFF",16)})
	--print(vpp.dump(reply))
	for k, v in pairs(reply) do
	  print("\t Bridge domain: ", v.bd_id)
	end
end

function create_loopback ()
	print("Creating loopback")
	reply = vpp:api_call("create_loopback")
	--print(vpp.dump(reply))
	print("\t Loopack created with rertval", reply[1].retval)
end


function create_bd ()
	print("Creating bd")
	reply = vpp:api_call("bridge_domain_add_del", {is_add = 1, bd_id = 99})
	--print(vpp.dump(reply))
	print("\t Bd created with rertval", reply[1].retval)
end

vpp_connect()
dump_interfaces()
create_loopback()
dump_interfaces()
dump_bds()
create_bd()
dump_bds()

vpp:disconnect()


