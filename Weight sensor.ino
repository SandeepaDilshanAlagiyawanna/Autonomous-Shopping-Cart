#include <HX711.h>

#define DOUT_PIN 2
#define CLK_PIN 3

HX711 scale;

int item1 = 0;

int* myArray = NULL;  // Declare a pointer to an int (initially pointing to NULL)
int arraySize = 0;    // Keep track of the array size

void appendToArray(int value) {
  // Create a new temporary array with an increased size
  int* tempArray = new int[arraySize + 1];
  
  // Copy the elements from the old array to the new array
  for (int i = 0; i < arraySize; i++) {
    tempArray[i] = myArray[i];
  }
  
  // Append the new value to the end of the new array
  tempArray[arraySize] = value;
  
  // Increase the array size
  arraySize++;
  
  // Delete the old array to free up memory
  delete[] myArray;
  
  // Set the pointer to the new array
  myArray = tempArray;
}

int getOrder() {
  while (true) {
    if (nodeMcuSerial.available()) {
      String receivedData = nodeMcuSerial.readStringUntil('\n');
      receivedData.trim();  // Remove leading/trailing whitespaces
      
      // Split the received string into individual values using comma as the delimiter
      int commaIndex = receivedData.indexOf(',');
      while (commaIndex != -1) {
        String value = receivedData.substring(0, commaIndex);
        value.trim();  // Trim the individual value
        int receivedValue = value.toInt();  // Convert the trimmed value to an integer
        Serial.print("Received value: ");
        Serial.println(receivedValue);
        appendToArray(receivedValue);
        
        receivedData = receivedData.substring(commaIndex + 1);  // Remove the processed value from the remaining data
        commaIndex = receivedData.indexOf(',');  // Find the next comma
      }
      
      // Process the last value (after the last comma or the whole string if there are no commas)
      receivedData.trim();  // Trim the last value
      int receivedValue = receivedData.toInt();  // Convert the trimmed value to an integer
      Serial.print("Received value: ");
      Serial.println(receivedValue);
      appendToArray(receivedValue);
    }
  }
}

void printArray(){
  for (int i = 0; i < arraySize; i++) {
    Serial.print(myArray[i]);
    Serial.print(" ");
  }
}

void readWeight() {
  float weight = scale.get_units();  // Get the weight reading in the appropriate units
  Serial.print("Weight: ");
  Serial.print(weight);
  Serial.println(" g");
  return weight;
}

void compareWeight(preWeight,cuWeight){
  if (currentWeight > previousWeight + 50) {
    Serial.println("Cart recieved the item");
  })
  else{
    Serial.println("Weight didn't recieve the item");
  }
}

void setup(){

  scale.begin(DOUT_PIN, CLK_PIN);
  scale.set_scale(471.6);
  scale.tare();

  for (int i = 0; i < arraySize; i++) {
    myArray[i] = item;

    float previousWeight = 0.0;
    float CurrentWeight = 0.0;

    if (item == 1){
      moveVehicle(item1Distance);
      previousWeight = readWeight();   
      fixPosition(); 
      rotateServoFirstMovement();     
      startMovement();
      CurrentWeight = readWeight();
      compareWeight();
    } 
    else if (item == 2) {  
      previousWeight = readWeight();   
      fixPosition(); 
      rotateServoFirstMovement();     
      startMovement();
      CurrentWeight = readWeight();
      compareWeight(); 
    } 
    else if (item == 3) { 
      previousWeight = readWeight();   
      fixPosition(); 
      rotateServoFirstMovement();     
      startMovement();
      CurrentWeight = readWeight();
      compareWeight(); 
    } 
    else if (item == 4) {
      previousWeight = readWeight();   
      fixPosition(); 
      rotateServoFirstMovement();     
      startMovement();
      CurrentWeight = readWeight();
      compareWeight(); 
    } 
  }
    
}

  





