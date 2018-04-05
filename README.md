interesting parts:
-frameprovider produces frames
-effect is a frameprovider that inputs other frameproviders
-mixing happens in Direct3D.cpp (haven't had time to make it an effect, but it should be an effect)
-dxc.cpp is where the pipeline get's set up
-need a webcam plugged in i think i just take the first webcam, crashes if none. Not doing a color convert on the webcam yet
-all single threaded, will need to change because webcam getting frames is blocking until new frame comes (basically limited to webcam fps which is non constant)
-shader library for caching compiled shaders
