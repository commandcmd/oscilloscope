#include "portaudio.h"

class oscilloscopeLibrary {
    public:
        extern void draw_line(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);

        PaError open_start(unsigned int sample_rate);
        PaError stop_close();

    private:
        unsigned int buffer_frames = 1;

        typedef struct {
            float left_channel[buffer_frames];
            float right_channel[buffer_frames];
        } prebuffer;
        static prebuffer preBuffer;

        PaStream *audio_stream;
        static int paCallBack(
            const void *inputBuffer,
            void* outputBuffer,
            unsigned long framesPerBuffer,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags flags,
            void *userData
        );

        extern unsigned int absolute(int input);
};

PaError oscilloscopeLibrary::open_start(unsigned int sample_rate){ //Initializes portAudio (if not already), opens a new stream with the requested settings and starts the playback
    PaError error_output; //Stores any errors occurred during the execution of the function
    const unsigned int channels = 2; //Since the oscilloscope is gonna be in XY mode, we're only using two channels

    error_output = Pa_Initialize(); //Initializing portAudio storing any errors caused during the process
    if(error_output != paNoError) return error_output; //If any error occured return it and end the function
    //If no error occurred continue executing the program

    if(!initialized){
                        error_output = Pa_Initialize();
                        if(error_output != paNoError) return error_output;
                    }

    error_output = Pa_OpenDefaultStream( //We're opening a default stream to save us the trouble of getting the default audio output device
        &oscilloscopeLibrary::audio_stream, //Audio stream defined in the class
        0, //In this library we're not using any input stream since we're not recording
        channels, //Number of audio channels
        paFloat32, //Floating 32-bit for audio output
        sample_rate, //The playback sample rate, highering it makes the drawing of the image faster but less precise
        frames_per_buffer, //The number of frames which will be contained into the audio output buffer
        oscilloscopeLibrary::paCallBack, //This is the callback function that portAudio will call everytime the audio is needed, we defined it in the library
        &preBuffer //All audio data that will be passed to the callback function, this struct gets filled when the draw_line() function is called
    );
    if(error_output != paNoError) return error_output; //Checking for errors during initialization of the audio stream

    error_output = Pa_StartStream( oscilloscopeLibrary::audio_stream ); //Starting audio playback
    if(error_output == paNoError)oscilloscopeLibrary::playing = true; //If the playback started with no errors set the "playing" boolean into the class as true
    return error_output; //Returns any error occured during Pa_StartStream, if there was no error the function will return paNoError
}

PaError oscilloscopeLibrary::stop_close(){ //Stops the playback of an already playing audio stream
    PaError error_output; //Stores any errors occurred during the execution of the function

    if(oscilloscopeLibrary::playing){ //If an audio stream is playing
        error_output = Pa_StopStream( oscilloscopeLibrary::audio_stream ); //Stopping the audio playback of the audio stream defined into the class
        if(error_output != paNoError) return error_output; //If any error occurred during the stopping of the playback return the error
        oscilloscopeLibrary::playing = false; //Set the "playing" boolean back to false state
    }

    //If no audio stream was playing close the stream anyway (if no stream was created in the first place then it will just return an error)
    error_output = Pa_CloseStream( oscilloscopeLibrary::audio_stream ); //Closing the audio stream
    return error_output; //Returns any error occured during Pa_StartStream, if there was no error the function will return paNoError
}

void oscilloscopeLibrary::absolute(int input){return input >= 0 ? input : input *= -1;}

static int oscilloscopeLibrary::paCallBack(
    const void *inputBuffer,
    void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags flags,
    void *userData
) {
    (void) inputBuffer; //Calling the inputbuffer like this so we don't get any unused variable warnings
    float *output = (float*)outputBuffer; //Casting outputBuffer from void to float pointer then storing it in *output
    paTestData *data = (paTestData*)userData; //Casting the userData from void to paTestData to store it into the *data pointer

    for(unsigned int i = 0; i < oscilloscopeLibrary::frames_per_buffer;i++){ //Passing all the data contained into the struct to the outputBuffer of portAudio
        *output++ = data->left_channel[i];
        *output++ = data->right_channel[i];
    }

    return 0; //We need to return an int since this function is defined to be an integer in portAudio
}

void oscilloscopeLibrary::draw_line(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2){
    const bool FALL = false;
    const bool RISE = true;

    unsigned long iterator = 0;
    unsigned int iX = x1;
    unsigned int iY = y1;

    unsigned int distanceToX2 = absolute(x2 - x1);
    unsigned int distanceToY2 = absolute(y2 - y1);
    signed int distanceRatio;

    bool directionX = (x2 - x1) >= 0;
    bool directionY = (y2 - y1) >= 0;

    while(iX < x2 && iY < y2){
        distanceToX2 = absolute(iX - x1);
        distanceToY2 = absolute(iY - y1);

        distanceRatio = distanceToX2 > distanceToY2 ? (int)(distanceToX2 / distanceToY2) : (int)(distanceToY2 / distanceToX2);

        if(distanceToX2 > distanceToY2){
            data->right_channel[iterator] = data->right_channel[iterator - 1] + (1 / 100);
            data->left_channel[iterator] = data->left_channel[iterator - 1] + (distanceRatio / 100);
        } else if(distanceToX2 < distanceToY2){
            data->left_channel[iterator] = data->left_channel[iterator - 1] + (1 / 100);
            data->right_channel[iterator] = data->right_channel[iterator - 1] + (distanceRatio / 100);
        } else if(distanceToX2 == distanceToY2){
            data->left_channel[iterator] = 
        }

        iterator++;
    }
}
