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


#define START_DISPLAY 0x04000050
#define OFSET 0x10

#define ZERO = 0x40
#define  ONE = 0b01111001
#define  TWO = 0x24




 int mytime = 0x000000;
 int hours = 0;
volatile int* leds = (volatile int*) 0x04000000;
volatile int* switches = (volatile int*) 0x04000010;


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
void labinit(void)
{}

void set_leds(int led_mask){
  *leds = led_mask;
}

void set_displays(int display_number, int value){ /* display_number =displayen som ska anv . value = siffran på display*/
  volatile int* segment_display = (volatile int*)(START_DISPLAY + OFSET * display_number);
  *segment_display = value;

}

int get_btn(void){
  volatile int* second_push_button = (volatile int*) 0x040000d0;
  return *second_push_button;
}

int hex_to_dec(int hex){
 int tens = (hex >> 4) & 0xF;
 int secs = (hex & 0xF);
 int dec = tens * 10 + secs;
 return dec;
}

void display_time(int mytime){
 
 mytime &= 0xFFFF;
  /*hours*/
  int h10 = hours/ 10;
  int h1 = hours % 10;
  /*Minutes*/
  int m10 = (mytime >> 12) & 0xF;
  int m1 = (mytime >> 8 ) & 0xF;
  
  /*Seconds*/
  int s10 = (mytime >> 4) & 0xF;
  int s1 = mytime & 0xF;

/*
  print(" [debug] ");
  print_dec(m10);
  print(":");
  print_dec(m1);
  print(":");
  print_dec(s10);
  print(":");
  print_dec(s1);
  print("\n");
*/
  

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
 //tick(&mytime );
  
  
  


  while(1){
    /*
    print("[mytime before time2string] ");
    print_dec(mytime);
    print("\n");

    time2string( textstring, mytime );
    
    print("[mytime after time2string] ");
    print_dec(mytime);
    print("\n");
    
    print("[textstring content] ");
    print(textstring);
    print("\n");

    display_time(mytime);
    
    print("[mytime before tick] ");
    print_dec(mytime);
    print("\n");
    
    tick(&mytime );
    
    print("[mytime after tick] ");
    print_dec(mytime);
    print("\n");
    
    delay(1500);

    print("[mytime] ");
    print_dec(mytime);
    print("\n");
    */

    time2string( textstring, mytime );
    display_string( textstring );

    display_time(mytime);

    if ((mytime & 0xFFFF) == 0x0000) {
      hours++;
      if (hours == 24) hours = 0;
    }
    tick(&mytime);
    delay(2000);

   
    


    if(get_btn() == 1){
      volatile int sw_binary = *switches; //Lagrar binära talet som finns i adressen switches.
      int switch_nr_display = sw_binary & 0x3F; //Talen som ska skrivas ut på displayen i binär form
      int switch_9 = (sw_binary & 0x200) >> 9;
      int switch_8 = (sw_binary & 0x100) >> 8;
      int tens  = switch_nr_display / 10;
      int secs = switch_nr_display % 10;


      if(switch_9 == 0 && switch_8 == 1){
        mytime = (mytime & 0xFF00) | (tens << 4) | secs;

      }
      else if(switch_9 == 1 && switch_8 == 0){
        mytime = (mytime & 0x00FF) | (tens << 12) | (secs<< 8);
      }
      else if(switch_9 == 1 && switch_8 == 1){
        hours = tens * 10 + secs;
        if(hours >= 24 ){
          hours = 0;
        }
      }

      
      
    }
    
  }



  ///*********************** LAB 3 UPPGIFT A***************************** */
   /*for(int i = 0x0; i < 0x10; i++){
    delay(1000); 
    set_leds(i);
   }
   delay(2000);
   set_leds(0x0);*/
    ///*********************** LAB 3 UPPGIFT A***************************** */
}


