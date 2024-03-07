<?php
if (isset($_COOKIE['user'])) {
    // If the cookie is set, display its value
    echo "Cookie 'user' is set! Value is: " . $_COOKIE['user'];
} else {
    // If the cookie is not set, display a message indicating that it is not set
	setcookie("user", "ofrikeel", time() + 3600);
    echo "Cookie 'user' is not set!";
}
?>