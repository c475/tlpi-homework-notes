/*
    The server in Listing 44-7 ( fifo_seqnum_server.c ) performs a second O_WRONLY open of
    the FIFO so that it never sees end-of-file when reading from the reading descriptor
    (serverFd) of the FIFO. Instead of doing this, an alternative approach could be tried:
    whenever the server sees end-of-file on the reading descriptor, it closes the
    descriptor, and then once more opens the FIFO for reading. (This open would
    block until the next client opened the FIFO for writing.) What is wrong with this
    approach?
*/

    It's the wrong approach because it would mean reopening the server FIFO to handle every request.