/* -*- coding:utf-8 -*- */
#ifndef ANIMAL_OBJECT_H
#define ANIMAL_OBJECT_H
#include <glib-object.h>
G_BEGIN_DECLS
//=== 定义类型 ===
/* 类型标识 */
GType animal_object_get_type();
#define ANIMAL_TYPE_OBJECT (animal_object_get_type())
/* 类型转换 */
#define ANIMAL_OBJECT(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ANIMAL_TYPE_OBJECT, AnimalObject))

//=== 类定义 ===
typedef struct _AnimalObject {
    GObject parent;
	/* 成员变量 */
	gint    m_head;
	gint    m_body;
} AnimalObject;

typedef struct _AnimalObjectClass {
	GObjectClass parent_class;
} AnimalObjectClass;

/* 成员函数 */
gint
animal_object_move(AnimalObject*, gint, gint);

G_END_DECLS
#endif // ANIMAL_OBJECT_H