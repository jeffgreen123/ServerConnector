# ServerConnector
</br>
This application is running on my local machine creating a socket for a application (Computer, Android and Windows phone all on Github) to connect to in order to take control of the computer.

</br>
</br>
Step 1. Setup all the socket info and start waiting for client to connect (accepts any)
</br>
</br>
Step 2. Wait for the next command of the client
</br>
</br>
Step 3. Upon recieving the command recieved the desired further information and parse it
</br>
</br>
Step 4. If the command was to send a new screenshot, take a screenshot, send the size followed by the compressed file
</br>
</br>
Step 5. return to step 2 and loop until the client closes the connection
