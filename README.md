# Cat-Feeder-Code
Arduino code for Cat Feeder

The code supports an I2C LCD, a push button for jogging the motor and changing the number of feedings. 
The feeder supports one to three feeding times. After a power reset the time defaults to midnight and the feeding times to two. 

After a power reset the arduino will wait for 30 seconds for the sync message to set the current time. The sync message is a T####### 
where the ##### is the time you wish to set in Epoch format. Send this message to the arduino over the serial monitor and the arduino 
will start at that time. If the message is not received in 30 seconds the time defaults to midnight. The time is displayed using military
time.
