#include "kernel.h"
#include "midi.hpp"
#include "midiinput.hpp"
#include "sequencer.hpp"
#include <circle/usb/usbkeyboard.h>
#include <cstring>

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

	if (bOK)
	{
		//bOK = m_MiniOrgan.Initialize ();
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
	
	Mixer::get().streams.push_back(std::make_unique<Synthesizer>());
	Mixer::get().init();
	
	MidiInput input;
	while (true) {
		input.read();
		Sequencer::get().cycle();
		MidiManager::get().run();
	}

	return ShutdownHalt;
}

