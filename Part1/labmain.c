/* main.c

   This file written 2024 by Artur Podobas and Pedro Antunes

   For copyright and licensing, see file COPYING */


/* Below functions are external and found in other files. */
extern void print(const char*);
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


/*Variables*/
int mytime = 0x005957;
int mytime_hours = 0;



/*Array for correct numbers on display*/
int hex_numbers [] = {
  0x40, //0
  0x79, //1
  0x24, //2
  0x30, //3
  0x19, //4
  0x12, //5
  0x02, //6
  0x78, //7
  0x00, //8
  0x18  //9
};


char textstring[] = "text, more text, and even more text!";

/* Below is the function that will be called when an interrupt is triggered. */
void handle_interrupt(unsigned cause) 
{}

/* Add your code here for initializing interrupts. */
/*UPPGIFT 2*/
void labinit(void){
  
}

/*UPPGIFT 1C*/
void set_leds(int led_mask){
  *LEDS = led_mask;
}

/*UPPGIFT 1E*/
void set_displays(int display_number, int value){ /* display_number =displayen som ska anv . value = siffran på display*/
  volatile int* segment_display = (volatile int*)(START_DISPLAY + (OFSET * display_number));
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


/* Your code goes into main as well as any needed functions. */
int main() {
  // Call labinit()
  labinit();
  
 for(int i = 0x0; i <= 0xF; i++){
    delay(1000); 
    set_leds(i);
    }
    delay(2000);
    set_leds(0x0);

  while(1){
     
    time2string( textstring, mytime );
    display_string( textstring );
    display_time(mytime);
    if ((mytime & 0xFFFF) == 0x0000) {
      mytime_hours++;
      if (mytime_hours == 24){
        mytime_hours = 0;
      } 
    }
    tick(&mytime);
    delay(1000);
    

    
    if(get_btn() == 1){
      int sw_binary = get_sw(); //Lagrar binära talet som finns i adressen switches.
      int switch_nr_display = sw_binary & 0x3F; //Talen som ska skrivas ut på displayen i binär form
      int switch_9 = (sw_binary & 0x200) >> 9;
      int switch_8 = (sw_binary & 0x100) >> 8;
      int exit_switch_6 = (sw_binary & 0x40) >> 6;
      int tens  = switch_nr_display / 10;
      int secs = switch_nr_display % 10;


      if(switch_9 == 0 && switch_8 == 1){
        mytime = (mytime & 0xFF00) | ((tens << 4) | secs);

      }
      else if(switch_9 == 1 && switch_8 == 0){
        mytime = (mytime & 0x00FF) | (tens << 12) | (secs<< 8);
      }
      else if(switch_9 == 1 && switch_8 == 1){
        mytime_hours = switch_nr_display;
        if(mytime_hours >= 24 ){
          mytime_hours = 0;
        }
      }
      else if(exit_switch_6 == 1){
        break;
      }
        

      
      
    }  
  }

  return 0;
 
}


