#include <stdbool.h>

enum centre_left_right // own type for storing last position of steering wheel
{
    centre,
    left,
    right
};

enum throttle // own type for storing last position of throttle
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
short LED_left_steering = 2;
short LED_right_steering = 4;
short pin_throttle = 9;
short LED_brakes = 5;
short LED_reverse = 10;

bool l_l_on = false; // left lights on
bool r_l_on = false; // rights lights on
bool brk_l_on = false; // brake lights on

short turn_l_interval = 500; // turn signal interval
short brk_l_interval = 250; //brake lights interval
enum centre_left_right c_steer_pos = centre; // current steering wheel position
enum centre_left_right p_steer_pos = centre; // previous steering wheel position
short steer_aux; // auxiliary variable for assigning proper value from INPUT PINs
short throt_aux; // auxiliary variable for assigning proper value from INPUT PINs

enum throttle c_throt_pos; // enumerated variable for storing state of throttle

short s_n_v = 1505; // steering centre value
short s_d_z = 30; // steering dead zone
short t_n_v = 1493; // throttle centre value
short t_d_z = 8; // throttle dead zone, its lower because we want it to be more sensitive
int c_throt_val; // current throttle value
int p_throt_val = t_n_v; // previous throttle value

void turn_LED_switch(int val1, int val2) {
    // sem si potom doplnis pripadne dalsi ledky, ktere budes rozsvecet/zhasinat
    // budes mit predni i zadni blinkry, ze?
    digitalWrite(LED_left_steering, val1);
    digitalWrite(LED_right_steering, val2);}

void setup() {
    pinMode(pin_steering, INPUT); // steering
    pinMode(pin_throttle, INPUT); // throttle

    pinMode(LED_right_steering, OUTPUT); // right turn signal LED
    pinMode(LED_left_steering, OUTPUT); //left turn signal LED

    pinMode(LED_brakes, OUTPUT); // brakes on LED
    pinMode(LED_reverse, OUTPUT); // reverse LED

    Serial.begin(9600);  // This is needed only if you want to send stuff to your PC


        // loading of INPUT + directions logic
    do {
        steer_aux = pulseIn(pin_steering, HIGH);
        throt_aux = pulseIn(pin_throttle, HIGH);
        // Nacte a vypocte ti to spravne neutralove hodnoty plynu a zataceni
        short counter = 0;
        long aux_steering = 0;
        long aux_throttle = 0;
        for (int i = 0; i < 100; i++) {
            counter++;
            aux_steering += pulseIn(pin_steering, HIGH);
            aux_throttle += pulseIn(pin_throttle, HIGH);}
        s_n_v = (aux_steering / counter);
        t_n_v = (aux_throttle / counter);
        p_throt_val = t_n_v;}
    while ((!steer_aux) && (!throt_aux));

}
void loop() {
    // loading of INPUT + directions logic
    steer_aux = pulseIn(pin_steering, HIGH);
    throt_aux = pulseIn(pin_throttle, HIGH);

    if (!steer_aux) {steer_aux = s_n_v;}
    if (steer_aux > (s_n_v + s_d_z)) {c_steer_pos = right;}
    else if (steer_aux < (s_n_v - s_d_z)) {c_steer_pos = left;}
    else {c_steer_pos = centre;}

    if (!throt_aux) {throt_aux = t_n_v;}
    if (throt_aux > (t_n_v + t_d_z)) {c_throt_pos = forwards;}
    else if (throt_aux < (t_n_v - t_d_z)) {c_throt_pos = reverse;}
    else {c_throt_pos = neutral;}
    c_throt_val = throt_aux;
    ///////////////////////////////////////////////////
    (c_throt_pos == reverse) ? digitalWrite(LED_reverse, HIGH) : digitalWrite(LED_reverse, LOW);

    // turn signal logics
    c_millis = millis();
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

    // turn signals execution
    if (c_steer_pos == left) {
        (l_l_on) ? turn_LED_switch(HIGH, LOW) : turn_LED_switch(LOW, LOW);}
    else if (c_steer_pos == right) {
        (r_l_on) ? turn_LED_switch(LOW, HIGH) : turn_LED_switch(LOW, LOW);}
    else {turn_LED_switch(LOW, LOW);}

    // brake lights logics
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

    // brake light exec
    if (brk_l_on) {digitalWrite(LED_brakes, HIGH);}
    else if ((start_brk_millis) && ((c_millis - start_brk_millis) < brk_l_interval)) {
        digitalWrite(LED_brakes, HIGH);}
    else {digitalWrite(LED_brakes, LOW);}
}
