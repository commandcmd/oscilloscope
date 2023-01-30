#include "oscilloscopelib.hpp"
#include "customTerminalIO.hpp"

#define SAMPLE_RATE (160000)

oscilloscopeLibrary oscilloscope;

int main(void){
    terminal::out::println("Starting portaudio...");

    oscilloscope.draw_point(20, 20, 2);

    PaError error = oscilloscope.open_start(SAMPLE_RATE);
    if(error != PaNoError)goto Error;

    terminal::out::println("audio started, press any key to stop.");
    terminal::in::get_ch();

    oscilloscope.stop_close();

    return;

    :Error
    Pa_Terminate();
    fprintf( stderr, "An error occurred while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
}