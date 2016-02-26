#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <env.h>
#include <sys/stat.h>

#include "Server.h"


int workProcess() {
    storage::Server server;
    server.work();
    server.destroy();
    return 0;
}


int monitorProcess() {
    int pid, retval;
    bool need_start = true;
    sigset_t sigset;
    siginfo_t siginfo;
    logging::Logger log;

    sigemptyset(&sigset);
    sigaddset(&sigset, SIGQUIT);        //process stopped by user
    sigaddset(&sigset, SIGINT);         //process stopped in terminal
    sigaddset(&sigset, SIGTERM);        //process end request
    sigaddset(&sigset, SIGCHLD);        //child process status changed

    sigprocmask(SIG_BLOCK, &sigset, NULL);

    // бесконечный цикл работы
    while(true) {

        if (need_start) {
            pid = fork();
        }

        need_start = true;

        if (pid == -1) {
            log.print("[MONITOR] Fork failed \n");
        } else if (not pid) {
            retval = workProcess();
            exit(retval);
        } else {
            sigwaitinfo(&sigset, &siginfo);

            if (siginfo.si_signo == SIGCHLD) {
                wait(&retval);
                need_start = true;
            } else {
                kill(pid, SIGTERM);
                retval = 0;
                break;
            }
        }
    }
    return retval;
}

int main() {
    storage::Server server;
    server.work();
    /*int pid = fork();

    if (pid == -1) {
        logging::Logger log;
        log.print("Error: Start Daemon failed");
        return -1;
    } else if (!pid) {
        umask(0);
        setsid();

        chdir("/");

        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        int status = monitorProcess();

        return status;
    }
    else*/ return 0;
}