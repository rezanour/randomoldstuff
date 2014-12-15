// TestList provides main with the list of tests to run
#pragma once

#ifdef BUILD_TESTS

// borrowed from CoreServicesTests

// TO ADD A NEW TEST:
//   1. Implement your test with the same signature
//   2. Put a declaration for your test function with the others
//   3. Add your test function to the bottom of the array

// All test functions use this signature
typedef bool (*TestFunction)();

// Function Declarations
bool GameApplicationTests();
//bool ScreenManagerTests();
//bool ScreenTests();
//bool SubsystemManagerTests();
bool GameObjectTests();

// Test List
TestFunction pfnTestList[] = 
{
    GameApplicationTests,
    //ScreenManagerTests,
    //ScreenTests,
    //SubsystemManagerTests,
    GameObjectTests,
};

#endif

