#ifndef RENDER_H
#define RENDER_H

#include <QImage>
#include "camera.h"
#include "scene.h"

class Render
{
public:
    Render();
    QImage renderImage(const Camera& cam, const Scene& scene);
};

#endif // RENDER_H
