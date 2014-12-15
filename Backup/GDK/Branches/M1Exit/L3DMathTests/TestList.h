// TestList contains declarations for all of the test methods, and includes them in an array of tests to run
// TO ADD A NEW TEST:
//   1. Implement your test with the same signature
//   2. Put a declaration for your test function with the others
//   3. Add your test function to the bottom of the array

#pragma once

// All test functions use this signature
typedef bool (*TestFunction)();

// Function Declarations
bool Vector3Tests();
bool MatrixTests();

// Test List
TestFunction pfnTestList[] = 
{
    Vector3Tests,
    MatrixTests,
};




