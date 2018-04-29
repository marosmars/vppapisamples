package com.mars.vpp.client.sample.kotlin

import io.fd.vpp.jvpp.JVppRegistryImpl
import io.fd.vpp.jvpp.core.JVppCoreImpl
import io.fd.vpp.jvpp.core.dto.BridgeDomainAddDel
import io.fd.vpp.jvpp.core.dto.BridgeDomainDump
import io.fd.vpp.jvpp.core.dto.CreateLoopback
import io.fd.vpp.jvpp.core.dto.SwInterfaceDump
import io.fd.vpp.jvpp.core.future.FutureJVppCoreFacade

object VppClient {

    private const val CLIENT_ID = "Vppclient"

    @JvmStatic
    fun main(args: Array<String>) {

        JVppRegistryImpl(CLIENT_ID).use { registry ->
            JVppCoreImpl().use { jVppCore ->
                FutureJVppCoreFacade(registry, jVppCore).use { coreFacade ->

                    // Dump interfaces
                    dumpInterfaces(coreFacade)

                    // Create loopback
                    createLoopback(coreFacade)

                    // Dump interfaces
                    dumpInterfaces(coreFacade)

                    // Dump bds
                    dumpBds(coreFacade)

                    // Create bd
                    createBd(coreFacade)

                    // Dump bds
                    dumpBds(coreFacade)
                }
            }
        }
    }

    private fun createLoopback(coreFacade: FutureJVppCoreFacade) {
        CreateLoopback().let { request ->
            println("Sending create loopback. Msg id: ${request.javaClass.simpleName}")
            coreFacade.createLoopback(request)
                    .toCompletableFuture()
                    .get()
                    .let {
                        println("\tCreate loopback reply received, message id: " +
                                "${it.javaClass.simpleName}, new interface index: ${it.swIfIndex}")
                    }
        }
    }

    private fun createBd(coreFacade: FutureJVppCoreFacade) {
        BridgeDomainAddDel().let { request ->
            request.bdId = 99
            request.isAdd = 1
            println("Sending create bd. Msg id: ${request.javaClass.simpleName}")
            coreFacade.bridgeDomainAddDel(request)
                    .toCompletableFuture()
                    .get()
                    .let {
                        println("\tCreate bd reply received, message id: ${it.javaClass.simpleName}")
                    }
        }
    }

    private fun dumpInterfaces(coreFacade: FutureJVppCoreFacade) {
        SwInterfaceDump().let { request ->
            println("Sending dump interfaces. Msg id: ${request.javaClass.simpleName}")
            coreFacade.swInterfaceDump(request)
                    .toCompletableFuture()
                    .get()
                    .swInterfaceDetails
                    .forEach {
                        println("\tInterface, message id: ${it.javaClass.simpleName}, interface index: ${it.swIfIndex}")
                    }
        }
    }

    private fun dumpBds(coreFacade: FutureJVppCoreFacade) {
        BridgeDomainDump().let { request ->
            request.bdId = 0.inv()
            println("Sending dump bridge domains. Msg id: ${request.javaClass.simpleName}")
            coreFacade.bridgeDomainDump(request)
                    .toCompletableFuture()
                    .get()
                    .bridgeDomainDetails
                    .forEach {
                        println("\tBridge domain, message id: ${it.javaClass.simpleName}, bd index: ${it.bdId}")
                    }
        }
    }
}
