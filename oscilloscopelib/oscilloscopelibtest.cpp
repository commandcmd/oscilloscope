#include "oscilloscopelib.hpp"
#include "customTerminalIO.hpp"

#define SAMPLE_RATE (160000)

oscilloscopeLibrary oscilloscope;

int main(void){
    terminal::out::println("Starting portaudio...");

    oscilloscope.draw_point(20, 20, 2);

    PaError error;
    error = oscilloscope.open_start(SAMPLE_RATE);
    if(error != paNoError)goto Error;

    terminal::out::println("audio started, press any key to stop.");
    terminal::in::get_ch();

    terminal::out::println("audio stream stopped.");
    error = oscilloscope.stop_close();
    if(error != paNoError)goto Error;

    return 0;

    Error:
    Pa_Terminate();
    terminal::out::println( "An error occurred while using the portaudio stream" );
    terminal::out::println( "Error number: ", error );
    terminal::out::println( "Error message: ", Pa_GetErrorText( error ) );
    return error;
}