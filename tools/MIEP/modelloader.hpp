#ifndef MODELLOADER_HPP
#define MODELLOADER_HPP

#include <acknext.h>
#include <QObject>

#include <assimp/scene.h>

#include <map>
#include <string>

class ModelLoader : public QObject
{
	Q_OBJECT
public:
	explicit ModelLoader(QObject *parent = nullptr);

	MODEL * load(QString const & fileName);

	QVector<ANIMATION*> loadAnimations(MODEL * reference, QString const & fileName);

signals:

public slots:

private:
	ANIMATION * convertAnimation(aiAnimation const * anim, std::map<std::string, int> const & boneMap);

	MATERIAL * convertMaterial(aiMaterial const * mtl, aiScene const * scene, std::string const & referenceFileName);
};

#endif // MODELLOADER_HPP
