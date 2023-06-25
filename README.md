# OTP
> This program includes five small programs that encrypt and decrypt information using a one-time pad-like system.


## Table of Contents
* [General Info](#general-information)
* [Technologies Used](#technologies-used)
* [Features](#features)
* [Screenshots](#screenshots)
* [Setup](#setup)
* [Usage](#usage)
* [Project Status](#project-status)
* [Room for Improvement](#room-for-improvement)
* [Acknowledgements](#acknowledgements)
* [Contact](#contact)
<!-- * [License](#license) -->


## General Information
- Five small programs that encrypt and decrypt information using a one-time pad-like system. These programs will combine multi-processing code with socket-based inter-process communication. This program will also be accessible from the command line using standard Unix features like input/output redirection, and job control.


## Technologies Used
- Linux/OS shell.


## Features
Program will encrypt and decrypt plaintext into ciphertext, using a key. Of the five small programs in C, Two of these will function as servers, and will be accessed using network sockets. Two will be clients, each one of these will use one of the servers to perform work, and the last program is a standalone utility. The programs will use an API for network IPC (socket, connect, bind, listen, & accept to establish connections; send, recv to send and receive sequences of bytes) for the purposes of encryption and decryption by the appropriate servers. Here are the specifications of the 5 files.
- enc_server: This program is the encryption server and will run in the background as a daemon.
- enc_client: This program connects to enc_server, and asks it to perform a one-time pad style encryption as detailed above. By itself, enc_client doesn’t do the encryption - enc_server does. The syntax of enc_client is as follows:
- dec_server: This program performs exactly like enc_server, in syntax and usage. In this case, however, dec_server will decrypt ciphertext it is given, using the passed-in ciphertext and key. Thus, it returns plaintext again to dec_client.
- dec_client: Similarly, this program will connect to dec_server and will ask it to decrypt ciphertext using a passed-in ciphertext and key, and otherwise performs exactly like enc_client, and must be runnable in the same three ways. dec_client should NOT be able to connect to enc_server, even if it tries to connect on the correct port - you’ll need to have the programs reject each other, as described in enc_client.
- keygen: This program creates a key file of specified length. The characters in the file generated will be any of the 27 allowed characters, generated using the standard Unix randomization methods. Do not create spaces every five characters, as has been historically done. Note that you specifically do not have to do any fancy random number generation: we’re not looking for cryptographically secure random number generation. rand()Links to an external site. is just fine. The last character keygen outputs should be a newline. Any error text must be output to stderr.


## Screenshots
See preview screenshot for grading script results.


## Setup
What are the project requirements/dependencies? Where are they listed? A requirements.txt or a Pipfile.lock file perhaps? Where is it located?


## Usage
Download and extract files to desired location. Open up command line interface and type make and run. Then run the executable that is created, typically by typing ./otp


## Project Status
Project is: _complete_


## Room for Improvement
N/A


## Acknowledgements
Give credit here.
- This project was inspired by OSU cs344.


## Contact
Created by Michael Vuong, https://github.com/mvuo - feel free to contact me!

