#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>
#include <iostream>
using namespace std;

typedef struct
{
    long msg_type;
    char msg_text[128];
} message_buf;

int main()
{
    // r w x ; r w x ; r w x
    // 1 1 0 ; 1 1 0 ; 1 1 0

    int msg_flags = IPC_CREAT | 0666;
    message_buf message;
    key_t key = 777;
    int msg_id = msgget(key, msg_flags);

    int rc = msgrcv(msg_id, &message, 128, 1, 0);
    cout << message.msg_text << std::endl;
}