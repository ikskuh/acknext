#ifndef MODELSAVER_HPP
#define MODELSAVER_HPP

#include <acknext.h>

class ModelSaver
{
public:
	ModelSaver() = delete;

	static void write(ACKFILE * file, MODEL const * model);
};

#endif // MODELSAVER_HPP
