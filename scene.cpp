#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "scene.h"

Scene::Scene() {
}

Scene::~Scene() {
    ObjMap::iterator iter, tmp;
    for (iter = obj_set_.begin(); iter != obj_set_.end(); ) {
        tmp = iter;
        ++iter;
        delete tmp->second;
    }
}

void Scene::Update(Object *object) {
    ObjMap::iterator iter;

    // send ENTER msg
    for (iter = enter_set_.begin(); iter != enter_set_.end(); ++iter) {
        printf("send [%d, %d:%d]\t ENTER msg to obj [%d, %d:%d]\n",
                object->id, object->x, object->y,
                iter->first, iter->second->x, iter->second->y);
        printf("send [%d, %d:%d]\t ENTER msg to obj [%d, %d:%d]\n",
                iter->first, iter->second->x, iter->second->y,
                object->id, object->x, object->y);
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

void Scene::Add(int id, int x, int y, int distance) {
    if (obj_set_.find(id) != obj_set_.end()) {
        return;
    }
    Object *object = new Object(id, x, y, distance);
    obj_set_[object->id] = object;

    ObjList::iterator iter, pos;
    bool flag;
    ObjMap x_set;

    // iterator x-axis object list
    flag = false;
    for (iter = obj_x_list_.begin(); iter != obj_x_list_.end(); ++iter) {
        int diff = (*iter)->x - object->x;
        // into the x set
        if (abs(diff) <= distance) {
            x_set[(*iter)->id] = *iter;
        } 
        if (!flag && diff > 0) {
            pos = iter;
            flag = true;
        }
        if (diff > distance) {
            break;
        }
    }
    if (flag) {
        obj_x_list_.insert(pos, object);
        object->x_pos = --pos;
    } else {
        obj_x_list_.push_front(object);
        object->x_pos = obj_x_list_.begin();
    }

    // iterator y-axis object list
    flag = false;
    for (iter = obj_y_list_.begin(); iter != obj_y_list_.end(); ++iter) {
        int diff = (*iter)->y - object->y;
        // into the enter set
        if (abs(diff) <= distance && x_set.find((*iter)->id) != x_set.end()) {
            enter_set_[(*iter)->id] = *iter;
        } 
        if (!flag && diff > 0) {
            pos = iter;
            flag = true;
        }
        if (diff > distance) {
            break;
        }
    }
    if (flag) {
        obj_y_list_.insert(pos, object);
        object->y_pos = --pos;
    } else {
        obj_y_list_.push_front(object);
        object->y_pos = obj_y_list_.begin();
    }
    Update(object);
}

void Scene::UpdateObjectPosition(Object *object, int x, int y) {
    int old_x = object->x;
    int old_y = object->y;
    object->x = x;
    object->y = y;

    ObjList::iterator iter, pos;

    // find the new x pos
    if (x > old_x) {
        if (object->x_pos != obj_x_list_.end()) {
            iter = object->x_pos;
            ++iter;
            obj_x_list_.erase(object->x_pos);
            while (iter != obj_x_list_.end()) {
                if (object->x - (*iter)->x < 0) {
                    pos = iter;
                    break;
                }
                ++iter;
            }
            if (iter != obj_x_list_.end()) {
                obj_x_list_.insert(pos, object);
                object->x_pos = --pos;
            } else {
                obj_x_list_.push_back(object);
                object->x_pos = --obj_x_list_.end();
            }
        }
    } else if (x < old_x) {
        if (object->x_pos != obj_x_list_.begin()) {
            iter = object->x_pos;
            --iter;
            obj_x_list_.erase(object->x_pos);
            while (iter != obj_x_list_.begin()) {
                if (object->x - (*iter)->x > 0) {
                    pos = ++iter;
                    break;
                }
                --iter;
            }
            if (iter != obj_x_list_.begin()) {
                obj_x_list_.insert(pos, object);
                object->x_pos = --pos;
            } else {
                obj_x_list_.push_front(object);
                object->x_pos = obj_x_list_.begin();
            }
        }
    }

    // find the new y pos
    if (y > old_y) {
        if (object->y_pos != obj_y_list_.end()) {
            iter = object->y_pos;
            ++iter;
            obj_y_list_.erase(object->y_pos);
            while (iter != obj_y_list_.end()) {
                if (object->y - (*iter)->y < 0) {
                    pos = iter;
                    break;
                }
                ++iter;
            }
            if (iter != obj_y_list_.end()) {
                obj_y_list_.insert(pos, object);
                object->y_pos = --pos;
            } else {
                obj_y_list_.push_back(object);
                object->y_pos = --obj_y_list_.end();
            }
        }
    } else if (y < old_y) {
        if (object->y_pos != obj_y_list_.begin()) {
            iter = object->y_pos;
            --iter;
            obj_y_list_.erase(object->y_pos);
            while (iter != obj_y_list_.begin()) {
                if (object->y - (*iter)->y > 0) {
                    pos = ++iter;
                    break;
                }
                --iter;
            }
            if (iter != obj_y_list_.begin()) {
                obj_y_list_.insert(pos, object);
                object->y_pos = --pos;
            } else {
                obj_y_list_.push_front(object);
                object->y_pos = obj_y_list_.begin();
            }
        }
    }
}

void Scene::Move(int id, int x, int y) {
    ObjMap::iterator obj_iter = obj_set_.find(id);
    if (obj_iter == obj_set_.end()) {
        return;
    }
    Object *object = obj_iter->second;

    ObjList::iterator list_iter;
    ObjMap old_set, new_set;

    // get the old set
    GetRangeSet(object, &old_set);

    // update object position
    UpdateObjectPosition(object, x, y);

    // get the new set
    GetRangeSet(object, &new_set);

    // move_set = old_set MIX new_set
    ObjMap::iterator iter;
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

void Scene::GetRangeSet(Object *object, ObjMap *set) {
    ObjMap x_set;
    ObjList::iterator iter;
    int distance = object->radius;

    // iterator x-axis object list
    if (object->x_pos != obj_x_list_.end()) {
        iter = object->x_pos;
        while (1) {
            ++iter;
            if (iter == obj_x_list_.end()) {
                break;
            }
            if (object->x - (*iter)->x > distance) {
                break;
            }
            x_set[(*iter)->id] = *iter;
        }
    }
    if (object->x_pos != obj_x_list_.begin()) {
        iter = object->x_pos;
        while (1) {
            --iter;
            if ((*iter)->x - object->x > distance) {
                break;
            }
            x_set[(*iter)->id] = *iter;
            if (iter == obj_x_list_.begin()) {
                break;
            }
        }
    }

    // iterator y-axis object list
    if (object->y_pos != obj_y_list_.end()) {
        iter = object->y_pos;
        while (1) {
            ++iter;
            if (iter == obj_y_list_.end()) {
                break;
            }
            if (object->y - (*iter)->y > distance) {
                break;
            }
            if (x_set.find((*iter)->id) != x_set.end()) {
                (*set)[(*iter)->id] = *iter;
            }
        }
    }
    if (object->y_pos != obj_y_list_.begin()) {
        iter = object->y_pos;
        while (1) {
            --iter;
            if ((*iter)->y - object->y > distance) {
                break;
            }
            if (x_set.find((*iter)->id) != x_set.end()) {
                (*set)[(*iter)->id] = *iter;
            }
            if (iter == obj_y_list_.begin()) {
                break;
            }
        }
    }
}

void Scene::Leave(int id) {
    ObjMap::iterator obj_iter = obj_set_.find(id);
    if (obj_iter == obj_set_.end()) {
        return;
    }
    Object *object = obj_iter->second;

    // get the leave set
    GetRangeSet(object, &leave_set_);
    Update(object);

    obj_x_list_.erase(object->x_pos);
    obj_y_list_.erase(object->y_pos);
    obj_set_.erase(object->id);
    delete object;
}
