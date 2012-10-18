# test the opencv lib if needed for the webcam compil
with-webcam {
    win32 {
        CONTRIBPATH=$${PWD}/../contrib/
        FILE=$${CONTRIBPATH}/opencv/include/opencv2/opencv.hpp
        exists($${FILE}) {
            message(OpenCV: Found header files)
        } else {
            error(You need to install the OpenCV lib (www.opencv.org) in the contrib sub-dir. contrib/opencv/include/opencv2/opencv.hpp missing)
        }

        OPENCV_PATH=$${CONTRIBPATH}/opencv/build/x86/mingw/bin
        FILE=$${OPENCV_PATH}/libopencv_core242.dll
        exists($${FILE}) {
            message(OpenCV: Found Core lib)
        } else {
            error(You need to install the OpenCV lib (www.opencv.org) in the contrib sub-dir. contrib/opencv/build/x86/mingw/bin/libopencv_core242.dll missing)
        }

        FILE=$${OPENCV_PATH}/libopencv_highgui242.dll
        exists($${FILE}) {
            message(OpenCV: Found HighGUI lib)
        } else {
            error(You need to install the OpenCV lib (www.opencv.org) in the contrib sub-dir. opencv/build/x86/mingw/bin/libopencv_highgui242.dll missing)
        }

        FILE=$${OPENCV_PATH}/libopencv_objdetect242.dll
        exists($${FILE}) {
            message(OpenCV: Found ObjectDetection lib)
        } else {
            error(You need to install the OpenCV lib (www.opencv.org) in the contrib sub-dir. opencv/build/x86/mingw/bin/libopencv_objdetect242.dll missing)
        }

        # Add include path and libs path
        INCLUDEPATH *= $${CONTRIBPATH}/opencv/include/ \
            $${CONTRIBPATH}/build/include
        LIBS *= -L$${OPENCV_PATH}
    }

    # // TODO: add mac opencv configuration
}
