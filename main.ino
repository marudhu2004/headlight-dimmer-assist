// Pin numbers for ultrasonic sensor
#define ECHO_PIN 3
#define TRIG_PIN 2

// Pin for photo resistors
#define AMBIENT_PHOTO_PIN A0
#define FRONT_PHOTO_PIN A1

// Manual override input
#define MANUAL_OVERRIDE_PIN 4
#define OVERRIDE_SWITCH_PIN 5

// Output pin and indicator pin
#define LED_PIN 13
#define WARNING_PIN 6

// Threshold values
#define DIST_THRESHOLD 30
#define FRONT_LIGHT_THRESHOLD 500
#define AMBIENT_LIGHT_THRESHOLD 500
#define RETRY_THRESHOLD 10

// delay value
#define CYCLE_DELAY 100
#define DIST_RETRY_DELAY 30

// Variables to store light intensity values
uint16_t ambient_intensity = 0;
uint16_t front_intensity = 0;

// Variables to store ultrasonic sensor values
long duration = 0;
int distance = 0;
int distry_count = 0;

// Distance validity and light threshold variables
bool valid_dist;
bool light_threshold;

// High beam and manual override variables
bool high_beam;
bool manual_override;

void setup(){

    // starting serial communication if necessary
    #ifdef DEBUG
    Serial.begin(9600);
    #endif

    // Setting ultrasonic pins up
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // Setting up photo resistor pins
    pinMode(AMBIENT_PHOTO_PIN, INPUT);
    pinMode(FRONT_PHOTO_PIN, INPUT);

    // Setting up manual ovrride pins
    pinMode(OVERRIDE_SWITCH_PIN, OUTPUT);
    pinMode(MANUAL_OVERRIDE_PIN, INPUT);

    // Setting up the output and warning pins
    pinMode(LED_PIN, OUTPUT);
    pinMode(WARNING_PIN, OUTPUT);
}


void loop(){

    // Checking if override is on
    manual_override = digitalRead(MANUAL_OVERRIDE_PIN);
    
    // printing debugging info
    #ifdef DEBUG
    Serial.println("================================");
    Serial.print("manual override: ");
    Serial.println(manual_override);
    #endif

    // Checking if manual override is off
    if (!manual_override) {

        // Resetting the distance error count
        distry_count = 0;

        // Getting valid distance 
        do {
            // Warning user if there are consecutive errors
            if (distry_count > RETRY_THRESHOLD){
                digitalWrite(WARNING_PIN, HIGH);
            }
            else {
                digitalWrite(WARNING_PIN, LOW);
            }

            // Setting control to automatic
            digitalWrite(OVERRIDE_SWITCH_PIN, LOW);
                    
            // Triggering the ultrasonic sensor
            digitalWrite(TRIG_PIN, LOW);
            delayMicroseconds(10);
            digitalWrite(TRIG_PIN, HIGH);
            delayMicroseconds(10);
            digitalWrite(TRIG_PIN, LOW);

            // Getting the value and parsing the distance
            duration = pulseIn(ECHO_PIN, HIGH);
            distance = (duration / 29) / 2;

            // Checing validity of the distance measurement
            valid_dist = (distance > 0) && (distance < 400);

            // Updating the try count
            distry_count += 1;

        } while (!valid_dist);
        
        // Getting light intensity from sensors
        ambient_intensity = analogRead(AMBIENT_PHOTO_PIN);
        front_intensity = analogRead(FRONT_PHOTO_PIN);

        // Checking if light intensity is less
        light_threshold = (front_intensity <= FRONT_LIGHT_THRESHOLD) && (ambient_intensity <= AMBIENT_LIGHT_THRESHOLD);
        high_beam = light_threshold && (distance >= DIST_THRESHOLD);

        // Printing debugging data
        #ifdef DEBUG
        Serial.print("Distance: ");
        Serial.println(distance);
        Serial.println();

        Serial.print("Ambient light intensity: ");
        Serial.println(ambient_intensity);
        Serial.print("front light intensity: ");
        Serial.println(front_intensity);
        Serial.println();
        
        Serial.print("Valid distance: ");
        Serial.println(valid_dist);
        Serial.print("light threshold: ");
        Serial.println(light_threshold);
        Serial.println();

        Serial.print("High beam status: ");
        Serial.println(high_beam);
        Serial.print("Warning light status: ");
        Serial.println(distry_count > RETRY_THRESHOLD);
        #endif

        // Updating the headlight state
        if (high_beam) {
            digitalWrite(LED_PIN, HIGH);
        }
        else {
            digitalWrite(LED_PIN, LOW);
        }
    }
 
    else {

        // Sending the default signal to the headlight
        digitalWrite(LED_PIN, LOW);
        digitalWrite(OVERRIDE_SWITCH_PIN, HIGH);
    }

    // Delay per cycle
    delay(CYCLE_DELAY);
}
