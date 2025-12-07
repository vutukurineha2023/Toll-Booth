#include <windows.h>
#include <string>
#include <sstream>

using namespace std;

// Queue definitions
#define MAX 5

string vehicleNum[MAX];
string vehicleType[MAX];
int frontIndex = -1, rearIndex = -1;

int revenue = 0, processed = 0;

// GUI controls
HWND hVehicleInput, hTypeCombo, hQueueList, hRevenueLabel, hProcessedLabel;

// Queue helper functions
bool isFull() { return rearIndex == MAX - 1; }
bool isEmpty() { return (frontIndex == -1 || frontIndex > rearIndex); }

int getToll(const string& type) {
    if (type == "Car") return 100;
    if (type == "Truck") return 200;
    if (type == "Bike") return 50;
    return 150; // Bus
}

void UpdateDisplay() {
    SendMessage(hQueueList, LB_RESETCONTENT, 0, 0);

    if (!isEmpty()) {
        for (int i = frontIndex; i <= rearIndex; i++) {
            string item = to_string(i - frontIndex + 1) + ". " + vehicleNum[i] + " (" + vehicleType[i] + ")";
            SendMessage(hQueueList, LB_ADDSTRING, 0, (LPARAM)item.c_str());
        }
    }

    // Update Revenue Label
    string r = "Revenue: ₹" + to_string(revenue);
    SetWindowText(hRevenueLabel, r.c_str());

    // Update Processed Label
    string p = "Processed: " + to_string(processed);
    SetWindowText(hProcessedLabel, p.c_str());
}

// Button handlers
void AddVehicle() {
    char num[50];
    GetWindowText(hVehicleInput, num, 50);
    string vehicle = num;

    if (vehicle.empty()) {
        MessageBox(NULL, "Enter vehicle number!", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    if (isFull()) {
        MessageBox(NULL, "Queue full! Vehicle cannot enter.", "Full", MB_OK | MB_ICONERROR);
        return;
    }

    // Get type
    int index = SendMessage(hTypeCombo, CB_GETCURSEL, 0, 0);
    char type[50];
    SendMessage(hTypeCombo, CB_GETLBTEXT, index, (LPARAM)type);

    if (frontIndex == -1) frontIndex = 0;
    rearIndex++;

    vehicleNum[rearIndex] = vehicle;
    vehicleType[rearIndex] = type;

    MessageBox(NULL, "Vehicle Added!", "Success", MB_OK);
    SetWindowText(hVehicleInput, "");

    UpdateDisplay();
}

void ProcessVehicle() {
    if (isEmpty()) {
        MessageBox(NULL, "No vehicles to process", "Empty", MB_OK | MB_ICONWARNING);
        return;
    }

    string num = vehicleNum[frontIndex];
    string type = vehicleType[frontIndex];

    int toll = getToll(type);
    revenue += toll;
    processed++;

    string msg = "Processed " + num + ". Paid ₹" + to_string(toll);
    MessageBox(NULL, msg.c_str(), "Toll Processed", MB_OK);

    frontIndex++;

    if (frontIndex > rearIndex) frontIndex = rearIndex = -1;

    UpdateDisplay();
}

// Win32 Callback Function
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {

    case WM_COMMAND:
        if (wp == 1) AddVehicle();       // Add vehicle button
        if (wp == 2) ProcessVehicle();   // Process button
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

// WinMain - Entry Point
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    const char g_szClassName[] = "myWindowClass";

    // Window class structure
    WNDCLASS wc = {0};
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpszClassName = g_szClassName;
    wc.lpfnWndProc = WindowProcedure;

    RegisterClass(&wc);

    // Main Window
    HWND hwnd = CreateWindow(
        g_szClassName, "Toll Booth Queue Simulation (Win32 GUI)",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        200, 100, 600, 500, NULL, NULL, hInst, NULL
    );

    // Vehicle Number Input
    CreateWindow("static", "Vehicle Number:", WS_VISIBLE | WS_CHILD, 30, 30, 150, 20, hwnd, NULL, NULL, NULL);
    hVehicleInput = CreateWindow("edit", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 150, 25, 200, 25, hwnd, NULL, NULL, NULL);

    // Combo Box for Vehicle Type
    CreateWindow("static", "Vehicle Type:", WS_VISIBLE | WS_CHILD, 30, 70, 150, 20, hwnd, NULL, NULL, NULL);
    hTypeCombo = CreateWindow("combobox", "", CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_BORDER,
                              150, 65, 200, 150, hwnd, NULL, NULL, NULL);

    SendMessage(hTypeCombo, CB_ADDSTRING, 0, (LPARAM)"Car");
    SendMessage(hTypeCombo, CB_ADDSTRING, 0, (LPARAM)"Truck");
    SendMessage(hTypeCombo, CB_ADDSTRING, 0, (LPARAM)"Bike");
    SendMessage(hTypeCombo, CB_ADDSTRING, 0, (LPARAM)"Bus");
    SendMessage(hTypeCombo, CB_SETCURSEL, 0, 0);

    // Buttons
    CreateWindow("button", "Add Vehicle", WS_VISIBLE | WS_CHILD,
                 100, 110, 120, 30, hwnd, (HMENU)1, NULL, NULL);

    CreateWindow("button", "Process Vehicle", WS_VISIBLE | WS_CHILD,
                 250, 110, 140, 30, hwnd, (HMENU)2, NULL, NULL);

    // Queue List
    CreateWindow("static", "Queue:", WS_VISIBLE | WS_CHILD, 30, 160, 100, 20, hwnd, NULL, NULL, NULL);
    hQueueList = CreateWindow("listbox", "", WS_VISIBLE | WS_CHILD | WS_BORDER | LBS_STANDARD,
                              30, 190, 520, 150, hwnd, NULL, NULL, NULL);

    // Stats Labels
    hRevenueLabel   = CreateWindow("static", "Revenue: ₹0", WS_VISIBLE | WS_CHILD, 30, 360, 200, 25, hwnd, NULL, NULL, NULL);
    hProcessedLabel = CreateWindow("static", "Processed: 0", WS_VISIBLE | WS_CHILD, 250, 360, 200, 25, hwnd, NULL, NULL, NULL);

    // Message Loop
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
