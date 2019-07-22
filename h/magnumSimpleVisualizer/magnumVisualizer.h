#ifndef __magnumVisualizer_h_
#define __magnumVisualizer_h_

#include <iostream>
#include <map>
#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/Image.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Color.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Primitives/Cylinder.h>
#include <Magnum/Primitives/Plane.h>
#include <Magnum/Primitives/UVSphere.h>
#include <Magnum/Primitives/Axis.h>
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Shaders/Generic.h>
#include <Magnum/Shaders/VertexColor.h>
#include <Magnum/Shaders/visibility.h>
#include <Magnum/DimensionTraits.h>

namespace Magnum {

using namespace Magnum::Math::Literals;

typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;

class PhongIdShader: public GL::AbstractShaderProgram {
    public:
        /* Not actually used, just for documentation purposes --- but attribute
           locations in the vertex shader source have to match these in order
           to make MeshTools::compile() work */
        typedef Shaders::Generic3D::Position Position;
        typedef Shaders::Generic3D::Normal Normal;

        enum: UnsignedInt {
            ColorOutput = 0,
            ObjectIdOutput = 1
        };

        explicit PhongIdShader();

        PhongIdShader& setObjectId(UnsignedInt id) {
            setUniform(_objectIdUniform, id);
            return *this;
        }

        PhongIdShader& setLightPosition(const Vector3& position) {
            setUniform(_lightPositionUniform, position);
            return *this;
        }

        PhongIdShader& setAmbientColor(const Color3& color) {
            setUniform(_ambientColorUniform, color);
            return *this;
        }

        PhongIdShader& setColor(const Color3& color) {
            setUniform(_colorUniform, color);
            return *this;
        }

        PhongIdShader& setTransformationMatrix(const Matrix4& matrix) {
            setUniform(_transformationMatrixUniform, matrix);
            return *this;
        }

        PhongIdShader& setNormalMatrix(const Matrix3x3& matrix) {
            setUniform(_normalMatrixUniform, matrix);
            return *this;
        }

        PhongIdShader& setProjectionMatrix(const Matrix4& matrix) {
            setUniform(_projectionMatrixUniform, matrix);
            return *this;
        }

    private:
        Int _objectIdUniform,
            _lightPositionUniform,
            _ambientColorUniform,
            _colorUniform,
            _transformationMatrixUniform,
            _normalMatrixUniform,
            _projectionMatrixUniform;
};

PhongIdShader::PhongIdShader() {
    Utility::Resource rs("picking-data");

    GL::Shader vert{GL::Version::GL430, GL::Shader::Type::Vertex},
        frag{GL::Version::GL430, GL::Shader::Type::Fragment};
    vert.addSource(rs.get("PhongId.vert"));
    frag.addSource(rs.get("PhongId.frag"));
    CORRADE_INTERNAL_ASSERT(GL::Shader::compile({vert, frag}));
    attachShaders({vert, frag});
    CORRADE_INTERNAL_ASSERT(link());

    _objectIdUniform = uniformLocation("objectId");
    _lightPositionUniform = uniformLocation("light");
    _ambientColorUniform = uniformLocation("ambientColor");
    _colorUniform = uniformLocation("color");
    _transformationMatrixUniform = uniformLocation("transformationMatrix");
    _projectionMatrixUniform = uniformLocation("projectionMatrix");
    _normalMatrixUniform = uniformLocation("normalMatrix");
}

class VertexColorId: public GL::AbstractShaderProgram //Shaders::VertexColor<3>//,
{
    // using namespace Magnum::Shaders;
    typedef Shaders::Generic3D::Normal Normal;

    /**
     * @brief Vertex position
     *
     * @ref shaders-generic "Generic attribute",
     * @ref Magnum::Vector2 "Vector2" in 2D @ref Magnum::Vector3 "Vector3"
     * in 3D.
     */
    typedef typename Shaders::Generic<3>::Position Position;

    /**
     * @brief Three-component vertex color.
     *
     * @ref shaders-generic "Generic attribute", @ref Magnum::Color3. Use
     * either this or the @ref Color4 attribute.
     */
    typedef typename Shaders::Generic<3>::Color3 Color3;

    /**
     * @brief Four-component vertex color.
     *
     * @ref shaders-generic "Generic attribute", @ref Magnum::Color4. Use
     * either this or the @ref Color3 attribute.
     */
    typedef typename Shaders::Generic<3>::Color4 Color4;


public:
    enum: UnsignedInt {
        ColorOutput = 0,
        ObjectIdOutput = 1
    };
  explicit VertexColorId();
  VertexColorId& setObjectId(UnsignedInt id) {
      setUniform(_objectIdUniform, id);
      return *this;
  }
  VertexColorId& setTransformationMatrix(const Matrix4& matrix) {
      setUniform(_transformationProjectionMatrixUniform, matrix);
      // setUniform(_transformationMatrixUniform, matrix);
      return *this;
  }
  VertexColorId& setBrightness(const float v) {
      setUniform(_brightness, v);
      // setUniform(_transformationMatrixUniform, matrix);
      return *this;
  }
private:
    Int
    _objectIdUniform,
    _transformationProjectionMatrixUniform{0},
    _brightness;
};

VertexColorId::VertexColorId(){
    Utility::Resource rs("picking-data");

    GL::Shader vert{GL::Version::GL430, GL::Shader::Type::Vertex},
        frag{GL::Version::GL430, GL::Shader::Type::Fragment};
    vert.addSource(rs.get("generic.glsl"));
    vert.addSource(rs.get("VertexColorId.vert"));
    frag.addSource(rs.get("VertexColorId.frag"));
    CORRADE_INTERNAL_ASSERT(GL::Shader::compile({vert, frag}));
    attachShaders({vert, frag});

    {
        bindAttributeLocation(Position::Location, "position");
        bindAttributeLocation(Color3::Location, "color"); /* Color4 is the same */
    }

    CORRADE_INTERNAL_ASSERT(link());

    _objectIdUniform = uniformLocation("objectId");
    _brightness = uniformLocation("brightness");
};

enum pickableShaders {phongShader, colorshader};

class PickableObject: public Object3D, SceneGraph::Drawable3D {
    public:
        explicit PickableObject(unsigned int id, PhongIdShader* shader, const Color3& color, GL::Mesh& mesh, Object3D& parent, SceneGraph::DrawableGroup3D& drawables): Object3D{&parent}, SceneGraph::Drawable3D{*this, &drawables}, _id{id}, _selected{false}, _phongShader(shader), _color{color}, _mesh(mesh), _shaderType(phongShader),  _vertexShader(nullptr) {}
        explicit PickableObject(unsigned int id, VertexColorId* shader, const Color3& color, GL::Mesh& mesh, Object3D& parent, SceneGraph::DrawableGroup3D& drawables): Object3D{&parent}, SceneGraph::Drawable3D{*this, &drawables}, _id{id}, _selected{false}, _vertexShader(shader), _color{color}, _mesh(mesh), _shaderType(colorshader), _phongShader(nullptr) {}

        void setSelected(bool selected) { _selected = selected; }
        unsigned int getId(){ return _id;}

    private:
        virtual void draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) {
            switch (_shaderType) {
                case phongShader:
                _phongShader->setTransformationMatrix(transformationMatrix)
                    .setNormalMatrix(transformationMatrix.rotationScaling())
                    .setProjectionMatrix(camera.projectionMatrix())
                    .setAmbientColor(_selected ? _color*0.3f : Color3{})
                    .setColor(_color*(_selected ? 2.0f : 1.0f))
                    /* relative to the camera */
                    .setLightPosition({13.0f, 2.0f, 5.0f})
                    .setObjectId(_id);
                _mesh.draw(*_phongShader);
                break;
                case colorshader:
                _vertexShader->setObjectId(_id)
                .setTransformationMatrix(camera.projectionMatrix()*transformationMatrix)
                .setBrightness(_selected ? 1.0f : 0.5f)
                ;
                _mesh.draw(*_vertexShader);
                break;
            }
        }

        unsigned int _id;
        bool _selected;
        PhongIdShader* _phongShader;
        VertexColorId* _vertexShader;
        pickableShaders _shaderType;
        Color3 _color;
        GL::Mesh& _mesh;
};

class magnumVisualizer: public Platform::Application {
    public:
        explicit magnumVisualizer(const Arguments& arguments);

        int add3dAxisVisualization(float* pos, float* rot){
            _objects.push_back(new PickableObject{_objects.size()+1, &_vertexShader, 0xa5c9ea_rgbf, _cube, _scene, _drawables});
            _objectReferencedPos.insert(std::make_pair(_objects.back(), pos));
            _objectReferencedRot.insert(std::make_pair(_objects.back(), rot));
            return _objects.size();
        };
        int add3dAxisGUI(float posx = 0.0, float posy = 0.0, float posz = 0.0){
            _objects.push_back(new PickableObject{_objects.size()+1, &_vertexShader, 0xa5c9ea_rgbf, _cube, _scene, _drawables});
            _objects.back()->translate(Vector3(posx, posy, posz));
            return _objects.size();
        };

        int addCylinder(float* pos, float* rot, const float s = 1.0f, const Color3 color = 0x3bd267_rgbf) {
            _objects.push_back(new PickableObject{_objects.size()+1, &_phongShader, color, _cylinder, _scene, _drawables});
            _objects.back()->scale(Vector3(s));
            _objectReferencedPos.insert(std::make_pair(_objects.back(), pos));
            _objectReferencedRot.insert(std::make_pair(_objects.back(), rot));
            return _objects.size();
        }
        bool getPos(int id, float pos[3]);
        bool getRot(int id, float rot[9]);
    private:
        void drawEvent() override;
        void mousePressEvent(MouseEvent& event) override;
        void mouseMoveEvent(MouseMoveEvent& event) override;
        void mouseReleaseEvent(MouseEvent& event) override;
        void keyPressEvent(KeyEvent& event) override;
        void tickEvent() override {
            stateUpdate();
            // updateCameraLocation();
            updateObjectStateFromReference();
        };
        virtual void stateUpdate() {};
        void updateCameraLocation();
        void updateObjectStateFromReference();

        Scene3D _scene;
        Object3D* _cameraObject;
        SceneGraph::Camera3D* _camera;
        SceneGraph::DrawableGroup3D _drawables;

        PhongIdShader _phongShader;
        VertexColorId _vertexShader;
        GL::Mesh _cube, _plane, _sphere, _cylinder;

        // PickableObject* _objects[ObjectCount];
        std::vector<PickableObject*> _objects;
        std::map<PickableObject*, float*> _objectReferencedPos;
        std::map<PickableObject*, float*> _objectReferencedRot;

        GL::Framebuffer _framebuffer;
        GL::Renderbuffer _color, _objectId, _depth;
        float _cameraPosX, _cameraPosY, _cameraPosZ;
        bool m_stepOneFrame;
        bool m_pause;
        int _selectedPrimative;

        Vector2i _previousMousePosition, _mousePressPosition;
};
bool magnumVisualizer::getPos(int id, float pos[3]){
    if(id >= 0 && id<_objects.size()){
        Magnum::Math::Matrix4 ct = _objects[id]->transformationMatrix();
        for(int j=0; j<3; j++)  pos[j] = ct.translation()[j];
        return true;
    }
    else return false;
};

bool magnumVisualizer::getRot(int id, float rot[9]){
    if(id >= 0 && id<_objects.size()){
        Magnum::Math::Matrix4 ct = _objects[id]->transformationMatrix();
        int k = 0;
        for(int j=0; j<3; j++)  rot[k++] = ct.right()[j];
        for(int j=0; j<3; j++)  rot[k++] = ct.up()[j];
        for(int j=0; j<3; j++)  rot[k++] = ct.backward()[j];
        return true;
    }
    else return false;
};

void magnumVisualizer::updateCameraLocation(){
    Magnum::Math::Matrix4  ct = _cameraObject->transformationMatrix();
    ct.translation() = _cameraObject->transformationMatrix().rotation()*Vector3(_cameraPosX, _cameraPosY, _cameraPosZ);
    _cameraObject->setTransformation(ct);
    redraw();
}

void magnumVisualizer::updateObjectStateFromReference(){
    for(auto &p: _objectReferencedPos)
    {
        Magnum::Math::Matrix4  ct = p.first->transformationMatrix();
        ct.translation() = Vector3(p.second[0],p.second[1],p.second[2]);
        p.first->setTransformation(ct);
    }

    for(auto &p: _objectReferencedRot)
    {
        Magnum::Math::Matrix4  ct = p.first->transformationMatrix();
        ct.right() = Vector3(p.second[0],p.second[1],p.second[2]);
        ct.up() = Vector3(p.second[3],p.second[4],p.second[5]);
        ct.backward() = Vector3(p.second[6],p.second[7],p.second[8]);
        p.first->setTransformation(ct);
    }
    redraw();
}

magnumVisualizer::magnumVisualizer(const Arguments& arguments):
    _cameraPosX(0.0f), _cameraPosY(0.0f), _cameraPosZ(8.0f),
    Platform::Application{arguments, Configuration{}.setTitle("Magnum object picking example")}, _framebuffer{GL::defaultFramebuffer.viewport()} {
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL430);

    /* Global renderer configuration */
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);

    /* Configure framebuffer (using R8UI for object ID which means 255 objects max) */
    _color.setStorage(GL::RenderbufferFormat::RGBA8, GL::defaultFramebuffer.viewport().size());
    _objectId.setStorage(GL::RenderbufferFormat::R8UI, GL::defaultFramebuffer.viewport().size());
    _depth.setStorage(GL::RenderbufferFormat::DepthComponent24, GL::defaultFramebuffer.viewport().size());
    _framebuffer.attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, _color)
               .attachRenderbuffer(GL::Framebuffer::ColorAttachment{1}, _objectId)
               .attachRenderbuffer(GL::Framebuffer::BufferAttachment::Depth, _depth)
               .mapForDraw({{PhongIdShader::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
                            {PhongIdShader::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}});
    CORRADE_INTERNAL_ASSERT(_framebuffer.checkStatus(GL::FramebufferTarget::Draw) == GL::Framebuffer::Status::Complete);

    /* Set up meshes */
    //_cube = MeshTools::compile(Primitives::cubeSolid());
    _cube = MeshTools::compile(Primitives::axis3D());
    _sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32));
    _plane = MeshTools::compile(Primitives::planeSolid());
    _cylinder = MeshTools::compile(Primitives::cylinderSolid(3, 20, 0.4,  Magnum::Primitives::CylinderFlags{Magnum::Primitives::CylinderFlag::CapEnds}));

    /* Set up objects */
    // _objects.push_back(new PickableObject{1, &_phongShader, 0x3bd267_rgbf, _cylinder, _scene, _drawables});
    //     _objects.back()->rotate(34.0_degf, Vector3(1.0f).normalized())
    //     .translate({1.0f, 0.3f, -1.2f});
    // _objects.push_back(new PickableObject{2, &_phongShader, 0x2f83cc_rgbf, _sphere, _scene, _drawables});
    //     _objects.back()->translate({-1.2f, -0.3f, -0.2f});
    // _objects.push_back(new PickableObject{3, &_phongShader, 0xdcdcdc_rgbf, _plane, _scene, _drawables});
    //     _objects.back()->rotate(278.0_degf, Vector3(1.0f).normalized())
    //     .scale(Vector3(0.45f))
    //     .translate({-1.0f, 1.2f, 1.5f});
    // _objects.push_back(new PickableObject{4, &_phongShader, 0xc7cf2f_rgbf, _sphere, _scene, _drawables});
    //     _objects.back()->translate({-0.2f, -1.7f, -2.7f});
    // _objects.push_back(new PickableObject{5, &_phongShader, 0xcd3431_rgbf, _sphere, _scene, _drawables});
    //     _objects.back()->translate({0.7f, 0.6f, 2.2f})
    //     .scale(Vector3(0.75f));
    // _objects.push_back(new PickableObject{6, &_vertexShader, 0xa5c9ea_rgbf, _cube, _scene, _drawables});
    //     _objects.back()->rotate(-92.0_degf, Vector3(1.0f).normalized())
    //     .scale(Vector3(0.25f))
    //     .translate({-0.5f, -0.3f, 1.8f});

    /* Configure camera */
    _cameraObject = new Object3D{&_scene};
    _cameraObject->translate(Vector3::zAxis(7.0f));
    _camera = new SceneGraph::Camera3D{*_cameraObject};
    _camera->setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
        .setProjectionMatrix(Matrix4::perspectiveProjection(35.0_degf, 4.0f/3.0f, 0.001f, 100.0f))
        .setViewport(GL::defaultFramebuffer.viewport().size());
}

void magnumVisualizer::drawEvent() {
    /* Draw to custom framebuffer */
    _framebuffer
        .clearColor(0, Color3{0.125f})
        .clearColor(1, Vector4ui{})
        .clearDepth(1.0f)
        .bind();
    _camera->draw(_drawables);

    /* Bind the main buffer back */
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth)
        .bind();

    /* Blit color to window framebuffer */
    _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{0});
    GL::AbstractFramebuffer::blit(_framebuffer, GL::defaultFramebuffer,
        {{}, _framebuffer.viewport().size()}, GL::FramebufferBlit::Color);

    swapBuffers();
}

// void magnumVisualizer::mouseScrollEvent(mouseScrollEvent& event) {
//     if(event.button() == MouseEvent::Button::WheelUp){
//         _cameraPosZ += 0.1f;
//         updateCameraLocation();
//     }
//     if(event.button() == MouseEvent::Button::WheelDown){
//         _cameraPosZ -= 0.1f;
//         updateCameraLocation();
//     }
// }

void magnumVisualizer::mousePressEvent(MouseEvent& event) {
    if(event.button() != MouseEvent::Button::Left) return;

    _previousMousePosition = _mousePressPosition = event.position();
    event.setAccepted();
}

void magnumVisualizer::mouseMoveEvent(MouseMoveEvent& event) {
    if(!(event.buttons() & MouseMoveEvent::Button::Left)) return;

    const Vector2 delta = 3.0f*
        Vector2{event.position() - _previousMousePosition}/
        Vector2{GL::defaultFramebuffer.viewport().size()};

    (*_cameraObject)
        .rotate(Rad{-delta.y()}, _cameraObject->transformation().right().normalized())
        .rotateY(Rad{-delta.x()});

    _previousMousePosition = event.position();
    event.setAccepted();
    redraw();
}

void magnumVisualizer::mouseReleaseEvent(MouseEvent& event) {
    if(event.button() != MouseEvent::Button::Left || _mousePressPosition != event.position()) return;

    /* Read object ID at given click position (framebuffer has Y up while windowing system Y down) */
    _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{1});
    Image2D data = _framebuffer.read(
        Range2Di::fromSize({event.position().x(), _framebuffer.viewport().sizeY() - event.position().y() - 1}, {1, 1}),
        {PixelFormat::R8UI});

    // only select if ID is valid
    if(data.data<UnsignedByte>()[0] > 0){
        /* Highlight object under mouse and deselect all other */
        for(auto* o: _objects) o->setSelected(false);
        UnsignedByte id = data.data<UnsignedByte>()[0];
        _selectedPrimative = int(id) - 1;
        if(id > 0 && id < _objects.size()+1)
        _objects[id - 1]->setSelected(true);
    }

    event.setAccepted();
    redraw();
}

void magnumVisualizer::keyPressEvent(KeyEvent& event) {
    switch (event.key()) {
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Unknown:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::LeftShift:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::RightShift:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::LeftCtrl:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::RightCtrl:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::LeftAlt:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::RightAlt:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::LeftSuper:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::RightSuper:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::AltGr:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Enter:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Esc:
            exit();
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Up:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Down:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Left:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Right:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Home:
            // _primRotXYZ[_selectable2primIdx[_selectedPrimative]][0] += 0.1f;
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::End:
            // _primRotXYZ[_selectable2primIdx[_selectedPrimative]][0] -= 0.1f;
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::PageUp:
            // _primRotXYZ[_selectable2primIdx[_selectedPrimative]][2] -= 0.1f;
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::PageDown:
            // _primRotXYZ[_selectable2primIdx[_selectedPrimative]][1] -= 0.1f;
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Backspace:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Insert:
            // _primRotXYZ[_selectable2primIdx[_selectedPrimative]][2] += 0.1f;
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Delete:
            // _primRotXYZ[_selectable2primIdx[_selectedPrimative]][1] += 0.1f;
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::F1:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::F2:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::F3:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::F4:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::F5:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::F6:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::F7:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::F8:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::F9:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::F10:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::F11:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::F12:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Space:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Tab:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Comma:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Period:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Minus:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Plus:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Slash:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Percent:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Semicolon:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Equal:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Zero:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::One:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Two:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Three:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Four:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Five:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Six:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Seven:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Eight:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Nine:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::A:
            _cameraPosX += 0.1f;
            updateCameraLocation();
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::B:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::C:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::D:
            _cameraPosX -= 0.1f;
            updateCameraLocation();
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::E:
            _cameraPosY -= 0.1f;
            updateCameraLocation();
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::F:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::G:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::H:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::I:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::J:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::K:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::L:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::M:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::N:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::O:
            m_stepOneFrame = true;
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::P:
            m_pause = !m_pause;
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Q:
            _cameraPosY += 0.1f;
            updateCameraLocation();
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::R:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::S:
            _cameraPosZ -= 0.1f;
            updateCameraLocation();
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::T:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::U:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::V:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::W:
            _cameraPosZ += 0.1f;
            updateCameraLocation();
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::X:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Y:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::Z:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::NumZero:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::NumOne:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::NumTwo:
            // _primPos[_selectable2primIdx[_selectedPrimative]][1] -= 0.1f;
            if(_selectedPrimative>=0){
                _objects[_selectedPrimative]->translate({0.0, -0.1, 0.0});
                redraw();
            }
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::NumThree:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::NumFour:
            if(_selectedPrimative>=0){
                _objects[_selectedPrimative]->translate({-0.1, 0.0, 0.0});
                redraw();
            }
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::NumFive:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::NumSix:
            if(_selectedPrimative>=0){
                _objects[_selectedPrimative]->translate({0.1, 0.0, 0.0});
                redraw();
            }
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::NumSeven:
            if(_selectedPrimative>=0){
                _objects[_selectedPrimative]->translate({0.0, 0.0, -0.1});
                redraw();
            }
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::NumEight:
            if(_selectedPrimative>=0){
                _objects[_selectedPrimative]->translate({0.0, 0.1, 0.0});
                redraw();
                }
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::NumNine:
            if(_selectedPrimative>=0){
                _objects[_selectedPrimative]->translate({0.0, 0.0, 0.1});
                redraw();
            }
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::NumDecimal:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::NumDivide:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::NumMultiply:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::NumSubtract:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::NumAdd:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::NumEnter:
            break;
        case Magnum::Platform::Sdl2Application::KeyEvent::Key::NumEqual:
            break;
        default:
            break;
      }
  }

}

#endif
