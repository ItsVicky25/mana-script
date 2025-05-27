// Comprehensive test suite for Manascript

// 1. Variables and Data Types
var intVar = 10;
var floatVar = 20.5;
var boolVar = true;
var stringVar = "Hello, ";
var anotherStringVar = "Manascript!";

print("--- Test 1: Variables and Types ---");
print(intVar);
print(floatVar);
print(boolVar);
print(stringVar + anotherStringVar); // Simple string concatenation test

// 2. Arithmetic Operations
var sum = intVar + 5;
var product = intVar * 2;
var difference = 25 - floatVar;
var quotient = floatVar / 2;

print("\n--- Test 2: Arithmetic Operations ---");
print("Sum (10 + 5): ");
print(sum);
print("Product (10 * 2): ");
print(product);
print("Difference (25 - 20.5): ");
print(difference);
print("Quotient (20.5 / 2): ");
print(quotient);

// 3. Conditional Statements (if/else)
print("\n--- Test 3: Conditional Statements ---");
if (intVar < floatVar) {
    print("10 is less than 20.5");
} else {
    print("10 is not less than 20.5");
}

if (boolVar) {
    print("Boolean variable is true");
}

if (!boolVar) {
    // This should not print
    print("Boolean variable is false (error)");
} else {
    print("Boolean variable is not false");
}

// 4. Loops (while)
print("\n--- Test 4: Loops ---");
var count = 0;
while (count < 5) {
    print("Loop iteration: ");
    print(count);
    count = count + 1;
}

// 5. Functions
function multiply(a, b) {
    return a * b;
}

print("\n--- Test 5: Functions ---");
var result = multiply(7, 8);
print("Result of multiply(7, 8): ");
print(result);

// End of script
print("\n--- Test Suite Complete ---"); 