<!DOCTYPE html>
<html>
<head>
    <title>PHP Example</title>
</head>
<body>

<h1>Welcome to My PHP Page!</h1>

<?php
// Display the current date and time using PHP
echo "<p>Current date and time: " . date("Y-m-d H:i:s") . "</p>";

// Calculate and display the square root of a number
$number = 25;
$squareRoot = sqrt($number);
echo "<p>The square root of $number is $squareRoot</p>";

// Define a simple function and call it
function greet($name) {
    echo "<p>Hello, $name! Welcome to the page.</p>";
}

greet("John");

?>

<p>This is a simple PHP example mixed with HTML.</p>

</body>
</html>
