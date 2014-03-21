FadeCandyCinderClient
=====================

A Cinder friendly FadeCandy Client based on Micah's cpp client

FadeCandy is easy to use open source hardware to drive WS2811 LEDs with high-quality color
https://github.com/scanlime/fadecandy

Setup notes:
There are project files for Visual Studio 2012 and XCode. For VS add an environment variable named CINDER_DIR pointing to your root Cinder directly. For XCode update your CINDER_PATH variable in the XCode project and put your project in the typical relative directory to the libs and blocks dirs.

This code uses the Cinder-ASIO block which you can get from:
https://github.com/BanTheRewind/Cinder-Asio

To create an effect create a class inheriting from FCEffect. Each frame update the shader function will be called for each led in your layout. Calculate your led color there. 

The Client will attempt to process the FadeCandy shader at 300fps. I've noticed on a newer macbook pro the app will vsync to 60hz unless the display window is minimized in which case I can get around 200fps. 