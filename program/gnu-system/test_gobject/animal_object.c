/* -*- coding:utf-8 -*- */
#include "animal_object.h"

G_DEFINE_TYPE(AnimalObject, animal_object, G_TYPE_OBJECT)
//=== 初始化 ===
static void
animal_object_init(AnimalObject *obj)
{
	obj->m_head = 0;
	obj->m_body = 1;
	g_print("hi, I am a animal\n");
}

static void
animal_object_class_init(AnimalObjectClass *obj)
{
}

gint
animal_object_move(AnimalObject *obj, gint n1, gint n2)
{
	g_print("animal_object_move\n");
	return 0;
}
