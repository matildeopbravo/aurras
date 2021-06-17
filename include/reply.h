#ifndef REPLY_H
#define REPLY_H

typedef enum state { NOTHING = -1, PENDING, PROCESSING, FINISHED } State;

typedef struct reply {
    State state;
} Reply;

#endif
