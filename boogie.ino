#include <Adafruit_GFX.h>    // Core graphics library
#include <SWTFT.h> // Hardware-specific library
#include <TouchScreen.h>

#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

SWTFT tft;

#define BOXSIZE 40
int PENRADIUS =2;
int oldcolor, currentcolor;
int state=0;
int button = 20;
volatile int button_state = LOW;
void send(){
  //button_state =HIGH;
}

void setup(void) {
  Serial.begin(9600);
  //Serial.println(F("Paint!"));
  
  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);

  //흰배경, 검은색 펜
  tft.fillScreen(WHITE);
  currentcolor = BLACK;

  attachInterrupt(3, send, RISING);
  
  pinMode(13, OUTPUT);
}

#define MINPRESSURE 5
#define MAXPRESSURE 1000

void loop()
{
  digitalWrite(13, HIGH);
  // Recently Point was renamed TSPoint in the TouchScreen library
  // If you are using an older version of the library, use the
  // commented definition instead.
  // Point p = ts.getPoint();
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

   //x,y반전
  int tmp;
  tmp = TS_MAXX - p.x + TS_MINX;
  p.x = TS_MAXY - p.y + TS_MINY;
  p.y = tmp;
  
  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  //pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  //pinMode(YM, OUTPUT);
  
  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    if (p.y < (TS_MINY+2)) {
      state=1;
    }

    // scale from 0->1023 to tft.width
    p.x = (map(p.x, TS_MINX, TS_MAXX, tft.width(), 0));
    p.y = tft.height()-(map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
    
    //누린 구역 box테두리 표시하기!!
    if(state ==1){
      tft.fillRect(0, 0, BOXSIZE, BOXSIZE, BLACK);
      tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, RED);
      tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, BLUE);
      tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, CYAN);
      tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, WHITE);
      tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, BLACK);
      tft.drawChar(BOXSIZE*5+8, 0, 'E', WHITE, BLACK, 5);

      if (p.y < BOXSIZE &&p.y>=  10) {
        PENRADIUS =2;
        if (p.x < BOXSIZE) { 
         currentcolor = BLACK; 
         state=0;
        } 
        else if (p.x < BOXSIZE*2) {
         currentcolor = RED;
         state=0;
        }
        else if (p.x < BOXSIZE*3) {
         currentcolor = BLUE;
         state=0;
        }
        else if (p.x < BOXSIZE*4) {
         currentcolor = CYAN;
         state=0;
        }
        else if (p.x < BOXSIZE*5) {
         currentcolor = WHITE;
         PENRADIUS =5;
         state=0;
        }
        else if (p.x < BOXSIZE*6) {
         currentcolor = BLACK;                                                          //FIX
         tft.fillRect(0, 0, tft.width(), tft.height(), WHITE);
         state=0;
        }

        if(state==0){
          tft.fillRect(0, 0, tft.width(), BOXSIZE, WHITE);
        }
      }    
    }
    if (((p.y-PENRADIUS) > 0) && ((p.y+PENRADIUS) < tft.height())) {
      tft.fillCircle(p.x, p.y, PENRADIUS, currentcolor);
    }
  }

  //전송 버튼
  if(button_state == HIGH){
    int y,x;
    Serial.println("High");
    for(y=0; y<320; y++){
      for(x=0; x<240; x++){
        Serial.println(tft.readPixel(x,y));
      }
    }
    button_state = LOW;
  }
}
