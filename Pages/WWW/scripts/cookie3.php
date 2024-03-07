<?php
// Check if form is submitted
if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST["username"])) {
    // Retrieve username from the form
    $username = $_POST["username"];
    
    // Set a cookie named "username" with the submitted username
    setcookie("username", $username, time() + (86400 * 30), "/"); // Cookie will expire in 30 days
    
    // Redirect to the same page to display the username stored in the cookie
    header("Location: " . $_SERVER['PHP_SELF']);
    exit;
}

// Check if the username cookie is set
if(isset($_COOKIE["username"])) {
    $username = $_COOKIE["username"];
    echo "<p>Welcome back, $username!</p>";
} else {
    echo "<p>No username stored in the cookie.</p>";
}

// Check if form is submitted and display submitted username
if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST["username"])) {
    // Retrieve username from the form
    $username = $_POST["username"];
    echo "<p>Username submitted through POST method: $username</p>";
}
?>

<!DOCTYPE html>
<html>
<head>
    <title>Cookie Test with POST Method</title>
</head>
<body>
    <h2>Cookie Test with POST Method</h2>
    
    <!-- Form to submit username -->
    <form method="post" action="<?php echo $_SERVER['PHP_SELF']; ?>">
        <label for="username">Enter your username:</label>
        <input type="text" name="username" id="username">
        <input type="submit" value="Submit">
    </form>
</body>
</html>
