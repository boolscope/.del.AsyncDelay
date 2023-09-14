#include "AsyncDelay.h"

AsyncDelay printDelay(500);

void setup() {
    Serial.begin(9600);
}

void loop() {
    if (printDelay.isReady()) {
        Serial.print("Loop count: ");
        Serial.println(printer.getCount());
    }
}