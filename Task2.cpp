#include <iostream>
#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDManager.h>

static void handleKeyboardEvent(void* context, IOReturn result, void* sender, IOHIDValueRef value) {
    if (result != kIOReturnSuccess) {
        std::cerr << "Помилка події клавіатури: " << result << std::endl;
        return;
    }

    IOHIDElementRef elementRef = IOHIDValueGetElement(value);
    if (!elementRef) {
        std::cerr << "Недійсний елемент події клавіатури." << std::endl;
        return;
    }

    CFStringRef elementNameRef = IOHIDElementGetName(elementRef);
    if (!elementNameRef) {
        std::cerr << "Не вдалося отримати ім'я елементу події клавіатури." << std::endl;
        return;
    }

    CFIndex usageIndex = IOHIDElementGetUsage(elementRef);
    CFIndex valueIndex = IOHIDValueGetIntegerValue(value);

    std::cout << "Подія клавіатури: " << CFStringGetCStringPtr(elementNameRef, kCFStringEncodingUTF8)
              << " (Використання: " << usageIndex << ", Значення: " << valueIndex << ")" << std::endl;

    CFRelease(elementNameRef);
}

int main() {

    IOHIDManagerRef hidManagerRef = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    if (!hidManagerRef) {
        std::cerr << "Не вдалося створити керування HID." << std::endl;
        return EXIT_FAILURE;
    }


    IOReturn hidManagerOpenStatus = IOHIDManagerOpen(hidManagerRef, kIOHIDOptionsTypeNone);
    if (hidManagerOpenStatus != kIOReturnSuccess) {
        std::cerr << "Не вдалося відкрити керування HID. Помилка: " << hidManagerOpenStatus << std::endl;
        CFRelease(hidManagerRef);
        return EXIT_FAILURE;
    }

    IOHIDManagerRegisterInputValueCallback(hidManagerRef, handleKeyboardEvent, nullptr);

    IOHIDManagerScheduleWithRunLoop(hidManagerRef, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);

    CFRunLoopRun();

    IOHIDManagerUnscheduleFromRunLoop(hidManagerRef, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    CFRelease(hidManagerRef);

    return EXIT_SUCCESS;
}