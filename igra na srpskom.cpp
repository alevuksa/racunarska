#include <wiringPi.h>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>

// Define constants for LED colors
const std::vector<std::string> LED_COLORS = {"crvena", "zelena", "plava", "zuta"};

// Initialize hardware pins
const int xAxisPin = 2;   // GPIO 2 (Physical pin 3)
const int yAxisPin = 3;   // GPIO 3 (Physical pin 5)
const int buttonPin = 4;  // GPIO 4 (Physical pin 7)

const int ledLeftPin = 14;   // GPIO 14 (Physical pin 8)
const int ledMiddlePin = 15; // GPIO 15 (Physical pin 10)
const int ledRightPin = 18;  // GPIO 18 (Physical pin 12)
const int ledUpPin = 17;     // GPIO 17 (Physical pin 11)
const int ledDownPin = 27;   // GPIO 27 (Physical pin 13)

void lightLed(int ledPin) {
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(200);
}

std::vector<std::string> playRound(const std::vector<int>& ledPins, int numSequences) {
    std::vector<std::string> sequence;

    for (int i = 0; i < numSequences; ++i) {
        std::string color = LED_COLORS[rand() % LED_COLORS.size()];
        sequence.push_back(color);

        for (size_t colorIndex = 0; colorIndex < LED_COLORS.size(); ++colorIndex) {
            if (color == LED_COLORS[colorIndex]) {
                lightLed(ledPins[colorIndex]);
            }
        }
    }

    return sequence;
}

int main() {
    srand(time(NULL));
    wiringPiSetupGpio();

    pinMode(xAxisPin, INPUT);
    pinMode(yAxisPin, INPUT);
    pinMode(buttonPin, INPUT);

    pinMode(ledLeftPin, OUTPUT);
    pinMode(ledMiddlePin, OUTPUT);
    pinMode(ledRightPin, OUTPUT);
    pinMode(ledUpPin, OUTPUT);
    pinMode(ledDownPin, OUTPUT);

    int level = 1;
    
    while (true) {
        std::cout << "Simon Says Game - Level " << level << std::endl;
        
        int numSequences = level;
        std::vector<int> ledPins = {ledLeftPin, ledRightPin, ledUpPin, ledDownPin};
        
        std::vector<std::string> roundSequence = playRound(ledPins, numSequences);
        
        std::cout << "Gledaj pazljivo i ponovi sekvencu!" << std::endl;
        
        for (const std::string& color : roundSequence) {
            int colorIndex = std::find(LED_COLORS.begin(), LED_COLORS.end(), color) - LED_COLORS.begin();
            lightLed(ledPins[colorIndex]);
            delay(500);
        }
        
        std::cout << "Tvoj red da igras!" << std::endl;
        
        for (const std::string& color : roundSequence) {
            while (true) {
                int xValue = analogRead(xAxisPin);
                int yValue = analogRead(yAxisPin);
                
                if (xValue <= 600) {
                    digitalWrite(ledLeftPin, HIGH);
                } else if (xValue >= 60000) {
                    digitalWrite(ledRightPin, HIGH);
                } else if (yValue <= 600) {
                    digitalWrite(ledUpPin, HIGH);
                } else if (yValue >= 60000) {
                    digitalWrite(ledDownPin, HIGH);
                } else {
                    digitalWrite(ledLeftPin, LOW);
                    digitalWrite(ledRightPin, LOW);
                    digitalWrite(ledUpPin, LOW);
                    digitalWrite(ledDownPin, LOW);
                }
                
                if (digitalRead(buttonPin) == LOW) {
                    if (color == roundSequence[std::distance(roundSequence.begin(), std::find(roundSequence.begin(), roundSequence.end(), color))]) {
                        int ledPinIndex = std::distance(LED_COLORS.begin(), std::find(LED_COLORS.begin(), LED_COLORS.end(), color));
                        lightLed(ledPins[ledPinIndex]);
                        break;
                    } else {
                        std::cout << "Wrong color! Game over." << std::endl;
                        return 0;  // End the game on a wrong input
                    }
                }
            }
        }
        
        std::cout << "Cestitke! Presli ste nivo." << std::endl;
        // Add feedback for passing (e.g., all LEDs lighting up)
        for (int ledPin : ledPins) {
            lightLed(ledPin);
        }
        
        delay(2000);  // Delay before starting the next level
        level++;
    }

    return 0;
}
