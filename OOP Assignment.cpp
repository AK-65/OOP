
#include <iostream>
#include <string>
using namespace std;

// Name: Azka Khalid 
// Section B
// Roll No: F25BSCS071
// OOP Assignment: Smart Room Manager
// This program allows users to manage devices in a smart room. Users can add devices, toggle their ON/OFF status, and adjust brightness levels. 
// The program demonstrates the use of classes, constructors, destructors, and basic OOP principles.

//Make a device class for start in room
class Device {
private:
    string name;        
    bool isOn;          
    int brightness;     

public:
    //default constructor for device
    Device() {
        name = "Unknown";
        isOn = false;
        brightness = 50;
    }

    //Overloaded Constructor : initializes with provided values
    Device(string n, bool status, int bright) {
        name = n;
        isOn = status;
        // Using setter to validate brightness
        setBrightness(bright);
    }

    
    ~Device() {
        cout << "The Device " << name << " is being removed." << endl;
    }

    //Setter Functions 

    void setName(string n) {
        name = n;
    }

    // Validates brightness is within 0-100 range
    void setBrightness(int b) {
        if (b >= 0 && b <= 100) {
            brightness = b;
        }
        else {
            cout << "Invalid brightness. Must be between 0 and 100." << endl;
        }
    }

    void turnOn() {
        isOn = true;
        cout << name << " turned ON." << endl;
    }

    void turnOff() {
        isOn = false;
        cout << name << " turned OFF." << endl;
    }

    // device getter functions 

    string getName() const {
        return name;
    }

    int getBrightness() const {
        return brightness;
    }

    bool getStatus() const {
        return isOn;
    }

    //display function for device
    void showInfo() const {
        string status = isOn ? "ON" : "OFF";
        cout << "Name: " << name
            << ", Status: " << status
            << ", Brightness: " << brightness
            << endl;
    }
};


//room class creation for multiple devices management
class Room {
private:
    string roomName;       
    Device devices[5];    
    int count;             //*important for keeping track of devices created

public:
    //parameterized constructor. Don't need default because not creating room arrays.

    Room(string name) {
        roomName = name;
        count = 0;
    }

    //destructor called when user presses exit or main program ends.
    ~Room() {
        cout << "Room " << roomName << " manager closed." << endl;
    }

    //add device function for creating devices
    void addDevice(Device d) {
        if (count < 5) {
            devices[count] = d;
            count++;
            cout << "Device '" << d.getName() << "' added to " << roomName << "." << endl;
        }
        else {
            cout << "Room is full. Cannot add more than 5 devices." << endl;
        }
    }
   // display function for displaying all devices currently in the room
    void showAllDevices() const {
        if (count == 0) {
            cout << "No devices in " << roomName << " yet." << endl;
            return;
        }
        cout << "\n--- Devices in " << roomName << " ---" << endl;
        for (int i = 0; i < count; i++) {
            cout << (i + 1) << ". ";
            devices[i].showInfo();
        }
        cout << "-----------------------------" << endl;
    }
    //device finding function for finding a specific device
    int findDevice(string name) {
        for (int i = 0; i < count; i++) {
            if (devices[i].getName() == name) {
                return i;
            }
        }
        return -1;
    }

    //function turns device ON if OFF, or OFF if ON
    void toggleDevice(string name) {
        int index = findDevice(name);
        if (index == -1) {
            cout << "Device '" << name << "' not found." << endl;
            return;
        }
        if (devices[index].getStatus()) {
            devices[index].turnOff();
        }
        else {
            devices[index].turnOn();
        }
    }

    //function for updating brightness of a specific device
    void setDeviceBrightness(string name, int brightness) {
        int index = findDevice(name);
        if (index == -1) {
            cout << "Device '" << name << "' not found." << endl;
            return;
        }
        devices[index].setBrightness(brightness);
        cout << "Brightness of '" << name << "' set to " << brightness << "." << endl;
    }
};



int main() {
    string roomName;
    cout << "Enter room name: ";
    getline(cin, roomName);

    Room myRoom(roomName); //first creating a room to store all the devices in

    int choice;

    do {
        cout << "\n=== Smart Room Manager ===" << endl;
        cout << "1. Add a Device" << endl;
        cout << "2. Show All Devices" << endl;
        cout << "3. Turn Device ON/OFF" << endl;
        cout << "4. Change Brightness" << endl;
        cout << "5. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore(); 

        switch (choice) {
        case 1: {
            string name;
            bool isOn;
            int brightness;
            int statusInput;

            cout << "Enter device name: ";
            getline(cin, name);

            cout << "Is the device ON? (1 = Yes, 0 = No): ";
            cin >> statusInput;
            isOn = (statusInput == 1);

            cout << "Enter brightness (should be between 0-100): ";
            cin >> brightness;
            cin.ignore();

            Device newDevice(name, isOn, brightness);
            myRoom.addDevice(newDevice);
            break;
        }

        case 2: {
            myRoom.showAllDevices();
            break;
        }

        case 3: {
            string name;
            cout << "Enter device name to toggle ON/OFF: ";
            getline(cin, name);
            myRoom.toggleDevice(name);
            break;
        }

        case 4: {
            string name;
            int brightness;

            cout << "Enter device name: ";
            getline(cin, name);
            cout << "Enter new brightness (0-100): ";
            cin >> brightness;
            cin.ignore();

            myRoom.setDeviceBrightness(name, brightness);
            break;
        }

        case 5: {
            cout << "Exiting Smart Room Manager." << endl;
            break;
        }

        default: {
            cout << "Invalid choice. Please enter 1-5." << endl;
        }
        }

    } while (choice != 5);

    // Now Destructors for Room and all Device objects will be called automatically here

    return 0;
}
