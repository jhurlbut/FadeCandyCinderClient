FadeCandyCinderClient
=====================

A Cinder friendly FadeCandy Client based on Micah's cpp client

FadeCandy is easy to use open source hardware to drive WS2811 LEDs with high-quality color
https://github.com/scanlime/fadecandy

Setup notes:
Currently there is only project files for Visual Studio 2012. Add an environment variable named CINDER_DIR pointing to your root Cinder directly.

This code uses the Cinder-ASIO block which you can get from:
https://github.com/BanTheRewind/Cinder-Asio

To create an effect create a class inheriting from FCEffect. Each frame update the shader function will be called for each led in your layout. Calculate your led color there. 

Currently the FadeCandy connection will be locked to your display refresh so ignore the fps code for now. 