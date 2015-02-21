#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>

#include <iostream>
#include <fstream>

using namespace std;

int fanstate = 0;

int setGpio(char c,unsigned int state)
{
    state&=1;
    ofstream co("/proc/fanio");
    co<<c<<" "<<state<<endl;
    co.close();
    return 0;
}

int getTemp()
{
    ifstream ci("/sys/class/hwmon/hwmon0/device/temp0_input");
    int temp;
    ci>>temp;
    ci.close();
    return temp;
}

int loop()
{
    int temp = getTemp();
    if (temp>60)
    {
        if (!fanstate)
            fanstate = 1;
        setGpio('l',0);
    }
    else
        setGpio('l',1);
    if (temp<40 && fanstate)
        fanstate = 0;
    setGpio('f',fanstate);
}


int main(void) {
			
	/* Our process ID and Session ID */
	pid_t pid, sid;

	/* Fork off the parent process */
	pid = fork();
	if (pid < 0) {
		exit(EXIT_FAILURE);
	}
	/* If we got a good PID, then
	   we can exit the parent process. */
	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	/* Change the file mode mask */
	umask(0);
			
	/* Open any logs here */        
			
	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0) {
		/* Log the failure */
		exit(EXIT_FAILURE);
	}



	/* Change the current working directory */
	if ((chdir("/")) < 0) {
		/* Log the failure */
		exit(EXIT_FAILURE);
	}

	/* Close out the standard file descriptors */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	/* Daemon-specific initialization goes here */

	/* The Big Loop */
	while (1) {
	   /* Do some task here ... */
	   loop();
	   sleep(10); /* wait 10 seconds */
	}
	exit(EXIT_SUCCESS);
}
