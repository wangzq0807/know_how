/* -*- coding:utf-8 -*- */
#include "bird_object.h"

//=== 定义类型 ===
/* 精简过的类型定义 */
// 声明: bird_object_init(), bird_object_class_init()
// 定义: bird_object_get_type(), bird_object_get_instance_private()
G_DEFINE_TYPE(BirdObject, bird_object, ANIMAL_TYPE_OBJECT)

static void 
bird_object_init(BirdObject* obj)
{
    obj->aaa = 0;
    g_print("hi, I am a bird\n");
}

static void
bird_object_class_init(BirdObjectClass* obje)
{ }

gint
bird_object_fly(BirdObject *obj, gint n, char* sz)
{
    g_print("I can fly\n");
    return 0;
}
