
#define KEB_DATA  0xC008
#define KEB_CTRL  0xC009
#define SCR_DATA  0xC00A
#define SCR_CTRL  0xC00B

#define SCR_WRITE 1

volatile unsigned char* const screen_data = (unsigned char*)SCR_DATA;
volatile unsigned char* const screen_ctrl = (unsigned char*)SCR_CTRL;

void main(void){
    // Put something simple for testing
    *screen_data = 'A';  // write 42 to address $2000
    *screen_ctrl = SCR_WRITE;
    while(1);            // loop forever
}