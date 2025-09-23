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
int mytime = 0x5957;
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


/* Your code goes into main as well as any needed functions. */
int main() {
  // Call labinit()
  labinit();

  
   for(int i = 0x0; i < 0x10; i++){
    delay(1000); 
    set_leds(i);
   }
   delay(2000);
   set_leds(0x0);
   
   

   set_displays(5, 0x40); /*Skapa ny funktion för omv från decimal till binär/hex*/

  // Enter a forever loop
   // while (1) {
    //set_leds(get_btn()); /*Test för get_btn fungerar*/
    //time2string( textstring, mytime ); // Converts mytime to string
    //display_string( textstring ); //Print out the string 'textstring'
    //delay(2);          // Delays 1 sec (adjust this value)
    //tick( &mytime );     // Ticks the clock once
    

   
  
  //}
  int seconds = 0;
  int minutes = 0;
  int hours = 0;
  int tens = 0;
  int secs = 0;
  
  while(1){
    if(get_btn() == 1){
      volatile int sw_binary = *switches; //Lagrar binära talet som finns i adressen switches.
      int switch_nr_display = sw_binary & 0x3F; //Talen som ska skrivas ut på displayen i binär form
      int switch_9 = (sw_binary & 0x200) >> 9;
      int switch_8 = (sw_binary & 0x100) >> 8;
      tens  = switch_nr_display / 10;
      secs = switch_nr_display % 10;


      if(switch_9 == 0 && switch_8 == 1){
        set_displays(0, hex_numbers[secs]);
        set_displays(1, hex_numbers[tens]);
        seconds++;
        if(seconds == 60){
          seconds = 0;
          minutes++;
        }
      }
      else if(switch_9 == 1 && switch_8 == 0){
        set_displays(2, hex_numbers[secs]);
        set_displays(3, hex_numbers[tens]);
        minutes++;
        if(minutes == 60){
          minutes = 0;
          hours++;
        }
      }
      else if(switch_9 == 1 && switch_8 == 1){
        set_displays(4, hex_numbers[secs]);
        set_displays(5, hex_numbers[tens]);
        if(hours == 24){
          hours = 0;
        }
      }

      
      
    }
    tick( &mytime );
  }
}


