#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "scene.h"

#define DISTANCE 5

Scene::Scene() {
}

Scene::~Scene() {
    map<int, Object*>::iterator iter, tmp;
    for (iter = obj_set_.begin(); iter != obj_set_.end(); ) {
        tmp = iter;
        ++iter;
        delete tmp->second;
    }
}

void Scene::Update(Object *object) {
    map<int, Object*>::iterator iter;

    // send ENTER msg
    for (iter = enter_set_.begin(); iter != enter_set_.end(); ++iter) {
        printf("send [%d, %d:%d]\t ENTER msg to obj [%d, %d:%d]\n",
                object->id, object->x, object->y,
                iter->first, iter->second->x, iter->second->y);
    }
    // send MOVE msg
    for (iter = move_set_.begin(); iter != move_set_.end(); ++iter) {
        printf("send [%d, %d:%d]\t MOVE msg to obj [%d, %d:%d]\n",
                object->id, object->x, object->y,
                iter->first, iter->second->x, iter->second->y);
    }
    // send LEAVE msg
    for (iter = leave_set_.begin(); iter != leave_set_.end(); ++iter) {
        printf("send [%d, %d:%d]\t LEAVE msg to obj [%d, %d:%d]\n",
                object->id, object->x, object->y,
                iter->first, iter->second->x, iter->second->y);
    }
    // clean the move, leave, enter set
    enter_set_.clear();
    move_set_.clear();
    leave_set_.clear();
}

void Scene::Add(int id, int x, int y) {
    if (obj_set_.find(id) != obj_set_.end()) {
        return;
    }
    Object *object = new Object(id, x, y);
    obj_set_[object->id] = object;

    list<Object*>::iterator iter, pos;
    bool flag;

    // iterator x-axis object list
    flag = false;
    for (iter = obj_x_list_.begin(); iter != obj_x_list_.end(); ++iter) {
        int diff = (*iter)->x - object->x;
        // into the enter set
        if (abs(diff) <= DISTANCE) {
            enter_set_[(*iter)->id] = *iter;
        } 
        if (!flag && diff > 0) {
            pos = iter;
            flag = true;
        }
        if (diff > DISTANCE) {
            break;
        }
    }
    if (flag) {
        obj_x_list_.insert(pos, object);
    } else {
        obj_x_list_.push_front(object);
    }

    // iterator y-axis object list
    flag = false;
    for (iter = obj_y_list_.begin(); iter != obj_y_list_.end(); ++iter) {
        int diff = (*iter)->y - object->y;
        // into the enter set
        if (abs(diff) <= DISTANCE) {
            enter_set_[(*iter)->id] = *iter;
        } 
        if (!flag && diff > 0) {
            pos = iter;
            flag = true;
        }
        if (diff > DISTANCE) {
            break;
        }
    }
    if (flag) {
        obj_y_list_.insert(pos, object);
    } else {
        obj_y_list_.push_front(object);
    }
    Update(object);
}

void Scene::Move(int id, int x, int y) {
    map<int, Object*>::iterator obj_iter = obj_set_.find(id);
    if (obj_iter == obj_set_.end()) {
        return;
    }
    Object *object = obj_iter->second;

    list<Object*>::iterator list_iter;
    map<int, Object*> old_set, new_set;

    // get the old set
    // iterator x-axis object list
    for (list_iter = obj_x_list_.begin(); list_iter != obj_x_list_.end(); ++list_iter) {
        if (object->id == (*list_iter)->id) {
            continue;
        }
        int diff = (*list_iter)->x - object->x;
        if (abs(diff) <= DISTANCE) {
            old_set[(*list_iter)->id] = *list_iter;
        }
        if (diff > DISTANCE) {
            break;
        }
    }
    // iterator y-axis object list
    for (list_iter = obj_y_list_.begin(); list_iter != obj_y_list_.end(); ++list_iter) {
        if (object->id == (*list_iter)->id) {
            continue;
        }
        int diff = (*list_iter)->y - object->y;
        if (abs(diff) <= DISTANCE) {
            old_set[(*list_iter)->id] = *list_iter;
        }
        if (diff > DISTANCE) {
            break;
        }
    }

    // update object position
    object->x = x;
    object->y = y;

    // get the new set
    // iterator x-axis object list
    for (list_iter = obj_x_list_.begin(); list_iter != obj_x_list_.end(); ++list_iter) {
        if (object->id == (*list_iter)->id) {
            continue;
        }
        int diff = (*list_iter)->x - object->x;
        if (abs(diff) <= DISTANCE) {
            new_set[(*list_iter)->id] = *list_iter;
        }
        if (diff > DISTANCE) {
            break;
        }
    }
    // iterator y-axis object list
    for (list_iter = obj_y_list_.begin(); list_iter != obj_y_list_.end(); ++list_iter) {
        if (object->id == (*list_iter)->id) {
            continue;
        }
        int diff = (*list_iter)->y - object->y;
        if (abs(diff) <= DISTANCE) {
            new_set[(*list_iter)->id] = *list_iter;
        }
        if (diff > DISTANCE) {
            break;
        }
    }

    // move_set = old_set MIX new_set
    map<int, Object*>::iterator iter;
    for (iter = old_set.begin(); iter != old_set.end(); ++iter) {
        if (new_set.find(iter->first) != new_set.end()) {
            move_set_[iter->first] = iter->second;
        }
    }

    // leave_set = old_set SUB move_set
    for (iter = old_set.begin(); iter != old_set.end(); ++iter) {
        if (move_set_.find(iter->first) == move_set_.end()) {
            leave_set_[iter->first] = iter->second;
        }
    }

    // enter_set = new_set SUB move_set
    for (iter = new_set.begin(); iter != new_set.end(); ++iter) {
        if (move_set_.find(iter->first) == move_set_.end()) {
            enter_set_[iter->first] = iter->second;
        }
    }

    Update(object);
}

void Scene::Leave(int id) {
    map<int, Object*>::iterator obj_iter = obj_set_.find(id);
    if (obj_iter == obj_set_.end()) {
        return;
    }
    Object *object = obj_iter->second;
    list<Object*>::iterator iter, pos;

    // iterator x-axis object list
    for (iter = obj_x_list_.begin(), pos = obj_x_list_.end(); iter != obj_x_list_.end(); ++iter) {
        if (object->id == (*iter)->id) {
            pos = iter;
            continue;
        }
        int diff = (*iter)->x - object->x;
        if (abs(diff) <= DISTANCE) {
            leave_set_[(*iter)->id] = *iter;
        }
        if (diff > DISTANCE) {
            break;
        }
    }
    if (pos != obj_x_list_.end()) {
        obj_x_list_.erase(pos);
    }

    // iterator y-axis object list
    for (iter = obj_y_list_.begin(), pos = obj_y_list_.end(); iter != obj_y_list_.end(); ++iter) {
        if (object->id == (*iter)->id) {
            pos = iter;
            continue;
        }
        int diff = (*iter)->y - object->y;
        if (abs(diff) <= DISTANCE) {
            leave_set_[(*iter)->id] = *iter;
        }
        if (diff > DISTANCE) {
            break;
        }
    }
    if (pos != obj_y_list_.end()) {
        obj_y_list_.erase(pos);
    }

    Update(object);

    obj_set_.erase(object->id);
    delete object;
}
