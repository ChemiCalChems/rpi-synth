#include "kernel.h"
#include "midi.hpp"
#include "midiinput.hpp"
#include "sequencer.hpp"
#include <circle/usb/usbkeyboard.h>
#include <cstring>
#include "waveform.hpp"
#include "clock.hpp"

static const char FromKernel[] = "kernel";

CKernel::CKernel (void)
:	m_Screen (m_Options.GetWidth (), m_Options.GetHeight ()),
	m_Timer (&m_Interrupt),
	m_Logger (m_Options.GetLogLevel (), &m_Timer),
	m_USBHCI (&m_Interrupt, &m_Timer) {
}

CKernel::~CKernel (void)
{
}

boolean CKernel::Initialize (void)
{
	boolean bOK = TRUE;

	if (bOK)
	{
		bOK = m_Screen.Initialize ();
	}

	if (bOK)
	{
		bOK = m_Logger.Initialize (&m_Screen);
	}

	if (bOK)
	{
		bOK = m_Interrupt.Initialize ();
	}

	if (bOK)
	{
		bOK = m_Timer.Initialize ();
	}

	if (bOK)
	{
		bOK = m_USBHCI.Initialize ();
	}

	return bOK;
}

void keypressed (const char* c) {
	if (strcmp(c, " ") == 0) {
		if (Sequencer::get().loop.status == Sequencer::Loop::Status::stopped)
			Sequencer::get().loop.status = Sequencer::Loop::Status::recording;
		else
			Sequencer::get().loop.status = Sequencer::Loop::Status::playing;
	}
}

TShutdownMode CKernel::Run (void)
{
	m_Logger.Write (FromKernel, LogNotice, "Compile time: " __DATE__ " " __TIME__);
	
	CUSBKeyboardDevice *pKeyboard = (CUSBKeyboardDevice *) m_DeviceNameService.GetDevice ("ukbd1", FALSE);

	pKeyboard->RegisterKeyPressedHandler (keypressed);
	Mixer::get().streams.push_back(std::make_unique<Synthesizer>(std::make_unique<WaveformBase<1>>()));
	
	MidiInput input;
	Clock::get();
	Sequencer::get();
	CLogger::Get()->Write(FromKernel, LogNotice, "Startup done");
	Mixer::get().init();
	while (true) {
		if (Mixer::get().debugReady) {
			std::string msg = std::to_string(Mixer::get().sampleCountBeforeCallback);
			if (Mixer::get().sampleCountBeforeCallback != 256) m_Logger.Write(FromKernel, LogDebug, msg.c_str());
			Mixer::get().debugReady = false;
		}
		input.read();
		MidiManager::get().run();
		Mixer::get().fillBuffer();
	}

	return ShutdownHalt;
}

