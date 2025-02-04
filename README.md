<p align="center">
    <img src="https://avatars.githubusercontent.com/u/122044824?s=400&u=a90857a96069dbb669412b1bbca8ef6757610d9c&v=4" align="center" width="30%">
</p>

<h1 align="center">WEBSERV</h1>

<p align="center">
    <em><code>❯ A lightweight, efficient HTTP server written in C++</code></em>
</p>

<p align="center">
    <img src="https://img.shields.io/github/license/aouaziz/webserv?style=default&logo=opensourceinitiative&logoColor=white&color=0080ff" alt="license">
    <img src="https://img.shields.io/github/last-commit/aouaziz/webserv?style=default&logo=git&logoColor=white&color=0080ff" alt="last-commit">
    <img src="https://img.shields.io/github/languages/top/aouaziz/webserv?style=default&color=0080ff" alt="repo-top-language">
    <img src="https://img.shields.io/github/languages/count/aouaziz/webserv?style=default&color=0080ff" alt="repo-language-count">
</p>

---

## 🔗 Table of Contents

- [📍 Overview](#-overview)
- [👾 Features](#-features)
- [📁 Project Structure](#-project-structure)
- [🚀 Getting Started](#-getting-started)
  - [☑️ Prerequisites](#-prerequisites)
  - [⚙️ Installation](#-installation)
  - [🤖 Usage](#-usage)
  - [🧪 Testing](#-testing)
- [📌 Project Roadmap](#-project-roadmap)
- [🔰 Contributing](#-contributing)
- [🎗 License](#-license)
- [🙌 Acknowledgments](#-acknowledgments)

---

## 📍 Overview

**WEBSERV** is a lightweight and efficient HTTP server implemented in C++. It is designed to handle static content as well as dynamic content via CGI. With a focus on performance and modularity, WEBSERV supports essential HTTP methods and customizable configurations, making it a great project for learning server architecture and network programming.

---

## 👾 Features

- **HTTP/1.1 Support:** Handles basic HTTP methods such as GET, POST, and DELETE.
- **CGI Integration:** Executes CGI scripts to generate dynamic content.
- **Customizable Configuration:** Easily modify server settings via configuration files.
- **Error Handling:** Serves custom error pages for common HTTP errors (e.g., 400, 404).
- **Modular Codebase:** Organized project structure with clear separation between server logic, client handling, and configuration parsing.

---

## 📁 Project Structure

```
webserv/
├── Makefile                # Build automation tool
├── Pages                   # HTML pages for the server
│   └── WWW
├── includes                # Header files for class definitions
│   ├── Cgi.hpp
│   ├── Client.hpp
│   ├── Config.hpp
│   ├── HTTPMethod.hpp
│   ├── Linker.hpp
│   ├── Request.hpp
│   ├── Server.hpp
│   ├── Webserv.hpp
│   └── include.hpp
├── php-cgi                 # PHP CGI executable or wrapper
├── srcs                    # Source files
│   ├── Cgi.cpp
│   ├── Client.cpp
│   ├── Config.cpp
│   ├── DELETE.cpp
│   ├── GET.cpp
│   ├── Linker.cpp
│   ├── POST.cpp
│   ├── Request.cpp
│   ├── Response.cpp
│   ├── Server.cpp
│   ├── Webserv.cpp
│   └── main.cpp
└── webServ.conf            # Server configuration file
```

---

## 🚀 Getting Started

### ☑️ Prerequisites

- **C++ Compiler:** (e.g., g++ or clang++)
- **Make:** To build the project
- Basic familiarity with Unix/Linux command line

### ⚙️ Installation

1. **Clone the repository:**
   ```sh
   git clone https://github.com/aouaziz/webserv.git
   ```
2. **Navigate to the project directory:**
   ```sh
   cd webserv
   ```
3. **Build the project:**
   ```sh
   make
   ```
   This will compile the source files and generate the `webserv` executable.

### 🤖 Usage

Run the server using:
```sh
./webserv webServ.conf
```
You can modify the `webServ.conf` file to customize your server settings (port, root directory, error pages, etc.).

### 🧪 Testing

*Currently, there is no dedicated test suite. However, you can perform manual testing by:*
- Sending HTTP requests (using tools like `curl` or Postman) to verify responses.
- Checking the server logs for error messages or unexpected behavior.

---

## 📌 Project Roadmap

- ✅ **Core HTTP Support:** GET, POST, DELETE
- 🔄 **Upcoming Enhancements:**
  - **Enhanced Logging**: Improve logging for debugging and monitoring.
  - **Advanced CGI Handling**: Expand support for different CGI scripts.
  - **Configuration Validator**: Create a tool to validate `webServ.conf`.
  - **Performance Improvements**: Optimize request handling and resource management.

---

## 🔰 Contributing

Contributions are welcome! To get started:

1. **Fork the repository.**
2. **Clone your fork locally.**
   ```sh
   git clone https://github.com/your-username/webserv.git
   ```
3. **Create a new branch.**
   ```sh
   git checkout -b feature/new-feature
   ```
4. **Commit your changes.**
   ```sh
   git commit -m "Add new feature"
   ```
5. **Push to GitHub.**
   ```sh
   git push origin feature/new-feature
   ```
6. **Submit a Pull Request.**

---

## 🙌 Acknowledgments

- **Inspiration & Guidance:** Thanks to all contributors and open-source projects that inspired this work.
- **Resources:** Documentation and tutorials on HTTP server development in C++.

---

