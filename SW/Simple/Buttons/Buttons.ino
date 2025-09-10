/* Buttons test for LaskaKit ESPink-4.2
 * 
 * Board:   LaskaKit ESPink-4.2   https://www.laskakit.cz/laskakit-espink-42-esp32-e-paper-pcb-antenna
 *  Only for ESPink-4.2 from version v3.0
 * 
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */
 
 // constants won't change. They're used here to set pin numbers:
const int buttonUP    = 39;
const int buttonPUSH  = 40;
const int buttonDOWN  = 41;

// variables will change:
int buttonStateUP   = 0;
int buttonStatePUSH = 0;
int buttonStateDOWN = 0;

void setup() {
  Serial.begin(115200);

  // initialize the pushbutton pin as an input:
  pinMode(buttonUP, INPUT);
  pinMode(buttonPUSH, INPUT);
  pinMode(buttonDOWN, INPUT);
}

void loop() {
  // read the state of the pushbutton value:
  buttonStateUP = digitalRead(buttonUP);
  buttonStatePUSH = digitalRead(buttonPUSH);
  buttonStateDOWN = digitalRead(buttonDOWN);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonStateUP == LOW) {
    Serial.println("Button UP pressed");
  } 
  if (buttonStatePUSH == LOW) {
    Serial.println("Button PUSH pressed");
  } 
  if (buttonStateDOWN == LOW) {
    Serial.println("Button DOWN pressed");
  } 
}
