
#include "comman.h" 

enum Proc_State{
    PROC_STATE_UNUSED = 0,
    PROC_STATE_USED,
    PROC_STATE_READY,
    PROC_STATE_RUNING,
    PROC_STATE_WAITING,
    PROC_STATE_ZOMBIE
};

struct Context{
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t sp;
    uint8_t p;
    uint16_t pc; 
};

typedef struct Proc {

    Context ctx; 
    uint8_t page_table[16];
    uint16_t sz;
     
    uint16_t pid;           
    uint8_t  state;         
    uint8_t  uid;           
    uint8_t  gid;           
    uint8_t  ecode;         
    uint8_t  priority;      
    uint8_t  ticks;   

    struct Proc* parent;    
    void* wchan;       
    bool  killed;      

    uint16_t cwd_inode;     
    uint8_t  fd_table[8];          

    // debug 
    char name[16];

} Proc;
