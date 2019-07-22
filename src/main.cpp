#include <project/magnumVisualizer.h>

class myApp : public Magnum::magnumVisualizer
{
public:
    explicit myApp(const Arguments& arguments):
    Magnum::magnumVisualizer(arguments)
    {
        _pos.push_back({0.0f, 0.0f, 0.0f});
        _rot.push_back({
            0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f,
            1.0f, 0.0f, 0.0f});

        _pos.push_back({2.0f, 0.0f, -2.0f});
        _rot.push_back({
            0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f,
            1.0f, 0.0f, 0.0f});
        add3dAxisGUI();
        add3dAxisGUI(0.0, 0.1);
        add3dAxisVisualization((float*)&_pos[0], (float*)&_rot[0]);
        addCylinder((float*)&_pos[1], (float*)&_rot[1], 0.01f);
    };
    virtual void stateUpdate(){
        _pos[0][0] += 0.001f;
    };
private:
    std::vector<std::array<float, 3>> _pos;
    std::vector<std::array<float, 9>> _rot;
};

MAGNUM_APPLICATION_MAIN(myApp)
