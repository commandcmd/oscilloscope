#include "portaudio.h"
#include "../libraries/customTerminalIO.hpp"

#define CHANNELS          (2)
#define SAMPLE_RATE       (160000)

#define FRAMES_PER_BUFFER (800)

#define LEFT  0
#define RIGHT 1

typedef struct {
    float channel[2];
} paTestData;

static int paCallback(
    const void *inputBuffer, //input
    void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags flags,
    void *userData
){
    (void) inputBuffer; //This will not be used but we wanna prevent unused variable warning
    float *output = (float*) outputBuffer; //Casting the outputBuffer from void* to float pointer
    paTestData *data = (paTestData*)userData;
    
    data->channel[RIGHT] = -1.00f;

    for(unsigned int i = 0; i < framesPerBuffer / 4; i++){
        *output++ = data->channel[LEFT]; //Sending the left channel out to the output
        *output++ = data->channel[RIGHT]; //Sending the right channel out to the output
        
        data->channel[LEFT] += 0.01f;
    }
    
    for(unsigned int i = 0; i < framesPerBuffer / 4; i++){
        *output++ = data->channel[LEFT]; //Sending the left channel out to the output
        *output++ = data->channel[RIGHT]; //Sending the right channel out to the output
        
        data->channel[RIGHT] += 0.01f;
    }
    
    for(unsigned int i = 0; i < framesPerBuffer / 4; i++){
        *output++ = data->channel[LEFT]; //Sending the left channel out to the output
        *output++ = data->channel[RIGHT]; //Sending the right channel out to the output

        data->channel[LEFT] -= 0.01f;
    }
    
    for(unsigned int i = 0; i < framesPerBuffer / 4; i++){
        *output++ = data->channel[LEFT]; //Sending the left channel out to the output
        *output++ = data->channel[RIGHT]; //Sending the right channel out to the output
        
        data->channel[RIGHT] -= 0.01f;
    }

    /*
    for(unsigned int i = 0; i < framesPerBuffer; i++){ //This for loop will create the array that will define the function
        *output++ = data->left_ch; //Sending the left channel out to the output
        *output++ = data->right_ch; //Sending the right channel out to the output

        if(data->left_ch_up){
            data->left_ch += 0.01f;
        } else data->left_ch -= 0.01f;

        if(data->right_ch_up){
            data->right_ch += 0.01f;
        } else data->right_ch -= 0.01f;

        if(data->left_ch >= 1.0f || data->left_ch <= -1.0f)
            data->left_ch_up = !data->left_ch_up;

        if(data->right_ch >= 1.0f || data->right_ch <= -1.0f)
            data->right_ch_up = !data->right_ch_up;
    }
    */

    return 0; //We had to create an int because the callback function inside the program is defined as int
}

static paTestData audioData; //Calling our struct and defining an object in it


int main(void){
    PaStream *audioStream; //Creating the audio stream for outputting the audio to the device
    PaError errout; //Creating a variable to store any errors occurred during the audio processing

    //Initializing the variables inside the struct
    audioData.channel[LEFT] = -1.00f;
    audioData.channel[RIGHT] = -1.00f;

    terminal::out::println("Initializing audio Stream...");

    errout = Pa_Initialize(); //Initializing the code storing any error occured during that process into the errout variable
    if (errout != paNoError) goto error; //If any error occured close the program and display the error code to the user.

    terminal::out::println("Audio stream initialized successfully", ENDLINE, "Opening the audio stream and setting the functions");

    errout = Pa_OpenDefaultStream(
        &audioStream, //tell the program function this is the output audio stream
        0, //No input stream hence we're not recording anything
        CHANNELS, //The number of channels of the output
        paFloat32, //32bit floating point output
        SAMPLE_RATE, //The sample rate defined at the beginning
        FRAMES_PER_BUFFER, //Number of frames per every buffer in paTestCallback function (defined at the beginning)
        paCallback, //tell the function this is the callback function that will be used for the callback later
        &audioData //This is the audio data that will be passed to the callback function later
    ); //Pa_OpenDefaultStream()
    if(errout != paNoError) goto error; //Check for errors

    terminal::out::println("Audio Stream opened successfully", ENDLINE, "Starting audio...");

    errout = Pa_StartStream( audioStream ); //Start playing the audio contained in the stream
    if(errout != paNoError)goto error;

    terminal::out::println("Audio Stream started - press any key to stop");

    //Wait till the user presses a key to stop the audio
    terminal::in::get_ch();

    terminal::out::println("Stopping and closing audio Stream...");

    errout = Pa_StopStream( audioStream ); //Stop the audio
    if(errout != paNoError)goto error;

    errout = Pa_CloseStream( audioStream ); //Terminate the audio stream
    if(errout != paNoError)goto error;

    terminal::out::println("Audio stream stopped and closed - terminating program...");

    Pa_Terminate(); //Close portaudio

    terminal::out::println("Program terminated successfully.");
    return 0;

error: //If an error occurred:
    Pa_Terminate(); //Close the program
    terminal::out::print("An error occurred while using the portaudio stream", ENDLINE, "Error number : ", errout, ENDLINE, "Error message : ", Pa_GetErrorText(errout), ENDLINE); //Inform the user of what happened
    return errout; //Return the error code
}