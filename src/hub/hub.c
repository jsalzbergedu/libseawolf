
/* Seawolf libraries */
#include "seawolf.h"
#include "seawolf_hub.h"

#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>

static void Hub_catchSignal(int sig);
static int _Hub_close(void);
static void Hub_close(void);
static void Hub_usage(char* arg0);

void Hub_exitError(void) {
    Hub_Logging_log(INFO, "Terminating hub due to error condition");
    exit(EXIT_FAILURE);
}

void Hub_exit(void) {
    Hub_close();
    exit(EXIT_SUCCESS);
}

bool Hub_fileExists(const char* file) {
    struct stat s;
    return stat(file, &s) != -1;
}

static void Hub_catchSignal(int sig) {
    /* Caught a "nice" signal. Try to exit gracefully and properly */
    if(sig == SIGTERM || sig == SIGINT) {
        /* Run close in a detached thread to ensure memory for it is freed */
        pthread_detach(Task_background(_Hub_close));
        return;
    }

    /* Hub considers other signals to be scary and respects their authoritah by
       quickly laying down and dying */
    Hub_Logging_log(CRITICAL, "Scary signal caught! Shutting down!");
    Hub_exitError();
}

static int _Hub_close(void) {
    Hub_close();
    return 0;
}

static void Hub_close(void) {
    static pthread_mutex_t hub_close_lock = PTHREAD_MUTEX_INITIALIZER;
    static bool closed = false;

    pthread_mutex_lock(&hub_close_lock);
    if(!closed) {
        Hub_Logging_log(INFO, "Closing");
        Hub_Var_close();
        Hub_Net_close();
        Hub_Logging_close();
        Hub_Config_close();
        
        /* Util is part of the core libseawolf, does not require an _init() call,
           but *does* require a _close() call */
        Util_close();
    }
    closed = true;
    pthread_mutex_unlock(&hub_close_lock);
}

static void Hub_usage(char* arg0) {
    printf("Usage: %s [-h] [-c conf]\n", arg0);
}

int main(int argc, char** argv) {
    int opt;
    char* conf_file = NULL;

    /* Parse arguments list */
    while((opt = getopt(argc, argv, ":hc:")) != -1) {
        switch(opt) {
        case 'h':
            Hub_usage(argv[0]);
            exit(EXIT_SUCCESS);
            break;
        case 'c':
            conf_file = optarg;
            break;
        case ':':
            fprintf(stderr, "Option '%c' requires an argument\n", optopt);
            Hub_usage(argv[0]);
            exit(EXIT_FAILURE);
        default:
            /* Invalid option */
            fprintf(stderr, "Invalid option '%c'\n", optopt);
            Hub_usage(argv[0]);
            exit(EXIT_FAILURE);
            break;
        }
    }
    
    /* Please *ignore* SIGPIPE. It will cause the program to close in the case
       of writing to a closed socket. We handle this ourselves. */
    signal(SIGPIPE, SIG_IGN);

    /* Catch common siginals and insure proper shutdown */
    signal(SIGINT, Hub_catchSignal);
    signal(SIGHUP, Hub_catchSignal);
    signal(SIGTERM, Hub_catchSignal);

    /* Use argument as configuration file */
    if(conf_file) {
        Hub_Config_loadConfig(conf_file);
    }

    /* Process configuration file */
    Hub_Config_init();
    Hub_Var_init();
    Hub_Logging_init();

    /* Ensure shutdown during normal exit */
    atexit(Hub_close);
    
    /* Run the main network loop */
    Hub_Net_mainLoop();

    /* Shutdown or wait for shutdown to complete */
    Hub_close();

    return 0;
}
