# Q-Car
##### Created By: Jerry Belich ([@j3rrytron](https://twitter.com/j3rrytron) - [jerrytron.com](http://jerrytron.com))
##### Video Series: Prototyping for Play
##### Segment: 11. Making Connected Games
##### Link: http://jerrytron.com/pfp

This is the repository for the code and project files required for the Q-Car project as demonstrated in segment 11 of the Prototyping for Play course by Jerry Belich, published by O'Reilly Media.

## Building Firmware for the Particle Photon
1. Navigate to [build.particle.io](https://build.particle.io). Login or create an account.
2. Click **CREATE NEW APP**, add a title like *Q-Car*, and a description if you want.
3. Open **application.cpp** from this repository, select all the content, and paste it into the coding windows, replacing ALL existing content.
4. After pasting in the code, save, and try to build (the circle w/ check marked verify), it should fail.
5. On the left bar, click the **Libraries** icon (looks like a bookmark)
6. Type *elapsedMillis* in the search field.
7. Click on the result **ELAPSEDMILLIS** and then click **INCLUDE IN APP**.
8. Select the title of the app you created for this, and click **ADD TO THIS APP**.
9. Repeat steps 5 - 8 except search and include the library **PhoBot**.
10. After both libraries are installed, click the **Verify** button again; it should now build without error.
11. If you already have your Photon registered and selected, skip to **step #16**
12. Navigate to the **Devices** section (the crosshairs) on the left bar. It will list your registered devices.
13. If you haven't added your Photon, here is a guide to getting your device id: [Finding Your Device ID](https://docs.particle.io/support/troubleshooting/device-management/photon/#how-do-i-find-my-device-id-)
14. If you need to add your device, click **ADD NEW DEVICE** and paste your device id.
15. Hover over the device you want to flash and click the star on it's left.
16. Plug in your Photon if you haven't already. Make sure it is powered and online (breathing cyan).
17. Click the **Flash** button (looks like a lightning bolt), and watch for the LED on your Photon to blink purple briefly. If it does, success!

## Test Drive Your Q-Car
1. Make sure you Q-Car is built, the Photon firmware flashed, and that the Photon is online.
2. Open the HTML file from this repository: [**q-car-control.html**](https://github.com/jerrytron/pfp-q-car/blob/master/q-car-control.html)
3. From [build.particle.io](https://build.particle.io), click the **Devices** button (crosshairs), and click the expanding arrow at the right of the Photon device you are using. Copy and paste the **Device ID** into the corresponding field on the **q-car-control.html**.
4. From [build.particle.io](https://build.particle.io), click the **Settings** button (gear in the bottom left). Copy and paste your **Access Token** into the corresponding field on the **q-car-control.html**.
5. Start queueing commands, and have fun!
