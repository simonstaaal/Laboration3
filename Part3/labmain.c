/* main.c

   This file written 2024 by Artur Podobas and Pedro Antunes

   For copyright and licensing, see file COPYING */


/* Below functions are external and found in other files. */
extern void print(const char*);
extern void enable_interrupt(void);
extern void print_dec(unsigned int);
extern void display_string(char*);
extern void time2string(char*,int);
extern void tick(int*);
extern void delay(int);
extern int nextprime( int );

/*ADDRESSES*/
#define START_DISPLAY 0x04000050
#define OFSET 0x10
#define LEDS  ((volatile int*) 0x04000000)
#define SWITCHES ((volatile int*) 0x04000010)
#define SECOND_PUSH_BUTTON ((volatile int*) 0x040000d0)
#define TIMER_PERIODL ((volatile int*) 0x04000028 )  /*Delar upp period i msb och lsb, periodl = lsb , periodh = msb*/ 
#define TIMER_PERIODH ((volatile int*) 0x0400002C)
#define TIMER_CONTROL ((volatile int*) 0x04000024)
#define TIMER_STATUS ((volatile int*) 0x04000020)



/*Variables*/
int mytime = 0x5957;
int mytime_hours = 0;
int counter = 0;
int prime = 1234567;

/*Array for correct numbers on display*/
int hex_numbers [] = {
  0xC0, // 0  (1100 0000)
  0xF9, // 1  (1111 1001)
  0xA4, // 2  (1010 0100)
  0xB0, // 3  (1011 0000)
  0x99, // 4  (1001 1001)
  0x92, // 5  (1001 0010)
  0x82, // 6  (1000 0010)
  0xF8, // 7  (1111 1000)
  0x80, // 8  (1000 0000)
  0x98  // 9  (1001 1000)
};


char textstring[] = "text, more text, and even more text!";



/* Add your code here for initializing interrupts. */
void labinit(void){ 
   //period = 3 000 000 - 1 = 2 999 999 = 0x2DC6BF
  	*TIMER_PERIODH = 0x2D; /*msb = 002D*/
  	*TIMER_PERIODL = 0xC6BF; /*lsb */
  	*TIMER_CONTROL = 0b0000111;//(0x0001 << 2) | (0x0001 << 1)| 0x0001 ; /*Bit 2 = START , Bit 1 = CONT , ITO = 1*/
    enable_interrupt();
    

}

/*UPPGIFT 1C*/
void set_leds(int led_mask){
  *LEDS = led_mask;
}

/*UPPGIFT 1E*/
void set_displays(int display_number, int value){ /* display_number =displayen som ska anv . value = siffran på display*/
  volatile int* segment_display = (volatile int*)(START_DISPLAY + OFSET * display_number);
  *segment_display = value;

}

/*UPPGIFT 1F*/
int get_sw(void){
  return (*SWITCHES) & 0x3FF;
}

/*UPPGIFT 1G*/
int get_btn(void){
  return (*SECOND_PUSH_BUTTON) & 0x1;
}



int check_timer_timeout(void){
  unsigned int status = *TIMER_STATUS;
  if(status & 0x1){
    (*TIMER_STATUS) = 0;
    return 1;
  }
  return 0;
}



/*UPPGIFT H*/
void display_time(int mytime){
 
  /*hours*/
  int h10 = mytime_hours/ 10;
  int h1 = mytime_hours % 10;
  /*Minutes*/
  int m10 = (mytime >> 12) & 0xF;
  int m1 = (mytime >> 8 ) & 0xF;
  
  /*Seconds*/
  int s10 = (mytime >> 4) & 0xF;
  int s1 = mytime & 0xF;

  //Conver to Hex and display time
  set_displays(5, hex_numbers[h10]);
  set_displays(4, hex_numbers[h1]);
  set_displays(3, hex_numbers[m10]);
  set_displays(2, hex_numbers[m1]);
  set_displays(1, hex_numbers[s10]);
  set_displays(0, hex_numbers[s1]);

}

/* Below is the function that will be called when an interrupt is triggered. */
void handle_interrupt(unsigned cause){
  if(*TIMER_STATUS & 0x0001){
    *TIMER_STATUS = 0x0000;
    counter++;
    if(counter >= 10){
      counter = 0;
      time2string(textstring, mytime);
      display_time(mytime);
      if((mytime & 0xFFFF) == 0x0000){
        mytime_hours++;
        if(mytime_hours == 24){
          mytime_hours = 0;
        }
      }
      tick(&mytime);
    }
  }
}






/* Your code goes into main as well as any needed functions. */
int main() {
  // Call labinit()
  labinit();
  
 while(1){
  print("Prime:");
  prime = nextprime(prime);
  print_dec(prime);
  print("\n");
 }
  
}


