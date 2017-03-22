#include <stdbool.h>

enum steering_pos // own type for storing last position of steering wheel
{
    centre,
    left,
    right
};

enum throttle_pos // own type for storing last position of throttle
              // I will keep all theese possible values here, because one day
              // You might wan't to use them...
{
    neutral,
    forwards,
    brakes,
    reverse
};

long c_millis; // current millis used for lights timing
long start_trn_millis = 0; // interval start millis used for turn signals timing
long start_brk_millis = 0;
short pin_steering = 8;
short LED_l_steer = 2;
short LED_r_steer = 4;
short pin_throttle = 9;
short LED_brakes = 5;
short LED_reverse = 10;

bool l_l_on = false; // left lights on
bool r_l_on = false; // rights lights on
bool brk_l_on = false; // brake lights on

short turn_l_interval = 500; // turn signal interval
short brk_l_interval = 250; //brake lights interval
enum steering_pos c_steer_pos = centre; // current steering wheel position
enum steering_pos p_steer_pos = centre; // previous steering wheel position
short c_steer_val; // auxiliary variable for assigning proper value from INPUT PINs

enum throttle_pos c_throt_pos; // enumerated variable for storing state of throttle

short s_n_v = 1505; // steering centre value
short s_d_z = 30; // steering dead zone
short t_n_v = 1493; // throttle centre value
short t_d_z = 8; // throttle dead zone, its lower because we want it to be more sensitive
short c_throt_val; // current throttle value
short p_throt_val = t_n_v; // previous throttle value

void turn_LED_switch(int led1, int val1,int led2, int val2) {
    // sem si potom doplnis pripadne dalsi ledky, ktere budes rozsvecet/zhasinat
    // budes mit predni i zadni blinkry, ze?
    digitalWrite(led1, val1);
    digitalWrite(led2, val2);}

void load_input() {
    // waiting for and loading of INPUT
    turn_LED_switch(LED_l_steer, HIGH, LED_r_steer, HIGH); // turn LED on to signals lack of signal
    digitalWrite(LED_brakes, LOW); //turn other LEDs off
    digitalWrite(LED_reverse, LOW);

    short counter = 0;
    long aux_steering = 0;
    long aux_throttle = 0;
    do {
    aux_steering = pulseIn(pin_steering, HIGH);//waiting for signal
    aux_throttle = pulseIn(pin_throttle, HIGH);}
    while ((!aux_steering) || (!aux_throttle));

    aux_steering = 0;
    aux_throttle = 0;
    for (int i = 0; i < 200; i++) {
        counter++;
        aux_steering += pulseIn(pin_steering, HIGH);
        aux_throttle += pulseIn(pin_throttle, HIGH);}
    s_n_v = (aux_steering / counter);// calculating value of n_v from actual data
    t_n_v = (aux_throttle / counter);
    p_throt_val = t_n_v;
    turn_LED_switch(LED_l_steer, LOW, LED_r_steer, LOW); //signals off
}

void setup() {
    pinMode(pin_steering, INPUT); // steering
    pinMode(pin_throttle, INPUT); // throttle

    pinMode(LED_r_steer, OUTPUT); // right turn signal LED
    pinMode(LED_l_steer, OUTPUT); //left turn signal LED

    pinMode(LED_brakes, OUTPUT); // brakes on LED
    pinMode(LED_reverse, OUTPUT); // reverse LED

    Serial.begin(9600);  // This is needed only if you want to send stuff to your PC
    load_input();

}
void loop() {
    // loading of INPUT
    c_steer_val = pulseIn(pin_steering, HIGH);
    c_throt_val = pulseIn(pin_throttle, HIGH);
    c_millis = millis();

    if ((!c_steer_val) || (!c_throt_val)) {load_input();} // If signal is lost, it will load it again
    if (c_throt_val < 1100) {c_steer_val = s_n_v; c_throt_val = t_n_v;} //sometimes, when signal is loosing, Tx send riddiculous data...

    //Logics
    //Steering
    if (c_steer_val > (s_n_v + s_d_z)) {c_steer_pos = right;}
    else if (c_steer_val < (s_n_v - s_d_z)) {c_steer_pos = left;}
    else {c_steer_pos = centre;}

    if (c_throt_val > (t_n_v + t_d_z)) {c_throt_pos = forwards;}
    else if (c_throt_val < (t_n_v - t_d_z)) {c_throt_pos = reverse;}
    else {c_throt_pos = neutral;}

    if (c_steer_pos == centre) {p_steer_pos = c_steer_pos;}
    else if (c_steer_pos == left) {
        if (p_steer_pos != left) {
            l_l_on = true;
            start_trn_millis = millis();
            p_steer_pos = c_steer_pos;}
        else {
            if (c_millis - start_trn_millis > turn_l_interval) {
                (l_l_on) ? l_l_on = false: l_l_on = true;
                p_steer_pos = c_steer_pos;
                start_trn_millis = c_millis;}}}
    else if (c_steer_pos == right) {
        if (p_steer_pos != right) {
            r_l_on = true;
            p_steer_pos = c_steer_pos;
            start_trn_millis = millis();}
        else {
            if (c_millis - start_trn_millis > turn_l_interval) {
                (r_l_on) ? r_l_on = false: r_l_on = true;
                p_steer_pos = c_steer_pos;
                start_trn_millis = c_millis;}}}

    //throttle
    if ((c_throt_val + t_d_z) < p_throt_val) {
        if (c_throt_val > (t_n_v + t_d_z)) {
            brk_l_on = true;
            start_brk_millis = millis();}
        else {brk_l_on = false;}}
    else if ((c_throt_val - t_d_z) > p_throt_val) {
        if (c_throt_val < (t_n_v - t_d_z)) {
            brk_l_on = true;
            start_brk_millis = millis();}}
    else {brk_l_on = false;}
    p_throt_val = c_throt_val;

    //Execution
    //turn signals
    if (c_steer_pos == left) {
        (l_l_on) ? turn_LED_switch(LED_l_steer, HIGH, LED_r_steer, LOW) : turn_LED_switch(LED_l_steer, LOW, LED_r_steer, LOW);}
    else if (c_steer_pos == right) {
        (r_l_on) ? turn_LED_switch(LED_l_steer, LOW, LED_r_steer, HIGH) : turn_LED_switch(LED_l_steer, LOW, LED_r_steer, LOW);}
    else {turn_LED_switch(LED_l_steer, LOW, LED_r_steer, LOW);}

    //reverse
    (c_throt_pos == reverse) ? digitalWrite(LED_reverse, HIGH) : digitalWrite(LED_reverse, LOW);

    //brakes
    if (brk_l_on) {digitalWrite(LED_brakes, HIGH);}
    else if ((start_brk_millis) && ((c_millis - start_brk_millis) < brk_l_interval)) {
        digitalWrite(LED_brakes, HIGH);}
    else {digitalWrite(LED_brakes, LOW);}
}
