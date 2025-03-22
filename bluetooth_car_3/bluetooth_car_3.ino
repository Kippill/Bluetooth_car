#include <GyverMotor.h>
#include <SoftwareSerial.h>

#define MOTOR_MAX 255
#define JOY_MAX 40
#define minDuty 0
#define RIGHT_MOTOR_DIRECTION NORMAL
#define LEFT_MOTOR_DIRECTION NORMAL
#define RIGHT_MOTOR_MODE HIGH
#define LEFT_MOTOR_MODE HIGH

#define RIGHT_MOTOR_D 10
#define RIGHT_MOTOR_PWM 11
#define LEFT_MOTOR_D 2
#define LEFT_MOTOR_PWM 3
#define BT_TX 13
#define BT_RX 12

GMotor motorR(DRIVER2WIRE, RIGHT_MOTOR_D, RIGHT_MOTOR_PWM, RIGHT_MOTOR_MODE);
GMotor motorL(DRIVER2WIRE, LEFT_MOTOR_D, LEFT_MOTOR_PWM, LEFT_MOTOR_MODE);

SoftwareSerial BTserial(BT_RX, BT_TX);

boolean doneParsing, startParsing;
int dataX, dataY;
String string_convert;

void setup() {
    #if (LEFT_MOTOR_PWM == 3 || LEFT_MOTOR_PWM == 11 || RIGHT_MOTOR_PWM == 3 || RIGHT_MOTOR_PWM == 11)
        // Настройка таймеров для пинов 3 и 11
        TCCR2B = 0b00000001;
        TCCR2A = 0b00000011;
    #elif (LEFT_MOTOR_PWM == 9 || LEFT_MOTOR_PWM == 10 || RIGHT_MOTOR_PWM == 9 || RIGHT_MOTOR_PWM == 10)
        // Настройка таймеров для пинов 9 и 10
        TCCR1A = 0b00000001;
        TCCR1B = 0b00001001;
    #endif

    BTserial.begin(9600);

    motorR.setMode(AUTO);
    motorL.setMode(AUTO);
    motorR.setMinDuty(minDuty);
    motorL.setMinDuty(minDuty);
    motorR.setDirection(RIGHT_MOTOR_DIRECTION);
    motorL.setDirection(LEFT_MOTOR_DIRECTION);
}

void loop() {
    parsing();
    if (doneParsing) {
        doneParsing = false;

        int joystickX = map(dataX, -JOY_MAX, JOY_MAX, -MOTOR_MAX / 2, MOTOR_MAX / 2);
        int joystickY = map(dataY, -JOY_MAX, JOY_MAX, -MOTOR_MAX, MOTOR_MAX);

        int dutyR = joystickY + joystickX;
        int dutyL = joystickY - joystickX;

        dutyR = constrain(dutyR, -MOTOR_MAX, MOTOR_MAX);
        dutyL = constrain(dutyL, -MOTOR_MAX, MOTOR_MAX);

        motorR.smoothTick(dutyR);
        motorL.smoothTick(dutyL);
    }
}

void parsing() {
    if (BTserial.available() > 0) {
        char incomingChar = BTserial.read();
        if (startParsing) {
            if (incomingChar == ' ') {
                dataX = string_convert.toInt();
                string_convert = "";
            } else if (incomingChar == ';') {
                dataY = string_convert.toInt();
                string_convert = "";
                startParsing = false;
                doneParsing = true;
            } else {
                string_convert += incomingChar;
            }
        }
        if (incomingChar == '$') {
            startParsing = true;
        }
    }
}
