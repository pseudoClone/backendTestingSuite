#include <iostream>
#include <portaudio.h>
#include <stdlib.h>
#include <cstring> /* For memset*/

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 512 /* Captures the amplitude of the waveform at the rate 44100 per second */
/*Take 512 of the samples and take it to the callback function */

static inline float max(float a, float b){
	return a > b? a: b;
}

static void checkErr(PaError err)
{
	if (err != paNoError){
		printf("PortAudio Error: %s\n", Pa_GetErrorText(err));
		exit(EXIT_FAILURE);
	}
}

static int patestCallback(
		const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
		void* userData
		)
{
	float* in = (float*)inputBuffer;
	(void) outputBuffer;

	int dispSize = 100;
	printf("\r");

	fflush(stdout);

	float vol_l = 0;
	float vol_r = 0;

	for (unsigned long i = 0; i < framesPerBuffer * 2; i += 2) {
		vol_l = max(vol_l, std::abs(in[i]));
		vol_r = max(vol_r, std::abs(in[i + 1]));
	}
	for (int i = 0; i < dispSize; i++) {
		float barProportion = i / (float)dispSize;
		if(barProportion <= vol_l && barProportion <= vol_r){
			printf("█");
		} else if(barProportion <= vol_l){
			printf("▀");
		}else if(barProportion <= vol_r){
			printf("▄");
		}else{
			printf(" ");
		}

	}
	return EXIT_SUCCESS;

}

int main (int argc, char *argv[])
{	
	PaError err;
	err = Pa_Initialize(); /* Inintializes port audio instance */
	checkErr(err);
	
	int numDevices = Pa_GetDeviceCount();
	printf("Number of devices: %d\n", numDevices);
	if (numDevices < 0){
		printf("Error getting number of devices\n");
		exit(EXIT_FAILURE);
	} else if (numDevices == 0){
		printf("There are no audio devices on the machine\n");
		exit(EXIT_SUCCESS);
	}
	const PaDeviceInfo* deviceinfo;
	for (int i = 0; i < numDevices; i++) {
		deviceinfo = Pa_GetDeviceInfo(i);
		printf("Device: %d\n",i);
		printf("Name: %s \n", deviceinfo -> name);
		printf("Max Input Channels: %d\n", deviceinfo -> maxInputChannels);
		printf("Max Output Channels: %d\n",deviceinfo -> maxOutputChannels);
		printf("Default Sampling Rate: %f\n",deviceinfo -> defaultSampleRate);
		printf("\n");
	}
	int device = 15; 
	

	PaStreamParameters inputParameters;
	PaStreamParameters outputParameters;

	memset(&inputParameters, 0, sizeof(inputParameters));
	inputParameters.channelCount = 2; /* Left and right*/
	inputParameters.device = device; /* Using device 15 for input*/
	inputParameters.hostApiSpecificStreamInfo = NULL;
	inputParameters.sampleFormat = paFloat32;
	inputParameters.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowInputLatency; /* Working in real time now*/

	/* Note that you can use seperate device for output/input is it does not have more than one or has 0 channels */
	memset(&outputParameters, 0, sizeof(outputParameters));
	outputParameters.channelCount = 2; /* Dont change this to 0 even if we dont use output channels */ 
	outputParameters.device = device; /* Using device 15 for input*/
	outputParameters.hostApiSpecificStreamInfo = NULL;
	outputParameters.sampleFormat = paFloat32;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowInputLatency; /* Working in real time now*/


	PaStream* stream;
	err = Pa_OpenStream(
			&stream,
			&inputParameters,
			&outputParameters,
			SAMPLE_RATE,
			FRAMES_PER_BUFFER,
			paNoFlag,
			patestCallback,
			NULL
			);
	checkErr(err);

	err = Pa_StartStream(stream);
	checkErr(err);

	Pa_Sleep(10 * 1000); /* Wait after starting stream and record for 10 seconds */

	err = Pa_StopStream(stream);
	checkErr(err);

	err = Pa_CloseStream(stream);
	checkErr(err);

	err = Pa_Terminate();
	checkErr(err);

	return EXIT_SUCCESS;
}
