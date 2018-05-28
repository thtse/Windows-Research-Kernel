/*++

Copyright (c) Microsoft Corporation. All rights reserved. 

You may only use this code if you agree to the terms of the Windows Research Kernel Source Code License agreement (see License.txt).
If you do not agree to the terms, do not use the code.


Module Name:

    lpclistn.c

Abstract:

    Local Inter-Process Communication (LPC) connection system services.

--*/

#include "lpcp.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE,NtListenPort)
#endif


NTSTATUS
NtListenPort (
    __in HANDLE PortHandle,
    __out PPORT_MESSAGE ConnectionRequest
    )

/*++

Routine Description:

    A server thread can listen for connection requests from client threads
    using the NtReplyWaitReceivePort service and looking for an
    LPC_CONNECTION_REQUEST message type.

    This call will loop, calling the NtReplyWaitReceivePort service, and
    return when it sees a message of type LPC_CONNECTION_REQUEST

Arguments:

    PortHandle - Specifies the connection port to listen for connection
        requests to.

    ConnectionRequest - Pointer to a structure that describes the
        connection request the client is making:

Return Value:

    NTSTATUS - An appropriate status value

--*/

{
    NTSTATUS Status;

    PAGED_CODE();

    //
    //  Keep on looping until we get a connection request on the lpc port
    //

    while (TRUE) {

        Status = NtReplyWaitReceivePort( PortHandle,
                                         NULL,
                                         NULL,
                                         ConnectionRequest );

        //
        //  We'll return from this procedure if ever we get back non success
        //  or the message is a connection request.  We still need to protect
        //  the testing of ConnectionRequest because it is a user supplied
        //  buffer.
        //

        try {

            if ((Status != STATUS_SUCCESS) ||
                ((ConnectionRequest->u2.s2.Type & ~LPC_KERNELMODE_MESSAGE) == LPC_CONNECTION_REQUEST)) {

                break;
            }

        } except( EXCEPTION_EXECUTE_HANDLER ) {

            Status = GetExceptionCode();

            break;
        }
    }

    //
    //  And return to our caller
    //

    return Status;
}

