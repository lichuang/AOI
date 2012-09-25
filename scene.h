#ifndef __SCENE_H__
#define __SCENE_H__
#include <map>
#include <list>

using namespace std;

class Scene {
public:
    Scene();
    ~Scene();

    void Add(int id, int x, int y);
    void Move(int id, int x, int y);
    void Leave(int id);

private:
    struct Object {
        int x;
        int y;
        int id;

        Object(int _id, int _x, int _y) 
            : id(_id)
            , x(_x)
            , y(_y) {
        }
    };

private:
    void Update(Object *object);

    list<Object*> obj_x_list_;
    list<Object*> obj_y_list_;
    map<int, Object*> obj_set_;

    map<int, Object*> move_set_;
    map<int, Object*> enter_set_;
    map<int, Object*> leave_set_;
};

#endif  // __SCENE_H__
