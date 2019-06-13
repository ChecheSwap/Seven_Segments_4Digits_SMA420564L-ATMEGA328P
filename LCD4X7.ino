/*
   Powered by: Jesús José Navarrete Baca @ChecheSwap
   LED 7 segments 4 Digits
   22/02/2019


  -PINS SELECTOR 10,11,12,13

  -PINS LCD: 2,3,4,5,6,7,8,9

  -LCD BINARY COMBINATIONS:
                00111111
                00000110
                01011011
                01001111
                01100110
                01101101
                01111101
                01000111
                01111111
                01100111

*/
#define MAX 9
#define MIN 0
#define BTNRST 15
#define BTNA 14
#define BTNB 16
#define BTNC 17
#define LED_a 18
#define LED_b 19

enum segment {S_A = 3, S_B = 2, S_C = 1, S_D = 0};

const unsigned short int _0X = 1000;
const unsigned short int _1X = 100;
const unsigned short int _2X = 10;
const unsigned short int _3X = 1;

struct lcd_Generics {

  unsigned short int  number[10] = {63, 6, 91, 79, 102, 109, 125, 71, 127, 103};
  unsigned short int  position[4] = {0b001110, 0b001101, 0b001011, 0b000111};
  unsigned short int  timerecord[2][4] = {{}, {0, 0, 0, 0}}; // {0}=>{CONST PREFIX} , {1}=>{VARIABLE}

  lcd_Generics() {
    for (int x = 0; x < 4; ++x) {
      this->timerecord[0][x] = 50 * (x + 1);
    }
  }
  bool onTime(unsigned short int seg) {
    if (this->timerecord[0][seg] == this->timerecord[1][seg]) {
      this->cls(seg);
      return false;
    }
    return true;
  }
  void increment(void) {
    for (int x = 0; x <= 3; ++x)
      timerecord[1][x]++;
  }

  void cls(unsigned short int seg) {
    this->timerecord[1][seg] = 0;
  }
  void cls() {
    for (int x = 0; x < 4; ++x) {
      this->timerecord[1][x] = 0;
    }
  }
} mylcd;

struct xtime {
  long long unsigned int before;
  unsigned short int lapse;

  xtime() {
    this->before = 0;
    this->lapse = _0X;
  }
} ttime;

struct xcounter {
  short int current[4] = {0};
  bool reversed;
  bool paused;
  bool synchronized;

  xcounter() {
    this->synchronized = false;
    this->paused = false;
    this->reversed = false;
  }

  void reverse(bool flag) {
    this->reversed = flag;
  }
  void pause(bool flag) {
    this->paused = flag;
  }
  void cls(int x, bool flag) {
    if (flag) {
      this->current[x] = MIN;
    }
    else {
      this->current[x] = MAX;
    }
  }

  void cls(bool x) {
    for (int a = 0; 3 >= a; ++a) {
      if (x) {
        this->current[a] = MIN;
      }
      else {
        this->current[a] = MAX;
      }
    }
  }
} counter;

struct xbtns {
  bool _befa, _befrst, _befb, _befc;

  xbtns() {
    this->_befa = true;
    this->_befrst = true;
    this->_befb = true;
    this->_befc = true;
  }
} btn;

struct xled{
  bool a_reg;
  bool b_reg;

  xled(){
    this->a_reg = true;
    this->b_reg = true;
  }
}led;

void setup() {

  DDRD = B11111111;
  DDRB = B001111;
  PORTB = B000000;

  pinMode(BTNA, INPUT_PULLUP);
  pinMode(BTNRST, INPUT_PULLUP);
  pinMode(BTNB, INPUT_PULLUP);
  pinMode(BTNC, INPUT_PULLUP);
  pinMode(LED_a, OUTPUT);
  pinMode(LED_b, OUTPUT);
}

void loop() {
  _sr0();
  _sr1();
  _sr2();
  _sr3();
}

void _sr0(void) {

  if (!counter.synchronized) {
    _addition();
    counter.synchronized = !counter.synchronized;
  }
  else if (tick(millis(), ttime.lapse, ttime.before) && !counter.paused) {
    _addition();
  }
}

void _sr1(void) {
  refresh();
}
void refresh() {

  mylcd.increment();

  if (!mylcd.onTime(S_D)) {
    spit_number(S_D);
  }

  if (!mylcd.onTime(S_C)) {
    spit_number(S_C);
  }

  if (!mylcd.onTime(S_B)) {
    spit_number(S_B);
  }

  if (!mylcd.onTime(S_A)) {
    spit_number(S_A);
  }
}

void spit_number(int seg) {
  PORTB = mylcd.position[seg];
  PORTD = mylcd.number[counter.current[seg]];
}

void _addition(void) {
  if (!counter.reversed) {
    ++counter.current[S_D];
    if (MAX < counter.current[S_D]) {
      counter.cls(S_D, true);
      ++counter.current[S_C];
      if (MAX < counter.current[S_C]) {
        counter.cls(S_C, true);
        ++counter.current[S_B];
        if (MAX < counter.current[S_B]) {
          counter.cls(S_B, true);
          ++counter.current[S_A];
          if (MAX < counter.current[S_A]) {
            counter.cls(true);
          }
        }
      }
    }
  }
  else {
    --counter.current[S_D];
    
    if (MIN > counter.current[S_D]) {
      counter.cls(S_D, false);
      --counter.current[S_C];
      if (MIN > counter.current[S_C]) {
        counter.cls(S_C, false);
        --counter.current[S_B];
        if (MIN > counter.current[S_B]) {
          counter.cls(S_B, false);
          --counter.current[S_A];
          if (MIN > counter.current[S_A]) {
            counter.cls(false);
          }
        }
      }
    }
  }
  led.b_reg = !led.b_reg;
}

void _sr2() {

  bool tmp_a = digitalRead(BTNA);
  bool tmp_rst = digitalRead(BTNRST);
  bool tmp_b = digitalRead(BTNB);
  bool tmp_c = digitalRead(BTNC);

  if (!(btn._befa) && (tmp_a)) {
    up_speed();
  }

  if (!btn._befrst && tmp_rst) {
    if (!counter.reversed) {
      counter.cls(true);
    }
    else {
      counter.cls(false);
    }
  }

  if (!btn._befb && tmp_b) {
    counter.pause(!counter.paused);
  }

  if (!btn._befc && tmp_c) {
    counter.reverse(!counter.reversed);
    led.a_reg = !led.a_reg;
  }

  btn._befa = tmp_a;
  btn._befrst = tmp_rst;
  btn._befb = tmp_b;
  btn._befc = tmp_c;
}

void up_speed(void) {
  switch (ttime.lapse) {
    case (_0X): {
        ttime.lapse = _1X;
        break;
      }
    case (_1X): {
        ttime.lapse = _2X;
        break;
      }
    case (_2X): {
        ttime.lapse = _3X;
        break;
      }
    case (_3X): {
        ttime.lapse = _0X;
        break;
      }
    default: {
        break;
      }
  }
}

void _sr3(){
  digitalWrite(LED_a,led.b_reg);
  digitalWrite(LED_b,led.a_reg);
}
bool tick(long long unsigned int current, int lapse, long long unsigned int & before) {
  if (lapse <= (current - before)) {
    before = current;
    return true;
  }
  return false;
}

