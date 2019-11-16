#ifndef __HC_SR04_HPP
#define __HC_SR04_HPP

class HC_SR04 {
public:

    // default constructor
    HC_SR04() = default;

    /*
     * setup the HC SR04 distance sensor with the trigger and echo pin
     * this function sleeps for 2 seconds while initializing the sensor
     */
    HC_SR04(int trigger, int echo);

    /*
     * get the distance in seconds
     */
    double distance();

private:

    // trigger pin
    int _trigger = 0;

    // echo pin
    int _echo = 0;

};

#endif // __HC_SR04_HPP
