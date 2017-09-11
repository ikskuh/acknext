#ifndef MODELLOADER_HPP
#define MODELLOADER_HPP

#include <acknext.h>
#include <QObject>

#include <assimp/scene.h>

class ModelLoader : public QObject
{
	Q_OBJECT
public:
	explicit ModelLoader(QObject *parent = nullptr);

	MODEL * load(QString const & fileName);

signals:

public slots:

private:
	MATERIAL * convertMaterial(aiMaterial const * mtl, aiScene const * scene, std::string const & referenceFileName);
};

#endif // MODELLOADER_HPP
