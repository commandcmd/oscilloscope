#ifndef OSCLIB_HPP
#define OSCLIB_HPP

#include "portaudio.h"
#include "customTerminalIO.hpp"
#include <cmath>

#define DEFAULT_SAMPLE_RATE (44100)

typedef struct {
    float *left_channel;
    float *right_channel;

    unsigned int buffer_frames;
} paData;
static paData preBufData;

enum osclib_err { //define an enumerator for the errors that can happen during the code
    osc_no_err = 1;

    audio_stream_ill_modif = 100;
}

class oscilloscopeLibrary {
    public:
        osclib_err draw_line(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
        osclib_err draw_point(unsigned int x, unsigned int y, unsigned short duration);

        PaError open_start(unsigned int sample_rate = DEFAULT_SAMPLE_RATE);
        PaError stop_close();

    private:
        bool initialised = false;
        bool buffer_initialised = false;

		unsigned int buffer_size_old = 1;
		unsigned int buffer_current_position = 1;

        osclib_err updateBuffer();

        PaStream *audio_stream;
        static int paCallBack(
            const void *inputBuffer,
            void* outputBuffer,
            unsigned long framesPerBuffer,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags flags,
            void *userData )
        {
            (void) inputBuffer; //Calling the inputbuffer like this so we don't get any unused variable warnings
            float *output = (float*)outputBuffer; //Casting outputBuffer from void to float pointer then storing it in *output
            paData *preBufData = (paData*)userData; //Casting the userData from void to paTestData to store it into the *data pointer

            for(unsigned int i = 0; i < preBufData->buffer_frames; i++){ //Passing all the data contained into the struct to the outputBuffer of portAudio
                *output++ = *(preBufData->left_channel + i);
                *output++ = *(preBufData->right_channel + i);
            }

            return 0; //We need to return an int since this function is defined to be an integer in portAudio
        } //oscilloscopeLibrary::paCallBack

         unsigned int absolute(int input);
}; //oscilloscopeLibrary class

PaError oscilloscopeLibrary::open_start(unsigned int sample_rate){ //Initializes portAudio (if not already), opens a new stream with the requested settings and starts the playback
    if(initialised)return paStreamIsNotStopped; //Prevent the code to run if an audio stream is already initialised and if it is return the error enumeration paStreamIsNotStopped to inform the user

    PaError error_output; //Stores any errors occurred during the execution of the function

	const unsigned int channels = 2; //Since the oscilloscope is gonna be in XY mode, we're only using two channels

    error_output = Pa_Initialize(); //Initializing portAudio storing any errors caused during the process
    if(error_output != paNoError) return error_output; //If any error occured return it and end the function
    //If no error occurred continue executing the program

    error_output = Pa_OpenDefaultStream( //We're opening a default stream to save us the trouble of getting the default audio output device
        &audio_stream, //Audio stream defined in the class
        0, //In this library we're not using any input stream since we're not recording
        channels, //Number of audio channels
        paFloat32, //Floating 32-bit for audio output
        sample_rate, //The playback sample rate, highering it makes the drawing of the image faster but less precise
        preBufData.buffer_frames, //The number of frames which will be contained into the audio output buffer
        oscilloscopeLibrary::paCallBack, //This is the callback function that portAudio will call everytime the audio is needed, we defined it in the library
        &preBufData //All audio data that will be passed to the callback function, this struct gets filled when the draw_line() function is called
    );
    if(error_output != paNoError) return error_output; //Checking for errors during initialization of the audio stream

    error_output = Pa_StartStream( oscilloscopeLibrary::audio_stream ); //Starting audio playback
    if(error_output == paNoError)initialised = true; //If there were no errors then set the boolean "initialised" as true
    return error_output; //Returns any error occured during Pa_StartStream, if there was no error the function will return paNoError
} //oscilloscopeLibrary::open_start

PaError oscilloscopeLibrary::stop_close(){ //Stops the playback of an already playing audio stream
    if(!initialised)return paStreamIsStopped; //Prevent the code to run if no audio stream is playing

    delete preBufData.left_channel; //Delete the buffer since the program ended and we don't need it anymore.
    delete preBufData.right_channel;

	PaError error_output; //Stores any errors occurred during the execution of the function

    error_output = Pa_StopStream( oscilloscopeLibrary::audio_stream ); //Stopping the audio playback of the audio stream defined into the class
    if(error_output != paNoError) return error_output; //If any error occurred during the stopping of the playback return the error

    //If no audio stream was playing close the stream anyway (if no stream was created in the first place then it will just return an error)
    error_output = Pa_CloseStream( oscilloscopeLibrary::audio_stream ); //Closing the audio stream
    if(error_output == paNoError)initialised = false; //If there was no error during the stopping of the stream then set the initialised boean as false
    return error_output; //Returns any error occured during Pa_StartStream, if there was no error the function will return paNoError
} //oscilloscopeLibrary::stop_close

osclib_err oscilloscopeLibrary::updateBuffer(){
    if(initialised)return audio_stream_ill_modif; //Prevent this code to run if an audio stream is playing, the buffer shouldn't be edited and resized during audio stream playback

    if(!buffer_initialised){ //If the buffer hasn't been defined yet define it, tell the program we defined it and end the function.
        terminal::out::println("creating new pointer with size ", preBufData.buffer_frames);

        preBufData.left_channel  = new float[preBufData.buffer_frames];
        preBufData.right_channel = new float[preBufData.buffer_frames];

        buffer_initialised = true;

        return osc_no_err;
    }

    terminal::out::println("executing the rest of the stuff");

    float bufferCopyLCH[preBufData.buffer_frames]; //Creating two temporary arrays to store the content of the buffer that's gonna be deleted
    float bufferCopyRCH[preBufData.buffer_frames]; //These are respectively the copy of the left channel and the copy of the right one

	//Copying into the just-created arrays the contents of the buffer
    for(unsigned int i = 0; i < buffer_size_old;i++){
        bufferCopyLCH[i] = *(preBufData.left_channel + i);
        bufferCopyRCH[i] = *(preBufData.right_channel + i);
    }

	//Delete the buffer pointer arrays
	delete preBufData.left_channel;
	delete preBufData.right_channel;

	//Create two new buffer pointer arrays and make them as big as the preBufData.buffer_frames variable specifies
	preBufData.left_channel = new float[preBufData.buffer_frames];
	preBufData.right_channel = new float[preBufData.buffer_frames];

	//Copy all the data into the new buffer
	for(unsigned int i = 0;i < preBufData.buffer_frames;i++){
		*(preBufData.left_channel + i)  = bufferCopyLCH[i];
		*(preBufData.right_channel + i) = bufferCopyRCH[i];
	}

	//buffer_size_old is the prebuffer's size after the last resizing
	buffer_size_old = preBufData.buffer_frames;

	return osc_no_err; //end the function
} //oscilloscopeLibrary::updateBuffer

unsigned int oscilloscopeLibrary::absolute(int input){ //This function is used only in the draw_line
	return input >= 0 ? input : input *= -1; //Take the input and make it negative if it's not already
} //oscilloscopeLibrary::absolute

//Draws a line on the screen
osclib_err oscilloscopeLibrary::draw_line(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2){
    if(initialised)return audio_stream_ill_modif; //Prevent the buffer to be modified while the audio stream is running

	const bool FALL = false; //Used to define the direction variable
	const bool RISE = true;  //If the direction goes up then the definition is in RISE, otherwise in FALL

	unsigned int distanceToX2 = absolute(x2 - x1); //the distance from the current value to the end value
	unsigned int distanceToY2 = absolute(y2 - y1); //Right now it's set as the distance between the first variable and the second one to calculate how much should the buffer be incremented

	signed int distanceRatio; //the ratio between distanceToX2 and distanceToY2

 	//Using the pythagorian theorem to calculate how long will the buffer need to be in order to draw the line
	preBufData.buffer_frames += sqrt((double)pow((double)distanceToX2, 2) + pow((double)distanceToY2, 2)) + 1;
	updateBuffer(); //Update the buffer to the calculated size

	unsigned long iterator = buffer_current_position; //Iterator for the cycle that writes to the prebuffer
	float iX = x1; //Iterator for the X coord
	float iY = y1; //Iterator for the Y coord

	bool directionX = ((x2 - x1) >= 0); //The direction in which the channels have to go at (RISE = true, FALL = false)
	bool directionY = ((y2 - y1) >= 0);

    terminal::out::println("buffer_frames = ", preBufData.buffer_frames);
    terminal::out::println("buffer_current_position = ", buffer_current_position);
    terminal::out::println("directionX = ", directionX, " - directionY = ", directionY, ENDLINE, ENDLINE);

    *(preBufData.left_channel + iterator)  = x1 * 0.01f - 1.00f; //Setting the first value of the buffer to the initial variables, modified to range from a scale of 0 to 200 to a scale of -1.00 to +1.00
    *(preBufData.right_channel + iterator) = y1 * 0.01f - 1.00f; //Not doing this results in the entire buffer being empty since every value of the buffer is dependant on the last one

    do{
        //Recalculate both distances every loop
		distanceToX2 = absolute((x2 * 0.01f - 1.00f) - iX);
		distanceToY2 = absolute((y2 * 0.01f - 1.00f) - iY);

        //Recalculate the distanceRatio every loop
        //This is the ratio between the distance from the biggest variable to the end and the smallest variable to the end rounded down
		distanceRatio = distanceToX2 > distanceToY2 ? (int)(distanceToX2 / distanceToY2) : (int)(distanceToY2 / distanceToX2);

        terminal::out::println("distanceToX2 = ", distanceToX2, " - distanceToY2 = ", distanceToY2, " - distanceRatio = ", distanceRatio);

		if(distanceToX2 > distanceToY2){
			*(preBufData.left_channel + iterator)  = *(preBufData.left_channel + iterator - 1)  + ((directionX==RISE ? 1.00f : -1.00f) * (distanceRatio / 100.00f));
			*(preBufData.right_channel + iterator) = *(preBufData.right_channel + iterator - 1) + ((directionY==RISE ? 1.00f : -1.00f) * 0.01f);
		} else if(distanceToX2 < distanceToY2){
			*(preBufData.left_channel + iterator)  = *(preBufData.left_channel + iterator - 1)  + ((directionX==RISE ? 1.00f : -1.00f) * 0.01f);
			*(preBufData.right_channel + iterator) = *(preBufData.right_channel + iterator - 1) + ((directionY==RISE ? 1.00f : -1.00f) * (distanceRatio / 100.00f));
		} else if(distanceToX2 == distanceToY2){
			*(preBufData.left_channel + iterator)  = *(preBufData.left_channel + iterator - 1)  + ((directionX==RISE ? 1.00f : -1.00f) * 0.01f);
			*(preBufData.right_channel + iterator) = *(preBufData.right_channel + iterator - 1) + ((directionY==RISE ? 1.00f : -1.00f) * 0.01f);
		}

        terminal::out::println("writing to buffer in position ", iterator, " - left_channel = ", *(preBufData.left_channel + iterator), " - right_channel = ", *(preBufData.right_channel + iterator));

        iX = *(preBufData.left_channel + iterator);
        iY = *(preBufData.right_channel + iterator);

        terminal::out::println("iX = ", iX, " - iY = ", iY);
        terminal::out::println("iterator = ", iterator);
        terminal::out::println(ENDLINE);

		iterator++;
	} while(distanceToX2 != 0 && distanceToY2 != 0);

    buffer_current_position = iterator;

    return osc_no_err;
} //oscilloscopeLibrary::draw_line

//Draws a dot for the screen and keeps the vectorscope on that dot for a certain duration
osclib_err oscilloscopeLibrary::draw_point(unsigned int x, unsigned int y, unsigned short duration){
    if(initialised)return audio_stream_ill_modif; //Prevent the buffer to be modified while the audio stream is running

    preBufData.buffer_frames += duration; //The duration is the amount of time the vectorscope should be staying on the defined coordinates, that defines the brightness of the dot and the speed at which it will be shown during drawing
    oscilloscopeLibrary::updateBuffer();

    *(preBufData.left_channel + buffer_current_position)  = x * 0.01f - 1.00f; //setting the first value of the buffer to the specified x and y coordinate
    *(preBufData.right_channel + buffer_current_position) = y * 0.01f - 1.00f; //Not doing this results in the buffer being empty since every value written depends on the last one

    buffer_current_position++; //Incrementing the position of the buffer by one to not override the just filled in values during the loop

    for(unsigned short i = buffer_current_position;i < duration + buffer_current_position;i++){
        *(preBufData.left_channel + i)  = *(preBufData.left_channel + i - 1);
        *(preBufData.right_channel + i) = *(preBufData.right_channel + i - 1);
    }

    return osc_no_err;
} //oscilloscopeLibrary::draw_point

#endif