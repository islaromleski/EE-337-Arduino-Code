#include <SoftwareSerial.h> // Allows for software serial ports on ports other than RX, TX.
#include <SPI.h>
#include <SD.h> // 10 = CS, 11 = MOSI, 12 = MISO, 13 = CLK
#include <pcmConfig.h>
#include <pcmRF.h>
#include <TMRpcm.h>

// Create the software serial ports.
SoftwareSerial bluetooth(5, 6); // 5 = RX, 6 = TX (TX unused).

// Create the audio player.
TMRpcm audioPlayer;

// Instantiate variables.
String bluetoothData; // String for Bluetooth communication.

void setup() {
  // Open serial communication.
  Serial.begin(9600);

  // Open bluetooth communication on the software serial ports.
  bluetooth.begin(9600);
  Serial.println("Bluetooth ready!");

  // Connect to the SD card.
  Serial.print("Initializing SD card... ");
  if(!SD.begin(4)) {
    Serial.println("failed!");
    while(true);
  }
  else {
    Serial.println("initialized!");
  }

  // Setup the speaker.
  audioPlayer.speakerPin = 9;
  Serial.println("Speaker connected!\n");
}

void loop() {

  bluetoothData = readBluetooth();
  
  if (bluetoothData.length() > 0) {
    Serial.println("Received Value: " + bluetoothData + "\n"); // Shows what the device received.

    int wordCnt = countWords(bluetoothData); // Sees how big the array of sanitized words has to be.
    String parsedData[wordCnt]; // Creates that array.
    parseData(bluetoothData, wordCnt, parsedData); // Fills that array with data (arrays are passed as pointers).
    bluetoothData = ""; // Clears bluetoothData for further usage.

    playRecipe(wordCnt, parsedData);    
  }
}

// TODO: Fix this function.
void playRecipe(int wordCnt, String parsedData[]) {
  int stepCnt = 1;
  int i = 0;

  while(i < wordCnt) {
    // Even when sending the keywords neither of these "if" functions work.
    if(parsedData[i] == "ingredients") {
      // In reality, an array with words up to the first "Step" would be sent to TTS.
      // audioPlayer.play("ingredients.wav"); // TODO: Fix this since TMRpcm doesn't play audio correctly.
    }
    else if(parsedData[i] == "Step") {
      // In reality, an array with words up to the next "Step" would be sent to TTS.
      if(stepCnt == 1) {
        // audioPlayer.play("step_1.wav");
      }
      else if(stepCnt == 2) {
        // audioPlayer.play("step_2.wav");
      }
      else if(stepCnt == 3) {
        // audioPlayer.play("step_3.wav");
      }
      stepCnt++;
    }
  
    waitForInput();
    bluetoothData = readBluetooth();
    
    if(bluetoothData == "Stop") {
      bluetoothData = "";
      audioPlayer.stopPlayback();
      i = wordCnt; // Force the while loop to end.
    }
    else if (bluetoothData == "Next") {
      bluetoothData = "";
      audioPlayer.stopPlayback();
    }
    else if (bluetoothData == "Previous") {
      bluetoothData = "";
      audioPlayer.stopPlayback();
      if(stepCnt != 1) {
        stepCnt--;
      }
      i--; // Counteract moving through the parsed data.  
    }
    i++;
  }
  Serial.println("Ending recipe.");
}

void waitForInput() {
  Serial.println("Waiting for input!");
  while(bluetooth.available() == 0) {} // Waits for data to come over bluetooth.
}

String readBluetooth() {
  String bluetoothData;
  char temp;

  // As long as data is being received, read the data and add it to the string.
  while (bluetooth.available() > 0) {
    delay(10);
    char temp = bluetooth.read();
    bluetoothData += temp;
  }
  return bluetoothData;
}

int countWords(String bluetoothData) {
  // Counts the number of seperations there are in the string.
  int wordCnt = 1;
  int i = 0;
  while (i != bluetoothData.length()) {
    if (isSpace(bluetoothData.charAt(i))) {
      wordCnt++;
    }
    i++;
  }
  return wordCnt;
}

void parseData(String bluetoothData, int wordCnt, String parsedData[]) {
  // TODO: Cleanup once I have actual input and add sanitation.
  
  // Converts the string to a character array for use in strtok.
  char bluetoothArray[bluetoothData.length()];
  bluetoothData.toCharArray(bluetoothArray, bluetoothData.length());
  
  // Use strtok to split the array into tokens using spaces as a delimiter.
  int i = 0;
  char *pch;
  pch = strtok(bluetoothArray, " ,.-:");
  while (pch != NULL) {
    parsedData[i] = pch; // Store those tokens in the parsed array.
    pch = strtok(NULL, " ,.-:");
    i++;
  }
}