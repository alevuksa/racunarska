#include <wiringPi.h>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>

// Define constants for LED colors
const std::vector<std::string> LED_COLORS = {"red", "green", "blue"};

// Initialize hardware pins
const int xAxisPin = 2;   // GPIO 2 (Physical pin 3)
const int yAxisPin = 3;   // GPIO 3 (Physical pin 5)
const int buttonPin = 4;  // GPIO 4 (Physical pin 7)

const int led1Pin = 14;   // GPIO 14 (Physical pin 8)
const int led2Pin = 15;   // GPIO 15 (Physical pin 10)
const int led3Pin = 18;   // GPIO 18 (Physical pin 12)

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

    pinMode(led1Pin, OUTPUT);
    pinMode(led2Pin, OUTPUT);
    pinMode(led3Pin, OUTPUT);

    int level = 1;
    
    while (true) {
        std::cout << "Simon Says Game - Level " << level << std::endl;
        
        int numSequences = level;
        std::vector<int> ledPins = {led1Pin, led2Pin, led3Pin};
        
        std::vector<std::string> roundSequence = playRound(ledPins, numSequences);
        
        std::cout << "Watch the sequence and repeat it!" << std::endl;
        
        for (const std::string& color : roundSequence) {
            int colorIndex = std::find(LED_COLORS.begin(), LED_COLORS.end(), color) - LED_COLORS.begin();
            lightLed(ledPins[colorIndex]);
            delay(500);
        }
        
        std::cout << "Your turn to repeat the sequence!" << std::endl;
        
        for (const std::string& color : roundSequence) {
            while (true) {
                int xValue = analogRead(xAxisPin);
                int yValue = analogRead(yAxisPin);
                
                if (xValue <= 600) {
                    digitalWrite(led1Pin, HIGH);
                } else if (xValue >= 60000) {
                    digitalWrite(led2Pin, HIGH);
                } else if (yValue <= 600) {
                    digitalWrite(led3Pin, HIGH);
                } else {
                    digitalWrite(led1Pin, LOW);
                    digitalWrite(led2Pin, LOW);
                    digitalWrite(led3Pin, LOW);
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
        
        std::cout << "Congratulations! You passed this level." << std::endl;
        // Add feedback for passing (e.g., all LEDs lighting up)
        for (int ledPin : ledPins) {
            lightLed(ledPin);
        }
        
        delay(2000);  // Delay before starting the next level
        level++;
    }

    return 0;
}
