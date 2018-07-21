// Sample vpp client

//go:generate /home/maros/go/bin/binapi-generator --input-file=/usr/share/vpp/api/vpe.api.json --output-dir=./bin_api
//go:generate /home/maros/go/bin/binapi-generator --input-file=/usr/share/vpp/api/interface.api.json --output-dir=./bin_api
//go:generate /home/maros/go/bin/binapi-generator --input-file=/usr/share/vpp/api/l2.api.json --output-dir=./bin_api


package main

import (
        "fmt"
        "os"

        "git.fd.io/govpp.git"
        "git.fd.io/govpp.git/api"

	"vppapisample/bin_api/interfaces"
	"vppapisample/bin_api/l2"
)



func main() {
        fmt.Println("Connecting to VPP")

        // connect to VPP
        conn, err := govpp.Connect("")
        if err != nil {
                fmt.Println("Error:", err)
                os.Exit(1)
        }
        defer conn.Disconnect()

	ch, err := conn.NewAPIChannel()
        if err != nil {
                fmt.Println("Error:", err)
                os.Exit(1)
        }
        defer ch.Close()

        interfaceDump(ch)
        createLoopback(ch)
        interfaceDump(ch)
	bdDump(ch)
	createBd(ch)
	bdDump(ch)
}

func interfaceDump(ch api.Channel) {
	fmt.Println("Sending dump interfaces. Msg id: sw_interface_dump")
        req := &interfaces.SwInterfaceDump{}
        reqCtx := ch.SendMultiRequest(req)

        for {
                msg := &interfaces.SwInterfaceDetails{}
                stop, err := reqCtx.ReceiveReply(msg)
                if stop {
                        break;
                }
                if err != nil {
                        fmt.Println("Error:", err)
                }
                fmt.Printf("\tInterface, message id: sw_interface_details, interface index: %v\n", string(msg.InterfaceName[:]))
        }
}

func createLoopback(ch api.Channel) {
        req := &interfaces.CreateLoopback{}

        ch.GetRequestChannel() <- &api.VppRequest{Message: req}

        vppReply := <-ch.GetReplyChannel()
        if vppReply.Error != nil {
                fmt.Println("Error:", vppReply.Error)
                return
        }

        reply := &interfaces.CreateLoopbackReply{}
        err := ch.GetMessageDecoder().DecodeMsg(vppReply.Data, reply)

        if err != nil {
                fmt.Println("Error:", err)
        }
}

func bdDump(ch api.Channel) {
	fmt.Println("Sending dump bd. Msg id: bridge_domain_dump")
        req := &l2.BridgeDomainDump{
		BdID: ^uint32(0),
	}
        reqCtx := ch.SendMultiRequest(req)

        for {
                msg := &l2.BridgeDomainDetails{}
                stop, err := reqCtx.ReceiveReply(msg)
                if stop {
                        break;
                }
                if err != nil {
                        fmt.Println("Error:", err)
                }
                fmt.Printf("\tBridge domain, message id: bridge_domain_details, bd index: %v\n", msg.BdID)
        }
}

func createBd(ch api.Channel) {
        req := &l2.BridgeDomainAddDel{
		BdID:	99,
		IsAdd:	1,
	}

        ch.GetRequestChannel() <- &api.VppRequest{Message: req}

        vppReply := <-ch.GetReplyChannel()
        if vppReply.Error != nil {
                fmt.Println("Error:", vppReply.Error)
                return
        }

        reply := &l2.BridgeDomainAddDelReply{}
        err := ch.GetMessageDecoder().DecodeMsg(vppReply.Data, reply)

        if err != nil {
                fmt.Println("Error:", err)
        }
}



