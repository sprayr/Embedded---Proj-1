#include <xc.h>
#pragma config JTAGEN = OFF
#pragma config FWDTEN = OFF
#pragma config FNOSC = FRCPLL
#pragma config FSOSCEN = OFF
#pragma config POSCMOD = EC
#pragma config OSCIOFNC = ON
#pragma config FPBDIV = DIV_1
#pragma config FPLLIDIV = DIV_2
#pragma config FPLLMUL = MUL_20
#pragma config FPLLODIV = DIV_1

void count();
void shift();
void fan();
void determineSpeed();
void makeSound();

int speed;
int soundSpeed;


void main(){ 
    TRISA &= 0xff00;
    TRISFbits.TRISF3 = 1; // RF3 (SW0) configured as input
    TRISFbits.TRISF5 = 1; // RF5 (SW1) configured as input
    TRISFbits.TRISF4 = 1; // RF4 (SW2) configured as input
    TRISDbits.TRISD15 = 1; // RD15 (SW3) configured as input
    TRISDbits.TRISD14 = 1; // RD14 (SW4) configured as input
    TRISBbits.TRISB11 = 1; // RB11 (SW5) configured as input
    ANSELBbits.ANSB11 = 0; // RB11 (SW5) disabled analog
    TRISBbits.TRISB10 = 1; // RB10 (SW6) configured as input
    ANSELBbits.ANSB10 = 0; // RB10 (SW6) disabled analog
    TRISBbits.TRISB9 = 1; // RB9 (SW7) configured as input
    ANSELBbits.ANSB9 = 0; // RB9 (SW7) disabled analog
    TRISBbits.TRISB14 = 0; // RB14 (Speaker) configured as output
    ANSELBbits.ANSB14 = 0; // RB14 (Speaker) disabled analog

    while(1)
    {
        determineSpeed();
        
        PORTA &= 0xff00; //Set all LEDs to 0
        
        //Counter - SW0
        if(PORTFbits.RF3 && !PORTBbits.RB9) {
            count();
        }

        //Shift - SW1
        if(PORTFbits.RF5 && !PORTBbits.RB9){
            shift();
        }

        //Fan - SW2
        if(PORTFbits.RF4 && !PORTBbits.RB9){
            fan();
        }
        
        if(PORTBbits.RB10 && !PORTBbits.RB9){
            makeSound();
        }
        
        if(PORTBbits.RB9){
            PORTA &= 0xff00;
            return;
        }
    }
}


void count(){
    //while SW0 is on
    while(PORTFbits.RF3 && !PORTBbits.RB9){
        
        //check PAUSE switch - SW05
        while(PORTBbits.RB11);
       
        //check priority ( SW0 < SW1 < SW2)
        if(PORTFbits.RF4){ 
            fan();
            break;
        }else if(PORTFbits.RF5){ 
            shift();
            break;
        }
        
        //check direction - SW03
        if(PORTDbits.RD15){
         PORTA--;
        }else{
         PORTA++;
        }
        
        //check SW04
        determineSpeed();
        
        //delay
        for(int i=0 ; i < speed ; i++);
        
    }
}

void shift(){
    
    //while SW1 is on
    while(PORTFbits.RF5 && !PORTBbits.RB9){
        PORTA &= 0xff00; 
        if(PORTDbits.RD15){
            PORTA |= 0x0080;
        }else{
            PORTA |= 0x0001;
        }
       
        
        for(int i = 0; i < 8; i++){
                
            //check priority ( SW0 < SW1 < SW2)
            if(PORTFbits.RF4){
                fan();
                break;
            }
            
            //check PAUSE switch - SW05
            while(PORTBbits.RB11);
            
            //check SW04
            determineSpeed();
            
            //delay
            for(int j=0 ; j <speed ; j++);
           
            //check direction - SW03
            if(PORTDbits.RD15){
                PORTA = PORTA>>1;
            }else{
                PORTA = PORTA<<1;
            }
     
            //check if SW1 is closed
            if(!PORTFbits.RF5){ 
                PORTA &= 0xff00;
                break;
            }
            
            //check SW07 - exit
            if(PORTBbits.RB9) return;
        }
    }
    
}


void fan(){
    int comb;
    
    //While SW2 is on
    while(PORTFbits.RF4 && !PORTBbits.RB9){
        int right, left;
        
        //check direction - SW03
        if(PORTDbits.RD15){
            right = 0x80;
            left = 0x01;
        }else{
            right = 0x08;
            left = 0x10;
        }
        PORTA &= 0xff00;
       
        
        while((right>0 && !PORTDbits.RD15) || (PORTDbits.RD15 && right >= 0x10)){
            comb = left | right; //next PORTA value
            PORTA = 0xff00 | comb; 
            
            //check SW04
            determineSpeed();
            
            //check PAUSE switch - SW05
            while(PORTBbits.RB11 && !PORTBbits.RB9);
            
            //check SW07 - exit
            if(PORTBbits.RB9) return;
            
            //delay
            for(int j=0 ; j < speed ; j++);
            right = right >> 1;
            left = left << 1;    
        }
    }
    
}


void determineSpeed(){
    if(PORTDbits.RD14){
        speed = 64000;
        soundSpeed = 150000;
    }else{
        speed = 120000;
        soundSpeed = 300000;
    }
}

void makeSound(){
    int i=100,j=100;
    
    //while SW06 is on
    while(PORTBbits.RB10 && !PORTBbits.RB9){
        PORTBbits.RB14 ^=1;
 
        //check PAUSE - SW05
        while(PORTBbits.RB11);
        
        for(;j<100+i; j++);
        i+=100;
          
        if(i>320000){
            
            //check SW04 - speed
            determineSpeed();
            
            for (int k = 0; k< soundSpeed; k++);
            i=100;
            j=10;
        }
        
    }
    
}