/* Project to control through Nextion touch screen the intensity of a LED using PWM */

#include<16f886.h>  
#include<MATH.H>

#fuses NOWDT,NOPROTECT          // To clarified the meaning of this "Configuration´s BITS" 
#fuses NOLVP,INTRC              // go to the datasheet and also on MPLAB following the path 
#fuses MCLR,NOBROWNOUT          // MPLAB-> Window-> Pic memory views->
#fuses NOIESO,NOFCMEN,DEBUG     // configuration bits

#use delay(clock=8000000)       // Inform to our compiler the clock frequency to be used  (8Mhz)
#define ocho_MHz 0b01110001     // Define the value of OSCCON register. Setting the internal clock
                                // as source of system clock, running at 8MHz.
/* Setup UART used to interact with Nextion touch screen*/
#use rs232(STREAM = pc, BAUD = 9600, XMIT = PIN_C6, RCV = PIN_C7)

long duty_cycle[11];

/* Nextion screen: characters to confirm an instruction 
this character must be sended after each instruction */
void send_end(){
    printf("ÿÿÿ");
}

/* Send a confirm instruction to Nextion, which change background screen */
void send_data(int img){
    fprintf(pc,"p0.pic=%i",img);
    send_end();
}

void main(){
    
/* The array "duty_cycle" is created.  
   The array has 11 positions from 0 to 10, each position represent a 
   a 10%. PWM has a 8 bits resolution -> as 10% a value 51 is used*/    
    long duty_cycle[11];
    long x=51;
    for(int i=0;i<=10;i++){
        duty_cycle[i]=x*i;
    }

/* How calculate Timer2 parameters:
   TS(PWM work period) = (value_Timer2 +1)*4*(Prescaler Timer2)*(1/Fosc)
   1ms = (x+1)*4*16*(1/8000000) => x= 124
*/    
/*Timer2 is configured to work with a 1KHz frequency = 1ms period*/  
    // setup_timer_2(prescaler Timer2, value_Timer2, postscaler Timer2)
    setup_timer_2(T2_DIV_BY_16,124,1);
    // Timer2 start to count from 0
    set_timer2(0);
    // Set CCP1 as PWM
    setup_ccp1(CCP_PWM);    
    // PWM start working with the value present in the 5th position on duty_cycle array
    set_pwm1_duty(duty_cycle[5]);
    // Variable used to keep received data from serial communication
    char c;

    while (true){
        // The function Kbhit check if any data has arrived  via serial communication
        if(kbhit(pc)){
        c = fgetc(pc);  // Save in "c" the char readed from the serial buffer
        // Compare the data received (hex representation) and it reacts to them 
        switch (c){   
                case 0x30: 
                    set_pwm1_duty(duty_cycle[0]);
                    send_data(0);
                break;
                case 0x31: 
                    set_pwm1_duty(duty_cycle[1]);
                    send_data(1);
                break;
                case 0x32: 
                    set_pwm1_duty(duty_cycle[2]);
                    send_data(2);
                break;
                case 0x33: 
                    set_pwm1_duty(duty_cycle[3]);
                    send_data(3);
                break;
                case 0x34: 
                    set_pwm1_duty(duty_cycle[4]);
                    send_data(4);
                break;
                case 0x35: 
                    set_pwm1_duty(duty_cycle[5]);
                    send_data(5);
                break;
                case 0x36: 
                    set_pwm1_duty(duty_cycle[6]);
                    send_data(6);
                break;
                case 0x37: 
                    set_pwm1_duty(duty_cycle[7]);
                    send_data(7);
                break;
                case 0x38: 
                    set_pwm1_duty(duty_cycle[8]);
                    send_data(8);
                break;  
                case 0x39: 
                    set_pwm1_duty(duty_cycle[9]);
                    send_data(9);
                break;
                case 0x61: 
                    set_pwm1_duty(duty_cycle[10]);
                    send_data(10);
                break;                    
            }
        }
    } 
}