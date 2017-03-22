/*
Smyslem tady je udelat robustni program, ktery bude ovladat svetelka na Petrove aute,
co nejvice veci presuneme do funkci, at to ame prehledne

TODO:
- blinkry pomoci millis
- brzdy
- zpateckove svetlo
*/

#include <stdbool.h> // not sure, wheter this will work on ArdUno

enum neutral_left_right // own type for storing last position of steering wheel
{
    neutral,
    left,
    right
};

long c_millis; // current millis used for turn signals timing
long start_millis = 0; // interval start millis used for turn signals timing

short imp_steering; // impuls from pin connected to steering
short pin_steering = 8;
short LED_left_steering = 2;
short LED_right_steering = 4;

short LED_neutral_steering = 3;

bool l_l_on = false; // left lights on
bool r_l_on = false; // rights lights on
short steer_l; // probably won't be used at all
short interval = 500;
enum neutral_left_right c_steer_pos = neutral; // current steering wheel position
enum neutral_left_right p_steer_pos = neutral; // previous steering wheel position
short aux; // auxiliary variable for assigning proper value from INPUT PINs

short n_v = 1505; // not sure, wheter won't be necessary to use multiple neutral values...
short s_d_z = 30; // steering dead zone

void turn_LED_switch(int val1, int val2, int val3) {
    digitalWrite(LED_neutral_steering, val1);
    digitalWrite(LED_left_steering, val2);
    digitalWrite(LED_right_steering, val3);
}

void setup() {
    pinMode(pin_steering, INPUT); // steering
    pinMode(LED_right_steering, OUTPUT); // right turn signal LED
    pinMode(LED_left_steering, OUTPUT); //left turn signal LED

    pinMode(LED_neutral_steering, OUTPUT);

    Serial.begin(9600);  // Pour a bowl of Serial
}

void loop() {
    // let's find out where are we steering

    aux = pulseIn(pin_steering, HIGH);
    if (!aux) {aux = n_v;}

    if (aux > (n_v + s_d_z)) {c_steer_pos = right;}
    else if (aux < (n_v - s_d_z)) {c_steer_pos = left;}
    else {c_steer_pos = neutral;}

    ///////////////////////////////////////////////////
    // here I want to decide whats really going on

    if (c_steer_pos == neutral) {p_steer_pos = c_steer_pos;}
    else if (c_steer_pos == left) {
        if (p_steer_pos != left) {
            l_l_on = true;
            p_steer_pos = c_steer_pos;
            start_millis = millis();}
        else {
            c_millis = millis();
            if (c_millis - start_millis > interval) {
                (l_l_on) ? l_l_on = false: l_l_on = true;
                p_steer_pos = c_steer_pos;
                start_millis = c_millis;}}}
    else if (c_steer_pos == right) {
        if (p_steer_pos != right) {
            r_l_on = true;
            p_steer_pos = c_steer_pos;
            start_millis = millis();}
        else {
            c_millis = millis();
            if (c_millis - start_millis > interval) {
                (r_l_on) ? r_l_on = false: r_l_on = true;
                p_steer_pos = c_steer_pos;
                start_millis = c_millis;}}}

    ///////////////////////////////////////////////////
    if (c_steer_pos == neutral) {
        turn_LED_switch(HIGH, LOW, LOW);
        }
    else if (c_steer_pos == left) {
        (l_l_on) ? turn_LED_switch(LOW, HIGH, LOW) : turn_LED_switch(LOW, LOW, LOW);
        }
    else if (c_steer_pos == right) {
        (r_l_on) ? turn_LED_switch(LOW, LOW, HIGH) : turn_LED_switch(LOW, LOW, LOW);
        }
    Serial.print(aux);
    Serial.print(" ");
    Serial.print("c_pos: ");
    Serial.print(c_steer_pos);
    Serial.print(" ");
    Serial.print("l_l_on: ");
    Serial.print(l_l_on);
    Serial.print(" ");
    Serial.print("c milis: ");
    Serial.print(c_millis);\
    Serial.print(" ");
    Serial.print("p milis: ");
    Serial.print(start_millis);
    Serial.print("\n");

}
