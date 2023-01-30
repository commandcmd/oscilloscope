#include "portaudio.h"
#include <cmath>

class oscilloscopeLibrary {
    public:
        extern void draw_line(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);

        PaError open_start(unsigned int sample_rate);
        PaError stop_close();

    private:
        unsigned int buffer_frames = 1;
		unsigned int buffer_size_old = 1;

		unsigned int buffer_current_position = 0;

        typedef struct {
            float *left_channel;
            float *right_channel;
        } prebuffer;
        static prebuffer preBuffer;

        extern void updateBuffer();

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
}; //oscilloscopeLibrary class

PaError oscilloscopeLibrary::open_start(unsigned int sample_rate){ //Initializes portAudio (if not already), opens a new stream with the requested settings and starts the playback
    PaError error_output; //Stores any errors occurred during the execution of the function

	const unsigned int channels = 2; //Since the oscilloscope is gonna be in XY mode, we're only using two channels

    error_output = Pa_Initialize(); //Initializing portAudio storing any errors caused during the process
    if(error_output != paNoError) return error_output; //If any error occured return it and end the function
    //If no error occurred continue executing the program

    error_output = Pa_OpenDefaultStream( //We're opening a default stream to save us the trouble of getting the default audio output device
        &oscilloscopeLibrary::audio_stream, //Audio stream defined in the class
        0, //In this library we're not using any input stream since we're not recording
        oscilloscopeLibrary::channels, //Number of audio channels
        paFloat32, //Floating 32-bit for audio output
        sample_rate, //The playback sample rate, highering it makes the drawing of the image faster but less precise
        frames_per_buffer, //The number of frames which will be contained into the audio output buffer
        oscilloscopeLibrary::paCallBack, //This is the callback function that portAudio will call everytime the audio is needed, we defined it in the library
        &preBuffer //All audio data that will be passed to the callback function, this struct gets filled when the draw_line() function is called
    );
    if(error_output != paNoError) return error_output; //Checking for errors during initialization of the audio stream

    error_output = Pa_StartStream( oscilloscopeLibrary::audio_stream ); //Starting audio playback
    return error_output; //Returns any error occured during Pa_StartStream, if there was no error the function will return paNoError
} //oscilloscopeLibrary::open_start

PaError oscilloscopeLibrary::stop_close(){ //Stops the playback of an already playing audio stream
	PaError error_output; //Stores any errors occurred during the execution of the function

    error_output = Pa_StopStream( oscilloscopeLibrary::audio_stream ); //Stopping the audio playback of the audio stream defined into the class
    if(error_output != paNoError) return error_output; //If any error occurred during the stopping of the playback return the error
    oscilloscopeLibrary::playing = false; //Set the "playing" boolean back to false state

    //If no audio stream was playing close the stream anyway (if no stream was created in the first place then it will just return an error)
    error_output = Pa_CloseStream( oscilloscopeLibrary::audio_stream ); //Closing the audio stream
    return error_output; //Returns any error occured during Pa_StartStream, if there was no error the function will return paNoError
} //oscilloscopeLibrary::stop_close

void oscilloscopeLibrary::updateBuffer(){
    float bufferCopyLCH[buffer_frames]; //Creating two temporary arrays to store the content of the buffer that's gonna be deleted
    float bufferCopyRCH[buffer_frames]; //These are respectively the copy of the left channel and the copy of the right one

	//Copying into the just-created arrays the contents of the buffer
    for(unsigned int i = 0; i < buffer_size_old;i++){
        bufferCopyLCH[i] = *(oscilloscopeLibrary::preBuffer->left_channel + i);
        bufferCopyRCH[i] = *(oscilloscopeLibrary::preBuffer->right_channel + i);
    }

	//Delete the buffer pointer arrays
	delete oscilloscopeLibrary::preBuffer.left_channel;
	delete oscilloscopeLibrary::preBuffer.right_channel;

	//Create two new buffer pointer arrays and make them as big as the buffer_frames variable specifies
	oscilloscopeLibrary::preBuffer.left_channel = new float[buffer_frames];
	oscilloscopeLibrary::preBuffer.right_channel = new float[buffer_frames];

	//Copy all the data into the new buffer
	for(unsigned int i = 0;i < buffer_frames;i++){
		*(oscilloscopeLibrary::preBuffer->left_channel + i)  = bufferCopyLCH[i];
		*(oscilloscopeLibrary::preBuffer->right_channel + i) = bufferCopyRCH[i];
	}

	//buffer_size_old is the prebuffer's size after the last resizing
	buffer_size_old = buffer_frames;
    buffer_current_position = buffer_size_old;

	return; //end the function
} //oscilloscopeLibrary::updateBuffer

void oscilloscopeLibrary::absolute(int input){ //This function is used only in the draw_line
	return input >= 0 ? input : input *= -1; //Take the input and make it negative if it's not already
} //oscilloscopeLibrary::absolute

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
        *output++ = *(data->left_channel + i);
        *output++ = *(data->right_channel + i);
    }

    return 0; //We need to return an int since this function is defined to be an integer in portAudio
} //oscilloscopeLibrary::paCallBack

//Draws a line on the screen
void oscilloscopeLibrary::draw_line(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2){
	const bool FALL = false; //Used to define the direction variable
	const bool RISE = true; //If the direction goes up then the definition is in RISE, otherwise in FALL

	unsigned long iterator = buffer_current_position; //Iterator for the cycle that writes to the prebuffer
	unsigned int iX = x1; //Iterator for the X coord
	unsigned int iY = y1; //Iterator for the Y coord

	unsigned int distanceToX2 = absolute(x2 - x1); //the distance from the current value to the end value
	unsigned int distanceToY2 = absolute(y2 - y1); //Right now it's set as the distance between the first variable and the second one to calculate how much should the buffer be incremented

	signed int distanceRatio; //the ratio between distanceToX2 and distanceToY2

 	//Using the pythagorian theorem to calculate how long will the buffer need to be in order to draw the line
	oscilloscopeLibrary::buffer_frames += sqrt((double)pow((double)distanceToX2, 2) + pow((double)distanceToY2, 2)) + 1;
	oscilloscopeLibrary::updateBuffer(); //Update the buffer to the calculated size

	bool directionX = (x2 - x1) >= 0; //The direction in which the channels have to go at (RISE = true, FALL = false)
	bool directionY = (y2 - y1) >= 0;

	while(iX < x2 && iY < y2){
        //Recalculate both distances every loop
		distanceToX2 = absolute(iX - x1);
		distanceToY2 = absolute(iY - y1);

        //Recalculate the distanceRatio every loop
        //This is the ratio between the distance from the biggest variable to the end and the smallest variable to the end
		distanceRatio = distanceToX2 > distanceToY2 ? (int)(distanceToX2 / distanceToY2) : (int)(distanceToY2 / distanceToX2);

		if(distanceToX2 > distanceToY2){
			*(data->right_channel + iterator) = *(data->right_channel + iterator - 1) + ((directionX==RISE ? 1 : -1) * (1 / 100));
			*(data->left_channel + iterator)  = *(data->left_channel + iterator - 1) + ((directionY==RISE ? 1 : -1) * (distanceRatio / 100));
		} else if(distanceToX2 < distanceToY2){
			*(data->left_channel + iterator)  = *(data->left_channel + iterator - 1) + ((directionX==RISE ? 1 : -1) * (1 / 100));
			*(data->right_channel + iterator) = *(data->right_channel + iterator - 1) + ((directionY==RISE ? 1 : -1) * (distanceRatio / 100));
		} else if(distanceToX2 == distanceToY2){
			*(data->left_channel + iterator)  = *(data->left_channel + iterator - 1) + ((directionX==RISE ? 1 : -1) * (1 / 100));
			*(data->right_channel + iterator) = *(data->right_channel + iterator - 1) + ((directionX==RISE ? 1 : -1) * (1 / 100));
		}

		iterator++;
	}

    buffer_current_position = iterator;

    return;
} //oscilloscopeLibrary::draw_line
