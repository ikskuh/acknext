#ifndef BLOB_HPP
#define BLOB_HPP

#include <engine.hpp>

class Blob : public EngineObject<BLOB>
{
public:
	explicit Blob(size_t size);
	NOCOPY(Blob);
	~Blob();

	void resize(size_t size);
};

#endif // BLOB_HPP
