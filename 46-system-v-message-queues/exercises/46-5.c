/*
    The client shown in Listing 46-9 ( svmsg_file_client.c ) doesn’t handle various
    possibilities for failure in the server. In particular, if the server message queue fills
    up (perhaps because the server terminated and the queue was filled by other
    clients), then the msgsnd() call will block indefinitely. Similarly, if the server fails to
    send a response to the client, then the msgrcv() call will block indefinitely. Add code
    to the client that sets timeouts (Section 23.3) on these calls. If either call times out,
    then the program should report an error to the user and terminate.

*/

/*

    This code is included in the 46-4-client.c file

*/
