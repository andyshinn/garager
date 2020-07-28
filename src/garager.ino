// This #include statement was automatically added by the Particle IDE.
#include <RelayShield.h>

RelayShield myRelays;
LEDStatus statusLED;

int buttonOpen = A4;
int switchUpper = D0;
int switchLower = D1;
bool switchUpperTrigger = false;
bool switchLowerTrigger = false;

int buttonOpenState = LOW;
int doorRelay = 1;
int triggerDelay = 500;

bool enabled = 1;

enum DoorState : int {
    DOOR_CLOSED = 0,
    DOOR_OPEN = 1,
    DOOR_UNKNOWN = 2,
    DOOR_ERROR = 3
};

DoorState state = DOOR_UNKNOWN;

int getUpper() {
    return digitalRead(switchUpper);
}

int getLower() {
    return digitalRead(switchLower);
}

int getState() {
    return state;
}

void setState(DoorState localState) {
    switch (localState)
    {
        case DOOR_CLOSED:
            state = DOOR_CLOSED;
            statusLED.setPattern(LED_PATTERN_SOLID);
            statusLED.setColor(RGB_COLOR_GREEN); 
            break;

        case DOOR_OPEN:
            state = DOOR_OPEN;
            statusLED.setPattern(LED_PATTERN_SOLID);
            statusLED.setColor(RGB_COLOR_YELLOW);
            break;

        case DOOR_UNKNOWN:
            state = DOOR_UNKNOWN;
            statusLED.setPattern(LED_PATTERN_SOLID);
            statusLED.setColor(RGB_COLOR_RED);
            break;

        case DOOR_ERROR:
            state = DOOR_ERROR;
            statusLED.setColor(RGB_COLOR_RED);
            statusLED.setPattern(LED_PATTERN_BLINK);
            break;
            
        default:
            break;
    }
}

void setup() {
    pinMode(buttonOpen, INPUT_PULLDOWN);
    pinMode(switchUpper, INPUT_PULLUP);
    pinMode(switchLower, INPUT_PULLUP);
    
    Particle.function("doorTrigger", cloudTriggerRelay);
    Particle.variable("switchUpper", getUpper);
    Particle.variable("switchLower", getLower);
    Particle.variable("state", getState);
    
    myRelays.begin();

    statusLED.setSpeed(LED_SPEED_FAST);
    statusLED.setActive(true);
}

void loop() {
    buttonOpenState = digitalRead(buttonOpen);

    if (buttonOpenState == HIGH) {
        Particle.publish("triggerFrom", "buttonOpen", PRIVATE);
        triggerRelay(doorRelay);
    }

    if (digitalRead(switchUpper) == HIGH && digitalRead(switchLower) == LOW)
    {
        setState(DOOR_OPEN);
    }

    if (digitalRead(switchUpper) == LOW && digitalRead(switchLower) == HIGH)
    {
       setState(DOOR_CLOSED);
    }

    if (digitalRead(switchUpper) == LOW && digitalRead(switchLower) == LOW)
    {
       setState(DOOR_ERROR);
    }
    
    if (digitalRead(switchUpper) == HIGH && digitalRead(switchLower) == HIGH)
    {
       setState(DOOR_UNKNOWN);
    }
}

void triggerHandler(const char *event, const char *data) {
  if (data) {
    Particle.publish("triggerFrom", "subscribe", PRIVATE);
    triggerRelay(doorRelay);
  }
}

int cloudTriggerRelay(String relay) {
    int relayNumber = relay.toInt();
    triggerRelay(relayNumber);
    return relayNumber;
}

void triggerRelay(int relayNumber) {
    String message;
    String messageEnabled = "enabled: triggered relay: ";
    String messageDisabled = "disabled: triggered relay: ";
    message = messageEnabled + String(relayNumber);
    
    if (enabled) {
        Particle.publish("relayTrigger", message, PRIVATE);
        statusLED.setColor(RGB_COLOR_BLUE);
        statusLED.setPattern(LED_PATTERN_BLINK);
        myRelays.on(relayNumber);
        delay(triggerDelay);
        myRelays.off(relayNumber);
    } else {
        message = messageDisabled + String(relayNumber);
        Particle.publish("relayTrigger", message, PRIVATE);
    }
}