/**
 * \file
 * \brief Request processing
 */

#include "seawolf.h"
#include "seawolf_hub.h"

static int Hub_Process_comm(Hub_Client* client, Comm_Message* message);
static int Hub_Process_notify(Hub_Client* client, Comm_Message* message);
static int Hub_Process_log(Comm_Message* message);
static int Hub_Process_var(Hub_Client* client, Comm_Message* message);

/**
 * \defgroup Process Process
 * \brief Message processing
 * \{
 */

/**
 * \brief Process a message with the COMM prefix
 *
 * Process a message related to core Comm functions. Connection establishment,
 * shutdown, authentication, etc.
 *
 * \param message The received message
 * \param client The client which sent the receive message
 * \return Return value specifies the scope of the response (RESPOND_TO_SENDER,
 * RESPOND_TO_ALL, etc.)
 */
static int Hub_Process_comm(Hub_Client* client, Comm_Message* message) {
    Comm_Message* response = NULL;
    const char* actual_password;
    char* supplied_password = NULL;

    if(message->count == 3 && strcmp(message->components[1], "AUTH") == 0) {
        actual_password = Hub_Config_getOption("password");
        supplied_password = message->components[2];

        if(actual_password == NULL) {
            Hub_Logging_log(ERROR, "No password set! Refusing to authenticate clients!");
            return -1;
        }

        response = Comm_Message_new(2);
        response->request_id = message->request_id;
        response->components[0] = MemPool_strdup(response->alloc, "COMM");

        if(strcmp(supplied_password, actual_password) == 0) {
            response->components[1] = MemPool_strdup(response->alloc, "SUCCESS");
            Hub_Net_sendMessage(client, response);
            client->state = CONNECTED;
        } else {
            response->components[1] = MemPool_strdup(response->alloc, "FAILURE");
            Hub_Net_sendMessage(client, response);
            Hub_Client_kick(client, "Authentication failure");
        }

        Comm_Message_destroy(response);
    } else if(message->count == 2 && strcmp(message->components[1], "SHUTDOWN") == 0) {
        Hub_Client_close(client);
    } else {
        return -1;
    }

    return 0;
}

/**
 * \brief Process an incoming notification
 *
 * Process a received notification message by rebroadcasting the notification
 * to all connected clients
 *
 * \param message The received message
 * \param response The response it stored here
 * \return Response action
 */
static int Hub_Process_notify(Hub_Client* client, Comm_Message* message) {
    static char* notify_0 = "NOTIFY";
    static char* notify_1 = "IN";

    Comm_Message* notification;

    if(message->count == 3 && strcmp(message->components[1], "OUT") == 0) {
    	notification = Comm_Message_new(3);
    	notification->components[0] = notify_0;
    	notification->components[1] = notify_1;
        notification->components[2] = message->components[2];
        Hub_Net_broadcastNotification(notification);

        Comm_Message_destroy(notification);

    } else if(message->count == 4 && strcmp(message->components[1], "ADD_FILTER") == 0) {
        Notify_FilterType type = (Notify_FilterType) atoi(message->components[2]);
        const char* filter_body = message->components[3];
        Hub_Client_addFilter(client, type, filter_body);


    } else if(message->count == 2 && strcmp(message->components[1], "CLEAR_FILTERS") == 0) {
        Hub_Client_clearFilters(client);

    } else {
        return -1;
    }

    return 0;
}

/**
 * \brief Process a log message
 *
 * Process a log message requesting a message be centrally logged
 *
 * \param message The recieved messsage
 * \param response Pointer to a location to allocate and store a response
 * \return Response action
 */
static int Hub_Process_log(Comm_Message* message) {
    if(message->count != 4) {
        return -1;
    }

    Hub_Logging_logWithName(message->components[1], atoi(message->components[2]), message->components[3]);
    return 0;
}

/**
 * \brief Process a variable message
 *
 * Process a message setting or getting a variable
 *
 * \param message The received message
 * \param response Pointer to a location to allocate and store a respones
 * \param client Pointer the the client initiating the request
 * \return Response action
 */
static int Hub_Process_var(Hub_Client* client, Comm_Message* message) {
    Comm_Message* response = NULL;
    Hub_Var* var;
    int n;

    if(message->count == 3 && strcmp(message->components[1], "GET") == 0) {
        var = Hub_Var_get(message->components[2]);

        if(var == NULL) {
            Hub_Logging_log(ERROR, Util_format("Get attempted on not-existent variable '%s'", message->components[2]));
            Hub_Client_kick(client, Util_format("Invalid variable access (%s)", message->components[2]));
            return -1;
        } else {
            pthread_rwlock_rdlock(&var->lock);

            response = Comm_Message_new(4);
            response->request_id = message->request_id;
            response->components[0] = MemPool_strdup(response->alloc, "VAR");
            response->components[1] = MemPool_strdup(response->alloc, "VALUE");
            response->components[3] = MemPool_strdup(response->alloc, Util_format("%f", var->value));

            if(var->readonly) {
                response->components[2] = MemPool_strdup(response->alloc, "RO");
            } else {
                response->components[2] = MemPool_strdup(response->alloc, "RW");
            }

            pthread_rwlock_unlock(&var->lock);

            Hub_Net_sendMessage(client, response);
            Comm_Message_destroy(response);

            return 0;
        }
    } else if(message->count == 4 && strcmp(message->components[1], "SET") == 0) {
        n = Hub_Var_setValue(message->components[2], atof(message->components[3]));
        if(n == -1) {
            Hub_Logging_log(ERROR, Util_format("Set attempted on not-existent variable '%s'", message->components[2]));
        } else if(n == -2) {
            Hub_Logging_log(ERROR, Util_format("Set attempted on read-only variable '%s'", message->components[2]));
        } else {
            /* Success! */
            return 0;
        }

        /* Invalid variable access! Banish the beast! */
        Hub_Client_kick(client, Util_format("Invalid variable access (%s)", message->components[2]));

        return -1;
    }

    return -1;
}

/**
 * \brief Process a request
 *
 * Process an incoming message from a client
 *
 * \param message Received message
 * \param response Pointer to a location to allocate and store a possible response
 * \param client Client which the message was received from
 * \return Response action
 */
int Hub_Process_process(Hub_Client* client, Comm_Message* message) {
    if(strcmp(message->components[0], "COMM") == 0) {
        return Hub_Process_comm(client, message);
    } else if(client->state == CONNECTED) {
        if(strcmp(message->components[0], "NOTIFY") == 0) {
            return Hub_Process_notify(client, message);
        } else if(strcmp(message->components[0], "VAR") == 0) {
            return Hub_Process_var(client, message);
        } else if(strcmp(message->components[0], "LOG") == 0) {
            return Hub_Process_log(message);
        }
    }

    return -1;
}

/** \} */
