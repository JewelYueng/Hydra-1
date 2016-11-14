// Hydra.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "pisqpipe.h"

namespace Hydra
{
	// globals
	const char *infotext = "name=\"Hydra\", "
		"author=\jason\", "
		"version=\"1.0\", "
		"country=\"China\"";


	// pip functions via pisqpipe.h
	void brain_init()
	{
		// init
		pipeOut("OK");
	}

	void brain_restart()
	{
		// delete old one
		// create new one
		pipeOut("OK");
	}

	void brain_turn()
	{

	}

	void brain_my(int x, int y)
	{

	}

	void brain_opponents(int x, int y)
	{

	}

	void brain_block(int x, int y)
	{

	}

	int	 brain_takeback(int x, int y)
	{
		return 0;
	}

	void brain_end()
	{

	}
}