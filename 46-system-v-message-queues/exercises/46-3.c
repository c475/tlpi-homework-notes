/*
    QUESTION:

    In the client-server application of Section 46.8, why does the client pass the
    identifier of its message queue in the body of the message (in the clientId field),
    rather than in the message type (mtype)?

*/


/*
    ANSWER:

    I guess it could work but that would mean that the server message queue is locked into 
    operating like a FIFO (where you would query by mtype = 0).

    This makes it unable to be extended to support querying other types of requests in the future, 
    based on the mtype field.

    So I looked at the actual answer and it's because 0 is a valid message queue identifier (and commonly is used),
    however it is not a valid number for the "mtype" field.
*/
