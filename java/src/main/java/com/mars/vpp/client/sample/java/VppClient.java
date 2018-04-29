package com.mars.vpp.client.sample.java;

import io.fd.vpp.jvpp.JVppRegistryImpl;
import io.fd.vpp.jvpp.core.JVppCore;
import io.fd.vpp.jvpp.core.JVppCoreImpl;
import io.fd.vpp.jvpp.core.dto.BridgeDomainAddDel;
import io.fd.vpp.jvpp.core.dto.BridgeDomainAddDelReply;
import io.fd.vpp.jvpp.core.dto.BridgeDomainDump;
import io.fd.vpp.jvpp.core.dto.CreateLoopback;
import io.fd.vpp.jvpp.core.dto.CreateLoopbackReply;
import io.fd.vpp.jvpp.core.dto.SwInterfaceDump;
import io.fd.vpp.jvpp.core.future.FutureJVppCoreFacade;

public class VppClient {

    public static final String CLIENT_ID = "Vppclient";

    public static void main(String[] args) throws Exception {

        try (JVppRegistryImpl registry = new JVppRegistryImpl(CLIENT_ID);

             JVppCore jVppCore = new JVppCoreImpl();
             FutureJVppCoreFacade coreFacade = new FutureJVppCoreFacade(registry, jVppCore)) {

            // Dump interfaces
            dumpInterfaces(coreFacade);

            // Create loopback
            createLoopback(coreFacade);

            // Dump interfaces
            dumpInterfaces(coreFacade);

            // Dump bds
            dumpBds(coreFacade);

            // Create bd
            createBd(coreFacade);

            // Dump bds
            dumpBds(coreFacade);
        }
    }

    private static void createLoopback(FutureJVppCoreFacade coreFacade) throws Exception {
        CreateLoopback createLoopback = new CreateLoopback();
        System.out.printf("Sending create loopback. Msg id: %s\n", createLoopback.getClass().getSimpleName());
        CreateLoopbackReply createLoopbackReply = coreFacade.createLoopback(createLoopback)
                .toCompletableFuture()
                .get();

        System.out.printf("\tCreate loopback reply received, message id: %s, new interface index: %d\n",
                createLoopbackReply.getClass().getSimpleName(), createLoopbackReply.swIfIndex);
    }

    private static void createBd(FutureJVppCoreFacade coreFacade) throws Exception {
        BridgeDomainAddDel createBd = new BridgeDomainAddDel();
        createBd.bdId = 99;
        createBd.isAdd = 1;
        System.out.printf("Sending create bd. Msg id: %s\n", createBd.getClass().getSimpleName());
        BridgeDomainAddDelReply createBdReply = coreFacade.bridgeDomainAddDel(createBd)
                .toCompletableFuture()
                .get();

        System.out.printf("\tCreate bd reply received, message id: %s\n",
                createBdReply.getClass().getSimpleName());
    }

    private static void dumpInterfaces(FutureJVppCoreFacade coreFacade) throws Exception {
        SwInterfaceDump swInterfaceDump = new SwInterfaceDump();
        System.out.printf("Sending dump interfaces. Msg id: %s\n", swInterfaceDump.getClass().getSimpleName());
        coreFacade.swInterfaceDump(swInterfaceDump)
                .toCompletableFuture()
                .get()
                .swInterfaceDetails
                .forEach(ifcDetail -> System.out.printf(
                        "\tInterface, message id: %s, interface index: %s\n",
                        ifcDetail.getClass().getSimpleName(), ifcDetail.swIfIndex));
    }

    private static void dumpBds(FutureJVppCoreFacade coreFacade) throws Exception {
        BridgeDomainDump bdDump = new BridgeDomainDump();
        bdDump.bdId = ~0;
        System.out.printf("Sending dump bridge domains. Msg id: %s\n", bdDump.getClass().getSimpleName());
        coreFacade.bridgeDomainDump(bdDump)
                .toCompletableFuture()
                .get()
                .bridgeDomainDetails
                .forEach(ifcDetail -> System.out.printf(
                        "\tBridge domain, message id: %s, bd index: %s\n",
                        ifcDetail.getClass().getSimpleName(), ifcDetail.bdId));
    }
}
