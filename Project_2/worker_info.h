//This is a struct to help the parent to know significant 
//informations for each Worker anytime.

struct Worker_Info
{
    pid_t pid;  //PID of Worker
    int num_of_directories;  //Num of directories that each Worker has
    char pipe_P2C[100];  //Name of pipe P2C
    char pipe_C2P[100];  //Name of pipe C2P
    char** worker_directories;  //An array of names for each Worker.
};

typedef struct Worker_Info worker_info;
