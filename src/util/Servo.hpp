#ifndef __SERVO_HPP
#define __SERVO_HPP

class Servo {
public:

    Servo(int pin, double degrees=180.0);

    ~Servo();

    double maxDegrees() const;

    void move(double degrees);

    void reset();

private:

    int _pin = 0;

    double _max_degrees = 0.0;

    double _degrees = 0.0;

};

#endif // __SERVO_HPP
