# MPassword
Simple password manager for macOS, created using Qt Creator and C++.

This is my first project in C++ that uses a real GUI, and that isn't a game. Qt was very fun to learn, and once I got the hang of it it made the frontend much easier to design. I learned a lot creating this project, such as GUI development, Qt Framework, data storage, encyrption/security, and most importantly learning how to work on this over an extended period of time, in contrast to something like a homework assignment that would take me a couple days at most. Overall, I am happy with how the program is in it's current state, and feel like I am now more prepared for the next project. 

- The program first loads to a master password screen, in which the user must either enter their saved password, or create a new one. In case of a forgotten password, the program can be wiped in order to set a new one.
![Login Page](https://github.com/marshallhm1/MPassword/blob/main/screenshots/Login%20Page.png)

- The next screen which is seen upon successful password input, is a home page that explains how to use the app.
![Home Page](https://github.com/marshallhm1/MPassword/blob/main/screenshots/Home%20Page.png)

- The next page is the screen in which a Label/Website, Username, and PAssword can be entered to be stored in the program. The passwords file is encrypted with XOR encryption.
![Add New](https://github.com/marshallhm1/MPassword/blob/main/screenshots/Save%20New%20Password.png)

- The next page is a tool to generate a secure password, which can then be copied and used in the program, or elsewhere.
![Generate Password](https://github.com/marshallhm1/MPassword/blob/main/screenshots/Generate%20Password.png)

- The final page is an options menu, in which the program can be reset (wipe master and saved passwords), and an option to choose a new master password.
![Options Page]()
